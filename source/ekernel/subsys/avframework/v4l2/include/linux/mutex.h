/*
 * Mutexes: blocking mutual exclusion locks
 *
 * started by Ingo Molnar:
 *
 *  Copyright (C) 2004, 2005, 2006 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *
 * This file contains the main data structure and API definitions.
 */
#ifndef __LINUX_MUTEX_H
#define __LINUX_MUTEX_H

//#include <asm/current.h>
//#include <linux/list.h>
//#include <linux/spinlock_types.h>
//#include <linux/linkage.h>
//#include <linux/lockdep.h>
//#include <linux/atomic.h>
//#include <asm/processor.h>
//#include <linux/osq_lock.h>

#include <rtdef.h>

#define __RT_MUTEX_INITIALIZER(lockname) \
		{ .value = 1 \
		, .owner = RT_NULL \
		, .original_priority = 0xFF \
		, .hold = 0 \
		}

#define DEFINE_MUTEX(mname) \
	struct rt_mutex mname = __RT_MUTEX_INITIALIZER(mname);

#endif /* __LINUX_MUTEX_H */
