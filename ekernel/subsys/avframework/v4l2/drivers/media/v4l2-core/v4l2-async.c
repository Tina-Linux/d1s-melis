/*
 * V4L2 asynchronous subdevice registration API
 *
 * Copyright (C) 2012-2013, Guennadi Liakhovetski <g.liakhovetski@gmx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <rt_device.h>
#include <linux/err.h>
#include <common/list.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
//#include "v4l2-types.h"

#include "v4l2-async.h"
#include "v4l2-device.h"
#include "v4l2-subdev.h"

static bool match_i2c(struct v4l2_subdev *sd, struct v4l2_async_subdev *asd)
{
#if CONFIG_I2C
	struct i2c_client *client = i2c_verify_client(sd->dev);
	return client &&
		asd->match.i2c.adapter_id == client->adapter->nr &&
		asd->match.i2c.address == client->addr;
#else
	return false;
#endif
}

static bool match_devname(struct v4l2_subdev *sd,
			  struct v4l2_async_subdev *asd)
{
	return !strcmp(asd->match.device_name.name, dev_name(sd->dev));
}

static bool match_of(struct v4l2_subdev *sd, struct v4l2_async_subdev *asd)
{
	return sd->of_node == asd->match.of.node;
}

static bool match_custom(struct v4l2_subdev *sd, struct v4l2_async_subdev *asd)
{
	if (!asd->match.custom.match)
		/* Match always */
		return true;

	return asd->match.custom.match(sd->dev, asd);
}

static LIST_HEAD(subdev_list);
static LIST_HEAD(notifier_list);
static DEFINE_MUTEX(list_lock);

static struct v4l2_async_subdev *v4l2_async_belongs(struct v4l2_async_notifier *notifier,
						    struct v4l2_subdev *sd)
{
	bool (*match)(struct v4l2_subdev *, struct v4l2_async_subdev *);
	struct v4l2_async_subdev *asd;

	list_for_each_entry(asd, &notifier->waiting, list) {
		/* bus_type has been verified valid before */
		switch (asd->match_type) {
		case V4L2_ASYNC_MATCH_CUSTOM:
			match = match_custom;
			break;
		case V4L2_ASYNC_MATCH_DEVNAME:
			match = match_devname;
			break;
		case V4L2_ASYNC_MATCH_I2C:
			match = match_i2c;
			break;
		case V4L2_ASYNC_MATCH_OF:
			match = match_of;
			break;
		default:
			/* Cannot happen, unless someone breaks us */
			WARN_ON(true);
			return NULL;
		}

		/* match cannot be NULL here */
		if (match(sd, asd))
			return asd;
	}

	return NULL;
}

static int v4l2_async_test_notify(struct v4l2_async_notifier *notifier,
				  struct v4l2_subdev *sd,
				  struct v4l2_async_subdev *asd)
{
	int ret;

	/* Remove from the waiting list */
	list_del(&asd->list);
	sd->asd = asd;
	sd->notifier = notifier;

	if (notifier->bound) {
		ret = notifier->bound(notifier, sd, asd);
		if (ret < 0)
			return ret;
	}
	/* Move from the global subdevice list to notifier's done */
	list_move(&sd->async_list, &notifier->done);

	ret = v4l2_device_register_subdev(notifier->v4l2_dev, sd);
	if (ret < 0) {
		if (notifier->unbind)
			notifier->unbind(notifier, sd, asd);
		return ret;
	}

	if (list_empty(&notifier->waiting) && notifier->complete)
		return notifier->complete(notifier);

	return 0;
}

static void v4l2_async_cleanup(struct v4l2_subdev *sd)
{
	v4l2_device_unregister_subdev(sd);
	/* Subdevice driver will reprobe and put the subdev back onto the list */
	list_del_init(&sd->async_list);
	sd->asd = NULL;
	sd->dev = NULL;
}

int v4l2_async_notifier_register(struct v4l2_device *v4l2_dev,
				 struct v4l2_async_notifier *notifier)
{
	struct v4l2_subdev *sd, *tmp;
	struct v4l2_async_subdev *asd;
	int i;

	if (!notifier->num_subdevs || notifier->num_subdevs > V4L2_MAX_SUBDEVS)
		return -EINVAL;

	notifier->v4l2_dev = v4l2_dev;
	INIT_LIST_HEAD(&notifier->waiting);
	INIT_LIST_HEAD(&notifier->done);

