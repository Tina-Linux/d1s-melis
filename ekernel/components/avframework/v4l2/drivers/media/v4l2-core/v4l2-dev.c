/*
 * Video capture interface for Linux version 2
 *
 *	A generic video device interface for the LINUX operating system
 *	using a set of device structures/vectors for low level operations.
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 *
 * Authors:	Alan Cox, <alan@lxorguk.ukuu.org.uk> (version 1)
 *              Mauro Carvalho Chehab <mchehab@infradead.org> (version 2)
 *
 * Fixes:	20000516  Claudio Matsuoka <claudio@conectiva.com>
 *		- Added procfs support
 */

#include <string.h>
#include <port.h>
#include <errno.h>
#include <init.h>
#include <melis/log.h>
#include <dfs_file.h>
#include <rtdevice.h>
#include <dfs_poll.h>
#include <linux/media.h>
#include "v4l2-common.h"
#include "v4l2-device.h"
#include "v4l2-ioctl.h"
#include <linux/atomic.h>
#include <linux/poll.h>
#include <linux/kern_levels.h>

#define VIDEO_NUM_DEVICES	32
#define VIDEO_NAME              "video4linux"

/*
 *Active devices
 */
static struct video_device *video_device[VIDEO_NUM_DEVICES];
static struct rt_mutex videodev_lock;
static DECLARE_BITMAP(devnode_nums[VFL_TYPE_MAX], VIDEO_NUM_DEVICES);
struct file *vdev_files[VIDEO_NUM_DEVICES] = {0};

/* Device node utility functions */

/* Note: these utility functions all assume that vfl_type is in the range
   [0, VFL_TYPE_MAX-1]. */

#ifdef CONFIG_VIDEO_FIXED_MINOR_RANGES
/* Return the bitmap corresponding to vfl_type. */
static inline unsigned long *devnode_bits(int vfl_type)
{
	/* Any types not assigned to fixed minor ranges must be mapped to
	   one single bitmap for the purposes of finding a free node number
	   since all those unassigned types use the same minor range. */
	int idx = (vfl_type > VFL_TYPE_RADIO) ? VFL_TYPE_MAX - 1 : vfl_type;

	return devnode_nums[idx];
}
#else
/* Return the bitmap corresponding to vfl_type. */
static inline unsigned long *devnode_bits(int vfl_type)
{
	return devnode_nums[vfl_type];
}
#endif

/* Mark device node number vdev->num as used */
static inline void devnode_set(struct video_device *vdev)
{
	set_bit(vdev->num, devnode_bits(vdev->vfl_type));
}

/* Mark device node number vdev->num as unused */
static inline void devnode_clear(struct video_device *vdev)
{
	clear_bit(vdev->num, devnode_bits(vdev->vfl_type));
}

/* Try to find a free device node number in the range [from, to> */
static inline int devnode_find(struct video_device *vdev, int from, int to)
{
	return find_next_zero_bit(devnode_bits(vdev->vfl_type), to, from);
}

struct video_device *video_device_alloc(void)
{
	struct video_device *p = (struct video_device *)rt_malloc(sizeof(struct video_device));
	if(p)
	{
		memset(p, 0x00, sizeof(struct video_device));
	}

	return p;
}

void video_device_release(struct video_device *vdev)
{
	rt_free(vdev);
}

void video_device_release_empty(struct video_device *vdev)
{
	/* Do nothing */
	/* Only valid when the video_device struct is a static. */
}

static inline void video_get(struct video_device *vdev)
{
	vdev->dev.ref_count++;
}

static inline void video_put(struct video_device *vdev)
{
	vdev->dev.ref_count--;
}

static inline int video_is_noref(struct video_device *vdev)
{
	return !vdev->dev.ref_count;
}

/* Called when the last user of the video device exits. */
static void v4l2_device_release(struct rt_device *cd)
{
	struct video_device *vdev = to_video_device(cd);
	struct v4l2_device *v4l2_dev = vdev->v4l2_dev;

	rt_mutex_take(&videodev_lock, RT_WAITING_FOREVER);
	if (WARN_ON(video_device[vdev->minor] != vdev)) {
		/* should not happen */
		rt_mutex_release(&videodev_lock);
		return;
	}

	/* Free up this device for reuse */
	video_device[vdev->minor] = NULL;

	/* Delete the cdev on this minor as well */
	//cdev_del(vdev->cdev);
	/* Just in case some driver tries to access this from
	   the release() callback. */
	//vdev->cdev = NULL;

	/* Mark device node number as free */
	devnode_clear(vdev);

	rt_mutex_release(&videodev_lock);

#if defined(CONFIG_MEDIA_CONTROLLER)
	if (v4l2_dev->mdev) {
		/* Remove interfaces and interface links */
		media_devnode_remove(vdev->intf_devnode);
		if (vdev->entity.function != MEDIA_ENT_F_UNKNOWN)
			media_device_unregister_entity(&vdev->entity);
	}
#endif

	/* Do not call v4l2_device_put if there is no release callback set.
	 * Drivers that have no v4l2_device release callback might free the
	 * v4l2_dev instance in the video_device release callback below, so we
	 * must perform this check here.
	 *
	 * TODO: In the long run all drivers that use v4l2_device should use the
	 * v4l2_device release callback. This check will then be unnecessary.
	 */
	if (v4l2_dev->release == NULL)
		v4l2_dev = NULL;

	/* Release video_device and perform other
	   cleanups as needed. */
	vdev->release(vdev);

	/* Decrease v4l2_device refcount */
	if (v4l2_dev)
		v4l2_device_put(v4l2_dev);
}

