/*
 * linux-4.9/drivers/media/platform/sunxi-vin/vin-tdm/vin_tdm.c
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 *
 * Authors:  Zheng Zequn <zequnzheng@allwinnertech.com>
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

#include <hal_mem.h>
#include <hal_interrupt.h>
#include <hal_timer.h>

#include "vin_tdm.h"
#include "../vin_video/vin_video.h"

struct tdm_dev *glb_tdm[VIN_MAX_TDM];
bool tdm_probe_flag[VIN_MAX_TDM];
bool tdm_rx_wdr_cfg[TDM_RX_NUM];

#if 1
#define TDM_LBC_RATIO			393     /*1.0x:1024; 1.5x:683; 2.0x 512; 2.6x:393; 3x:342; 3.5x:293; 4x:256*/
#else
#define TDM_LBC_RATIO			1024
#endif

static struct tdm_format sunxi_tdm_formats[] = {
	{
		.code = MEDIA_BUS_FMT_SBGGR8_1X8,
		.input_type = INPUTTPYE_8BIT,
		.input_bit_width = RAW_8BIT,
		.raw_fmt = BAYER_BGGR,
	}, {
		.code = MEDIA_BUS_FMT_SGBRG8_1X8,
		.input_type = INPUTTPYE_8BIT,
		.input_bit_width = RAW_8BIT,
		.raw_fmt = BAYER_GBRG,
	}, {
		.code = MEDIA_BUS_FMT_SGRBG8_1X8,
		.input_type = INPUTTPYE_8BIT,
		.input_bit_width = RAW_8BIT,
		.raw_fmt = BAYER_GRBG,
	}, {
		.code = MEDIA_BUS_FMT_SRGGB8_1X8,
		.input_type = INPUTTPYE_8BIT,
		.input_bit_width = RAW_8BIT,
		.raw_fmt = BAYER_RGGB,
	}, {
		.code = MEDIA_BUS_FMT_SBGGR10_1X10,
		.input_type = INPUTTPYE_10BIT,
		.input_bit_width = RAW_10BIT,
		.raw_fmt = BAYER_BGGR,
	}, {
		.code = MEDIA_BUS_FMT_SGBRG10_1X10,
		.input_type = INPUTTPYE_10BIT,
		.input_bit_width = RAW_10BIT,
		.raw_fmt = BAYER_GBRG,
	}, {
		.code = MEDIA_BUS_FMT_SGRBG10_1X10,
		.input_type = INPUTTPYE_10BIT,
		.input_bit_width = RAW_10BIT,
		.raw_fmt = BAYER_GRBG,
	}, {
		.code = MEDIA_BUS_FMT_SRGGB10_1X10,
		.input_type = INPUTTPYE_10BIT,
		.input_bit_width = RAW_10BIT,
		.raw_fmt = BAYER_RGGB,
	}, {
		.code = MEDIA_BUS_FMT_SBGGR12_1X12,
		.input_type = INPUTTPYE_12BIT,
		.input_bit_width = RAW_12BIT,
		.raw_fmt = BAYER_BGGR,
	}, {
		.code = MEDIA_BUS_FMT_SGBRG12_1X12,
		.input_type = INPUTTPYE_12BIT,
		.input_bit_width = RAW_12BIT,
		.raw_fmt = BAYER_GBRG,
	}, {
		.code = MEDIA_BUS_FMT_SGRBG12_1X12,
		.input_type = INPUTTPYE_12BIT,
		.input_bit_width = RAW_12BIT,
		.raw_fmt = BAYER_GRBG,
	}, {
		.code = MEDIA_BUS_FMT_SRGGB12_1X12,
		.input_type = INPUTTPYE_12BIT,
		.input_bit_width = RAW_12BIT,
		.raw_fmt = BAYER_RGGB,
	},
};

static struct tdm_format *__tdm_try_format(unsigned int id)
{
	struct tdm_format *tdm_fmt = NULL;
	struct sensor_format_struct *sensor_format;
	struct tdm_dev *tdm = glb_tdm[id/TDM_RX_NUM];
	struct tdm_rx_dev *tdm_rx = &tdm->tdm_rx[id];
	int i;
	int mipi_sel = global_video[id].mipi_sel;

	if (global_sensors[mipi_sel].sensor_core->sensor_g_format)
		sensor_format = global_sensors[mipi_sel].sensor_core->sensor_g_format(mipi_sel);
	else
		return NULL;

	for (i = 0; i < ARRAY_SIZE(sunxi_tdm_formats); i++)
		if (sensor_format->mbus_code == sunxi_tdm_formats[i].code)
			tdm_fmt = &sunxi_tdm_formats[i];

	if (tdm_fmt == NULL)
		tdm_fmt = &sunxi_tdm_formats[0];

	sensor_format->mbus_code = tdm_fmt->code;
	tdm_rx->width = sensor_format->width;
	tdm_rx->height = sensor_format->height;

	return tdm_fmt;
}

static void tdm_set_tx_blank(struct tdm_dev *tdm)
{
	csic_tdm_omode(tdm->id, 1);
	if (tdm->work_mode == TDM_OFFLINE)
		csic_tdm_set_hblank(tdm->id, TDM_TX_HBLANK_OFFLINE);
	else
		csic_tdm_set_hblank(tdm->id, TDM_TX_HBLANK);
	csic_tdm_set_bblank_fe(tdm->id, TDM_TX_VBLANK/2);
	csic_tdm_set_bblank_be(tdm->id, TDM_TX_VBLANK/2);
}

#if 0
static void tdm_rx_bufs_free(struct tdm_rx_dev *tdm_rx)
{
	struct tdm_dev *tdm = container_of(tdm_rx, struct tdm_dev, tdm_rx[tdm_rx->id]);
	int i;

	if (tdm->tdm_rx_buf_en[tdm_rx->id] == 0)
		return;

	if (tdm_rx->buf_cnt == 0)
		return;

	for (i = 0; i < tdm_rx->buf_cnt; i++) {
		struct tdm_buffer *buf = &tdm_rx->buf[i];
		struct vin_mm *mm = &tdm_rx->ion_man[i];

		mm->size = tdm_rx->buf_size;
		if (!buf->virt_addr)
			continue;

		mm->vir_addr = buf->virt_addr;
		mm->dma_addr = buf->dma_addr;
		os_mem_free(&tdm->pdev->dev, mm);

		buf->dma_addr = NULL;
		buf->virt_addr = NULL;
	}

	vin_log(VIN_LOG_TDM, "%s: all buffers were freed.\n", tdm_rx->subdev.name);

	tdm->tdm_rx_buf_en[tdm_rx->id] = 0;
	tdm_rx->buf_size = 0;
	tdm_rx->buf_cnt = 0;
}

static int tdm_rx_bufs_alloc(struct tdm_rx_dev *tdm_rx, u32 size, u32 count)
{
	struct tdm_dev *tdm = container_of(tdm_rx, struct tdm_dev, tdm_rx[tdm_rx->id]);
	int i;

	if (tdm->tdm_rx_buf_en[tdm_rx->id] == 1)
		return 0;

	tdm_rx->buf_size = size;
	tdm_rx->buf_cnt = count;

	for (i = 0; i < tdm_rx->buf_cnt; i++) {
		struct tdm_buffer *buf = &tdm_rx->buf[i];
		struct vin_mm *mm = &tdm_rx->ion_man[i];

		mm->size = size;
		if (!os_mem_alloc(&tdm->pdev->dev, mm)) {
			buf->virt_addr = mm->vir_addr;
			buf->dma_addr = mm->dma_addr;
		}
		if (!buf->virt_addr || !buf->dma_addr) {
			vin_err("%s: Can't acquire memory for DMA buffer %d\n",	tdm_rx->subdev.name, i);
			tdm_rx_bufs_free(tdm_rx);
			return -ENOMEM;
		}
		/*vin_print("tx%d:buf%d:dma_addr is 0x%p ~ 0x%p\n", tdm_rx->id, i, buf->dma_addr, buf->dma_addr + size);*/
	}

	vin_log(VIN_LOG_TDM, "%s: allocing buffers successfully, buf_cnt and size is %d and %d.\n",
				tdm_rx->subdev.name, tdm_rx->buf_cnt, tdm_rx->buf_size);

	tdm->tdm_rx_buf_en[tdm_rx->id] = 1;
	return 0;
}
#endif
static void tdm_rx_lbc_cal_para(struct tdm_rx_dev *tdm_rx)
{
	struct tdm_rx_lbc *lbc = &tdm_rx->lbc;
	unsigned int width_stride;
	unsigned int bit_depth;
	unsigned int mb_max_bits_ratio = 204;/*default*/
	unsigned int mb_min_bits_ratio = 820;/*default*/
	unsigned int align = 256;
	unsigned int mb_len = 48;

	if (TDM_LBC_RATIO != 1024 && tdm_rx->tdm_fmt->input_bit_width == RAW_10BIT)
		lbc->cmp_ratio = 293;/*3.5x*/
	else
		lbc->cmp_ratio = clamp(TDM_LBC_RATIO, 256, 1024);

	if (tdm_rx->tdm_fmt->input_bit_width == RAW_12BIT) {
		if (lbc->cmp_ratio >= 682)
			lbc->start_qp = 1;
		else if (lbc->cmp_ratio >= 512)
			lbc->start_qp = 2;
		else
			lbc->start_qp = 3;
	} else {
		if (lbc->cmp_ratio > 512)
			lbc->start_qp = 0;
		else
			lbc->start_qp = 1;
	}

	if (lbc->cmp_ratio == 1024)
		lbc->is_lossy = 0;
	else
		lbc->is_lossy = 1;

	lbc->mb_th = 0x18;/*12, 24, 36, 48*/
	lbc->mb_num = clamp((int)DIV_ROUND_UP(tdm_rx->width, mb_len), 3, 88);

	bit_depth = 12;
	width_stride = ALIGN(tdm_rx->width, 48);
	if (lbc->is_lossy) {
		lbc->line_tar_bits = roundup((lbc->cmp_ratio * width_stride * bit_depth) / 1024, align);
		lbc->line_tar_bits = min(lbc->line_tar_bits, roundup(width_stride * bit_depth, align));
	} else
		lbc->line_tar_bits = roundup(width_stride / 24 * (bit_depth * 24 + 24 + 2), align);

	if (lbc->is_lossy) {
		lbc->line_max_bit = roundup((mb_max_bits_ratio + 1024) * lbc->line_tar_bits / 1024, align);
		lbc->line_max_bit = min(lbc->line_max_bit, roundup(width_stride * bit_depth, align));
	} else
		lbc->line_max_bit = roundup(width_stride / 24 * (bit_depth * 24 + 24 + 2), align);
	lbc->mb_min_bit = clamp((int)((lbc->cmp_ratio * bit_depth * mb_len) / 1024 / 2 * mb_min_bits_ratio / 1024), 36, 288);
	lbc->gbgr_ratio = 0x200;
}

