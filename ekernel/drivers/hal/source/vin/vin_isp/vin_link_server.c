/*
 * linux-4.9/drivers/media/platform/sunxi-vin/vin-isp/vin_link_server.c
 *
 * Copyright (c) 2007-2022 Allwinnertech Co., Ltd.
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
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <hal_mem.h>
#include <sunxi_hal_common.h>

#include "isp_server/isp_server.h"
#include "../vin.h"
#include "vin_link_server.h"

extern unsigned int isp_default_reg[ISP_LOAD_REG_SIZE>>2];
//extern unsigned int isp_bdnf_reg[4+17*3];

static int isp_config_sensor_info(struct isp_dev *isp)
{
	struct hw_isp_device *hw_isp = isp->hw_isp;
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);
	struct sensor_format_struct *sensor_format = NULL;
	struct vin_core *vinc = NULL;
	int sensor_id;
	int ret = 0;
	int i;

	if (ctx == NULL)
		return -1;

	for (i = 0; i < VIN_MAX_DEV; i++) {
		if (global_video[i].used == 0)
			continue;

		vinc = &global_video[i];
		if (vinc->isp_sel == isp->id)
			break;
	}
	if (vinc == NULL) {
		vin_err("%s:cannot find a real vinc to get sensor info\n", __func__);
		return -1;
	}

	sensor_id = vinc->mipi_sel;
	ctx->sensor_info.name = global_sensors[sensor_id].sensor_name;

	if (global_sensors[sensor_id].sensor_core->sensor_g_format)
		sensor_format = global_sensors[sensor_id].sensor_core->sensor_g_format(sensor_id);
	else
		return -1;

	ctx->sensor_info.sensor_width = sensor_format->width;
	ctx->sensor_info.sensor_height = sensor_format->height;
	ctx->sensor_info.fps_fixed = sensor_format->fps_fixed;
	ctx->sensor_info.wdr_mode = sensor_format->wdr_mode;
	// BT601_FULL_RANGE
	ctx->sensor_info.color_space = 1;

	switch (sensor_format->mbus_code) {
	case MEDIA_BUS_FMT_SBGGR8_1X8:
		ctx->sensor_info.bayer_bit = 8;
		ctx->sensor_info.input_seq = ISP_BGGR;
		break;
	case MEDIA_BUS_FMT_SBGGR10_1X10:
		ctx->sensor_info.bayer_bit = 10;
		ctx->sensor_info.input_seq = ISP_BGGR;
		break;
	case MEDIA_BUS_FMT_SBGGR12_1X12:
		ctx->sensor_info.bayer_bit = 12;
		ctx->sensor_info.input_seq = ISP_BGGR;
		break;
	case MEDIA_BUS_FMT_SGBRG8_1X8:
		ctx->sensor_info.bayer_bit = 8;
		ctx->sensor_info.input_seq = ISP_GBRG;
		break;
	case MEDIA_BUS_FMT_SGBRG10_1X10:
		ctx->sensor_info.bayer_bit = 10;
		ctx->sensor_info.input_seq = ISP_GBRG;
		break;
	case MEDIA_BUS_FMT_SGBRG12_1X12:
		ctx->sensor_info.bayer_bit = 12;
		ctx->sensor_info.input_seq = ISP_GBRG;
		break;
	case MEDIA_BUS_FMT_SGRBG8_1X8:
		ctx->sensor_info.bayer_bit = 8;
		ctx->sensor_info.input_seq = ISP_GRBG;
		break;
	case MEDIA_BUS_FMT_SGRBG10_1X10:
		ctx->sensor_info.bayer_bit = 10;
		ctx->sensor_info.input_seq = ISP_GRBG;
		break;
	case MEDIA_BUS_FMT_SGRBG12_1X12:
		ctx->sensor_info.bayer_bit = 12;
		ctx->sensor_info.input_seq = ISP_GRBG;
		break;
	case MEDIA_BUS_FMT_SRGGB8_1X8:
		ctx->sensor_info.bayer_bit = 8;
		ctx->sensor_info.input_seq = ISP_RGGB;
		break;
	case MEDIA_BUS_FMT_SRGGB10_1X10:
		ctx->sensor_info.bayer_bit = 10;
		ctx->sensor_info.input_seq = ISP_RGGB;
		break;
	case MEDIA_BUS_FMT_SRGGB12_1X12:
		ctx->sensor_info.bayer_bit = 12;
		ctx->sensor_info.input_seq = ISP_RGGB;
		break;
	default:
		ctx->sensor_info.bayer_bit = 10;
		ctx->sensor_info.input_seq = ISP_BGGR;
		break;
	}

	if (sensor_format->hts && sensor_format->vts && sensor_format->pclk) {
		ctx->sensor_info.hts = sensor_format->hts;
		ctx->sensor_info.vts = sensor_format->vts;
		ctx->sensor_info.pclk = sensor_format->pclk;
		ctx->sensor_info.bin_factor = sensor_format->bin_factor;
		ctx->sensor_info.gain_min = sensor_format->gain_min;
		ctx->sensor_info.gain_max = sensor_format->gain_max;
		ctx->sensor_info.exp_min = sensor_format->intg_min;
		ctx->sensor_info.exp_max = sensor_format->intg_max;
		ctx->sensor_info.exp_mid_min = sensor_format->intg_mid_min;
		ctx->sensor_info.exp_mid_max = sensor_format->intg_mid_max;
		ctx->sensor_info.exp_short_min = sensor_format->intg_short_min;
		ctx->sensor_info.exp_short_max = sensor_format->intg_short_max;
		ctx->sensor_info.hoffset = sensor_format->hoffset;
		ctx->sensor_info.voffset = sensor_format->voffset;

	} else {
		ctx->sensor_info.hts = sensor_format->width;
		ctx->sensor_info.vts = sensor_format->height;
		ctx->sensor_info.pclk = sensor_format->width * sensor_format->height * 30;
		ctx->sensor_info.bin_factor = 1;
		ctx->sensor_info.gain_min = 16;
		ctx->sensor_info.gain_max = 255;
		ctx->sensor_info.exp_min = 16;
		ctx->sensor_info.exp_max = sensor_format->height * 16;
		ctx->sensor_info.exp_mid_min = 16;
		ctx->sensor_info.exp_mid_max = sensor_format->height;
		ctx->sensor_info.exp_short_min = 16;
		ctx->sensor_info.exp_short_max = sensor_format->height / 16;
		ctx->sensor_info.hoffset = 0;
		ctx->sensor_info.voffset = 0;
	}

	ctx->stat.pic_size.width = sensor_format->width;
	ctx->stat.pic_size.height = sensor_format->height;

	ctx->stats_ctx.pic_w = sensor_format->width;
	ctx->stats_ctx.pic_h = sensor_format->height;
#if 1
	// update otp infomation
	if (ctx->otp_enable == -1) {
		ISP_PRINT("otp disabled, msc use 1024\n");
		for (i = 0; i < 16*16*3; i++) {
			((unsigned short *)(ctx->pmsc_table))[i] = 1024;
		}
	}
#endif
	return 0;
}

static int isp_config_switch_sensor_info(struct isp_dev *isp)
{
	struct hw_isp_device *hw_isp = isp->hw_isp;
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);
	struct sensor_format_struct *sensor_format = NULL;
	struct vin_core *vinc = NULL;
	int sensor_id;
	int ret = 0;
	int i;

	if (ctx == NULL)
		return -1;

	for (i = 0; i < VIN_MAX_DEV; i++) {
		if (global_video[i].used == 0)
			continue;

		vinc = &global_video[i];
		if (vinc->isp_sel == isp->id)
			break;
	}
	if (vinc == NULL) {
		vin_err("%s:cannot find a real vinc to get sensor info\n", __func__);
		return -1;
	}

	sensor_id = vinc->mipi_sel;
	ctx->sensor_info.name = global_sensors[sensor_id].sensor_name;

	if (global_sensors[sensor_id].sensor_core->sensor_g_switch_format)
		sensor_format = global_sensors[sensor_id].sensor_core->sensor_g_switch_format(sensor_id);
	else
		return -1;

	ctx->sensor_info.sensor_width = sensor_format->width;
	ctx->sensor_info.sensor_height = sensor_format->height;
	ctx->sensor_info.fps_fixed = sensor_format->fps_fixed;
	ctx->sensor_info.wdr_mode = sensor_format->wdr_mode;
	// BT601_FULL_RANGE
	ctx->sensor_info.color_space = 1;

	switch (sensor_format->mbus_code) {
	case MEDIA_BUS_FMT_SBGGR8_1X8:
		ctx->sensor_info.bayer_bit = 8;
		ctx->sensor_info.input_seq = ISP_BGGR;
		break;
	case MEDIA_BUS_FMT_SBGGR10_1X10:
		ctx->sensor_info.bayer_bit = 10;
		ctx->sensor_info.input_seq = ISP_BGGR;
		break;
	case MEDIA_BUS_FMT_SBGGR12_1X12:
		ctx->sensor_info.bayer_bit = 12;
		ctx->sensor_info.input_seq = ISP_BGGR;
		break;
	case MEDIA_BUS_FMT_SGBRG8_1X8:
		ctx->sensor_info.bayer_bit = 8;
		ctx->sensor_info.input_seq = ISP_GBRG;
		break;
	case MEDIA_BUS_FMT_SGBRG10_1X10:
		ctx->sensor_info.bayer_bit = 10;
		ctx->sensor_info.input_seq = ISP_GBRG;
		break;
	case MEDIA_BUS_FMT_SGBRG12_1X12:
		ctx->sensor_info.bayer_bit = 12;
		ctx->sensor_info.input_seq = ISP_GBRG;
		break;
	case MEDIA_BUS_FMT_SGRBG8_1X8:
		ctx->sensor_info.bayer_bit = 8;
		ctx->sensor_info.input_seq = ISP_GRBG;
		break;
	case MEDIA_BUS_FMT_SGRBG10_1X10:
		ctx->sensor_info.bayer_bit = 10;
		ctx->sensor_info.input_seq = ISP_GRBG;
		break;
	case MEDIA_BUS_FMT_SGRBG12_1X12:
		ctx->sensor_info.bayer_bit = 12;
		ctx->sensor_info.input_seq = ISP_GRBG;
		break;
	case MEDIA_BUS_FMT_SRGGB8_1X8:
		ctx->sensor_info.bayer_bit = 8;
		ctx->sensor_info.input_seq = ISP_RGGB;
		break;
	case MEDIA_BUS_FMT_SRGGB10_1X10:
		ctx->sensor_info.bayer_bit = 10;
		ctx->sensor_info.input_seq = ISP_RGGB;
		break;
	case MEDIA_BUS_FMT_SRGGB12_1X12:
		ctx->sensor_info.bayer_bit = 12;
		ctx->sensor_info.input_seq = ISP_RGGB;
		break;
	default:
		ctx->sensor_info.bayer_bit = 10;
		ctx->sensor_info.input_seq = ISP_BGGR;
		break;
	}

	if (sensor_format->hts && sensor_format->vts && sensor_format->pclk) {
		ctx->sensor_info.hts = sensor_format->hts;
		ctx->sensor_info.vts = sensor_format->vts;
		ctx->sensor_info.pclk = sensor_format->pclk;
		ctx->sensor_info.bin_factor = sensor_format->bin_factor;
		ctx->sensor_info.gain_min = sensor_format->gain_min;
		ctx->sensor_info.gain_max = sensor_format->gain_max;
		ctx->sensor_info.exp_min = sensor_format->intg_min;
		ctx->sensor_info.exp_max = sensor_format->intg_max;
		ctx->sensor_info.exp_mid_min = sensor_format->intg_mid_min;
		ctx->sensor_info.exp_mid_max = sensor_format->intg_mid_max;
		ctx->sensor_info.exp_short_min = sensor_format->intg_short_min;
		ctx->sensor_info.exp_short_max = sensor_format->intg_short_max;
		ctx->sensor_info.hoffset = sensor_format->hoffset;
		ctx->sensor_info.voffset = sensor_format->voffset;

	} else {
		ctx->sensor_info.hts = sensor_format->width;
		ctx->sensor_info.vts = sensor_format->height;
		ctx->sensor_info.pclk = sensor_format->width * sensor_format->height * 30;
		ctx->sensor_info.bin_factor = 1;
		ctx->sensor_info.gain_min = 16;
		ctx->sensor_info.gain_max = 255;
		ctx->sensor_info.exp_min = 16;
		ctx->sensor_info.exp_max = sensor_format->height * 16;
		ctx->sensor_info.exp_mid_min = 16;
		ctx->sensor_info.exp_mid_max = sensor_format->height;
		ctx->sensor_info.exp_short_min = 16;
		ctx->sensor_info.exp_short_max = sensor_format->height / 16;
		ctx->sensor_info.hoffset = 0;
		ctx->sensor_info.voffset = 0;
	}

	ctx->stat.pic_size.width = sensor_format->width;
	ctx->stat.pic_size.height = sensor_format->height;

	ctx->stats_ctx.pic_w = sensor_format->width;
	ctx->stats_ctx.pic_h = sensor_format->height;

	return 0;
}

static int isp_alloc_reg_tbl(struct isp_dev *isp)
{
	struct hw_isp_device *hw_isp = isp->hw_isp;
	struct isp_lib_context *isp_gen = isp_dev_get_ctx(hw_isp);

	isp->server_stat.size = ISP_LOAD_DRAM_SIZE;
	/* REG LOAD*/
	isp->server_stat.phy_addr = rt_memheap_alloc_align(&isp_mempool, isp->server_stat.size, 0x1000);
	if (isp->server_stat.phy_addr == NULL) {
		vin_err("%s:isp%d:alloc isp server_stat buffer error\n", __func__, isp->id);
		return -1;
	}
	isp_gen->load_reg_base = isp->server_stat.phy_addr;
	if (isp_gen->load_reg_base == NULL) {
		ISP_ERR("load_reg_base alloc failed, no memory!");
		return -1;
	}
	memset(isp_gen->load_reg_base, 0, ISP_LOAD_REG_SIZE);

	isp_gen->module_cfg.isp_dev_id = isp_gen->isp_id;
	//isp_gen->module_cfg.isp_platform_id = ISP_PLATFORM_SUN8IW21P1;

	isp_map_addr(&isp_gen->module_cfg, (unsigned long)isp_gen->load_reg_base);
