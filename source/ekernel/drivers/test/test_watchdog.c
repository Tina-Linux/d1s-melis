#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <hal_osal.h>
#include "sunxi_drv_watchdog.h"
#include <pthread.h>

extern void sleep(int seconds);

pthread_t thread;

void* feed_thread(void *temp)
{
	int time = 10000;
	while (time--) {
		drv_watchdog_feed();
		sleep(1);
	}

}

int cmd_watchdog(int argc, char **argv)
{
	int err;
	int to;

	printf("========WATCHDOG TEST========\n");

	drv_watchdog_info();
/*
	// late_initcall() will do the init stuff...
	err = sunxi_wdt_init();
	if (err) {
		printf("ERROR: sunxi_wdt_init() failed\n");
		return err;
	}
*/
	printf("\nTEST1: Feeding watchdog, timeout=5s. Should not reset...\n");
	drv_watchdog_reset(5);
	to = 5;
	while (to--) {
		drv_watchdog_feed();
		sleep(1);
	}
	sleep(1);

	printf("\nTEST2: Disabling watchdog, timeout=3s. Should not reset...\n");
	drv_watchdog_reset(3);
	drv_watchdog_disable();
	sleep(5);

	printf("\nTEST3: Watchdog bark, timeout=1s. Waiting for CPU reset...\n");
	drv_watchdog_reset(1);
	sleep(2);

	return 0;
}

int cmd_watchdog_reset(int argc, char **argv)
{
	int err;

	printf("========WATCHDOG RESET TEST========\n");
	printf("TEST1: reset_cpu(). Waiting for CPU reset...\n");
	reset_cpu();

	return 0;
}

int cmd_watchdog_suspend(int argc, char **argv)
{
	int err;
	int i = 10;

	printf("==============WATCHDOG SUSPEND TEST=================\n");
	printf("\nTEST1: Feeding watchdog, timeout=5s. Should not reset...\n");
	drv_watchdog_reset(5);
	err = pthread_create(&thread, NULL, feed_thread, &i);

}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_watchdog, __cmd_watchdog_test, Watchdog test code);
FINSH_FUNCTION_EXPORT_ALIAS(cmd_watchdog_reset, cmd_watchdog_reset_test, Watchdog reset test code);
FINSH_FUNCTION_EXPORT_ALIAS(cmd_watchdog_suspend, cmd_watchdog_suspend_test, Watchdog suspend test code);
