#ifndef _LINUX_TYPES_H
#define _LINUX_TYPES_H

#include <melis/typedef.h>

#define DECLARE_BITMAP(name,bits) \
	unsigned long name[BITS_TO_LONGS(bits)]

typedef __u32 __le32;

typedef int		pid_t;

typedef     signed long long  loff_t;

#if defined CONFIG_SOC_SUN8IW19P1
typedef unsigned int		size_t;
#endif

typedef unsigned long pgoff_t;

#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
typedef u64 dma_addr_t;
#else
typedef u32 dma_addr_t;
#endif

typedef unsigned int gfp_t;

typedef struct {
	int counter;
} atomic_t;

#endif /* _LINUX_TYPES_H */
