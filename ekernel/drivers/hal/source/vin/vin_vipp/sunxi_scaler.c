/*
 * sunxi_scaler.c for scaler and osd v4l2 subdev
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <hal_mem.h>
#include <hal_interrupt.h>

#include "../vin.h"
#include "../platform/platform_cfg.h"
#include "sunxi_scaler.h"
#include "../vin_video/vin_core.h"

struct scaler_dev *global_scaler[VIN_MAX_SCALER];
bool scaler_probe_flag[VIN_MAX_SCALER];

#define VIPP_ISR_EN

#define ALIGN_4B(x)	(((x) + (3)) & ~(3))
#define ALIGN_2B(x)	(((x) + (1)) & ~(1))

#define MIN_IN_WIDTH			192
#define MIN_IN_HEIGHT			128
#define MAX_IN_WIDTH			4224
#define MAX_IN_HEIGHT			4224

#define MIN_OUT_WIDTH			16
#define MIN_OUT_HEIGHT			10
#define MAX_OUT_WIDTH			4224
#define MAX_OUT_HEIGHT			4224

#define MIN_RATIO			256
#define MAX_RATIO			4096

static int __scaler_w_shift(int x_ratio, int y_ratio)
{
	int m, n;
	int sum_weight = 0;
	int weight_shift;
	int xr = (x_ratio >> 8) + 1;
	int yr = (y_ratio >> 8) + 1;
	int weight_shift_bak = 0;

	weight_shift = -9;
	for (weight_shift = 17; weight_shift > 0; weight_shift--) {
		sum_weight = 0;
		for (m = 0; m <= xr; m++) {
			for (n = 0; n <= yr; n++) {
				sum_weight += (y_ratio - abs((n << 8) - (yr << 7)))*
					(x_ratio - abs((m << 8) - (yr << 7))) >> (weight_shift + 8);
			}
		}
		if (sum_weight > 0 && sum_weight < 256)
			weight_shift_bak = weight_shift;
		if (sum_weight > 255 && sum_weight < 486)
			break;
	}
	if (weight_shift == 0)
		weight_shift = weight_shift_bak;

	return weight_shift;
}

static void __scaler_calc_ratios(unsigned int id, struct v4l2_rect *input,
				struct scaler_para *para)
{
	unsigned int width;
	unsigned int height;
	unsigned int r_min;

	global_video[id].o_width = clamp(global_video[id].o_width, MIN_IN_WIDTH, input->width);
	global_video[id].o_height =
		clamp(global_video[id].o_height, MIN_IN_HEIGHT, input->height);

	para->xratio = 256 * input->width / global_video[id].o_width;
	para->yratio = 256 * input->height / global_video[id].o_height;
	para->xratio = clamp(para->xratio, MIN_RATIO, MAX_RATIO);
	para->yratio = clamp(para->yratio, MIN_RATIO, MAX_RATIO);

	r_min = min(para->xratio, para->yratio);

	if (para->xratio == para->yratio) {
		width = min(ALIGN(global_video[id].o_width * r_min / 256, 2), input->width);
		height = min(ALIGN(global_video[id].o_height * r_min / 256, 2), input->height);
		para->width = global_video[id].o_width;
		para->height = global_video[id].o_height;

		vin_log(VIN_LOG_SCALER, "para: xr = %ld, yr = %ld, w = %ld, h = %ld\n",
			  para->xratio, para->yratio, para->width, para->height);
		/* Center the new crop rectangle.
		 * crop is before scaler
		 */
		input->left += (input->width - width) / 2;
		input->top += (input->height - height) / 2;
		input->left = ALIGN(input->left, 2);
		input->top = ALIGN(input->top, 1);
	} else {
		width = ALIGN_4B(global_video[id].o_width * r_min / 256);
		height = ALIGN_2B(global_video[id].o_height * r_min / 256);
		para->xratio = 256 * width / global_video[id].o_width;
		para->yratio = 256 * height / global_video[id].o_height;
		para->xratio = clamp(para->xratio, MIN_RATIO, MAX_RATIO);
		para->yratio = clamp(para->yratio, MIN_RATIO, MAX_RATIO);
		para->width = global_video[id].o_width;
		para->height = global_video[id].o_height;
		vin_log(VIN_LOG_SCALER, "para: xr = %ld, yr = %ld, w = %ld, h = %ld\n",
			  para->xratio, para->yratio, para->width, para->height);
		/* Center the new crop rectangle.
		 * crop is before scaler
		 */
		input->left += (input->width - width) / 2;
		input->top += (input->height - height) / 2;
		input->left = ALIGN_4B(input->left);
		input->top = ALIGN_2B(input->top);
		input->width = width;
		input->height = height;
	}

	vin_log(VIN_LOG_SCALER, "crop: left = %ld, top = %ld, w = %ld, h = %ld\n",
		input->left, input->top, input->width, input->height);
}

