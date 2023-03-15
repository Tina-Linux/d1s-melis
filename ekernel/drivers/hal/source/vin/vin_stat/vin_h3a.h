/*
 * linux-4.9/drivers/media/platform/sunxi-vin/vin-stat/vin_h3a.h
 *
 * Copyright (c) 2007-2017 Allwinnertech Co., Ltd.
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

#ifndef _VIN_3A_H_
#define _VIN_3A_H_

#include <sunxi_hal_common.h>
#include <hal_cache.h>

#define STAT_MAX_BUFS		2

#define STAT_BUF_DONE		0	/* Buffer is ready */
#define STAT_NO_BUF		1	/* An error has occurred */

enum ispstat_buf_state_t {
	ISPSTAT_IDLE = 0,
	ISPSTAT_LOAD_SET,
	ISPSTAT_SAVE_SET,
	ISPSTAT_READY,
};

struct ispstat_buffer {
	void *phy_addr;
	u32 buf_size;
	u32 frame_number;
	u8 empty;
	enum ispstat_buf_state_t state;
	u8 id;
};

struct isp_dev;

struct isp_stat {
	struct isp_dev *isp;

	/* Buffer */
	bool stat_en_flag;
	u8 sensor_fps;
	u8 buf_cnt;
	u32 frame_number;
	u32 buf_size;
	struct ispstat_buffer buf[STAT_MAX_BUFS];
	struct ispstat_buffer *active_buf;
	struct ispstat_buffer *locked_buf;
};

int isp_stat_request_statistics(struct isp_stat *stat);
void isp_stat_isr(struct isp_stat *stat);
void isp_stat_load_set(struct isp_stat *stat);
int vin_isp_h3a_init(struct isp_dev *isp);
void vin_isp_h3a_cleanup(struct isp_dev *isp);

#endif
