/*
 * linux-4.9/drivers/media/platform/sunxi-vin/vin-isp/vin_link_server.h
 *
 * Copyright (c) 2007-2020 Allwinnertech Co., Ltd.
 *
 * Authors:  Zheng ZeQun <zeuqnzheng@allwinnertech.com>
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

#ifndef _VIN_LINK_SERVER_H_
#define _VIN_LINK_SERVER_H_

struct isp_part_para {
	int ir_mode;
	int flicker_mode;
};

struct isp_dev;

void isp_free_reg_tbl(struct isp_dev *isp);
void isp_set_load_reg(struct isp_dev *isp);
void isp_sensor_set_exp_gain(struct isp_dev *isp);
int isp_sensor_set_ir_status(struct isp_dev *isp);
int isp_sensor_otp_exit(struct isp_dev *isp);
int vin_server_init(struct isp_dev *isp, struct isp_part_para isp_part);
void vin_server_reset(struct isp_dev *isp, int ir_mode);
void vin_server_update(struct isp_dev *isp, struct isp_part_para isp_part);
int vin_isp_get_lv(struct isp_dev *isp);
void vin_isp_set_ae_lv(struct isp_dev *isp, unsigned int ev_lv);
int vin_isp_get_exp_gain(struct isp_dev *isp, struct sensor_exp_gain *exp_gain);
int vin_isp_get_hist(struct isp_dev *isp, unsigned int *hist);
int vin_isp_ae_done(struct isp_dev *isp, int drap_frame_en);

#endif /*_VIN_LINK_SERVER_H_*/

