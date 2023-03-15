#ifndef _LINUX_KERNEL_H
#define _LINUX_KERNEL_H

#include "sys/defs.h"

/* Trap pasters of __FUNCTION__ at compile-time */
#define __FUNCTION__ (__func__)

/* This helps us to avoid #ifdef CONFIG_NUMA */
#ifdef CONFIG_NUMA
#define NUMA_BUILD 1
#else
#define NUMA_BUILD 0
#endif

#endif
