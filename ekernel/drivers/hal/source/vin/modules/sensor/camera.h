
/*
 * header for cameras.
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *	Yang Feng <yangfeng@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __CAMERA__H__
#define __CAMERA__H__

#include <sunxi_hal_common.h>
#include "../../vin.h"
#include "../../utility/vin_common.h"
#include "../../vin_cci/cci_helper.h"
#include "../../platform/platform_cfg.h"

/*
 * Basic window sizes.  These probably belong somewhere more globally
 * useful.
 */
#define ABS_SENSOR(x)                 ((x) > 0 ? (x) : -(x))

#define HXGA_WIDTH    4000
#define HXGA_HEIGHT   3000
#define QUXGA_WIDTH   3264
#define QUXGA_HEIGHT  2448
#define QSXGA_WIDTH   2592
#define QSXGA_HEIGHT  1936
#define QXGA_WIDTH    2048
#define QXGA_HEIGHT   1536
#define HD1080_WIDTH  1920
#define HD1080_HEIGHT 1080
#define UXGA_WIDTH    1600
#define UXGA_HEIGHT   1200
#define SXGA_WIDTH    1280
#define SXGA_HEIGHT   960
#define HD720_WIDTH   1280
#define HD720_HEIGHT  720
#define XGA_WIDTH     1024
#define XGA_HEIGHT    768
#define SVGA_WIDTH    800
#define SVGA_HEIGHT   600
#define VGA_WIDTH     640
#define VGA_HEIGHT    480
#define QVGA_WIDTH    320
#define QVGA_HEIGHT   240
#define CIF_WIDTH     352
#define CIF_HEIGHT    288
#define QCIF_WIDTH    176
#define QCIF_HEIGHT   144

#define CSI_GPIO_HIGH     1
#define CSI_GPIO_LOW     0
#define CCI_BITS_8           8
#define CCI_BITS_16         16
#define SENSOR_MAGIC_NUMBER 0x156977

#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))

struct isp_get_cfg_t {
	unsigned int sensor_wdr_on;
	unsigned int sensor_get_fps;
	unsigned char sensor_deinit;
	unsigned char get_yuv_en;
};

extern struct isp_get_cfg_t isp_get_cfg[2];

enum power_seq_cmd {
	PWR_OFF = 0,
	PWR_ON = 1,
};

enum ir_status_cmd {
	IR_DAY,
	IR_NIGHT,
};
struct sensor_format_struct {
	u32 mbus_code;
	unsigned int width;
	unsigned int height;
	unsigned int offs_h;
	unsigned int offs_v;
	unsigned int mipi_bps;
	unsigned int hoffset;
	unsigned int voffset;
	unsigned int hts;
	unsigned int vts;
	unsigned int pclk;
	unsigned int fps_fixed;
	unsigned int bin_factor;
	unsigned int intg_min;
	unsigned int intg_max;
	unsigned int intg_mid_min;
	unsigned int intg_mid_max;
	unsigned int intg_short_min;
	unsigned int intg_short_max;
	unsigned int gain_min;
	unsigned int gain_max;
	unsigned int wdr_mode;
	enum v4l2_field field;

	void *regs; /* Regs to tweak */
	int regs_size;
	void *switch_regs; /* Regs to tweak */
	int switch_regs_size;
};

#define DEV_DBG_EN   0
#if (DEV_DBG_EN == 1)
#define sensor_dbg(x, arg...) printk(KERN_DEBUG"[%s_debug]"x, SENSOR_NAME, ##arg)
#else
#define sensor_dbg(x, arg...)
#endif

#define sensor_err(x, arg...)  printk(KERN_ERR"[%s_err]"x, SENSOR_NAME, ##arg)
#define sensor_print(x, arg...) printk(KERN_DEBUG"[%s]"x, SENSOR_NAME, ##arg)

struct sensor_exp_gain;

struct sensor_fuc_core {
	int (*g_mbus_config)(int id, struct v4l2_mbus_config *cfg, struct mbus_framefmt_res *res);
	int (*sensor_test_i2c)(int id);
	int (*sensor_power)(int id, int on);
	int (*s_ir_status)(int id, int status);
	int (*s_stream)(int id, int enable);
	int (*s_switch)(int id);
	int (*s_exp_gain)(int id, struct sensor_exp_gain *exp_gain);
	struct sensor_format_struct *(*sensor_g_format)(int id);
	struct sensor_format_struct *(*sensor_g_switch_format)(int id);
};

extern struct sensor_fuc_core gc4663_core;
extern struct sensor_fuc_core gc2053_core;
extern struct sensor_fuc_core sc2355_core;
extern struct sensor_fuc_core sc5336_core;

#endif /*__CAMERA__H__*/