static void tdm_rx_cpy(struct tdm_rx_dev *source, struct tdm_rx_dev *rx)
{
	if (!source || !rx)
		return;

	memcpy(rx->tdm_fmt, source->tdm_fmt, sizeof(struct tdm_format));
	rx->buf_size = source->buf_size;
	rx->buf_cnt = source->buf_cnt;
	rx->ws.wdr_mode = source->ws.wdr_mode;
	rx->width = source->width;
	rx->height = source->height;
}

static int tdm_set_rx_cfg(struct tdm_rx_dev *tdm_rx, unsigned int en)
{
	struct tdm_dev *tdm = glb_tdm[tdm_rx->id/TDM_RX_NUM];
	u32 size;
	u16 rx_pkg_line_words = 0;
	int ret, i;
	unsigned int tdm_buf_num;

	if (en) {
		csic_tdm_rx_set_min_ddr_size(tdm->id, tdm_rx->id, DDRSIZE_512b);
		csic_tdm_rx_input_bit(tdm->id, tdm_rx->id, tdm_rx->tdm_fmt->input_type);
		csic_tdm_rx_input_fmt(tdm->id, tdm_rx->id, tdm_rx->tdm_fmt->raw_fmt);
		csic_tdm_rx_input_size(tdm->id, tdm_rx->id, tdm_rx->width, tdm_rx->height);

		if (tdm->work_mode == TDM_ONLINE) {
			csic_tdm_rx_data_fifo_depth(tdm->id, tdm_rx->id, tdm->tx_cfg.data_depth);
			csic_tdm_rx_head_fifo_depth(tdm->id, tdm_rx->id, tdm->tx_cfg.head_depth);
		} else {
			if (tdm_rx->ws.wdr_mode == ISP_DOL_WDR_MODE) {
				csic_tdm_rx_data_fifo_depth(tdm->id, tdm_rx->id, (512)/4);
				csic_tdm_rx_head_fifo_depth(tdm->id, tdm_rx->id, (32)/4);
			} else {
				csic_tdm_rx_data_fifo_depth(tdm->id, tdm_rx->id, 512/2);
				csic_tdm_rx_head_fifo_depth(tdm->id, tdm_rx->id, 32/2);
			}
		}
		if (tdm_rx->ws.pkg_en) {
			csic_tdm_rx_pkg_enable(tdm->id, tdm_rx->id);
			rx_pkg_line_words = DIV_ROUND_UP(tdm_rx->tdm_fmt->input_bit_width * tdm_rx->width, 32);
			csic_tdm_rx_pkg_line_words(tdm->id, tdm_rx->id, ALIGN(rx_pkg_line_words, 8));
		} else if (tdm_rx->ws.lbc_en) {
			if (tdm_rx->id != 0) {
				vin_err("Only tdm_rx0 support lbc!, set to ptg\n");
				csic_tdm_rx_pkg_enable(tdm->id, tdm_rx->id);
				rx_pkg_line_words = DIV_ROUND_UP(tdm_rx->tdm_fmt->input_bit_width * tdm_rx->width, 32);
				csic_tdm_rx_pkg_line_words(tdm->id, tdm_rx->id, ALIGN(rx_pkg_line_words, 8));
			} else {
				csic_tdm_rx_lbc_enable(tdm->id, tdm_rx->id);
				tdm_rx_lbc_cal_para(tdm_rx);
				rx_pkg_line_words = roundup(tdm_rx->lbc.line_max_bit + tdm_rx->lbc.mb_min_bit, 512) / 32;
				csic_tdm_rx_pkg_line_words(tdm->id, tdm_rx->id, ALIGN(rx_pkg_line_words, 8));
				csic_tdm_lbc_cfg(tdm->id, tdm_rx->id, &tdm_rx->lbc);

				if (tdm->work_mode == TDM_OFFLINE)
					tdm_rx->ws.line_num_ddr_en = 1;
				else
					tdm_rx->ws.line_num_ddr_en = 0;
				csic_tdm_rx_set_line_num_ddr(tdm->id, tdm_rx->id, tdm_rx->ws.line_num_ddr_en);
			}
		} else if (tdm_rx->ws.sync_en) {
			csic_tdm_rx_sync_enable(tdm->id, tdm_rx->id);
		}

		if (tdm->work_mode == TDM_ONLINE) {
			if (tdm_rx->ws.pkg_en || tdm_rx->ws.lbc_en)
				tdm_buf_num = 1;
			else
				tdm_buf_num = 0;
		} else {
			if (tdm_rx->sensor_fps <= 30)
				tdm_buf_num = 2;
			else if (tdm_rx->sensor_fps <= 60)
				tdm_buf_num = 3;
			else if (tdm_rx->sensor_fps <= 120)
				tdm_buf_num = 4;
			else
				tdm_buf_num = TDM_BUFS_NUM;
		}
		if (tdm_buf_num) {
			if (tdm_rx->ws.pkg_en)
				size = ALIGN(tdm_rx->width * tdm_rx->tdm_fmt->input_bit_width, 512) * tdm_rx->height / 8 + ALIGN(tdm_rx->height, 64);
			else if (tdm_rx->ws.lbc_en)
				size = ALIGN(rx_pkg_line_words, 8) * tdm_rx->height * 4 + ALIGN(tdm_rx->height, 64);
			else
				size = 0;
			//ret = tdm_rx_bufs_alloc(tdm_rx, size, tdm_buf_num);
			//if (ret)
			//	return ret;
			tdm_rx->buf_cnt = tdm_buf_num;
			tdm_rx->buf_size = size;
			if (size) {
				csic_tdm_rx_set_buf_num(tdm->id, tdm_rx->id, tdm_buf_num - 1);
				for (i = 0; i < tdm_buf_num; i++) {
					tdm_rx->buf[i].dma_addr = rt_memheap_alloc_align(&isp_mempool, size, 0x1000);
					if (tdm_rx->buf[i].dma_addr == NULL) {
						vin_err("%s:rx%d:alloc tdm buffer%d error\n", __func__, tdm_rx->id, i);
						return -1;
					}
					csic_tdm_rx_set_address(tdm->id, tdm_rx->id, (unsigned long)tdm_rx->buf[i].dma_addr);
				}
			}
		} else
			csic_tdm_rx_set_buf_num(tdm->id, tdm_rx->id, 0);

		if (tdm_rx->ws.tx_func_en)
			csic_tdm_rx_tx_enable(tdm->id, tdm_rx->id);
		else
			csic_tdm_rx_tx_disable(tdm->id, tdm_rx->id);
	} else {
		csic_tdm_rx_tx_disable(tdm->id, tdm_rx->id);
		csic_tdm_rx_pkg_disable(tdm->id, tdm_rx->id);
		csic_tdm_rx_lbc_disable(tdm->id, tdm_rx->id);
		csic_tdm_rx_sync_disable(tdm->id, tdm_rx->id);
		csic_tdm_rx_set_line_num_ddr(tdm->id, tdm_rx->id, 0);
	}

	vin_log(VIN_LOG_TDM, "tdm%d set rx%d tx en OK!\n", tdm->id, tdm_rx->id);
	return 0;
}

