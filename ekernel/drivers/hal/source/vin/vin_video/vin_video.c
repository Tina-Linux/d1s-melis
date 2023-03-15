
/*
 * vin_video.c for video api
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *           Yang Feng <yangfeng@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "../utility/vin_io.h"
#include "../vin_csi/sunxi_csi.h"
#include "../vin_mipi/sunxi_mipi.h"
#include "../vin.h"

extern struct vin_core *global_vinc[VIN_MAX_VIDEO];

int vin_set_addr(unsigned int id, unsigned int phy_addr)
{
	struct vin_core *vinc = global_vinc[id];
	struct vin_addr paddr;

	paddr.y = phy_addr;
	paddr.cb = (unsigned int)(phy_addr + global_video[id].o_width * global_video[id].o_height);
	paddr.cr = 0;

	if (global_video[id].fourcc == V4L2_PIX_FMT_LBC_1_0X || global_video[id].fourcc == V4L2_PIX_FMT_LBC_1_5X ||
			global_video[id].fourcc == V4L2_PIX_FMT_LBC_2_0X || global_video[id].fourcc == V4L2_PIX_FMT_LBC_2_5X) {
		paddr.cb = 0;
		paddr.cr = 0;
	}

	csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_0_A, paddr.y);
	csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_1_A, paddr.cb);
	csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_2_A, paddr.cr);

	return 0;
}

static void vin_detect_buffer_cfg(struct vin_core *vinc)
{
	int sensor_id = vinc->mipi_sel;
	struct isp_autoflash_config_s *isp_autoflash_cfg = NULL;
	unsigned int sign;
	int ret = -1;

	if (sensor_id == 0) {
		isp_autoflash_cfg = (struct isp_autoflash_config_s *)ISP0_NORFLASH_SAVE;
		sign = 0xAA11AA11;
	} else {
		isp_autoflash_cfg = (struct isp_autoflash_config_s *)ISP1_NORFLASH_SAVE;
		sign = 0xBB11BB11;
	}

	if (vinc->get_yuv_en) {
		isp_autoflash_cfg->melisyuv_sign_id = sign;
		isp_autoflash_cfg->melisyuv_paddr = (unsigned int)vinc->buff[0].phy_addr;
		isp_autoflash_cfg->melisyuv_size = vinc->buffer_size;
	} else {
		isp_autoflash_cfg->melisyuv_sign_id = 0xFFFFFFFF;
	}
}

int buffer_queue(unsigned int id)
{
	unsigned int size;
	unsigned int i;
	unsigned int buffer_num;

	size = global_video[id].o_width * global_video[id].o_height * 3 / 2;

#ifdef DMA_USE_IRQ_BUFFER_QUEUE
	if (global_vinc[id]->get_yuv_en)
		buffer_num = 2;
	else
		buffer_num = 1;
	for (i = 0; i < buffer_num; i++) {
		global_vinc[id]->buff[i].phy_addr = rt_memheap_alloc_align(&isp_mempool, size, 0x1000);
		global_vinc[id]->buffer_size = size;
		if (global_vinc[id]->buff[i].phy_addr == NULL) {
			vin_err("%s:video%d:alloc bk buffer%d error\n", __func__, id, i);
			return -1;
		}
		vin_log(VIN_LOG_MD, "video%d: buffer[%d] phy_addr is 0x%x\n", id, i, (unsigned int)global_vinc[id]->buff[i].phy_addr);
		if (global_vinc[id]->get_yuv_en)
			vin_print("video%d: buffer[%d] phy_addr is 0x%x\n", id, i, (unsigned int)global_vinc[id]->buff[i].phy_addr);
	}
	vin_detect_buffer_cfg(global_vinc[id]);
#else
	global_vinc[id]->buff[0].phy_addr = rt_memheap_alloc_align(&isp_mempool, size, 0x1000);
	if (global_vinc[id]->buff[0].phy_addr == NULL) {
		vin_err("%s:video%d:alloc bk buffer error\n", __func__, id);
		return -1;
	}
#endif
	return 0;
}

int buffer_free(unsigned int id)
{
#ifdef DMA_USE_IRQ_BUFFER_QUEUE
	if (global_vinc[id]->get_yuv_en)
		rt_memheap_free_align(global_vinc[id]->buff[1].phy_addr);
	else
		rt_memheap_free_align(global_vinc[id]->buff[0].phy_addr);
#else
	rt_memheap_free_align(global_vinc[id]->buff[0].phy_addr);
#endif
	vin_log(VIN_LOG_VIDEO, "buf free!\n");
	return 0;
}

#if 0
/* The color format (colplanes, memplanes) must be already configured. */
int vin_set_addr(struct vin_core *vinc, struct vb2_buffer *vb,
		      struct vin_frame *frame, struct vin_addr *paddr)
{
	u32 pix_size, depth, y_stride, u_stride, v_stride;

