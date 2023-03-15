/*
 * videobuf2-v4l2.c - V4L2 driver helper framework
 *
 * Copyright (C) 2010 Samsung Electronics
 *
 * Author: Pawel Osciak <pawel@osciak.com>
 *	   Marek Szyprowski <m.szyprowski@samsung.com>
 *
 * The vb2_thread implementation was based on code from videobuf-dvb.c:
 *	(c) 2004 Gerd Knorr <kraxel@bytesex.org> [SUSE Labs]
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 */

#include <melis/log.h>
#include <melis/ktimer.h>

#include <media/v4l2-device.h>
#include "v4l2-dev.h"
#include "v4l2-fh.h"
#include "v4l2-event.h"
#include "v4l2-common.h"
#include "videobuf2-v4l2.h"

#include <linux/poll.h>

#ifdef DEBUG
static int debug = 0;
#define dprintk(level, fmt, arg...)					      \
	do {								      \
		if (debug >= level)					      \
			pr_info("vb2-v4l2: %s: " fmt, __func__, ## arg); \
	} while (0)
#else
#define dprintk(level, fmt, arg...)					      \
	do {								      \
	} while (0)
#endif

/* Flags that are set by the vb2 core */
#define V4L2_BUFFER_MASK_FLAGS	(V4L2_BUF_FLAG_MAPPED | V4L2_BUF_FLAG_QUEUED | \
				 V4L2_BUF_FLAG_DONE | V4L2_BUF_FLAG_ERROR | \
				 V4L2_BUF_FLAG_PREPARED | \
				 V4L2_BUF_FLAG_TIMESTAMP_MASK)
/* Output buffer flags that should be passed on to the driver */
#define V4L2_BUFFER_OUT_FLAGS	(V4L2_BUF_FLAG_PFRAME | V4L2_BUF_FLAG_BFRAME | \
				 V4L2_BUF_FLAG_KEYFRAME | V4L2_BUF_FLAG_TIMECODE)

/**
 * __verify_planes_array() - verify that the planes array passed in struct
 * v4l2_buffer from userspace can be safely used
 */
static int __verify_planes_array(struct vb2_buffer *vb, const struct v4l2_buffer *b)
{
	if (!V4L2_TYPE_IS_MULTIPLANAR(b->type))
		return 0;

	/* Is memory for copying plane information present? */
	if (b->m.planes == NULL) {
		dprintk(1, "multi-planar buffer passed but "
			   "planes array not provided\n");
		return -EINVAL;
	}

	if (b->length < vb->num_planes || b->length > VB2_MAX_PLANES) {
		dprintk(1, "incorrect planes array length, "
			   "expected %d, got %d\n", vb->num_planes, b->length);
		return -EINVAL;
	}

	return 0;
}

static int __verify_planes_array_core(struct vb2_buffer *vb, const void *pb)
{
	return __verify_planes_array(vb, pb);
}

/**
 * __verify_length() - Verify that the bytesused value for each plane fits in
 * the plane length and that the data offset doesn't exceed the bytesused value.
 */
static int __verify_length(struct vb2_buffer *vb, const struct v4l2_buffer *b)
{
	unsigned int length;
	unsigned int bytesused;
	unsigned int plane;

	if (!V4L2_TYPE_IS_OUTPUT(b->type))
		return 0;

	if (V4L2_TYPE_IS_MULTIPLANAR(b->type)) {
		for (plane = 0; plane < vb->num_planes; ++plane) {
			length = (b->memory == VB2_MEMORY_USERPTR ||
				  b->memory == VB2_MEMORY_DMABUF)
			       ? b->m.planes[plane].length
				: vb->planes[plane].length;
			bytesused = b->m.planes[plane].bytesused
				  ? b->m.planes[plane].bytesused : length;

			if (b->m.planes[plane].bytesused > length)
				return -EINVAL;

			if (b->m.planes[plane].data_offset > 0 &&
			    b->m.planes[plane].data_offset >= bytesused)
				return -EINVAL;
		}
	} else {
		length = (b->memory == VB2_MEMORY_USERPTR)
			? b->length : vb->planes[0].length;

		if (b->bytesused > length)
			return -EINVAL;
	}

	return 0;
}

