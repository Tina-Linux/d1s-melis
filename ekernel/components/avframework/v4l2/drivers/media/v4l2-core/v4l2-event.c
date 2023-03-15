/*
 * v4l2-event.c
 *
 * V4L2 events.
 *
 * Copyright (C) 2009--2010 Nokia Corporation.
 *
 * Contact: Sakari Ailus <sakari.ailus@iki.fi>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <rthw.h>
#include <ktimer.h>
#include <aw_list.h>
#include <waitqueue.h>
#include "v4l2-dev.h"
#include "v4l2-fh.h"
#include "v4l2-event.h"

static unsigned sev_pos(const struct v4l2_subscribed_event *sev, unsigned idx)
{
	idx += sev->first;
	return idx >= sev->elems ? idx - sev->elems : idx;
}

static int __v4l2_event_dequeue(struct v4l2_fh *fh, struct v4l2_event *event)
{
	struct v4l2_kevent *kev;
	unsigned long flags;

	rt_hw_spin_lock(&fh->vdev->fh_lock);

	if (list_empty(&fh->available)) {
		rt_hw_spin_unlock(&fh->vdev->fh_lock);
		return -ENOENT;
	}

	WARN_ON(fh->navailable == 0);

	kev = list_first_entry(&fh->available, struct v4l2_kevent, list);
	list_del(&kev->list);
	fh->navailable--;

	kev->event.pending = fh->navailable;
	*event = kev->event;
	kev->sev->first = sev_pos(kev->sev, 1);
	kev->sev->in_use--;

	rt_hw_spin_unlock(&fh->vdev->fh_lock);

	return 0;
}

int v4l2_event_dequeue(struct v4l2_fh *fh, struct v4l2_event *event,
		       int nonblocking)
{
	int ret;

	if (nonblocking)
		return __v4l2_event_dequeue(fh, event);

	/* Release the vdev lock while waiting */
	if (fh->vdev->lock)
		rt_mutex_release(fh->vdev->lock);

	do {
		rt_wqueue_wait(&fh->wait, (fh->navailable != 0), RT_WAITING_FOREVER);
		ret = __v4l2_event_dequeue(fh, event);
	} while (ret == -ENOENT);

	if (fh->vdev->lock)
		rt_mutex_take(fh->vdev->lock, RT_WAITING_FOREVER);

	return ret;
}

/* Caller must hold fh->vdev->fh_lock! */
static struct v4l2_subscribed_event *v4l2_event_subscribed(
		struct v4l2_fh *fh, u32 type, u32 id)
{
	struct v4l2_subscribed_event *sev;

	//assert_rt_hw_spin_locked(&fh->vdev->fh_lock);

	list_for_each_entry(sev, &fh->subscribed, list)
		if (sev->type == type && sev->id == id)
			return sev;

	return NULL;
}

static void __v4l2_event_queue_fh(struct v4l2_fh *fh, const struct v4l2_event *ev,
		const struct timespec *ts)
{
	struct v4l2_subscribed_event *sev;
	struct v4l2_kevent *kev;
	bool copy_payload = true;

	/* Are we subscribed? */
	sev = v4l2_event_subscribed(fh, ev->type, ev->id);
	if (sev == NULL)
		return;

	/*
	 * If the event has been added to the fh->subscribed list, but its
	 * add op has not completed yet elems will be 0, treat this as
	 * not being subscribed.
	 */
	if (!sev->elems)
		return;

	/* Increase event sequence number on fh. */
	fh->sequence++;

	/* Do we have any free events? */
	if (sev->in_use == sev->elems) {
		/* no, remove the oldest one */
		kev = sev->events + sev_pos(sev, 0);
		list_del(&kev->list);
		sev->in_use--;
		sev->first = sev_pos(sev, 1);
		fh->navailable--;
		if (sev->elems == 1) {
			if (sev->ops && sev->ops->replace) {
				sev->ops->replace(&kev->event, ev);
				copy_payload = false;
			}
		} else if (sev->ops && sev->ops->merge) {
			struct v4l2_kevent *second_oldest =
				sev->events + sev_pos(sev, 0);
			sev->ops->merge(&kev->event, &second_oldest->event);
		}
	}

	/* Take one and fill it. */
	kev = sev->events + sev_pos(sev, sev->in_use);
	kev->event.type = ev->type;
	if (copy_payload)
		kev->event.u = ev->u;
	kev->event.id = ev->id;
	kev->event.timestamp = *ts;
	kev->event.sequence = fh->sequence;
	sev->in_use++;
	list_add_tail(&kev->list, &fh->available);
	fh->navailable++;

	rt_wqueue_wakeup(&fh->wait, NULL);
}

void v4l2_event_queue(struct video_device *vdev, const struct v4l2_event *ev)
{
	struct v4l2_fh *fh;
	unsigned long flags;
	struct timespec timestamp;

	if (vdev == NULL)
		return;

	ktime_get_ts(&timestamp);

	rt_hw_spin_lock(&vdev->fh_lock);

	list_for_each_entry(fh, &vdev->fh_list, list)
		__v4l2_event_queue_fh(fh, ev, &timestamp);

	rt_hw_spin_unlock(&vdev->fh_lock);
}

