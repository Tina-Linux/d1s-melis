/*
 * ekernel/components/avframework/v4l2/platform/sunxi-vin/vin-stat/vin_h3a.c
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

#include <hal_atomic.h>
#include <hal_cache.h>
#include "vin_h3a.h"
#include "../vin-isp/sunxi_isp.h"
#include "../vin-video/vin_video.h"

static hal_spinlock_t vin_lock;

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

		if (curr->empty) {
			found = curr;
			break;
		}

		if (!found ||
		    (s32)curr->frame_number - (s32)found->frame_number < 0)
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

static int isp_stat_buf_queue(struct isp_stat *stat)
{
	if (!stat->active_buf)
		return STAT_NO_BUF;

	stat->active_buf->buf_size = stat->buf_size;

	stat->active_buf->frame_number = stat->frame_number;
	stat->active_buf->empty = 0;
	stat->active_buf = NULL;

	return STAT_BUF_DONE;
}

/* Get next free buffer to write the statistics to and mark it active. */
static void isp_stat_buf_next(struct isp_stat *stat)
{
	if (unlikely(stat->active_buf)) {
		/* Overwriting unused active buffer */
		vin_log(VIN_LOG_STAT, "%s: new buffer requested without queuing active one.\n",	stat->sd.name);
	} else {
		stat->active_buf = isp_stat_buf_find_oldest_or_empty(stat);
	}

	if(stat->active_buf && stat->active_buf->virt_addr)
	{
		hal_dcache_invalidate((unsigned long)stat->active_buf->virt_addr,stat->active_buf->buf_size);
		//printf("%s line %d, virt_add 0x%08x, dma 0x%08x.\n", __func__, __LINE__, stat->active_buf->virt_addr,stat->active_buf->buf_size);
	}
}

static void isp_stat_buf_release(struct isp_stat *stat)
{
	uint32_t __cpsr;

	__cpsr = hal_spin_lock_irqsave(&vin_lock);
	stat->locked_buf = NULL;
	hal_spin_unlock_irqrestore(&vin_lock, __cpsr);
}

/* Get buffer to userspace. */
static struct ispstat_buffer *isp_stat_buf_get(struct isp_stat *stat, struct vin_isp_stat_data *data)
{
	struct ispstat_buffer *buf;
	uint32_t __cpsr;

	__cpsr = hal_spin_lock_irqsave(&vin_lock);

	while (1) {
		buf = isp_stat_buf_find_oldest(stat);
		if (!buf) {
			hal_spin_unlock_irqrestore(&vin_lock, __cpsr);
			vin_log(VIN_LOG_STAT, "%s: cannot find a buffer.\n", stat->sd.name);
			return ERR_PTR(-EBUSY);
		}
		break;
	}

	stat->locked_buf = buf;

	hal_spin_unlock_irqrestore(&vin_lock, __cpsr);
	if (NULL != data) {
		if (buf->buf_size > data->buf_size) {
			vin_warn("%s: userspace's buffer size is not enough.\n", stat->sd.name);
			isp_stat_buf_release(stat);
			return ERR_PTR(-EINVAL);
		}

		hal_dcache_invalidate((unsigned long)buf->virt_addr, buf->buf_size);
		memcpy(data->buf, buf->virt_addr, buf->buf_size);
	}
	return buf;
}

static void isp_stat_bufs_free(struct isp_stat *stat)
{
	int i;

	for (i = 1; i < stat->buf_cnt; i++) {
		struct ispstat_buffer *buf = &stat->buf[i];
		struct vin_mm *mm = &stat->ion_man[i];
		mm->size = stat->buf_size;

		if (!buf->virt_addr)
			continue;

		mm->vir_addr = buf->virt_addr;
		mm->dma_addr = buf->dma_addr;
		os_mem_free(mm);

		buf->dma_addr = NULL;
		buf->virt_addr = NULL;
		buf->empty = 1;
	}

	vin_log(VIN_LOG_STAT, "%s: all buffers were freed.\n", stat->sd.name);

	stat->buf_size = 0;
	stat->active_buf = NULL;
}