#if (ISP_VERSION >= 600)
	/*FE TABLE*/
	isp_gen->module_cfg.fe_table = isp_gen->load_reg_base + ISP_LOAD_REG_SIZE;
	memset(isp_gen->module_cfg.fe_table, 0, ISP_FE_TABLE_SIZE);

	isp_gen->module_cfg.ch0_msc_table = isp_gen->module_cfg.fe_table + ISP_CH0_MSC_FE_MEM_OFS;
	isp_gen->module_cfg.ch1_msc_table = isp_gen->module_cfg.fe_table + ISP_CH1_MSC_FE_MEM_OFS;
	isp_gen->module_cfg.ch2_msc_table = isp_gen->module_cfg.fe_table + ISP_CH2_MSC_FE_MEM_OFS;

	/*BAYER TABLE*/
	isp_gen->module_cfg.bayer_table = isp_gen->module_cfg.fe_table + ISP_FE_TABLE_SIZE;
	memset(isp_gen->module_cfg.bayer_table, 0, ISP_BAYER_TABLE_SIZE);

	isp_gen->module_cfg.lens_table = isp_gen->module_cfg.bayer_table + ISP_LSC_MEM_OFS;
	isp_gen->module_cfg.msc_table = isp_gen->module_cfg.bayer_table + ISP_MSC_MEM_OFS;
	isp_gen->module_cfg.nr_msc_table = isp_gen->module_cfg.bayer_table + ISP_MSC_NR_MEM_OFS;
	isp_gen->module_cfg.tdnf_table = isp_gen->module_cfg.bayer_table + ISP_TDNF_DK_MEM_OFS;
	isp_gen->module_cfg.pltm_table = isp_gen->module_cfg.bayer_table + ISP_PLTM_TM_MEM_OFS;

	/*RGB TABLE*/
	isp_gen->module_cfg.rgb_table = isp_gen->module_cfg.bayer_table + ISP_BAYER_TABLE_SIZE;
	memset(isp_gen->module_cfg.rgb_table, 0, ISP_RGB_TABLE_SIZE);

	isp_gen->module_cfg.drc_table = isp_gen->module_cfg.rgb_table + ISP_DRC_MEM_OFS;
	isp_gen->module_cfg.gamma_table = isp_gen->module_cfg.rgb_table + ISP_GAMMA_MEM_OFS;

	/*YUV TABLE*/
	isp_gen->module_cfg.yuv_table = isp_gen->module_cfg.rgb_table + ISP_RGB_TABLE_SIZE;
	memset(isp_gen->module_cfg.yuv_table, 0, ISP_YUV_TABLE_SIZE);

	isp_gen->module_cfg.cem_table = isp_gen->module_cfg.yuv_table + ISP_CEM_MEM_OFS;
