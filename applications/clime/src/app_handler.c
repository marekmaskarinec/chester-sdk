#include "app_handler.h"
#include "app_data.h"
#include "app_init.h"
#include "app_loop.h"

/* CHESTER includes */
#include <ctr_lrw.h>
#include <ctr_lte.h>

/* Zephyr includes */
#include <zephyr/logging/log.h>
#include <zephyr/zephyr.h>

/* Standard includes */
#include <math.h>
#include <stdbool.h>
#include <stddef.h>

LOG_MODULE_REGISTER(app_handler, LOG_LEVEL_DBG);

#if defined(CONFIG_SHIELD_CTR_LRW)

void app_handler_lrw(enum ctr_lrw_event event, union ctr_lrw_event_data *data, void *param)
{
	int ret;

	switch (event) {
	case CTR_LRW_EVENT_FAILURE:
		LOG_INF("Event `CTR_LRW_EVENT_FAILURE`");
		ret = ctr_lrw_start(NULL);
		if (ret) {
			LOG_ERR("Call `ctr_lrw_start` failed: %d", ret);
		}
		break;
	case CTR_LRW_EVENT_START_OK:
		LOG_INF("Event `CTR_LRW_EVENT_START_OK`");
		ret = ctr_lrw_join(NULL);
		if (ret) {
			LOG_ERR("Call `ctr_lrw_join` failed: %d", ret);
		}
		break;
	case CTR_LRW_EVENT_START_ERR:
		LOG_INF("Event `CTR_LRW_EVENT_START_ERR`");
		break;
	case CTR_LRW_EVENT_JOIN_OK:
		LOG_INF("Event `CTR_LRW_EVENT_JOIN_OK`");
		break;
	case CTR_LRW_EVENT_JOIN_ERR:
		LOG_INF("Event `CTR_LRW_EVENT_JOIN_ERR`");
		break;
	case CTR_LRW_EVENT_SEND_OK:
		LOG_INF("Event `CTR_LRW_EVENT_SEND_OK`");
		break;
	case CTR_LRW_EVENT_SEND_ERR:
		LOG_INF("Event `CTR_LRW_EVENT_SEND_ERR`");
		break;
	default:
		LOG_WRN("Unknown event: %d", event);
	}
}

#endif /* defined(CONFIG_SHIELD_CTR_LRW) */

#if defined(CONFIG_SHIELD_CTR_LTE)

static void start(void)
{
	int ret;

	ret = ctr_lte_start(NULL);
	if (ret) {
		LOG_ERR("Call `ctr_lte_start` failed: %d", ret);
		k_oops();
	}
}

static void attach(void)
{
	int ret;

	ret = ctr_lte_attach(NULL);
	if (ret) {
		LOG_ERR("Call `ctr_lte_attach` failed: %d", ret);
		k_oops();
	}
}

void app_handler_lte(enum ctr_lte_event event, union ctr_lte_event_data *data, void *param)
{
	switch (event) {
	case CTR_LTE_EVENT_FAILURE:
		LOG_ERR("Event `CTR_LTE_EVENT_FAILURE`");
		start();
		break;

	case CTR_LTE_EVENT_START_OK:
		LOG_INF("Event `CTR_LTE_EVENT_START_OK`");
		attach();
		break;

	case CTR_LTE_EVENT_START_ERR:
		LOG_ERR("Event `CTR_LTE_EVENT_START_ERR`");
		start();
		break;

	case CTR_LTE_EVENT_ATTACH_OK:
		LOG_INF("Event `CTR_LTE_EVENT_ATTACH_OK`");
		k_sem_give(&g_app_init_sem);
		break;

	case CTR_LTE_EVENT_ATTACH_ERR:
		LOG_ERR("Event `CTR_LTE_EVENT_ATTACH_ERR`");
		start();
		break;

	case CTR_LTE_EVENT_DETACH_OK:
		LOG_INF("Event `CTR_LTE_EVENT_DETACH_OK`");
		break;

	case CTR_LTE_EVENT_DETACH_ERR:
		LOG_ERR("Event `CTR_LTE_EVENT_DETACH_ERR`");
		start();
		break;

	case CTR_LTE_EVENT_EVAL_OK:
		LOG_INF("Event `CTR_LTE_EVENT_EVAL_OK`");

		struct ctr_lte_eval *eval = &data->eval_ok.eval;

		LOG_DBG("EEST: %d", eval->eest);
		LOG_DBG("ECL: %d", eval->ecl);
		LOG_DBG("RSRP: %d", eval->rsrp);
		LOG_DBG("RSRQ: %d", eval->rsrq);
		LOG_DBG("SNR: %d", eval->snr);
		LOG_DBG("PLMN: %d", eval->plmn);
		LOG_DBG("CID: %d", eval->cid);
		LOG_DBG("BAND: %d", eval->band);
		LOG_DBG("EARFCN: %d", eval->earfcn);

		k_mutex_lock(&g_app_data_lte_eval_mut, K_FOREVER);
		memcpy(&g_app_data_lte_eval, &data->eval_ok.eval, sizeof(g_app_data_lte_eval));
		g_app_data_lte_eval_valid = true;
		k_mutex_unlock(&g_app_data_lte_eval_mut);

		break;

	case CTR_LTE_EVENT_EVAL_ERR:
		LOG_ERR("Event `CTR_LTE_EVENT_EVAL_ERR`");

		k_mutex_lock(&g_app_data_lte_eval_mut, K_FOREVER);
		g_app_data_lte_eval_valid = false;
		k_mutex_unlock(&g_app_data_lte_eval_mut);

		break;

	case CTR_LTE_EVENT_SEND_OK:
		LOG_INF("Event `CTR_LTE_EVENT_SEND_OK`");
		break;

	case CTR_LTE_EVENT_SEND_ERR:
		LOG_ERR("Event `CTR_LTE_EVENT_SEND_ERR`");
		start();
		break;

	default:
		LOG_WRN("Unknown event: %d", event);
		return;
	}
}

