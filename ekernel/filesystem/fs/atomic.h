/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
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