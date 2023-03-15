/*
 *  atomic.h
 *
 *  Copyright (C) 1996 Russell King.
 *  Copyright (C) 2002 Deep Blue Solutions Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __ASM_ARM_ATOMIC_H
#define __ASM_ARM_ATOMIC_H

#include "xr_types.h"
#include "irqflags.h"

typedef struct {
	volatile int counter;
} atomic_t;

#define ATOMIC_INIT(i)	{ (i) }

/*
 * On ARM, ordinary assignment (str instruction) doesn't clear the local
 * strex/ldrex monitor on some implementations. The reason we can use it for
 * atomic_set() is the clrex or dummy strex done on every exception return.
 */
#define atomic_read(v)	(*(volatile int *)&(v)->counter)
#define atomic_set(v,i)	(((v)->counter) = (i))

int atomic_add_return(int i, atomic_t *v);

#define atomic_add(i, v)	(void) atomic_add_return(i, v)

#if defined(CONFIG_USE_RTTHREAD_PORT) || defined(CONFIG_USE_YUNOS_PORT) \
		|| defined(CONFIG_OS_NUTTX)
int atomic_sub_return(int i, atomic_t *v);
int atomic_cmpxchg(atomic_t *v, int old, int new_v);
void atomic_clear_mask(unsigned long mask, unsigned long *addr);
#else
static XR_INLINE int atomic_sub_return(int i, atomic_t *v)
{
	unsigned long flags;
	int val;

	raw_local_irq_save(flags);
	val = v->counter;
	v->counter = val -= i;
	raw_local_irq_restore(flags);

	return val;
}

static XR_INLINE int atomic_cmpxchg(atomic_t *v, int old, int new_v)
{
	int ret;
	unsigned long flags;

	raw_local_irq_save(flags);
	ret = v->counter;
	if (likely(ret == old))
		v->counter = new_v;
	raw_local_irq_restore(flags);

	return ret;
}

static XR_INLINE void atomic_clear_mask(unsigned long mask, unsigned long *addr)
{
	unsigned long flags;

	raw_local_irq_save(flags);
	*addr &= ~mask;
	raw_local_irq_restore(flags);
}
#endif

#define atomic_sub(i, v)	(void) atomic_sub_return(i, v)

static XR_INLINE void atomic_inc(atomic_t *v)
{
	atomic_add_return(1, v);
}

static XR_INLINE void atomic_dec(atomic_t *v)
{
	atomic_sub_return(1, v);
}

#define atomic_dec_return(v)		atomic_sub_return(1, (v))
#define atomic_inc_return(v)		atomic_add_return(1, (v))

#define atomic_sub_and_test(i, v)	(atomic_sub_return((i), (v)) == 0)
#define atomic_dec_and_test(v)		(atomic_dec_return(v) == 0)
#define atomic_inc_and_test(v)		(atomic_inc_return(v) == 0)

#ifndef xchg

/*
 * This function doesn't exist, so you'll get a linker error if
 * something tries to do an invalidly-sized xchg().
 */

static XR_INLINE
unsigned long __xchg(unsigned long x, volatile void *ptr, int size)
{
	unsigned long ret, flags;

	switch (size) {
	case 1:
#ifdef __xchg_u8
		return __xchg_u8(x, ptr);
#else
		local_irq_save(flags);
		ret = *(volatile u8 *)ptr;
		*(volatile u8 *)ptr = x;
		local_irq_restore(flags);
		return ret;
#endif /* __xchg_u8 */

	case 2:
#ifdef __xchg_u16
		return __xchg_u16(x, ptr);
#else
		local_irq_save(flags);
		ret = *(volatile u16 *)ptr;
		*(volatile u16 *)ptr = x;
		local_irq_restore(flags);
		return ret;
#endif /* __xchg_u16 */

	case 4:
#ifdef __xchg_u32
		return __xchg_u32(x, ptr);
#else
		local_irq_save(flags);
		ret = *(volatile u32 *)ptr;
		*(volatile u32 *)ptr = x;
		local_irq_restore(flags);
		return ret;
#endif /* __xchg_u32 */

#ifdef CONFIG_64BIT
	case 8:
#ifdef __xchg_u64
		return __xchg_u64(x, ptr);
#else
		local_irq_save(flags);
		ret = *(volatile u64 *)ptr;
		*(volatile u64 *)ptr = x;
		local_irq_restore(flags);
		return ret;
#endif /* __xchg_u64 */
#endif /* CONFIG_64BIT */

	default:
		//BUG_ON(1);
		return x;
	}
}

#define xchg(ptr, x) \
	(__xchg((unsigned long)(x), (ptr), sizeof(*(ptr))))
//(__typeof__(*(ptr)))
#define atomic_xchg(ptr, v)		(xchg(&(ptr)->counter, (v)))

#endif /* xchg */

#endif
