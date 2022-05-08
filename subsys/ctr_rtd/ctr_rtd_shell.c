/* CHESTER includes */
#include <ctr_rtd.h>

/* Zephyr includes */
#include <logging/log.h>
#include <shell/shell.h>
#include <zephyr.h>

/* Standard includes */
#include <stddef.h>
#include <string.h>

LOG_MODULE_REGISTER(ctr_rtd_shell, CONFIG_CTR_RTD_LOG_LEVEL);

static int cmd_rtd_read(const struct shell *shell, size_t argc, char **argv)
{
	int ret;

	enum ctr_rtd_channel channel;

	if (strcmp(argv[1], "a1") == 0) {
		channel = CTR_RTD_CHANNEL_A1;
	} else if (strcmp(argv[1], "a2") == 0) {
		channel = CTR_RTD_CHANNEL_A2;
	} else if (strcmp(argv[1], "b1") == 0) {
		channel = CTR_RTD_CHANNEL_B1;
	} else if (strcmp(argv[1], "b2") == 0) {
		channel = CTR_RTD_CHANNEL_B2;
	} else {
		shell_error(shell, "invalid channel: %s", argv[1]);
		shell_help(shell);
		return -EINVAL;
	}

	enum ctr_rtd_type type;

	if (strcmp(argv[2], "pt100") == 0) {
		type = CTR_RTD_TYPE_PT100;
	} else if (strcmp(argv[2], "pt1000") == 0) {
		type = CTR_RTD_TYPE_PT1000;
	} else {
		shell_error(shell, "invalid type: %s", argv[2]);
		shell_help(shell);
		return -EINVAL;
	}

	float temperature;
	ret = ctr_rtd_read(channel, type, &temperature);
	if (ret) {
		LOG_ERR("Call `ctr_rtd_read` failed: %d", ret);
		shell_error(shell, "command failed");
		return ret;
	}

	shell_print(shell, "temperature: %.3f celsius", temperature);

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
	sub_rtd,

	SHELL_CMD_ARG(read, NULL,
	              "Read temperature (format: <a1|a2|b1|b2> <pt100|pt1000>).",
	              cmd_rtd_read, 3, 0),

        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(rtd, &sub_rtd, "RTD commands.", print_help);

/* clang-format on */