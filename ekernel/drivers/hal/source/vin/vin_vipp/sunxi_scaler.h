
/*
 * sunxi_scaler.h for scaler and osd v4l2 subdev
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#ifndef _SUNXI_SCALER_H_
#define _SUNXI_SCALER_H_

#include <sunxi_hal_common.h>
#include <hal_atomic.h>

#include "vipp200/vipp200_reg.h"

#define VIPP_REG_MODE

enum vipp_work_mode {
	VIPP_ONLINE = 0,
	VIPP_OFFLINE = 1,
};

struct scaler_para {
	u32 xratio;
	u32 yratio;
	u32 w_shift;
	u32 width;
	u32 height;
};

struct vin_mmm {
	size_t size;
	void *phy_addr;
};

struct v4l2_rect {
	__s32   left;
	__s32   top;
	__u32   width;
	__u32   height;
};

struct scaler_dev {
	unsigned int is_empty;
	unsigned int id;
	unsigned long base;
	int stream_count;
	struct vin_mmm vipp_reg;
	struct {
		struct v4l2_rect active;
	} crop;
	struct scaler_para para;
	hal_spinlock_t slock;
	struct vin_mmm load_para[2];
	bool load_select; /*load_select = 0 select load_para[0], load_select = 1 select load_para[1]*/
	int irq;
	unsigned char is_irq_empty;
	unsigned int logic_top_stream_count;
	unsigned int work_mode;
	unsigned int noneed_register;
};

int scaler_probe(unsigned int id);
int scaler_remove(unsigned int id);
int sunxi_scaler_subdev_s_stream(unsigned int id, int enable);

#endif /*_SUNXI_SCALER_H_*/