void v4l2_event_queue_fh(struct v4l2_fh *fh, const struct v4l2_event *ev)
{
	unsigned long flags;
	struct timespec timestamp;

	ktime_get_ts(&timestamp);

	rt_hw_spin_lock(&fh->vdev->fh_lock);
	__v4l2_event_queue_fh(fh, ev, &timestamp);
	rt_hw_spin_unlock(&fh->vdev->fh_lock);
}

int v4l2_event_pending(struct v4l2_fh *fh)
{
	return fh->navailable;
}

int v4l2_event_subscribe(struct v4l2_fh *fh,
			 const struct v4l2_event_subscription *sub, unsigned elems,
			 const struct v4l2_subscribed_event_ops *ops)
{
	struct v4l2_subscribed_event *sev, *found_ev;
	unsigned long flags;
	unsigned i;

	if (sub->type == V4L2_EVENT_ALL)
		return -EINVAL;

	if (elems < 1)
		elems = 1;

	sev  = rt_malloc(sizeof(*sev) + sizeof(struct v4l2_kevent) * elems);
	if (!sev)
		return -ENOMEM;

	memset(sev, 0x00, sizeof(*sev) + sizeof(struct v4l2_kevent) * elems);

	for (i = 0; i < elems; i++)
		sev->events[i].sev = sev;
	sev->type = sub->type;
	sev->id = sub->id;
	sev->flags = sub->flags;
	sev->fh = fh;
	sev->ops = ops;

	rt_hw_spin_lock(&fh->vdev->fh_lock);
	found_ev = v4l2_event_subscribed(fh, sub->type, sub->id);
	if (!found_ev)
		list_add(&sev->list, &fh->subscribed);
	rt_hw_spin_unlock(&fh->vdev->fh_lock);

	if (found_ev) {
		rt_free(sev);
		return 0; /* Already listening */
	}

	if (sev->ops && sev->ops->add) {
		int ret = sev->ops->add(sev, elems);
		if (ret) {
			sev->ops = NULL;
			v4l2_event_unsubscribe(fh, sub);
			return ret;
		}
	}

	/* Mark as ready for use */
	sev->elems = elems;

	return 0;
}

void v4l2_event_unsubscribe_all(struct v4l2_fh *fh)
{
	struct v4l2_event_subscription sub;
	struct v4l2_subscribed_event *sev;
	unsigned long flags;

	do {
		sev = NULL;

		rt_hw_spin_lock(&fh->vdev->fh_lock);
		if (!list_empty(&fh->subscribed)) {
			sev = list_first_entry(&fh->subscribed,
					struct v4l2_subscribed_event, list);
			sub.type = sev->type;
			sub.id = sev->id;
		}
		rt_hw_spin_unlock(&fh->vdev->fh_lock);
		if (sev)
			v4l2_event_unsubscribe(fh, &sub);
	} while (sev);
}

int v4l2_event_unsubscribe(struct v4l2_fh *fh,
			   const struct v4l2_event_subscription *sub)
{
	struct v4l2_subscribed_event *sev;
	unsigned long flags;
	int i;

	if (sub->type == V4L2_EVENT_ALL) {
		v4l2_event_unsubscribe_all(fh);
		return 0;
	}

	rt_hw_spin_lock(&fh->vdev->fh_lock);

	sev = v4l2_event_subscribed(fh, sub->type, sub->id);
	if (sev != NULL) {
		/* Remove any pending events for this subscription */
		for (i = 0; i < sev->in_use; i++) {
			list_del(&sev->events[sev_pos(sev, i)].list);
			fh->navailable--;
		}
		list_del(&sev->list);
	}

	rt_hw_spin_unlock(&fh->vdev->fh_lock);

	if (sev && sev->ops && sev->ops->del)
		sev->ops->del(sev);

	rt_free(sev);

	return 0;
}

int v4l2_event_subdev_unsubscribe(struct v4l2_subdev *sd, struct v4l2_fh *fh,
				  struct v4l2_event_subscription *sub)
{
	return v4l2_event_unsubscribe(fh, sub);
}

static void v4l2_event_src_replace(struct v4l2_event *old,
				const struct v4l2_event *new)
{
	u32 old_changes = old->u.src_change.changes;

	old->u.src_change = new->u.src_change;
	old->u.src_change.changes |= old_changes;
}

static void v4l2_event_src_merge(const struct v4l2_event *old,
				struct v4l2_event *new)
{
	new->u.src_change.changes |= old->u.src_change.changes;
}

static const struct v4l2_subscribed_event_ops v4l2_event_src_ch_ops = {
	.replace = v4l2_event_src_replace,
	.merge = v4l2_event_src_merge,
};

int v4l2_src_change_event_subscribe(struct v4l2_fh *fh,
				const struct v4l2_event_subscription *sub)
{
	if (sub->type == V4L2_EVENT_SOURCE_CHANGE)
		return v4l2_event_subscribe(fh, sub, 0, &v4l2_event_src_ch_ops);
	return -EINVAL;
}

int v4l2_src_change_event_subdev_subscribe(struct v4l2_subdev *sd,
		struct v4l2_fh *fh, struct v4l2_event_subscription *sub)
{
	return v4l2_src_change_event_subscribe(fh, sub);
}
