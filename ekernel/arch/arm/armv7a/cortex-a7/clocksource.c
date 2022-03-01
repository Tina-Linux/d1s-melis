/*
 * ===========================================================================================
 *
 *       Filename:  clocksource.c
 *
 *    Description:  clocksource operation
 *
 *        Version:  Melis3.0
 *         Create:  2019-12-17 18:10:31
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-06-02 13:52:55
 *
 * ===========================================================================================
 */

#include <stdint.h>
#include <stddef.h>
#include <rtthread.h>
#include <kconfig.h>
#include <barrier.h>
#include <ktimer.h>
#include <debug.h>
#include <stdio.h>
#include <init.h>
#include <log.h>

static uint32_t arch_timer_rate;
static inline uint32_t arch_timer_get_cntfrq(void)
{
    uint32_t val;
    asm volatile("mrc p15, 0, %0, c14, c0, 0" : "=r"(val));
    return val;
}

static inline uint64_t arch_counter_get_cntvct(void)
{
    uint64_t cval;

    isb();
    asm volatile("mrrc p15, 1, %Q0, %R0, c14" : "=r"(cval));
    return cval;
}

static inline uint64_t arch_counter_get_cntpct(void)
{
    uint64_t cval;

    isb();
    asm volatile("mrrc p15, 0, %Q0, %R0, c14" : "=r"(cval));
    return cval;
}

uint64_t (*arch_timer_read_counter)(void) = arch_counter_get_cntpct;
static cycle_t arch_counter_read(struct clocksource *cs)
{
    return arch_timer_read_counter();
}

static struct clocksource arch_timer_clocksource =
{
    .name   = "arch_sys_counter",
    .read   = arch_counter_read,
    .mask   = CLOCKSOURCE_MASK(56),
};

int64_t ktime_get(void)
{
    struct clocksource *cs = &arch_timer_clocksource;

    double ns_per_ticks = cs->mult / (1 << cs->shift);

    //~41.6666667 nano seconds per tick.
    ns_per_ticks = NS_PER_TICK;
    uint64_t arch_counter = cs->read(cs);

    return arch_counter * ns_per_ticks;
}

int do_gettimeofday(struct timespec64 *ts)
{
    if (ts == NULL)
    {
        __err("fatal error, not valide paramter.");
        software_break();
    }

    int64_t nsecs = ktime_get();

    ts->tv_sec  = nsecs / NSEC_PER_SEC;
    ts->tv_nsec = nsecs % NSEC_PER_SEC;

    return 0;
}

void timekeeping_init(void)
{
    struct clocksource *cs = &arch_timer_clocksource;

    cs->shift = 24;
    cs->mult = 699050667;
    cs->arch_timer_rate = arch_timer_get_cntfrq();
    if (cs->arch_timer_rate == 0)
    {
        cs->arch_timer_rate = 24000000;
    }

    return;
}

void timestamp(char *tag)
{
    struct timespec64 ts;
    do_gettimeofday(&ts);

    printf("[TSM]: %*.*s]:sec %lld, nsec %ld.\n", 12, 12, tag, ts.tv_sec, ts.tv_nsec);
}
