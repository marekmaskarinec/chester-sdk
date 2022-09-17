#include "app_cbor.h"
#include "app_data.h"
#include "msg_key.h"

/* CHESTER includes */
#include <chester/ctr_info.h>
#include <chester/ctr_lte.h>
#include <chester/ctr_rtc.h>

/* Zephyr includes */
#include <zephyr/logging/log.h>
#include <zephyr/zephyr.h>
#include <zcbor_common.h>
#include <zcbor_encode.h>

/* Standard includes */
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

LOG_MODULE_REGISTER(app_cbor, LOG_LEVEL_DBG);

int app_cbor_encode(zcbor_state_t *zs)
{
	int ret;

	zs->constant_state->stop_on_error = true;

	zcbor_map_start_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);

	zcbor_uint32_put(zs, MSG_KEY_FRAME);
	{
		zcbor_map_start_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);

		uint8_t protocol = 1;
		zcbor_uint32_put(zs, MSG_KEY_PROTOCOL);
		zcbor_uint32_put(zs, protocol);

		static uint32_t sequence;
		zcbor_uint32_put(zs, MSG_KEY_SEQUENCE);
		zcbor_uint32_put(zs, sequence++);

		uint64_t timestamp;
		ret = ctr_rtc_get_ts(&timestamp);
		if (ret) {
			LOG_ERR("Call `ctr_rtc_get_ts` failed: %d", ret);
			return ret;
		}

		zcbor_uint32_put(zs, MSG_KEY_TIMESTAMP);
		zcbor_uint64_put(zs, timestamp);

		zcbor_map_end_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);
	}

	zcbor_uint32_put(zs, MSG_KEY_ATTRIBUTE);
	{
		zcbor_map_start_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);

		char *vendor_name;
		ctr_info_get_vendor_name(&vendor_name);

		zcbor_uint32_put(zs, MSG_KEY_VENDOR_NAME);
		zcbor_tstr_put_term(zs, vendor_name);

		char *product_name;
		ctr_info_get_product_name(&product_name);

		zcbor_uint32_put(zs, MSG_KEY_PRODUCT_NAME);
		zcbor_tstr_put_term(zs, product_name);

		char *hw_variant;
		ctr_info_get_hw_variant(&hw_variant);

		zcbor_uint32_put(zs, MSG_KEY_HW_VARIANT);
		zcbor_tstr_put_term(zs, hw_variant);

		char *hw_revision;
		ctr_info_get_hw_revision(&hw_revision);

		zcbor_uint32_put(zs, MSG_KEY_HW_REVISION);
		zcbor_tstr_put_term(zs, hw_revision);

		char *fw_version;
		ctr_info_get_fw_version(&fw_version);

		zcbor_uint32_put(zs, MSG_KEY_FW_VERSION);
		zcbor_tstr_put_term(zs, fw_version);

		char *serial_number;
		ctr_info_get_serial_number(&serial_number);

		zcbor_uint32_put(zs, MSG_KEY_SERIAL_NUMBER);
		zcbor_tstr_put_term(zs, serial_number);

		zcbor_map_end_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);
	}

	zcbor_uint32_put(zs, MSG_KEY_STATE);
	{
		zcbor_map_start_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);

		zcbor_uint32_put(zs, MSG_KEY_UPTIME);
		zcbor_uint64_put(zs, k_uptime_get() / 1000);

		zcbor_map_end_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);
	}

	zcbor_uint32_put(zs, MSG_KEY_BATTERY);
	{
		zcbor_map_start_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);

		zcbor_uint32_put(zs, MSG_KEY_VOLTAGE_REST);
		if (g_app_data.errors.batt_voltage_rest) {
			zcbor_nil_put(zs, NULL);
		} else {
			zcbor_uint32_put(zs, g_app_data.states.batt_voltage_rest * 1000.f);
		}

		zcbor_uint32_put(zs, MSG_KEY_VOLTAGE_LOAD);
		if (g_app_data.errors.batt_voltage_load) {
			zcbor_nil_put(zs, NULL);
		} else {
			zcbor_uint32_put(zs, g_app_data.states.batt_voltage_load * 1000.f);
		}

		zcbor_uint32_put(zs, MSG_KEY_CURRENT_LOAD);
		if (g_app_data.errors.batt_current_load) {
			zcbor_nil_put(zs, NULL);
		} else {
			zcbor_uint32_put(zs, g_app_data.states.batt_current_load);
		}

		zcbor_map_end_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);
	}

	zcbor_uint32_put(zs, MSG_KEY_BACKUP);
	{
		zcbor_map_start_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);

		zcbor_uint32_put(zs, MSG_KEY_VOLTAGE);
		if (g_app_data.errors.bckp_voltage) {
			zcbor_nil_put(zs, NULL);
		} else {
			zcbor_uint32_put(zs, g_app_data.states.bckp_voltage * 1000.f);
		}

		zcbor_map_end_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);
	}

	zcbor_uint32_put(zs, MSG_KEY_LINE);
	{
		zcbor_map_start_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);

		zcbor_uint32_put(zs, MSG_KEY_PRESENT);
		if (g_app_data.errors.line_present) {
			zcbor_nil_put(zs, NULL);
		} else {
			zcbor_bool_put(zs, g_app_data.states.line_present);
		}

		zcbor_uint32_put(zs, MSG_KEY_VOLTAGE);
		if (g_app_data.errors.line_voltage) {
			zcbor_nil_put(zs, NULL);
		} else {
			zcbor_uint32_put(zs, g_app_data.states.line_voltage * 1000.f);
		}

		zcbor_map_end_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);
	}

	zcbor_uint32_put(zs, MSG_KEY_NETWORK);
	{
		zcbor_map_start_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);

		uint64_t imei;
		ret = ctr_lte_get_imei(&imei);
		if (ret) {
			LOG_ERR("Call `ctr_lte_get_imei` failed: %d", ret);
			return ret;
		}

		zcbor_uint32_put(zs, MSG_KEY_IMEI);
		zcbor_uint64_put(zs, imei);

		uint64_t imsi;
		ret = ctr_lte_get_imsi(&imsi);
		if (ret) {
			LOG_ERR("Call `ctr_lte_get_imsi` failed: %d", ret);
			return ret;
		}

		zcbor_uint32_put(zs, MSG_KEY_IMSI);
		zcbor_uint64_put(zs, imsi);

		zcbor_uint32_put(zs, MSG_KEY_PARAMETER);
		{
			zcbor_map_start_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);

			k_mutex_lock(&g_app_data_lte_eval_mut, K_FOREVER);

			zcbor_uint32_put(zs, MSG_KEY_EEST);
			if (g_app_data_lte_eval_valid) {
				zcbor_int32_put(zs, g_app_data_lte_eval.eest);
			} else {
				zcbor_nil_put(zs, NULL);
			}

			zcbor_uint32_put(zs, MSG_KEY_ECL);
			if (g_app_data_lte_eval_valid) {
				zcbor_int32_put(zs, g_app_data_lte_eval.ecl);
			} else {
				zcbor_nil_put(zs, NULL);
			}

			zcbor_uint32_put(zs, MSG_KEY_RSRP);
			if (g_app_data_lte_eval_valid) {
				zcbor_int32_put(zs, g_app_data_lte_eval.rsrp);
			} else {
				zcbor_nil_put(zs, NULL);
			}

			zcbor_uint32_put(zs, MSG_KEY_RSRQ);
			if (g_app_data_lte_eval_valid) {
				zcbor_int32_put(zs, g_app_data_lte_eval.rsrq);
			} else {
				zcbor_nil_put(zs, NULL);
			}

			zcbor_uint32_put(zs, MSG_KEY_SNR);
			if (g_app_data_lte_eval_valid) {
				zcbor_int32_put(zs, g_app_data_lte_eval.snr);
			} else {
				zcbor_nil_put(zs, NULL);
			}

			zcbor_uint32_put(zs, MSG_KEY_PLMN);
			if (g_app_data_lte_eval_valid) {
				zcbor_int32_put(zs, g_app_data_lte_eval.plmn);
			} else {
				zcbor_nil_put(zs, NULL);
			}

			zcbor_uint32_put(zs, MSG_KEY_CID);
			if (g_app_data_lte_eval_valid) {
				zcbor_int32_put(zs, g_app_data_lte_eval.cid);
			} else {
				zcbor_nil_put(zs, NULL);
			}

			zcbor_uint32_put(zs, MSG_KEY_BAND);
			if (g_app_data_lte_eval_valid) {
				zcbor_int32_put(zs, g_app_data_lte_eval.band);
			} else {
				zcbor_nil_put(zs, NULL);
			}

			zcbor_uint32_put(zs, MSG_KEY_EARFCN);
			if (g_app_data_lte_eval_valid) {
				zcbor_int32_put(zs, g_app_data_lte_eval.earfcn);
			} else {
				zcbor_nil_put(zs, NULL);
			}

			g_app_data_lte_eval_valid = false;

			k_mutex_unlock(&g_app_data_lte_eval_mut);

			zcbor_map_end_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);
		}

		zcbor_map_end_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);
	}

	zcbor_uint32_put(zs, MSG_KEY_THERMOMETER);
	{
		zcbor_map_start_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);

		zcbor_uint32_put(zs, MSG_KEY_TEMPERATURE);
		if (g_app_data.errors.int_temperature) {
			zcbor_nil_put(zs, NULL);
		} else {
			zcbor_int32_put(zs, g_app_data.states.int_temperature * 100.f);
		}

		zcbor_map_end_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);
	}

	zcbor_uint32_put(zs, MSG_KEY_ACCELEROMETER);
	{
		zcbor_map_start_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);

		zcbor_uint32_put(zs, MSG_KEY_ACCELERATION_X);
		if (g_app_data.errors.acceleration_x) {
			zcbor_nil_put(zs, NULL);
		} else {
			zcbor_int32_put(zs, g_app_data.states.acceleration_x * 1000.f);
		}

		zcbor_uint32_put(zs, MSG_KEY_ACCELERATION_Y);
		if (g_app_data.errors.acceleration_y) {
			zcbor_nil_put(zs, NULL);
		} else {
			zcbor_int32_put(zs, g_app_data.states.acceleration_y * 1000.f);
		}

		zcbor_uint32_put(zs, MSG_KEY_ACCELERATION_Z);
		if (g_app_data.errors.acceleration_z) {
			zcbor_nil_put(zs, NULL);
		} else {
			zcbor_int32_put(zs, g_app_data.states.acceleration_z * 1000.f);
		}

		zcbor_uint32_put(zs, MSG_KEY_ORIENTATION);
		if (g_app_data.errors.orientation) {
			zcbor_nil_put(zs, NULL);
		} else {
			zcbor_uint32_put(zs, g_app_data.states.orientation);
		}

		zcbor_map_end_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);
	}

	zcbor_uint32_put(zs, MSG_KEY_BUTTON);
	{
		zcbor_map_start_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_X_CLICK_EVENT);
		zcbor_bool_put(zs, atomic_get(&g_app_data.sources.button_x_click));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_1_CLICK_EVENT);
		zcbor_bool_put(zs, atomic_get(&g_app_data.sources.button_1_click));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_2_CLICK_EVENT);
		zcbor_bool_put(zs, atomic_get(&g_app_data.sources.button_2_click));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_3_CLICK_EVENT);
		zcbor_bool_put(zs, atomic_get(&g_app_data.sources.button_3_click));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_4_CLICK_EVENT);
		zcbor_bool_put(zs, atomic_get(&g_app_data.sources.button_4_click));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_X_HOLD_EVENT);
		zcbor_bool_put(zs, atomic_get(&g_app_data.sources.button_x_hold));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_1_HOLD_EVENT);
		zcbor_bool_put(zs, atomic_get(&g_app_data.sources.button_1_hold));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_2_HOLD_EVENT);
		zcbor_bool_put(zs, atomic_get(&g_app_data.sources.button_2_hold));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_3_HOLD_EVENT);
		zcbor_bool_put(zs, atomic_get(&g_app_data.sources.button_3_hold));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_4_HOLD_EVENT);
		zcbor_bool_put(zs, atomic_get(&g_app_data.sources.button_4_hold));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_X_CLICK_COUNT);
		zcbor_uint32_put(zs, atomic_get(&g_app_data.events.button_x_click));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_1_CLICK_COUNT);
		zcbor_uint32_put(zs, atomic_get(&g_app_data.events.button_1_click));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_2_CLICK_COUNT);
		zcbor_uint32_put(zs, atomic_get(&g_app_data.events.button_2_click));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_3_CLICK_COUNT);
		zcbor_uint32_put(zs, atomic_get(&g_app_data.events.button_3_click));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_4_CLICK_COUNT);
		zcbor_uint32_put(zs, atomic_get(&g_app_data.events.button_4_click));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_X_HOLD_COUNT);
		zcbor_uint32_put(zs, atomic_get(&g_app_data.events.button_x_hold));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_1_HOLD_COUNT);
		zcbor_uint32_put(zs, atomic_get(&g_app_data.events.button_1_hold));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_2_HOLD_COUNT);
		zcbor_uint32_put(zs, atomic_get(&g_app_data.events.button_2_hold));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_3_HOLD_COUNT);
		zcbor_uint32_put(zs, atomic_get(&g_app_data.events.button_3_hold));

		zcbor_uint32_put(zs, MSG_KEY_BUTTON_4_HOLD_COUNT);
		zcbor_uint32_put(zs, atomic_get(&g_app_data.events.button_4_hold));

		zcbor_map_end_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);
	}

	zcbor_map_end_encode(zs, ZCBOR_VALUE_IS_INDEFINITE_LENGTH);

	atomic_set(&g_app_data.sources.button_x_click, false);
	atomic_set(&g_app_data.sources.button_x_hold, false);
	atomic_set(&g_app_data.sources.button_1_click, false);
	atomic_set(&g_app_data.sources.button_1_hold, false);
	atomic_set(&g_app_data.sources.button_2_click, false);
	atomic_set(&g_app_data.sources.button_2_hold, false);
	atomic_set(&g_app_data.sources.button_3_click, false);
	atomic_set(&g_app_data.sources.button_3_hold, false);
	atomic_set(&g_app_data.sources.button_4_click, false);
	atomic_set(&g_app_data.sources.button_4_hold, false);

	if (!zcbor_check_error(zs)) {
		LOG_ERR("Encoding failed: %d", zcbor_pop_error(zs));
		return -EFAULT;
	}

	return 0;
}