#endif
	return 0;
}

void isp_free_reg_tbl(struct isp_dev *isp)
{

}

#if 0
int isp_sensor_otp_init(struct isp_dev *isp)
{
	struct hw_isp_device *hw_isp = isp->hw_isp;
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);
	struct vin_md *vind = dev_get_drvdata(isp->subdev.v4l2_dev->dev);
	struct vin_core *vinc = NULL;
	int ret = 0;
	int i = 0;

	if (ctx == NULL)
		return -1;

	// shading
	isp->isp_opt.size = ISP_MSC_TBL_LENGTH * sizeof(unsigned short);
	os_mem_alloc(&isp->pdev->dev, &isp->isp_opt);
	ctx->pmsc_table = isp->isp_opt.vir_addr;
	memset(ctx->pmsc_table, 0, ISP_MSC_TBL_LENGTH*sizeof(unsigned short));

	if (!ctx->pmsc_table) {
		ISP_ERR("msc_table alloc failed, no memory!\n");
		return -1;
	}

	ctx->otp_enable = 1;

	for (i = 0; i < VIN_MAX_DEV; i++) {
		if (vind->vinc[i] == NULL)
			continue;
		if (!vin_busy(&vind->vinc[i]->vid_cap))
			continue;

		vinc = vind->vinc[i];
		if (vinc->isp_sel == isp->id)
			break;
	}
	if (vinc == NULL) {
		vin_err("%s:cannot find a real vinc to get sensor otp info\n", __func__);
		return -1;
	}

	ret = v4l2_subdev_call(vinc->vid_cap.pipe.sd[VIN_IND_SENSOR], core, ioctl,
				VIDIOC_VIN_GET_SENSOR_OTP_INFO, ctx->pmsc_table);
	if (ret) {
		ISP_ERR("VIDIOC_VIN_GET_SENSOR_OTP_INFO return error:%d\n", ret);
		ctx->otp_enable = -1;
	}

	//ctx->otp_enable = -1;
	ctx->pwb_table = (void *)((HW_U64)(ctx->pmsc_table) + 16*16*3*sizeof(unsigned short));

	return 0;
}

