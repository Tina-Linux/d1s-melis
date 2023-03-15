/*
 * linux-4.9/drivers/media/platform/sunxi-vin/vin-isp/isp_server/include/isp_rolloff.h
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

#ifndef _ISP_3A_ROLLOFF_H_
#define _ISP_3A_ROLLOFF_H_

#define ISP_LIB_USE_ROLLOFF	0

typedef enum isp_rolloff_param_type {
	ISP_ROLLOFF_INI_DATA,
	ISP_ROLLOFF_PARAM_TYPE_MAX,
} rolloff_param_type_t;
typedef struct isp_rolloff_ini_cfg {
	int rolloff_ratio;
	unsigned short lens_table_ini[ISP_LENS_TBL_SIZE*3];
} rolloff_ini_cfg_t;

typedef struct isp_rolloff_param {
	rolloff_param_type_t type;
	int isp_platform_id;
	int rolloff_frame_id;
	isp_sensor_info_t rolloff_sensor_info;
	struct isp_lsc_config lsc_cfg;
	rolloff_ini_cfg_t rolloff_ini;
} rolloff_param_t;

typedef struct isp_rolloff_stats {
	struct isp_stats_s *rolloff_stats;
} rolloff_stats_t;

typedef struct isp_rolloff_result {
	unsigned short lens_table_output[ISP_LENS_TBL_SIZE*3];
} rolloff_result_t;

typedef struct isp_rolloff_core_ops {
	HW_S32 (*isp_rolloff_set_params)(void *rolloff_core_obj, rolloff_param_t *param, rolloff_result_t *result);
	HW_S32 (*isp_rolloff_get_params)(void *rolloff_core_obj, rolloff_param_t **param);
	HW_S32 (*isp_rolloff_run)(void *rolloff_core_obj, rolloff_stats_t *stats, rolloff_result_t *result);
} isp_rolloff_core_ops_t;

void* rolloff_init(isp_rolloff_core_ops_t **rolloff_core_ops);
void  rolloff_exit(void *rolloff_core_obj);

#endif /*_ISP_3A_ROLLOFF_H_*/

