/*
 * linux-4.9/drivers/media/platform/sunxi-vin/vin-isp/isp_server/isp_cfg/isp_ini_parse.c
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

#include "../include/isp_ini_parse.h"
#include "../include/isp_manage.h"
#include "../include/isp_debug.h"

#if (ISP_VERSION >= 600)
#if defined CONFIG_SENSOR_GC2053_MIPI || defined CONFIG_SENSOR_GC4663_MIPI || defined CONFIG_SENSOR_SC5336_MIPI
#ifdef CONFIG_SENSOR_GC4663_MIPI
#include "SENSOR_H/gc4663_mipi_default_ini_v853.h"
#include "SENSOR_H/gc4663_120fps_mipi_default_ini_v853.h"
#include "SENSOR_H/gc4663_120fps_mipi_linear_to_wdr_ini_v853.h"
//#include "SENSOR_H/gc4663_mipi_wdr_default_v853.h"
#include "SENSOR_H/gc4663_mipi_wdr_auto_ratio_v853.h"
#include "SENSOR_REG_H/gc4663_mipi_120fps_720p_day_reg.h"
#include "SENSOR_REG_H/gc4663_mipi_2560_1440_15fps_day_reg.h"
//#include "SENSOR_REG_H/gc4663_mipi_2560_1440_wdr_15fps_day_reg.h"
#include "SENSOR_REG_H/gc4663_mipi_2560_1440_wdr_auto_ratio_15fps_day_reg.h"
#endif // CONFIG_SENSOR_GC4663_MIPI

#ifdef CONFIG_SENSOR_GC2053_MIPI
#include "SENSOR_H/gc2053_120fps_mipi_default_ini_v853.h"
#include "SENSOR_H/gc2053_mipi_default_ini_v853.h"
#include "SENSOR_REG_H/gc2053_mipi_120fps_480p_day_reg.h"
#include "SENSOR_REG_H/gc2053_mipi_1080p_day_reg.h"
#endif //CONFIG_SENSOR_GC2053_MIPI

#ifdef CONFIG_SENSOR_SC5336_MIPI
#include "SENSOR_H/sc5336_mipi_default_ini_v853.h"
#include "SENSOR_H/sc5336_130fps_mipi_default_ini_v853.h"
#include "SENSOR_REG_H/sc5336_mipi_2880_1620_day_reg.h"
#include "SENSOR_REG_H/sc5336_mipi_130fps_1440_400_day_reg.h"
#endif // CONFIG_SENSOR_SC5336_MIPI
#else
#include "SENSOR_H/gc2053_mipi_default_ini_v853.h"
#endif
#endif //(ISP_VERSION >= 600)

unsigned int isp_cfg_log_param = ISP_LOG_CFG;

#define SIZE_OF_LSC_TBL     (12*768*2)
#define SIZE_OF_GAMMA_TBL   (5*1024*3*2)

struct isp_cfg_array cfg_arr[] = {
#if (ISP_VERSION >= 600)
#if defined CONFIG_SENSOR_GC2053_MIPI || defined CONFIG_SENSOR_GC4663_MIPI || defined CONFIG_SENSOR_SC5336_MIPI
#ifdef CONFIG_SENSOR_GC2053_MIPI
	{"gc2053_mipi", "gc2053_mipi_default_ini_v853_day", 1920, 1088, 20, 0, 0, &gc2053_mipi_v853_isp_cfg},
	{"gc2053_mipi", "gc2053_mipi_default_ini_v853_night", 1920, 1088, 20, 0, 1, &gc2053_mipi_v853_isp_cfg},
	{"gc2053_mipi", "gc2053_120fps_mipi_default_ini_v853_day", 640, 480, 120, 0, 0, &gc2053_mipi_120fps_v853_isp_cfg},
	{"gc2053_mipi", "gc2053_120fps_mipi_default_ini_v853_night", 640, 480, 120, 0, 1, &gc2053_mipi_120fps_v853_isp_cfg},
#endif //CONFIG_SENSOR_GC2053_MIPI
#ifdef CONFIG_SENSOR_GC4663_MIPI
	{"gc4663_mipi", "gc4663_mipi_default_ini_day", 2560, 1440, 15, 0, 0, &gc4663_mipi_v853_isp_cfg},
	{"gc4663_mipi", "gc4663_mipi_default_ini_night", 2560, 1440, 15, 0, 1, &gc4663_mipi_v853_isp_cfg},
	{"gc4663_mipi", "gc4663_mipi_wdr_v853_isp_cfg_day", 2560, 1440, 15, 1, 0, &gc4663_mipi_wdr_v853_isp_cfg},
	{"gc4663_mipi", "gc4663_mipi_wdr_v853_isp_cfg_night", 2560, 1440, 15, 1, 1, &gc4663_mipi_wdr_v853_isp_cfg},
	{"gc4663_mipi", "gc4663_120fps_mipi_default_ini_day", 1280, 720, 120, 0, 0, &gc4663_120fps_mipi_v853_isp_cfg},
	{"gc4663_mipi", "gc4663_120fps_mipi_default_ini_night", 1280, 720, 120, 0, 1, &gc4663_120fps_mipi_v853_isp_cfg},
	{"gc4663_mipi", "gc4663_120fps_linear_to_wdr_day", 1280, 720, 120, 1, 0, &gc4663_120fps_mipi_linear_to_wdr_v853_isp_cfg},
	{"gc4663_mipi", "gc4663_120fps_linear_to_wdr_night", 1280, 720, 120, 1, 1, &gc4663_120fps_mipi_linear_to_wdr_v853_isp_cfg},
#endif // CONFIG_SENSOR_GC4663_MIPI
#ifdef CONFIG_SENSOR_SC5336_MIPI
	{"sc5336_mipi", "sc5336_130fps_mipi_default_ini_day", 1440, 400, 130, 0, 0, &sc5336_mipi_130fps_isp_cfg},
	{"sc5336_mipi", "sc5336_130fps_mipi_default_ini_night", 1440, 400, 130, 0, 1, &sc5336_mipi_130fps_isp_cfg},
	{"sc5336_mipi", "sc5336_mipi_default_ini_day", 2880, 1620, 20, 0, 0, &sc5336_mipi_isp_cfg},
	{"sc5336_mipi", "sc5336_mipi_default_ini_night", 2880, 1620, 20, 0, 1, &sc5336_mipi_isp_cfg},
#endif // CONFIG_SENSOR_SC5336_MIPI
#else
	{"gc2053_mipi", "gc2053_mipi_default_ini_v853", 1920, 1088, 20, 0, 0, &gc2053_mipi_v853_isp_cfg},
#endif
#endif //(ISP_VERSION >= 600)
};

int parser_ini_info(struct isp_param_config *param, char *sensor_name,
			int w, int h, int fps, int wdr, int ir, int sync_mode, int isp_id)
{
	int i;
	struct isp_cfg_pt *cfg = NULL;

	//load header parameter
	for (i = 0; i < ARRAY_SIZE(cfg_arr); i++) {
		if (!strncmp(sensor_name, cfg_arr[i].sensor_name, 6) &&
		    (w == cfg_arr[i].width) && (h == cfg_arr[i].height) &&
		    (fps == cfg_arr[i].fps) && (wdr == cfg_arr[i].wdr) &&
		    (ir == cfg_arr[i].ir)) {
				cfg = cfg_arr[i].cfg;
				ISP_PRINT("find %s_%d_%d_%d_%d [%s] isp config\n", cfg_arr[i].sensor_name,
					cfg_arr[i].width, cfg_arr[i].height, cfg_arr[i].fps, cfg_arr[i].wdr, cfg_arr[i].isp_cfg_name);
				break;
		}
	}

	if (i == ARRAY_SIZE(cfg_arr)) {
		for (i = 0; i < ARRAY_SIZE(cfg_arr); i++) {
			if (!strncmp(sensor_name, cfg_arr[i].sensor_name, 6) && (wdr == cfg_arr[i].wdr)) {
				cfg = cfg_arr[i].cfg;
				ISP_WARN("cannot find %s_%d_%d_%d_%d_%d isp config, use %s_%d_%d_%d_%d_%d -> [%s]\n", sensor_name, w, h, fps, wdr, ir,
				         cfg_arr[i].sensor_name, cfg_arr[i].width, cfg_arr[i].height, cfg_arr[i].fps, cfg_arr[i].wdr,
				         cfg_arr[i].ir, cfg_arr[i].isp_cfg_name);
				break;
			}
		}
		if (i == ARRAY_SIZE(cfg_arr)) {
			for (i = 0; i < ARRAY_SIZE(cfg_arr); i++) {
				if (wdr == cfg_arr[i].wdr) {
					cfg = cfg_arr[i].cfg;
					ISP_WARN("cannot find %s_%d_%d_%d_%d_%d isp config, use %s_%d_%d_%d_%d_%d -> [%s]\n", sensor_name, w, h, fps, wdr, ir,
					         cfg_arr[i].sensor_name, cfg_arr[i].width, cfg_arr[i].height, cfg_arr[i].fps, cfg_arr[i].wdr,
					         cfg_arr[i].ir, cfg_arr[i].isp_cfg_name);
					break;
				}
			}
		}
		if (i == ARRAY_SIZE(cfg_arr)) {
			ISP_WARN("cannot find %s_%d_%d_%d_%d_%d isp config, use default config [%s]\n",
				sensor_name, w, h, fps, wdr, ir, cfg_arr[i-1].isp_cfg_name);
			cfg = cfg_arr[i-1].cfg;// use the last one
		}
	}

	if (cfg != NULL) {
		param->isp_test_settings = *cfg->isp_test_settings;
		param->isp_3a_settings = *cfg->isp_3a_settings;
		param->isp_iso_settings = *cfg->isp_iso_settings;
		param->isp_tunning_settings = *cfg->isp_tunning_settings;
	}

	return 0;
}

struct isp_reg_array reg_arr[] = {
#if (ISP_VERSION >= 600)
#ifdef CONFIG_SENSOR_GC2053_MIPI
	{"gc2053_mipi", "gc2053_mipi_120fps_480p_day_reg", 640, 480, 120, 0, 0, &gc2053_mipi_480p_isp_day_reg},
	{"gc2053_mipi", "gc2053_mipi_120fps_480p_night_reg", 640, 480, 120, 0, 1, &gc2053_mipi_480p_isp_day_reg},
	{"gc2053_mipi", "gc2053_mipi_1080p_20fps_day_reg_day", 1920, 1088, 20, 0, 0, &gc2053_mipi_isp_day_reg},
	{"gc2053_mipi", "gc2053_mipi_1080p_20fps_day_reg_night", 1920, 1088, 20, 0, 1, &gc2053_mipi_isp_day_reg},
#endif
#ifdef CONFIG_SENSOR_GC4663_MIPI
	{"gc4663_mipi", "gc4663_mipi_720p_120fps_day_reg", 1280, 720, 120, 0, 0, &gc4663_mipi_720p_isp_day_reg},
	{"gc4663_mipi", "gc4663_mipi_720p_120fps_night_reg", 1280, 720, 120, 0, 1, &gc4663_mipi_720p_isp_day_reg},
	{"gc4663_mipi", "gc4663_mipi_1440p_15fps_day_reg_day", 2560, 1440, 15, 0, 0, &gc4663_mipi_isp_day_reg},
	{"gc4663_mipi", "gc4663_mipi_1440p_15fps_day_reg_night", 2560, 1440, 15, 0, 1, &gc4663_mipi_isp_day_reg},
	{"gc4663_mipi", "gc4663_mipi_1440p_wdr_15fps_day_reg_day", 2560, 1440, 15, 1, 0, &gc4663_mipi_wdr_isp_day_reg},
	{"gc4663_mipi", "gc4663_mipi_1440p_wdr_15fps_day_reg_night", 2560, 1440, 15, 1, 1, &gc4663_mipi_wdr_isp_day_reg},
#endif
#ifdef CONFIG_SENSOR_SC5336_MIPI
	{"sc5336_mipi", "sc5336_mipi_130fps_1440_400_day_reg", 1440, 400, 130, 0, 0, &sc5336_mipi_1440_400_isp_day_reg},
	{"sc5336_mipi", "sc5336_mipi_130fps_1440_400_night_reg", 1440, 400, 130, 0, 1, &sc5336_mipi_1440_400_isp_day_reg},
	{"sc5336_mipi", "sc5336_mipi_2880_1620_day_reg_day", 2880, 1620, 20, 0, 0, &sc5336_mipi_isp_day_reg},
	{"sc5336_mipi", "sc5336_mipi_2880_1620_day_reg_night", 2880, 1620, 20, 0, 1, &sc5336_mipi_isp_day_reg},
#endif // CONFIG_SENSOR_SC5336_MIPI
#endif
};

int parser_ini_regs_info(struct isp_lib_context *ctx, char *sensor_name,
			int w, int h, int fps, int wdr, int ir)
{
	int i;
	struct isp_reg_pt *reg = NULL;

	for (i = 0; i < ARRAY_SIZE(reg_arr); i++) {
		if (!strncmp(sensor_name, reg_arr[i].sensor_name, 6) &&
			(w == reg_arr[i].width) && (h == reg_arr[i].height) &&// (fps == reg_arr[i].fps) &&
			(wdr == reg_arr[i].wdr)) {

			if (reg_arr[i].ir == ir) {
				reg = reg_arr[i].reg;
				ISP_PRINT("find %s_%d_%d_%d_%d ---- [%s] isp reg\n", reg_arr[i].sensor_name,
					reg_arr[i].width, reg_arr[i].height, reg_arr[i].fps, reg_arr[i].wdr, reg_arr[i].isp_cfg_name);
				break;
			}
		}
	}

	if (i == ARRAY_SIZE(reg_arr)) {
		ISP_WARN("cannot find %s_%d_%d_%d_%d_%d isp reg!!!\n", sensor_name, w, h, fps, wdr, ir);
		return -1;
	}

	if (reg != NULL) {
		if (reg->isp_save_reg)
			memcpy(ctx->load_reg_base, reg->isp_save_reg, ISP_LOAD_REG_SIZE);
		if (reg->isp_save_fe_table)
			memcpy(ctx->module_cfg.fe_table, reg->isp_save_fe_table, ISP_FE_TABLE_SIZE);
		if (reg->isp_save_bayer_table)
			memcpy(ctx->module_cfg.bayer_table, reg->isp_save_bayer_table, ISP_BAYER_TABLE_SIZE);
		if (reg->isp_save_rgb_table)
			memcpy(ctx->module_cfg.rgb_table, reg->isp_save_rgb_table, ISP_RGB_TABLE_SIZE);
		if (reg->isp_save_yuv_table)
			memcpy(ctx->module_cfg.yuv_table, reg->isp_save_yuv_table, ISP_YUV_TABLE_SIZE);
	}

	return 0;
}