#ifdef VIPP_REG_MODE
static int sunxi_scaler_subdev_init(unsigned int id)
{
	struct scaler_dev *scaler = global_scaler[id];

	memset((void *)scaler->vipp_reg.phy_addr, 0, VIPP_REG_SIZE);
	scaler->load_select = true;
	vipp_set_reg_load_addr(scaler->id, (unsigned long)scaler->load_para[0].phy_addr);
	vipp_set_osd_cv_update(scaler->id, NOT_UPDATED);
	vipp_set_osd_ov_update(scaler->id, NOT_UPDATED);
	vipp_set_para_ready(scaler->id, NOT_READY);
	return 0;
}
#endif

static int sunxi_scaler_logic_s_stream(unsigned int virtual_id, int on)
{
	unsigned char logic_id = vipp_virtual_find_logic[virtual_id];
	struct scaler_dev *logic_scaler = global_scaler[logic_id];
	int i;

	if (logic_scaler->work_mode == VIPP_ONLINE && virtual_id != logic_id) {
		vin_err("vipp%d work on online mode, vipp%d cannot to work!!\n", logic_id, virtual_id);
		return -1;
	}

	if (on && (logic_scaler->logic_top_stream_count)++ > 0)
		return 0;
	else if (!on && (logic_scaler->logic_top_stream_count == 0 || --(logic_scaler->logic_top_stream_count) > 0))
		return 0;

	if (on) {
		vipp_cap_disable(logic_scaler->id);
		for (i = 0; i < VIPP_VIRT_NUM; i++)
			vipp_chn_cap_disable(logic_scaler->id);

		vipp_work_mode(logic_scaler->id, logic_scaler->work_mode);
		vipp_top_clk_en(logic_scaler->id, on);
		vipp_clear_status(logic_scaler->id, VIPP_STATUS_ALL);
#ifdef VIPP_ISR_EN
#if 0
		vipp_irq_enable(logic_scaler->id, ID_LOST_EN | AHB_MBUS_W_EN |
				CHN0_REG_LOAD_EN | CHN0_FRAME_LOST_EN | CHN0_HBLANK_SHORT_EN | CHN0_PARA_NOT_READY_EN |
				CHN1_REG_LOAD_EN | CHN1_FRAME_LOST_EN | CHN1_HBLANK_SHORT_EN | CHN1_PARA_NOT_READY_EN |
				CHN2_REG_LOAD_EN | CHN2_FRAME_LOST_EN | CHN2_HBLANK_SHORT_EN | CHN2_PARA_NOT_READY_EN |
				CHN3_REG_LOAD_EN | CHN3_FRAME_LOST_EN | CHN3_HBLANK_SHORT_EN | CHN3_PARA_NOT_READY_EN);
#else
		vipp_irq_enable(logic_scaler->id, ID_LOST_EN | AHB_MBUS_W_EN |
				CHN0_FRAME_LOST_EN | CHN0_HBLANK_SHORT_EN | CHN0_PARA_NOT_READY_EN |
				CHN1_FRAME_LOST_EN | CHN1_HBLANK_SHORT_EN | CHN1_PARA_NOT_READY_EN |
				CHN2_FRAME_LOST_EN | CHN2_HBLANK_SHORT_EN | CHN2_PARA_NOT_READY_EN |
				CHN3_FRAME_LOST_EN | CHN3_HBLANK_SHORT_EN | CHN3_PARA_NOT_READY_EN);
		hal_enable_irq(logic_scaler->irq);
#endif
#endif
		vipp_cap_enable(logic_scaler->id);
	} else {
		hal_disable_irq(logic_scaler->irq);
		vipp_cap_disable(logic_scaler->id);
		vipp_clear_status(logic_scaler->id, VIPP_STATUS_ALL);
		vipp_irq_disable(logic_scaler->id, VIPP_EN_ALL);
		vipp_top_clk_en(logic_scaler->id, on);
	}

	return 0;
}


