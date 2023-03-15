/*
 * A V4L2 driver for Raw cameras.
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *    Liang WeiJie <liangweijie@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <hal_timer.h>

#include "../../vin_mipi/combo_common.h"
#include "camera.h"
#include "../../utility/sunxi_camera_v2.h"
#include "../../utility/media-bus-format.h"
#include "../../utility/vin_supply.h"

#define MCLK            (24*1000*1000)
#define V4L2_IDENT_SENSOR   0x2355

//define the registers
#define EXP_HIGH    0x3e00
#define EXP_MID     0x3e01
#define EXP_LOW     0x3e02
#define DIG_GAIN    0x3e06
#define DIG_FINE_GAIN   0x3e07
#define ANA_GAIN    0x3e09
#define GAIN_STEP_BASE 128

#define ID_REG_HIGH 0x3e07
#define ID_REG_LOW  0x3e08
#define ID_VAL_HIGH 0x80
#define ID_VAL_LOW  0x03
#define SENSOR_FRAME_RATE   30

#define I2C_ADDR 0x60
//#define I2C_ADDR_2 0x32

#define SENSOR_NUM 0x2
#define SENSOR_NAME "sc2355_mipi"
#define SENSOR_NAME_2 "sc2355_mipi_2"

#if 0
#define SC2355_1600X1200_30FPS
#else
#define SC2355_800X600_30FPS
#endif

static int sensor_power_count[2];
static int sensor_stream_count[2];
static struct sensor_format_struct *current_win[2];

/*
 * The default register settings
 */

static struct regval_list sensor_default_regs[] = {

};

#ifdef SC2355_800X600_30FPS
static struct regval_list sensor_800x600_30_regs[] = {
	{0x0103, 0x01},
	{0x0100, 0x00},
	{0x36e9, 0x80},
	{0x36ea, 0x0f},
	{0x36eb, 0x24},
	{0x36ed, 0x14},
	{0x36e9, 0x01},
	{0x301f, 0x0c},
	{0x303f, 0x82},
	{0x3208, 0x03},
	{0x3209, 0x20},
	{0x320a, 0x02},
	{0x320b, 0x58},
	{0x320c, 0x07},//hts=1920
	{0x320d, 0x80},
	{0x320e, 0x04},//vts=1250	1920*1250*30=72
	{0x320f, 0xe2},
	{0x3211, 0x02},
	{0x3213, 0x02},
	{0x3215, 0x31},
	{0x3220, 0x01},
	{0x3248, 0x02},
	{0x3253, 0x0a},
	{0x3301, 0xff},
	{0x3302, 0xff},
	{0x3303, 0x10},
	{0x3306, 0x28},
	{0x3307, 0x02},
	{0x330a, 0x00},
	{0x330b, 0xb0},
	{0x3318, 0x02},
	{0x3320, 0x06},
	{0x3321, 0x02},
	{0x3326, 0x12},
	{0x3327, 0x0e},
	{0x3328, 0x03},
	{0x3329, 0x0f},
	{0x3364, 0x4f},
	{0x33b3, 0x40},
	{0x33f9, 0x2c},
	{0x33fb, 0x38},
	{0x33fc, 0x0f},
	{0x33fd, 0x1f},
	{0x349f, 0x03},
	{0x34a6, 0x01},
	{0x34a7, 0x1f},
	{0x34a8, 0x40},
	{0x34a9, 0x30},
	{0x34ab, 0xa6},
	{0x34ad, 0xa6},
	{0x3622, 0x60},
	{0x3623, 0x40},
	{0x3624, 0x61},
	{0x3625, 0x08},
	{0x3626, 0x03},
	{0x3630, 0xa8},
	{0x3631, 0x84},
	{0x3632, 0x90},
	{0x3633, 0x43},
	{0x3634, 0x09},
	{0x3635, 0x82},
	{0x3636, 0x48},
	{0x3637, 0xe4},
	{0x3641, 0x22},
	{0x3670, 0x0f},
	{0x3674, 0xc0},
	{0x3675, 0xc0},
	{0x3676, 0xc0},
	{0x3677, 0x86},
	{0x3678, 0x88},
	{0x3679, 0x8c},
	{0x367c, 0x01},
	{0x367d, 0x0f},
	{0x367e, 0x01},
	{0x367f, 0x0f},
	{0x3690, 0x63},
	{0x3691, 0x63},
	{0x3692, 0x73},
	{0x369c, 0x01},
	{0x369d, 0x1f},
	{0x369e, 0x8a},
	{0x369f, 0x9e},
	{0x36a0, 0xda},
	{0x36a1, 0x01},
	{0x36a2, 0x03},
	{0x3900, 0x0d},
	{0x3904, 0x04},
	{0x3905, 0x98},
	{0x391b, 0x81},
	{0x391c, 0x10},
	{0x391d, 0x19},
	{0x3933, 0x01},
	{0x3934, 0x82},
	{0x3940, 0x5d},
	{0x3942, 0x01},
	{0x3943, 0x82},
	{0x3949, 0xc8},
	{0x394b, 0x64},
	{0x3952, 0x02},
	{0x3e00, 0x00},
	{0x3e01, 0x4d},
	{0x3e02, 0xe0},
	{0x4502, 0x34},
	{0x4509, 0x30},
	{0x450a, 0x71},
	{0x4819, 0x09},
	{0x481b, 0x05},
	{0x481d, 0x13},
	{0x481f, 0x04},
	{0x4821, 0x0a},
	{0x4823, 0x05},
	{0x4825, 0x04},
	{0x4827, 0x05},
	{0x4829, 0x08},
	{0x5000, 0x46},
	{0x5900, 0x01},
	{0x5901, 0x04},
	{0x0100, 0x01},
};
#endif