static void __copy_timestamp(struct vb2_buffer *vb, const void *pb)
{
	const struct v4l2_buffer *b = pb;
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct vb2_queue *q = vb->vb2_queue;

	if (q->is_output) {
		/*
		 * For output buffers copy the timestamp if needed,
		 * and the timecode field and flag if needed.
		 */
		if (q->copy_timestamp)
			vb->timestamp = timeval_to_ns(&b->timestamp);
		vbuf->flags |= b->flags & V4L2_BUF_FLAG_TIMECODE;
		if (b->flags & V4L2_BUF_FLAG_TIMECODE)
			vbuf->timecode = b->timecode;
	}
}

static void vb2_warn_zero_bytesused(struct vb2_buffer *vb)
{
	static bool check_once;

	if (check_once)
		return;

	check_once = true;
	WARN_ON(1);

	pr_warn("use of bytesused == 0 is deprecated and will be removed in the future,\n");
	if (vb->vb2_queue->allow_zero_bytesused)
		pr_warn("use VIDIOC_DECODER_CMD(V4L2_DEC_CMD_STOP) instead.\n");
	else
		pr_warn("use the actual size instead.\n");
}

static int vb2_queue_or_prepare_buf(struct vb2_queue *q, struct v4l2_buffer *b,
				    const char *opname)
{
	if (b->type != q->type) {
		dprintk(1, "%s: invalid buffer type\n", opname);
		return -EINVAL;
	}

	if (b->index >= q->num_buffers) {
		dprintk(1, "%s: buffer index out of range\n", opname);
		return -EINVAL;
	}

	if (q->bufs[b->index] == NULL) {
		/* Should never happen */
		dprintk(1, "%s: buffer is NULL\n", opname);
		return -EINVAL;
	}

	if (b->memory != q->memory) {
		dprintk(1, "%s: invalid memory type\n", opname);
		return -EINVAL;
	}

	return __verify_planes_array(q->bufs[b->index], b);
}

/**
 * __fill_v4l2_buffer() - fill in a struct v4l2_buffer with information to be
 * returned to userspace
 */
