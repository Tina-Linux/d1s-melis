/*
 * Media device node
 *
 * Copyright (C) 2010 Nokia Corporation
 *
 * Based on drivers/media/video/v4l2_dev.c code authored by
 *	Mauro Carvalho Chehab <mchehab@infradead.org> (version 2)
 *	Alan Cox, <alan@lxorguk.ukuu.org.uk> (version 1)
 *
 * Contacts: Laurent Pinchart <laurent.pinchart@ideasonboard.com>
 *	     Sakari Ailus <sakari.ailus@iki.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * --
 *
 * Generic media device node infrastructure to register and unregister
 * character devices using a dynamic major number and proper reference
 * counting.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <errno.h>
#include <init.h>
#include <string.h>

#include <dfs_poll.h>
//#include "v4l2-types.h"

#include <linux/poll.h>
#include <linux/mutex.h>
#include <linux/printk.h>

#include "media-devnode.h"
#include "media-device.h"

#define MEDIA_NUM_DEVICES	32
#define MEDIA_NAME		"media"

static dev_t media_dev_t;

/*
 *	Active devices
 */
static DEFINE_MUTEX(media_devnode_lock);
static DECLARE_BITMAP(media_devnode_nums, MEDIA_NUM_DEVICES);
struct file *mdev_files[MEDIA_NUM_DEVICES] = {NULL};

#if 0
/* Called when the last user of the media device exits. */
static void media_devnode_release(struct device *cd)
{
	struct media_devnode *devnode = to_media_devnode(cd);

//	rt_mutex_take(&media_devnode_lock, RT_WAITING_FOREVER);
	/* Mark device node number as free */
	clear_bit(devnode->minor, media_devnode_nums);
//	rt_mutex_release(&media_devnode_lock);

	/* Release media_devnode and perform other cleanups as needed. */
	if (devnode->release)
		devnode->release(devnode);

	rt_free(devnode);
	pr_debug("%s: Media Devnode Deallocated\n", __func__);
}

static struct bus_type media_bus_type = {
	.name = MEDIA_NAME,
};
#endif

static inline void media_get(struct media_devnode *devnode)
{
	devnode->dev.ref_count++;
}

static inline void media_put(struct media_devnode *devnode)
{
	devnode->dev.ref_count--;
}

static inline int media_is_noref(struct media_devnode *devnode)
{
	return !devnode->dev.ref_count;
}

static int media_read(struct dfs_fd *dfp, void *buf, size_t sz)
{
	struct media_devnode *devnode = dfs_to_mdev(dfp);
	loff_t off = dfp->pos;

	if (!devnode->fops->read)
		return -EINVAL;
	if (!media_devnode_is_registered(devnode))
		return -EIO;
	return devnode->fops->read(media_file(dfp), (char *)buf, sz, &off);
}

static int media_write(struct dfs_fd *dfp, const void *buf, size_t sz)
{
	struct media_devnode *devnode = dfs_to_mdev(dfp);
	loff_t off = dfp->pos;

	if (!devnode->fops->write)
		return -EINVAL;
	if (!media_devnode_is_registered(devnode))
		return -EIO;
	return devnode->fops->write(media_file(dfp), (const char *)buf, sz, &off);
}

static int media_poll(struct dfs_fd *dfp,
			       struct rt_pollreq *poll)
{
	struct media_devnode *devnode = dfs_to_mdev(dfp);

	if (!media_devnode_is_registered(devnode))
		return POLLERR | POLLHUP;
	if (!devnode->fops->poll)
		return DEFAULT_POLLMASK;
	return devnode->fops->poll(media_file(dfp), poll);
}

static long
__media_ioctl(struct dfs_fd *dfp, unsigned int cmd, unsigned long arg,
	      long (*ioctl_func)(struct file *filp, unsigned int cmd,
				 unsigned long arg))
{
	struct media_devnode *devnode = dfs_to_mdev(dfp);

	if (!ioctl_func)
		return -ENOTTY;

	if (!media_devnode_is_registered(devnode))
		return -EIO;

	return ioctl_func(media_file(dfp), cmd, arg);
}

static int media_ioctl(struct dfs_fd *dfp, int cmd, void *arg)
{
	struct media_devnode *devnode = dfs_to_mdev(dfp);
	unsigned long usr_arg = (unsigned long)arg;

	return __media_ioctl(dfp, (unsigned int)cmd, usr_arg, devnode->fops->ioctl);
}

/* Override for the open function */
static int media_open(struct dfs_fd *dfp)
{
	struct media_devnode *devnode;
	struct file *file;
	int ret;

	/* Check if the media device is available. This needs to be done with
	 * the media_devnode_lock held to prevent an open/unregister race:
	 * without the lock, the device could be unregistered and freed between
	 * the media_devnode_is_registered() and get_device() calls, leading to
	 * a crash.
	 */
	if (dfp->data == NULL)
		return -EINVAL;

//	rt_mutex_take(&media_devnode_lock, RT_WATTING_FOREVER);
	devnode = container_of(dfp->data, struct media_devnode, dev);
	/* return ENXIO if the media device has been removed
	   already or if it is not registered anymore. */
	if (!media_devnode_is_registered(devnode)) {
//		rt_mutex_release(&media_devnode_lock);
		return -ENXIO;
	}
	/* and increase the device refcount */
	media_get(devnode);
//	rt_mutex_release(&media_devnode_lock);
	if (devnode->dev.ref_count > 1) {
		return 0;
	}

	file = rt_malloc(sizeof(struct file));
	if (!file)
		return -ENOMEM;

	memset(file, 0x00, sizeof(sizeof(struct file)));
	mdev_files[devnode->minor] = file;
	file->private_data = devnode;

	if (devnode->fops->open) {
		ret = devnode->fops->open(file);
		if (ret) {
			media_put(devnode);
			file->private_data = NULL;
			rt_free(file);
			return ret;
		}
	}

	return 0;
}

