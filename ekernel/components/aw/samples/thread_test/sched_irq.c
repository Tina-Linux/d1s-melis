/*
 * =====================================================================================
 *
 *       Filename:  sched_irq.c
 *
 *    Description:  schedule during irq lock.
 *
 *        Version:  2.0
 *         Create:  2017-11-09 15:37:55
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2018-11-30 12:21:55
 *
 * =====================================================================================
 */

#include <typedef.h>
#include <rtthread.h>
#include <rthw.h>
#include <arch.h>
#include <log.h>

/* ----------------------------------------------------------------------------*/
/**
 * @brief  irq_lock_thread
 *
 * @param para, not used.
 */
/* ----------------------------------------------------------------------------*/
static void irq_lock_thread_1(void *ARG_UNUSED(para))
{
    uint32_t cpsr;
    uint32_t curcpsr;

    //each task has its own cpsr context, so irq & fiq environt not shared.
    while (1)
    {
        curcpsr = __get_cpsr();
        __log("comm = %s. curcpsr = 0x%08x.", rt_thread_self()->name, curcpsr);
        cpsr = rt_hw_interrupt_disable();
        curcpsr = __get_cpsr();
        __log("comm = %s. curcpsr = 0x%08x.", rt_thread_self()->name, curcpsr);
        rt_thread_delay(100);
        curcpsr = __get_cpsr();
        __log("comm = %s. curcpsr = 0x%08x.", rt_thread_self()->name, curcpsr);
        rt_hw_interrupt_enable(cpsr);
        curcpsr = __get_cpsr();
        __log("comm = %s. curcpsr = 0x%08x.", rt_thread_self()->name, curcpsr);
    }
}

static void irq_lock_thread_2(void *ARG_UNUSED(para))
{
    uint32_t curcpsr;
    while (1)
    {
        curcpsr = __get_cpsr();
        __log("comm = %s. curcpsr = 0x%08x.", rt_thread_self()->name, curcpsr);
        rt_thread_delay(1000);
        curcpsr = __get_cpsr();
        __log("comm = %s. curcpsr = 0x%08x.", rt_thread_self()->name, curcpsr);
    }

}

/* ----------------------------------------------------------------------------*/
/*
 *  @brief  schedule_in_irqlock <test pattern, Thread swith out when iterrupt
 *          disable>
 */
/* ----------------------------------------------------------------------------*/
void schedule_in_irqlock(void)
{
    rt_thread_t thread;
    thread = rt_thread_create("irq_lock_1", irq_lock_thread_1, RT_NULL, 0x1000, 1, 10);
    rt_thread_startup(thread);

    thread = rt_thread_create("irq_lock_2", irq_lock_thread_2, RT_NULL, 0x1000, 1, 10);
    rt_thread_startup(thread);
}
