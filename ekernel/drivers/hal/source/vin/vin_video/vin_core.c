/*
 * vin_core.c for video manage
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

#include <hal_mem.h>
#include <hal_interrupt.h>

#include "../vin.h"
#include "vin_core.h"
#include "../vin_cci/cci_helper.h"
#include "../modules/sensor/camera.h"
#include "../utility/vin_io.h"
#include "../vin_csi/sunxi_csi.h"
#include "../vin_mipi/sunxi_mipi.h"

struct vin_core *global_vinc[VIN_MAX_VIDEO];
bool vinc_probe_flag[VIN_MAX_ISP];

#ifdef DMA_USE_IRQ_BUFFER_QUEUE
static hal_irqreturn_t vin_isr(void *priv)
{
	struct vin_core *vinc = (struct vin_core *)priv;
	struct dma_int_status status;
	unsigned int i, j;
	uint32_t __cpsr;
	int ret = -1;

	memset(&status, 0, sizeof(struct dma_int_status));

	if (vinc->work_mode == BK_ONLINE) {
		csic_dma_int_get_status(vinc->vipp_sel, &status);
		if (!status.mask)
			return 0;
	} else {
		for (i = vinc->vir_prosess_ch; i < (vinc->vir_prosess_ch + 4); i++) {
			j = clamp(i >= (vinc->vipp_sel + 4) ? i - 4 : i, vinc->vipp_sel, vinc->vipp_sel + 4);
			csic_dma_int_get_status(j, &status);
			if (!status.mask) {
				vinc->vir_prosess_ch = (j + 1) >= (vinc->vipp_sel + 4) ? j + 1 - 4 : j + 1;
				return 0;
			}
			vinc->vir_prosess_ch = (j + 1) >= (vinc->vipp_sel + 4) ? j + 1 - 4 : j + 1;
			vinc = global_vinc[j];
			break;
		}
	}

	__cpsr = hal_spin_lock_irqsave(&vinc->slock);

	if ((status.buf_0_overflow) || (status.hblank_overflow)) {
		if (status.buf_0_overflow) {
			csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_BUF_0_OVERFLOW);

			vin_err("video%d fifo overflow\n", vinc->id);
		}

		if (status.hblank_overflow) {
			csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_HBLANK_OVERFLOW);
			vin_err("video%d line information fifo(16 lines) overflow\n", vinc->id);
		}
	}
#if 0
	if (status.fbc_ovhd_wrddr_full) {
		csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_FBC_OVHD_WRDDR_FULL);
		/*when open isp debug please ignore fbc error!*/
		if (!vinc->isp_dbg.debug_en)
			vin_err("video%d fbc overhead write ddr full\n", vinc->id);
	}
	if (status.fbc_data_wrddr_full) {
		csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_FBC_DATA_WRDDR_FULL);
		vin_err("video%d fbc data write ddr full\n", vinc->id);
	}
#endif
	if (status.lbc_hb) {
		csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_LBC_HB);
		vin_err("video%d lbc hblanking less than 48 bk_clk cycles\n", vinc->id);
	}
	if (status.addr_no_ready) {
		/*new frame comes but buffer address is not ready*/
		csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_ADDR_NO_READY);
		//vin_err("video%d new frame comes but buffer address is not ready\n", vinc->id);
	}
	if (status.addr_overflow) {
		/*buffer address is overwrite before used*/
		csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_ADDR_OVERFLOW);
		//vin_err("video%d buffer address is overwrite before used\n", vinc->id);
	}

	if (status.vsync_trig) {
		csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_VSYNC_TRIG);

		if (vinc->first_frame == 0) {
			vinc->first_frame = 1;
			vin_print("video%d First Frame!\n", vinc->id);
		} else if (vinc->get_yuv_en && vinc->first_frame == 1) {
			vinc->first_frame = 2;
			vin_set_addr(vinc->id, (unsigned int)vinc->buff[1].phy_addr);
			vin_print("video%d second Frame!\n", vinc->id);
		} else if (vinc->get_yuv_en && vinc->first_frame == 2) {
			vinc->first_frame = 3;
			csic_dma_int_disable(vinc->vipp_sel, DMA_INT_ALL);
			vin_print("video%d third Frame!\n", vinc->id);
		}
	}

	/*if (status.line_cnt_flag) {
		csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_LINE_CNT);
		vin_log(VIN_LOG_VIDEO, "video%d line_cnt interrupt!\n", vinc->id);
	}*/
	if (status.capture_done) {
		csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_CAPTURE_DONE);
	}
	if (status.frame_done) {
		csic_dma_int_clear_status(vinc->vipp_sel, DMA_INT_FRAME_DONE);
		vinc->frame_cnt++;
		if (vinc->get_yuv_en)
			vin_print("video%d %d frame done!\n", vinc->id, vinc->frame_cnt);
	}

	hal_spin_unlock_irqrestore(&vinc->slock, __cpsr);

	return 0;
}
#endif