	if (vb == NULL || frame == NULL)
		return -EINVAL;

	pix_size = ALIGN(frame->o_width, VIN_ALIGN_WIDTH) * frame->o_height;

	depth = frame->fmt.depth[0] + frame->fmt.depth[1] + frame->fmt.depth[2];

	paddr->y = vb2_dma_contig_plane_dma_addr(vb, 0);

	if (frame->fmt.memplanes == 1) {
		switch (frame->fmt.colplanes) {
		case 1:
			paddr->cb = 0;
			paddr->cr = 0;
			break;
		case 2:
			/* decompose Y into Y/Cb */

			if (frame->fmt.fourcc == V4L2_PIX_FMT_FBC) {
				paddr->cb = (u32)(paddr->y + CEIL_EXP(frame->o_width, 7) * CEIL_EXP(frame->o_height, 5) * 96);
				paddr->cr = 0;

			} else {
				paddr->cb = (u32)(paddr->y + pix_size);
				paddr->cr = 0;
			}
			break;
		case 3:
			paddr->cb = (u32)(paddr->y + pix_size);
			/* 420 */
			if (12 == frame->fmt.depth[0])
				paddr->cr = (u32)(paddr->cb + (pix_size >> 2));
			/* 422 */
			else
				paddr->cr = (u32)(paddr->cb + (pix_size >> 1));
			break;
		default:
			return -EINVAL;
		}
	} else if (!frame->fmt.mdataplanes) {
		if (frame->fmt.memplanes >= 2)
			paddr->cb = vb2_dma_contig_plane_dma_addr(vb, 1);

		if (frame->fmt.memplanes == 3)
			paddr->cr = vb2_dma_contig_plane_dma_addr(vb, 2);
	}

	if (vinc->vid_cap.frame.fmt.fourcc == V4L2_PIX_FMT_YVU420) {
		csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_0_A, paddr->y);
		csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_2_A, paddr->cb);
		csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_1_A, paddr->cr);
	} else {
		csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_0_A, paddr->y);
		csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_1_A, paddr->cb);
		csic_dma_buffer_address(vinc->vipp_sel, CSI_BUF_2_A, paddr->cr);
	}
	return 0;
}
#endif