	for (i = 0; i < notifier->num_subdevs; i++) {
		asd = notifier->subdevs[i];

		switch (asd->match_type) {
		case V4L2_ASYNC_MATCH_CUSTOM:
		case V4L2_ASYNC_MATCH_DEVNAME:
		case V4L2_ASYNC_MATCH_I2C:
		case V4L2_ASYNC_MATCH_OF:
			break;
		default:
			dev_err(notifier->v4l2_dev ? notifier->v4l2_dev->dev : NULL,
				"Invalid match type %u on %p\n",
				asd->match_type, asd);
			return -EINVAL;
		}
		list_add_tail(&asd->list, &notifier->waiting);
	}

//	rt_mutex_take(&list_lock, RT_WAITING_FOREVER);

	/* Keep also completed notifiers on the list */
	list_add(&notifier->list, &notifier_list);

	list_for_each_entry_safe(sd, tmp, &subdev_list, async_list) {
		int ret;

		asd = v4l2_async_belongs(notifier, sd);
		if (!asd)
			continue;

		ret = v4l2_async_test_notify(notifier, sd, asd);
		if (ret < 0) {
//			rt_mutex_release(&list_lock);
			return ret;
		}
	}

//	rt_mutex_release(&list_lock);

	return 0;
}

void v4l2_async_notifier_unregister(struct v4l2_async_notifier *notifier)
{
	struct v4l2_subdev *sd, *tmp;
	unsigned int notif_n_subdev = notifier->num_subdevs;
	unsigned int n_subdev = min(notif_n_subdev, V4L2_MAX_SUBDEVS);
	struct device **dev;
	int i = 0;

	if (!notifier->v4l2_dev)
		return;

	dev = rt_malloc(n_subdev * sizeof(*dev));
	if (!dev) {
		dev_err(notifier->v4l2_dev->dev,
			"Failed to allocate device cache!\n");
	}

	memset(dev, 0x00, sizeof(n_subdev * sizeof(*dev)));
//	rt_mutex_take(&list_lock, RT_WAITING_FOREVER);

	list_del(&notifier->list);

	list_for_each_entry_safe(sd, tmp, &notifier->done, async_list) {
		struct device *d;

//		d = get_device(sd->dev);

		v4l2_async_cleanup(sd);

		/* If we handled USB devices, we'd have to lock the parent too */
		device_release_driver(d);

		if (notifier->unbind)
			notifier->unbind(notifier, sd, sd->asd);

		/*
		 * Store device at the device cache, in order to call
		 * //put_device() on the final step
		 */
		if (dev)
			dev[i++] = d;
		else
			//put_device(d);
	}

//	rt_mutex_release(&list_lock);

	/*
	 * Call device_attach() to reprobe devices
	 *
	 * NOTE: If dev allocation fails, i is 0, and the whole loop won't be
	 * executed.
	 */
	while (i--) {
		struct device *d = dev[i];

		if (d && device_attach(d) < 0) {
			const char *name = "(none)";
			int lock = device_trylock(d);

			if (lock && d->driver)
				name = d->driver->name;
			dev_err(d, "Failed to re-probe to %s\n", name);
			if (lock)
				device_unlock(d);
		}
		//put_device(d);
	}
	rt_free(dev);

	notifier->v4l2_dev = NULL;

	/*
	 * Don't care about the waiting list, it is initialised and populated
	 * upon notifier registration.
	 */
}

int v4l2_async_register_subdev(struct v4l2_subdev *sd)
{
	struct v4l2_async_notifier *notifier;

	/*
	 * No reference taken. The reference is held by the device
	 * (struct v4l2_subdev.dev), and async sub-device does not
	 * exist independently of the device at any point of time.
	 */
	if (!sd->of_node && sd->dev)
		sd->of_node = sd->dev->of_node;

//	rt_mutex_take(&list_lock, RT_WAITING_FOREVER);

	INIT_LIST_HEAD(&sd->async_list);

	list_for_each_entry(notifier, &notifier_list, list) {
		struct v4l2_async_subdev *asd = v4l2_async_belongs(notifier, sd);
		if (asd) {
			int ret = v4l2_async_test_notify(notifier, sd, asd);
//			rt_mutex_release(&list_lock);
			return ret;
		}
	}

	/* None matched, wait for hot-plugging */
	list_add(&sd->async_list, &subdev_list);

//	rt_mutex_release(&list_lock);

	return 0;
}

void v4l2_async_unregister_subdev(struct v4l2_subdev *sd)
{
	struct v4l2_async_notifier *notifier = sd->notifier;

	if (!sd->asd) {
		if (!list_empty(&sd->async_list))
			v4l2_async_cleanup(sd);
		return;
	}

//	rt_mutex_take(&list_lock, RT_WAITING_FOREVER);

	list_add(&sd->asd->list, &notifier->waiting);

	v4l2_async_cleanup(sd);

	if (notifier->unbind)
		notifier->unbind(notifier, sd, sd->asd);

//	rt_mutex_release(&list_lock);
}
