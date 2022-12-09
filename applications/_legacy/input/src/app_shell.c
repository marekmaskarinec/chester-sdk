#include "app_config.h"

/* Zephyr includes */
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>
#include <zephyr/zephyr.h>

LOG_MODULE_REGISTER(app_shell, LOG_LEVEL_INF);

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

	SHELL_CMD_ARG(active-filter, NULL,
	              "Get/Set active filter in milliseconds (format: <10-3600000>).",
	              app_config_cmd_config_active_filter, 1, 1),

	SHELL_CMD_ARG(inactive-filter, NULL,
	              "Get/Set inactive filter in milliseconds (format: <10-3600000>).",
	              app_config_cmd_config_inactive_filter, 1, 1),

	SHELL_CMD_ARG(report-interval, NULL,
	              "Get/Set report interval in seconds (format: <30-86400>).",
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

/* clang-format on */