static int isp_stat_bufs_alloc(struct isp_stat *stat, u32 size, u32 count)
{
	int i;
	uint32_t __cpsr;

	__cpsr = hal_spin_lock_irqsave(&vin_lock);

	BUG_ON(stat->locked_buf != NULL);

	for (i = 1; i < stat->buf_cnt; i++)
	{
		stat->buf[i].empty = 1;
	}

	/* Are the old buffers big enough? */
	if ((stat->buf_size >= size) && (stat->buf_cnt == count))
	{
		hal_spin_unlock_irqrestore(&vin_lock, __cpsr);
		vin_log(VIN_LOG_STAT, "%s: old stat buffers are enough.\n", stat->sd.name);
		return 0;
	}

	hal_spin_unlock_irqrestore(&vin_lock, __cpsr);

	isp_stat_bufs_free(stat);

	stat->buf_size = size;
	stat->buf_cnt = count;

	for (i = 1; i < stat->buf_cnt; i++) {
		struct ispstat_buffer *buf = &stat->buf[i];
		struct vin_mm *mm = &stat->ion_man[i];
		mm->size = size;
		if (!os_mem_alloc(mm)) {
			buf->virt_addr = mm->vir_addr;
			buf->dma_addr = mm->dma_addr;
		}
		if (!buf->virt_addr || !buf->dma_addr) {
			vin_err("%s: Can't acquire memory for DMA buffer %d\n",	stat->sd.name, i);
			isp_stat_bufs_free(stat);
			return -ENOMEM;
		}
		buf->empty = 1;
	}
	return 0;
}

static void isp_stat_queue_event(struct isp_stat *stat, int err)
{
	struct video_device *vdev = stat->sd.devnode;
	struct v4l2_event event;
	struct vin_isp_stat_event_status *status = (void *)event.u.data;

	memset(&event, 0, sizeof(event));
	if (!err)
		status->frame_number = stat->frame_number;
	else
		status->buf_err = 1;

	event.type = stat->event_type;
	v4l2_event_queue(vdev, &event);
}

int isp_stat_request_statistics(struct isp_stat *stat,
				     struct vin_isp_stat_data *data)
{
	struct ispstat_buffer *buf;

	if (stat->state != ISPSTAT_ENABLED) {
		vin_log(VIN_LOG_STAT, "%s: engine not enabled.\n", stat->sd.name);
		return -EINVAL;
	}
	vin_log(VIN_LOG_STAT, "user wants to request statistics.\n");

	rt_mutex_take(&stat->ioctl_lock, RT_WAITING_FOREVER);
	buf = isp_stat_buf_get(stat, data);
	if (IS_ERR((long)buf)) {
		rt_mutex_release(&stat->ioctl_lock);
		return PTR_ERR(buf);
	}

	data->frame_number = buf->frame_number;
	data->buf_size = buf->buf_size;

	buf->empty = 1;
	isp_stat_buf_release(stat);
	rt_mutex_release(&stat->ioctl_lock);

	return 0;
}

int isp_stat_config(struct isp_stat *stat, void *new_conf)
{
	int ret;
	u32 count;
	struct vin_isp_h3a_config *user_cfg = new_conf;

	if (!new_conf) {
		vin_log(VIN_LOG_STAT, "%s: configuration is NULL\n", stat->sd.name);
		return -EINVAL;
	}

	rt_mutex_take(&stat->ioctl_lock, RT_WAITING_FOREVER);

	user_cfg->buf_size = ISP_STAT_TOTAL_SIZE;

	if (stat->sensor_fps <= 30)
		count = 2;
	else if (stat->sensor_fps <= 60)
		count = 3;
	else if (stat->sensor_fps <= 120)
		count = 4;
	else
		count = 5;

	ret = isp_stat_bufs_alloc(stat, user_cfg->buf_size, count);
	if (ret) {
		rt_mutex_release(&stat->ioctl_lock);
		return ret;
	}

	/* Module has a valid configuration. */
	stat->configured = 1;

	rt_mutex_release(&stat->ioctl_lock);

	return 0;
}

static int isp_stat_buf_process(struct isp_stat *stat, int buf_state)
{
	int ret = STAT_NO_BUF;
	dma_addr_t dma_addr;

	if (buf_state == STAT_BUF_DONE && stat->state == ISPSTAT_ENABLED) {
		ret = isp_stat_buf_queue(stat);
		isp_stat_buf_next(stat);

		if (!stat->active_buf)
			return STAT_NO_BUF;

		dma_addr = (dma_addr_t)(stat->active_buf->dma_addr);
		bsp_isp_set_statistics_addr(stat->isp->id, dma_addr);
	}

	return ret;
}

