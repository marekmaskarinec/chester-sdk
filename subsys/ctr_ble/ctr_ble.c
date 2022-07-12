/* CHESTER includes */
#include <ctr_info.h>

/* Zephyr includes */
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci_vs.h>
#include <bluetooth/hci.h>
#include <bluetooth/services/dfu_smp.h>
#include <bluetooth/services/nus.h>
#include <bluetooth/uuid.h>
#include <img_mgmt/img_mgmt.h>
#include <init.h>
#include <logging/log.h>
#include <mgmt/mcumgr/smp_bt.h>
#include <os_mgmt/os_mgmt.h>
#include <shell_mgmt/shell_mgmt.h>
#include <shell/shell_bt_nus.h>
#include <sys/byteorder.h>
#include <zephyr.h>

/* Standard includes */
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

LOG_MODULE_REGISTER(ctr_ble, CONFIG_CTR_BLE_LOG_LEVEL);

#define TX_POWER_DBM_ADV 8
#define TX_POWER_DBM_CONN 8

/* clang-format off */
#define ADV_OPT                                                                                    \
	BT_LE_ADV_OPT_CONNECTABLE |                                                                \
	BT_LE_ADV_OPT_SCANNABLE |                                                                  \
	BT_LE_ADV_OPT_USE_NAME |                                                                   \
	BT_LE_ADV_OPT_FORCE_NAME_IN_AD
/* clang-format on */

static const struct bt_data m_ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
};

static const struct bt_data m_sd[] = {
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_DFU_SMP_SERVICE_VAL),
};

static struct bt_conn *m_current_conn;

static int set_tx_power(uint8_t handle_type, uint16_t handle, int8_t tx_power_level)
{
	int ret;

	struct bt_hci_cp_vs_write_tx_power_level *cp;
	struct net_buf *buf;
	buf = bt_hci_cmd_create(BT_HCI_OP_VS_WRITE_TX_POWER_LEVEL, sizeof(*cp));
	if (!buf) {
		LOG_ERR("Call `bt_hci_cmd_create` failed");
		return -EIO;
	}

	cp = net_buf_add(buf, sizeof(*cp));
	cp->handle_type = handle_type;
	cp->handle = sys_cpu_to_le16(handle);
	cp->tx_power_level = tx_power_level;

	struct net_buf *rsp;
	ret = bt_hci_cmd_send_sync(BT_HCI_OP_VS_WRITE_TX_POWER_LEVEL, buf, &rsp);
	if (ret) {
		LOG_ERR("Call `bt_hci_cmd_send_sync` failed: %d", ret);
		return ret;
	}

	struct bt_hci_rp_vs_write_tx_power_level *rp = (void *)rsp->data;
	LOG_INF("Handle type: %" PRIu8 "; Handle: %" PRIu16 "; Requested: %" PRId8
	        " dBm; Selected: %" PRId8 " dBm\n",
	        handle_type, handle, tx_power_level, rp->selected_tx_power);

	net_buf_unref(rsp);

	return 0;
}

#ifdef CONFIG_CTR_INFO
static int set_device_name(void)
{
	int ret;

	static char bt_dev_name[CONFIG_BT_DEVICE_NAME_MAX + 1];

	char *serial_number;
	ret = ctr_info_get_serial_number(&serial_number);
	if (ret) {
		LOG_WRN("Call `ctr_info_get_serial_number` failed: %d", ret);
		return ret;
	}

	ret = snprintf(bt_dev_name, sizeof(bt_dev_name), "%s %s", CONFIG_BT_DEVICE_NAME,
	               serial_number);
	if (ret < 0) {
		LOG_ERR("Call `snprintf` failed: %d", ret);
		return ret;
	}

	ret = bt_set_name(bt_dev_name);
	if (ret) {
		LOG_ERR("Call `bt_set_name` failed: %d", ret);
		return ret;
	}

	return 0;
}
#endif

static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		LOG_ERR("Connection failed (err %" PRIu8 ")", err);
		return;
	}

	LOG_INF("Connected");

	m_current_conn = bt_conn_ref(conn);

#ifdef CONFIG_SHELL_BT_NUS
	shell_bt_nus_enable(conn);
#endif /* CONFIG_SHELL_BT_NUS */
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	LOG_INF("Disconnected (reason: %" PRIu8 ")", reason);

#ifdef CONFIG_SHELL_BT_NUS
	shell_bt_nus_disable();
#endif /* CONFIG_SHELL_BT_NUS */

	if (m_current_conn) {
		bt_conn_unref(m_current_conn);
		m_current_conn = NULL;
	}
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

static int init(const struct device *dev)
{
	int ret;

	LOG_INF("System initialization");

#ifdef CONFIG_MCUMGR_CMD_OS_MGMT
	os_mgmt_register_group();
#endif

#ifdef CONFIG_MCUMGR_CMD_IMG_MGMT
	img_mgmt_register_group();
#endif

#ifdef CONFIG_MCUMGR_CMD_SHELL_MGMT
	shell_mgmt_register_group();
#endif

	ret = bt_enable(NULL);
	if (ret) {
		LOG_ERR("Call `bt_enable` failed: %d", ret);
		return ret;
	}

#ifdef CONFIG_MCUMGR_SMP_BT
	ret = smp_bt_register();
	if (ret) {
		LOG_ERR("Call `smp_bt_register` failed: %d", ret);
		return ret;
	}
#endif

#ifdef CONFIG_SHELL_BT_NUS
	ret = shell_bt_nus_init();
	if (ret) {
		LOG_ERR("Call `shell_bt_nus_init` failed: %d", ret);
		return ret;
	}
#endif /* CONFIG_SHELL_BT_NUS */

#ifdef CONFIG_CTR_INFO
	ret = set_device_name();
	if (ret) {
		LOG_WRN("Call `set_device_name` failed: %d", ret);
	}
#endif

	struct bt_le_adv_param *adv_param =
	        BT_LE_ADV_PARAM(ADV_OPT, BT_GAP_ADV_SLOW_INT_MIN, BT_GAP_ADV_SLOW_INT_MAX, NULL);

	ret = bt_le_adv_start(adv_param, m_ad, ARRAY_SIZE(m_ad), m_sd, ARRAY_SIZE(m_sd));
	if (ret) {
		LOG_ERR("Call `bt_le_adv_start` failed: %d", ret);
		return ret;
	}

	ret = set_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_ADV, 0, TX_POWER_DBM_ADV);
	if (ret) {
		LOG_ERR("Call `set_tx_power` (BT_HCI_VS_LL_HANDLE_TYPE_ADV) failed: %d", ret);
		return ret;
	}

	ret = set_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_CONN, 0, TX_POWER_DBM_CONN);
	if (ret) {
		LOG_ERR("Call `set_tx_power` (BT_HCI_VS_LL_HANDLE_TYPE_CONN) failed: %d", ret);
		return ret;
	}

	return 0;
}

SYS_INIT(init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