static void __fill_v4l2_buffer(struct vb2_buffer *vb, void *pb)
{
	struct v4l2_buffer *b = pb;
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct vb2_queue *q = vb->vb2_queue;
	unsigned int plane;

	/* Copy back data such as timestamp, flags, etc. */
	b->index = vb->index;
	b->type = vb->type;
	b->memory = vb->memory;
	b->bytesused = 0;

	b->flags = vbuf->flags;
	b->field = vbuf->field;
	b->timestamp = ns_to_timeval(vb->timestamp);
	b->timecode = vbuf->timecode;
	b->sequence = vbuf->sequence;
	b->reserved2 = vbuf->exp_time;
	b->reserved = vbuf->framecnt;

	if (q->is_multiplanar) {
		/*
		 * Fill in plane-related data if userspace provided an array
		 * for it. The caller has already verified memory and size.
		 */
		b->length = vb->num_planes;
		for (plane = 0; plane < vb->num_planes; ++plane) {
			struct v4l2_plane *pdst = &b->m.planes[plane];
			struct vb2_plane *psrc = &vb->planes[plane];

			pdst->bytesused = psrc->bytesused;
			pdst->length = psrc->length;
			if (q->memory == VB2_MEMORY_MMAP)
				pdst->m.mem_offset = psrc->m.offset;
			else if (q->memory == VB2_MEMORY_USERPTR)
				pdst->m.userptr = psrc->m.userptr;
			else if (q->memory == VB2_MEMORY_DMABUF)
				pdst->m.fd = psrc->m.fd;
			pdst->data_offset = psrc->data_offset;
			memset(pdst->reserved, 0, sizeof(pdst->reserved));
		}
	} else {
		/*
		 * We use length and offset in v4l2_planes array even for
		 * single-planar buffers, but userspace does not.
		 */
		b->length = vb->planes[0].length;
		b->bytesused = vb->planes[0].bytesused;
		if (q->memory == VB2_MEMORY_MMAP)
			b->m.offset = vb->planes[0].m.offset;
		else if (q->memory == VB2_MEMORY_USERPTR)
			b->m.userptr = vb->planes[0].m.userptr;
		else if (q->memory == VB2_MEMORY_DMABUF)
			b->m.fd = vb->planes[0].m.fd;
	}

	/*
	 * Clear any buffer state related flags.
	 */
	b->flags &= ~V4L2_BUFFER_MASK_FLAGS;
	b->flags |= q->timestamp_flags & V4L2_BUF_FLAG_TIMESTAMP_MASK;
	if (!q->copy_timestamp) {
		/*
		 * For non-COPY timestamps, drop timestamp source bits
		 * and obtain the timestamp source from the queue.
		 */
		b->flags &= ~V4L2_BUF_FLAG_TSTAMP_SRC_MASK;
		b->flags |= q->timestamp_flags & V4L2_BUF_FLAG_TSTAMP_SRC_MASK;
	}

	switch (vb->state) {
	case VB2_BUF_STATE_QUEUED:
	case VB2_BUF_STATE_ACTIVE:
		b->flags |= V4L2_BUF_FLAG_QUEUED;
		break;
	case VB2_BUF_STATE_ERROR:
		b->flags |= V4L2_BUF_FLAG_ERROR;
		/* fall through */
	case VB2_BUF_STATE_DONE:
		b->flags |= V4L2_BUF_FLAG_DONE;
		break;
	case VB2_BUF_STATE_PREPARED:
		b->flags |= V4L2_BUF_FLAG_PREPARED;
		break;
	case VB2_BUF_STATE_PREPARING:
	case VB2_BUF_STATE_DEQUEUED:
	case VB2_BUF_STATE_REQUEUEING:
		/* nothing */
		break;
	}

	if (vb2_buffer_in_use(q, vb))
		b->flags |= V4L2_BUF_FLAG_MAPPED;

	if (!q->is_output &&
		b->flags & V4L2_BUF_FLAG_DONE &&
		b->flags & V4L2_BUF_FLAG_LAST)
		q->last_buffer_dequeued = true;
}

/**
 * __fill_vb2_buffer() - fill a vb2_buffer with information provided in a
 * v4l2_buffer by the userspace. It also verifies that struct
 * v4l2_buffer has a valid number of planes.
 */