/*
 * compatible linux struct file
 */
struct video_device *video_devdata(struct file *file)
{
	int i;
	struct video_device *vdev;

	for (i = 0; i < VIDEO_NUM_DEVICES; i++) {
		if (vdev_files[i] == file)
			return video_device[i];
	}

	return NULL;
}


/* Priority handling */

static inline bool prio_is_valid(enum v4l2_priority prio)
{
	return prio == V4L2_PRIORITY_BACKGROUND ||
	       prio == V4L2_PRIORITY_INTERACTIVE ||
	       prio == V4L2_PRIORITY_RECORD;
}

void v4l2_prio_init(struct v4l2_prio_state *global)
{
	memset(global, 0, sizeof(*global));
}

int v4l2_prio_change(struct v4l2_prio_state *global, enum v4l2_priority *local,
		     enum v4l2_priority new)
{
	if (!prio_is_valid(new))
		return -EINVAL;
	if (*local == new)
		return 0;

	atomic_inc(&global->prios[new]);
	if (prio_is_valid(*local))
		atomic_dec(&global->prios[*local]);
	*local = new;
	return 0;
}

void v4l2_prio_open(struct v4l2_prio_state *global, enum v4l2_priority *local)
{
	v4l2_prio_change(global, local, V4L2_PRIORITY_DEFAULT);
}

void v4l2_prio_close(struct v4l2_prio_state *global, enum v4l2_priority local)
{
	if (prio_is_valid(local))
		atomic_dec(&global->prios[local]);
}

enum v4l2_priority v4l2_prio_max(struct v4l2_prio_state *global)
{
	if (atomic_read(&global->prios[V4L2_PRIORITY_RECORD]) > 0)
		return V4L2_PRIORITY_RECORD;
	if (atomic_read(&global->prios[V4L2_PRIORITY_INTERACTIVE]) > 0)
		return V4L2_PRIORITY_INTERACTIVE;
	if (atomic_read(&global->prios[V4L2_PRIORITY_BACKGROUND]) > 0)
		return V4L2_PRIORITY_BACKGROUND;
	return V4L2_PRIORITY_UNSET;
}

int v4l2_prio_check(struct v4l2_prio_state *global, enum v4l2_priority local)
{
	return (local < v4l2_prio_max(global)) ? -EBUSY : 0;
}

static int v4l2_read(struct dfs_fd *dfp, void *buf, size_t sz)
{
	struct video_device *vdev = dfs_to_vdev(dfp);
	int ret = -ENODEV;
	loff_t off = dfp->pos;

	video_file(dfp)->f_pos = dfp->pos;

	if (!vdev->fops->read)
		return -EINVAL;
	if (video_is_registered(vdev))
		ret = vdev->fops->read(video_file(dfp), (char *)buf, sz, &off);
	if ((vdev->dev_debug & V4L2_DEV_DEBUG_FOP) &&
	    (vdev->dev_debug & V4L2_DEV_DEBUG_STREAMING))
		printk(KERN_DEBUG "%s: read: %zd (%d)\n",
			video_device_node_name(vdev), sz, ret);
	return ret;
}

static int v4l2_write(struct dfs_fd *dfp, const void *buf, size_t sz)
{
	struct video_device *vdev = dfs_to_vdev(dfp);
	int ret = -ENODEV;
	loff_t off = dfp->pos;
	
	video_file(dfp)->f_pos = dfp->pos;

	if (!vdev->fops->write)
		return -EINVAL;
	if (video_is_registered(vdev))
		ret = vdev->fops->write(video_file(dfp), (const char *)buf, sz, &off);
	if ((vdev->dev_debug & V4L2_DEV_DEBUG_FOP) &&
	    (vdev->dev_debug & V4L2_DEV_DEBUG_STREAMING))
		printk(KERN_DEBUG "%s: write: %zd (%d)\n",
			video_device_node_name(vdev), sz, ret);
	return ret;
}

static int v4l2_poll(struct dfs_fd *dfp, struct rt_pollreq *poll)
{
	struct video_device *vdev = dfs_to_vdev(dfp);
	unsigned int res = POLLERR | POLLHUP;

	if (!vdev->fops->poll)
		return DEFAULT_POLLMASK;
	if (video_is_registered(vdev))
		res = vdev->fops->poll(video_file(dfp), poll);
	if (vdev->dev_debug & V4L2_DEV_DEBUG_POLL)
		printk(KERN_DEBUG "%s: poll: %08x\n",
			video_device_node_name(vdev), res);
	return res;
}

static int v4l2_ioctl(struct dfs_fd *dfp, int cmd, void *arg)
{
	struct video_device *vdev = dfs_to_vdev(dfp);
	unsigned long usr_arg = (unsigned long)arg;
	int ret = -ENODEV;

	if (vdev->fops->ioctl) {
		rt_mutex_t lock = v4l2_ioctl_get_lock(vdev, cmd);

		if (lock && rt_mutex_take(lock, RT_WAITING_FOREVER))
			return -ERESTARTSYS;
		if (video_is_registered(vdev))
			ret = vdev->fops->ioctl(video_file(dfp), (unsigned int)cmd, usr_arg);
		if (lock)
			rt_mutex_release(lock);
	} else
		ret = -ENOTTY;

	return ret;
}