int isp_sensor_otp_exit(struct isp_dev *isp)
{
	struct hw_isp_device *hw_isp = isp->hw_isp;
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);

	if (ctx == NULL)
		return -1;

	// shading
	os_mem_free(&isp->pdev->dev, &isp->isp_opt);

	return 0;
}
#endif
void isp_set_load_reg(struct isp_dev *isp)
{
	struct hw_isp_device *hw_isp = isp->hw_isp;
	struct isp_lib_context *isp_gen = isp_dev_get_ctx(hw_isp);
	uint32_t __cpsr;

	if (!isp->use_isp)
		return;

	if (isp_gen == NULL)
		return;

	hal_mutex_lock(isp_gen->ctx_lock);
	if (isp_gen->isp_load_flag) {
		hal_dcache_invalidate((unsigned long)isp_gen->load_reg_base, ISP_LOAD_DRAM_SIZE);
		memcpy(&isp->load_shadow[0], isp_gen->load_reg_base, ISP_LOAD_DRAM_SIZE);
	}
	hal_mutex_unlock(isp_gen->ctx_lock);

	__cpsr = hal_spin_lock_irqsave(&isp->slock);
	isp->load_flag = 1;
	hal_spin_unlock_irqrestore(&isp->slock, __cpsr);
}

void isp_sensor_set_exp_gain(struct isp_dev *isp)
{
	struct hw_isp_device *hw_isp = isp->hw_isp;
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);
	struct sensor_exp_gain exp_gain;
	static struct sensor_exp_gain exp_gain_save[HW_ISP_DEVICE_NUM];
	ae_result_t *ae_result = NULL;
	awb_result_t *awb_result = NULL;
	struct vin_core *vinc = NULL;
	unsigned int sensor_id;
	int i;

	if (ctx == NULL)
		return;

	ae_result = &ctx->ae_entity_ctx.ae_result;
	awb_result = &ctx->awb_entity_ctx.awb_result;
	exp_gain.exp_val = ae_result->sensor_set.ev_set_curr.ev_sensor_exp_line;
	exp_gain.exp_mid_val = ae_result->sensor_set_short.ev_set_curr.ev_sensor_exp_line;
	exp_gain.gain_val = ae_result->sensor_set.ev_set_curr.ev_analog_gain >> 4;
	exp_gain.gain_mid_val = ae_result->sensor_set_short.ev_set_curr.ev_analog_gain >> 4;
	exp_gain.r_gain = awb_result->wb_gain_output.r_gain * 256 / awb_result->wb_gain_output.gr_gain;
	exp_gain.b_gain = awb_result->wb_gain_output.b_gain * 256 / awb_result->wb_gain_output.gb_gain;