static int __fill_vb2_buffer(struct vb2_buffer *vb,
		const void *pb, struct vb2_plane *planes)
{
	struct vb2_queue *q = vb->vb2_queue;
	const struct v4l2_buffer *b = pb;
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	unsigned int plane;
	int ret;

	ret = __verify_length(vb, b);
	if (ret < 0) {
		dprintk(1, "plane parameters verification failed: %d\n", ret);
		return ret;
	}
	if (b->field == V4L2_FIELD_ALTERNATE && q->is_output) {
		/*
		 * If the format's field is ALTERNATE, then the buffer's field
		 * should be either TOP or BOTTOM, not ALTERNATE since that
		 * makes no sense. The driver has to know whether the
		 * buffer represents a top or a bottom field in order to
		 * program any DMA correctly. Using ALTERNATE is wrong, since
		 * that just says that it is either a top or a bottom field,
		 * but not which of the two it is.
		 */
		dprintk(1, "the field is incorrectly set to ALTERNATE "
					"for an output buffer\n");
		return -EINVAL;
	}
	vb->timestamp = 0;
	vbuf->sequence = 0;

	if (V4L2_TYPE_IS_MULTIPLANAR(b->type)) {
		if (b->memory == VB2_MEMORY_USERPTR) {
			for (plane = 0; plane < vb->num_planes; ++plane) {
				planes[plane].m.userptr =
					b->m.planes[plane].m.userptr;
				planes[plane].length =
					b->m.planes[plane].length;
			}
		}
		if (b->memory == VB2_MEMORY_DMABUF) {
			for (plane = 0; plane < vb->num_planes; ++plane) {
				planes[plane].m.fd =
					b->m.planes[plane].m.fd;
				planes[plane].length =
					b->m.planes[plane].length;
			}
		}

		/* Fill in driver-provided information for OUTPUT types */
		if (V4L2_TYPE_IS_OUTPUT(b->type)) {
			/*
			 * Will have to go up to b->length when API starts
			 * accepting variable number of planes.
			 *
			 * If bytesused == 0 for the output buffer, then fall
			 * back to the full buffer size. In that case
			 * userspace clearly never bothered to set it and
			 * it's a safe assumption that they really meant to
			 * use the full plane sizes.
			 *
			 * Some drivers, e.g. old codec drivers, use bytesused == 0
			 * as a way to indicate that streaming is finished.
			 * In that case, the driver should use the
			 * allow_zero_bytesused flag to keep old userspace
			 * applications working.
			 */
			for (plane = 0; plane < vb->num_planes; ++plane) {
				struct vb2_plane *pdst = &planes[plane];
				struct v4l2_plane *psrc = &b->m.planes[plane];

				if (psrc->bytesused == 0)
					vb2_warn_zero_bytesused(vb);

				if (vb->vb2_queue->allow_zero_bytesused)
					pdst->bytesused = psrc->bytesused;
				else
					pdst->bytesused = psrc->bytesused ?
						psrc->bytesused : pdst->length;
				pdst->data_offset = psrc->data_offset;
			}
		}
	} else {
		/*
		 * Single-planar buffers do not use planes array,
		 * so fill in relevant v4l2_buffer struct fields instead.
		 * In videobuf we use our internal V4l2_planes struct for
		 * single-planar buffers as well, for simplicity.
		 *
		 * If bytesused == 0 for the output buffer, then fall back
		 * to the full buffer size as that's a sensible default.
		 *
		 * Some drivers, e.g. old codec drivers, use bytesused == 0 as
		 * a way to indicate that streaming is finished. In that case,
		 * the driver should use the allow_zero_bytesused flag to keep
		 * old userspace applications working.
		 */
		if (b->memory == VB2_MEMORY_USERPTR) {
			planes[0].m.userptr = b->m.userptr;
			planes[0].length = b->length;
		}

		if (b->memory == VB2_MEMORY_DMABUF) {
			planes[0].m.fd = b->m.fd;
			planes[0].length = b->length;
		}

		if (V4L2_TYPE_IS_OUTPUT(b->type)) {
			if (b->bytesused == 0)
				vb2_warn_zero_bytesused(vb);

			if (vb->vb2_queue->allow_zero_bytesused)
				planes[0].bytesused = b->bytesused;
			else
				planes[0].bytesused = b->bytesused ?
					b->bytesused : planes[0].length;
		} else
			planes[0].bytesused = 0;

	}

	/* Zero flags that the vb2 core handles */
	vbuf->flags = b->flags & ~V4L2_BUFFER_MASK_FLAGS;
	if (!vb->vb2_queue->copy_timestamp || !V4L2_TYPE_IS_OUTPUT(b->type)) {
		/*
		 * Non-COPY timestamps and non-OUTPUT queues will get
		 * their timestamp and timestamp source flags from the
		 * queue.
		 */
		vbuf->flags &= ~V4L2_BUF_FLAG_TSTAMP_SRC_MASK;
	}

	if (V4L2_TYPE_IS_OUTPUT(b->type)) {
		/*
		 * For output buffers mask out the timecode flag:
		 * this will be handled later in vb2_qbuf().
		 * The 'field' is valid metadata for this output buffer
		 * and so that needs to be copied here.
		 */
		vbuf->flags &= ~V4L2_BUF_FLAG_TIMECODE;
		vbuf->field = b->field;
	} else {
		/* Zero any output buffer flags as this is a capture buffer */
		vbuf->flags &= ~V4L2_BUFFER_OUT_FLAGS;
	}

	return 0;
}

static const struct vb2_buf_ops v4l2_buf_ops = {
	.verify_planes_array	= __verify_planes_array_core,
	.fill_user_buffer	= __fill_v4l2_buffer,
	.fill_vb2_buffer	= __fill_vb2_buffer,
	.copy_timestamp		= __copy_timestamp,
};

