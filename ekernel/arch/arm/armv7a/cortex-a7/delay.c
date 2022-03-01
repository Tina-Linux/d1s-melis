/*
 * ===========================================================================================
 *
 *       Filename:  delay.c
 *
 *    Description:  dealy operation
 *
 *        Version:  Melis3.0
 *         Create:  2019-12-17 18:10:31
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-04-16 14:41:49
 *
 * ===========================================================================================
 */
#include <stdint.h>
#include <stddef.h>
#include <rtthread.h>
#include <kconfig.h>
#include <ktimer.h>
#include <debug.h>
#include <stdio.h>
#include <init.h>
#include <log.h>

int64_t system_beginning_timestamp;

static inline uint64_t read_cntpct(void)
{
    uint64_t val;

    __asm__ __volatile__("mrrc p15, 0, %Q0, %R0, c14" : "=r"(val));
    return val;
}

static inline uint32_t read_cntfrq(void)
{
    uint32_t frq;

    __asm__ __volatile__("mrc p15, 0, %0, c14, c0, 0" : "=r"(frq));
    return frq;
}

void udelay(unsigned int us)
{
    uint64_t start, target;
    uint64_t frequency;

    frequency = read_cntfrq();
    start = read_cntpct();
    target = frequency / 1000000ULL * us;

    while (read_cntpct() - start <= target) ;
}

int usleep(unsigned int usecs)
{
    udelay(usecs);
    return 0;
}

int64_t sys_get_tick(void)
{
    return (int64_t)rt_tick_get();
}

static int sys_begin_time(void)
{
    system_beginning_timestamp = read_cntpct();
    //printf("[DBG]: %s line %d. bootup timer is %lld.\n", __func__, __LINE__, system_beginning_timestamp);
    return 0;
}

void show_bootup_time(void)
{
    int64_t bootup_time = ktime_get();
    printf("%s line %d, bootup cost %lld ticks\n", __func__, __LINE__, bootup_time);
}
pure_initcall(sys_begin_time);
