/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : atomic.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: atomic operation sevices, code is extracted from linux.
* Update  : date                auther      ver     notes
*           2011-3-15 15:26:17  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __ATOMIC_H__
#define __ATOMIC_H__
#include "fsys_libs.h"

extern __u32 CPU_SR_Save(void);
extern void CPU_SR_Restore(__u32 x);

typedef struct
{
    volatile int counter;
} atomic_t;

#define ATOMIC_INIT(i)  { (i) }

#define atomic_read(v)  ((v)->counter)

#define atomic_set(v,i) (((v)->counter) = (i))

static inline int atomic_add_return(int i, atomic_t *v)
{
    unsigned long flags;
    int val;

    flags = awos_arch_lock_irq();
    val = v->counter;
    v->counter = val += i;
    awos_arch_unlock_irq(flags);

    return val;
}

static inline int atomic_sub_return(int i, atomic_t *v)
{
    unsigned long flags;
    int val;

    flags = awos_arch_lock_irq();
    val = v->counter;
    v->counter = val -= i;
    awos_arch_unlock_irq(flags);

    return val;
}

static inline int atomic_cmpxchg(atomic_t *v, int old, int new)
{
    int ret;
    unsigned long flags;

    flags = awos_arch_lock_irq();
    ret = v->counter;
    if (ret == old)
    {
        v->counter = new;
    }
    awos_arch_unlock_irq(flags);

    return ret;
}

static inline void atomic_clear_mask(unsigned long mask, unsigned long *addr)
{
    unsigned long flags;

    flags = awos_arch_lock_irq();
    *addr &= ~mask;
    awos_arch_unlock_irq(flags);
}

//#define atomic_xchg(v, new) (xchg(&((v)->counter), new))

static inline int atomic_add_unless(atomic_t *v, int a, int u)
{
    int c, old;

    c = atomic_read(v);
    while (c != u && (old = atomic_cmpxchg((v), c, c + a)) != c)
    {
        c = old;
    }
    return c != u;
}
#define atomic_inc_not_zero(v) atomic_add_unless((v), 1, 0)

#define atomic_add(i, v)    (void) atomic_add_return(i, v)
#define atomic_inc(v)       (void) atomic_add_return(1, v)
#define atomic_sub(i, v)    (void) atomic_sub_return(i, v)
#define atomic_dec(v)       (void) atomic_sub_return(1, v)

#define atomic_inc_and_test(v)  (atomic_add_return(1, v) == 0)
#define atomic_dec_and_test(v)  (atomic_sub_return(1, v) == 0)
#define atomic_inc_return(v)    (atomic_add_return(1, v))
#define atomic_dec_return(v)    (atomic_sub_return(1, v))
#define atomic_sub_and_test(i, v) (atomic_sub_return(i, v) == 0)

#define atomic_add_negative(i,v) (atomic_add_return(i, v) < 0)

#define atomic_dec_and_lock(atomic,lock) atomic_dec_and_test(atomic)

#endif  /* __ATOMIC_H__ */
