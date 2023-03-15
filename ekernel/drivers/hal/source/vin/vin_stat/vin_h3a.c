/*
 * linux-4.9/drivers/media/platform/sunxi-vin/vin-stat/vin_h3a.c
 *
 * Copyright (c) 2007-2017 Allwinnertech Co., Ltd.
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

#include "vin_h3a.h"

#include "../vin.h"

static struct ispstat_buffer *__isp_stat_buf_find(struct isp_stat *stat, int look_empty)
{
	struct ispstat_buffer *found = NULL;
	int i;

	for (i = 0; i < stat->buf_cnt; i++) {
		struct ispstat_buffer *curr = &stat->buf[i];

		/*
		 * Don't select the buffer which is being copied to
		 * userspace or used by the module.
		 */
		if (curr == stat->locked_buf || curr == stat->active_buf)
			continue;

		/* Don't select uninitialised buffers if it's not required */
		if (!look_empty && curr->empty)
			continue;

		if (curr->empty && (curr->state == ISPSTAT_IDLE)) {
			found = curr;
			break;
		}

		if ((!found || (s32)curr->frame_number - (s32)found->frame_number < 0)
			&& (curr->state == ISPSTAT_READY))
			found = curr;
	}

	return found;
}

static inline struct ispstat_buffer *isp_stat_buf_find_oldest(struct isp_stat *stat)
{
	return __isp_stat_buf_find(stat, 0);
}

static inline struct ispstat_buffer *isp_stat_buf_find_oldest_or_empty(struct isp_stat *stat)
{
	return __isp_stat_buf_find(stat, 1);
}

/* Get next free buffer to write the statistics to and mark it active. */
static void isp_stat_buf_next(struct isp_stat *stat)
{
	//if (stat->active_buf == NULL) {
	//	/* Overwriting unused active buffer */
	//	vin_log(VIN_LOG_STAT, "new buffer requested without queuing active one.\n");
	//} else {
		stat->active_buf = isp_stat_buf_find_oldest_or_empty(stat);
	//}
}

static void isp_stat_buf_release(struct isp_stat *stat)
{
	uint32_t __cpsr;

	__cpsr = hal_spin_lock_irqsave(&stat->isp->slock);
	stat->locked_buf->state = ISPSTAT_IDLE;
	stat->locked_buf = NULL;
	hal_spin_unlock_irqrestore(&stat->isp->slock, __cpsr);
}

static void isp_stat_bufs_free(struct isp_stat *stat)
{
#if 0
	int i;

	for (i = 1; i < stat->buf_cnt; i++) {
		struct ispstat_buffer *buf = &stat->buf[i];
		struct vin_mm *mm = &stat->ion_man[i];

		mm->size = stat->buf_size;

		if (!buf->virt_addr)
			continue;

		mm->vir_addr = buf->virt_addr;
		mm->dma_addr = buf->dma_addr;
		os_mem_free(&stat->isp->pdev->dev, mm);

		buf->dma_addr = NULL;
		buf->virt_addr = NULL;
	}

	vin_log(VIN_LOG_STAT, "%s: all buffers were freed.\n", __func__);

	stat->buf_size = 0;
	stat->buf_cnt = 0;
	stat->active_buf = NULL;

	os_mem_free(&stat->isp->pdev->dev, &stat->isp->isp_stat);
#endif
}

