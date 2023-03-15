/*
 * Media device
 *
 * Copyright (C) 2010 Nokia Corporation
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
 */

/* We need to access legacy defines from linux/media.h */
#define __NEED_MEDIA_LEGACY_API

//#include <linux/idr.h>
//#include "v4l2-types.h"
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/printk.h>

#include "media-device.h"
#include "media-devnode.h"
#include "media-entity.h"
#include <linux/media.h>

#ifdef CONFIG_MEDIA_CONTROLLER
#define ENTITY_ENUMS   256
static DECLARE_BITMAP(entity_nums_bmp, ENTITY_ENUMS);

/* -----------------------------------------------------------------------------
 * Userspace API
 */

static inline void  *media_get_uptr(__u64 arg)
{
	return (void  *)(uintptr_t)arg;
}

static int media_device_open(struct file *filp)
{
	return 0;
}

static int media_device_close(struct file *filp)
{
	return 0;
}

static long media_device_get_info(struct media_device *dev, void *arg)
{
	struct media_device_info *info = (struct media_device_info *)arg;

	memset(info, 0, sizeof(*info));

#if 0
	if (dev->driver_name[0])
		strlcpy(info->driver, dev->driver_name, sizeof(info->driver));
	else
		strlcpy(info->driver, dev->dev->driver->name,
			sizeof(info->driver));

	strlcpy(info->model, dev->model, sizeof(info->model));
	strlcpy(info->serial, dev->serial, sizeof(info->serial));
	strlcpy(info->bus_info, dev->bus_info, sizeof(info->bus_info));

	info->media_version = MEDIA_API_VERSION;
	info->hw_revision = dev->hw_revision;
	info->driver_version = dev->driver_version;
#endif

	return 0;
}

static struct media_entity *find_entity(struct media_device *mdev, u32 id)
{
	struct media_entity *entity;
	int next = id & MEDIA_ENT_ID_FLAG_NEXT;

	id &= ~MEDIA_ENT_ID_FLAG_NEXT;

	media_device_for_each_entity(entity, mdev) {
		if (((media_entity_id(entity) == id) && !next) ||
		    ((media_entity_id(entity) > id) && next)) {
			return entity;
		}
	}

	return NULL;
}

static long media_device_enum_entities(struct media_device *mdev, void *arg)
{
	struct media_entity_desc *entd = (struct media_entity_desc *)arg;
	struct media_entity *ent;

	ent = find_entity(mdev, entd->id);
	if (ent == NULL)
		return -EINVAL;

	memset(entd, 0, sizeof(*entd));

	entd->id = media_entity_id(ent);
	if (ent->name)
		strlcpy(entd->name, ent->name, sizeof(entd->name));
	entd->type = ent->function;
	entd->revision = 0;		/* Unused */
	entd->flags = ent->flags;
	entd->group_id = 0;		/* Unused */
	entd->pads = ent->num_pads;
	entd->links = ent->num_links - ent->num_backlinks;

	/*
	 * Workaround for a bug at media-ctl <= v1.10 that makes it to
	 * do the wrong thing if the entity function doesn't belong to
	 * either MEDIA_ENT_F_OLD_BASE or MEDIA_ENT_F_OLD_SUBDEV_BASE
	 * Ranges.
	 *
	 * Non-subdevices are expected to be at the MEDIA_ENT_F_OLD_BASE,
	 * or, otherwise, will be silently ignored by media-ctl when
	 * printing the graphviz diagram. So, map them into the devnode
	 * old range.
	 */
	if (ent->function < MEDIA_ENT_F_OLD_BASE ||
	    ent->function > MEDIA_ENT_F_TUNER) {
		if (is_media_entity_v4l2_subdev(ent))
			entd->type = MEDIA_ENT_F_V4L2_SUBDEV_UNKNOWN;
		else if (ent->function != MEDIA_ENT_F_IO_V4L)
			entd->type = MEDIA_ENT_T_DEVNODE_UNKNOWN;
	}

	memcpy(&entd->raw, &ent->info, sizeof(ent->info));

	return 0;
}

static void media_device_kpad_to_upad(const struct media_pad *kpad,
				      struct media_pad_desc *upad)
{
	upad->entity = media_entity_id(kpad->entity);
	upad->index = kpad->index;
	upad->flags = kpad->flags;
}

