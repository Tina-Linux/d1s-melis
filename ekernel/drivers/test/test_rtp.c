/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
