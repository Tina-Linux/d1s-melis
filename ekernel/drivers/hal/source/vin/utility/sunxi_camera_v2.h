/*
 * utility/sunxi_camera_v2.h -- Ctrl IDs definitions for sunxi-vin
 *
 * Copyright (C) 2014 Allwinnertech Co., Ltd.
 * Copyright (C) 2015 Yang Feng
 *
 * Author: Yang Feng <yangfeng@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */
#ifndef _SUNXI_CAMERA_H_
#define _SUNXI_CAMERA_H_

#include <sunxi_hal_common.h>
#include <melis/typedef.h>

/*  Flags for 'capability' and 'capturemode' fields */
#define V4L2_MODE_HIGHQUALITY		0x0001
#define V4L2_MODE_VIDEO			0x0002
#define V4L2_MODE_IMAGE			0x0003
#define V4L2_MODE_PREVIEW		0x0004

/*  for yuv420 FBC mode*/
#define V4L2_PIX_FMT_FBC           v4l2_fourcc('F', 'C', '2', '1')
#define V4L2_PIX_FMT_LBC_2_0X      v4l2_fourcc('L', 'C', '2', '1')
#define V4L2_PIX_FMT_LBC_2_5X      v4l2_fourcc('L', 'C', '2', '2')
#define V4L2_PIX_FMT_LBC_1_0X      v4l2_fourcc('L', 'C', '2', '3')
#define V4L2_PIX_FMT_LBC_1_5X  	   v4l2_fourcc('L', 'C', '2', '4')

struct csi_sync_ctrl {
	__s32 type;
	__s32 prs_sync_en;
	__s32 prs_sync_scr_sel;
	__s32 prs_sync_bench_sel;
	__s32 prs_sync_input_vsync_en;
	__s32 prs_sync_singal_via_by;
	__s32 prs_sync_singal_scr_sel;
	__s32 prs_sync_pulse_cfg;
	__s32 prs_sync_dist;
	__s32 prs_sync_wait_n;
	__s32 prs_sync_wait_m;
	__s32 dma_clr_dist;

	__s32 prs_xvs_out_en;
	__s32 prs_xhs_out_en;
	__s32 prs_xvs_t;
	__s32 prs_xhs_t;
	__s32 prs_xvs_len;
	__s32 prs_xhs_len;
};

struct isp_debug_mode {
	__u32 debug_en;
	__u32 debug_sel;
};

struct sensor_exp_gain {
	int exp_val;
	int exp_mid_val;
	int exp_short_val;
	int gain_val;
	int gain_mid_val;
	int gain_short_val;
	int r_gain;
	int b_gain;
};

struct v4l2_win_coordinate {
	__s32 x1;
	__s32 y1;
	__s32 x2;
	__s32 y2;
};

struct v4l2_win_setting {
	__s32 metering_mode;
	struct v4l2_win_coordinate coor;
};

#endif /*_SUNXI_CAMERA_H_*/

