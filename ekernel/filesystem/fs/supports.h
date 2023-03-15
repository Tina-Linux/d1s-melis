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
#ifndef __SUPPORT_H__
#define __SUPPORT_H__
#include "sys_fsys_i.h"
#include <kconfig.h>
#include <kapi.h>
#include <arch.h>
/*
 * Generic macro to convert pointers to values for comparison purposes.
 */
#ifndef p2n
#define p2n(p)      ((ptrdiff_t)((ptrdiff_t*)(p)))
#endif

/*
 * min()/max() macros that also do
 * strict type-checking.. See the
 * "unnecessary" pointer comparison.
 */
#ifndef min
#define min(x,y) ({ \
        typeof(x) _x = (x); \
        typeof(y) _y = (y); \
        (void) (&_x == &_y);        \
        _x < _y ? _x : _y; })
#endif

#ifndef max
#define max(x,y) ({ \
        typeof(x) _x = (x); \
        typeof(y) _y = (y); \
        (void) (&_x == &_y);        \
        _x > _y ? _x : _y; })
#endif

/*
 * ..and if you can't take the strict
 * types, you can specify one yourself.
 *
 * Or not use min/max at all, of course.
 */
#define min_t(type,x,y) \
    ({ type __x = (x); type __y = (y); __x < __y ? __x: __y; })
#define max_t(type,x,y) \
    ({ type __x = (x); type __y = (y); __x > __y ? __x: __y; })

#ifdef FSYS_ALIGN
#undef FSYS_ALIGN
#endif
#define FSYS_ALIGN(val,align)   (((val) + ((align) - 1)) & ~((align) - 1))

#define BITS_PER_LONG 32
#define LONG_ALIGN(x) (((x)+(sizeof(long))-1)&~((sizeof(long))-1))
#define INT_MAX     ((int)(~0U>>1))
#define INT_MIN     (-INT_MAX - 1)
#define UINT_MAX    (~0U)
#define LONG_MAX    ((long)(~0UL>>1))
#define LONG_MIN    (-LONG_MAX - 1)
#define ULONG_MAX   (~0UL)
#define LLONG_MAX   ((long long)(~0ULL>>1))
#define LLONG_MIN   (-LLONG_MAX - 1)
#define ULLONG_MAX  (~0ULL)

typedef enum
{
#ifndef FALSE
    FALSE = 0,
#endif
#ifndef NO
    NO = 0,
#endif
#ifndef ZERO
    ZERO = 0,
#endif
#ifndef TRUE
    TRUE = 1,
#endif
#ifndef YES
    YES = 1,
#endif
#ifndef ONE
    ONE = 1,
#endif
#ifndef OK
    OK = 0,
#endif
#ifndef FAIL
    FAIL = -1,
#endif
} BOOL;

/*
 * Check at compile time that something is of a particular type.
 * Always evaluates to 1 so you may use it easily in comparisons.
 */
#define typecheck(type,x) \
    ({  type __dummy; \
        typeof(x) __dummy2; \
        (void)(&__dummy == &__dummy2); \
        1; \
    })
//#define offsetof(TYPE, MEMBER) ((unsigned int) &((TYPE *)0)->MEMBER)
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({          \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

static inline int is_power_of_2(unsigned long n)
{
    return (n != 0 && ((n & (n - 1)) == 0));
}

/*
 * Round up and down @num to 2 in power of @order.
 */
#define ROUND_UP(num,order) (((num) + ((1 << (order)) - 1)) & \
                             ~((1 << (order)) - 1))
#define ROUND_DOWN(num,order)   ((num) & ~((1 << (order)) - 1))

/**
 * test_bit - Determine whether a bit is set
 * @nr: bit number to test
 * @addr: Address to start counting from
 */
static inline int test_bit(unsigned int nr, const unsigned long *addr)
{
    int mask;

    addr += nr >> 5;
    mask = 1 << (nr & 0x1f);
    return ((mask & *addr) != 0);
}

/*
 * These functions are the basis of our bit ops.
 *
 * First, the atomic bitops. These use native endian.
 */
static inline void set_bit(unsigned int bit, volatile unsigned long *p)
{
    unsigned long flags;
    unsigned long mask = 1UL << (bit & 31);

    p += bit >> 5;

    flags = awos_arch_lock_irq();
    *p |= mask;
    awos_arch_unlock_irq(flags);
}

static inline void clear_bit(unsigned int bit, volatile unsigned long *p)
{
    unsigned long flags;
    unsigned long mask = 1UL << (bit & 31);

    p += bit >> 5;

    flags = awos_arch_lock_irq();
    *p &= ~mask;
    awos_arch_unlock_irq(flags);
}

static inline void change_bit(unsigned int bit, volatile unsigned long *p)
{
    unsigned long flags;
    unsigned long mask = 1UL << (bit & 31);

    p += bit >> 5;

    flags = awos_arch_lock_irq();
    *p ^= mask;
    awos_arch_unlock_irq(flags);
}

static inline int test_and_set_bit(unsigned int bit, volatile unsigned long *p)
{
    unsigned long flags;
    unsigned int res;
    unsigned long mask = 1UL << (bit & 31);

    p += bit >> 5;

    flags = awos_arch_lock_irq();
    res = *p;
    *p = res | mask;
    awos_arch_unlock_irq(flags);

    return res & mask;
}

static inline int test_and_clear_bit(unsigned int bit, volatile unsigned long *p)
{
    unsigned long flags;
    unsigned int res;
    unsigned long mask = 1UL << (bit & 31);

    p += bit >> 5;

    flags = awos_arch_lock_irq();
    res = *p;
    *p = res & ~mask;
    awos_arch_unlock_irq(flags);

    return res & mask;
}

static inline int test_and_change_bit(unsigned int bit, volatile unsigned long *p)
{
    unsigned long flags;
    unsigned int res;
    unsigned long mask = 1UL << (bit & 31);

    p += bit >> 5;

    flags = awos_arch_lock_irq();
    res = *p;
    *p = res ^ mask;
    awos_arch_unlock_irq(flags);

    return res & mask;
}

/* -------------------------------- jiffies -----------------------------*/

#define HZ      CONFIG_HZ
#define jiffies ((unsigned long)esKRNL_Time())

/*
 *  These inlines deal with timer wrapping correctly. You are
 *  strongly encouraged to use them
 *  1. Because people otherwise forget
 *  2. Because if the timer wrap changes in future you won't have to
 *     alter your driver code.
 *
 * time_after(a,b) returns true if the time a is after time b.
 *
 * Do this with "<0" and ">=0" to only test the sign of the result. A
 * good compiler would generate better code (and a really good compiler
 * wouldn't care). Gcc is currently neither.
 */
#define time_after(a,b)     \
    (typecheck(unsigned long, a) && \
     typecheck(unsigned long, b) && \
     ((int)(b) - (int)(a) < 0))
#define time_before(a,b)    time_after(b,a)

#define time_after_eq(a,b)  \
    (typecheck(unsigned long, a) && \
     typecheck(unsigned long, b) && \
     ((int)(a) - (int)(b) >= 0))
#define time_before_eq(a,b) time_after_eq(b,a)

#endif /* __SUPPORT_H__ */