static int v4l2_mmap(struct dfs_fd *dfp, struct map_struct *mp)
{
	struct video_device *vdev = dfs_to_vdev(dfp);
	int ret = -ENODEV;

	if (!vdev->fops->mmap)
		return -ENODEV;

	if (video_is_registered(vdev))
		ret = vdev->fops->mmap(video_file(dfp), mp);
	if (vdev->dev_debug & V4L2_DEV_DEBUG_FOP)
		printk(KERN_DEBUG "%s: mmap (%d)\n",
			video_device_node_name(vdev), ret);
	return ret;
}

/* Override for the open function */
static int v4l2_open(struct dfs_fd *dfp)
{
	struct video_device *vdev = dfs_to_vdev(dfp);
	struct file *filp = NULL;
	int ret = 0;

	/* Check if the video device is available */
	rt_mutex_take(&videodev_lock, RT_WAITING_FOREVER);
	/* return ENODEV if the video device has already been removed. */
	if (vdev == NULL || !video_is_registered(vdev)) {
		rt_mutex_release(&videodev_lock);
		return -ENODEV;
	}

	/* and increase the device refcount */
	video_get(vdev);

	rt_mutex_release(&videodev_lock);

	if (vdev->dev.ref_count > 1)
		return 0;

	filp = rt_malloc(sizeof(struct file));
	if (!filp) {
		return -ENOMEM;
	}

	memset(filp, 0x00, sizeof(struct file));

	filp->dfd     = dfp;
	filp->f_flags = dfp->flags;
	filp->f_pos   = dfp->pos;

	vdev_files[vdev->minor] = filp;

	if (vdev->fops->open) {
		if (video_is_registered(vdev))
			ret = vdev->fops->open(video_file(dfp));
		else
			ret = -ENODEV;
	}

	if (vdev->dev_debug & V4L2_DEV_DEBUG_FOP)
		printk(KERN_DEBUG "%s: open (%d)\n",
			video_device_node_name(vdev), ret);
	/* decrease the refcount in case of an error */
	if (ret) {
		video_put(vdev);
		vdev_files[vdev->minor] = NULL;
		rt_free(filp);
	}

	return ret;
}

/* Override for the release function */
static int v4l2_close(struct dfs_fd *dfp)
{
	struct video_device *vdev = dfs_to_vdev(dfp);
	struct file *filp = video_file(dfp);
	int ret = 0;

	video_put(vdev);

	if (!video_is_noref(vdev))
		return 0;

	if (vdev->fops->release)
		ret = vdev->fops->release(filp);

	if (vdev->dev_debug & V4L2_DEV_DEBUG_FOP)
		printk(KERN_DEBUG "%s: release\n",
			video_device_node_name(vdev));

	/* decrease the refcount unconditionally since the release()
	   return value is ignored. */
	//v4l2_device_release(&vdev->dev);
	rt_free(filp);
	vdev_files[vdev->minor] = NULL;
	
	return ret;
}

static const struct dfs_file_ops v4l2_fops = {
	.read                = v4l2_read,
	.write               = v4l2_write,
	.open                = v4l2_open,
	.mmap                = v4l2_mmap,
	.ioctl               = v4l2_ioctl,
	.close               = v4l2_close,
	.poll                = v4l2_poll,
	.lseek               = NULL,
	.ftruncate           = NULL,
	.getdents            = NULL,
	.flush               = NULL,
};

/**
 * get_index - assign stream index number based on v4l2_dev
 * @vdev: video_device to assign index number to, vdev->v4l2_dev should be assigned
 *
 * Note that when this is called the new device has not yet been registered
 * in the video_device array, but it was able to obtain a minor number.
 *
 * This means that we can always obtain a free stream index number since
 * the worst case scenario is that there are VIDEO_NUM_DEVICES - 1 slots in
 * use of the video_device array.
 *
 * Returns a free index number.
 */
static int get_index(struct video_device *vdev)
{
	/* This can be static since this function is called with the global
	   videodev_lock held. */
	static DECLARE_BITMAP(used, VIDEO_NUM_DEVICES);
	int i;

	bitmap_zero(used, VIDEO_NUM_DEVICES);

	for (i = 0; i < VIDEO_NUM_DEVICES; i++) {
		if (video_device[i] != NULL &&
		    video_device[i]->v4l2_dev == vdev->v4l2_dev) {
			set_bit(video_device[i]->index, used);
		}
	}

	return find_first_zero_bit(used, VIDEO_NUM_DEVICES);
}

#define SET_VALID_IOCTL(ops, cmd, op)			\
	if (ops->op)					\
		set_bit(_IOC_NR(cmd), valid_ioctls)

/* This determines which ioctls are actually implemented in the driver.
   It's a one-time thing which simplifies video_ioctl2 as it can just do
   a bit test.

   Note that drivers can override this by setting bits to 1 in
   vdev->valid_ioctls. If an ioctl is marked as 1 when this function is
   called, then that ioctl will actually be marked as unimplemented.

   It does that by first setting up the local valid_ioctls bitmap, and
   at the end do a:

   vdev->valid_ioctls = valid_ioctls & ~(vdev->valid_ioctls)
 */