/* Override for the release function */
static int media_release(struct dfs_fd *dfp)
{
	struct media_devnode *devnode = dfs_to_mdev(dfp);
	struct file *file = media_file(dfp);

	/* decrease the refcount unconditionally since the release()
	   return value is ignored. */
	media_put(devnode);

	if (!media_is_noref(devnode))
		return 0;

	if (devnode->fops->release)
		devnode->fops->release(file);

	file->private_data = NULL;
	rt_free(file);

	pr_debug("%s: Media Release\n", __func__);
	return 0;
}

static const struct dfs_file_ops media_devnode_fops = {
	.read = media_read,
	.write = media_write,
	.open = media_open,
	.ioctl = media_ioctl,
	.close = media_release,
	.poll = media_poll,
	.lseek = NULL,
};

int  media_devnode_register(struct media_device *mdev,
					struct media_devnode *devnode,
					struct module *owner)
{
	int minor;
	int ret;
	unsigned char devname[RT_NAME_MAX];

	/* Part 1: Find a free minor number */
//	rt_mutex_take(&media_devnode_lock, RT_WAITING_FOREVER);
	minor = find_next_zero_bit(media_devnode_nums, MEDIA_NUM_DEVICES, 0);
	if (minor == MEDIA_NUM_DEVICES) {
//		rt_mutex_release(&media_devnode_lock);
		pr_err("could not get a free minor\n");
		rt_free(devnode);
		return -ENFILE;
	}

	set_bit(minor, media_devnode_nums);
//	rt_mutex_release(&media_devnode_lock);

	devnode->minor = minor;
	devnode->media_dev = mdev;

	/* Part 1: Initialize dev now to use dev.kobj for cdev.kobj.parent */
	//devnode->dev.bus = &media_bus_type;
	//devnode->dev.devt = MKDEV(MAJOR(media_dev_t), devnode->minor);
	//devnode->dev.release = media_devnode_release;
	//if (devnode->parent)
	//	devnode->dev.parent = devnode->parent;
	//dev_set_name(&devnode->dev, "media%d", devnode->minor);
	//device_initialize(&devnode->dev);
	snprintf(devname, RT_NAME_MAX, "media%d", devnode->minor);

	/* Part 2: Initialize and register the character device */
	//cdev_init(&devnode->cdev, &media_devnode_fops);
	//devnode->cdev.owner = owner;
	//devnode->cdev.kobj.parent = &devnode->dev.kobj;

	//ret = cdev_add(&devnode->cdev, MKDEV(MAJOR(media_dev_t), devnode->minor), 1);
	//if (ret < 0) {
	//	pr_err("%s: cdev_add failed\n", __func__);
	//	goto cdev_add_error;
	//}

	/* Part 3: Add the media device */
	ret = rt_device_register(&devnode->dev, devname,
				RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
	if (ret < 0) {
		pr_err("%s: device_add failed\n", __func__);
		goto device_add_error;
	}
	devnode->dev.fops = &media_devnode_fops;

	/* Part 4: Activate this minor. The char device can now be used. */
	set_bit(MEDIA_FLAG_REGISTERED, &devnode->flags);

	return 0;

device_add_error:
	//cdev_del(&devnode->cdev);
cdev_add_error:
//	rt_mutex_take(&media_devnode_lock, RT_WAITING_FOREVER);
	clear_bit(devnode->minor, media_devnode_nums);
	devnode->media_dev = NULL;
//	rt_mutex_release(&media_devnode_lock);

	//put_device(&devnode->dev);
	return ret;
}

void media_devnode_unregister_prepare(struct media_devnode *devnode)
{
	/* Check if devnode was ever registered at all */
	if (!media_devnode_is_registered(devnode))
		return;

//	rt_mutex_take(&media_devnode_lock, RT_WAITING_FOREVER);
	clear_bit(MEDIA_FLAG_REGISTERED, &devnode->flags);
//	rt_mutex_release(&media_devnode_lock);
}

void media_devnode_unregister(struct media_devnode *devnode)
{
//	rt_mutex_take(&media_devnode_lock, RT_WAITING_FOREVER);
	/* Delete the cdev on this minor as well */
	//cdev_del(&devnode->cdev);
//	rt_mutex_release(&media_devnode_lock);
	//device_del(&devnode->dev);
	devnode->media_dev = NULL;
	//put_device(&devnode->dev);
}

/*
 *	Initialise media for linux
 */
static int media_devnode_init(void)
{
	int ret;

#if 0
	pr_info("Linux media interface: v0.10\n");
	ret = alloc_chrdev_region(&media_dev_t, 0, MEDIA_NUM_DEVICES,
				  MEDIA_NAME);
	if (ret < 0) {
		pr_warn("unable to allocate major\n");
		return ret;
	}

	ret = bus_register(&media_bus_type);
	if (ret < 0) {
		unregister_chrdev_region(media_dev_t, MEDIA_NUM_DEVICES);
		pr_warn("bus_register failed\n");
		return -EIO;
	}
#endif

	return 0;
}

static void media_devnode_exit(void)
{
#if 0
	bus_unregister(&media_bus_type);
	unregister_chrdev_region(media_dev_t, MEDIA_NUM_DEVICES);
#endif
}

//subsys_initcall(media_devnode_init);
//module_exit(media_devnode_exit)