int vin_core_probe(unsigned int id)
{
	struct vin_core *vinc = NULL;
	struct vin_core *logic_vinc = NULL;
	int ret = 0;

	if (vinc_probe_flag[id])
		return 0;
	else
		vinc_probe_flag[id] = true;

	if (id > VIN_MAX_SCALER) {
		vin_err("DMA%d is not existing, max is %d\n", id, VIN_MAX_SCALER);
		return -1;
	}

	vin_log(VIN_LOG_VIDEO, "%s\n", __func__);

	vinc = hal_malloc(sizeof(struct vin_core));
	if (!vinc) {
		vin_err("Fail to int bk dev!\n");
		return -1;
	}
	memset(vinc, 0, sizeof(struct vin_core));
	vinc->id = id;
	vinc->vipp_sel = id;
	vinc->vir_prosess_ch = vinc->id;

	if (vinc->id == dma_virtual_find_logic[vinc->id]) {
		vinc->work_mode = vin_work_mode;//clamp(vinc->work_mode, BK_ONLINE, BK_OFFLINE);
		vin_log(VIN_LOG_MD, "vinc%d work mode is %d\n", vinc->id, vinc->work_mode);
	} else {
		vinc->work_mode = 0xff;
		logic_vinc = global_vinc[dma_virtual_find_logic[vinc->id]];
		if (!logic_vinc) {
			vin_err("vinc%d get logic vinc fail\n", vinc->id);
			goto freedev;
		}
		if (logic_vinc->work_mode == BK_ONLINE) {
			/*online mode*/
			vinc->noneed_register = 1;
			vin_warn("video%d work in online mode, video%d cannot be founded\n", logic_vinc->id, vinc->id);
			return 0;
		}
	}

	vinc->base = global_video[id].base;
	if (!vinc->base) {
		vin_err("Fail to get DMA base addr!\n");
		vinc->is_empty = 1;
		ret = -1;
		goto freedev;
	}
	vin_log(VIN_LOG_MD, "vinc%d reg is 0x%lx\n", vinc->id, vinc->base);
	csic_dma_set_base_addr(vinc->id, vinc->base);

	global_vinc[id] = vinc;
#ifdef DMA_USE_IRQ_BUFFER_QUEUE
	if (vinc->id == dma_virtual_find_logic[vinc->id]) {
		vinc->irq =  global_video[id].irq;
		if (vinc->irq <= 0) {
			vin_err("failed to get CSI DMA IRQ resource\n");
			goto freedev;
		}
		ret = hal_request_irq(vinc->irq, vin_isr, "sunxi_video", vinc);
		if (ret != vinc->irq) {
			vin_err("vinc%d request irq failed\n", vinc->id);
		}
		vin_log(VIN_LOG_MD, "vinc%d irq is %d\n", vinc->id, vinc->irq);
	}
#endif
	ret = vin_vinc_parser(id);
	if (ret != 0) {
		vin_err("parser vinc fail!\n");
		goto freedev;
	}

	vin_log(VIN_LOG_VIDEO, "rear_sensor_sel = %d\n", vinc->rear_sensor);
	vin_log(VIN_LOG_VIDEO, "csi_sel = %d\n", vinc->csi_sel);
	vin_log(VIN_LOG_VIDEO, "mipi_sel = %d\n", vinc->mipi_sel);
	vin_log(VIN_LOG_VIDEO, "isp_sel = %d\n", vinc->isp_sel);
	vin_log(VIN_LOG_VIDEO, "vipp_sel = %d\n", vinc->vipp_sel);

	return 0;
freedev:
	hal_free(vinc);

	return ret;
}

int vin_core_remove(unsigned int id)
{
	struct vin_core *vinc = global_vinc[id];

	if (!vinc_probe_flag[id])
		return 0;
	else
		vinc_probe_flag[id] = false;

	if (vinc->noneed_register) {
		if (!vinc->is_empty)
			hal_free(vinc);
		vin_log(VIN_LOG_VIDEO, "%s end\n", __func__);
		return 0;
	}

#ifdef DMA_USE_IRQ_BUFFER_QUEUE
	if (vinc->id == dma_virtual_find_logic[vinc->id]) {
		if (vinc->irq > 0)
			hal_free_irq(vinc->irq);
	}
#endif

	if (!vinc->is_empty) {
		hal_free(vinc);
	}
	vin_log(VIN_LOG_VIDEO, "%s end\n", __func__);
	return 0;
}