static long media_device_enum_links(struct media_device *mdev, void *arg)
{
	struct media_links_enum *links = (struct media_links_enum *)arg;
	struct media_entity *entity;

	entity = find_entity(mdev, links->entity);
	if (entity == NULL)
		return -EINVAL;

	if (links->pads) {
		unsigned int p;

		for (p = 0; p < entity->num_pads; p++) {
			memset(&links->pads[p], 0, sizeof(struct media_pad_desc));
			media_device_kpad_to_upad(&entity->pads[p], &links->pads[p]);
		}
	}

	if (links->links) {
		struct media_link *link;
		struct media_link_desc  *ulink_desc = links->links;

		list_for_each_entry(link, &entity->links, list) {
			/* Ignore backlinks. */
			if (link->source->entity != entity)
				continue;

			memset(ulink_desc, 0, sizeof(struct media_link_desc));
			media_device_kpad_to_upad(link->source,
						  &ulink_desc->source);
			media_device_kpad_to_upad(link->sink,
						  &ulink_desc->sink);
			ulink_desc->flags = link->flags;

			ulink_desc++;
		}
	}

	return 0;
}

static long media_device_setup_link(struct media_device *mdev, void *arg)
{
	struct media_link_desc *linkd = (struct media_link_desc *)arg;
	struct media_link *link = NULL;
	struct media_entity *source;
	struct media_entity *sink;

	/* Find the source and sink entities and link.
	 */
	source = find_entity(mdev, linkd->source.entity);
	sink = find_entity(mdev, linkd->sink.entity);

	if (source == NULL || sink == NULL)
		return -EINVAL;

	if (linkd->source.index >= source->num_pads ||
	    linkd->sink.index >= sink->num_pads)
		return -EINVAL;

	link = media_entity_find_link(&source->pads[linkd->source.index],
				      &sink->pads[linkd->sink.index]);
	if (link == NULL)
		return -EINVAL;

	/* Setup the link on both entities. */
	return __media_entity_setup_link(link, linkd->flags);
}

static long media_device_get_topology(struct media_device *mdev, void *arg)
{
	struct media_v2_topology *topo = (struct media_v2_topology *)arg;
	struct media_entity *entity;
	struct media_interface *intf;
	struct media_pad *pad;
	struct media_link *link;
	struct media_v2_entity kentity,  *uentity;
	struct media_v2_interface  *uintf;
	struct media_v2_pad   *upad;
	struct media_v2_link  *ulink;
	unsigned int i;
	int ret = 0;

	topo->topology_version = mdev->topology_version;

	/* Get entities and number of entities */
	i = 0;
	uentity = media_get_uptr(topo->ptr_entities);
	media_device_for_each_entity(entity, mdev) {
		i++;
		if (ret || !uentity)
			continue;

		if (i > topo->num_entities) {
			ret = -ENOSPC;
			continue;
		}

		/* Copy fields to userspace struct if not error */
		uentity->id = media_entity_id(entity);
		uentity->function = entity->function;
		strncpy(uentity->name, entity->name,
			sizeof(uentity->name));

		uentity++;
	}
	topo->num_entities = i;

	/* Get interfaces and number of interfaces */
	i = 0;
	uintf = media_get_uptr(topo->ptr_interfaces);
	media_device_for_each_intf(intf, mdev) {
		i++;
		if (ret || !uintf)
			continue;

		if (i > topo->num_interfaces) {
			ret = -ENOSPC;
			continue;
		}

		memset(uintf, 0, sizeof(struct media_v2_interface));

		/* Copy intf fields to userspace struct */
		uintf->id = intf->graph_obj.id;
		uintf->intf_type = intf->type;
		uintf->flags = intf->flags;

		if (media_type(&intf->graph_obj) == MEDIA_GRAPH_INTF_DEVNODE) {
			struct media_intf_devnode *devnode;

			devnode = intf_to_devnode(intf);

			uintf->devnode.major = devnode->major;
			uintf->devnode.minor = devnode->minor;
		}

		uintf++;
	}
	topo->num_interfaces = i;

	/* Get pads and number of pads */
	i = 0;
	upad = media_get_uptr(topo->ptr_pads);
	media_device_for_each_pad(pad, mdev) {
		i++;
		if (ret || !upad)
			continue;

		if (i > topo->num_pads) {
			ret = -ENOSPC;
			continue;
		}

		memset(upad, 0, sizeof(struct media_v2_pad));

		/* Copy pad fields to userspace struct */
		upad->id = pad->graph_obj.id;
		upad->entity_id = pad->entity->graph_obj.id;
		upad->flags = pad->flags;

		upad++;
	}
	topo->num_pads = i;

	/* Get links and number of links */
	i = 0;
	ulink = media_get_uptr(topo->ptr_links);
	media_device_for_each_link(link, mdev) {
		if (link->is_backlink)
			continue;

		i++;

		if (ret || !ulink)
			continue;

		if (i > topo->num_links) {
			ret = -ENOSPC;
			continue;
		}

		memset(ulink, 0, sizeof(struct media_v2_link));

		/* Copy link fields to userspace struct */
		ulink->id = link->graph_obj.id;
		ulink->source_id = link->gobj0->id;
		ulink->sink_id = link->gobj1->id;
		ulink->flags = link->flags;

		ulink++;
	}
	topo->num_links = i;

	return ret;
}


