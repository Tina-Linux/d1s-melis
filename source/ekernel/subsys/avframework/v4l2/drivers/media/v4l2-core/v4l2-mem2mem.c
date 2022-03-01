/*
 * Memory-to-memory device framework for Video for Linux 2 and videobuf.
 *
 * Helper functions for devices that use videobuf buffers for both their
 * source and destination.
 *
 * Copyright (c) 2009-2010 Samsung Electronics Co., Ltd.
 * Pawel Osciak, <pawel@osciak.com>
 * Marek Szyprowski, <m.szyprowski@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */
#include <linux/sched.h>

#include "videobuf2-v4l2.h"
#include "v4l2-mem2mem.h"
#include "v4l2-dev.h"
#include "v4l2-fh.h"
#include "v4l2-event.h"


static bool debug;
module_param(debug, bool, 0644);

#define dprintk(fmt, arg...)						\
	do {								\
		if (debug)						\
			printk(KERN_DEBUG "%s: " fmt, __func__, ## arg);\
	} while (0)


/* Instance is already queued on the job_queue */
#define TRANS_QUEUED		(1 << 0)
/* Instance is currently running in hardware */
#define TRANS_RUNNING		(1 << 1)
/* Instance is currently aborting */
#define TRANS_ABORT		(1 << 2)


/* Offset base for buffers on the destination queue - used to distinguish
 * between source and destination buffers when mmapping - they receive the same
 * offsets but for different queues */
#define DST_QUEUE_OFF_BASE	(1 << 30)


/**
 * struct v4l2_m2m_dev - per-device context
 * @curr_ctx:		currently running instance
 * @job_queue:		instances queued to run
 * @job_spinlock:	protects job_queue
 * @m2m_ops:		driver callbacks
 */
struct v4l2_m2m_dev {
	struct v4l2_m2m_ctx	*curr_ctx;

	struct list_head	job_queue;
	spinlock_t		job_spinlock;

	const struct v4l2_m2m_ops *m2m_ops;
};

static struct v4l2_m2m_queue_ctx *get_queue_ctx(struct v4l2_m2m_ctx *m2m_ctx,
						enum v4l2_buf_type type)
{
	if (V4L2_TYPE_IS_OUTPUT(type))
		return &m2m_ctx->out_q_ctx;
	else
		return &m2m_ctx->cap_q_ctx;
}

struct vb2_queue *v4l2_m2m_get_vq(struct v4l2_m2m_ctx *m2m_ctx,
				       enum v4l2_buf_type type)
{
	struct v4l2_m2m_queue_ctx *q_ctx;

	q_ctx = get_queue_ctx(m2m_ctx, type);
	if (!q_ctx)
		return NULL;

	return &q_ctx->q;
}

void *v4l2_m2m_next_buf(struct v4l2_m2m_queue_ctx *q_ctx)
{
	struct v4l2_m2m_buffer *b;
	unsigned long flags;

	//rt_hw_spin_lock_irqsave(&q_ctx->rdy_spinlock, flags);

	if (list_empty(&q_ctx->rdy_queue)) {
		//rt_hw_spin_unlock_irqrestore(&q_ctx->rdy_spinlock, flags);
		return NULL;
	}

	b = list_first_entry(&q_ctx->rdy_queue, struct v4l2_m2m_buffer, list);
	//rt_hw_spin_unlock_irqrestore(&q_ctx->rdy_spinlock, flags);
	return &b->vb;
}

void *v4l2_m2m_buf_remove(struct v4l2_m2m_queue_ctx *q_ctx)
{
	struct v4l2_m2m_buffer *b;
	unsigned long flags;

	//rt_hw_spin_lock_irqsave(&q_ctx->rdy_spinlock, flags);
	if (list_empty(&q_ctx->rdy_queue)) {
		//rt_hw_spin_unlock_irqrestore(&q_ctx->rdy_spinlock, flags);
		return NULL;
	}
	b = list_first_entry(&q_ctx->rdy_queue, struct v4l2_m2m_buffer, list);
	list_del(&b->list);
	q_ctx->num_rdy--;
	//rt_hw_spin_unlock_irqrestore(&q_ctx->rdy_spinlock, flags);

	return &b->vb;
}

/*
 * Scheduling handlers
 */

void *v4l2_m2m_get_curr_priv(struct v4l2_m2m_dev *m2m_dev)
{
	unsigned long flags;
	void *ret = NULL;

	//rt_hw_spin_lock_irqsave(&m2m_dev->job_spinlock, flags);
	if (m2m_dev->curr_ctx)
		ret = m2m_dev->curr_ctx->priv;
	//rt_hw_spin_unlock_irqrestore(&m2m_dev->job_spinlock, flags);

	return ret;
}

/**
 * v4l2_m2m_try_run() - select next job to perform and run it if possible
 *
 * Get next transaction (if present) from the waiting jobs list and run it.
 */
static void v4l2_m2m_try_run(struct v4l2_m2m_dev *m2m_dev)
{
	unsigned long flags;

	//rt_hw_spin_lock_irqsave(&m2m_dev->job_spinlock, flags);
	if (NULL != m2m_dev->curr_ctx) {
		//rt_hw_spin_unlock_irqrestore(&m2m_dev->job_spinlock, flags);
		dprintk("Another instance is running, won't run now\n");
		return;
	}

	if (list_empty(&m2m_dev->job_queue)) {
		//rt_hw_spin_unlock_irqrestore(&m2m_dev->job_spinlock, flags);
		dprintk("No job pending\n");
		return;
	}

	m2m_dev->curr_ctx = list_first_entry(&m2m_dev->job_queue,
				   struct v4l2_m2m_ctx, queue);
	m2m_dev->curr_ctx->job_flags |= TRANS_RUNNING;
	//rt_hw_spin_unlock_irqrestore(&m2m_dev->job_spinlock, flags);

	m2m_dev->m2m_ops->device_run(m2m_dev->curr_ctx->priv);
}

void v4l2_m2m_try_schedule(struct v4l2_m2m_ctx *m2m_ctx)
{
	struct v4l2_m2m_dev *m2m_dev;
	unsigned long flags_job, flags_out, flags_cap;

	m2m_dev = m2m_ctx->m2m_dev;
	dprintk("Trying to schedule a job for m2m_ctx: %p\n", m2m_ctx);

	if (!m2m_ctx->out_q_ctx.q.streaming
	    || !m2m_ctx->cap_q_ctx.q.streaming) {
		dprintk("Streaming needs to be on for both queues\n");
		return;
	}

	//rt_hw_spin_lock_irqsave(&m2m_dev->job_spinlock, flags_job);

	/* If the context is aborted then don't schedule it */
	if (m2m_ctx->job_flags & TRANS_ABORT) {
		//rt_hw_spin_unlock_irqrestore(&m2m_dev->job_spinlock, flags_job);
		dprintk("Aborted context\n");
		return;
	}

	if (m2m_ctx->job_flags & TRANS_QUEUED) {
		//rt_hw_spin_unlock_irqrestore(&m2m_dev->job_spinlock, flags_job);
		dprintk("On job queue already\n");
		return;
	}

	//rt_hw_spin_lock_irqsave(&m2m_ctx->out_q_ctx.rdy_spinlock, flags_out);
	if (list_empty(&m2m_ctx->out_q_ctx.rdy_queue)
	    && !m2m_ctx->out_q_ctx.buffered) {
		//rt_hw_spin_unlock_irqrestore(&m2m_ctx->out_q_ctx.rdy_spinlock, flags_out);
		//rt_hw_spin_unlock_irqrestore(&m2m_dev->job_spinlock, flags_job);
		dprintk("No input buffers available\n");
		return;
	}
	//rt_hw_spin_lock_irqsave(&m2m_ctx->cap_q_ctx.rdy_spinlock, flags_cap);
	if (list_empty(&m2m_ctx->cap_q_ctx.rdy_queue)
	    && !m2m_ctx->cap_q_ctx.buffered) {
		//rt_hw_spin_unlock_irqrestore(&m2m_ctx->cap_q_ctx.rdy_spinlock, flags_cap);
		//rt_hw_spin_unlock_irqrestore(&m2m_ctx->out_q_ctx.rdy_spinlock, flags_out);
		//rt_hw_spin_unlock_irqrestore(&m2m_dev->job_spinlock, flags_job);
		dprintk("No output buffers available\n");
		return;
	}
	//rt_hw_spin_unlock_irqrestore(&m2m_ctx->cap_q_ctx.rdy_spinlock, flags_cap);
	//rt_hw_spin_unlock_irqrestore(&m2m_ctx->out_q_ctx.rdy_spinlock, flags_out);

	if (m2m_dev->m2m_ops->job_ready
		&& (!m2m_dev->m2m_ops->job_ready(m2m_ctx->priv))) {
		//rt_hw_spin_unlock_irqrestore(&m2m_dev->job_spinlock, flags_job);
		dprintk("Driver not ready\n");
		return;
	}

	list_add_tail(&m2m_ctx->queue, &m2m_dev->job_queue);
	m2m_ctx->job_flags |= TRANS_QUEUED;

	//rt_hw_spin_unlock_irqrestore(&m2m_dev->job_spinlock, flags_job);

	v4l2_m2m_try_run(m2m_dev);
}

/**
 * v4l2_m2m_cancel_job() - cancel pending jobs for the context
 *
 * In case of streamoff or release called on any context,
 * 1] If the context is currently running, then abort job will be called
 * 2] If the context is queued, then the context will be removed from
 *    the job_queue
 */
static void v4l2_m2m_cancel_job(struct v4l2_m2m_ctx *m2m_ctx)
{
	struct v4l2_m2m_dev *m2m_dev;
	unsigned long flags;

	m2m_dev = m2m_ctx->m2m_dev;
	//rt_hw_spin_lock_irqsave(&m2m_dev->job_spinlock, flags);

	m2m_ctx->job_flags |= TRANS_ABORT;
	if (m2m_ctx->job_flags & TRANS_RUNNING) {
		//rt_hw_spin_unlock_irqrestore(&m2m_dev->job_spinlock, flags);
		m2m_dev->m2m_ops->job_abort(m2m_ctx->priv);
		dprintk("m2m_ctx %p running, will wait to complete", m2m_ctx);
		wait_event(m2m_ctx->finished,
				!(m2m_ctx->job_flags & TRANS_RUNNING));
	} else if (m2m_ctx->job_flags & TRANS_QUEUED) {
		list_del(&m2m_ctx->queue);
		m2m_ctx->job_flags &= ~(TRANS_QUEUED | TRANS_RUNNING);
		//rt_hw_spin_unlock_irqrestore(&m2m_dev->job_spinlock, flags);
		dprintk("m2m_ctx: %p had been on queue and was removed\n",
			m2m_ctx);
	} else {
		/* Do nothing, was not on queue/running */
		//rt_hw_spin_unlock_irqrestore(&m2m_dev->job_spinlock, flags);
	}
}

void v4l2_m2m_job_finish(struct v4l2_m2m_dev *m2m_dev,
			 struct v4l2_m2m_ctx *m2m_ctx)
{
	unsigned long flags;

	//rt_hw_spin_lock_irqsave(&m2m_dev->job_spinlock, flags);
	if (!m2m_dev->curr_ctx || m2m_dev->curr_ctx != m2m_ctx) {
		//rt_hw_spin_unlock_irqrestore(&m2m_dev->job_spinlock, flags);
		dprintk("Called by an instance not currently running\n");
		return;
	}

	list_del(&m2m_dev->curr_ctx->queue);
	m2m_dev->curr_ctx->job_flags &= ~(TRANS_QUEUED | TRANS_RUNNING);
	//wake_up(&m2m_dev->curr_ctx->finished);
	m2m_dev->curr_ctx = NULL;

	//rt_hw_spin_unlock_irqrestore(&m2m_dev->job_spinlock, flags);

	/* This instance might have more buffers ready, but since we do not
	 * allow more than one job on the job_queue per instance, each has
	 * to be scheduled separately after the previous one finishes. */
	v4l2_m2m_try_schedule(m2m_ctx);
	v4l2_m2m_try_run(m2m_dev);
}

int v4l2_m2m_reqbufs(struct file *file, struct v4l2_m2m_ctx *m2m_ctx,
		     struct v4l2_requestbuffers *reqbufs)
{
	struct vb2_queue *vq;
	int ret;

	vq = v4l2_m2m_get_vq(m2m_ctx, reqbufs->type);
	ret = vb2_reqbufs(vq, reqbufs);
	/* If count == 0, then the owner has released all buffers and he
	   is no longer owner of the queue. Otherwise we have an owner. */
	if (ret == 0)
		vq->owner = reqbufs->count ? file->private_data : NULL;

	return ret;
}

int v4l2_m2m_querybuf(struct file *file, struct v4l2_m2m_ctx *m2m_ctx,
		      struct v4l2_buffer *buf)
{
	struct vb2_queue *vq;
	int ret = 0;
	unsigned int i;

	vq = v4l2_m2m_get_vq(m2m_ctx, buf->type);
	ret = vb2_querybuf(vq, buf);

	/* Adjust MMAP memory offsets for the CAPTURE queue */
	if (buf->memory == V4L2_MEMORY_MMAP && !V4L2_TYPE_IS_OUTPUT(vq->type)) {
		if (V4L2_TYPE_IS_MULTIPLANAR(vq->type)) {
			for (i = 0; i < buf->length; ++i)
				buf->m.planes[i].m.mem_offset
					+= DST_QUEUE_OFF_BASE;
		} else {
			buf->m.offset += DST_QUEUE_OFF_BASE;
		}
	}

	return ret;
}

int v4l2_m2m_qbuf(struct file *file, struct v4l2_m2m_ctx *m2m_ctx,
		  struct v4l2_buffer *buf)
{
	struct vb2_queue *vq;
	int ret;

	vq = v4l2_m2m_get_vq(m2m_ctx, buf->type);
	ret = vb2_qbuf(vq, buf);
	if (!ret)
		v4l2_m2m_try_schedule(m2m_ctx);

	return ret;
}

int v4l2_m2m_dqbuf(struct file *file, struct v4l2_m2m_ctx *m2m_ctx,
		   struct v4l2_buffer *buf)
{
	struct vb2_queue *vq;

	vq = v4l2_m2m_get_vq(m2m_ctx, buf->type);
	return vb2_dqbuf(vq, buf, file->f_flags & O_NONBLOCK);
}

int v4l2_m2m_prepare_buf(struct file *file, struct v4l2_m2m_ctx *m2m_ctx,
			 struct v4l2_buffer *buf)
{
	struct vb2_queue *vq;
	int ret;

	vq = v4l2_m2m_get_vq(m2m_ctx, buf->type);
	ret = vb2_prepare_buf(vq, buf);
	if (!ret)
		v4l2_m2m_try_schedule(m2m_ctx);

	return ret;
}

int v4l2_m2m_create_bufs(struct file *file, struct v4l2_m2m_ctx *m2m_ctx,
			 struct v4l2_create_buffers *create)
{
	struct vb2_queue *vq;

	vq = v4l2_m2m_get_vq(m2m_ctx, create->format.type);
	return vb2_create_bufs(vq, create);
}

int v4l2_m2m_expbuf(struct file *file, struct v4l2_m2m_ctx *m2m_ctx,
		  struct v4l2_exportbuffer *eb)
{
	struct vb2_queue *vq;

	vq = v4l2_m2m_get_vq(m2m_ctx, eb->type);
	return vb2_expbuf(vq, eb);
}

int v4l2_m2m_streamon(struct file *file, struct v4l2_m2m_ctx *m2m_ctx,
		      enum v4l2_buf_type type)
{
	struct vb2_queue *vq;
	int ret;

	vq = v4l2_m2m_get_vq(m2m_ctx, type);
	ret = vb2_streamon(vq, type);
	if (!ret)
		v4l2_m2m_try_schedule(m2m_ctx);

	return ret;
}

int v4l2_m2m_streamoff(struct file *file, struct v4l2_m2m_ctx *m2m_ctx,
		       enum v4l2_buf_type type)
{
	struct v4l2_m2m_dev *m2m_dev;
	struct v4l2_m2m_queue_ctx *q_ctx;
	unsigned long flags_job, flags;
	int ret;

	/* wait until the current context is dequeued from job_queue */
	v4l2_m2m_cancel_job(m2m_ctx);

	q_ctx = get_queue_ctx(m2m_ctx, type);
	ret = vb2_streamoff(&q_ctx->q, type);
	if (ret)
		return ret;

	m2m_dev = m2m_ctx->m2m_dev;
	//rt_hw_spin_lock_irqsave(&m2m_dev->job_spinlock, flags_job);
	/* We should not be scheduled anymore, since we're dropping a queue. */
	if (m2m_ctx->job_flags & TRANS_QUEUED)
		list_del(&m2m_ctx->queue);
	m2m_ctx->job_flags = 0;

	//rt_hw_spin_lock_irqsave(&q_ctx->rdy_spinlock, flags);
	/* Drop queue, since streamoff returns device to the same state as after
	 * calling reqbufs. */
	INIT_LIST_HEAD(&q_ctx->rdy_queue);
	q_ctx->num_rdy = 0;
	//rt_hw_spin_unlock_irqrestore(&q_ctx->rdy_spinlock, flags);

	if (m2m_dev->curr_ctx == m2m_ctx) {
		m2m_dev->curr_ctx = NULL;
		//wake_up(&m2m_ctx->finished);
	}
	//rt_hw_spin_unlock_irqrestore(&m2m_dev->job_spinlock, flags_job);

	return 0;
}

unsigned int v4l2_m2m_poll(struct file *file, struct v4l2_m2m_ctx *m2m_ctx,
			   struct rt_pollreq *wait)
{
	struct video_device *vfd = video_devdata(file);
	unsigned long req_events = poll_requested_events(wait);
	struct vb2_queue *src_q, *dst_q;
	struct vb2_buffer *src_vb = NULL, *dst_vb = NULL;
	unsigned int rc = 0;
	unsigned long flags;

	if (test_bit(V4L2_FL_USES_V4L2_FH, &vfd->flags)) {
		struct v4l2_fh *fh = file->private_data;

		if (v4l2_event_pending(fh))
			rc = POLLPRI;
		else if (req_events & POLLPRI)
			rt_poll_add( &fh->wait, wait);
		if (!(req_events & (POLLOUT | POLLWRNORM | POLLIN | POLLRDNORM)))
			return rc;
	}

	src_q = v4l2_m2m_get_src_vq(m2m_ctx);
	dst_q = v4l2_m2m_get_dst_vq(m2m_ctx);

	/*
	 * There has to be at least one buffer queued on each queued_list, which
	 * means either in driver already or waiting for driver to claim it
	 * and start processing.
	 */
	if ((!src_q->streaming || list_empty(&src_q->queued_list))
		&& (!dst_q->streaming || list_empty(&dst_q->queued_list))) {
		rc |= POLLERR;
		goto end;
	}

	//rt_hw_spin_lock_irqsave(&src_q->done_lock, flags);
	if (list_empty(&src_q->done_list))
		rt_poll_add( &src_q->done_wq, wait);
	//rt_hw_spin_unlock_irqrestore(&src_q->done_lock, flags);

	//rt_hw_spin_lock_irqsave(&dst_q->done_lock, flags);
	if (list_empty(&dst_q->done_list)) {
		/*
		 * If the last buffer was dequeued from the capture queue,
		 * return immediately. DQBUF will return -EPIPE.
		 */
		if (dst_q->last_buffer_dequeued) {
			//rt_hw_spin_unlock_irqrestore(&dst_q->done_lock, flags);
			return rc | POLLIN | POLLRDNORM;
		}

		rt_poll_add( &dst_q->done_wq, wait);
	}
	//rt_hw_spin_unlock_irqrestore(&dst_q->done_lock, flags);

	//rt_hw_spin_lock_irqsave(&src_q->done_lock, flags);
	if (!list_empty(&src_q->done_list))
		src_vb = list_first_entry(&src_q->done_list, struct vb2_buffer,
						done_entry);
	if (src_vb && (src_vb->state == VB2_BUF_STATE_DONE
			|| src_vb->state == VB2_BUF_STATE_ERROR))
		rc |= POLLOUT | POLLWRNORM;
	//rt_hw_spin_unlock_irqrestore(&src_q->done_lock, flags);

	//rt_hw_spin_lock_irqsave(&dst_q->done_lock, flags);
	if (!list_empty(&dst_q->done_list))
		dst_vb = list_first_entry(&dst_q->done_list, struct vb2_buffer,
						done_entry);
	if (dst_vb && (dst_vb->state == VB2_BUF_STATE_DONE
			|| dst_vb->state == VB2_BUF_STATE_ERROR))
		rc |= POLLIN | POLLRDNORM;
	//rt_hw_spin_unlock_irqrestore(&dst_q->done_lock, flags);

end:
	return rc;
}

int v4l2_m2m_mmap(struct file *file, struct v4l2_m2m_ctx *m2m_ctx,
			 struct map_struct *vma)
{
	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
	struct vb2_queue *vq;

	if (offset < DST_QUEUE_OFF_BASE) {
		vq = v4l2_m2m_get_src_vq(m2m_ctx);
	} else {
		vq = v4l2_m2m_get_dst_vq(m2m_ctx);
		vma->vm_pgoff -= (DST_QUEUE_OFF_BASE >> PAGE_SHIFT);
	}

	return vb2_mmap(vq, vma);
}

struct v4l2_m2m_dev *v4l2_m2m_init(const struct v4l2_m2m_ops *m2m_ops)
{
	struct v4l2_m2m_dev *m2m_dev;

	if (!m2m_ops || WARN_ON(!m2m_ops->device_run) ||
			WARN_ON(!m2m_ops->job_abort))
		return ERR_PTR(-EINVAL);

	m2m_dev  = rt_malloc(sizeof *m2m_dev);
	if (!m2m_dev)
		return ERR_PTR(-ENOMEM);

	memset(m2m_dev, 0x00, sizeof *m2m_dev);
	m2m_dev->curr_ctx = NULL;
	m2m_dev->m2m_ops = m2m_ops;
	INIT_LIST_HEAD(&m2m_dev->job_queue);
	//rt_hw_spin_lock_init(&m2m_dev->job_spinlock);

	return m2m_dev;
}

void v4l2_m2m_release(struct v4l2_m2m_dev *m2m_dev)
{
	rt_free(m2m_dev);
}

struct v4l2_m2m_ctx *v4l2_m2m_ctx_init(struct v4l2_m2m_dev *m2m_dev,
		void *drv_priv,
		int (*queue_init)(void *priv, struct vb2_queue *src_vq, struct vb2_queue *dst_vq))
{
	struct v4l2_m2m_ctx *m2m_ctx;
	struct v4l2_m2m_queue_ctx *out_q_ctx, *cap_q_ctx;
	int ret;

	m2m_ctx  = rt_malloc(sizeof *m2m_ctx);
	if (!m2m_ctx)
		return ERR_PTR(-ENOMEM);

	memset(m2m_ctx, 0x00, sizeof *m2m_ctx);
	m2m_ctx->priv = drv_priv;
	m2m_ctx->m2m_dev = m2m_dev;
	init_waitqueue_head(&m2m_ctx->finished);

	out_q_ctx = &m2m_ctx->out_q_ctx;
	cap_q_ctx = &m2m_ctx->cap_q_ctx;

	INIT_LIST_HEAD(&out_q_ctx->rdy_queue);
	INIT_LIST_HEAD(&cap_q_ctx->rdy_queue);
	//rt_hw_spin_lock_init(&out_q_ctx->rdy_spinlock);
	//rt_hw_spin_lock_init(&cap_q_ctx->rdy_spinlock);

	INIT_LIST_HEAD(&m2m_ctx->queue);

	ret = queue_init(drv_priv, &out_q_ctx->q, &cap_q_ctx->q);

	if (ret)
		goto err;
	/*
	 * If both queues use same mutex assign it as the common buffer
	 * queues lock to the m2m context. This lock is used in the
	 * v4l2_m2m_ioctl_* helpers.
	 */
	if (out_q_ctx->q.lock == cap_q_ctx->q.lock)
		m2m_ctx->q_lock = out_q_ctx->q.lock;

	return m2m_ctx;
err:
	rt_free(m2m_ctx);
	return ERR_PTR(ret);
}

void v4l2_m2m_ctx_release(struct v4l2_m2m_ctx *m2m_ctx)
{
	/* wait until the current context is dequeued from job_queue */
	v4l2_m2m_cancel_job(m2m_ctx);

	vb2_queue_release(&m2m_ctx->cap_q_ctx.q);
	vb2_queue_release(&m2m_ctx->out_q_ctx.q);

	rt_free(m2m_ctx);
}

void v4l2_m2m_buf_queue(struct v4l2_m2m_ctx *m2m_ctx,
		struct vb2_v4l2_buffer *vbuf)
{
	struct v4l2_m2m_buffer *b = container_of(vbuf,
				struct v4l2_m2m_buffer, vb);
	struct v4l2_m2m_queue_ctx *q_ctx;
	unsigned long flags;

	q_ctx = get_queue_ctx(m2m_ctx, vbuf->vb2_buf.vb2_queue->type);
	if (!q_ctx)
		return;

	//rt_hw_spin_lock_irqsave(&q_ctx->rdy_spinlock, flags);
	list_add_tail(&b->list, &q_ctx->rdy_queue);
	q_ctx->num_rdy++;
	//rt_hw_spin_unlock_irqrestore(&q_ctx->rdy_spinlock, flags);
}

/* Videobuf2 ioctl helpers */

int v4l2_m2m_ioctl_reqbufs(struct file *file, void *priv,
				struct v4l2_requestbuffers *rb)
{
	struct v4l2_fh *fh = file->private_data;

	return v4l2_m2m_reqbufs(file, fh->m2m_ctx, rb);
}

int v4l2_m2m_ioctl_create_bufs(struct file *file, void *priv,
				struct v4l2_create_buffers *create)
{
	struct v4l2_fh *fh = file->private_data;

	return v4l2_m2m_create_bufs(file, fh->m2m_ctx, create);
}

int v4l2_m2m_ioctl_querybuf(struct file *file, void *priv,
				struct v4l2_buffer *buf)
{
	struct v4l2_fh *fh = file->private_data;

	return v4l2_m2m_querybuf(file, fh->m2m_ctx, buf);
}

int v4l2_m2m_ioctl_qbuf(struct file *file, void *priv,
				struct v4l2_buffer *buf)
{
	struct v4l2_fh *fh = file->private_data;

	return v4l2_m2m_qbuf(file, fh->m2m_ctx, buf);
}

int v4l2_m2m_ioctl_dqbuf(struct file *file, void *priv,
				struct v4l2_buffer *buf)
{
	struct v4l2_fh *fh = file->private_data;

	return v4l2_m2m_dqbuf(file, fh->m2m_ctx, buf);
}

int v4l2_m2m_ioctl_prepare_buf(struct file *file, void *priv,
			       struct v4l2_buffer *buf)
{
	struct v4l2_fh *fh = file->private_data;

	return v4l2_m2m_prepare_buf(file, fh->m2m_ctx, buf);
}

int v4l2_m2m_ioctl_expbuf(struct file *file, void *priv,
				struct v4l2_exportbuffer *eb)
{
	struct v4l2_fh *fh = file->private_data;

	return v4l2_m2m_expbuf(file, fh->m2m_ctx, eb);
}

int v4l2_m2m_ioctl_streamon(struct file *file, void *priv,
				enum v4l2_buf_type type)
{
	struct v4l2_fh *fh = file->private_data;

	return v4l2_m2m_streamon(file, fh->m2m_ctx, type);
}

int v4l2_m2m_ioctl_streamoff(struct file *file, void *priv,
				enum v4l2_buf_type type)
{
	struct v4l2_fh *fh = file->private_data;

	return v4l2_m2m_streamoff(file, fh->m2m_ctx, type);
}

/*
 * v4l2_file_operations helpers. It is assumed here same lock is used
 * for the output and the capture buffer queue.
 */

int v4l2_m2m_fop_mmap(struct file *file, struct map_struct *vma)
{
	struct v4l2_fh *fh = file->private_data;

	return v4l2_m2m_mmap(file, fh->m2m_ctx, vma);
}

unsigned int v4l2_m2m_fop_poll(struct file *file, struct rt_pollreq *wait)
{
	struct v4l2_fh *fh = file->private_data;
	struct v4l2_m2m_ctx *m2m_ctx = fh->m2m_ctx;
	unsigned int ret;

//	if (m2m_ctx->q_lock)
//		rt_mutex_take(m2m_ctx->q_lock, RT_WAITING_FOREVER);

	ret = v4l2_m2m_poll(file, m2m_ctx, wait);

//	if (m2m_ctx->q_lock)
//		rt_mutex_release(m2m_ctx->q_lock);

	return ret;
}