/**
 * vb2_querybuf() - query video buffer information
 * @q:		videobuf queue
 * @b:		buffer struct passed from userspace to vidioc_querybuf handler
 *		in driver
 *
 * Should be called from vidioc_querybuf ioctl handler in driver.
 * This function will verify the passed v4l2_buffer structure and fill the
 * relevant information for the userspace.
 *
 * The return values from this function are intended to be directly returned
 * from vidioc_querybuf handler in driver.
 */
int vb2_querybuf(struct vb2_queue *q, struct v4l2_buffer *b)
{
	struct vb2_buffer *vb;
	int ret;

	if (b->type != q->type) {
		dprintk(1, "wrong buffer type\n");
		return -EINVAL;
	}

	if (b->index >= q->num_buffers) {
		dprintk(1, "buffer index out of range\n");
		return -EINVAL;
	}
	vb = q->bufs[b->index];
	ret = __verify_planes_array(vb, b);
	if (!ret)
		vb2_core_querybuf(q, b->index, b);
	return ret;
}

int vb2_reqbufs(struct vb2_queue *q, struct v4l2_requestbuffers *req)
{
	int ret = vb2_verify_memory_type(q, req->memory, req->type);

	return ret ? ret : vb2_core_reqbufs(q, req->memory, (void *)&req->count);
}

int vb2_prepare_buf(struct vb2_queue *q, struct v4l2_buffer *b)
{
	int ret;

	if (vb2_fileio_is_active(q)) {
		dprintk(1, "file io in progress\n");
		return -EBUSY;
	}

	ret = vb2_queue_or_prepare_buf(q, b, "prepare_buf");

	return ret ? ret : vb2_core_prepare_buf(q, b->index, b);
}

int vb2_create_bufs(struct vb2_queue *q, struct v4l2_create_buffers *create)
{
	unsigned requested_planes = 1;
	unsigned requested_sizes[VIDEO_MAX_PLANES];
	struct v4l2_format *f = &create->format;
	int ret = vb2_verify_memory_type(q, create->memory, f->type);
	unsigned i;

	create->index = q->num_buffers;
	if (create->count == 0)
		return ret != -EBUSY ? ret : 0;

	switch (f->type) {
	case V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE:
	case V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE:
		requested_planes = f->fmt.pix_mp.num_planes;
		if (requested_planes == 0 ||
		    requested_planes > VIDEO_MAX_PLANES)
			return -EINVAL;
		for (i = 0; i < requested_planes; i++)
			requested_sizes[i] =
				f->fmt.pix_mp.plane_fmt[i].sizeimage;
		break;
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
		requested_sizes[0] = f->fmt.pix.sizeimage;
		break;
	case V4L2_BUF_TYPE_VBI_CAPTURE:
	case V4L2_BUF_TYPE_VBI_OUTPUT:
		requested_sizes[0] = f->fmt.vbi.samples_per_line *
			(f->fmt.vbi.count[0] + f->fmt.vbi.count[1]);
		break;
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		requested_sizes[0] = f->fmt.sliced.io_size;
		break;
	case V4L2_BUF_TYPE_SDR_CAPTURE:
	case V4L2_BUF_TYPE_SDR_OUTPUT:
		requested_sizes[0] = f->fmt.sdr.buffersize;
		break;
	default:
		return -EINVAL;
	}
	for (i = 0; i < requested_planes; i++)
		if (requested_sizes[i] == 0)
			return -EINVAL;
	return ret ? ret : vb2_core_create_bufs(q, create->memory,
		(unsigned int*)&create->count, requested_planes, requested_sizes);
}

int vb2_qbuf(struct vb2_queue *q, struct v4l2_buffer *b)
{
	int ret;

	if (vb2_fileio_is_active(q)) {
		dprintk(1, "file io in progress\n");
		return -EBUSY;
	}

	ret = vb2_queue_or_prepare_buf(q, b, "qbuf");
	return ret ? ret : vb2_core_qbuf(q, b->index, b);
}