static int sunxi_tdm_top_s_stream(struct tdm_rx_dev *rx, int enable)
{
	struct tdm_dev *tdm = glb_tdm[rx->id/TDM_RX_NUM];
	unsigned int ini_en = 0;

	if (tdm->work_mode == TDM_ONLINE && rx->id != 0) {
		vin_err("tdm%d work on online mode, tdm_rx%d cannot to work!!\n", tdm->id, rx->id);
		return -1;
	}

	if (enable && (tdm->stream_cnt)++ > 0) {
		if (rx->id == 0) {
			if (tdm_rx_wdr_cfg[rx->id] == ISP_DOL_WDR_MODE) {
				if (tdm_rx_wdr_cfg[2] == ISP_DOL_WDR_MODE)
					tdm->ws.rx_chn_mode = WDR_2Fx2;
				else
					tdm->ws.rx_chn_mode = WDR_2F_AND_LINEARx2;
			} else
				tdm->ws.rx_chn_mode = LINEARx4;
		} else if (rx->id == 2) {
			if (tdm_rx_wdr_cfg[rx->id] == ISP_DOL_WDR_MODE)
				tdm->ws.rx_chn_mode = WDR_2Fx2;
			else if (tdm_rx_wdr_cfg[0] == ISP_DOL_WDR_MODE)
				tdm->ws.rx_chn_mode = WDR_2F_AND_LINEARx2;
			else
				tdm->ws.rx_chn_mode = LINEARx4;
		} else {
			if (tdm_rx_wdr_cfg[0] == ISP_DOL_WDR_MODE)
				tdm->ws.rx_chn_mode = WDR_2F_AND_LINEARx2;
			else
				tdm->ws.rx_chn_mode = LINEARx4;
		}
		csic_tdm_set_rx_chn_cfg_mode(tdm->id, tdm->ws.rx_chn_mode);
		return 0;
	} else if (!enable && (tdm->stream_cnt == 0 || --(tdm->stream_cnt) > 0))
		return 0;

	if (enable) {
		csic_tdm_top_enable(tdm->id);
		if (tdm->ws.tdm_en)
			csic_tdm_enable(tdm->id);
		if (rx->id == 0) {
			if (tdm_rx_wdr_cfg[rx->id] == ISP_DOL_WDR_MODE)
				tdm->ws.rx_chn_mode = WDR_2F_AND_LINEARx2;
			else
				tdm->ws.rx_chn_mode = LINEARx4;
		} else if (rx->id == 2) {
			if (tdm_rx_wdr_cfg[rx->id] == ISP_DOL_WDR_MODE)
				tdm->ws.rx_chn_mode = WDR_2Fx2;
			else
				tdm->ws.rx_chn_mode = LINEARx4;
		} else {
			tdm->ws.rx_chn_mode = LINEARx4;
		}
		vin_log(VIN_LOG_TDM, "%s rx mode %d, tx mode %d, work mode %ld\n",
			__func__, tdm->ws.rx_chn_mode, tdm->ws.tx_chn_mode, tdm->work_mode);
		csic_tdm_set_rx_chn_cfg_mode(tdm->id, tdm->ws.rx_chn_mode);
		csic_tdm_set_tx_chn_cfg_mode(tdm->id, tdm->ws.tx_chn_mode);
		csic_tdm_set_work_mode(tdm->id, tdm->work_mode);
		/*if (tdm->work_mode == TDM_OFFLINE)
			tdm->ws.speed_dn_en = 1;*/
		csic_tdm_set_speed_dn(tdm->id, tdm->ws.speed_dn_en);

		/*tx init*/
		tdm_set_tx_blank(tdm);
		csic_tdm_set_tx_t1_cycle(tdm->id, tdm->tx_cfg.t1_cycle = 0x40);
		if (tdm->work_mode == TDM_ONLINE) {
			csic_tdm_set_tx_fifo_depth(tdm->id, tdm->tx_cfg.head_depth, tdm->tx_cfg.data_depth);
			csic_tdm_set_tx_t2_cycle(tdm->id, tdm->tx_cfg.t2_cycle = 0xfa0);
		} else {
			csic_tdm_set_tx_fifo_depth(tdm->id, 32/2, 512/2);
			csic_tdm_set_tx_t2_cycle(tdm->id, tdm->tx_cfg.t2_cycle = 0x0);
		}
		if (tdm->ws.speed_dn_en) {
			csic_tdm_set_tx_data_rate(tdm->id, tdm->tx_cfg.valid_num, tdm->tx_cfg.invalid_num);
		}
		csic_tdm_tx_enable(tdm->id);
		csic_tdm_tx_cap_enable(tdm->id);
		ini_en = RX_FRM_LOST_INT_EN | RX_FRM_ERR_INT_EN | RX_BTYPE_ERR_INT_EN |
				RX_BUF_FULL_INT_EN | RX_HB_SHORT_INT_EN | RX_FIFO_FULL_INT_EN |
				TDM_LBC_ERR_INT_EN | TDM_FIFO_UNDER_INT_EN | SPEED_DN_FIFO_FULL_INT_EN |
				SPEED_DN_HSYNC_INT_EN | RX_CHN_CFG_MODE_INT_EN | TX_CHN_CFG_MODE_INT_EN |
				RDM_LBC_FIFO_FULL_INT_EN;
#ifdef CONFIG_ARCH_SUN20IW3
		if (tdm->work_mode == TDM_ONLINE && tdm->ws.speed_dn_en) {
			if (rx->ws.wdr_mode == ISP_NORMAL_MODE)
				ini_en |= RX0_FRM_DONE_INT_EN;
			else if (rx->ws.wdr_mode == ISP_DOL_WDR_MODE)
				ini_en |= RX1_FRM_DONE_INT_EN;
		}
#endif
		csic_tdm_int_clear_status(tdm->id, TDM_INT_ALL);
		csic_tdm_int_enable(tdm->id, ini_en);
		hal_enable_irq(tdm->irq);
		vin_log(VIN_LOG_TDM, "tdm%d open first, setting the interrupt and tx configuration!\n", tdm->id);

	} else {
		tdm->ws.speed_dn_en = 0;
		hal_disable_irq(tdm->irq);
		csic_tdm_int_disable(tdm->id, TDM_INT_ALL);
		if (tdm->work_mode == TDM_ONLINE) {
			csic_tdm_set_speed_dn(tdm->id, tdm->ws.speed_dn_en);
			csic_tdm_tx_cap_disable(tdm->id);
			csic_tdm_tx_disable(tdm->id);
			csic_tdm_disable(tdm->id);
			csic_tdm_top_disable(tdm->id);
		}
	}

	return 0;
}

static void tdm_set_rx_data_rate(struct tdm_rx_dev *tdm_rx, unsigned int en)
{
#if 0
#if 0
	struct tdm_dev *tdm = container_of(tdm_rx, struct tdm_dev, tdm_rx[tdm_rx->id]);
	struct rx_chn_fmt *chn_fmt = NULL;
	unsigned int tx_need_clk = 0;
	unsigned int total_tx_rate_numerator = 0;

	if (tdm->work_mode == TDM_ONLINE  || !tdm->ws.speed_dn_en)
		return;

	if (en) {
		tx_need_clk = roundup((tdm_rx->format.width + TDM_TX_HBLANK) * (tdm_rx->height + TDM_TX_VBLANK) * tdm_rx->sensor_fps, 1000000);
		tdm_rx->ws.tx_rate_denominator = 255;
		tdm_rx->ws.tx_rate_numerator = tdm_rx->ws.tx_rate_denominator * (tx_need_clk / 1000000) / (tdm_rx->isp_clk / 100 * 90 / 1000000);
		total_tx_rate_numerator += tdm_rx->ws.tx_rate_numerator;
		vin_log(VIN_LOG_TDM, "tdm_rx%d tx_need_clk is %d, tx_rate_numerator is %d\n", tdm_rx->id, tx_need_clk, tdm_rx->ws.tx_rate_numerator);
	}

	list_for_each_entry(chn_fmt, &tdm->working_chn_fmt, list) {
		if (chn_fmt->rx_dev->id == tdm_rx->id)
			continue;
		total_tx_rate_numerator += chn_fmt->rx_dev->ws.tx_rate_numerator;
	}

	if (total_tx_rate_numerator > 255) {
		total_tx_rate_numerator = 255;
		vin_warn("tdm tx valid_num morn than 255, you maybe need increase isp_clk\n");
	}
	tdm->tx_cfg.valid_num = total_tx_rate_numerator;
	if (tdm->tx_cfg.valid_num)
		tdm->tx_cfg.invalid_num = 255 - tdm->tx_cfg.valid_num;
	else
		tdm->tx_cfg.invalid_num = 0;
	csic_tdm_set_tx_data_rate(tdm->id, tdm->tx_cfg.valid_num, tdm->tx_cfg.invalid_num);
	vin_log(VIN_LOG_TDM, "tdm_rx%d %s, tx valid_num is %d, invalid_num is %d\n",
				tdm_rx->id, en ? "enable" : "disable", tdm->tx_cfg.valid_num, tdm->tx_cfg.invalid_num);
#else
	struct tdm_dev *tdm = container_of(tdm_rx, struct tdm_dev, tdm_rx[tdm_rx->id]);
	struct rx_chn_fmt *chn_fmt = NULL;
	unsigned int width[TDM_RX_NUM], height[TDM_RX_NUM], fps[TDM_RX_NUM];
	bool rx_stream[TDM_RX_NUM] = {false, false, false, false};
	unsigned int wh_fps = 0, vw_fps = 0, hh_fps = 0, vh_fps = 0;
	unsigned int isp_clk = 0;
	unsigned int i;

	if (tdm->work_mode == TDM_ONLINE)
		return;

	if (en) {
		rx_stream[tdm_rx->id] = true;
		width[tdm_rx->id] = tdm_rx->width;
		height[tdm_rx->id] = tdm_rx->height;
		fps[tdm_rx->id] = tdm_rx->sensor_fps;
		isp_clk = tdm_rx->isp_clk / 100 * 90; /* 90% */
	} else {
		rx_stream[tdm_rx->id] = false;
		width[tdm_rx->id] = 0;
		height[tdm_rx->id] = 0;
		fps[tdm_rx->id] = 0;
		isp_clk = tdm_rx->isp_clk / 100 * 90; /* 90% */
	}

	list_for_each_entry(chn_fmt, &tdm->working_chn_fmt, list) {
		if (chn_fmt->rx_dev->id == tdm_rx->id)
			continue;
		rx_stream[chn_fmt->rx_dev->id] = true;
		width[chn_fmt->rx_dev->id] = chn_fmt->rx_dev->width;
		height[chn_fmt->rx_dev->id] = chn_fmt->rx_dev->height;
		fps[chn_fmt->rx_dev->id] = chn_fmt->rx_dev->sensor_fps;
	}

	for (i = 0; i < TDM_RX_NUM; i++) {
		if (rx_stream[i] == false) {
			width[i] = 0;
			height[i] = 0;
			fps[i] = 0;
		}
	}

	/*
	 * m: valid num, n: invalid num, m + n = 255
	 * rx0 real work clock num: sum0 = ((w0*(255/m)+hb)*h0+(w0+hb)*vb)*fps0
	 * rx0 real work clock num: sum1 = ((w1*(255/m)+hb)*h1+(w1+hb)*vb)*fps1
	 * rx0 real work clock num: sum2 = ((w2*(255/m)+hb)*h2+(w2+hb)*vb)*fps2
	 * rx0 real work clock num: sum3 = ((w3*(255/m)+hb)*h3+(w3+hb)*vb)*fps3
	 * sum0 + sum1 + sum2 + sum3 = isp_clk
	 * 255/m * wh_fps = (isp_clk - hh_fps - vw_fps - vh_fps)
	 */
	for (i = 0; i < TDM_RX_NUM; i++) {
		wh_fps += (width[i] * height[i] * fps[i]);
		hh_fps += (TDM_TX_HBLANK * height[i] * fps[i]);
		vw_fps += (TDM_TX_VBLANK * width[i] * fps[i]);
		vh_fps += (TDM_TX_HBLANK * TDM_TX_VBLANK * fps[i]);
	}
	vin_log(VIN_LOG_TDM, "wh_fps %d, hh_fps %d, vw_fps %d, vh_fps %d\n", wh_fps, hh_fps, vw_fps, vh_fps);

	if (wh_fps) {
		wh_fps = roundup(wh_fps, 100);
		tdm->tx_cfg.valid_num = DIV_ROUND_UP(10 * 255 * 128 / (128 * ((isp_clk - hh_fps - vw_fps - vh_fps) / 100) / (wh_fps / 100)), 10);
	} else
		tdm->tx_cfg.valid_num = 0;

	if (tdm->tx_cfg.valid_num > 255) {
		tdm->tx_cfg.valid_num = 255;
		vin_warn("tdm tx valid_num morn than 255, you maybe need increase isp_clk\n");
	}
	if (tdm->tx_cfg.valid_num)
		tdm->tx_cfg.invalid_num = 255 - tdm->tx_cfg.valid_num;
	else
		tdm->tx_cfg.invalid_num = 0;
	csic_tdm_set_tx_data_rate(tdm->id, tdm->tx_cfg.valid_num, tdm->tx_cfg.invalid_num);
	vin_log(VIN_LOG_TDM, "tdm_rx%d %s, tx valid_num is %d, invalid_num is %d\n",
				tdm_rx->id, en ? "enable" : "disable", tdm->tx_cfg.valid_num, tdm->tx_cfg.invalid_num);
#endif
#endif
}

