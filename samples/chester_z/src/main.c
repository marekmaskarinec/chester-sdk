#include <device.h>
#include <devicetree.h>
#include <drivers/ctr_z.h>
#include <logging/log.h>
#include <zephyr.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

void event_cb(const struct device *dev, enum ctr_z_event event, void *user_data)
{
	LOG_DBG("event: %d, user_data: %p", event, user_data);

	if (event == CTR_Z_EVENT_BUTTON_1_CLICK) {
		static int led;

		struct ctr_z_led_param param = {
			.brightness = CTR_Z_LED_BRIGHTNESS_HIGH,
			.command = CTR_Z_LED_COMMAND_1X_1_2,
			.pattern = CTR_Z_LED_PATTERN_NONE,
		};

		ctr_z_set_led(dev, led++ % 15, &param);
		ctr_z_apply(dev);
	}
}

void main(void)
{
	int ret;

	LOG_INF("Hello CHESTER_Z!");

	const struct device *chester_z_dev = device_get_binding("CHESTER_Z");
	__ASSERT_NO_MSG(chester_z_dev);

	ret = ctr_z_set_callback(chester_z_dev, event_cb, NULL);
	if (ret < 0) {
		LOG_ERR("Call `chester_z_set_callback` failed: %d", ret);
	}

	uint32_t serial_number;

	ret = ctr_z_get_serial_number(chester_z_dev, &serial_number);
	if (ret < 0) {
		LOG_ERR("Call `chester_z_get_serial_number` failed: %d", ret);
	}

	LOG_INF("Serial number: %u", serial_number);

	uint16_t hw_revision;

	ret = ctr_z_get_hw_revision(chester_z_dev, &hw_revision);
	if (ret < 0) {
		LOG_ERR("Call `chester_z_get_hw_revision` failed: %d", ret);
	}

	LOG_INF("Hardware revision: %u", hw_revision);

	uint32_t hw_variant;

	ret = ctr_z_get_hw_variant(chester_z_dev, &hw_variant);
	if (ret < 0) {
		LOG_ERR("Call `chester_z_get_hw_variant` failed: %d", ret);
	}

	LOG_INF("Hardware variant: %u", hw_variant);

	uint32_t fw_version;

	ret = ctr_z_get_fw_version(chester_z_dev, &fw_version);
	if (ret < 0) {
		LOG_ERR("Call `chester_z_get_fw_version` failed: %d", ret);
	}

	LOG_INF("Firmware version: %u", fw_version);

	for (;;) {
		uint16_t vdc;

		ret = ctr_z_get_vdc_mv(chester_z_dev, &vdc);

		if (ret < 0) {
			LOG_ERR("Call `ctr_z_get_vdc_mv` failed: %d", ret);

		} else {
			LOG_INF("Voltage VDC: %u mV", vdc);
		}

		uint16_t vbat;

		ret = ctr_z_get_vbat_mv(chester_z_dev, &vbat);

		if (ret < 0) {
			LOG_ERR("Call `ctr_z_get_vbat_mv` failed: %d", ret);

		} else {
			LOG_INF("Voltage VBAT: %u mV", vbat);
		}

		k_sleep(K_MSEC(1000));
	}
}