static int lbc_mode_select(struct dma_lbc_cmp *lbc_cmp, unsigned int fourcc)
{
	switch (fourcc) {
	case V4L2_PIX_FMT_LBC_2_0X: /* 2x */
		lbc_cmp->is_lossy = 1;
		lbc_cmp->bit_depth = 8;
		lbc_cmp->glb_enable = 1;
		lbc_cmp->dts_enable = 1;
		lbc_cmp->ots_enable = 1;
		lbc_cmp->msq_enable = 1;
		lbc_cmp->cmp_ratio_even = 600;
		lbc_cmp->cmp_ratio_odd  = 450;
		lbc_cmp->mb_mi_bits[0]  = 55;
		lbc_cmp->mb_mi_bits[1]  = 110;
		lbc_cmp->rc_adv[0] = 60;
		lbc_cmp->rc_adv[1] = 30;
		lbc_cmp->rc_adv[2] = 15;
		lbc_cmp->rc_adv[3] = 8;
		lbc_cmp->lmtqp_en  = 1;
		lbc_cmp->lmtqp_min = 1;
		lbc_cmp->updata_adv_en = 1;
		lbc_cmp->updata_adv_ratio = 2;
		break;
	case V4L2_PIX_FMT_LBC_1_5X: /* 1.5x */
		lbc_cmp->is_lossy = 1;
		lbc_cmp->bit_depth = 8;
		lbc_cmp->glb_enable = 1;
		lbc_cmp->dts_enable = 1;
		lbc_cmp->ots_enable = 1;
		lbc_cmp->msq_enable = 1;
		lbc_cmp->cmp_ratio_even = 670;
		lbc_cmp->cmp_ratio_odd	= 658;
		lbc_cmp->mb_mi_bits[0]	= 87;
		lbc_cmp->mb_mi_bits[1]	= 167;
		lbc_cmp->rc_adv[0] = 60;
		lbc_cmp->rc_adv[1] = 30;
		lbc_cmp->rc_adv[2] = 15;
		lbc_cmp->rc_adv[3] = 8;
		lbc_cmp->lmtqp_en  = 1;
		lbc_cmp->lmtqp_min = 1;
		lbc_cmp->updata_adv_en = 1;
		lbc_cmp->updata_adv_ratio = 2;
		break;
	case V4L2_PIX_FMT_LBC_2_5X: /* 2.5x */
		lbc_cmp->is_lossy = 1;
		lbc_cmp->bit_depth = 8;
		lbc_cmp->glb_enable = 1;
		lbc_cmp->dts_enable = 1;
		lbc_cmp->ots_enable = 1;
		lbc_cmp->msq_enable = 1;
		lbc_cmp->cmp_ratio_even = 440;
		lbc_cmp->cmp_ratio_odd  = 380;
		lbc_cmp->mb_mi_bits[0]  = 55;
		lbc_cmp->mb_mi_bits[1]  = 94;
		lbc_cmp->rc_adv[0] = 60;
		lbc_cmp->rc_adv[1] = 30;
		lbc_cmp->rc_adv[2] = 15;
		lbc_cmp->rc_adv[3] = 8;
		lbc_cmp->lmtqp_en  = 1;
		lbc_cmp->lmtqp_min = 1;
		lbc_cmp->updata_adv_en = 1;
		lbc_cmp->updata_adv_ratio = 2;
		break;
	case V4L2_PIX_FMT_LBC_1_0X: /* lossless */
		lbc_cmp->is_lossy = 0;
		lbc_cmp->bit_depth = 8;
		lbc_cmp->glb_enable = 1;
		lbc_cmp->dts_enable = 1;
		lbc_cmp->ots_enable = 1;
		lbc_cmp->msq_enable = 1;
		lbc_cmp->cmp_ratio_even = 1000;
		lbc_cmp->cmp_ratio_odd  = 1000;
		lbc_cmp->mb_mi_bits[0]  = 55;
		lbc_cmp->mb_mi_bits[1]  = 94;
		lbc_cmp->rc_adv[0] = 60;
		lbc_cmp->rc_adv[1] = 30;
		lbc_cmp->rc_adv[2] = 15;
		lbc_cmp->rc_adv[3] = 8;
		lbc_cmp->lmtqp_en  = 1;
		lbc_cmp->lmtqp_min = 1;
		lbc_cmp->updata_adv_en = 1;
		lbc_cmp->updata_adv_ratio = 2;
		break;
	default:
		return -1;
	}
	return 0;
}

