#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <hal_cmd.h>
#include <sunxi_hal_watchdog.h>
#include <pthread.h>

#include <hal_time.h>

extern void udelay(unsigned int us);

int cmd_watchdog(int argc, char **argv)
{
	int to;

	printf("========WATCHDOG TEST========\n");

	hal_watchdog_init();

	hal_watchdog_info();

	printf("\nTEST1: Feeding watchdog, timeout=5s. Should not reset...\n");
	hal_watchdog_reset(5);
	to = 5;
	while (to--) {
		hal_watchdog_feed();
		udelay(1*1000*1000);
	}
	udelay(1*1000*1000);

	printf("\nTEST2: Disabling watchdog, timeout=3s. Should not reset...\n");
	hal_watchdog_reset(3);
	hal_watchdog_disable();
	udelay(5*1000*1000);

	printf("\nTEST3: Watchdog bark, timeout=1s. Waiting for CPU reset...\n");
	hal_watchdog_reset(1);
	udelay(2*1000*1000);
	return 0;
}

int cmd_watchdog_reset(int argc, char **argv)
{
	printf("========WATCHDOG RESET TEST========\n");
	printf("TEST1: reset_cpu(). Waiting for CPU reset...\n");
	hal_watchdog_init();
	hal_watchdog_restart();

	return 0;
}
FINSH_FUNCTION_EXPORT_CMD(cmd_watchdog, hal_watchdog, Watchdog hal APIs tests);

FINSH_FUNCTION_EXPORT_CMD(cmd_watchdog_reset, watchdog_reset_test, Watchdog reset test code);