#if 0
	if (memcmp(&exp_gain_save[isp->id], &exp_gain, sizeof(struct sensor_exp_gain))) {
		memcpy(&exp_gain_save[isp->id], &exp_gain, sizeof(struct sensor_exp_gain));
	} else {
		return;
	}
#else
	if ((exp_gain_save[isp->id].exp_val != exp_gain.exp_val) || (exp_gain_save[isp->id].gain_val != exp_gain.gain_val) ||
		(exp_gain_save[isp->id].exp_mid_val != exp_gain.exp_mid_val) || (exp_gain_save[isp->id].gain_mid_val != exp_gain.gain_mid_val)) {
		memcpy(&exp_gain_save[isp->id], &exp_gain, sizeof(struct sensor_exp_gain));
	} else {
		return;
	}
#endif

	for (i = 0; i < VIN_MAX_DEV; i++) {
		if (global_video[i].used == 0)
			continue;

		vinc = &global_video[i];
		if (vinc->isp_sel == isp->id)
			break;
	}
	if (vinc != NULL) {
		sensor_id = vinc->mipi_sel;
		if (global_sensors[sensor_id].sensor_core->s_exp_gain)
			global_sensors[sensor_id].sensor_core->s_exp_gain(sensor_id, &exp_gain);
	}
}