int sunxi_scaler_subdev_s_stream(unsigned int id, int enable)
{
	struct scaler_dev *scaler = global_scaler[id];
	struct vipp_scaler_config scaler_cfg;
	struct vipp_scaler_size scaler_size;
	struct vipp_crop crop;
	enum vipp_format out_fmt;
	enum vipp_format sc_fmt;
	struct sensor_format_struct *sensor_format;
	int mipi_sel = global_video[scaler->id].mipi_sel;
	int *stream_count;

	stream_count = &scaler->stream_count;
	if (enable && (*stream_count)++ > 0)
		return 0;
	else if (!enable && (*stream_count == 0 || --(*stream_count) > 0))
		return 0;

	switch (global_video[id].fourcc) {
	case V4L2_PIX_FMT_SBGGR8:
	case V4L2_PIX_FMT_SGBRG8:
	case V4L2_PIX_FMT_SGRBG8:
	case V4L2_PIX_FMT_SRGGB8:
	case V4L2_PIX_FMT_SBGGR10:
	case V4L2_PIX_FMT_SGBRG10:
	case V4L2_PIX_FMT_SGRBG10:
	case V4L2_PIX_FMT_SRGGB10:
	case V4L2_PIX_FMT_SBGGR12:
	case V4L2_PIX_FMT_SGBRG12:
	case V4L2_PIX_FMT_SGRBG12:
	case V4L2_PIX_FMT_SRGGB12:
		vin_log(VIN_LOG_FMT, "%s output fmt is raw, return directly\n", __func__);
		return 0;
	default:
		break;
	}
	if (global_sensors[mipi_sel].sensor_core->sensor_g_format)
		sensor_format = global_sensors[mipi_sel].sensor_core->sensor_g_format(mipi_sel);
	else
		return -1;

	if (sensor_format->field == V4L2_FIELD_INTERLACED || sensor_format->field == V4L2_FIELD_TOP ||
	    sensor_format->field == V4L2_FIELD_BOTTOM) {
		vin_log(VIN_LOG_SCALER, "Scaler not support field mode, return directly!\n");
		return 0;
	}

	if (enable) {
		if (sunxi_scaler_logic_s_stream(scaler->id, enable))
			return -1;

		crop.hor = scaler->crop.active.left;
		crop.ver = scaler->crop.active.top;
		crop.width = scaler->crop.active.width;
		crop.height = scaler->crop.active.height;
		vipp_set_crop(scaler->id, &crop);
		scaler_size.sc_width = scaler->para.width;
		scaler_size.sc_height = scaler->para.height;
		vipp_scaler_output_size(scaler->id, &scaler_size);

		switch (global_video[id].fourcc) {
		case V4L2_PIX_FMT_YUV420:
		case V4L2_PIX_FMT_YUV420M:
		case V4L2_PIX_FMT_YVU420:
		case V4L2_PIX_FMT_YVU420M:
		case V4L2_PIX_FMT_NV21:
		case V4L2_PIX_FMT_NV21M:
		case V4L2_PIX_FMT_NV12:
		case V4L2_PIX_FMT_NV12M:
		case V4L2_PIX_FMT_FBC:
		case V4L2_PIX_FMT_LBC_2_0X:
		case V4L2_PIX_FMT_LBC_2_5X:
		case V4L2_PIX_FMT_LBC_1_0X:
			if (scaler->id < MAX_OSD_NUM) {
				sc_fmt = YUV422;
				out_fmt = YUV420;
				vipp_chroma_ds_en(scaler->id, 1);
			} else {
				sc_fmt = YUV420;
				out_fmt = YUV420;
			}
			break;
		case V4L2_PIX_FMT_YUV422P:
		case V4L2_PIX_FMT_NV16:
		case V4L2_PIX_FMT_NV61:
		case V4L2_PIX_FMT_NV61M:
		case V4L2_PIX_FMT_NV16M:
			sc_fmt = YUV422;
			out_fmt = YUV422;
			break;
		default:
			sc_fmt = YUV420;
			out_fmt = YUV420;
			break;
		}
		scaler_cfg.sc_out_fmt = sc_fmt;
		scaler_cfg.sc_x_ratio = scaler->para.xratio;
		scaler_cfg.sc_y_ratio = scaler->para.yratio;
		scaler_cfg.sc_w_shift = __scaler_w_shift(scaler->para.xratio, scaler->para.yratio);
		vipp_scaler_cfg(scaler->id, &scaler_cfg);
		vipp_output_fmt_cfg(scaler->id, out_fmt);
		vipp_scaler_en(scaler->id, 1);
		vipp_osd_en(scaler->id, 1);
		vipp_set_osd_ov_update(scaler->id, HAS_UPDATED);
		vipp_set_osd_cv_update(scaler->id, HAS_UPDATED);
		memcpy(scaler->load_para[0].phy_addr, scaler->vipp_reg.phy_addr, VIPP_REG_SIZE);
		vipp_set_para_ready(scaler->id, HAS_READY);
		vipp_chn_cap_enable(scaler->id);
	} else {
		vipp_chn_cap_disable(scaler->id);
		vipp_set_para_ready(scaler->id, NOT_READY);
		vipp_chroma_ds_en(scaler->id, 0);
		vipp_osd_en(scaler->id, 0);
		vipp_scaler_en(scaler->id, 0);
		vipp_set_osd_ov_update(scaler->id, NOT_UPDATED);
		vipp_set_osd_cv_update(scaler->id, NOT_UPDATED);

		sunxi_scaler_logic_s_stream(scaler->id, enable);
	}
	vin_log(VIN_LOG_FMT, "vipp%d %s, %ld*%ld hoff: %ld voff: %ld xr: %ld yr: %ld\n",
		scaler->id, enable ? "stream on" : "stream off",
		scaler->para.width, scaler->para.height,
		scaler->crop.active.left, scaler->crop.active.top,
		scaler->para.xratio, scaler->para.yratio);

	return 0;
}

