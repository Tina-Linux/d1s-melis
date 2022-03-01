#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <rtthread.h>

#include <sunxi_input.h>
#include "sunxi_drv_cir.h"

int cmd_cir(int argc, char **argv)
{
	int i, ret = -1;
	int key_count = 0;
	int fd = -1;

	struct sunxi_input_event event;

	printf("========KEY TEST========\n");

	fd = sunxi_input_open("sunxi-ir-rx");
	if (fd < 0)
	{
		printf("====keyboard open err====\n");
		return -1;
	}

	printf("press all keys\n");


	if (sunxi_input_read(fd, &event, sizeof(struct sunxi_input_event)) < 0) {
		printf("no scancode\n");
	}

	if (event.type != EV_KEY) {
		printf("no EVKEY\n");
	}

	printf("=====key code %d====\n", event.code);

	return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_cir, cmd_cir_test, keyboard test code);
