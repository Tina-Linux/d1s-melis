/*
 * mipi subdev driver module
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _SUNXI_MIPI__H_
#define _SUNXI_MIPI__H_

#include "combo_common.h"
#include "combo_csi/combo_csi_reg.h"
#include "../platform/platform_cfg.h"

struct mipi_dev {
	unsigned int id;
	unsigned long base;
	unsigned long phy_base;
	unsigned long port_base;
	char if_name[20];
	unsigned int if_type;
	unsigned int cmb_mode;
	int top_stream_count;
	int stream_count;
	unsigned char phy_offset;
	unsigned char time_hs;
	struct combo_csi_cfg cmb_csi_cfg;
	struct mbus_framefmt_res *res;
};

int mipi_probe(unsigned int id);
int mipi_remove(unsigned int id);
int sunxi_mipi_s_mbus_config(unsigned int id, const struct v4l2_mbus_config *cfg, const struct mbus_framefmt_res *res);
int sunxi_mipi_subdev_s_stream(unsigned int id, int enable);

#endif /*_SUNXI_MIPI__H_*/
