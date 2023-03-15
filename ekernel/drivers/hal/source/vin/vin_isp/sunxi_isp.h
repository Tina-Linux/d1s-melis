/*
 * lsunxi_isp.h
 *
 * Copyright (c) 2007-2017 Allwinnertech Co., Ltd.
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef _SUNXI_ISP_H_
#define _SUNXI_ISP_H_

#include <hal_workqueue.h>
#include <hal_atomic.h>

#include "../utility/sunxi_camera_v2.h"
#include "../vin_video/vin_core.h"
#include "../vin_stat/vin_h3a.h"
#if defined CONFIG_ARCH_SUN20IW3
#include "isp600/isp600_reg_cfg.h"
#endif

#include "vin_link_server.h"
#include "isp_server/isp_server.h"

#define ISP_SERVER_FASTINIT

enum isp_pad {
	ISP_PAD_SINK,
	ISP_PAD_SOURCE_ST,
	ISP_PAD_SOURCE,
	ISP_PAD_NUM,
};

enum isp_work_mode {
	ISP_ONLINE = 0,
	ISP_OFFLINE = 1,
};

enum isp_sram_boot_mode {
	SRAM_NORMAL_MODE = 0,
	SRAM_BOOT_MODE = 1,
};

enum isp_bit_width {
	RAW_8     = 8,
	RAW_10    = 10,
	RAW_12    = 12,
	RAW_14    = 14,
	RAW_16    = 16,
	RAW_18    = 18,
	RAW_20    = 20,
	RAW_22    = 22,
	RAW_24    = 24,
};

struct isp_pix_fmt {
	u32 mbus_code;
	enum isp_input_seq infmt;
	u32 fourcc;
	enum isp_bit_width input_bit;
};

struct vin_mm {
	size_t size;
	void *phy_addr;
};

struct isp_dev {
	int capture_mode;
	int use_isp;
	int irq;
	unsigned int is_empty;
	unsigned int load_flag;
	unsigned int f1_after_librun;/*fisrt frame after server run*/
	unsigned int have_init;
	unsigned int wdr_mode;
	unsigned int id;
	unsigned long base;
	unsigned long syscfg_base;
	int stream_count;
	unsigned long isp_frame_number;
	struct vin_mm isp_stat;
	struct vin_mm isp_load;
	struct vin_mm isp_save;
	struct isp_size err_size;
	unsigned int hb_max;
	unsigned int hb_min;
	struct isp_debug_mode isp_dbg;
	struct isp_pix_fmt *isp_fmt;
	struct isp_size_settings isp_ob;
	char load_shadow[ISP_LOAD_DRAM_SIZE];
	struct vin_mm d3d_pingpong[4];
	struct isp_lbc_cfg d3d_lbc;
	struct vin_mm isp_save_load;
	struct vin_mm load_para[2];
	unsigned int logic_top_stream_count;
	unsigned int work_mode;
	char save_get_flag;
	bool load_select; /*load_select = 0 select load_para[0], load_select = 1 select load_para[1]*/
	struct mbus_framefmt_res *res;
	hal_spinlock_t slock;
	unsigned int event_lost_cnt;

	bool save_stat_end;
	bool load_stat_end;
	struct isp_stat h3a_stat;
	unsigned char colorspace;
	struct vin_mm isp_stat_buf;
	struct vin_mm server_stat;
	bool isp_server_init;
	hal_workqueue *isp_server_run_task;
	hal_work start_isp_server_work;
	struct hw_isp_device *hw_isp;
};

void sunxi_isp_reset_server(unsigned int id);
void sunxi_isp_update_server(unsigned int id);
int sunxi_isp_ae_done(unsigned int id, int drap_frame_en);
int sunxi_isp_s_mbus_config(unsigned int id, const struct mbus_framefmt_res *res);
int isp_probe(unsigned int id);
int isp_remove(unsigned int id);
int sunxi_isp_subdev_s_stream(unsigned int id, unsigned int vinc_id, int enable);

#endif /*_SUNXI_ISP_H_*/