int isp_sensor_set_ir_status(struct isp_dev *isp)
{
#ifdef CONFIG_ISP_FAST_CONVERGENCE
	struct hw_isp_device *hw_isp = isp->hw_isp;
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);
	ae_result_t *ae_result = NULL;
	struct vin_core *vinc = NULL;
	unsigned int sensor_id;
	int i;

	if (ctx == NULL)
		return 0;

	ae_result = &ctx->ae_entity_ctx.ae_result;
	if (ctx->isp_ir_flag == 0 && ae_result->ir_status_output == 1) {
		for (i = 0; i < VIN_MAX_DEV; i++) {
			if (global_video[i].used == 0)
				continue;

			vinc = &global_video[i];
			if (vinc->isp_sel == isp->id)
				break;
		}
		if (vinc != NULL) {
			sensor_id = vinc->mipi_sel;
			if (global_sensors[sensor_id].sensor_core->s_ir_status)
				global_sensors[sensor_id].sensor_core->s_ir_status(sensor_id, IR_NIGHT);
		}
		isp_server_reset(hw_isp, hw_isp->id, 0x02);
		//hal_msleep(200);
		return ctx->sensor_info.fps_fixed / 24;//200ms
	}
#endif
	return 0;
}

void isp_sensor_set_first_exp_gain(struct isp_dev *isp)
{
	struct hw_isp_device *hw_isp = isp->hw_isp;
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);
	struct sensor_exp_gain exp_gain;
	ae_result_t *ae_result = NULL;
	awb_result_t *awb_result = NULL;
	struct vin_core *vinc = NULL;
	unsigned int sensor_id;
	int i;

	if (ctx == NULL)
		return;

	ae_result = &ctx->ae_entity_ctx.ae_result;
	awb_result = &ctx->awb_entity_ctx.awb_result;
	exp_gain.exp_val = ae_result->sensor_set.ev_set_curr.ev_sensor_exp_line;
	exp_gain.exp_mid_val = ae_result->sensor_set_short.ev_set_curr.ev_sensor_exp_line;
	exp_gain.gain_val = ae_result->sensor_set.ev_set_curr.ev_analog_gain >> 4;
	exp_gain.gain_mid_val = ae_result->sensor_set_short.ev_set_curr.ev_analog_gain >> 4;
	exp_gain.r_gain = awb_result->wb_gain_output.r_gain * 256 / awb_result->wb_gain_output.gr_gain;
	exp_gain.b_gain = awb_result->wb_gain_output.b_gain * 256 / awb_result->wb_gain_output.gb_gain;

	for (i = 0; i < VIN_MAX_DEV; i++) {
		if (global_video[i].used == 0)
			continue;

		vinc = &global_video[i];
		if (vinc->isp_sel == isp->id)
			break;
	}
	if (vinc != NULL) {
		sensor_id = vinc->mipi_sel;
		if (global_sensors[sensor_id].sensor_core->s_exp_gain)
			global_sensors[sensor_id].sensor_core->s_exp_gain(sensor_id, &exp_gain);
	}
}

