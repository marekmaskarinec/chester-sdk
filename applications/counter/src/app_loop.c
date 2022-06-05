#include "app_loop.h"
#include "app_data.h"
#include "app_measure.h"
#include "app_send.h"

/* CHESTER includes */
#include <ctr_led.h>
#include <ctr_wdog.h>
#include <drivers/ctr_batt.h>

/* Zephyr includes */
#include <device.h>
#include <devicetree.h>
#include <logging/log.h>
#include <zephyr.h>

/* Standard includes */
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

LOG_MODULE_REGISTER(app_loop, LOG_LEVEL_DBG);

#define BATT_TEST_INTERVAL_MSEC (12 * 60 * 60 * 1000)

K_SEM_DEFINE(g_app_loop_sem, 1, 1);
atomic_t g_app_loop_measure = true;
atomic_t g_app_loop_send = true;

extern struct ctr_wdog_channel g_app_wdog_channel;

static int update_battery(void)
{
	int ret;

	static int64_t next;

	if (k_uptime_get() < next) {
		return 0;
	}

	static const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(ctr_batt));

	if (!device_is_ready(dev)) {
		LOG_ERR("Device not ready");
		ret = -ENODEV;
		goto error;
	}

	int rest_mv;
	ret = ctr_batt_get_rest_voltage_mv(dev, &rest_mv, CTR_BATT_REST_TIMEOUT_DEFAULT_MS);
	if (ret) {
		LOG_ERR("Call `ctr_batt_get_rest_voltage_mv` failed: %d", ret);
		goto error;
	}

	int load_mv;
	ret = ctr_batt_get_load_voltage_mv(dev, &load_mv, CTR_BATT_LOAD_TIMEOUT_DEFAULT_MS);
	if (ret) {
		LOG_ERR("Call `ctr_batt_get_load_voltage_mv` failed: %d", ret);
		goto error;
	}

	int current_ma;
	ctr_batt_get_load_current_ma(dev, &current_ma, load_mv);

	LOG_INF("Battery voltage (rest): %u mV", rest_mv);
	LOG_INF("Battery voltage (load): %u mV", load_mv);
	LOG_INF("Battery current (load): %u mA", current_ma);

	g_app_data.batt_voltage_rest = rest_mv / 1000.f;
	g_app_data.batt_voltage_load = load_mv / 1000.f;
	g_app_data.batt_current_load = current_ma;

	next = k_uptime_get() + BATT_TEST_INTERVAL_MSEC;

	return 0;

error:
	g_app_data.batt_voltage_rest = NAN;
	g_app_data.batt_voltage_load = NAN;
	g_app_data.batt_current_load = NAN;

	return ret;
}

int app_loop(void)
{
	int ret;

	ret = ctr_wdog_feed(&g_app_wdog_channel);
	if (ret) {
		LOG_ERR("Call `ctr_wdog_feed` failed: %d", ret);
		return ret;
	}

	ctr_led_set(CTR_LED_CHANNEL_G, true);
	k_sleep(K_MSEC(30));
	ctr_led_set(CTR_LED_CHANNEL_G, false);

	ret = k_sem_take(&g_app_loop_sem, K_SECONDS(5));
	if (ret == -EAGAIN) {
		return 0;
	} else if (ret) {
		LOG_ERR("Call `k_sem_take` failed: %d", ret);
		return ret;
	}

	ret = update_battery();
	if (ret) {
		LOG_ERR("Call `task_battery` failed: %d", ret);
	}

	if (atomic_set(&g_app_loop_measure, false)) {
		ret = app_measure();
		if (ret) {
			LOG_ERR("Call `app_measure` failed: %d", ret);
		}
	}

	if (atomic_set(&g_app_loop_send, false)) {
		ret = app_send();
		if (ret) {
			LOG_ERR("Call `app_send` failed: %d", ret);
		}
	}

	return 0;
}