static void determine_valid_ioctls(struct video_device *vdev)
{
	DECLARE_BITMAP(valid_ioctls, BASE_VIDIOC_PRIVATE);
	const struct v4l2_ioctl_ops *ops = vdev->ioctl_ops;
	bool is_vid = vdev->vfl_type == VFL_TYPE_GRABBER;
	bool is_vbi = vdev->vfl_type == VFL_TYPE_VBI;
	bool is_radio = vdev->vfl_type == VFL_TYPE_RADIO;
	bool is_sdr = vdev->vfl_type == VFL_TYPE_SDR;
	bool is_tch = vdev->vfl_type == VFL_TYPE_TOUCH;
	bool is_rx = vdev->vfl_dir != VFL_DIR_TX;
	bool is_tx = vdev->vfl_dir != VFL_DIR_RX;

	bitmap_zero(valid_ioctls, BASE_VIDIOC_PRIVATE);

	/* vfl_type and vfl_dir independent ioctls */

	SET_VALID_IOCTL(ops, VIDIOC_QUERYCAP, vidioc_querycap);
	set_bit(_IOC_NR(VIDIOC_G_PRIORITY), valid_ioctls);
	set_bit(_IOC_NR(VIDIOC_S_PRIORITY), valid_ioctls);

	/* Note: the control handler can also be passed through the filehandle,
	   and that can't be tested here. If the bit for these control ioctls
	   is set, then the ioctl is valid. But if it is 0, then it can still
	   be valid if the filehandle passed the control handler. */
	if (vdev->ctrl_handler || ops->vidioc_queryctrl)
		set_bit(_IOC_NR(VIDIOC_QUERYCTRL), valid_ioctls);
	if (vdev->ctrl_handler || ops->vidioc_query_ext_ctrl)
		set_bit(_IOC_NR(VIDIOC_QUERY_EXT_CTRL), valid_ioctls);
	if (vdev->ctrl_handler || ops->vidioc_g_ctrl || ops->vidioc_g_ext_ctrls)
		set_bit(_IOC_NR(VIDIOC_G_CTRL), valid_ioctls);
	if (vdev->ctrl_handler || ops->vidioc_s_ctrl || ops->vidioc_s_ext_ctrls)
		set_bit(_IOC_NR(VIDIOC_S_CTRL), valid_ioctls);
	if (vdev->ctrl_handler || ops->vidioc_g_ext_ctrls)
		set_bit(_IOC_NR(VIDIOC_G_EXT_CTRLS), valid_ioctls);
	if (vdev->ctrl_handler || ops->vidioc_s_ext_ctrls)
		set_bit(_IOC_NR(VIDIOC_S_EXT_CTRLS), valid_ioctls);
	if (vdev->ctrl_handler || ops->vidioc_try_ext_ctrls)
		set_bit(_IOC_NR(VIDIOC_TRY_EXT_CTRLS), valid_ioctls);
	if (vdev->ctrl_handler || ops->vidioc_querymenu)
		set_bit(_IOC_NR(VIDIOC_QUERYMENU), valid_ioctls);
#ifdef CONFIG_VIDEO_RADIO
	SET_VALID_IOCTL(ops, VIDIOC_G_FREQUENCY, vidioc_g_frequency);
	SET_VALID_IOCTL(ops, VIDIOC_S_FREQUENCY, vidioc_s_frequency);
#endif
	SET_VALID_IOCTL(ops, VIDIOC_LOG_STATUS, vidioc_log_status);
#ifdef CONFIG_VIDEO_ADV_DEBUG
	set_bit(_IOC_NR(VIDIOC_DBG_G_CHIP_INFO), valid_ioctls);
	set_bit(_IOC_NR(VIDIOC_DBG_G_REGISTER), valid_ioctls);
	set_bit(_IOC_NR(VIDIOC_DBG_S_REGISTER), valid_ioctls);
#endif
	/* yes, really vidioc_subscribe_event */
	SET_VALID_IOCTL(ops, VIDIOC_DQEVENT, vidioc_subscribe_event);
	SET_VALID_IOCTL(ops, VIDIOC_SUBSCRIBE_EVENT, vidioc_subscribe_event);
	SET_VALID_IOCTL(ops, VIDIOC_UNSUBSCRIBE_EVENT, vidioc_unsubscribe_event);

#ifdef CONFIG_VIDEO_RADIO
	if (ops->vidioc_enum_freq_bands || ops->vidioc_g_tuner || ops->vidioc_g_modulator)
		set_bit(_IOC_NR(VIDIOC_ENUM_FREQ_BANDS), valid_ioctls);
#endif

	if (is_vid || is_tch) {
		/* video specific ioctls */
		if ((is_rx && (ops->vidioc_enum_fmt_vid_cap ||
			       ops->vidioc_enum_fmt_vid_cap_mplane ||
			       ops->vidioc_enum_fmt_vid_overlay)) ||
		    (is_tx && (ops->vidioc_enum_fmt_vid_out ||
			       ops->vidioc_enum_fmt_vid_out_mplane)))
			set_bit(_IOC_NR(VIDIOC_ENUM_FMT), valid_ioctls);
		if ((is_rx && (ops->vidioc_g_fmt_vid_cap ||
			       ops->vidioc_g_fmt_vid_cap_mplane ||
			       ops->vidioc_g_fmt_vid_overlay)) ||
		    (is_tx && (ops->vidioc_g_fmt_vid_out ||
			       ops->vidioc_g_fmt_vid_out_mplane ||
			       ops->vidioc_g_fmt_vid_out_overlay)))
			 set_bit(_IOC_NR(VIDIOC_G_FMT), valid_ioctls);
		if ((is_rx && (ops->vidioc_s_fmt_vid_cap ||
			       ops->vidioc_s_fmt_vid_cap_mplane ||
			       ops->vidioc_s_fmt_vid_overlay)) ||
		    (is_tx && (ops->vidioc_s_fmt_vid_out ||
			       ops->vidioc_s_fmt_vid_out_mplane ||
			       ops->vidioc_s_fmt_vid_out_overlay)))
			 set_bit(_IOC_NR(VIDIOC_S_FMT), valid_ioctls);
		if ((is_rx && (ops->vidioc_try_fmt_vid_cap ||
			       ops->vidioc_try_fmt_vid_cap_mplane ||
			       ops->vidioc_try_fmt_vid_overlay)) ||
		    (is_tx && (ops->vidioc_try_fmt_vid_out ||
			       ops->vidioc_try_fmt_vid_out_mplane ||
			       ops->vidioc_try_fmt_vid_out_overlay)))
			 set_bit(_IOC_NR(VIDIOC_TRY_FMT), valid_ioctls);
		SET_VALID_IOCTL(ops, VIDIOC_OVERLAY, vidioc_overlay);
		SET_VALID_IOCTL(ops, VIDIOC_G_FBUF, vidioc_g_fbuf);
		SET_VALID_IOCTL(ops, VIDIOC_S_FBUF, vidioc_s_fbuf);
#ifdef CONFIG_VIDEO_JPEG
		SET_VALID_IOCTL(ops, VIDIOC_G_JPEGCOMP, vidioc_g_jpegcomp);
		SET_VALID_IOCTL(ops, VIDIOC_S_JPEGCOMP, vidioc_s_jpegcomp);
#endif
		SET_VALID_IOCTL(ops, VIDIOC_G_ENC_INDEX, vidioc_g_enc_index);
		SET_VALID_IOCTL(ops, VIDIOC_ENCODER_CMD, vidioc_encoder_cmd);
		SET_VALID_IOCTL(ops, VIDIOC_TRY_ENCODER_CMD, vidioc_try_encoder_cmd);
		SET_VALID_IOCTL(ops, VIDIOC_DECODER_CMD, vidioc_decoder_cmd);
		SET_VALID_IOCTL(ops, VIDIOC_TRY_DECODER_CMD, vidioc_try_decoder_cmd);
		SET_VALID_IOCTL(ops, VIDIOC_ENUM_FRAMESIZES, vidioc_enum_framesizes);
		SET_VALID_IOCTL(ops, VIDIOC_ENUM_FRAMEINTERVALS, vidioc_enum_frameintervals);
		if (ops->vidioc_g_crop || ops->vidioc_g_selection)
			set_bit(_IOC_NR(VIDIOC_G_CROP), valid_ioctls);
		if (ops->vidioc_s_crop || ops->vidioc_s_selection)
			set_bit(_IOC_NR(VIDIOC_S_CROP), valid_ioctls);
		SET_VALID_IOCTL(ops, VIDIOC_G_SELECTION, vidioc_g_selection);
		SET_VALID_IOCTL(ops, VIDIOC_S_SELECTION, vidioc_s_selection);
		if (ops->vidioc_cropcap || ops->vidioc_g_selection)
			set_bit(_IOC_NR(VIDIOC_CROPCAP), valid_ioctls);
	} else if (is_vbi) {
		/* vbi specific ioctls */
		if ((is_rx && (ops->vidioc_g_fmt_vbi_cap ||
			       ops->vidioc_g_fmt_sliced_vbi_cap)) ||
		    (is_tx && (ops->vidioc_g_fmt_vbi_out ||
			       ops->vidioc_g_fmt_sliced_vbi_out)))
			set_bit(_IOC_NR(VIDIOC_G_FMT), valid_ioctls);
		if ((is_rx && (ops->vidioc_s_fmt_vbi_cap ||
			       ops->vidioc_s_fmt_sliced_vbi_cap)) ||
		    (is_tx && (ops->vidioc_s_fmt_vbi_out ||
			       ops->vidioc_s_fmt_sliced_vbi_out)))
			set_bit(_IOC_NR(VIDIOC_S_FMT), valid_ioctls);
		if ((is_rx && (ops->vidioc_try_fmt_vbi_cap ||
			       ops->vidioc_try_fmt_sliced_vbi_cap)) ||
		    (is_tx && (ops->vidioc_try_fmt_vbi_out ||
			       ops->vidioc_try_fmt_sliced_vbi_out)))
			set_bit(_IOC_NR(VIDIOC_TRY_FMT), valid_ioctls);
		SET_VALID_IOCTL(ops, VIDIOC_G_SLICED_VBI_CAP, vidioc_g_sliced_vbi_cap);
	} else if (is_sdr && is_rx) {
#ifdef CONFIG_VIDEO_RADIO
		/* SDR receiver specific ioctls */
		if (ops->vidioc_enum_fmt_sdr_cap)
			set_bit(_IOC_NR(VIDIOC_ENUM_FMT), valid_ioctls);
		if (ops->vidioc_g_fmt_sdr_cap)
			set_bit(_IOC_NR(VIDIOC_G_FMT), valid_ioctls);
		if (ops->vidioc_s_fmt_sdr_cap)
			set_bit(_IOC_NR(VIDIOC_S_FMT), valid_ioctls);
		if (ops->vidioc_try_fmt_sdr_cap)
			set_bit(_IOC_NR(VIDIOC_TRY_FMT), valid_ioctls);
#endif
	} else if (is_sdr && is_tx) {
#ifdef CONFIG_VIDEO_RADIO
		/* SDR transmitter specific ioctls */
		if (ops->vidioc_enum_fmt_sdr_out)
			set_bit(_IOC_NR(VIDIOC_ENUM_FMT), valid_ioctls);
		if (ops->vidioc_g_fmt_sdr_out)
			set_bit(_IOC_NR(VIDIOC_G_FMT), valid_ioctls);
		if (ops->vidioc_s_fmt_sdr_out)
			set_bit(_IOC_NR(VIDIOC_S_FMT), valid_ioctls);
		if (ops->vidioc_try_fmt_sdr_out)
			set_bit(_IOC_NR(VIDIOC_TRY_FMT), valid_ioctls);
#endif
	}

	if (is_vid || is_vbi || is_sdr || is_tch) {
		/* ioctls valid for video, vbi or sdr */
		SET_VALID_IOCTL(ops, VIDIOC_REQBUFS, vidioc_reqbufs);
		SET_VALID_IOCTL(ops, VIDIOC_QUERYBUF, vidioc_querybuf);
		SET_VALID_IOCTL(ops, VIDIOC_QBUF, vidioc_qbuf);
		SET_VALID_IOCTL(ops, VIDIOC_EXPBUF, vidioc_expbuf);
		SET_VALID_IOCTL(ops, VIDIOC_DQBUF, vidioc_dqbuf);
		SET_VALID_IOCTL(ops, VIDIOC_CREATE_BUFS, vidioc_create_bufs);
		SET_VALID_IOCTL(ops, VIDIOC_PREPARE_BUF, vidioc_prepare_buf);
		SET_VALID_IOCTL(ops, VIDIOC_STREAMON, vidioc_streamon);
		SET_VALID_IOCTL(ops, VIDIOC_STREAMOFF, vidioc_streamoff);
	}

	if (is_vid || is_vbi || is_tch) {
		/* ioctls valid for video or vbi */
		if (ops->vidioc_s_std)
			set_bit(_IOC_NR(VIDIOC_ENUMSTD), valid_ioctls);
		SET_VALID_IOCTL(ops, VIDIOC_S_STD, vidioc_s_std);
		SET_VALID_IOCTL(ops, VIDIOC_G_STD, vidioc_g_std);
		if (is_rx) {
			SET_VALID_IOCTL(ops, VIDIOC_QUERYSTD, vidioc_querystd);
			SET_VALID_IOCTL(ops, VIDIOC_ENUMINPUT, vidioc_enum_input);
			SET_VALID_IOCTL(ops, VIDIOC_G_INPUT, vidioc_g_input);
			SET_VALID_IOCTL(ops, VIDIOC_S_INPUT, vidioc_s_input);
#ifdef CONFIG_VIDEO_RADIO
			SET_VALID_IOCTL(ops, VIDIOC_ENUMAUDIO, vidioc_enumaudio);
			SET_VALID_IOCTL(ops, VIDIOC_G_AUDIO, vidioc_g_audio);
			SET_VALID_IOCTL(ops, VIDIOC_S_AUDIO, vidioc_s_audio);
#endif
#ifdef CONFIG_VIDEO_DV
			SET_VALID_IOCTL(ops, VIDIOC_QUERY_DV_TIMINGS, vidioc_query_dv_timings);
#endif
#ifdef CONFIG_VIDEO_EDID
			SET_VALID_IOCTL(ops, VIDIOC_S_EDID, vidioc_s_edid);
#endif
		}
		if (is_tx) {
			SET_VALID_IOCTL(ops, VIDIOC_ENUMOUTPUT, vidioc_enum_output);
			SET_VALID_IOCTL(ops, VIDIOC_G_OUTPUT, vidioc_g_output);
			SET_VALID_IOCTL(ops, VIDIOC_S_OUTPUT, vidioc_s_output);
#ifdef CONFIG_VIDEO_RADIO
			SET_VALID_IOCTL(ops, VIDIOC_ENUMAUDOUT, vidioc_enumaudout);
			SET_VALID_IOCTL(ops, VIDIOC_G_AUDOUT, vidioc_g_audout);
			SET_VALID_IOCTL(ops, VIDIOC_S_AUDOUT, vidioc_s_audout);
#endif
		}
		if (ops->vidioc_g_parm || (vdev->vfl_type == VFL_TYPE_GRABBER &&
					ops->vidioc_g_std))
			set_bit(_IOC_NR(VIDIOC_G_PARM), valid_ioctls);
		SET_VALID_IOCTL(ops, VIDIOC_S_PARM, vidioc_s_parm);
#ifdef CONFIG_VIDEO_DV
		SET_VALID_IOCTL(ops, VIDIOC_S_DV_TIMINGS, vidioc_s_dv_timings);
		SET_VALID_IOCTL(ops, VIDIOC_G_DV_TIMINGS, vidioc_g_dv_timings);
		SET_VALID_IOCTL(ops, VIDIOC_ENUM_DV_TIMINGS, vidioc_enum_dv_timings);
		SET_VALID_IOCTL(ops, VIDIOC_DV_TIMINGS_CAP, vidioc_dv_timings_cap);
#endif
#ifdef CONFIG_VIDEO_EDID
		SET_VALID_IOCTL(ops, VIDIOC_G_EDID, vidioc_g_edid);
#endif
	}
#ifdef CONFIG_VIDEO_RADIO
	if (is_tx && (is_radio || is_sdr)) {
		/* radio transmitter only ioctls */
		SET_VALID_IOCTL(ops, VIDIOC_G_MODULATOR, vidioc_g_modulator);
		SET_VALID_IOCTL(ops, VIDIOC_S_MODULATOR, vidioc_s_modulator);
	}
	if (is_rx) {
		/* receiver only ioctls */
		SET_VALID_IOCTL(ops, VIDIOC_G_TUNER, vidioc_g_tuner);
		SET_VALID_IOCTL(ops, VIDIOC_S_TUNER, vidioc_s_tuner);
		SET_VALID_IOCTL(ops, VIDIOC_S_HW_FREQ_SEEK, vidioc_s_hw_freq_seek);
	}
#endif

	bitmap_andnot(vdev->valid_ioctls, valid_ioctls, vdev->valid_ioctls,
			BASE_VIDIOC_PRIVATE);
}