int sunxi_tdm_subdev_s_stream(unsigned int id, int vinc_id, int enable)
{
	struct tdm_dev *tdm = glb_tdm[id/TDM_RX_NUM];
	struct tdm_rx_dev *tdm_rx = &tdm->tdm_rx[id];
	struct mbus_framefmt_res *res = tdm_rx->res;
	struct tdm_rx_dev *tdm_rx1 = NULL, *tdm_rx2 = NULL, *tdm_rx3 = NULL;
	int i;
	int *stream_count;

	stream_count = &tdm_rx->stream_count;
	if (enable && (*stream_count)++ > 0)
		return 0;
	else if (!enable && (*stream_count == 0 || --(*stream_count) > 0))
		return 0;

	switch (global_video[vinc_id].fourcc) {
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
	if (enable) {
		tdm_rx->ws.wdr_mode = res->res_wdr_mode;

		tdm_rx->tdm_fmt = __tdm_try_format(vinc_id);

		tdm->rx_stream_cnt[tdm_rx->id] = 0;
		if (tdm_rx->ws.wdr_mode == ISP_3FDOL_WDR_MODE) {
			vin_log(VIN_LOG_FMT, "rx%d wdr mode is 3f wdr\n", tdm_rx->id);
			tdm->ws.tdm_en = 1;
			tdm->tx_cfg.data_depth = 512/3;
			tdm->tx_cfg.head_depth = 32/3;
			tdm_rx->ws.tx_func_en = 1;
			tdm_rx->ws.pkg_en = 1;
			tdm_rx->ws.lbc_en = 0;
			tdm_rx->ws.sync_en = 0;
			tdm_rx1 = &tdm->tdm_rx[1];
			tdm_rx2 = &tdm->tdm_rx[2];
			tdm_rx1->tdm_fmt = tdm_rx->tdm_fmt;
			tdm_rx2->tdm_fmt = tdm_rx->tdm_fmt;
			tdm_rx_cpy(tdm_rx, tdm_rx1);
			tdm_rx_cpy(tdm_rx, tdm_rx2);
			tdm->rx_stream_cnt[1] = 0;
			tdm->rx_stream_cnt[2] = 0;
			if (tdm->work_mode == TDM_OFFLINE) {
				tdm_rx1->ws.tx_func_en = 1;
				tdm_rx2->ws.tx_func_en = 1;
				tdm_rx1->ws.pkg_en = 1;
				tdm_rx2->ws.pkg_en = 1;
				tdm_rx1->ws.lbc_en = 0;
				tdm_rx2->ws.lbc_en = 0;
				tdm_rx1->ws.sync_en = 0;
				tdm_rx2->ws.sync_en = 0;
			} else {
				tdm_rx1->ws.tx_func_en = 1;
				tdm_rx2->ws.tx_func_en = 0;
				tdm_rx1->ws.pkg_en = 1;
				tdm_rx2->ws.pkg_en = 0;
				tdm_rx1->ws.lbc_en = 0;
				tdm_rx2->ws.lbc_en = 0;
				tdm_rx1->ws.sync_en = 0;
				tdm_rx2->ws.sync_en = 0;
			}
		} else if (tdm_rx->ws.wdr_mode == ISP_DOL_WDR_MODE) {
			vin_log(VIN_LOG_FMT, "rx%d wdr mode is 2f wdr\n", tdm_rx->id);
			tdm->ws.tdm_en = 1;
			tdm->tx_cfg.data_depth = 512/2;
			tdm->tx_cfg.head_depth = 32/2;
			tdm_rx->ws.tx_func_en = 1;
			if (tdm_rx->id == 0) {
				tdm_rx->ws.pkg_en = 0;
				tdm_rx->ws.lbc_en = 1;
				tdm_rx->ws.sync_en = 0;
				tdm_rx1 = &tdm->tdm_rx[1];
				tdm_rx1->tdm_fmt = tdm_rx->tdm_fmt;
				tdm_rx_cpy(tdm_rx, tdm_rx1);
				tdm->rx_stream_cnt[1] = 0;
				if (tdm->work_mode == TDM_OFFLINE) {
					tdm_rx1->ws.tx_func_en = 1;
					tdm_rx1->ws.pkg_en = 1;
					tdm_rx1->ws.lbc_en = 0;
					tdm_rx1->ws.sync_en = 0;
				} else {
					tdm_rx1->ws.tx_func_en = 0;
					tdm_rx1->ws.pkg_en = 0;
					tdm_rx1->ws.lbc_en = 0;
					tdm_rx1->ws.sync_en = 0;
				}
			} else if (tdm_rx->id == 2) {
				tdm_rx->ws.pkg_en = 1;
				tdm_rx->ws.lbc_en = 0;
				tdm_rx->ws.sync_en = 0;
				tdm_rx3 = &tdm->tdm_rx[3];
				tdm_rx3->tdm_fmt = tdm_rx->tdm_fmt;
				tdm_rx_cpy(tdm_rx, tdm_rx3);
				tdm->rx_stream_cnt[3] = 0;
				if (tdm->work_mode == TDM_OFFLINE) {
					tdm_rx3->ws.tx_func_en = 1;
					tdm_rx3->ws.pkg_en = 1;
					tdm_rx3->ws.lbc_en = 0;
					tdm_rx3->ws.sync_en = 0;
				} else {
					tdm_rx3->ws.tx_func_en = 0;
					tdm_rx3->ws.pkg_en = 0;
					tdm_rx3->ws.lbc_en = 0;
					tdm_rx3->ws.sync_en = 0;
				}
			}
		} else {
			vin_log(VIN_LOG_FMT, "rx%d wdr mode is normal\n", tdm_rx->id);
			if (tdm->work_mode == TDM_OFFLINE) {
				tdm->ws.tdm_en = 1;
				tdm->tx_cfg.data_depth = 512;
				tdm->tx_cfg.head_depth = 32;
				tdm_rx->ws.tx_func_en = 1;
				if (tdm_rx->id == 0) {
					tdm_rx->ws.pkg_en = 0;
					tdm_rx->ws.lbc_en = 1;
					tdm_rx->ws.sync_en = 0;
				} else {
					tdm_rx->ws.pkg_en = 1;
					tdm_rx->ws.lbc_en = 0;
					tdm_rx->ws.sync_en = 0;
				}
			} else {
				tdm->ws.tdm_en = 1;
				tdm->tx_cfg.data_depth = 512;
				tdm->tx_cfg.head_depth = 32;
				tdm_rx->ws.tx_func_en = 0;
				tdm_rx->ws.pkg_en = 0;
				tdm_rx->ws.lbc_en = 0;
				tdm_rx->ws.sync_en = 0;
				if (!tdm->ws.speed_dn_en)
					return 0;
			}
		}

		/*reg int*/
		sunxi_tdm_top_s_stream(tdm_rx, enable);
		tdm_set_rx_data_rate(tdm_rx, enable);
		tdm->tdm_rx_reset[tdm_rx->id] = 0;
		tdm_set_rx_cfg(tdm_rx, 1);
		csic_tdm_rx_enable(tdm->id, tdm_rx->id);
		csic_tdm_rx_cap_enable(tdm->id, tdm_rx->id);
		vin_log(VIN_LOG_FMT, "rx%d init end\n", tdm_rx->id);
		if (tdm_rx1) {
			tdm->tdm_rx_reset[1] = 0;
			tdm_set_rx_cfg(tdm_rx1, 1);
			csic_tdm_rx_enable(tdm->id, tdm_rx1->id);
			csic_tdm_rx_cap_enable(tdm->id, tdm_rx1->id);
			vin_log(VIN_LOG_FMT, "rx1 init end\n");
		}
		if (tdm_rx2) {
			tdm->tdm_rx_reset[2] = 0;
			tdm_set_rx_cfg(tdm_rx2, 1);
			csic_tdm_rx_enable(tdm->id, tdm_rx2->id);
			csic_tdm_rx_cap_enable(tdm->id, tdm_rx2->id);
			vin_log(VIN_LOG_FMT, "rx2 init end\n");
		}
		if (tdm_rx3) {
			tdm->tdm_rx_reset[3] = 0;
			tdm_set_rx_cfg(tdm_rx3, 1);
			csic_tdm_rx_enable(tdm->id, tdm_rx3->id);
			csic_tdm_rx_cap_enable(tdm->id, tdm_rx3->id);
			vin_log(VIN_LOG_FMT, "rx3 init end\n");
		}
	} else {
		//if (!tdm_rx->streaming)
		//	return 0;
		if (tdm_rx->ws.wdr_mode == ISP_3FDOL_WDR_MODE) {
			tdm_rx1 = &tdm->tdm_rx[1];
			tdm_rx2 = &tdm->tdm_rx[2];
		} else if (tdm_rx->ws.wdr_mode == ISP_DOL_WDR_MODE) {
			if (tdm_rx->id == 0)
				tdm_rx1 = &tdm->tdm_rx[1];
			else if (tdm_rx->id == 2)
				tdm_rx3 = &tdm->tdm_rx[3];
		}
		csic_tdm_rx_disable(tdm->id, tdm_rx->id);
		csic_tdm_int_clear_status(tdm->id, TX_FRM_DONE_INT_EN);
		csic_tdm_rx_cap_disable(tdm->id, tdm_rx->id);
		tdm_set_rx_cfg(tdm_rx, 0);
		if (tdm_rx1) {
			csic_tdm_rx_disable(tdm->id, tdm_rx1->id);
			csic_tdm_rx_cap_disable(tdm->id, tdm_rx1->id);
			tdm_set_rx_cfg(tdm_rx1, 0);
			memset(&tdm_rx1->ws, 0, sizeof(struct rx_work_status));
		}
		if (tdm_rx2) {
			csic_tdm_rx_disable(tdm->id, tdm_rx2->id);
			csic_tdm_rx_cap_disable(tdm->id, tdm_rx2->id);
			tdm_set_rx_cfg(tdm_rx2, 0);
			memset(&tdm_rx2->ws, 0, sizeof(struct rx_work_status));
		}
		if (tdm_rx3) {
			csic_tdm_rx_disable(tdm->id, tdm_rx3->id);
			csic_tdm_rx_cap_disable(tdm->id, tdm_rx3->id);
			tdm_set_rx_cfg(tdm_rx3, 0);
			memset(&tdm_rx3->ws, 0, sizeof(struct rx_work_status));
		}
		memset(&tdm_rx->ws, 0, sizeof(struct rx_work_status));
		tdm_set_rx_data_rate(tdm_rx, enable);
		sunxi_tdm_top_s_stream(tdm_rx, enable);

#if 1
		if (tdm->stream_cnt == 0) {
			vin_log(VIN_LOG_FMT, "tdm_rx%d get stream_cnt is 0\n", tdm_rx->id);
			hal_usleep(1000);
		} else {
			if (csic_tdm_get_tx_ctrl_status(tdm->id) == 0) {
				hal_usleep(1000);
				vin_log(VIN_LOG_FMT, "tdm_rx%d get tx ctrl status\n", tdm_rx->id);
			} else {
				for (i = 0; i < 100; i++) {
					if (csic_tdm_internal_get_status(tdm->id, TX_FRM_DONE_PD_MASK)) {
						vin_log(VIN_LOG_FMT, "tdm_rx%d get tx frame done, wait %d(ms)\n", tdm_rx->id, i);
						break;
					}
					if (tdm->stream_cnt == 0) {
						vin_log(VIN_LOG_FMT, "tdm_rx%d get stream_cnt is 0, wait %d(ms)\n", tdm_rx->id, i);
						hal_usleep(1000);
						break;
					}
					hal_usleep(1000);
					if (i >= 99)
						vin_warn("rx%d wait %dms to free buffer\n", tdm_rx->id, i + 1);
				}
			}
		}
#endif
		if ((tdm->work_mode == TDM_ONLINE && tdm->stream_cnt == 0) || tdm->stream_cnt) {
			//tdm_rx_bufs_free(tdm_rx);
			//if (tdm_rx1)
			//	tdm_rx_bufs_free(tdm_rx1);
			//if (tdm_rx2)
			//	tdm_rx_bufs_free(tdm_rx2);
			//if (tdm_rx3)
			//	tdm_rx_bufs_free(tdm_rx3);
		}
	}

	vin_log(VIN_LOG_FMT, "tdm_rx%d %s, %ld*%ld==%ld*%ld?\n",
			tdm_rx->id, enable ? "stream on" : "stream off",
			tdm_rx->width, tdm_rx->height,
			tdm_rx->width, tdm_rx->height);
	return 0;
}

static void __sunxi_tdm_wdr_reset(struct tdm_dev *tdm)
{
#if 0
	struct vin_md *vind = dev_get_drvdata(tdm->tdm_rx[0].subdev.v4l2_dev->dev);
	struct vin_core *vinc = NULL;
	struct csi_dev *csi = NULL;
	struct tdm_rx_dev *tdm_rx = NULL;
	struct isp_dev *isp = NULL;
	struct prs_cap_mode mode = {.mode = VCAP};
	bool flags = 1;
	int i = 0;
	bool need_ve_callback = false;
	static bool line_num_set = 1;

	if (tdm->work_mode == TDM_OFFLINE)
		return;

	/*****************stop*******************/
	for (i = 0; i < VIN_MAX_DEV; i++) {
		if (vind->vinc[i] == NULL)
			continue;
		if (!vin_streaming(&vind->vinc[i]->vid_cap))
			continue;

		tdm_rx = &tdm->tdm_rx[vind->vinc[i]->tdm_rx_sel];

		if (vind->vinc[i]->tdm_rx_sel == tdm_rx->id) {
			vinc = vind->vinc[i];
			vinc->vid_cap.frame_delay_cnt = 1;
			if (i == 0)
				need_ve_callback = true;

			if (flags) {
				isp = container_of(vinc->vid_cap.pipe.sd[VIN_IND_ISP], struct isp_dev, subdev);
				bsp_isp_set_para_ready(isp->id, PARA_NOT_READY);
#if defined CONFIG_D3D
				if ((isp->load_shadow[0x320 + 0x0]) & (1<<0)) {
					/* clear D3D rec_en */
					isp->load_shadow[0x320 + 0x0] = (isp->load_shadow[0x320 + 0x0]) & (~(1<<0));
					memcpy(isp->isp_load.vir_addr, &isp->load_shadow[0], ISP_LOAD_DRAM_SIZE);
				}
#endif
				csic_prs_capture_stop(vinc->csi_sel);

				csic_prs_disable(vinc->csi_sel);
				csic_isp_bridge_disable(0);

				csic_tdm_disable(0);
				csic_tdm_top_disable(0);

				bsp_isp_clr_irq_status(isp->id, ISP_IRQ_EN_ALL);
				bsp_isp_capture_stop(isp->id);
				bsp_isp_top_capture_stop(isp->id);
				bsp_isp_enable(isp->id, 0);

				flags = 0;
			}

			vipp_chn_cap_disable(vinc->vipp_sel);
			vipp_cap_disable(vinc->vipp_sel);
			vipp_clear_status(vinc->vipp_sel, VIPP_STATUS_ALL);
			vipp_top_clk_en(vinc->vipp_sel, 0);

			csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_ALL);
			csic_dma_top_disable(vinc->vipp_sel);
		}
	}

	/*****************start*******************/
	flags = 1;
	for (i = 0; i < VIN_MAX_DEV; i++) {
		if (vind->vinc[i] == NULL)
			continue;
		if (!vin_streaming(&vind->vinc[i]->vid_cap))
			continue;

		tdm_rx = &tdm->tdm_rx[vind->vinc[i]->tdm_rx_sel];

		if (vind->vinc[i]->tdm_rx_sel == tdm_rx->id) {
			vinc = vind->vinc[i];

			csic_dma_top_enable(vinc->vipp_sel);

			vipp_clear_status(vinc->vipp_sel, VIPP_STATUS_ALL);
			vipp_cap_enable(vinc->vipp_sel);
			vipp_top_clk_en(vinc->vipp_sel, 1);
			vipp_chn_cap_enable(vinc->vipp_sel);
			vinc->vin_status.frame_cnt = 0;
			vinc->vin_status.lost_cnt = 0;

			if (flags) {
				isp = container_of(vinc->vid_cap.pipe.sd[VIN_IND_ISP], struct isp_dev, subdev);
				bsp_isp_enable(isp->id, 1);
				bsp_isp_top_capture_start(isp->id);
				bsp_isp_set_para_ready(isp->id, PARA_READY);
				bsp_isp_capture_start(isp->id);
				isp->isp_frame_number = 0;

				csic_isp_bridge_enable(0);

				csic_tdm_top_enable(0);
				csic_tdm_enable(0);
				csic_tdm_tx_enable(0);
				csic_tdm_tx_cap_enable(0);
				csic_tdm_int_clear_status(0, TDM_INT_ALL);

				vin_print("%s:tdm_rx%d reset!!!\n", __func__, vinc->tdm_rx_sel);
				if (line_num_set) {
					csic_tdm_rx_set_line_num_ddr(tdm->id, tdm_rx->id, 1);//delayline > 64
					line_num_set = 0;
				} else {
					csic_tdm_rx_set_line_num_ddr(tdm->id, tdm_rx->id, 0);//delayline < 64
					line_num_set = 1;
				}
				csic_tdm_rx_enable(0, vinc->tdm_rx_sel);
				csic_tdm_rx_cap_enable(0, vinc->tdm_rx_sel);
				vin_print("%s:tdm_rx%d reset!!!\n", __func__, vinc->tdm_rx_sel + 1);
				csic_tdm_rx_enable(0, vinc->tdm_rx_sel + 1);
				csic_tdm_rx_cap_enable(0, vinc->tdm_rx_sel + 1);
				if (tdm_rx->ws.wdr_mode == ISP_3FDOL_WDR_MODE) {
					vin_print("%s:tdm_rx%d reset!!!\n", __func__, vinc->tdm_rx_sel + 2);
					csic_tdm_rx_enable(0, vinc->tdm_rx_sel + 2);
					csic_tdm_rx_cap_enable(0, vinc->tdm_rx_sel + 2);
				}
				csic_prs_enable(vinc->csi_sel);

				csi = container_of(vinc->vid_cap.pipe.sd[VIN_IND_CSI], struct csi_dev, subdev);
				csic_prs_capture_start(vinc->csi_sel, csi->bus_info.ch_total_num, &mode);
				flags = 0;
			}
		}
	}

	if (need_ve_callback) {
		vinc = vind->vinc[0];
		if (vinc->ve_online_cfg.ve_online_en && vinc->vid_cap.online_csi_reset_callback)
			vinc->vid_cap.online_csi_reset_callback(vinc->id);
	}
#endif
}