static int isp_stat_bufs_alloc(struct isp_stat *stat, u32 size, u32 count)
{
#if 0
	int i;
	struct isp_dev *isp = stat->isp;

	if (count > STAT_MAX_BUFS)
		count = STAT_MAX_BUFS;

	stat->buf_size = size;
	stat->buf_cnt = count;

	for (i = 1; i < stat->buf_cnt; i++) {
		struct ispstat_buffer *buf = &stat->buf[i];
		struct vin_mm *mm = &stat->ion_man[i];

		mm->size = size;
		if (!os_mem_alloc(&stat->isp->pdev->dev, mm)) {
			buf->virt_addr = mm->vir_addr;
			buf->dma_addr = mm->dma_addr;
			buf->state = ISPSTAT_IDLE;
			buf->empty = 1;
		}
		if (!buf->virt_addr || !buf->dma_addr) {
			vin_err("%s: Can't acquire memory for stat buffer %d\n", __func__, i);
			isp_stat_bufs_free(stat);
			return -ENOMEM;
		}
	}

	isp->isp_stat_buf.size = ISP_STAT_TOTAL_SIZE + ISP_SAVE_LOAD_STATISTIC_SIZE;
	if (os_mem_alloc(&isp->pdev->dev, &isp->isp_stat_buf)) {
		vin_err("isp statistic and save_load buf requset failed!\n");
		return -ENOMEM;
	}
#else
	int i;
	struct isp_dev *isp = stat->isp;

	if (count > STAT_MAX_BUFS)
		count = STAT_MAX_BUFS;

	stat->buf_size = size;
	stat->buf_cnt = count;

	for (i = 1; i < stat->buf_cnt; i++) {
		struct ispstat_buffer *buf = &stat->buf[i];
		buf->phy_addr = rt_memheap_alloc_align(&isp_mempool, stat->buf_size, 0x1000);
		if (buf->phy_addr == NULL) {
			vin_err("%s:h3a%d:alloc stat buffer error\n", __func__, isp->id);
			return -1;
		}
		memset(buf->phy_addr, 0, stat->buf_size);
		buf->state = ISPSTAT_IDLE;
		buf->empty = 1;
		buf->id = i;
	}

	isp->isp_stat_buf.size = ISP_STAT_TOTAL_SIZE + ISP_SAVE_LOAD_STATISTIC_SIZE;
	isp->isp_stat_buf.phy_addr = rt_memheap_alloc_align(&isp_mempool, isp->isp_stat_buf.size, 0x1000);
	if (isp->isp_stat_buf.phy_addr == NULL) {
		vin_err("%s:isp%d:alloc isp_stat_buf buffer error\n", __func__, isp->id);
		return -1;
	}
#endif
	return 0;
}

static struct ispstat_buffer *isp_stat_buf_get(struct isp_stat *stat)
{
	uint32_t __cpsr;
	struct ispstat_buffer *buf;

	__cpsr = hal_spin_lock_irqsave(&stat->isp->slock);

	while (1) {
		buf = isp_stat_buf_find_oldest(stat);
		if (!buf) {
			hal_spin_unlock_irqrestore(&stat->isp->slock, __cpsr);
			vin_log(VIN_LOG_STAT, "cannot find a buffer.\n");
			return NULL;
		}
		break;
	}

	stat->locked_buf = buf;

	hal_spin_unlock_irqrestore(&stat->isp->slock, __cpsr);
	return buf;
}


int isp_stat_request_statistics(struct isp_stat *stat)
{
	struct ispstat_buffer *buf;
	struct isp_dev *isp = stat->isp;

	vin_log(VIN_LOG_STAT, "user wants to request statistics.\n");

	buf = isp_stat_buf_get(stat);
	if (buf == NULL) {
		return -1;
	}

	//vin_log(VIN_LOG_STAT1, "isp request stat, 0x%lx\n", (unsigned long)buf->phy_addr);
	//vin_print("isp request stat, 0x100 is 0x%lx\n", *((unsigned int *)(buf->phy_addr + 0x100)));
	vin_log(VIN_LOG_STAT1, "isp request stat%d\n", buf->id);

	hal_dcache_invalidate((unsigned long)buf->phy_addr, ISP_STAT_TOTAL_SIZE);
	hal_dcache_invalidate((unsigned long)(isp->isp_save_load.phy_addr + ISP_SAVE_LOAD_REG_SIZE), ISP_SAVE_LOAD_STATISTIC_SIZE);

	memcpy(isp->isp_stat_buf.phy_addr, buf->phy_addr, ISP_STAT_TOTAL_SIZE);
	memcpy(isp->isp_stat_buf.phy_addr + ISP_STAT_TOTAL_SIZE, isp->isp_save_load.phy_addr + ISP_SAVE_LOAD_REG_SIZE, ISP_SAVE_LOAD_STATISTIC_SIZE);
	buf->state = ISPSTAT_IDLE;

	buf->empty = 1;
	isp_stat_buf_release(stat);

	return 0;
}

static int isp_stat_buf_queue(struct isp_stat *stat)
{
	if (!stat->active_buf)
		return STAT_NO_BUF;

	stat->active_buf->buf_size = stat->buf_size;

	stat->active_buf->frame_number = stat->frame_number;
	stat->active_buf = NULL;

	return STAT_BUF_DONE;
}

