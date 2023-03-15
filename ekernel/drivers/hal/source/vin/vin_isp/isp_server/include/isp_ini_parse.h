/*
 * linux-4.9/drivers/media/platform/sunxi-vin/vin-isp/isp_server/include/isp_ini_parse.h
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

#ifndef _ISP_INI_PARSE_H_
#define _ISP_INI_PARSE_H_
#include "isp_manage.h"
#include "isp_debug.h"

struct isp_cfg_pt {
	struct isp_test_param *isp_test_settings;
	struct isp_3a_param *isp_3a_settings;
	struct isp_tunning_param *isp_tunning_settings;
	struct isp_dynamic_param *isp_iso_settings;
};

struct isp_cfg_array {
	char sensor_name[20];
	char isp_cfg_name[50];
	int width;
	int height;
	int fps;
	int wdr;
	int ir;
	struct isp_cfg_pt *cfg;
};

struct isp_reg_pt {
	unsigned int *isp_save_reg;
	unsigned int *isp_save_fe_table;
	unsigned int *isp_save_bayer_table;
	unsigned int *isp_save_rgb_table;
	unsigned int *isp_save_yuv_table;
};

struct isp_reg_array {
	char sensor_name[20];
	char isp_cfg_name[50];
	int width;
	int height;
	int fps;
	int wdr;
	int ir;
	struct isp_reg_pt *reg;
};

int parser_sync_info(struct isp_param_config *param, char *isp_cfg_name, int isp_id);
int parser_ini_info(struct isp_param_config *param, char *sensor_name,
			int w, int h, int fps, int wdr, int ir, int sync_mode, int isp_id);
int parser_ini_regs_info(struct isp_lib_context *ctx, char *sensor_name,
			int w, int h, int fps, int wdr, int ir);
#endif	/*_ISP_INI_PARSE_H_*/