int vb2_dqbuf(struct vb2_queue *q, struct v4l2_buffer *b, bool nonblocking)
{
	int ret;

	if (vb2_fileio_is_active(q)) {
		dprintk(1, "file io in progress\n");
		return -EBUSY;
	}

	if (b->type != q->type) {
		dprintk(1, "invalid buffer type\n");
		return -EINVAL;
	}

	ret = vb2_core_dqbuf(q, NULL, b, nonblocking);

	/*
	 *  After calling the VIDIOC_DQBUF V4L2_BUF_FLAG_DONE must be
	 *  cleared.
	 */
	b->flags &= ~V4L2_BUF_FLAG_DONE;

	return ret;
}

int vb2_streamon(struct vb2_queue *q, enum v4l2_buf_type type)
{
	if (vb2_fileio_is_active(q)) {
		dprintk(1, "file io in progress\n");
		return -EBUSY;
	}
	return vb2_core_streamon(q, type);
}

int vb2_streamoff(struct vb2_queue *q, enum v4l2_buf_type type)
{
	if (vb2_fileio_is_active(q)) {
		dprintk(1, "file io in progress\n");
		return -EBUSY;
	}
	return vb2_core_streamoff(q, type);
}

int vb2_expbuf(struct vb2_queue *q, struct v4l2_exportbuffer *eb)
{
	return vb2_core_expbuf(q, (int *)&eb->fd, eb->type, eb->index,
				eb->plane, eb->flags);
}

int vb2_queue_init(struct vb2_queue *q)
{
	/*
	 * Sanity check
	 */
	if (WARN_ON(!q)			  ||
	    WARN_ON(q->timestamp_flags &
		    ~(V4L2_BUF_FLAG_TIMESTAMP_MASK |
		      V4L2_BUF_FLAG_TSTAMP_SRC_MASK)))
		return -EINVAL;

	/* Warn that the driver should choose an appropriate timestamp type */
	WARN_ON((q->timestamp_flags & V4L2_BUF_FLAG_TIMESTAMP_MASK) ==
		V4L2_BUF_FLAG_TIMESTAMP_UNKNOWN);

	/* Warn that vb2_memory should match with v4l2_memory */
	if (WARN_ON(VB2_MEMORY_MMAP != (int)V4L2_MEMORY_MMAP)
		|| WARN_ON(VB2_MEMORY_USERPTR != (int)V4L2_MEMORY_USERPTR)
		|| WARN_ON(VB2_MEMORY_DMABUF != (int)V4L2_MEMORY_DMABUF))
		return -EINVAL;

	if (q->buf_struct_size == 0)
		q->buf_struct_size = sizeof(struct vb2_v4l2_buffer);

	q->buf_ops = &v4l2_buf_ops;
	q->is_multiplanar = V4L2_TYPE_IS_MULTIPLANAR(q->type);
	q->is_output = V4L2_TYPE_IS_OUTPUT(q->type);
	q->copy_timestamp = (q->timestamp_flags & V4L2_BUF_FLAG_TIMESTAMP_MASK)
			== V4L2_BUF_FLAG_TIMESTAMP_COPY;
	__log("q->is_output %d, q->copy_timestamp %d.", q->is_output, q->copy_timestamp);
	/*
	 * For compatibility with vb1: if QBUF hasn't been called yet, then
	 * return POLLERR as well. This only affects capture queues, output
	 * queues will always initialize waiting_for_buffers to false.
	 */
	q->quirk_poll_must_check_waiting_for_buffers = true;

	return vb2_core_queue_init(q);
}

void vb2_queue_release(struct vb2_queue *q)
{
	vb2_core_queue_release(q);
}

unsigned int vb2_poll(struct vb2_queue *q, struct file *file, struct rt_pollreq *wait)
{
	struct video_device *vfd = video_devdata(file);
	unsigned long req_events = poll_requested_events(wait);
	unsigned int res = 0;

	if (test_bit(V4L2_FL_USES_V4L2_FH, &vfd->flags)) {
		struct v4l2_fh *fh = file->private_data;

		if (v4l2_event_pending(fh))
			res = POLLPRI | POLLIN;
		else if (req_events & POLLPRI)
			rt_poll_add(&fh->wait, wait);
	}

	return res | vb2_core_poll(q, file, wait);

	return 0;
}

/*
 * The following functions are not part of the vb2 core API, but are helper
 * functions that plug into struct v4l2_ioctl_ops, struct v4l2_file_operations
 * and struct vb2_ops.
 * They contain boilerplate code that most if not all drivers have to do
 * and so they simplify the driver code.
 */

