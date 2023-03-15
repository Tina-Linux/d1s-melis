/*
    V4L2 device support.

    Copyright (C) 2008  Hans Verkuil <hverkuil@xs4all.nl>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <melis/typedef.h>
#include <linux/media.h>
#include "linux/videodev2.h"
#include "v4l2-device.h"
#include "v4l2-ctrls.h"
#include <linux/atomic.h>

int v4l2_device_register(struct rt_device *dev, struct v4l2_device *v4l2_dev)
{
	if (v4l2_dev == NULL)
		return -EINVAL;

	INIT_LIST_HEAD(&v4l2_dev->subdevs);
	//rt_hw_spin_lock_init(&v4l2_dev->lock);
	v4l2_prio_init(&v4l2_dev->prio);
	//kref_init(&v4l2_dev->ref);
	//get_device(dev);
#if 1
	v4l2_dev->dev = dev;
	if (dev == NULL) {
		/* If dev == NULL, then name must be filled in by the caller */
		if (WARN_ON(!v4l2_dev->name[0]))
			return -EINVAL;
		return 0;
	}

#if 0
	/* Set name to driver name + device name if it is empty. */
	if (!v4l2_dev->name[0])
		snprintf(v4l2_dev->name, sizeof(v4l2_dev->name), "%s %s",
			dev->driver->name, dev_name(dev));
#endif
	if (!dev_get_drvdata(dev))
		dev_set_drvdata(dev, v4l2_dev);
#endif
	return 0;
}

#if 0
static void v4l2_device_release(struct kref *ref)
{
	struct v4l2_device *v4l2_dev =
		container_of(ref, struct v4l2_device, ref);

	if (v4l2_dev->release)
		v4l2_dev->release(v4l2_dev);
}
#endif

int v4l2_device_put(struct v4l2_device *v4l2_dev)
{
	//return kref_put(&v4l2_dev->ref, v4l2_device_release);
	return 0;
}

int v4l2_device_set_name(struct v4l2_device *v4l2_dev, const char *basename,
						atomic_t *instance)
{
	int num = atomic_inc_return(instance) - 1;
	int len = strlen(basename);

	if (basename[len - 1] >= '0' && basename[len - 1] <= '9')
		snprintf(v4l2_dev->name, sizeof(v4l2_dev->name),
				"%s-%d", basename, num);
	else
		snprintf(v4l2_dev->name, sizeof(v4l2_dev->name),
				"%s%d", basename, num);
	return num;
}

void v4l2_device_disconnect(struct v4l2_device *v4l2_dev)
{
#if 0
	if (v4l2_dev->dev == NULL)
		return;

	if (dev_get_drvdata(v4l2_dev->dev) == v4l2_dev)
		dev_set_drvdata(v4l2_dev->dev, NULL);
	//put_device(v4l2_dev->dev);
	v4l2_dev->dev = NULL;
#endif
}

void v4l2_device_unregister(struct v4l2_device *v4l2_dev)
{
	struct v4l2_subdev *sd, *next;

	/* Just return if v4l2_dev is NULL or if it was already
	 * unregistered before. */
	if (v4l2_dev == NULL || !v4l2_dev->name[0])
		return;

	v4l2_device_disconnect(v4l2_dev);

	/* Unregister subdevs */
	list_for_each_entry_safe(sd, next, &v4l2_dev->subdevs, list) {
		v4l2_device_unregister_subdev(sd);
	}
	/* Mark as unregistered, thus preventing duplicate unregistrations */
	v4l2_dev->name[0] = '\0';
}

int v4l2_device_register_subdev(struct v4l2_device *v4l2_dev,
				struct v4l2_subdev *sd)
{
#if defined(CONFIG_MEDIA_CONTROLLER)
	struct media_entity *entity = &sd->entity;
#endif
	int err;

	/* Check for valid input */
	if (!v4l2_dev || !sd || sd->v4l2_dev || !sd->name[0])
		return -EINVAL;

	/*
	 * The reason to acquire the module here is to avoid unloading
	 * a module of sub-device which is registered to a media
	 * device. To make it possible to unload modules for media
	 * devices that also register sub-devices, do not
	 * try_module_get() such sub-device owners.
	 */
#if 0
	sd->owner_v4l2_dev = v4l2_dev->dev && v4l2_dev->dev->driver &&
		sd->owner == v4l2_dev->dev->driver->owner;

	if (!sd->owner_v4l2_dev && !try_module_get(sd->owner))
		return -ENODEV;
#endif