static void __sunxi_tdm_reset(struct tdm_dev *tdm)
{
#if 0
	struct tdm_rx_dev *tdm_rx;
	struct vin_md *vind = dev_get_drvdata(tdm->tdm_rx[0].subdev.v4l2_dev->dev);
	struct vin_core *vinc = NULL;
	struct csi_dev *csi = NULL;
	struct prs_cap_mode mode = {.mode = VCAP};
	int i;

	if (tdm->work_mode == TDM_ONLINE)
		return;

	/*****************stop*******************/
	for (i = 0; i < VIN_MAX_DEV; i++) {
		if (vind->vinc[i] == NULL)
			continue;
		if (!vin_streaming(&vind->vinc[i]->vid_cap))
			continue;
		if (!tdm->tdm_rx_reset[vind->vinc[i]->tdm_rx_sel])
			continue;

		vinc = vind->vinc[i];

		csic_prs_capture_stop(vinc->csi_sel);
		csic_prs_disable(vinc->csi_sel);

		csic_tdm_rx_disable(tdm->id, vinc->tdm_rx_sel);
		csic_tdm_rx_cap_disable(tdm->id, vinc->tdm_rx_sel);
		vin_print("%s:tdm_rx%d reset!!!\n", __func__, vinc->tdm_rx_sel);
		tdm_rx = &tdm->tdm_rx[vinc->tdm_rx_sel];
		if (tdm_rx->ws.wdr_mode == ISP_DOL_WDR_MODE) {
			csic_tdm_rx_disable(tdm->id, vinc->tdm_rx_sel + 1);
			csic_tdm_rx_cap_disable(tdm->id, vinc->tdm_rx_sel + 1);
			vin_print("%s:tdm_rx%d reset!!!\n", __func__, vinc->tdm_rx_sel + 1);
		} else if (tdm_rx->ws.wdr_mode == ISP_3FDOL_WDR_MODE) {
			csic_tdm_rx_disable(tdm->id, vinc->tdm_rx_sel + 1);
			csic_tdm_rx_cap_disable(tdm->id, vinc->tdm_rx_sel + 1);
			csic_tdm_rx_disable(tdm->id, vinc->tdm_rx_sel + 2);
			csic_tdm_rx_cap_disable(tdm->id, vinc->tdm_rx_sel + 2);
			vin_print("%s:tdm_rx%d && tdm_rx%d reset!!!\n", __func__, vinc->tdm_rx_sel + 1, vinc->tdm_rx_sel + 2);
		}
		break;
	}

	/*****************start*******************/
	for (i = 0; i < VIN_MAX_DEV; i++) {
		if (vind->vinc[i] == NULL)
			continue;
		if (!vin_streaming(&vind->vinc[i]->vid_cap))
			continue;
		if (!tdm->tdm_rx_reset[vind->vinc[i]->tdm_rx_sel])
			continue;

		vinc = vind->vinc[i];

		tdm->tdm_rx_reset[vinc->tdm_rx_sel] = 0;
		csic_tdm_rx_enable(tdm->id, vinc->tdm_rx_sel);
		csic_tdm_rx_cap_enable(tdm->id, vinc->tdm_rx_sel);
		tdm_rx = &tdm->tdm_rx[vinc->tdm_rx_sel];
		if (tdm_rx->ws.wdr_mode == ISP_DOL_WDR_MODE) {
			tdm->tdm_rx_reset[vinc->tdm_rx_sel + 1] = 0;
			csic_tdm_rx_enable(tdm->id, vinc->tdm_rx_sel + 1);
			csic_tdm_rx_cap_enable(tdm->id, vinc->tdm_rx_sel + 1);
		} else if (tdm_rx->ws.wdr_mode == ISP_3FDOL_WDR_MODE) {
			tdm->tdm_rx_reset[vinc->tdm_rx_sel + 1] = 0;
			tdm->tdm_rx_reset[vinc->tdm_rx_sel + 2] = 0;
			csic_tdm_rx_enable(tdm->id, vinc->tdm_rx_sel + 1);
			csic_tdm_rx_cap_enable(tdm->id, vinc->tdm_rx_sel + 1);
			csic_tdm_rx_enable(tdm->id, vinc->tdm_rx_sel + 2);
			csic_tdm_rx_cap_enable(tdm->id, vinc->tdm_rx_sel + 2);
		}

		csic_prs_enable(vinc->csi_sel);
		csi = container_of(vinc->vid_cap.pipe.sd[VIN_IND_CSI], struct csi_dev, subdev);
		csic_prs_capture_start(vinc->csi_sel, csi->bus_info.ch_total_num, &mode);
		break;
	}
#endif
}