static int video_register_media_controller(struct video_device *vdev, int type)
{
#if defined(CONFIG_MEDIA_CONTROLLER)
	u32 intf_type;
	int ret;

	if (!vdev->v4l2_dev->mdev)
		return 0;

	vdev->entity.obj_type = MEDIA_ENTITY_TYPE_VIDEO_DEVICE;
	vdev->entity.function = MEDIA_ENT_F_UNKNOWN;

	switch (type) {
	case VFL_TYPE_GRABBER:
		intf_type = MEDIA_INTF_T_V4L_VIDEO;
		vdev->entity.function = MEDIA_ENT_F_IO_V4L;
		break;
	case VFL_TYPE_VBI:
		intf_type = MEDIA_INTF_T_V4L_VBI;
		vdev->entity.function = MEDIA_ENT_F_IO_VBI;
		break;
	case VFL_TYPE_TOUCH:
		intf_type = MEDIA_INTF_T_V4L_TOUCH;
		vdev->entity.function = MEDIA_ENT_F_IO_V4L;
		break;
#ifdef CONFIG_VIDEO_RADIO
	case VFL_TYPE_SDR:
		intf_type = MEDIA_INTF_T_V4L_SWRADIO;
		vdev->entity.function = MEDIA_ENT_F_IO_SWRADIO;
		break;
	case VFL_TYPE_RADIO:
		intf_type = MEDIA_INTF_T_V4L_RADIO;
		/*
		 * Radio doesn't have an entity at the V4L2 side to represent
		 * radio input or output. Instead, the audio input/output goes
		 * via either physical wires or ALSA.
		 */
		break;
#endif
	case VFL_TYPE_SUBDEV:
		intf_type = MEDIA_INTF_T_V4L_SUBDEV;
		/* Entity will be created via v4l2_device_register_subdev() */
		break;
	default:
		return 0;
	}

	if (vdev->entity.function != MEDIA_ENT_F_UNKNOWN) {
		vdev->entity.name = vdev->name;

		/* Needed just for backward compatibility with legacy MC API */
		vdev->entity.info.dev.major = VIDEO_MAJOR + vdev->num;
		vdev->entity.info.dev.minor = vdev->minor;

		ret = media_device_register_entity(vdev->v4l2_dev->mdev,
						   &vdev->entity);
		if (ret < 0) {
			printk(KERN_WARNING
				"%s: media_device_register_entity failed\n",
				__func__);
			return ret;
		}
	}

	vdev->intf_devnode = media_devnode_create(vdev->v4l2_dev->mdev,
						  intf_type,
						  0, VIDEO_MAJOR,
						  vdev->minor);
	if (!vdev->intf_devnode) {
		media_device_unregister_entity(&vdev->entity);
		return -ENOMEM;
	}

	if (vdev->entity.function != MEDIA_ENT_F_UNKNOWN) {
		struct media_link *link;

		link = media_create_intf_link(&vdev->entity,
					      &vdev->intf_devnode->intf,
					      MEDIA_LNK_FL_ENABLED);
		if (!link) {
			media_devnode_remove(vdev->intf_devnode);
			media_device_unregister_entity(&vdev->entity);
			return -ENOMEM;
		}
	}

	/* FIXME: how to create the other interface links? */

#endif
	return 0;
}