	sd->v4l2_dev = v4l2_dev;
	/* This just returns 0 if either of the two args is NULL */
	err = v4l2_ctrl_add_handler(v4l2_dev->ctrl_handler, sd->ctrl_handler, NULL);
	if (err)
		goto error_module;

#if defined(CONFIG_MEDIA_CONTROLLER)
	/* Register the entity. */
	if (v4l2_dev->mdev) {
		err = media_device_register_entity(v4l2_dev->mdev, entity);
		if (err < 0)
			goto error_module;
	}
#endif

	if (sd->internal_ops && sd->internal_ops->registered) {
		err = sd->internal_ops->registered(sd);
		if (err)
			goto error_unregister;
	}

	//rt_hw_spin_lock(&v4l2_dev->lock);
	list_add_tail(&sd->list, &v4l2_dev->subdevs);
	//rt_hw_spin_unlock(&v4l2_dev->lock);

	return 0;

error_unregister:
#if defined(CONFIG_MEDIA_CONTROLLER)
	media_device_unregister_entity(entity);
#endif
error_module:
#if 0
	if (!sd->owner_v4l2_dev)
		module_put(sd->owner);
#endif
	sd->v4l2_dev = NULL;
	return err;
}

static void v4l2_device_release_subdev_node(struct video_device *vdev)
{
	struct v4l2_subdev *sd = video_get_drvdata(vdev);
	sd->devnode = NULL;
	rt_free(vdev);
}

int v4l2_device_register_subdev_nodes(struct v4l2_device *v4l2_dev)
{
	struct video_device *vdev;
	struct v4l2_subdev *sd;
	int err;

	/* Register a device node for every subdev marked with the
	 * V4L2_SUBDEV_FL_HAS_DEVNODE flag.
	 */
	list_for_each_entry(sd, &v4l2_dev->subdevs, list) {
		if (!(sd->flags & V4L2_SUBDEV_FL_HAS_DEVNODE))
		{
			continue;
		}

		vdev  = rt_malloc(sizeof(*vdev));
		if (!vdev) {
			err = -ENOMEM;
			goto clean_up;
		}

		memset(vdev, 0x00, sizeof(*vdev));
		video_set_drvdata(vdev, sd);
		strlcpy(vdev->name, sd->name, sizeof(vdev->name));
		vdev->v4l2_dev = v4l2_dev;
		vdev->fops = &v4l2_subdev_fops;
		vdev->release = v4l2_device_release_subdev_node;
		vdev->ctrl_handler = sd->ctrl_handler;
		vdev->ioctl_ops = NULL;
		vdev->queue = NULL;
		vdev->flags |= V4L2_FL_USES_V4L2_FH;
		err = __video_register_device(vdev, VFL_TYPE_SUBDEV, -1, 1);
		if (err < 0) {
			rt_free(vdev);
			goto clean_up;
		}
#if defined(CONFIG_MEDIA_CONTROLLER)
		sd->entity.info.dev.major = VIDEO_MAJOR + vdev->num;
		sd->entity.info.dev.minor = vdev->minor;

		/* Interface is created by __video_register_device() */
		if (vdev->v4l2_dev->mdev) {
			struct media_link *link;

			link = media_create_intf_link(&sd->entity,
						      &vdev->intf_devnode->intf,
						      MEDIA_LNK_FL_ENABLED);
			if (!link) {
				err = -ENOMEM;
				goto clean_up;
			}
		}
#endif
		sd->devnode = vdev;
	}
	return 0;

clean_up:
	list_for_each_entry(sd, &v4l2_dev->subdevs, list) {
		if (!sd->devnode)
			break;
		video_unregister_device(sd->devnode);
	}

	return err;
}

void v4l2_device_unregister_subdev(struct v4l2_subdev *sd)
{
	struct v4l2_device *v4l2_dev;

	/* return if it isn't registered */
	if (sd == NULL || sd->v4l2_dev == NULL)
		return;

	v4l2_dev = sd->v4l2_dev;

	//rt_hw_spin_lock(&v4l2_dev->lock);
	list_del(&sd->list);
	//rt_hw_spin_unlock(&v4l2_dev->lock);

	if (sd->internal_ops && sd->internal_ops->unregistered)
		sd->internal_ops->unregistered(sd);
	sd->v4l2_dev = NULL;

#if defined(CONFIG_MEDIA_CONTROLLER)
	if (v4l2_dev->mdev) {
		/*
		 * No need to explicitly remove links, as both pads and
		 * links are removed by the function below, in the right order
		 */
		media_device_unregister_entity(&sd->entity);
	}
#endif
	video_unregister_device(sd->devnode);
#if 0
	if (!sd->owner_v4l2_dev)
		module_put(sd->owner);
#endif
}
