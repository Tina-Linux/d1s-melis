/*
 * linux-4.9/drivers/media/platform/sunxi-vin/vin-tdm/vin_tdm.h
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 *
 * Authors:  Zheng Zequn <zequnzheng@allwinnertech.com>
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

#ifndef _VIN_TDM_H_
#define _VIN_TDM_H_

#include <sunxi_hal_common.h>
#include <hal_atomic.h>

#include "../platform/platform_cfg.h"
#include "tdm200/tdm200_reg.h"
//#include "../vin_video/vin_core.h"

/* #define TDM_USE_VGM */

#define TDM_BUFS_NUM 6
#define TDM_TX_HBLANK 256
#define TDM_TX_HBLANK_OFFLINE 128
#define TDM_TX_VBLANK 36

struct tdm_buffer {
	void *dma_addr;
	u32 buf_size;
	u8 empty;
};

enum input_bit_width {
	RAW_8BIT = 8,
	RAW_10BIT = 10,
	RAW_12BIT = 12,
};

struct tdm_format {
	u32 code;
	enum input_bit_width input_bit_width;
	enum input_image_type_sel input_type;
	enum tdm_input_fmt raw_fmt;
};

struct tdm_work_status {
	bool wdr_work;
	bool speed_dn_en;
	bool tdm_en;
	enum  rx_chn_cfg_mode rx_chn_mode;
	enum  tx_chn_cfg_mode tx_chn_mode;
	unsigned int chn_total;
};

struct rx_work_status {
	u8 wdr_mode;
	bool tx_func_en;
	bool lbc_en;
	bool pkg_en;
	bool sync_en;
	bool line_num_ddr_en;
	u16 data_fifo_depth;
	u16 head_fifo_depth;
	u8 tx_rate_numerator;
	u8 tx_rate_denominator;
};

struct tdm_rx_dev {
	unsigned char id;
	struct tdm_format *tdm_fmt;
	struct mbus_framefmt_res *res;
	u32 stream_cnt;
	u32 width;
	u32 height;
	u32 sensor_fps;
	u32 isp_clk;
	int stream_count;

	bool streaming;
	struct rx_work_status ws;
	struct tdm_rx_lbc lbc;

	/* Buffer */
	u32 buf_size;
	u8 buf_cnt;
	//struct vin_mm ion_man[TDM_BUFS_NUM]; /* for ion alloc/free manage */
	struct tdm_buffer buf[TDM_BUFS_NUM];
};

struct tdm_dev {
	unsigned long base;
	unsigned char id;
	int irq;
	char is_empty;
	hal_spinlock_t slock;

	struct tdm_rx_dev tdm_rx[TDM_RX_NUM];
	bool tdm_rx_buf_en[TDM_RX_NUM];
	bool tdm_rx_reset[TDM_RX_NUM];

	/* Control */
	bool configured;
	u32 stream_cnt;

	struct tdm_work_status ws;
	struct tdm_tx_cfg tx_cfg;
	u32 work_mode;
	u32 bitmap_chn_use;
	u32 rx_stream_cnt[TDM_RX_NUM];

};

int sunxi_tdm_subdev_s_stream(unsigned int id, int vinc_id, int enable);
int sunxi_tdm_s_mbus_config(unsigned int id, const struct mbus_framefmt_res *res);
int tdm_probe(unsigned int id);
int tdm_remove(unsigned int id);

#endif /*_VIN_TDM_H_*/