#ifdef SC2355_1600X1200_30FPS
static struct regval_list sensor_1600x1200_30_regs[] = {
	{0x0103, 0x01},
	{0x0100, 0x00},
	{0x36e9, 0x80},
	{0x36ea, 0x0f},
	{0x36eb, 0x25},
	{0x36ed, 0x04},
	{0x36e9, 0x01},
	{0x301f, 0x0d},
	{0x320c, 0x07},//HTS=1900
	{0x320d, 0x6c},
	{0x320e, 0x04},//vts=1250
	{0x320f, 0xe2},
	{0x3248, 0x02},
	{0x3253, 0x0a},
	{0x3301, 0xff},
	{0x3302, 0xff},
	{0x3303, 0x10},
	{0x3306, 0x28},
	{0x3307, 0x02},
	{0x330a, 0x00},
	{0x330b, 0xb0},
	{0x3318, 0x02},
	{0x3320, 0x06},
	{0x3321, 0x02},
	{0x3326, 0x12},
	{0x3327, 0x0e},
	{0x3328, 0x03},
	{0x3329, 0x0f},
	{0x3364, 0x4f},
	{0x33b3, 0x40},
	{0x33f9, 0x2c},
	{0x33fb, 0x38},
	{0x33fc, 0x0f},
	{0x33fd, 0x1f},
	{0x349f, 0x03},
	{0x34a6, 0x01},
	{0x34a7, 0x1f},
	{0x34a8, 0x40},
	{0x34a9, 0x30},
	{0x34ab, 0xa6},
	{0x34ad, 0xa6},
	{0x3622, 0x60},
	{0x3623, 0x40},
	{0x3624, 0x61},
	{0x3625, 0x08},
	{0x3626, 0x03},
	{0x3630, 0xa8},
	{0x3631, 0x84},
	{0x3632, 0x90},
	{0x3633, 0x43},
	{0x3634, 0x09},
	{0x3635, 0x82},
	{0x3636, 0x48},
	{0x3637, 0xe4},
	{0x3641, 0x22},
	{0x3670, 0x0f},
	{0x3674, 0xc0},
	{0x3675, 0xc0},
	{0x3676, 0xc0},
	{0x3677, 0x86},
	{0x3678, 0x88},
	{0x3679, 0x8c},
	{0x367c, 0x01},
	{0x367d, 0x0f},
	{0x367e, 0x01},
	{0x367f, 0x0f},
	{0x3690, 0x63},
	{0x3691, 0x63},
	{0x3692, 0x73},
	{0x369c, 0x01},
	{0x369d, 0x1f},
	{0x369e, 0x8a},
	{0x369f, 0x9e},
	{0x36a0, 0xda},
	{0x36a1, 0x01},
	{0x36a2, 0x03},
	{0x3900, 0x0d},
	{0x3904, 0x06},
	{0x3905, 0x98},
	{0x391b, 0x81},
	{0x391c, 0x10},
	{0x391d, 0x19},
	{0x3933, 0x01},
	{0x3934, 0x82},
	{0x3940, 0x5d},
	{0x3942, 0x01},
	{0x3943, 0x82},
	{0x3949, 0xc8},
	{0x394b, 0x64},
	{0x3952, 0x02},
	{0x3e00, 0x00},
	{0x3e01, 0x4d},
	{0x3e02, 0xe0},
	{0x4502, 0x34},
	{0x4509, 0x30},
	{0x450a, 0x71},
	{0x0100, 0x01},
};
#endif