/* Do acquire the graph mutex */
#define MEDIA_IOC_FL_GRAPH_MUTEX	BIT(0)

#define MEDIA_IOC_ARG(__cmd, func, fl)					\
	[_IOC_NR(MEDIA_IOC_##__cmd)] = {				\
		.cmd = MEDIA_IOC_##__cmd,				\
		.fn = (long (*)(struct media_device *, void *))func,	\
		.flags = fl,						\
	}

#define MEDIA_IOC(__cmd, func, fl)					\
	MEDIA_IOC_ARG(__cmd, func, fl)

/* the table is indexed by _IOC_NR(cmd) */
struct media_ioctl_info {
	unsigned int cmd;
	unsigned short flags;
	long (*fn)(struct media_device *dev, void *arg);
};

static const struct media_ioctl_info ioctl_info[] = {
	MEDIA_IOC(DEVICE_INFO, media_device_get_info, MEDIA_IOC_FL_GRAPH_MUTEX),
	MEDIA_IOC(ENUM_ENTITIES, media_device_enum_entities, MEDIA_IOC_FL_GRAPH_MUTEX),
	MEDIA_IOC(ENUM_LINKS, media_device_enum_links, MEDIA_IOC_FL_GRAPH_MUTEX),
	MEDIA_IOC(SETUP_LINK, media_device_setup_link, MEDIA_IOC_FL_GRAPH_MUTEX),
	MEDIA_IOC(G_TOPOLOGY, media_device_get_topology, MEDIA_IOC_FL_GRAPH_MUTEX),
};

static long media_device_ioctl(struct file *filp, unsigned int cmd,
			       unsigned long __arg)
{
	struct media_devnode *devnode = media_devnode_data(filp);
	struct media_device *dev = devnode->media_dev;
	const struct media_ioctl_info *info;
	char *karg = (char *)__arg;
	long ret;

	if (_IOC_NR(cmd) >= ARRAY_SIZE(ioctl_info)
	    || ioctl_info[_IOC_NR(cmd)].cmd != cmd)
		return -ENOIOCTLCMD;

	info = &ioctl_info[_IOC_NR(cmd)];


//	if (info->flags & MEDIA_IOC_FL_GRAPH_MUTEX)
//		rt_mutex_take(&dev->graph_mutex, RT_WAITING_FOREVER);

	ret = info->fn(dev, karg);

//	if (info->flags & MEDIA_IOC_FL_GRAPH_MUTEX)
//		rt_mutex_release(&dev->graph_mutex);

	return ret;
}


static const struct media_file_operations media_device_fops = {
	.open = media_device_open,
	.ioctl = media_device_ioctl,
	.release = media_device_close,
};


/* -----------------------------------------------------------------------------
 * Registration/unregistration
 */

static void media_device_release(struct media_devnode *mdev)
{
	dev_dbg(mdev->parent, "Media device released\n");
}

/**
 * media_device_register_entity - Register an entity with a media device
 * @mdev:	The media device
 * @entity:	The entity
 */
int  media_device_register_entity(struct media_device *mdev,
					      struct media_entity *entity)
{
	struct media_entity_notify *notify, *next;
	unsigned int i;
	int ret;

	if (entity->function == MEDIA_ENT_F_V4L2_SUBDEV_UNKNOWN ||
	    entity->function == MEDIA_ENT_F_UNKNOWN)
		dev_warn(mdev->dev,
			 "Entity type for entity %s was not initialized!\n",
			 entity->name);

	/* Warn if we apparently re-register an entity */
	WARN_ON(entity->graph_obj.mdev != NULL);
	entity->graph_obj.mdev = mdev;
	INIT_LIST_HEAD(&entity->links);
	entity->num_links = 0;
	entity->num_backlinks = 0;

#if 0
	if (!ida_pre_get(&mdev->entity_internal_idx, GFP_KERNEL))
		return -ENOMEM;

	ret = ida_get_new_above(&mdev->entity_internal_idx, 1,
				&entity->internal_idx);
	if (ret < 0) {
		rt_mutex_release(&mdev->graph_mutex);
		return ret;
	}
#else

	entity->internal_idx = find_next_zero_bit(entity_nums_bmp, ENTITY_ENUMS, 0);
	if (entity->internal_idx == ENTITY_ENUMS) {
		pr_err("could not get a free idx\n");
		return -ENFILE;
	}
	set_bit(entity->internal_idx, entity_nums_bmp);
#endif

	mdev->entity_internal_idx_max =
		max(mdev->entity_internal_idx_max, entity->internal_idx);

	/* Initialize media_gobj embedded at the entity */
	media_gobj_create(mdev, MEDIA_GRAPH_ENTITY, &entity->graph_obj);

	/* Initialize objects at the pads */
	for (i = 0; i < entity->num_pads; i++)
		media_gobj_create(mdev, MEDIA_GRAPH_PAD,
			       &entity->pads[i].graph_obj);

	/* invoke entity_notify callbacks */
	list_for_each_entry_safe(notify, next, &mdev->entity_notify, list) {
		(notify)->notify(entity, notify->notify_data);
	}

	if (mdev->entity_internal_idx_max
	    >= mdev->pm_count_walk.ent_enum.idx_max) {
		struct media_entity_graph new = { .top = 0 };

		/*
		 * Initialise the new graph walk before cleaning up
		 * the old one in order not to spoil the graph walk
		 * object of the media device if graph walk init fails.
		 */
		ret = media_entity_graph_walk_init(&new, mdev);
		if (ret) {
//			rt_mutex_release(&mdev->graph_mutex);
			return ret;
		}
		media_entity_graph_walk_cleanup(&mdev->pm_count_walk);
		mdev->pm_count_walk = new;
	}
//	rt_mutex_release(&mdev->graph_mutex);

	return 0;
}

static void __media_device_unregister_entity(struct media_entity *entity)
{
	struct media_device *mdev = entity->graph_obj.mdev;
	struct media_link *link, *tmp;
	struct media_interface *intf;
	unsigned int i;

#if 0
	//ida_simple_remove(&mdev->entity_internal_idx, entity->internal_idx);
#else
	clear_bit(entity->internal_idx, entity_nums_bmp);
#endif

	/* Remove all interface links pointing to this entity */
	list_for_each_entry(intf, &mdev->interfaces, graph_obj.list) {
		list_for_each_entry_safe(link, tmp, &intf->links, list) {
			if (link->entity == entity)
				__media_remove_intf_link(link);
		}
	}

	/* Remove all data links that belong to this entity */
	__media_entity_remove_links(entity);

	/* Remove all pads that belong to this entity */
	for (i = 0; i < entity->num_pads; i++)
		media_gobj_destroy(&entity->pads[i].graph_obj);

	/* Remove the entity */
	media_gobj_destroy(&entity->graph_obj);

	/* invoke entity_notify callbacks to handle entity removal?? */

	entity->graph_obj.mdev = NULL;
}

void media_device_unregister_entity(struct media_entity *entity)
{
	struct media_device *mdev = entity->graph_obj.mdev;

	if (mdev == NULL)
		return;

//	rt_mutex_take(&mdev->graph_mutex, RT_WAITING_FOREVER);
	__media_device_unregister_entity(entity);
//	rt_mutex_release(&mdev->graph_mutex);
}

/**
 * media_device_init() - initialize a media device
 * @mdev:	The media device
 *
 * The caller is responsible for initializing the media device before
 * registration. The following fields must be set:
 *
 * - dev must point to the parent device
 * - model must be filled with the device model name
 */
void media_device_init(struct media_device *mdev)
{
	INIT_LIST_HEAD(&mdev->entities);
	INIT_LIST_HEAD(&mdev->interfaces);
	INIT_LIST_HEAD(&mdev->pads);
	INIT_LIST_HEAD(&mdev->links);
	INIT_LIST_HEAD(&mdev->entity_notify);
	rt_mutex_init(&mdev->graph_mutex, "mediamtx", RT_IPC_FLAG_FIFO);

	dev_dbg(mdev->dev, "Media device initialized\n");
}

void media_device_cleanup(struct media_device *mdev)
{
	mdev->entity_internal_idx_max = 0;
	media_entity_graph_walk_cleanup(&mdev->pm_count_walk);
	rt_mutex_detach(&mdev->graph_mutex);
}

int  __media_device_register(struct media_device *mdev,
					 struct module *owner)
{
	struct media_devnode *devnode;
	int ret;

	devnode  = rt_malloc(sizeof(*devnode));
	if (!devnode)
		return -ENOMEM;

	memset(devnode, 0x00, sizeof(*devnode));
	/* Register the device node. */
	mdev->devnode = devnode;
	devnode->fops = &media_device_fops;
	//devnode->parent = mdev->dev;
	devnode->release = media_device_release;

	/* Set version 0 to indicate user-space that the graph is static */
	mdev->topology_version = 0;

	ret = media_devnode_register(mdev, devnode, owner);
	if (ret < 0) {
		/* devnode free is handled in media_devnode_*() */
		mdev->devnode = NULL;
		return ret;
	}

#if 0
	ret = device_create_file(&devnode->dev, &dev_attr_model);
	if (ret < 0) {
		/* devnode free is handled in media_devnode_*() */
		mdev->devnode = NULL;
		media_devnode_unregister_prepare(devnode);
		media_devnode_unregister(devnode);
		return ret;
	}
#endif

	dev_dbg(mdev->dev, "Media device registered\n");

	return 0;
}

int  media_device_register_entity_notify(struct media_device *mdev,
					struct media_entity_notify *nptr)
{
//	rt_mutex_take(&mdev->graph_mutex, RT_WAITING_FOREVER);
	list_add_tail(&nptr->list, &mdev->entity_notify);
//	rt_mutex_release(&mdev->graph_mutex);
	return 0;
}

/*
 * Note: Should be called with mdev->lock held.
 */
static void __media_device_unregister_entity_notify(struct media_device *mdev,
					struct media_entity_notify *nptr)
{
	list_del(&nptr->list);
}

void media_device_unregister_entity_notify(struct media_device *mdev,
					struct media_entity_notify *nptr)
{
//	rt_mutex_take(&mdev->graph_mutex, RT_WAITING_FOREVER);
	__media_device_unregister_entity_notify(mdev, nptr);
//	rt_mutex_release(&mdev->graph_mutex);
}

void media_device_unregister(struct media_device *mdev)
{
	struct media_entity *entity;
	struct media_entity *next;
	struct media_interface *intf, *tmp_intf;
	struct media_entity_notify *notify, *nextp;

	if (mdev == NULL)
		return;

//	rt_mutex_take(&mdev->graph_mutex, RT_WAITING_FOREVER);

	/* Check if mdev was ever registered at all */
	if (!media_devnode_is_registered(mdev->devnode)) {
//		rt_mutex_release(&mdev->graph_mutex);
		return;
	}

	/* Clear the devnode register bit to avoid races with media dev open */
	media_devnode_unregister_prepare(mdev->devnode);

	/* Remove all entities from the media device */
	list_for_each_entry_safe(entity, next, &mdev->entities, graph_obj.list)
		__media_device_unregister_entity(entity);

	/* Remove all entity_notify callbacks from the media device */
	list_for_each_entry_safe(notify, nextp, &mdev->entity_notify, list)
		__media_device_unregister_entity_notify(mdev, notify);

	/* Remove all interfaces from the media device */
	list_for_each_entry_safe(intf, tmp_intf, &mdev->interfaces,
				 graph_obj.list) {
		__media_remove_intf_links(intf);
		media_gobj_destroy(&intf->graph_obj);
		rt_free(intf);
	}

//	rt_mutex_release(&mdev->graph_mutex);

	dev_dbg(mdev->dev, "Media device unregistered\n");

	//device_remove_file(&mdev->devnode->dev, &dev_attr_model);
	media_devnode_unregister(mdev->devnode);
	/* devnode free is handled in media_devnode_*() */
	mdev->devnode = NULL;
}

static void media_device_release_devres(struct rt_device *dev, void *res)
{
}

struct media_device *media_device_get_devres(struct rt_device *dev)
{
	struct media_device *mdev;

#if 0
	mdev = devres_find(dev, media_device_release_devres, NULL, NULL);
	if (mdev)
		return mdev;

	mdev = devres_alloc(media_device_release_devres,
				sizeof(struct media_device), GFP_KERNEL);
	if (!mdev)
		return NULL;
	return devres_get(dev, mdev, NULL, NULL);
#endif
	return NULL;
}

struct media_device *media_device_find_devres(struct rt_device *dev)
{
	//return devres_find(dev, media_device_release_devres, NULL, NULL);
	return NULL;
}



#endif /* CONFIG_MEDIA_CONTROLLER */