int sunxi_tdm_s_mbus_config(unsigned int id, const struct mbus_framefmt_res *res)
{
	struct tdm_dev *tdm = glb_tdm[id/TDM_RX_NUM];
	struct tdm_rx_dev *tdm_rx = &tdm->tdm_rx[id];

	memcpy(tdm_rx->res, res, sizeof(struct mbus_framefmt_res));
	tdm_rx_wdr_cfg[id] = tdm_rx->res->res_wdr_mode;
	return 0;
}

static void tdm_rx_set_reset(struct tdm_dev *tdm, unsigned int rx_id)
{
	if (tdm->work_mode == TDM_ONLINE)
		return;
	if (rx_id == 0) {
		tdm->tdm_rx_reset[0] = 1;
	} else if (rx_id == 1) {
		if (tdm->tdm_rx[0].ws.wdr_mode == ISP_DOL_WDR_MODE || tdm->tdm_rx[0].ws.wdr_mode == ISP_3FDOL_WDR_MODE)
			tdm->tdm_rx_reset[0] = 1;
		else
			tdm->tdm_rx_reset[1] = 1;

	} else if (rx_id == 2) {
		if (tdm->tdm_rx[0].ws.wdr_mode == ISP_3FDOL_WDR_MODE)
			tdm->tdm_rx_reset[0] = 1;
		else
			tdm->tdm_rx_reset[2] = 1;
	} else if (rx_id == 3) {
		if (tdm->tdm_rx[2].ws.wdr_mode == ISP_DOL_WDR_MODE)
			tdm->tdm_rx_reset[2] = 1;
		else
			tdm->tdm_rx_reset[3] = 1;
	}
}