#ifdef VIPP_ISR_EN
static hal_irqreturn_t scaler_isr(void *priv)
{
	uint32_t __cpsr;
	struct scaler_dev *scaler = (struct scaler_dev *)priv;
	struct vipp_status vipp_status;

	memset(&vipp_status, 0, sizeof(struct vipp_status));
	vipp_get_status(scaler->id, &vipp_status);

	__cpsr = hal_spin_lock_irqsave(&scaler->slock);

	if (vipp_status.id_lost_pd) {
		vipp_clear_status(scaler->id, ID_LOST_PD);
		vin_err("scaler%d channel ID nember is lost!!!\n", scaler->id);
	}

	if (vipp_status.ahb_mbus_w_pd) {
		vipp_clear_status(scaler->id, VIPPAHB_MBUS_W_PD);
		vin_err("scaler%d AHB and MBUS write conflict!!!\n", scaler->id);
	}

	if (vipp_status.chn0_frame_lost_pd) {
		vipp_clear_status(scaler->id, CHN0_FRAME_LOST_PD);
		vin_err("scaler%d frame lost!!!\n", scaler->id + 0);
	}
	if (vipp_status.chn1_frame_lost_pd) {
		vipp_clear_status(scaler->id, CHN1_FRAME_LOST_PD);
		vin_err("scaler%d frame lost!!!\n", scaler->id + 1);
	}
	if (vipp_status.chn2_frame_lost_pd) {
		vipp_clear_status(scaler->id, CHN2_FRAME_LOST_PD);
		vin_err("scaler%d frame lost!!!\n", scaler->id + 2);
	}
	if (vipp_status.chn3_frame_lost_pd) {
		vipp_clear_status(scaler->id, CHN3_FRAME_LOST_PD);
		vin_err("scaler%d frame lost!!!\n", scaler->id + 3);
	}

	if (vipp_status.chn0_hblank_short_pd) {
		vipp_clear_status(scaler->id, CHN0_HBLANK_SHORT_PD);
		vin_err("scaler%d Hblank short!!!\n", scaler->id + 0);
	}
	if (vipp_status.chn1_hblank_short_pd) {
		vipp_clear_status(scaler->id, CHN1_HBLANK_SHORT_PD);
		vin_err("scaler%d Hblank short!!!\n", scaler->id + 1);
	}
	if (vipp_status.chn2_hblank_short_pd) {
		vipp_clear_status(scaler->id, CHN2_HBLANK_SHORT_PD);
		vin_err("scaler%d Hblank short!!!\n", scaler->id + 2);
	}
	if (vipp_status.chn3_hblank_short_pd) {
		vipp_clear_status(scaler->id, CHN3_HBLANK_SHORT_PD);
		vin_err("scaler%d Hblank short!!!\n", scaler->id + 3);
	}

	if (vipp_status.chn0_para_not_ready_pd) {
		vipp_clear_status(scaler->id, CHN0_PARA_NOT_READY_PD);
		vin_err("scaler%d param not ready!!!\n", scaler->id + 0);
	}
	if (vipp_status.chn1_para_not_ready_pd) {
		vipp_clear_status(scaler->id, CHN1_PARA_NOT_READY_PD);
		vin_err("scaler%d param not ready!!!\n", scaler->id + 1);
	}
	if (vipp_status.chn2_para_not_ready_pd) {
		vipp_clear_status(scaler->id, CHN2_PARA_NOT_READY_PD);
		vin_err("scaler%d param not ready!!!\n", scaler->id + 2);
	}
	if (vipp_status.chn3_para_not_ready_pd) {
		vipp_clear_status(scaler->id, CHN3_PARA_NOT_READY_PD);
		vin_err("scaler%d param not ready!!!\n", scaler->id + 3);
	}

	if (vipp_status.chn0_reg_load_pd) {
		vipp_clear_status(scaler->id, CHN0_REG_LOAD_PD);
		if (!vipp_get_irq_en(scaler->id, CHN0_REG_LOAD_EN)) {
			hal_spin_unlock_irqrestore(&scaler->slock, __cpsr);
			return 0;
		}
		vin_print("vipp%d change to load reg\n", scaler->id);
		if (scaler->load_select) {
			memcpy(scaler->load_para[1].phy_addr, scaler->vipp_reg.phy_addr, VIPP_REG_SIZE);
			vipp_set_reg_load_addr(scaler->id, (unsigned long)scaler->load_para[1].phy_addr);
			scaler->load_select = false;
		} else {
			memcpy(scaler->load_para[0].phy_addr, scaler->vipp_reg.phy_addr, VIPP_REG_SIZE);
			vipp_set_reg_load_addr(scaler->id, (unsigned long)scaler->load_para[0].phy_addr);
			scaler->load_select = true;
		}
		vipp_irq_disable(scaler->id, CHN0_REG_LOAD_EN);
	}
	if (vipp_status.chn1_reg_load_pd) {
		vipp_clear_status(scaler->id, CHN1_REG_LOAD_PD);
		if (!vipp_get_irq_en(scaler->id, CHN1_REG_LOAD_EN)) {
			hal_spin_unlock_irqrestore(&scaler->slock, __cpsr);
			return 0;
		}
		vin_print("vipp%d change to load reg\n", scaler->id + 1);
		if (global_scaler[scaler->id + 1]->load_select) {
			memcpy(global_scaler[scaler->id + 1]->load_para[1].phy_addr, global_scaler[scaler->id + 1]->vipp_reg.phy_addr, VIPP_REG_SIZE);
			vipp_set_reg_load_addr(global_scaler[scaler->id + 1]->id, (unsigned long)global_scaler[scaler->id + 1]->load_para[1].phy_addr);
			scaler->load_select = false;
		} else {
			memcpy(global_scaler[scaler->id + 1]->load_para[0].phy_addr, global_scaler[scaler->id + 1]->vipp_reg.phy_addr, VIPP_REG_SIZE);
			vipp_set_reg_load_addr(global_scaler[scaler->id + 1]->id, (unsigned long)global_scaler[scaler->id + 1]->load_para[0].phy_addr);
			scaler->load_select = true;
		}
		vipp_irq_disable(scaler->id, CHN1_REG_LOAD_EN);
	}
	if (vipp_status.chn2_reg_load_pd) {
		vipp_clear_status(scaler->id, CHN2_REG_LOAD_PD);
		if (!vipp_get_irq_en(scaler->id, CHN2_REG_LOAD_EN)) {
			hal_spin_unlock_irqrestore(&scaler->slock, __cpsr);
			return 0;
		}
		vin_print("vipp%d change to load reg\n", scaler->id + 2);
		if (global_scaler[scaler->id + 2]->load_select) {
			memcpy(global_scaler[scaler->id + 2]->load_para[1].phy_addr, global_scaler[scaler->id + 2]->vipp_reg.phy_addr, VIPP_REG_SIZE);
			vipp_set_reg_load_addr(global_scaler[scaler->id + 2]->id, (unsigned long)global_scaler[scaler->id + 2]->load_para[1].phy_addr);
			scaler->load_select = false;
		} else {
			memcpy(global_scaler[scaler->id + 2]->load_para[0].phy_addr, global_scaler[scaler->id + 2]->vipp_reg.phy_addr, VIPP_REG_SIZE);
			vipp_set_reg_load_addr(global_scaler[scaler->id + 2]->id, (unsigned long)global_scaler[scaler->id + 2]->load_para[0].phy_addr);
			scaler->load_select = true;
		}
		vipp_irq_disable(scaler->id, CHN2_REG_LOAD_EN);
	}
	if (vipp_status.chn3_reg_load_pd) {
		vipp_clear_status(scaler->id, CHN3_REG_LOAD_PD);
		if (!vipp_get_irq_en(scaler->id, CHN3_REG_LOAD_EN)) {
			hal_spin_unlock_irqrestore(&scaler->slock, __cpsr);
			return 0;
		}
		vin_print("vipp%d change to load reg\n", scaler->id + 3);
		if (global_scaler[scaler->id + 3]->load_select) {
			memcpy(global_scaler[scaler->id + 3]->load_para[1].phy_addr, global_scaler[scaler->id + 3]->vipp_reg.phy_addr, VIPP_REG_SIZE);
			vipp_set_reg_load_addr(global_scaler[scaler->id + 3]->id, (unsigned long)global_scaler[scaler->id + 3]->load_para[1].phy_addr);
			scaler->load_select = false;
		} else {
			memcpy(global_scaler[scaler->id + 3]->load_para[0].phy_addr, global_scaler[scaler->id + 3]->vipp_reg.phy_addr, VIPP_REG_SIZE);
			vipp_set_reg_load_addr(global_scaler[scaler->id + 3]->id, (unsigned long)global_scaler[scaler->id + 3]->load_para[0].phy_addr);
			scaler->load_select = true;
		}
		vipp_irq_disable(scaler->id, CHN3_REG_LOAD_EN);
	}

	hal_spin_unlock_irqrestore(&scaler->slock, __cpsr);

	return 0;
}
#endif