/*
 * Here we'll try to encapsulate the changes for just the output
 * video format.
 *
 */

static struct regval_list sensor_fmt_raw[] = {

};

static int sensor_s_exp(int id, unsigned int exp_val)
{
	unsigned int tmp_exp_val = exp_val;

	sensor_write(id, EXP_HIGH, (tmp_exp_val >> 12) & 0x0f);
	sensor_write(id, EXP_MID, (tmp_exp_val >> 4) & 0xFF);
	sensor_write(id, EXP_LOW, (tmp_exp_val << 4) & 0xf0);

	return 0;
}


unsigned char analog_Gain_Reg[] =
	{0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f};

static int setSensorGain(int id, int gain)
{
	int ana_gain = gain / GAIN_STEP_BASE;
	int gain_flag = 1;

	if(ana_gain >= 32 * 2) {
		gain_flag = 5;
	} else if(ana_gain >= 32) {
		gain_flag = 5;
	} else if(ana_gain >= 16) {
		gain_flag = 4;
	} else if(ana_gain >= 8) {
		gain_flag = 3;
	} else if(ana_gain >= 4) {
		gain_flag = 2;
	} else if(ana_gain >= 2) {
		gain_flag = 1;
	} else {
		gain_flag = 0;
	}

	sensor_write(id, ANA_GAIN, analog_Gain_Reg[gain_flag]);
	int dig_Gain = gain >> gain_flag;
	if(dig_Gain < 2 * GAIN_STEP_BASE) {
		//1/128
		sensor_write(id, DIG_GAIN, 0x00);
		sensor_write(id, DIG_FINE_GAIN, dig_Gain - 128 + 0x80);
		//sensor_print("sensor set analog_gain:0x%02x, dig_gain:0x%02x, dig_fine_gain:0x%02x", analog_Gain_Reg[gain_flag], 0x00, dig_Gain - 128 + 0x80);
	} else if(dig_Gain < 4 * GAIN_STEP_BASE) {
		//1/64
		sensor_write(id, DIG_GAIN, 0x01);
		sensor_write(id, DIG_FINE_GAIN, (dig_Gain - 128 * 2) / 2 + 0x80);
		//sensor_print("sensor set analog_gain:0x%02x, dig_gain:0x%02x, dig_fine_gain:0x%02x", analog_Gain_Reg[gain_flag], 0x01, (dig_Gain - 128 * 2) / 2 + 0x80);
	} else if(dig_Gain < 8 * GAIN_STEP_BASE) {
		//1/32
		sensor_write(id, DIG_GAIN, 0x03);
		sensor_write(id, DIG_FINE_GAIN, (dig_Gain - 128 * 4) / 4 + 0x80);
		//sensor_print("sensor set analog_gain:0x%02x, dig_gain:0x%02x, dig_fine_gain:0x%02x", analog_Gain_Reg[gain_flag], 0x03, (dig_Gain - 128 * 4) / 4 + 0x80);
	} else {
		sensor_write(id, DIG_GAIN, 0x03);
		sensor_write(id, DIG_FINE_GAIN, 0xfe);
	}

    return 0;
}

static int sensor_s_gain(int id, int gain_val)
{
	sensor_dbg("gain_val:%d\n", gain_val);
	setSensorGain(id, gain_val);

	return 0;
}

static int sc2355_sensor_vts;
static int sensor_s_exp_gain(int id, struct sensor_exp_gain *exp_gain)
{
	int exp_val,gain_val;
	int shutter = 0, frame_length = 0;

	//sensor_print("sensor_s_exp_gain exp:%d gain:%d\n", exp_gain->gain_val, exp_gain->exp_val);
	if((exp_gain->exp_val - exp_gain->exp_val / 16 * 16) > 0) {
		exp_val = exp_gain->exp_val / 16 + 1;
	} else {
		exp_val = exp_gain->exp_val / 16;
	}
	exp_val = exp_val & 0xffff;

	gain_val = exp_gain->gain_val * GAIN_STEP_BASE;
	if((gain_val - gain_val / 16 * 16) > 0) {
		gain_val = gain_val / 16 + 1;
	} else {
		gain_val = gain_val / 16;
	}

	sensor_s_exp(id, exp_val);
	sensor_s_gain(id, gain_val);

    return 0;
}

