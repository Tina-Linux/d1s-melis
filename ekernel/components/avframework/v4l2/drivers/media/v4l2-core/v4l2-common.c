/*
 *	Video for Linux Two
 *
 *	A generic video device interface for the LINUX operating system
 *	using a set of device structures/vectors for low level operations.
 *
 *	This file replaces the videodev.c file that comes with the
 *	regular kernel distribution.
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 *
 * Author:	Bill Dirks <bill@thedirks.org>
 *		based on code by Alan Cox, <alan@cymru.net>
 *
 */

/*
 * Video capture interface for Linux
 *
 *	A generic video device interface for the LINUX operating system
 *	using a set of device structures/vectors for low level operations.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 * Author:	Alan Cox, <alan@lxorguk.ukuu.org.uk>
 *
 * Fixes:
 */

/*
 * Video4linux 1/2 integration by Justin Schoeman
 * <justin@suntiger.ee.up.ac.za>
 * 2.4 PROCFS support ported from 2.4 kernels by
 *  Iñaki García Etxebarria <garetxe@euskalnet.net>
 * Makefile fix by "W. Michael Petullo" <mike@flyn.org>
 * 2.4 devfs support ported from 2.4 kernels by
 *  Dan Merillat <dan@merillat.org>
 * Added Gerd Knorrs v4l1 enhancements (Justin Schoeman)
 */

//#include "v4l2-types.h"
#include <string.h>
#include <errno.h>

#include <melis/ktimer.h>

#include "v4l2-common.h"
#include "v4l2-device.h"
#include "v4l2-ctrls.h"

#include "linux/videodev2.h"


/*
 *
 *	V 4 L 2   D R I V E R   H E L P E R   A P I
 *
 */

/*
 *  Video Standard Operations (contributed by Michael Schimek)
 */

/* Helper functions for control handling			     */

/* Fill in a struct v4l2_queryctrl */
int v4l2_ctrl_query_fill(struct v4l2_queryctrl *qctrl, s32 _min, s32 _max, s32 _step, s32 _def)
{
	const char *name;
	s64 min = _min;
	s64 max = _max;
	u64 step = _step;
	s64 def = _def;

	v4l2_ctrl_fill(qctrl->id, &name, (void *)&qctrl->type,
		       &min, &max, &step, &def, &qctrl->flags);

	if (name == NULL)
		return -EINVAL;

	qctrl->minimum = min;
	qctrl->maximum = max;
	qctrl->step = step;
	qctrl->default_value = def;
	qctrl->reserved[0] = qctrl->reserved[1] = 0;
	strlcpy(qctrl->name, name, sizeof(qctrl->name));
	return 0;
}

/* Clamp x to be between min and max, aligned to a multiple of 2^align.  min
 * and max don't have to be aligned, but there must be at least one valid
 * value.  E.g., min=17,max=31,align=4 is not allowed as there are no multiples
 * of 16 between 17 and 31.  */
static unsigned int clamp_align(unsigned int x, unsigned int min,
				unsigned int max, unsigned int align)
{
	/* Bits that must be zero to be aligned */
	unsigned int mask = ~((1 << align) - 1);

	/* Clamp to aligned min and max */
	x = clamp(x, (min + ~mask) & mask, max & mask);

	/* Round to nearest aligned value */
	if (align)
		x = (x + (1 << (align - 1))) & mask;

	return x;
}

/* Bound an image to have a width between wmin and wmax, and height between
 * hmin and hmax, inclusive.  Additionally, the width will be a multiple of
 * 2^walign, the height will be a multiple of 2^halign, and the overall size
 * (width*height) will be a multiple of 2^salign.  The image may be shrunk
 * or enlarged to fit the alignment constraints.
 *
 * The width or height maximum must not be smaller than the corresponding
 * minimum.  The alignments must not be so high there are no possible image
 * sizes within the allowed bounds.  wmin and hmin must be at least 1
 * (don't use 0).  If you don't care about a certain alignment, specify 0,
 * as 2^0 is 1 and one byte alignment is equivalent to no alignment.  If
 * you only want to adjust downward, specify a maximum that's the same as
 * the initial value.
 */
void v4l_bound_align_image(u32 *w, unsigned int wmin, unsigned int wmax,
			   unsigned int walign,
			   u32 *h, unsigned int hmin, unsigned int hmax,
			   unsigned int halign, unsigned int salign)
{
	*w = clamp_align(*w, wmin, wmax, walign);
	*h = clamp_align(*h, hmin, hmax, halign);

	/* Usually we don't need to align the size and are done now. */
	if (!salign)
		return;

	/* How much alignment do we have? */
	walign = __ffs(*w);
	halign = __ffs(*h);
	/* Enough to satisfy the image alignment? */
	if (walign + halign < salign) {
		/* Max walign where there is still a valid width */
		unsigned int wmaxa = __fls(wmax ^ (wmin - 1));
		/* Max halign where there is still a valid height */
		unsigned int hmaxa = __fls(hmax ^ (hmin - 1));

		/* up the smaller alignment until we have enough */
		do {
			if (halign >= hmaxa ||
			    (walign <= halign && walign < wmaxa)) {
				*w = clamp_align(*w, wmin, wmax, walign + 1);
				walign = __ffs(*w);
			} else {
				*h = clamp_align(*h, hmin, hmax, halign + 1);
				halign = __ffs(*h);
			}
		} while (halign + walign < salign);
	}
}

const struct v4l2_frmsize_discrete *v4l2_find_nearest_format(
		const struct v4l2_discrete_probe *probe,
		s32 width, s32 height)
{
	int i;
	u32 error, min_error = UINT_MAX;
	const struct v4l2_frmsize_discrete *size, *best = NULL;

	if (!probe)
		return best;

	for (i = 0, size = probe->sizes; i < probe->num_sizes; i++, size++) {
		error = abs((int)(size->width - width)) + abs((int)(size->height - height));
		if (error < min_error) {
			min_error = error;
			best = size;
		}
		if (!error)
			break;
	}

	return best;
}

void v4l2_get_timestamp(struct timeval *tv)
{
	struct timespec ts;

	ktime_get_ts(&ts);
	tv->tv_sec = ts.tv_sec;
	tv->tv_usec = ts.tv_nsec / NSEC_PER_USEC;
}
