/*
 * linux-4.9/drivers/media/platform/sunxi-vin/vin-isp/isp_server/include/isp_cmd_intf.h
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

#ifndef _ISP_CMD_INTF_H_
#define _ISP_CMD_INTF_H_

void isp_s_brightness(struct isp_lib_context *isp_gen, int value);
void isp_s_contrast(struct isp_lib_context *isp_gen, int value);
void isp_s_saturation(struct isp_lib_context *isp_gen, int value);
void isp_s_hue(struct isp_lib_context *isp_gen, int value);
void isp_s_auto_white_balance(struct isp_lib_context *isp_gen, int value);
void isp_s_exposure(struct isp_lib_context *isp_gen, int value);
void isp_s_auto_gain(struct isp_lib_context *isp_gen, int value);
void isp_s_gain(struct isp_lib_context *isp_gen, int value);
void isp_s_power_line_frequency(struct isp_lib_context *isp_gen, int value);
void isp_s_white_balance_temperature(struct isp_lib_context *isp_gen, int value);
void isp_s_sharpness(struct isp_lib_context *isp_gen, int value);
void isp_s_auto_brightness(struct isp_lib_context *isp_gen,	int value);
void isp_s_band_stop_filter(struct isp_lib_context *isp_gen, int value);
void isp_s_illuminators_1(struct isp_lib_context *isp_gen, int value);
void isp_s_illuminators_2(struct isp_lib_context *isp_gen, int value);
void isp_s_af_metering_mode(struct isp_lib_context *isp_gen, struct v4l2_win_setting *win);
void isp_s_flash_mode(struct isp_lib_context *isp_gen, int value);
void isp_s_flash_mode_v1(struct isp_lib_context *isp_gen, int value);
void isp_s_ae_metering_mode(struct isp_lib_context *isp_gen, int value);
void isp_s_ae_roi(struct isp_lib_context *isp_gen, int value, struct isp_h3a_coor_win *coor);
void isp_s_light_mode(struct isp_lib_context *isp_gen, int value);
void isp_s_exposure_auto(struct isp_lib_context *isp_gen, int value);
void isp_s_exposure_absolute(struct isp_lib_context *isp_gen, int value);
void isp_s_aperture(struct isp_lib_context *isp_gen, int value);
void isp_s_focus_absolute(struct isp_lib_context *isp_gen, int value);
void isp_s_focus_relative(struct isp_lib_context *isp_gen, int value);
void isp_s_focus_auto(struct isp_lib_context *isp_gen, int value);
void isp_s_auto_exposure_bias(struct isp_lib_context *isp_gen, int value);
void isp_s_auto_n_preset_white_balance(struct isp_lib_context *isp_gen, int value);
void isp_s_iso_sensitivity(struct isp_lib_context *isp_gen, int value);
void isp_s_iso_sensitivity_auto(struct isp_lib_context *isp_gen, int value);
void isp_s_scene_mode(struct isp_lib_context *isp_gen, int value);
void isp_s_3a_lock(struct isp_lib_context *isp_gen, int value);
void isp_s_auto_focus_start(struct isp_lib_context *isp_gen,	int value);
void isp_s_auto_focus_stop(struct isp_lib_context *isp_gen,	int value);
void isp_s_auto_focus_status(struct isp_lib_context *isp_gen, int value);
void isp_s_auto_focus_range(struct isp_lib_context *isp_gen, int value);
void isp_s_take_picture(struct isp_lib_context *isp_gen, int value);
void isp_s_r_gain(struct isp_lib_context *isp_gen, int value);
void isp_s_g_gain(struct isp_lib_context *isp_gen, int value);
void isp_s_b_gain(struct isp_lib_context *isp_gen, int value);
int isp_set_ae_ev_idx(struct isp_lib_context *isp_gen, int curr_idx);
int isp_set_ae_tbl(struct isp_lib_context *isp_gen, int *rp_data);
int isp_set_ae_roi_target(struct isp_lib_context *isp_gen, int *rp_data);
#endif /*_ISP_CMD_INTF_H_*/