static hal_irqreturn_t tdm_isr(void *priv)
{
	struct tdm_dev *tdm = (struct tdm_dev *)priv;
	struct tdm_int_status status;
	unsigned int hb_min = 0xffff, hb_max = 0;
	unsigned int width = 0, height = 0;
	uint32_t __cpsr;

	if (tdm->stream_cnt == 0) {
		csic_tdm_int_clear_status(tdm->id, TDM_INT_ALL);
		return 0;
	}

	memset(&status, 0, sizeof(struct tdm_int_status));
	csic_tdm_int_get_status(tdm->id, &status);

	__cpsr = hal_spin_lock_irqsave(&tdm->slock);

	if (status.rx_frm_lost) {
		csic_tdm_int_clear_status(tdm->id, RX_FRM_LOST_INT_EN);
		if (csic_tdm_internal_get_status0(tdm->id, RX0_FRM_LOST_PD)) {
			vin_err("tdm%d rx0 frame lost!\n", tdm->id);
			csic_tdm_internal_clear_status0(tdm->id, RX0_FRM_LOST_PD);
			tdm_rx_set_reset(tdm, 0);
		}
		if (csic_tdm_internal_get_status0(tdm->id, RX1_FRM_LOST_PD)) {
			vin_err("tdm%d rx1 frame lost!\n", tdm->id);
			csic_tdm_internal_clear_status0(tdm->id, RX1_FRM_LOST_PD);
			tdm_rx_set_reset(tdm, 1);
		}
		if (csic_tdm_internal_get_status0(tdm->id, RX2_FRM_LOST_PD)) {
			vin_err("tdm%d rx2 frame lost!\n", tdm->id);
			csic_tdm_internal_clear_status0(tdm->id, RX2_FRM_LOST_PD);
			tdm_rx_set_reset(tdm, 2);
		}
		if (csic_tdm_internal_get_status0(tdm->id, RX3_FRM_LOST_PD)) {
			vin_err("tdm%d rx2 frame lost!\n", tdm->id);
			csic_tdm_internal_clear_status0(tdm->id, RX3_FRM_LOST_PD);
			tdm_rx_set_reset(tdm, 3);
		}
		__sunxi_tdm_reset(tdm);
	}

	if (status.rx_frm_err) {
		csic_tdm_int_clear_status(tdm->id, RX_FRM_ERR_INT_EN);
		if (csic_tdm_internal_get_status0(tdm->id, RX0_FRM_ERR_PD)) {
			csic_tdm_rx_get_size(tdm->id, 0, &width, &height);
			vin_err("tdm%d rx0 frame error! width is %d, height is %d\n", tdm->id, width, height);
			csic_tdm_internal_clear_status0(tdm->id, RX0_FRM_ERR_PD);
			tdm_rx_set_reset(tdm, 0);
		}
		if (csic_tdm_internal_get_status0(tdm->id, RX1_FRM_ERR_PD)) {
			csic_tdm_rx_get_size(tdm->id, 1, &width, &height);
			vin_err("tdm%d rx1 frame error! width is %d, height is %d\n", tdm->id, width, height);
			csic_tdm_internal_clear_status0(tdm->id, RX1_FRM_ERR_PD);
			tdm_rx_set_reset(tdm, 1);
		}
		if (csic_tdm_internal_get_status0(tdm->id, RX2_FRM_ERR_PD)) {
			csic_tdm_rx_get_size(tdm->id, 0, &width, &height);
			vin_err("tdm%d rx2 frame error! width is %d, height is %d\n", tdm->id, width, height);
			csic_tdm_internal_clear_status0(tdm->id, RX2_FRM_ERR_PD);
			tdm_rx_set_reset(tdm, 2);
		}
		if (csic_tdm_internal_get_status0(tdm->id, RX3_FRM_ERR_PD)) {
			csic_tdm_rx_get_size(tdm->id, 1, &width, &height);
			vin_err("tdm%d rx3 frame error! width is %d, height is %d\n", tdm->id, width, height);
			csic_tdm_internal_clear_status0(tdm->id, RX3_FRM_ERR_PD);
			tdm_rx_set_reset(tdm, 3);
		}
		__sunxi_tdm_reset(tdm);
	}

	if (status.rx_btype_err) {
		csic_tdm_int_clear_status(tdm->id, RX_BTYPE_ERR_INT_EN);
		if (csic_tdm_internal_get_status0(tdm->id, RX0_BTYPE_ERR_PD)) {
			vin_err("tdm%d rx0 btype error!\n", tdm->id);
			csic_tdm_internal_clear_status0(tdm->id, RX0_BTYPE_ERR_PD);
			tdm_rx_set_reset(tdm, 0);
		}
		if (csic_tdm_internal_get_status0(tdm->id, RX1_BTYPE_ERR_PD)) {
			vin_err("tdm%d rx1 btype error!\n", tdm->id);
			csic_tdm_internal_clear_status0(tdm->id, RX1_BTYPE_ERR_PD);
			tdm_rx_set_reset(tdm, 1);
		}
		if (csic_tdm_internal_get_status0(tdm->id, RX2_BTYPE_ERR_PD)) {
			vin_err("tdm%d rx2 btype error!\n", tdm->id);
			csic_tdm_internal_clear_status0(tdm->id, RX2_BTYPE_ERR_PD);
			tdm_rx_set_reset(tdm, 2);
		}
		if (csic_tdm_internal_get_status0(tdm->id, RX3_BTYPE_ERR_PD)) {
			vin_err("tdm%d rx3 btype error!\n", tdm->id);
			csic_tdm_internal_clear_status0(tdm->id, RX3_BTYPE_ERR_PD);
			tdm_rx_set_reset(tdm, 3);
		}
		__sunxi_tdm_reset(tdm);
	}

	if (status.rx_buf_full) {
		csic_tdm_int_clear_status(tdm->id, RX_BUF_FULL_INT_EN);
		if (csic_tdm_internal_get_status0(tdm->id, RX0_BUF_FULL_PD)) {
			vin_err("tdm%d rx0 buffer full!\n", tdm->id);
			csic_tdm_internal_clear_status0(tdm->id, RX0_BUF_FULL_PD);
			tdm_rx_set_reset(tdm, 0);
		}
		if (csic_tdm_internal_get_status0(tdm->id, RX1_BUF_FULL_PD)) {
			vin_err("tdm%d rx1 buffer full!\n", tdm->id);
			csic_tdm_internal_clear_status0(tdm->id, RX1_BUF_FULL_PD);
			tdm_rx_set_reset(tdm, 1);
		}
		if (csic_tdm_internal_get_status0(tdm->id, RX2_BUF_FULL_PD)) {
			vin_err("tdm%d rx2 buffer full!\n", tdm->id);
			csic_tdm_internal_clear_status0(tdm->id, RX2_BUF_FULL_PD);
			tdm_rx_set_reset(tdm, 2);
		}
		if (csic_tdm_internal_get_status0(tdm->id, RX3_BUF_FULL_PD)) {
			vin_err("tdm%d rx3 buffer full!\n", tdm->id);
			csic_tdm_internal_clear_status0(tdm->id, RX3_BUF_FULL_PD);
			tdm_rx_set_reset(tdm, 3);
		}
		__sunxi_tdm_reset(tdm);
	}

	if (status.rx_hb_short) {
		csic_tdm_int_clear_status(tdm->id, RX_HB_SHORT_INT_EN);
		if (csic_tdm_internal_get_status1(tdm->id, RX0_HB_SHORT_PD)) {
			csic_tdm_rx_get_hblank(tdm->id, 0, &hb_min, &hb_max);
			vin_err("tdm%d rx0 hblank short! min is %d, max is %d\n", tdm->id, hb_min, hb_max);
			csic_tdm_internal_clear_status1(tdm->id, RX0_HB_SHORT_PD);
			tdm_rx_set_reset(tdm, 0);
		}
		if (csic_tdm_internal_get_status1(tdm->id, RX1_HB_SHORT_PD)) {
			csic_tdm_rx_get_hblank(tdm->id, 1, &hb_min, &hb_max);
			vin_err("tdm%d rx1 hblank short! min is %d, max is %d\n", tdm->id, hb_min, hb_max);
			csic_tdm_internal_clear_status1(tdm->id, RX1_HB_SHORT_PD);
			tdm_rx_set_reset(tdm, 1);
		}
		if (csic_tdm_internal_get_status1(tdm->id, RX2_HB_SHORT_PD)) {
			csic_tdm_rx_get_hblank(tdm->id, 2, &hb_min, &hb_max);
			vin_err("tdm%d rx2 hblank short! min is %d, max is %d\n", tdm->id, hb_min, hb_max);
			csic_tdm_internal_clear_status1(tdm->id, RX2_HB_SHORT_PD);
			tdm_rx_set_reset(tdm, 2);
		}
		if (csic_tdm_internal_get_status1(tdm->id, RX3_HB_SHORT_PD)) {
			csic_tdm_rx_get_hblank(tdm->id, 3, &hb_min, &hb_max);
			vin_err("tdm%d rx3 hblank short! min is %d, max is %d\n", tdm->id, hb_min, hb_max);
			csic_tdm_internal_clear_status1(tdm->id, RX3_HB_SHORT_PD);
			tdm_rx_set_reset(tdm, 3);
		}
		__sunxi_tdm_reset(tdm);
	}

	if (status.rx_fifo_full) {
		csic_tdm_int_clear_status(tdm->id, RX_FIFO_FULL_INT_EN);
		if (csic_tdm_internal_get_status1(tdm->id, RX0_FIFO_FULL_PD)) {
			vin_err("tdm%d rx0 write DMA fifo overflow!\n", tdm->id);
			csic_tdm_internal_clear_status1(tdm->id, RX0_FIFO_FULL_PD);
			tdm_rx_set_reset(tdm, 0);
		}
		if (csic_tdm_internal_get_status1(tdm->id, RX1_FIFO_FULL_PD)) {
			vin_err("tdm%d rx1 write DMA fifo overflow!\n", tdm->id);
			csic_tdm_internal_clear_status1(tdm->id, RX1_FIFO_FULL_PD);
			tdm_rx_set_reset(tdm, 1);
		}
		if (csic_tdm_internal_get_status1(tdm->id, RX2_FIFO_FULL_PD)) {
			vin_err("tdm%d rx2 write DMA fifo overflow!\n", tdm->id);
			csic_tdm_internal_clear_status1(tdm->id, RX2_FIFO_FULL_PD);
			tdm_rx_set_reset(tdm, 2);
		}
		if (csic_tdm_internal_get_status1(tdm->id, RX3_FIFO_FULL_PD)) {
			vin_err("tdm%d rx3 write DMA fifo overflow!\n", tdm->id);
			csic_tdm_internal_clear_status1(tdm->id, RX3_FIFO_FULL_PD);
			tdm_rx_set_reset(tdm, 3);
		}
		if (csic_tdm_internal_get_status1(tdm->id, RX0_HEAD_FIFO_FULL_0)) {
			vin_err("tdm%d rx0 write line number fifo overflow from bandwidth!\n", tdm->id);
			csic_tdm_internal_clear_status1(tdm->id, RX0_HEAD_FIFO_FULL_0);
		}
		if (csic_tdm_internal_get_status1(tdm->id, RX0_HEAD_FIFO_FULL_1)) {
			vin_err("tdm%d rx0 write line number fifo overflow from para_cfg!\n", tdm->id);
			csic_tdm_internal_clear_status1(tdm->id, RX0_HEAD_FIFO_FULL_1);
		}
		if (csic_tdm_internal_get_status1(tdm->id, RX1_HEAD_FIFO_FULL_0)) {
			vin_err("tdm%d rx1 write line number fifo overflow from bandwidth!\n", tdm->id);
			csic_tdm_internal_clear_status1(tdm->id, RX1_HEAD_FIFO_FULL_0);
		}
		if (csic_tdm_internal_get_status1(tdm->id, RX1_HEAD_FIFO_FULL_1)) {
			vin_err("tdm%d rx1 write line number fifo overflow from para_cfg!\n", tdm->id);
			csic_tdm_internal_clear_status1(tdm->id, RX1_HEAD_FIFO_FULL_1);
		}
		__sunxi_tdm_reset(tdm);
	}

	if (status.rx0_frm_done) {
		csic_tdm_int_clear_status(tdm->id, RX0_FRM_DONE_INT_EN);
		vin_log(VIN_LOG_TDM, "tdm%d rx0 frame done!\n", tdm->id);
#ifdef CONFIG_ARCH_SUN20IW3
		tdm->rx_stream_cnt[0]++;
		if (tdm->rx_stream_cnt[0] > 100) {
			csic_tdm_int_disable(tdm->id, RX0_FRM_DONE_INT_EN);
			csic_tdm_int_enable(tdm->id, SPEED_DN_FIFO_FULL_INT_EN | SPEED_DN_FIFO_FULL_INT_EN);
		}
#endif
	}
	if (status.rx1_frm_done) {
		csic_tdm_int_clear_status(tdm->id, RX1_FRM_DONE_INT_EN);
		vin_log(VIN_LOG_TDM, "tdm%d rx1 frame done!\n", tdm->id);
#ifdef CONFIG_ARCH_SUN20IW3
		tdm->rx_stream_cnt[1]++;
		if (tdm->rx_stream_cnt[1] > 100) {
			csic_tdm_int_disable(tdm->id, RX1_FRM_DONE_INT_EN);
			csic_tdm_int_enable(tdm->id, SPEED_DN_FIFO_FULL_INT_EN | SPEED_DN_FIFO_FULL_INT_EN);
		}
#endif
	}
	if (status.rx2_frm_done) {
		csic_tdm_int_clear_status(tdm->id, RX2_FRM_DONE_INT_EN);
		vin_log(VIN_LOG_TDM, "tdm%d rx2 frame done!\n", tdm->id);
	}
	if (status.rx3_frm_done) {
		csic_tdm_int_clear_status(tdm->id, RX3_FRM_DONE_INT_EN);
		vin_log(VIN_LOG_TDM, "tdm%d rx3 frame done!\n", tdm->id);
	}
	if (status.tx_frm_done) {
		csic_tdm_int_clear_status(tdm->id, TX_FRM_DONE_INT_EN);
		vin_log(VIN_LOG_TDM, "tdm%d tx frame done!\n", tdm->id);
	}
	if (status.rx_chn_cfg_mode) {
		csic_tdm_int_clear_status(tdm->id, RX_CHN_CFG_MODE_INT_EN);
		vin_log(VIN_LOG_TDM, "tdm%d rx chn cfg!\n", tdm->id);
	}
	if (status.tx_chn_cfg_mode) {
		csic_tdm_int_clear_status(tdm->id, TX_CHN_CFG_MODE_INT_EN);
		vin_log(VIN_LOG_TDM, "tdm%d tx chn cfg!\n", tdm->id);
	}

	if (status.speed_dn_hsync) {
		csic_tdm_int_clear_status(tdm->id, SPEED_DN_HSYNC_INT_EN);
		vin_err("tdm%d speed dn hsync!\n", tdm->id);
	}

	if (status.speed_dn_fifo_full) {
		csic_tdm_int_clear_status(tdm->id, SPEED_DN_FIFO_FULL_INT_EN);
		vin_err("tdm%d speed dn fifo full!\n", tdm->id);
	}

	if (status.tx_fifo_under) {
		csic_tdm_int_clear_status(tdm->id, TDM_FIFO_UNDER_INT_EN);
		if (csic_tdm_internal_get_status1(tdm->id, TX0_FIFO_UNDER)) {
			vin_err("tdm%d tx0 fifo under!\n", tdm->id);
			csic_tdm_internal_clear_status1(tdm->id, TX0_FIFO_UNDER);
		}
		if (csic_tdm_internal_get_status1(tdm->id, TX1_FIFO_UNDER)) {
			vin_err("tdm%d tx1 fifo under!\n", tdm->id);
			csic_tdm_internal_clear_status1(tdm->id, TX1_FIFO_UNDER);
		}
		if (csic_tdm_internal_get_status1(tdm->id, TX2_FIFO_UNDER)) {
			vin_err("tdm%d tx2 fifo under!\n", tdm->id);
			csic_tdm_internal_clear_status1(tdm->id, TX2_FIFO_UNDER);
		}
		__sunxi_tdm_wdr_reset(tdm);
	}

	if (status.tdm_lbc_err) {
		csic_tdm_int_clear_status(tdm->id, TDM_LBC_ERR_INT_EN);
		if (csic_tdm_internal_get_status1(tdm->id, LBC0_ERROR)) {
			vin_err("tdm%d rx0 lbc error!\n", tdm->id);
			csic_tdm_internal_clear_status1(tdm->id, LBC0_ERROR);
		}
		if (csic_tdm_internal_get_status1(tdm->id, LBC1_ERROR)) {
			vin_err("tdm%d rx1 lbc error!\n", tdm->id);
			csic_tdm_internal_clear_status1(tdm->id, LBC1_ERROR);
		}
	}

	if (status.tdm_lbc_fifo_full) {
		csic_tdm_int_clear_status(tdm->id, RDM_LBC_FIFO_FULL_INT_EN);
		vin_err("tdm%d lbc fifo overflow!\n", tdm->id);
	}

	hal_spin_unlock_irqrestore(&tdm->slock, __cpsr);

	return 0;
}