#endif /* defined(CONFIG_SHIELD_CTR_LTE) */

#if defined(CONFIG_SHIELD_CTR_S1)
void ctr_s1_event_handler(const struct device *dev, enum ctr_s1_event event, void *user_data)
{
	int ret;

	switch (event) {
	case CTR_S1_EVENT_DEVICE_RESET:
		LOG_INF("Event `CTR_S1_EVENT_DEVICE_RESET`");

		ret = ctr_s1_apply(dev);
		if (ret) {
			LOG_ERR("Call `ctr_s1_apply` failed: %d", ret);
			k_oops();
		}

		ret = ctr_s1_set_motion_sensitivity(dev, CTR_S1_PIR_SENSITIVITY_MEDIUM);
		if (ret) {
			LOG_ERR("Call `ctr_s1_set_motion_sensitivity` failed: %d", ret);
			k_oops();
		}

		break;

	case CTR_S1_EVENT_BUTTON_PRESSED:
		LOG_INF("Event `CTR_S1_EVENT_BUTTON_PRESSED`");

		atomic_inc(&g_app_data.iaq_press_count);

		struct ctr_s1_led_param param_led = {
			.brightness = CTR_S1_LED_BRIGHTNESS_HIGH,
			.command = CTR_S1_LED_COMMAND_1X_1_1,
			.pattern = CTR_S1_LED_PATTERN_NONE,
		};

		ret = ctr_s1_set_led(dev, CTR_S1_LED_CHANNEL_B, &param_led);
		if (ret) {
			LOG_ERR("Call `ctr_s1_set_led` failed: %d", ret);
			k_oops();
		}

		struct ctr_s1_buzzer_param param_buzzer = {
			.command = CTR_S1_BUZZER_COMMAND_1X_1_1,
			.pattern = CTR_S1_BUZZER_PATTERN_NONE,
		};

		ret = ctr_s1_set_buzzer(dev, &param_buzzer);
		if (ret) {
			LOG_ERR("Call `ctr_s1_set_buzzer` failed: %d", ret);
			k_oops();
		}

		ret = ctr_s1_apply(dev);
		if (ret) {
			LOG_ERR("Call `ctr_s1_apply` failed: %d", ret);
			k_oops();
		}

		atomic_set(&g_app_loop_send, true);
		k_sem_give(&g_app_loop_sem);

		break;

	case CTR_S1_EVENT_BUTTON_CLICKED:
		LOG_INF("Event `CTR_S1_EVENT_BUTTON_CLICKED`");
		break;

	case CTR_S1_EVENT_BUTTON_HOLD:
		LOG_INF("Event `CTR_S1_EVENT_BUTTON_HOLD`");
		break;

	case CTR_S1_EVENT_BUTTON_RELEASED:
		LOG_INF("Event `CTR_S1_EVENT_BUTTON_RELEASED`");
		break;

	case CTR_S1_EVENT_MOTION_DETECTED:
		LOG_INF("Event `CTR_S1_EVENT_MOTION_DETECTED`");

		atomic_inc(&g_app_data.iaq_motion_count);

		int motion_count;
		ret = ctr_s1_read_motion_count(dev, &motion_count);
		if (ret) {
			LOG_ERR("Call `ctr_s1_read_motion_count` failed: %d", ret);
			k_oops();
		}

		LOG_INF("Motion count: %d", motion_count);

		break;

	default:
		break;
	}
}
#endif /* defined(CONFIG_SHIELD_CTR_S1) */