int __video_register_device(struct video_device *vdev, int type, int nr,
		int warn_if_nr_in_use)
{
	int i = 0;
	int ret;
	int minor_offset = 0;
	int minor_cnt = VIDEO_NUM_DEVICES;
	const char *name_base;
	unsigned char devname[RT_NAME_MAX];

	/* A minor value of -1 marks this video device as never
	   having been registered */
	vdev->minor = -1;

	/* the release callback MUST be present */
	if (WARN_ON(!vdev->release))
		return -EINVAL;
	/* the v4l2_dev pointer MUST be present */
	if (WARN_ON(!vdev->v4l2_dev))
		return -EINVAL;

	/* v4l2_fh support */
	//rt_hw_spin_lock_init(&vdev->fh_lock);
	INIT_LIST_HEAD(&vdev->fh_list);

	/* Part 1: check device type */
	switch (type) {
	case VFL_TYPE_GRABBER:
		name_base = "video";
		break;
	case VFL_TYPE_VBI:
		name_base = "vbi";
		break;
#ifdef CONFIG_VIDEO_RADIO
	case VFL_TYPE_RADIO:
		name_base = "radio";
		break;
	case VFL_TYPE_SDR:
		/* Use device name 'swradio' because 'sdr' was already taken. */
		name_base = "swradio";
		break;
#endif
	case VFL_TYPE_SUBDEV:
		name_base = "v4l-subdev";
		break;
	case VFL_TYPE_TOUCH:
		name_base = "v4l-touch";
		break;
	default:
		printk(KERN_ERR "%s called with unknown type: %d\n",
		       __func__, type);
		return -EINVAL;
	}

	vdev->vfl_type = type;
	//vdev->cdev = NULL;
#if 0
	if (vdev->dev_parent == NULL)
		vdev->dev_parent = vdev->v4l2_dev->dev;
#endif
	if (vdev->ctrl_handler == NULL)
		vdev->ctrl_handler = vdev->v4l2_dev->ctrl_handler;
	/* If the prio state pointer is NULL, then use the v4l2_device
	   prio state. */
	if (vdev->prio == NULL)
		vdev->prio = &vdev->v4l2_dev->prio;

	/* Part 2: find a free minor, device node number and device index. */
#ifdef CONFIG_VIDEO_FIXED_MINOR_RANGES
	/* Keep the ranges for the first four types for historical
	 * reasons.
	 * Newer devices (not yet in place) should use the range
	 * of 128-191 and just pick the first free minor there
	 * (new style). */
	switch (type) {
	case VFL_TYPE_GRABBER:
		minor_offset = 0;
		minor_cnt = 8;
		break;
	case VFL_TYPE_RADIO:
		minor_offset = 8;
		minor_cnt = 8;
		break;
	case VFL_TYPE_VBI:
		minor_offset = 16;
		minor_cnt = 8;
		break;
	default:
		minor_offset = 24;
		minor_cnt = 8;
		break;
	}
#endif

	/* Pick a device node number */
	rt_mutex_take(&videodev_lock, RT_WAITING_FOREVER);
	nr = devnode_find(vdev, nr == -1 ? 0 : nr, minor_cnt);
	if (nr == minor_cnt)
		nr = devnode_find(vdev, 0, minor_cnt);
	if (nr == minor_cnt) {
		printk(KERN_ERR "could not get a free device node number\n");
		rt_mutex_release(&videodev_lock);
		return -ENFILE;
	}
#ifdef CONFIG_VIDEO_FIXED_MINOR_RANGES
	/* 1-on-1 mapping of device node number to minor number */
	i = nr;
#else
	/* The device node number and minor numbers are independent, so
	   we just find the first free minor number. */
	for (i = 0; i < VIDEO_NUM_DEVICES; i++)
		if (video_device[i] == NULL)
			break;
	if (i == VIDEO_NUM_DEVICES) {
		rt_mutex_release(&videodev_lock);
		printk(KERN_ERR "could not get a free minor\n");
		return -ENFILE;
	}
#endif
	vdev->minor = i + minor_offset;
	vdev->num = nr;
	devnode_set(vdev);

	/* Should not happen since we thought this minor was free */
	WARN_ON(video_device[vdev->minor] != NULL);
	vdev->index = get_index(vdev);
	video_device[vdev->minor] = vdev;

	if(vdev->index != vdev->minor) {
		rt_mutex_release(&videodev_lock);
		printk(KERN_ERR "minor and index corrupted.\n");
		return -ENFILE;
	}

	rt_mutex_release(&videodev_lock);

	if (vdev->ioctl_ops) {
		determine_valid_ioctls(vdev);
	}

	/* Part 3: Initialize the character device */
	// Nothing!

	/* Part 4: register the device to rt-thread */
	snprintf(devname, RT_NAME_MAX, "%s%d", name_base, vdev->num);
	
	ret = rt_device_register(&vdev->dev, devname,
				RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
	if (ret < 0) {
		printk(KERN_ERR "%s: device_register failed\n", __func__);
		goto cleanup;
	}
	vdev->dev.fops = &v4l2_fops;

	/* Register the release callback that will be called when the last
	   reference to the device goes away. */
	//vdev->dev.release = v4l2_device_release;

	if (nr != -1 && nr != vdev->num && warn_if_nr_in_use)
		printk(KERN_WARNING "%s: requested %s%d, got %s\n", __func__,
			name_base, nr, video_device_node_name(vdev));

	/* Increase v4l2_device refcount */
	v4l2_device_get(vdev->v4l2_dev);

	/* Part 5: Register the entity. */
	ret = video_register_media_controller(vdev, type);

	/* Part 6: Activate this minor. The char device can now be used. */
	set_bit(V4L2_FL_REGISTERED, &vdev->flags);

	return 0;

cleanup:
	rt_mutex_take(&videodev_lock, RT_WAITING_FOREVER);
	video_device[vdev->minor] = NULL;
	devnode_clear(vdev);
	rt_mutex_release(&videodev_lock);

	/* Mark this video device as never having been registered. */
	vdev->minor = -1;
	return ret;
}

/**
 *	video_unregister_device - unregister a video4linux device
 *	@vdev: the device to unregister
 *
 *	This unregisters the passed device. Future open calls will
 *	be met with errors.
 */
void video_unregister_device(struct video_device *vdev)
{
	/* Check if vdev was ever registered at all */
	if (!vdev || !video_is_registered(vdev))
		return;

	rt_mutex_take(&videodev_lock, RT_WAITING_FOREVER);
	/* This must be in a critical section to prevent a race with v4l2_open.
	 * Once this bit has been cleared video_get may never be called again.
	 */
	clear_bit(V4L2_FL_REGISTERED, &vdev->flags);
	rt_mutex_release(&videodev_lock);
	v4l2_device_release(&vdev->dev);
	rt_device_unregister(&vdev->dev);
}

/*
 *Initialise video for linux
 */
static int videodev_init(void)
{
	rt_mutex_init(&videodev_lock, "videodev_lock", RT_IPC_FLAG_FIFO);
	return 0;
}

static void videodev_exit(void)
{
	rt_mutex_detach(&videodev_lock);
	return;
}

pure_initcall(videodev_init);