/* The queue is busy if there is a owner and you are not that owner. */
static inline bool vb2_queue_is_busy(struct video_device *vdev, struct file *file)
{
	return vdev->queue->owner && vdev->queue->owner != file->private_data;
}

/* vb2 ioctl helpers */

int vb2_ioctl_reqbufs(struct file *file, void *priv,
			  struct v4l2_requestbuffers *p)
{
	struct video_device *vdev = video_devdata(file);
	int res = vb2_verify_memory_type(vdev->queue, p->memory, p->type);

	if (res)
		return res;
	if (vb2_queue_is_busy(vdev, file))
		return -EBUSY;
	res = vb2_core_reqbufs(vdev->queue, p->memory, (unsigned int *)&p->count);
	/* If count == 0, then the owner has released all buffers and he
	   is no longer owner of the queue. Otherwise we have a new owner. */
	if (res == 0)
		vdev->queue->owner = p->count ? file->private_data : NULL;
	return res;
}

int vb2_ioctl_create_bufs(struct file *file, void *priv,
			  struct v4l2_create_buffers *p)
{
	struct video_device *vdev = video_devdata(file);
	int res = vb2_verify_memory_type(vdev->queue, p->memory,
			p->format.type);

	p->index = vdev->queue->num_buffers;
	/*
	 * If count == 0, then just check if memory and type are valid.
	 * Any -EBUSY result from vb2_verify_memory_type can be mapped to 0.
	 */
	if (p->count == 0)
		return res != -EBUSY ? res : 0;
	if (res)
		return res;
	if (vb2_queue_is_busy(vdev, file))
		return -EBUSY;

	res = vb2_create_bufs(vdev->queue, p);
	if (res == 0)
		vdev->queue->owner = file->private_data;
	return res;
}

int vb2_ioctl_prepare_buf(struct file *file, void *priv,
			  struct v4l2_buffer *p)
{
	struct video_device *vdev = video_devdata(file);

	if (vb2_queue_is_busy(vdev, file))
		return -EBUSY;
	return vb2_prepare_buf(vdev->queue, p);
}

int vb2_ioctl_querybuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
	struct video_device *vdev = video_devdata(file);

	/* No need to call vb2_queue_is_busy(), anyone can query buffers. */
	return vb2_querybuf(vdev->queue, p);
}

int vb2_ioctl_qbuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
	struct video_device *vdev = video_devdata(file);

	if (vb2_queue_is_busy(vdev, file))
		return -EBUSY;
	return vb2_qbuf(vdev->queue, p);
}

int vb2_ioctl_dqbuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
	struct video_device *vdev = video_devdata(file);

#if 0
	printf("%s line %d, ctrl_hadnler = 0x%lx, name %s\n", \
		__func__, __LINE__, (unsigned long)vdev->v4l2_dev->ctrl_handler,vdev->v4l2_dev->name);
#endif
	if (vb2_queue_is_busy(vdev, file))
		return -EBUSY;
	return vb2_dqbuf(vdev->queue, p, file->f_flags & O_NONBLOCK);
}

int vb2_ioctl_streamon(struct file *file, void *priv, enum v4l2_buf_type i)
{
	struct video_device *vdev = video_devdata(file);

	if (vb2_queue_is_busy(vdev, file))
		return -EBUSY;
	return vb2_streamon(vdev->queue, i);
}

int vb2_ioctl_streamoff(struct file *file, void *priv, enum v4l2_buf_type i)
{
	struct video_device *vdev = video_devdata(file);

	if (vb2_queue_is_busy(vdev, file))
		return -EBUSY;
	return vb2_streamoff(vdev->queue, i);
}

int vb2_ioctl_expbuf(struct file *file, void *priv, struct v4l2_exportbuffer *p)
{
	struct video_device *vdev = video_devdata(file);

	if (vb2_queue_is_busy(vdev, file))
		return -EBUSY;
	return vb2_expbuf(vdev->queue, p);
}

/* v4l2_file_operations helpers */

