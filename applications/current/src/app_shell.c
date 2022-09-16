#include "app_config.h"
#include "app_measure.h"
#include "app_send.h"

/* Zephyr includes */
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>
#include <zephyr/zephyr.h>

LOG_MODULE_REGISTER(app_shell, LOG_LEVEL_INF);

static int cmd_measure(const struct shell *shell, size_t argc, char **argv)
{
	if (argc > 1) {
		shell_error(shell, "unknown parameter: %s", argv[1]);
		shell_help(shell);
		return -EINVAL;
	}

	k_timer_start(&g_app_measure_timer, K_NO_WAIT, K_FOREVER);

	return 0;
}

static int cmd_send(const struct shell *shell, size_t argc, char **argv)
{
	if (argc > 1) {
		shell_error(shell, "unknown parameter: %s", argv[1]);
		shell_help(shell);
		return -EINVAL;
	}

	k_timer_start(&g_app_send_timer, K_NO_WAIT, K_FOREVER);

	return 0;
}

static int print_help(const struct shell *shell, size_t argc, char **argv)
{
	if (argc > 1) {
		shell_error(shell, "command not found: %s", argv[1]);
		shell_help(shell);
		return -EINVAL;
	}

	shell_help(shell);

	return 0;
}

/* clang-format off */

SHELL_STATIC_SUBCMD_SET_CREATE(
	sub_app_config,

	SHELL_CMD_ARG(show, NULL,
	              "List current configuration.",
	              app_config_cmd_config_show, 1, 0),

	SHELL_CMD_ARG(channel-active, NULL,
	              "Get/Set channel activation (format: <channel> <true|false>).",
	              app_config_cmd_config_channel_active, 2, 1),

	SHELL_CMD_ARG(channel-differential, NULL,
	              "Get/Set channel differential mode (format: <channel> <true|false>).",
	              app_config_cmd_config_channel_differential, 2, 1),

	SHELL_CMD_ARG(channel-calib-x0, NULL,
	              "Get/Set channel X0 calibration point "
	              "(format: <channel> <-2147483648..2147483647>).",
	              app_config_cmd_config_channel_calib_x0, 2, 1),

	SHELL_CMD_ARG(channel-calib-y0, NULL,
	              "Get/Set channel Y0 calibration point "
	              "(format: <channel> <-2147483648..2147483647>).",
	              app_config_cmd_config_channel_calib_y0, 2, 1),

	SHELL_CMD_ARG(channel-calib-x1, NULL,
	              "Get/Set channel X1 calibration point "
	              "(format: <channel> <-2147483648..2147483647>).",
	              app_config_cmd_config_channel_calib_x1, 2, 1),

	SHELL_CMD_ARG(channel-calib-y1, NULL,
	              "Get/Set channel Y1 calibration point "
	              "(format: <channel> <-2147483648..2147483647>).",
	              app_config_cmd_config_channel_calib_y1, 2, 1),

	SHELL_CMD_ARG(measurement-interval, NULL,
	              "Get/Set measurement interval in seconds (format: <5..3600>).",
	              app_config_cmd_config_measurement_interval, 1, 1),

	SHELL_CMD_ARG(report-interval, NULL,
	              "Get/Set report interval in seconds (format: <30..86400>).",
	              app_config_cmd_config_report_interval, 1, 1),

	SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(
	sub_app,

	SHELL_CMD_ARG(config, &sub_app_config, "Configuration commands.",
	              print_help, 1, 0),

	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(app, &sub_app, "Application commands.", print_help);

SHELL_CMD_REGISTER(measure, NULL, "Start measurement immediately.", cmd_measure);
SHELL_CMD_REGISTER(send, NULL, "Send data immediately.", cmd_send);

/* clang-format on */