int vin_server_init(struct isp_dev *isp, struct isp_part_para isp_part)
{
	struct isp_lib_context *isp_gen;
	int ir;

	if (isp->isp_server_init) {
		vin_warn("vin isp server is inited!!\n");
		return -1;
	}

	isp->hw_isp = isp_server_init(isp->id);

	isp_gen = isp_dev_get_ctx(isp->hw_isp);
	isp_gen->flicker_mode = isp_part.flicker_mode;
	isp_get_ir_threshold(isp->hw_isp);
	isp_get_ir_mode(isp->hw_isp, &isp_part.ir_mode);

	//isp_sensor_otp_init(isp);
	if (isp_config_sensor_info(isp))
		return -1;

	isp_server_save_ctx(isp->id);
	isp_alloc_reg_tbl(isp);

	isp_gen->isp_load_flag = 1;

	if (isp_part.ir_mode == 2)
		ir = 1;
	else
		ir = 0;
	if (isp_server_set_paras(isp->hw_isp, ir, isp->id))
		return -1;

	if (!isp_server_save_reg(isp->id, ir)) {
#if defined CONFIG_D3D
		if ((isp->load_shadow[0x320 + 0x0]) & (1<<0)) {
			/* clear D3D rec_en */
			isp->load_shadow[0x320 + 0x0] = (isp->load_shadow[0x320 + 0x0]) & (~(1<<0));
			memcpy(isp->isp_load.phy_addr, &isp->load_shadow[0], ISP_LOAD_DRAM_SIZE);
		}
#endif
		/* increase bdnf because of tdnf not effect in the first*/
		//memcpy(isp_gen->load_reg_base + 0x2a0, &isp_bdnf_reg[0], 4 << 2);
		//memcpy(isp_gen->load_reg_base + 0x9b8, &isp_bdnf_reg[4], (17*3) << 2);

		memcpy(isp->isp_load.phy_addr, isp_gen->load_reg_base, ISP_LOAD_DRAM_SIZE);
		hal_dcache_clean((unsigned long)isp->isp_load.phy_addr, ISP_LOAD_DRAM_SIZE);
		isp->load_flag = 0;
	} else {
		memcpy(isp_gen->load_reg_base, &isp_default_reg[0], ISP_LOAD_REG_SIZE);
		hal_dcache_clean((unsigned long)isp_gen->load_reg_base, ISP_LOAD_REG_SIZE);
		isp_set_load_reg(isp);
	}

	return 0;

}

void vin_server_reset(struct isp_dev *isp, int ir_mode)
{
	isp_server_reset(isp->hw_isp, isp->id, ir_mode);
}

/*
 * use to change from higt frame rate to low frame rate in the beginning, in order to get a normal effect as soon as possible!
 * and then get the lv to choose the nignt or day effect file
 */
void vin_server_update(struct isp_dev *isp, struct isp_part_para isp_part)
{
#if 1
	//int ir;
	struct isp_lib_context *isp_gen;

	//if (isp_part.ir_mode == 2)
	//	ir = 1;
	//else
	//	ir = 0;

	isp_gen = isp_dev_get_ctx(isp->hw_isp);
	isp_gen->flicker_mode = isp_part.flicker_mode;

	if (isp_config_switch_sensor_info(isp))
		return;

	isp_server_reset(isp->hw_isp, isp->id, isp_part.ir_mode);
	isp_sensor_set_first_exp_gain(isp);

	isp_set_firstframe_wb(isp->id);
#endif
}

int vin_isp_get_lv(struct isp_dev *isp)
{
	return isp_get_lv(isp->hw_isp, isp->id);
}

void vin_isp_set_ae_lv(struct isp_dev *isp, unsigned int ev_lv)
{
	struct hw_isp_device *hw_isp = isp->hw_isp;
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);
	ae_result_t *ae_result = NULL;

	if (ctx == NULL)
		return;

	ae_result = &ctx->ae_entity_ctx.ae_result;

	ae_result->sensor_set.ev_set_curr.ev_lv = ev_lv;
}

