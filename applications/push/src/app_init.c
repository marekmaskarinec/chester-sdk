#include "app_init.h"
#include "app_handler.h"

/* CHESTER includes */
#include <ctr_led.h>
#include <ctr_lrw.h>
#include <ctr_therm.h>
#include <ctr_wdog.h>
#include <drivers/ctr_z.h>

/* Zephyr includes */
#include <device.h>
#include <devicetree.h>
#include <logging/log.h>
#include <zephyr.h>

/* Standard includes */
#include <stddef.h>
#include <stdint.h>

LOG_MODULE_REGISTER(app_init, LOG_LEVEL_DBG);

struct ctr_wdog_channel g_app_wdog_channel;

static int init_chester_z(void)
{
	int ret;

	static const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(ctr_z));

	if (!device_is_ready(dev)) {
		LOG_ERR("Device not ready");
		return -ENODEV;
	}

	ret = ctr_z_set_handler(dev, app_handler_ctr_z, NULL);
	if (ret) {
		LOG_ERR("Call `app_handler_ctr_z` failed: %d", ret);
		return ret;
	}

	uint32_t serial_number;
	ret = ctr_z_get_serial_number(dev, &serial_number);
	if (ret) {
		LOG_ERR("Call `ctr_z_get_serial_number` failed: %d", ret);
		return ret;
	}

	LOG_INF("Serial number: %08x", serial_number);

	uint16_t hw_revision;
	ret = ctr_z_get_hw_revision(dev, &hw_revision);
	if (ret) {
		LOG_ERR("Call `ctr_z_get_hw_revision` failed: %d", ret);
		return ret;
	}

	LOG_INF("HW revision: %04x", hw_revision);

	uint32_t hw_variant;
	ret = ctr_z_get_hw_variant(dev, &hw_variant);
	if (ret) {
		LOG_ERR("Call `ctr_z_get_hw_variant` failed: %d", ret);
		return ret;
	}

	LOG_INF("HW variant: %08x", hw_variant);

	uint32_t fw_version;
	ret = ctr_z_get_fw_version(dev, &fw_version);
	if (ret) {
		LOG_ERR("Call `ctr_z_get_fw_version` failed: %d", ret);
		return ret;
	}

	LOG_INF("FW version: %08x", fw_version);

	char vendor_name[64];
	ret = ctr_z_get_vendor_name(dev, vendor_name, sizeof(vendor_name));
	if (ret) {
		LOG_ERR("Call `ctr_z_get_vendor_name` failed: %d", ret);
		return ret;
	}

	LOG_INF("Vendor name: %s", vendor_name);

	char product_name[64];
	ret = ctr_z_get_product_name(dev, product_name, sizeof(product_name));
	if (ret) {
		LOG_ERR("Call `ctr_z_get_product_name` failed: %d", ret);
		return ret;
	}

	LOG_INF("Product name: %s", product_name);

	return 0;
}

static int init_sensors(void)
{
	int ret;

	ret = ctr_therm_init();
	if (ret) {
		LOG_ERR("Call `ctr_therm_init` failed: %d", ret);
		return ret;
	}

	return 0;
}

int app_init(void)
{
	int ret;

	ctr_led_set(CTR_LED_CHANNEL_R, true);

	ret = ctr_wdog_set_timeout(120000);
	if (ret) {
		LOG_ERR("Call `ctr_wdog_set_timeout` failed: %d", ret);
		return ret;
	}

	ret = ctr_wdog_install(&g_app_wdog_channel);
	if (ret) {
		LOG_ERR("Call `ctr_wdog_install` failed: %d", ret);
		return ret;
	}

	ret = init_chester_z();
	if (ret) {
		LOG_ERR("Call `init_chester_z` failed: %d", ret);
		k_oops();
	}

	ret = init_sensors();
	if (ret) {
		LOG_ERR("Call `init_sensors` failed: %d", ret);
		k_oops();
	}

	ret = ctr_lrw_init(app_handler_lrw, NULL);
	if (ret) {
		LOG_ERR("Call `ctr_lrw_init` failed: %d", ret);
		k_oops();
	}

	ret = ctr_lrw_start(NULL);
	if (ret) {
		LOG_ERR("Call `ctr_lrw_start` failed: %d", ret);
		k_oops();
	}

	k_sleep(K_SECONDS(2));

	ctr_led_set(CTR_LED_CHANNEL_R, false);

	return 0;
}