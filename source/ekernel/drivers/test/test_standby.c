#include <getopt.h>
#include <log.h>
#include <rtthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <hal_uart.h>
#include <melis/standby/standby.h>

static int cmd_hal_standby(int argc, const char **argv)
{
	standby_enter(STANDBY_NORMAL);

	return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_hal_standby, __cmd_standby, standby test code);

static int cmd_hal_standby_for(int argc, const char **argv)
{
	int i = 0, m;

	if (argc > 1)
		m = strtol(argv[1], NULL, 0);
	else
		m = 100000;

	while (i < m) {
		printf("standby cnt: %d\n", ++i);
		standby_enter(STANDBY_TEST);
	}

	return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_hal_standby_for, __cmd_standby_for, standby code);