static int vin_subdev_logic_s_stream(unsigned char virtual_id, int on)
{
	unsigned char logic_id = dma_virtual_find_logic[virtual_id];
	struct vin_core *logic_vinc = global_vinc[logic_id];

	if (logic_vinc->work_mode == BK_ONLINE && virtual_id != logic_id) {
		vin_err("video%d work on online mode, video%d cannot to work!!\n", logic_id, virtual_id);
		return -1;
	}

	if (on && (logic_vinc->logic_top_stream_count)++ > 0)
		return 0;
	else if (!on && (logic_vinc->logic_top_stream_count == 0 || --(logic_vinc->logic_top_stream_count) > 0))
		return 0;

	if (on) {
		csic_dma_top_enable(logic_id);
		csic_dma_mul_ch_enable(logic_id, logic_vinc->work_mode);
		//if (logic_vinc->id == CSI_VE_ONLINE_VIDEO && logic_vinc->ve_online_cfg.ve_online_en) {
		//	csic_ve_online_hs_enable(logic_id);
		//	logic_vinc->ve_ol_ch = CSI_VE_ONLINE_VIDEO;
		//	csic_ve_online_ch_sel(logic_id, logic_vinc->ve_ol_ch);
		//}
		csic_dma_buf_length_software_enable(logic_vinc->vipp_sel, 0);
		csi_dam_flip_software_enable(logic_vinc->vipp_sel, 0);
		//csic_dma_top_interrupt_en(logic_id, VIDEO_INPUT_TO_INT | CLR_FS_FRM_CNT_INT | FS_PUL_INT);//for debug
		hal_enable_irq(logic_vinc->irq);
	} else {
		//csic_dma_top_interrupt_disable(logic_id, DMA_TOP_INT_ALL);
		hal_disable_irq(logic_vinc->irq);
		csic_ve_online_hs_disable(logic_id);
		csic_dma_top_disable(logic_id);
	}
	vin_log(VIN_LOG_FMT, "dma%d top init by video%d, %s\n", logic_id, virtual_id, on ? "steram on" : "steam off");
	return 0;
}