int vin_isp_get_exp_gain(struct isp_dev *isp, struct sensor_exp_gain *exp_gain)
{
	struct hw_isp_device *hw_isp = isp->hw_isp;
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);
	ae_result_t *ae_result = NULL;
	awb_result_t *awb_result = NULL;

	if (ctx == NULL)
		return -1;

	ae_result = &ctx->ae_entity_ctx.ae_result;
	awb_result = &ctx->awb_entity_ctx.awb_result;
	exp_gain->exp_val = ae_result->sensor_set.ev_set_curr.ev_sensor_exp_line;
	exp_gain->exp_mid_val = ae_result->sensor_set_short.ev_set_curr.ev_sensor_exp_line;
	exp_gain->gain_val = ae_result->sensor_set.ev_set_curr.ev_analog_gain >> 4;
	exp_gain->gain_mid_val = ae_result->sensor_set_short.ev_set_curr.ev_analog_gain >> 4;
	exp_gain->r_gain = awb_result->wb_gain_output.r_gain * 256 / awb_result->wb_gain_output.gr_gain;
	exp_gain->b_gain = awb_result->wb_gain_output.b_gain * 256 / awb_result->wb_gain_output.gb_gain;

	return 0;
}

int vin_isp_get_hist(struct isp_dev *isp, unsigned int *hist)
{
	struct hw_isp_device *hw_isp = isp->hw_isp;
	struct isp_stats_context *stats_ctx;

	if (hw_isp == NULL)
		return -1;

	stats_ctx = hal_malloc(sizeof(struct isp_stats_context));
	if (!stats_ctx) {
		ISP_ERR("stats_ctx malloc fail!\n");
		return -1;
	}
	memset(stats_ctx, 0, sizeof(struct isp_stats_context));

	isp_stats_req(hw_isp, isp->id, stats_ctx);

	memcpy(hist, stats_ctx->stats.ae_stats.hist, ISP_HIST_NUM*sizeof(unsigned int));

	hal_free(stats_ctx);

	return 0;
}

int vin_isp_ae_done(struct isp_dev *isp, int drap_frame_en)
{
	struct hw_isp_device *hw_isp = isp->hw_isp;
	struct isp_lib_context *isp_gen = isp_dev_get_ctx(hw_isp);
	ae_result_t *ae_result = NULL;
	awb_result_t *awb_result = NULL;

	if (isp_gen == NULL)
		return -1;

	ae_result = &isp_gen->ae_entity_ctx.ae_result;
	awb_result = &isp_gen->awb_entity_ctx.awb_result;

#if defined CONFIG_ISP_FAST_CONVERGENCE
	if (!drap_frame_en ||
			(ae_result->ae_status == AE_STATUS_DONE || isp_gen->ae_frame_cnt >= 15)) {
			//(awb_result->awb_status == AWB_STATUS_DONE || isp_gen->awb_frame_cnt >= 30))) {
		if (drap_frame_en) {
			vin_print("isp%d:high frame rate convergence! isp_count:%ld, ae_conut:%ld, awb_count:%ld\n",
				isp->id, isp->isp_frame_number, (unsigned long)isp_gen->ae_frame_cnt, (unsigned long)isp_gen->awb_frame_cnt);
		}
		return 0;
	}
#elif defined CONFIG_ISP_HARD_LIGHTADC
	if (!drap_frame_en ||
			(ae_result->ae_status == AE_STATUS_DONE || isp_gen->ae_frame_cnt >= 15)) {
			//(awb_result->awb_status == AWB_STATUS_DONE || isp_gen->awb_frame_cnt >= 30))) {
		if (drap_frame_en) {
			vin_print("isp%d:hard light adc! isp_count:%ld, ae_conut:%ld, awb_count:%ld\n",
				isp->id, isp->isp_frame_number, (unsigned long)isp_gen->ae_frame_cnt, (unsigned long)isp_gen->awb_frame_cnt);
		}
		return 0;
	}
#else //CONFIG_ISP_READ_THRESHOLD
	if (drap_frame_en) {
		vin_print("isp%d:read threshold! isp_count:%ld, ae_conut:%ld, awb_count:%ld\n",
			isp->id, isp->isp_frame_number, (unsigned long)isp_gen->ae_frame_cnt, (unsigned long)isp_gen->awb_frame_cnt);
		return 0;
	}
#endif
	return -1;
}

