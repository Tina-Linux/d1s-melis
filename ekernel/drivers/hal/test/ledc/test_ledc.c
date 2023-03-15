#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <hal_timer.h>
#include <hal_cmd.h>
#include "sunxi_hal_ledc.h"

#define DEFAULT_BRIGHTNESS	127

void show_default_rgb(void)
{
	printf("======show default RGB======\n");
	sunxi_set_led_brightness(1, DEFAULT_BRIGHTNESS << 8);
	sunxi_set_led_brightness(1, DEFAULT_BRIGHTNESS << 16);
	sunxi_set_led_brightness(1, DEFAULT_BRIGHTNESS << 0);
	sunxi_set_led_brightness(1, 0);
}

int ledc_test(int argc, char **argv)
{
	int brightness = 0;

	printf("========LEDC TEST========\n");

	hal_ledc_init();

	if(argc < 3)
	{
		show_default_rgb();
		printf("uasge : cmd_ledc [RGB] [rgb_brightness], eg:cmd_ledc R 100 \n");
		return 0;
	}

	brightness = atoi(argv[2]);

	switch(argv[1][0])
	{
		case 'R' : brightness <<= 8; break;
		case 'G' : brightness <<= 16; break;
		case 'B' : brightness <<= 0; break;
		default  : printf("parameters err\n");
			   return -1;
	}

	sunxi_set_led_brightness(1, brightness);
	printf("brightness is %d\n", brightness);

	return 0;
}

FINSH_FUNCTION_EXPORT_CMD(ledc_test, hal_ledc, drv ledc test_code);