int vin_subdev_s_stream(unsigned int id, int enable)
{
	struct vin_core *vinc = global_vinc[id];
	struct csic_dma_cfg cfg;
	struct csic_dma_flip flip;
	struct dma_output_size size;
	struct dma_buf_len buf_len;
	struct dma_flip_size flip_size;
	struct dma_lbc_cmp lbc_cmp;
	int flag = 0;
	int flip_mul = 2;
	int wth;
	int *stream_count;
	struct sensor_format_struct *sensor_format;

	if (global_sensors[vinc->mipi_sel].sensor_core->sensor_g_format)
		sensor_format = global_sensors[vinc->mipi_sel].sensor_core->sensor_g_format(vinc->mipi_sel);
	else
		return -1;

	stream_count = &vinc->stream_count;
	if (enable && (*stream_count)++ > 0)
		return 0;
	else if (!enable && (*stream_count == 0 || --(*stream_count) > 0))
		return 0;

	vin_log(VIN_LOG_FMT, "csic_dma%d %s, %d*%d hoff: %d voff: %d\n",
		vinc->id, enable ? "stream on" : "stream off",
		global_video[id].o_width, global_video[id].o_height,
		sensor_format->offs_h, sensor_format->offs_v);

	if (enable) {
		memset(&cfg, 0, sizeof(cfg));
		memset(&size, 0, sizeof(size));
		memset(&buf_len, 0, sizeof(buf_len));

		if (global_video[id].fourcc == V4L2_PIX_FMT_LBC_1_0X || global_video[id].fourcc == V4L2_PIX_FMT_LBC_2_0X
			|| global_video[id].fourcc == V4L2_PIX_FMT_LBC_2_5X) {
			lbc_mode_select(&lbc_cmp, global_video[id].fourcc);
			wth = roundup(global_video[id].o_width, 32);
			if (lbc_cmp.is_lossy) {
				lbc_cmp.line_tar_bits[0] = roundup(lbc_cmp.cmp_ratio_even * wth * lbc_cmp.bit_depth/1000, 512);
				lbc_cmp.line_tar_bits[1] = roundup(lbc_cmp.cmp_ratio_odd * wth * lbc_cmp.bit_depth/500, 512);
			} else {
				lbc_cmp.line_tar_bits[0] = roundup(wth * lbc_cmp.bit_depth * 1 + (wth * 1 / 16 * 2), 512);
				lbc_cmp.line_tar_bits[1] = roundup(wth * lbc_cmp.bit_depth * 2 + (wth * 2 / 16 * 2), 512);
			}
		}

		switch (sensor_format->field) {
		case V4L2_FIELD_ANY:
		case V4L2_FIELD_NONE:
			cfg.field = FIELD_EITHER;
			break;
		case V4L2_FIELD_TOP:
			cfg.field = FIELD_1;
			flag = 1;
			break;
		case V4L2_FIELD_BOTTOM:
			cfg.field = FIELD_2;
			flag = 1;
			break;
		case V4L2_FIELD_INTERLACED:
			cfg.field = FIELD_EITHER;
			flag = 1;
			break;
		default:
			cfg.field = FIELD_EITHER;
			break;
		}

		switch (global_video[id].fourcc) {
		case V4L2_PIX_FMT_NV12:
		case V4L2_PIX_FMT_NV12M:
		case V4L2_PIX_FMT_FBC:
			cfg.fmt = flag ? FRAME_UV_CB_YUV420 : FIELD_UV_CB_YUV420;
			buf_len.buf_len_y = global_video[id].o_width;
			buf_len.buf_len_c = buf_len.buf_len_y;
			break;
		case V4L2_PIX_FMT_LBC_2_0X:
		case V4L2_PIX_FMT_LBC_2_5X:
		case V4L2_PIX_FMT_LBC_1_5X:
		case V4L2_PIX_FMT_LBC_1_0X:
			cfg.fmt = LBC_MODE_OUTPUT;
			buf_len.buf_len_y = lbc_cmp.line_tar_bits[1] >> 3;
			buf_len.buf_len_c = lbc_cmp.line_tar_bits[0] >> 3;
			break;
		case V4L2_PIX_FMT_NV21:
		case V4L2_PIX_FMT_NV21M:
			cfg.fmt = flag ? FRAME_VU_CB_YUV420 : FIELD_VU_CB_YUV420;
			buf_len.buf_len_y = global_video[id].o_width;
			buf_len.buf_len_c = buf_len.buf_len_y;
			break;
		case V4L2_PIX_FMT_YVU420:
		case V4L2_PIX_FMT_YUV420:
		case V4L2_PIX_FMT_YUV420M:
			cfg.fmt = flag ? FRAME_PLANAR_YUV420 : FIELD_PLANAR_YUV420;
			buf_len.buf_len_y = global_video[id].o_width;
			buf_len.buf_len_c = buf_len.buf_len_y >> 1;
			break;
		case V4L2_PIX_FMT_GREY:
			cfg.fmt = flag ? FRAME_CB_YUV400 : FIELD_CB_YUV400;
			buf_len.buf_len_y = global_video[id].o_width;
			break;
		case V4L2_PIX_FMT_YUV422P:
			cfg.fmt = flag ? FRAME_PLANAR_YUV422 : FIELD_PLANAR_YUV422;
			buf_len.buf_len_y = global_video[id].o_width;
			buf_len.buf_len_c = buf_len.buf_len_y >> 1;
			break;
		case V4L2_PIX_FMT_NV61:
		case V4L2_PIX_FMT_NV61M:
			cfg.fmt = flag ? FRAME_VU_CB_YUV422 : FIELD_VU_CB_YUV422;
			buf_len.buf_len_y = global_video[id].o_width;
			buf_len.buf_len_c = buf_len.buf_len_y;
			break;
		case V4L2_PIX_FMT_NV16:
		case V4L2_PIX_FMT_NV16M:
			cfg.fmt = flag ? FRAME_UV_CB_YUV422 : FIELD_UV_CB_YUV422;
			buf_len.buf_len_y = global_video[id].o_width;
			buf_len.buf_len_c = buf_len.buf_len_y;
			break;
		case V4L2_PIX_FMT_SBGGR8:
		case V4L2_PIX_FMT_SGBRG8:
		case V4L2_PIX_FMT_SGRBG8:
		case V4L2_PIX_FMT_SRGGB8:
			flip_mul = 1;
			cfg.fmt = flag ? FRAME_RAW_8 : FIELD_RAW_8;
			buf_len.buf_len_y = global_video[id].o_width;
			buf_len.buf_len_c = buf_len.buf_len_y;
			break;
		case V4L2_PIX_FMT_SBGGR10:
		case V4L2_PIX_FMT_SGBRG10:
		case V4L2_PIX_FMT_SGRBG10:
		case V4L2_PIX_FMT_SRGGB10:
			flip_mul = 1;
			cfg.fmt = flag ? FRAME_RAW_10 : FIELD_RAW_10;
			buf_len.buf_len_y = global_video[id].o_width * 2;
			buf_len.buf_len_c = buf_len.buf_len_y;
			break;
		case V4L2_PIX_FMT_SBGGR12:
		case V4L2_PIX_FMT_SGBRG12:
		case V4L2_PIX_FMT_SGRBG12:
		case V4L2_PIX_FMT_SRGGB12:
			flip_mul = 1;
			cfg.fmt = flag ? FRAME_RAW_12 : FIELD_RAW_12;
			buf_len.buf_len_y = global_video[id].o_width * 2;
			buf_len.buf_len_c = buf_len.buf_len_y;
			break;
		default:
			cfg.fmt = flag ? FRAME_UV_CB_YUV420 : FIELD_UV_CB_YUV420;
			buf_len.buf_len_y = global_video[id].o_width;
			buf_len.buf_len_c = buf_len.buf_len_y;
			break;
		}

		if (vinc->isp_dbg.debug_en) {
			buf_len.buf_len_y = 0;
			buf_len.buf_len_c = 0;
		}

		cfg.ds = vinc->fps_ds;
		vinc->frame_cnt = 0;

		if (vin_subdev_logic_s_stream(vinc->id, enable))
			return -1;

		csic_dma_config(vinc->vipp_sel, &cfg);
		size.hor_len = vinc->isp_dbg.debug_en ? 0 : global_video[id].o_width;
		size.ver_len = vinc->isp_dbg.debug_en ? 0 : global_video[id].o_height;
		size.hor_start = vinc->isp_dbg.debug_en ? 0 : sensor_format->offs_h;
		size.ver_start = vinc->isp_dbg.debug_en ? 0 : sensor_format->offs_v;
		flip_size.hor_len = vinc->isp_dbg.debug_en ? 0 : global_video[id].o_width * flip_mul;
		flip_size.ver_len = vinc->isp_dbg.debug_en ? 0 : global_video[id].o_height;
		flip.hflip_en = vinc->hflip;
		flip.vflip_en = vinc->vflip;

		csic_dma_output_size_cfg(vinc->vipp_sel, &size);

		csic_dma_buffer_length(vinc->vipp_sel, &buf_len);
		csic_dma_flip_size(vinc->vipp_sel, &flip_size);


		buffer_queue(id);
		//vin_set_addr(id, (unsigned int)buff[id].phy_addr);
		vin_set_addr(id, (unsigned int)vinc->buff[0].phy_addr);

#ifdef DMA_USE_IRQ_BUFFER_QUEUE
		csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_ALL);
		csic_dma_int_enable(vinc->vipp_sel, DMA_INT_BUF_0_OVERFLOW | DMA_INT_HBLANK_OVERFLOW |
			DMA_INT_VSYNC_TRIG | DMA_INT_CAPTURE_DONE | DMA_INT_FRAME_DONE | DMA_INT_LBC_HB);
		//csic_dma_int_enable(vinc->vipp_sel, DMA_INT_ADDR_NO_READY | DMA_INT_ADDR_OVERFLOW);
#endif

		switch (global_video[id].fourcc) {
		case V4L2_PIX_FMT_LBC_2_0X:
		case V4L2_PIX_FMT_LBC_2_5X:
		case V4L2_PIX_FMT_LBC_1_5X:
		case V4L2_PIX_FMT_LBC_1_0X:
			csic_lbc_enable(vinc->vipp_sel);
			csic_lbc_cmp_ratio(vinc->vipp_sel, &lbc_cmp);
			break;
		default:
			csic_dma_flip_en(vinc->vipp_sel, &flip);
			csic_dma_enable(vinc->vipp_sel);
			break;
		}
	} else {
#ifdef DMA_USE_IRQ_BUFFER_QUEUE
		csic_dma_int_disable(vinc->vipp_sel, DMA_INT_ALL);
		csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_ALL);
#endif
		switch (global_video[id].fourcc) {
		case V4L2_PIX_FMT_FBC:
			csic_fbc_disable(vinc->vipp_sel);
			break;
		case V4L2_PIX_FMT_LBC_2_0X:
		case V4L2_PIX_FMT_LBC_2_5X:
		case V4L2_PIX_FMT_LBC_1_5X:
		case V4L2_PIX_FMT_LBC_1_0X:
			csic_lbc_disable(vinc->vipp_sel);
			break;
		default:
			csic_dma_disable(vinc->vipp_sel);
			break;
		}
		vin_subdev_logic_s_stream(vinc->id, enable);

		//buffer_free(id);
	}

	return 0;
}