#ifdef VIPP_REG_MODE
static int scaler_resource_alloc(unsigned int id)
{
	struct scaler_dev *scaler = global_scaler[id];

	scaler->vipp_reg.size = 3 * VIPP_REG_SIZE;
	scaler->vipp_reg.phy_addr = rt_memheap_alloc_align(&isp_mempool, scaler->vipp_reg.size, 0x1000);
	if (scaler->vipp_reg.phy_addr == NULL) {
		vin_err("%s:vipp%d:alloc vipp buffer error\n", __func__, id);
		return -1;
	}
	memset(scaler->vipp_reg.phy_addr, 0, scaler->vipp_reg.size);
	//vin_print("%s:vipp_reg phy_addr is 0x%x\n", __func__, (unsigned long)scaler->vipp_reg.phy_addr);

	scaler->load_para[0].phy_addr = scaler->vipp_reg.phy_addr + VIPP_REG_SIZE;
	scaler->load_para[1].phy_addr = scaler->vipp_reg.phy_addr + VIPP_REG_SIZE * 2;
	return 0;
}

static void scaler_resource_free(unsigned int id)
{
	//struct scaler_dev *scaler = global_scaler[id];

}
#endif

int scaler_probe(unsigned int id)
{
	struct scaler_dev *scaler = NULL;
	struct scaler_dev *logic_scaler = NULL;
	struct sensor_format_struct *sensor_format;
	int mipi_sel = global_video[id].mipi_sel;
	int ret = 0;

	if (scaler_probe_flag[id])
		return 0;
	else
		scaler_probe_flag[id] = true;

	if (id > VIN_MAX_SCALER) {
		vin_err("vipp%d is not existing, max is %d\n", id, VIN_MAX_SCALER);
		return -1;
	}

	scaler = hal_malloc(sizeof(struct scaler_dev));
	if (!scaler) {
		ret = -1;
		goto ekzalloc;
	}
	memset(scaler, 0, sizeof(struct scaler_dev));
	scaler->id = id;

	if (scaler->id == vipp_virtual_find_logic[scaler->id]) {
		scaler->work_mode = vin_work_mode;//clamp(scaler->work_mode, VIPP_ONLINE, VIPP_OFFLINE);
	} else {
		scaler->work_mode = 0xff;
		logic_scaler = global_scaler[vipp_virtual_find_logic[scaler->id]];
		if (logic_scaler->work_mode == VIPP_ONLINE) { /*logic vipp work in online*/
			vin_log(VIN_LOG_VIDEO, "scaler%d work in online mode, scaler%d cannot to work!\n", logic_scaler->id, scaler->id);
			global_scaler[scaler->id] = scaler;
			scaler->noneed_register = 1;
			vin_log(VIN_LOG_SCALER, "scaler%d probe end\n", scaler->id);
			return 0;
		}
	}

	scaler->base = sunxi_vin_get_scaler_base(scaler->id);
	if (!scaler->base) {
		vin_err("Fail to get VIPP base addr!\n");
		ret = -1;
		goto freedev;
	}
	vin_log(VIN_LOG_MD, "vipp%d base reg is 0x%lx\n", scaler->id, scaler->base);

	global_scaler[id] = scaler;
#ifdef VIPP_REG_MODE
	scaler_resource_alloc(id);
#endif
	vipp_set_base_addr(scaler->id, scaler->base);

#ifdef VIPP_REG_MODE
	vipp_map_reg_load_addr(scaler->id, (unsigned long)scaler->vipp_reg.phy_addr);

	sunxi_scaler_subdev_init(id);
#endif

#ifdef VIPP_ISR_EN
	/*get irq resource */
	if (scaler->id == vipp_virtual_find_logic[scaler->id]) {
		scaler->irq = sunxi_vin_get_vipp_irq(scaler->id);
		if (scaler->irq <= 0) {
			scaler->is_irq_empty = 1;
			if (scaler->id == vipp_virtual_find_logic[scaler->id])
				vin_err("failed to get vipp%d IRQ resource\n", scaler->id);
		} else {
			ret = hal_request_irq(scaler->irq, scaler_isr, "sunxi_scaler", scaler);
			if (ret != scaler->irq) {
				vin_err("vipp%d request irq failed\n", scaler->id);
				ret = -1;
				goto freedev;
			}
		}
	}
#endif

	if (global_sensors[mipi_sel].sensor_core->sensor_g_format)
		sensor_format = global_sensors[mipi_sel].sensor_core->sensor_g_format(mipi_sel);
	else
		return -1;

	scaler->crop.active.width  = sensor_format->width;
	scaler->crop.active.height = sensor_format->height;
	__scaler_calc_ratios(id, &scaler->crop.active, &scaler->para);

	vin_log(VIN_LOG_SCALER, "scaler%d probe end\n", scaler->id);
	return 0;

freedev:
	hal_free(scaler);
ekzalloc:
	vin_err("scaler%d probe err!\n", scaler->id);
	return ret;
}

int scaler_remove(unsigned int id)
{
	struct scaler_dev *scaler = global_scaler[id];

	if (!scaler_probe_flag[id])
		return 0;
	else
		scaler_probe_flag[id] = false;

	if (scaler->noneed_register == 1) {
		hal_free(scaler);
		return 0;
	}

#ifdef VIPP_REG_MODE
	scaler_resource_free(id);
#endif
#ifdef VIPP_ISR_EN
	if (scaler->id == vipp_virtual_find_logic[scaler->id]) {
		if (!scaler->is_irq_empty)
			hal_free_irq(scaler->irq);
	}
#endif
	hal_free(scaler);
	return 0;
}