int vb2_fop_mmap(struct file *file, struct map_struct *mp)
{
	struct video_device *vdev = video_devdata(file);

	return vb2_mmap(vdev->queue, mp);
}

int _vb2_fop_release(struct file *file, struct rt_mutex *lock)
{
	struct video_device *vdev = video_devdata(file);

	if (lock)
		rt_mutex_take(lock, RT_WAITING_FOREVER);
	if (file->private_data == vdev->queue->owner) {
		vb2_queue_release(vdev->queue);
		vdev->queue->owner = NULL;
	}
	if (lock)
		rt_mutex_release(lock);
	return v4l2_fh_release(file);
}

int vb2_fop_release(struct file *file)
{
	struct video_device *vdev = video_devdata(file);
	struct rt_mutex *lock = vdev->queue->lock ? vdev->queue->lock : vdev->lock;

	return _vb2_fop_release(file, lock);
}

ssize_t vb2_fop_write(struct file *file, const char  *buf,
		size_t count, loff_t *ppos)
{
	struct video_device *vdev = video_devdata(file);
	struct rt_mutex *lock = vdev->queue->lock ? vdev->queue->lock : vdev->lock;
	int err = -EBUSY;

	if (!(vdev->queue->io_modes & VB2_WRITE))
		return -EINVAL;
	if (lock && rt_mutex_take(lock, RT_WAITING_FOREVER))
		return -EINVAL;
	if (vb2_queue_is_busy(vdev, file))
		goto exit;
	err = vb2_write(vdev->queue, buf, count, ppos,
		       file->f_flags & O_NONBLOCK);
	if (vdev->queue->fileio)
		vdev->queue->owner = file->private_data;
exit:
	if (lock)
		rt_mutex_release(lock);

	return err;
}

ssize_t vb2_fop_read(struct file *file, char  *buf,
		size_t count, loff_t *ppos)
{
	struct video_device *vdev = video_devdata(file);
	struct rt_mutex *lock = vdev->queue->lock ? vdev->queue->lock : vdev->lock;
	int err = -EBUSY;

	if (!(vdev->queue->io_modes & VB2_READ))
		return -EINVAL;
	if (lock && rt_mutex_take(lock, RT_WAITING_FOREVER))
		return -EINVAL;

	if (vb2_queue_is_busy(vdev, file))
		goto exit;
	err = vb2_read(vdev->queue, buf, count, ppos,
		       file->f_flags & O_NONBLOCK);
	if (vdev->queue->fileio)
		vdev->queue->owner = file->private_data;
exit:
	if (lock)
		rt_mutex_release(lock);

	return err;
}

unsigned int vb2_fop_poll(struct file *file, struct rt_pollreq *wait)
{
	struct video_device *vdev = video_devdata(file);
	struct vb2_queue *q = vdev->queue;
	struct rt_mutex *lock = q->lock ? q->lock : vdev->lock;
	unsigned res;
	void *fileio;
	/*
	 * If this helper doesn't know how to lock, then you shouldn't be using
	 * it but you should write your own.
	 */
	WARN_ON(!lock);

	if (lock && rt_mutex_take(lock, RT_WAITING_FOREVER))
		return POLLERR;

	fileio = q->fileio;

	res = vb2_poll(vdev->queue, file, wait);

	/* If fileio was started, then we have a new queue owner. */
	if (!fileio && q->fileio)
		q->owner = file->private_data;
	if (lock)
		rt_mutex_release(lock);
	return res;
}

#ifndef CONFIG_MMU
unsigned long vb2_fop_get_unmapped_area(struct file *file, unsigned long addr,
		unsigned long len, unsigned long pgoff, unsigned long flags)
{
	struct video_device *vdev = video_devdata(file);

	return vb2_get_unmapped_area(vdev->queue, addr, len, pgoff, flags);
}
#endif

/* vb2_ops helpers. Only use if vq->lock is non-NULL. */

void vb2_ops_wait_prepare(struct vb2_queue *vq)
{
	rt_mutex_release(vq->lock);
}

void vb2_ops_wait_finish(struct vb2_queue *vq)
{
	rt_mutex_take(vq->lock, RT_WAITING_FOREVER);
}