static int isp_stat_buf_process(struct isp_stat *stat, int buf_state)
{
	unsigned long dma_addr;

	if (buf_state == STAT_BUF_DONE) {
		isp_stat_buf_next(stat);

		if (!stat->active_buf)
			return STAT_NO_BUF;

		dma_addr = (unsigned long)(stat->active_buf->phy_addr);
		bsp_isp_set_statistics_addr(stat->isp->id, dma_addr);
		vin_log(VIN_LOG_STAT1, "isp set save addr, 0x%lx\n", dma_addr);
		vin_log(VIN_LOG_STAT1, "isp set stat%d\n", stat->active_buf->id);
		if (bsp_isp_get_irq_status(stat->isp->id, PARA_LOAD_PD)) {
			stat->active_buf = NULL;
			vin_warn("para_load_pd, set active bufffer failed!\n");
		} else
			stat->active_buf->state = ISPSTAT_LOAD_SET;
	}

	return STAT_NO_BUF;
}

void isp_stat_load_set(struct isp_stat *stat)
{
	int ret = STAT_BUF_DONE;
	int i;

	for (i = 0; i < stat->buf_cnt; i++) {
		if (stat->buf[i].state == ISPSTAT_LOAD_SET) {
			//vin_warn("lost save_pd to get buffer\n");
			return;
		}
	}

	stat->isp->h3a_stat.frame_number++;

	isp_stat_buf_process(stat, ret);

#if 0
	for (i = 0; i < stat->buf_cnt; i++) {
		vin_print("load buffer%d stat is %d\n", i, stat->buf[i].state);
	}
#endif
}

void isp_stat_isr(struct isp_stat *stat)
{
	int ret = STAT_NO_BUF;
	//uint32_t __cpsr;
	int i;

	vin_log(VIN_LOG_STAT, "frame number is %ld 0x%lx\n",
		stat->frame_number, stat->buf_size);

	//__cpsr = hal_spin_lock_irqsave(&stat->isp->slock);
	for (i = 0; i < stat->buf_cnt; i++) {
		if (stat->buf[i].state == ISPSTAT_LOAD_SET) {
			//hal_spin_unlock_irqrestore(&stat->isp->slock, __cpsr);
			goto next;
		}
	}
	//vin_warn("lost load_pd to set buffer\n");
	//hal_spin_unlock_irqrestore(&stat->isp->slock, __cpsr);

next:
	//__cpsr = hal_spin_lock_irqsave(&stat->isp->slock);
	for (i = 0; i < stat->buf_cnt; i++) {
		if (stat->buf[i].state == ISPSTAT_LOAD_SET) {
			stat->buf[i].state = ISPSTAT_SAVE_SET;
		} else if (stat->buf[i].state == ISPSTAT_SAVE_SET) {
			stat->buf[i].state = ISPSTAT_READY;
			stat->buf[i].empty = 0;
			ret = STAT_BUF_DONE;
			//vin_log(VIN_LOG_STAT1, "buf%d ready, 0x%lx\n", i, (unsigned long)stat->buf[i].phy_addr);
			vin_log(VIN_LOG_STAT1, "statbuf%d ready\n", i);
		}
		vin_log(VIN_LOG_STAT, "save buffer%d stat is %d\n", i, stat->buf[i].state);
	}
	if (ret != STAT_BUF_DONE)
		stat->isp->save_get_flag = 1;
	else
		stat->isp->save_get_flag = 0;

	isp_stat_buf_queue(stat);

	//hal_spin_unlock_irqrestore(&stat->isp->slock, __cpsr);
}

int vin_isp_h3a_init(struct isp_dev *isp)
{
	struct isp_stat *stat = &isp->h3a_stat;

	if (isp->is_empty)
		return 0;

	vin_log(VIN_LOG_STAT, "%s\n", __func__);

	stat->isp = isp;

	stat->stat_en_flag = 1;
	stat->isp->f1_after_librun = 0;

	isp_stat_bufs_alloc(stat, ISP_STAT_TOTAL_SIZE, STAT_MAX_BUFS);

	return 0;
}
void vin_isp_h3a_cleanup(struct isp_dev *isp)
{
	struct isp_stat *stat = &isp->h3a_stat;

	if (isp->is_empty)
		return;

	vin_log(VIN_LOG_STAT, "%s\n", __func__);

	stat->stat_en_flag = 0;

	isp_stat_bufs_free(stat);
}

