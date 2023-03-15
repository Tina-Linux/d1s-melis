/*
 * linux-4.9/drivers/media/platform/sunxi-vin/vin-isp/isp_server/include/isp_base.h
 *
 * Copyright (c) 2007-2022 Allwinnertech Co., Ltd.
 *
 * Authors:  Yang Feng <yangfeng@allwinnertech.com>
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

#ifndef _ISP_BASE_H_
#define _ISP_BASE_H_

struct isp_lib_context;
void config_wdr_split(struct isp_lib_context *isp_gen);
void config_fe_channel(struct isp_lib_context *isp_gen);
void config_dig_gain(struct isp_lib_context *isp_gen, unsigned int exp_digital_gain, unsigned int exp_digital_gain_s);
void config_wdr(struct isp_lib_context *isp_gen, int flag);
void config_gamma(struct isp_lib_context *isp_gen) ;
void config_lens_table(struct isp_lib_context * isp_gen, int vcm_std_pos);
void config_msc_table(struct isp_lib_context * isp_gen, int vcm_std_pos, int comp);
void config_nr_msc_table(struct isp_lib_context * isp_gen, int flag);
void config_lens_center(struct isp_lib_context * isp_gen);
void config_band_step(struct isp_lib_context * isp_gen);
void isp_handle_stats(struct isp_lib_context *isp_gen, const void *buffer);
void isp_handle_stats_sync(struct isp_lib_context *isp_gen, const void *buffer0, const void *buffer1);
void isp_apply_colormatrix(struct isp_lib_context *isp_gen);
void isp_apply_settings(struct isp_lib_context *isp_gen);
void gm_smooth(HW_U16* table, int size);

#endif /*_ISP_BASE_H_*/