int isp_stat_enable(struct isp_stat *stat, u8 enable)
{
	uint32_t __cpsr;

	vin_log(VIN_LOG_STAT, "%s: user wants to %s module.\n", stat->sd.name, enable ? "enable" : "disable");

	/* Prevent enabling while configuring */
	rt_mutex_take(&stat->ioctl_lock, RT_WAITING_FOREVER);

	__cpsr = hal_spin_lock_irqsave(&vin_lock);

	if (!stat->configured && enable) {
		hal_spin_unlock_irqrestore(&vin_lock, __cpsr);
		rt_mutex_release(&stat->ioctl_lock);
		vin_log(VIN_LOG_STAT, "%s: cannot enable module as it's never been successfully configured so far.\n", stat->sd.name);
		return -EINVAL;
	}
	stat->stat_en_flag = enable;
	stat->isp->f1_after_librun = 0;

	if (enable)
		stat->state = ISPSTAT_ENABLED;
	else
		stat->state = ISPSTAT_DISABLED;

	isp_stat_buf_next(stat);

	hal_spin_unlock_irqrestore(&vin_lock, __cpsr);
	rt_mutex_release(&stat->ioctl_lock);

	return 0;
}

void isp_stat_isr(struct isp_stat *stat)
{
	int ret = STAT_BUF_DONE;
	uint32_t __cpsr;

	vin_log(VIN_LOG_STAT, "buf state is %d, frame number is %d 0x%x %d\n",
		stat->state, stat->frame_number, stat->buf_size, stat->configured);

	__cpsr = hal_spin_lock_irqsave(&vin_lock);
	if (stat->state == ISPSTAT_DISABLED) {
		hal_spin_unlock_irqrestore(&vin_lock, __cpsr);
		return;
	}
	stat->isp->h3a_stat.frame_number++;
	//printf("%s line %d, frame no  %d.\n", __func__, __LINE__, stat->isp->h3a_stat.frame_number);

	ret = isp_stat_buf_process(stat, ret);

	hal_spin_unlock_irqrestore(&vin_lock, __cpsr);

	isp_stat_queue_event(stat, ret != STAT_BUF_DONE);
}

static long h3a_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
	struct isp_stat *stat = v4l2_get_subdevdata(sd);
	vin_log(VIN_LOG_STAT, "%s cmd is 0x%x\n", __func__, cmd);
	switch (cmd) {
	case VIDIOC_VIN_ISP_H3A_CFG:
		return isp_stat_config(stat, arg);
	case VIDIOC_VIN_ISP_STAT_REQ:
		return isp_stat_request_statistics(stat, arg);
	case VIDIOC_VIN_ISP_STAT_EN:
		return isp_stat_enable(stat, *(u8 *)arg);
	}

	return -ENOIOCTLCMD;
}

int isp_stat_subscribe_event(struct v4l2_subdev *subdev,
				  struct v4l2_fh *fh,
				  struct v4l2_event_subscription *sub)
{
	struct isp_stat *stat = v4l2_get_subdevdata(subdev);

	if (sub->type != stat->event_type)
		return -EINVAL;

	return v4l2_event_subscribe(fh, sub, STAT_NEVENTS, NULL);
}

static const struct v4l2_subdev_core_ops h3a_subdev_core_ops = {
	.ioctl = h3a_ioctl,
	.subscribe_event = isp_stat_subscribe_event,
	.unsubscribe_event = v4l2_event_subdev_unsubscribe,
};

static const struct v4l2_subdev_ops h3a_subdev_ops = {
	.core = &h3a_subdev_core_ops,
};

int vin_isp_h3a_init(struct isp_dev *isp)
{
	struct isp_stat *stat = &isp->h3a_stat;

	vin_log(VIN_LOG_STAT, "%s\n", __func__);

	memset(stat, 0x00, sizeof(*stat));
	stat->isp = isp;
	stat->event_type = V4L2_EVENT_VIN_H3A;

	rt_mutex_init(&stat->ioctl_lock, "vinstatmtx", RT_IPC_FLAG_FIFO);

	v4l2_subdev_init(&stat->sd, &h3a_subdev_ops);
	snprintf(stat->sd.name, V4L2_SUBDEV_NAME_SIZE, "sunxi_h3a.%u", isp->id);
	stat->sd.grp_id = VIN_GRP_ID_STAT;
	stat->sd.flags |= V4L2_SUBDEV_FL_HAS_EVENTS | V4L2_SUBDEV_FL_HAS_DEVNODE;
	v4l2_set_subdevdata(&stat->sd, stat);

	stat->pad.flags = MEDIA_PAD_FL_SINK;
	stat->sd.entity.function = MEDIA_ENT_F_PROC_VIDEO_STATISTICS;

	return media_entity_pads_init(&stat->sd.entity, 1, &stat->pad);
}

void vin_isp_h3a_cleanup(struct isp_dev *isp)
{
	struct isp_stat *stat = &isp->h3a_stat;

	vin_log(VIN_LOG_STAT, "%s\n", __func__);

	media_entity_cleanup(&stat->sd.entity);
	rt_mutex_delete(&stat->ioctl_lock);
	isp_stat_bufs_free(stat);
}
