/* CHESTER includes */
#include <chester/ctr_edge.h>
#include <chester/drivers/ctr_x0.h>

/* Zephyr includes */
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/zephyr.h>

/* Standard includes */
#include <stddef.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

void edge_callback(struct ctr_edge *edge, enum ctr_edge_event event, void *user_data)
{
	LOG_INF("%p: Event: %d", edge, event);
}

static int init_chester_x0(void)
{
	int ret;

	static const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(ctr_x0_a));

	if (!device_is_ready(dev)) {
		LOG_ERR("Device not ready");
		return -ENODEV;
	}

#define SETUP(ch)                                                                                  \
	do {                                                                                       \
		ret = ctr_x0_set_mode(dev, CTR_X0_CHANNEL_##ch, CTR_X0_MODE_NPN_INPUT);            \
		if (ret) {                                                                         \
			LOG_ERR("Call `ctr_x0_set_mode` failed: %d", ret);                         \
			return ret;                                                                \
		}                                                                                  \
		const struct gpio_dt_spec *spec;                                                   \
		ret = ctr_x0_get_spec(dev, CTR_X0_CHANNEL_##ch, &spec);                            \
		if (ret) {                                                                         \
			LOG_ERR("Call `ctr_x0_get_spec` failed: %d", ret);                         \
			return ret;                                                                \
		}                                                                                  \
		ret = gpio_pin_configure_dt(spec, GPIO_INPUT | GPIO_ACTIVE_LOW);                   \
		if (ret) {                                                                         \
			LOG_ERR("Call `gpio_configure_dt` failed: %d", ret);                       \
			return ret;                                                                \
		}                                                                                  \
		static struct ctr_edge edge;                                                       \
		ret = ctr_edge_init(&edge, spec, false);                                           \
		if (ret) {                                                                         \
			LOG_ERR("Call `ctr_edge_init` failed: %d", ret);                           \
			return ret;                                                                \
		}                                                                                  \
		ret = ctr_edge_set_callback(&edge, edge_callback, STRINGIFY(ch));                  \
		if (ret) {                                                                         \
			LOG_ERR("Call `ctr_edge_set_callback` failed: %d", ret);                   \
			return ret;                                                                \
		}                                                                                  \
		ret = ctr_edge_set_cooldown_time(&edge, 10);                                       \
		if (ret) {                                                                         \
			LOG_ERR("Call `ctr_edge_set_cooldown_time` failed: %d", ret);              \
			return ret;                                                                \
		}                                                                                  \
		ret = ctr_edge_set_active_duration(&edge, 50);                                     \
		if (ret) {                                                                         \
			LOG_ERR("Call `ctr_edge_set_active_duration` failed: %d", ret);            \
			return ret;                                                                \
		}                                                                                  \
		ret = ctr_edge_set_inactive_duration(&edge, 50);                                   \
		if (ret) {                                                                         \
			LOG_ERR("Call `ctr_edge_set_inactive_duration` failed: %d", ret);          \
			return ret;                                                                \
		}                                                                                  \
		ret = ctr_edge_watch(&edge);                                                       \
		if (ret) {                                                                         \
			LOG_ERR("Call `ctr_edge_watch` failed: %d", ret);                          \
			return ret;                                                                \
		}                                                                                  \
	} while (0)

	SETUP(1);
	SETUP(2);
	SETUP(3);
	SETUP(4);

	return 0;
}

void main(void)
{
	int ret;

	LOG_INF("Build time: " __DATE__ " " __TIME__);

	ret = init_chester_x0();
	if (ret) {
		LOG_ERR("Call `init_chester_x0` failed: %d", ret);
		k_oops();
	}

	for (;;) {
		LOG_INF("Alive");
		k_sleep(K_SECONDS(1));
	}
}