int tdm_probe(unsigned int id)
{
	struct tdm_dev *tdm = NULL;
	unsigned int i;
	int ret = 0;

	if (tdm_probe_flag[id/TDM_RX_NUM])
		return 0;
	else
		tdm_probe_flag[id/TDM_RX_NUM] = true;

	tdm = hal_malloc(sizeof(struct tdm_dev));
	if (!tdm) {
		ret = -1;
		goto ekzalloc;
	}
	memset(tdm, 0, sizeof(struct tdm_dev));

	tdm->id = id/TDM_RX_NUM;
	tdm->stream_cnt = 0;

	tdm->work_mode = vin_work_mode;
	//tdm->work_mode = clamp(tdm->work_mode, TDM_ONLINE, TDM_OFFLINE);
	vin_log(VIN_LOG_TDM, "tdm%d work mode is %ld\n", tdm->id, tdm->work_mode);

	tdm->base = vin_tdm_base[tdm->id];
	if (!tdm->base) {
		tdm->is_empty = 1;
	} else {
		tdm->is_empty = 0;
		/*get irq resource */
		tdm->irq = vin_tdm_irq[tdm->id];
		if (tdm->irq <= 0) {
			vin_err("failed to get TDM IRQ resource\n");
			goto freedev;
		}
		ret = hal_request_irq(tdm->irq, tdm_isr, "vin_tdm", tdm);
		if (ret != tdm->irq) {
			vin_err("tdm%d request tdm failed\n", tdm->id);
			goto freedev;
		}
	}

	csic_tdm_set_base_addr(tdm->id, (unsigned long)tdm->base);

	for (i = 0; i < TDM_RX_NUM; i++) {
		tdm->tdm_rx[i].res = hal_malloc(sizeof(struct mbus_framefmt_res));
		memset(tdm->tdm_rx[i].res, 0, sizeof(struct mbus_framefmt_res));
		tdm->tdm_rx[i].id = i;
		tdm->tdm_rx[i].stream_cnt = 0;
		tdm->tdm_rx_buf_en[i] = 0;
	}

	glb_tdm[tdm->id] = tdm;

	vin_log(VIN_LOG_TDM, "tdm%d probe end!\n", tdm->id);
	return 0;

freedev:
	hal_free(tdm);
ekzalloc:
	vin_err("tdm probe err!\n");
	return ret;
}

void sunxi_tdm_buffer_free(unsigned int id)
{
	struct tdm_rx_dev *tdm_rx = NULL;
	unsigned int i, j;

	hal_usleep(1000);

	for (i = 0; i < TDM_RX_NUM; i++) {
		tdm_rx = &glb_tdm[id/TDM_RX_NUM]->tdm_rx[i];
		if (tdm_rx->buf_cnt) {
			for (j = 0; j < tdm_rx->buf_cnt; j++) {
				rt_memheap_free_align(tdm_rx->buf[j].dma_addr);
				tdm_rx->buf[j].dma_addr = NULL;
			}
			tdm_rx->buf_cnt = 0;
		}
	}
}

int tdm_remove(unsigned int id)
{
	struct tdm_dev *tdm = glb_tdm[id/TDM_RX_NUM];
	int i;

	if (!tdm_probe_flag[id/TDM_RX_NUM])
		return 0;
	else
		tdm_probe_flag[id/TDM_RX_NUM] = false;

	if (!tdm->is_empty) {
		hal_free_irq(tdm->irq);
	}

	for (i = 0; i < TDM_RX_NUM; i++) {
		hal_free(tdm->tdm_rx[i].res);
	}

	//sunxi_tdm_buffer_free(id);

	hal_free(tdm);
	return 0;
}

/*
void sunxi_tdm_fps_clk(unsigned int id, int fps, unsigned int isp_clk)
{
	struct tdm_rx_dev *tdm_rx = glb_tdm[id/TDM_RX_NUM]->tdm_rx[id];

	tdm_rx->sensor_fps = fps;
	tdm_rx->isp_clk = roundup(isp_clk, 1000000);
}
*/


