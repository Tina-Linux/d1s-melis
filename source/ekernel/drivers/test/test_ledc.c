#include <log.h>
#include <rtthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <rtthread.h>
#include <sunxi_drv_ledc.h>
#include <sunxi_hal_ledc.h>

#define DEFAULT_BRIGHTNESS	127

void show_default_rgb(void)
{
	printf("======show default RGB======\n");

	sunxi_set_led_brightness(1, DEFAULT_BRIGHTNESS << 8);
	rt_thread_delay(1);
	sunxi_set_led_brightness(1, DEFAULT_BRIGHTNESS << 16);
	rt_thread_delay(1);
	sunxi_set_led_brightness(1, DEFAULT_BRIGHTNESS << 0);
	rt_thread_delay(1);
	sunxi_set_led_brightness(1, 0);
}

int ledc_test(int argc, char **argv)
{
	int brightness = 0;

	printf("========LEDC TEST========\n");

	sunxi_led_init();

	if(argc < 3)
	{
		printf("uasge : cmd_ledc [RGB] [rgb_brightness], eg:cmd_ledc R 100 \n");
		show_default_rgb();
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
	rt_thread_delay(1);
	sunxi_set_led_brightness(1, 0);

	return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(ledc_test, cmd_ledc, drv ledc test code);
