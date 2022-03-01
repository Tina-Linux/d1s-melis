/*
 * ===========================================================================================
 *
 *       Filename:  drv_rtp.c
 *
 *    Description:  implemtaton of tpadc driver core based on hal.
 *
 *        Version:  Melis3.0
 *         Create:  2020-1-13
 *       Revision:  none
 *       Compiler:  GCC:version 9.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  liuyu@allwinnertech.com
 *   Organization:  SWC-BPD
 *  Last Modified:  2021-1-14
 *
 * ===========================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <sunxi_drv_rtp.h>
#include <sunxi_hal_tpadc.h>
#include <sunxi_input.h>
#include <log.h>
#include <init.h>
#include <standby/standby.h>

struct sunxi_input_dev *input_rtp= NULL;
uint32_t data_0, data_1, data_2, data_3;

#ifdef CONFIG_STANDBY
int sunxi_tpadc_resume(void)
{
	hal_tpadc_resume();
	return 0;
}

int sunxi_tpadc_suspend(void)
{
	hal_tpadc_suspend();
	return 0;
}
#endif

/* tpadc : the rtpadc mode interface */
static int sunxi_rtp_register(struct sunxi_input_dev *input_dev)
{
	uint32_t ret;

	input_dev->name = "sunxi-rtp";
	input_set_capability(input_dev, EV_ABS, ABS_X);
	input_set_capability(input_dev, EV_ABS, ABS_Y);
	input_set_capability(input_dev, EV_KEY, BTN_TOUCH);

	ret = sunxi_input_register_device(input_dev);
	if (ret) {
		pr_err("rtp input register fail\n");
		return -1;
	}

	return 0;
}

/* sunxi_tpadc_irq_usercallback : report specific events to the input layer
 * data : the data collected from hal layer
 * flag : x or y data flag, 0 : x_data, 1 : y_data
 */
int sunxi_rtp_irq_usercallback(uint32_t data, data_flag_t flag)
{
	switch(flag) {
	case 0 :
		input_report_abs(input_rtp, ABS_X, data);
		input_report_key(input_rtp, BTN_TOUCH, 1);
		input_sync(input_rtp);
		break;
	case 1 :
		input_report_abs(input_rtp, ABS_Y, data);
		input_report_key(input_rtp, BTN_TOUCH, 1);
		input_sync(input_rtp);
		break;
	case 2 :
		input_report_key(input_rtp, BTN_TOUCH, 0);
		input_sync(input_rtp);
		break;
	default :
		TPADC_ERR("data flag error ");
		break;
	}

	return 0;
}

int drv_tpadc_register_callback(tpadc_usercallback_t callback)
{
	int ret = -1;
	if(!callback)
	{
		TPADC_ERR("sunxi tpadc  callback null \n");
		return -1;
	}
	ret = hal_tpadc_register_callback(callback);
	if(ret < 0)
	{
		return -1;
	}
	return 0;
}

int sunxi_rtp_init(void)
{
	int ret, i;

	TPADC_INFO("sunxi tpadc init start \n");

	ret = hal_tpadc_init();
	if (ret)
		TPADC_ERR("sunxi tpadc  init failed \n");

	input_rtp= sunxi_input_allocate_device();
	if (input_rtp == NULL) {
		TPADC_ERR("rtp allocate input_dev failed.\n");
		return -1;
	}

	ret = sunxi_rtp_register(input_rtp);
	if (ret) {
		TPADC_ERR("rtp register error \n");
	}

	hal_tpadc_register_callback(sunxi_rtp_irq_usercallback);

	TPADC_INFO("sunxi tpadc init success \n");

	return 0;
}

/* tpadc : auxiliary adc mode interface */
static int sunxi_tpadc_adc_usercallback(uint32_t data, tp_channel_id channel)
{
	switch(channel) {
	case 0 :
		data_0 = data;
		TPADC_INFO("channel 1 adc data is %d", data_0);
		break;
	case 1 :
		data_1 = data;
		TPADC_INFO("channel 2 adc data is %d", data_1);
		break;
	case 2 :
		data_2 = data;
		TPADC_INFO("channel 3 adc data is %d", data_2);
		break;
	case 3 :
		data_3 = data;
		TPADC_INFO("channel 4 adc data is %d", data_3);
		break;
	default :
		break;
	}

	return 0;
}

int drv_tpadc_adc_init(void)
{
	int ret, i;

	TPADC_INFO("sunxi tpadc init start \n");

	ret = hal_tpadc_adc_init();
	if (ret)
	{
		TPADC_ERR("sunxi tpadc  init failed \n");
		hal_tpadc_adc_exit();
		return -1;
	}

#ifdef CONFIG_STANDBY
	register_pm_dev_notify(sunxi_tpadc_suspend, sunxi_tpadc_resume, NULL);
#endif

	return 0;
}

int drv_tpadc_adc_channel_init(tp_channel_id channel)
{
	if (hal_tpadc_adc_channel_init(channel))
	{
		TPADC_ERR("tpadc adc channel init error");
		return -1;
	}

	hal_tpadc_adc_register_callback(channel, sunxi_tpadc_adc_usercallback);

	TPADC_INFO("sunxi tpadc init success \n");

	return 0;
}

int drv_get_tpadc_adc_data(tp_channel_id channel)
{
	switch(channel) {
	case 0 :
		return data_0;
		break;
	case 1 :
		return data_1;
		break;
	case 2 :
		return data_2;
		break;
	case 3 :
		return data_3;
		break;
	default :
		return -1;
	}
}


//late_initcall(sunxi_rtp_init);