static int sensor_power(int id, int on)
{

	if (on && (sensor_power_count[id])++ > 0) {
		return 0;
	} else if (!on && (sensor_power_count[id] == 0 || --(sensor_power_count[id]) > 0)) {
		return 0;
	}

	switch(on){
	case PWR_ON:
		sensor_dbg("PWR_ON!\n");
		vin_set_mclk(id, 0);
		hal_usleep(1000);
		vin_set_mclk_freq(id, MCLK);
		vin_set_mclk(id, 1);
		hal_usleep(1000);
//		if(id == 0)
//		{
			vin_gpio_set_status(id, PWDN, 1);
			vin_gpio_set_status(id, RESET, 1);
			vin_gpio_write(id, PWDN, CSI_GPIO_LOW);
			vin_gpio_write(id, RESET, CSI_GPIO_LOW);
			hal_usleep(1000);
			vin_gpio_write(id, PWDN, CSI_GPIO_HIGH);
			vin_gpio_write(id, RESET, CSI_GPIO_HIGH);
			hal_usleep(1000);
//		}
	    break;
	case PWR_OFF:
		sensor_dbg("PWR_OFF!do nothing\n");
		vin_set_mclk(id, 0);
		hal_usleep(1000);
		vin_gpio_set_status(id, PWDN, 1);
		vin_gpio_set_status(id, RESET, 1);
		vin_gpio_write(id, PWDN, CSI_GPIO_LOW);
		vin_gpio_write(id, RESET, CSI_GPIO_LOW);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int sensor_detect(int id){
	data_type rdval;
	/*int eRet;
	int times_out = 3;

	do{
		eRet = sensor_read(id, ID_REG_HIGH, &rdval);
		sensor_print("eRet:%d, ID_VAL_HIGH:0x%x,times_out:%d\n", eRet, rdval, times_out);
		hal_usleep(200);
		times_out--;
	}while(eRet < 0 && times_out > 0);*/

	sensor_read(id, ID_REG_HIGH, &rdval);
	sensor_print("ID_VAL_HIGH = %2x, Done!\n", rdval);

	if(rdval != ID_VAL_HIGH)
		return -ENODEV;

	sensor_read(id, ID_REG_LOW, &rdval);
	sensor_print("ID_VAL_LOW = %2x, Done!\n", rdval);
	if(rdval != ID_VAL_LOW)
		return -ENODEV;
	sensor_dbg("Done!\n");

	return 0;
}

static int sensor_init(int id)
{
	int ret;

	sensor_dbg("sensor_init\n");
	ret = sensor_detect(id);
	if(ret) {
		sensor_err("chip found is not an target chip.\n");
		return ret;
	}

	return 0;
}

/*
 * Store information about the video data format.
 */
static struct sensor_format_struct sensor_formats[] = {
#ifdef SC2355_800X600_30FPS
	{
		.mbus_code = MEDIA_BUS_FMT_SBGGR10_1X10, /*.mbus_code = MEDIA_BUS_FMT_SBGGR10_1X10, */
		.width      = 800,
		.height     = 600,//1080,
		.hoffset    = 0,
		.voffset    = 0,
		.hts        = 1920,
		.vts        = 1250,
		.pclk       = 72000000,
		.mipi_bps   = 297 * 1000 * 1000,
		.fps_fixed  = 30,
		.bin_factor = 1,
		.intg_min   = 1 << 4,
		.intg_max   = 1125 << 4,
		.gain_min   = 1 << 4,
		.gain_max   = 110 << 4,
		.offs_h     = 0,
		.offs_v     = 0,
		.regs	    = sensor_800x600_30_regs,
		.regs_size  = ARRAY_SIZE(sensor_800x600_30_regs),
	},
#endif

#ifdef SC2355_1600X1200_30FPS
	{
		.mbus_code = MEDIA_BUS_FMT_SBGGR10_1X10, /*.mbus_code = MEDIA_BUS_FMT_SBGGR10_1X10, */
		.width      = 1600,
		.height     = 1200,//1080,
		.hoffset    = 0,
		.voffset    = 0,
		.hts        = 1920,
		.vts        = 1250,
		.pclk       = 72000000,
		.mipi_bps   = 297 * 1000 * 1000,
		.fps_fixed  = 30,
		.bin_factor = 1,
		.intg_min   = 1 << 4,
		.intg_max   = 1125 << 4,
		.gain_min   = 1 << 4,
		.gain_max   = 110 << 4,
		.offs_h     = 0,
		.offs_v     = 0,
		.regs	    = sensor_1600x1200_30_regs,
		.regs_size  = ARRAY_SIZE(sensor_1600x1200_30_regs),
	},
#endif
};

static struct sensor_format_struct *sensor_get_format(int id)
{
	struct sensor_format_struct *sensor_format = NULL;
	int wdr_on = isp_get_cfg[id].sensor_wdr_on;
	int fps = isp_get_cfg[id].sensor_get_fps;
	int i;

	if (current_win[id])
		return current_win[id];

	for (i = 0; i < ARRAY_SIZE(sensor_formats); i++) {
		if (sensor_formats[i].wdr_mode == wdr_on) {
			if (sensor_formats[i].fps_fixed == fps) {
				sensor_format = &sensor_formats[i];
				sensor_print("fine wdr is %d, fine fps is %d\n", wdr_on, fps);
				goto done;
			}
		}
	}

	if (sensor_format == NULL) {
		for (i = 0; i < ARRAY_SIZE(sensor_formats); i++) {
			if (sensor_formats[i].wdr_mode == wdr_on) {
				sensor_format = &sensor_formats[i];
				isp_get_cfg[id].sensor_get_fps = sensor_format->fps_fixed;
				sensor_print("fine wdr is %d, use fps is %d\n", wdr_on, sensor_format->fps_fixed);
				goto done;
			}
		}
	}

	if (sensor_format == NULL) {
		sensor_format = &sensor_formats[0];
		isp_get_cfg[id].sensor_wdr_on = sensor_format->wdr_mode;
		isp_get_cfg[id].sensor_get_fps = sensor_format->fps_fixed;
		sensor_print("use wdr is %d, use fps is %d\n", sensor_format->wdr_mode, sensor_format->fps_fixed);
	}

done:
	current_win[id] = sensor_format;
	return sensor_format;
}

static int sensor_g_mbus_config(int id, struct v4l2_mbus_config *cfg, struct mbus_framefmt_res *res)
{
	cfg->type  = V4L2_MBUS_CSI2;
	cfg->flags = 0 | V4L2_MBUS_CSI2_1_LANE | V4L2_MBUS_CSI2_CHANNEL_0;
	res->res_time_hs = 0x28;

	return 0;
}

static int sensor_reg_init(int id)
{
	sensor_dbg("step sensor_reg_init\n");
	int ret;
	struct sensor_exp_gain exp_gain;

	ret = sensor_write_array(id, sensor_default_regs,
				 ARRAY_SIZE(sensor_default_regs));
	if (ret < 0) {
		sensor_err("write sensor_default_regs error\n");
		return ret;
	}

	if (current_win[id]->regs)
		ret = sensor_write_array(id, current_win[id]->regs, current_win[id]->regs_size);
	if (ret < 0)
		return ret;

#ifndef CONFIG_ISP_READ_THRESHOLD
	exp_gain.exp_val = 15408;
	exp_gain.gain_val = 16;
#else
	if (id == 0) {
		exp_gain.exp_val = clamp(*((unsigned int *)ISP0_NORFLASH_SAVE + 2), 16, 1125 << 4);
		exp_gain.gain_val = clamp(*((unsigned int *)ISP0_NORFLASH_SAVE + 1), 16, 110 << 4);
	} else {
		exp_gain.exp_val = clamp(*((unsigned int *)ISP1_NORFLASH_SAVE + 2), 16, 1125 << 4);
		exp_gain.gain_val = clamp(*((unsigned int *)ISP1_NORFLASH_SAVE + 1), 16, 110 << 4);
	}
#endif
	sensor_s_exp_gain(id, &exp_gain);

	sc2355_sensor_vts = current_win[id]->vts;
	//sensor_dbg("sc2355_sensor_vts = %d\n", sc2355_sensor_vts);

	return 0;
}

static int sensor_s_stream(int id, int enable)
{
	sensor_dbg("step sensor_s_stream\n");
	if (enable && sensor_stream_count[id]++ > 0)
		return 0;
	else if (!enable && (sensor_stream_count[id] == 0 || --sensor_stream_count[id] > 0))
		return 0;

	if (!enable)
		return 0;

	return sensor_reg_init(id);
}

static int sensor_test_i2c(int id)
{
	int ret;
	sensor_power(id, PWR_ON);
	ret = sensor_init(id);
	sensor_power(id, PWR_OFF);

	return ret;
}

struct sensor_fuc_core sc2355_core  = {
	.g_mbus_config = sensor_g_mbus_config,
	.sensor_test_i2c = sensor_test_i2c,
	.sensor_power = sensor_power,
	.s_stream = sensor_s_stream,
	.s_exp_gain = sensor_s_exp_gain,
	.sensor_g_format = sensor_get_format,
};
