#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <rtthread.h>

#include <sunxi_input.h>
#include <sunxi_drv_rtp.h>

int cmd_rtp(int argc, char **argv)
{
	int fd = -1;
	struct sunxi_input_event event;

	printf("========RTP TEST========\n");

	fd = sunxi_input_open("sunxi-rtp");
	if (fd < 0)
	{
		printf("====rtp open err====\n");
		printf("must run sunxi_rtp_init\n");
		return -1;
	}

	while(1)
	{
		if (sunxi_input_read(fd, &event, sizeof(struct sunxi_input_event)) < 0)
		{
			continue;
		}

		if ((event.type != EV_ABS) && (event.type != EV_KEY))
		{
			continue;
		}

		printf("=====rtp code %d====\n", event.code);
		printf("=====rtp value  %d====\n", event.value);
		printf("\n");
	}

	return 0;
}

int cmd_tpadc_adc(int argc, char **argv)
{
	printf("========RTPADC ADC MODE  TEST========\n");

	int data;

	drv_tpadc_adc_init();

	drv_tpadc_adc_channel_init(TP_CH0_SELECT);
	drv_tpadc_adc_channel_init(TP_CH1_SELECT);
	drv_tpadc_adc_channel_init(TP_CH2_SELECT);
	drv_tpadc_adc_channel_init(TP_CH3_SELECT);

	while(1)
	{
		data = drv_get_tpadc_adc_data(TP_CH0_SELECT);
		printf("channel 1 data is data");

		data = drv_get_tpadc_adc_data(TP_CH1_SELECT);
		printf("channel 2 data is %d", data);

		data = drv_get_tpadc_adc_data(TP_CH2_SELECT);
		printf("channel 3 data is %d", data);

		data = drv_get_tpadc_adc_data(TP_CH3_SELECT);
		printf("channel 4 data is %d", data);
	}

	return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_tpadc_adc, cmd_tpadc_adc, tpadc adc mode test code);
