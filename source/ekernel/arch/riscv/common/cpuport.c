/*
 * ===========================================================================================
 *
 *       Filename:  cpuport.c
 *
 *    Description:  glue layer of os with specific mach.
 *
 *        Version:  Melis3.0
 *         Create:  2020-06-28 19:12:26
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-07-27 10:40:12
 *
 * ===========================================================================================
 */

#include <rthw.h>
#include <rtthread.h>
#include "cpuport.h"
#include <csr.h>
#include <port.h>
#include <log.h>
#include <irqflags.h>
#include <preempt.h>
#include <csr.h>
#include <sbi.h>
#include <debug.h>

/* ----------------------------------------------------------------------------*/
/**
 * @brief  rt_hw_context_switch_to <trigger thread context switch>
 *
 * @param to, stack pointer of the next thread.
 */
/* ----------------------------------------------------------------------------*/
void rt_hw_context_switch_to(rt_ubase_t to)
{
    rt_thread_t thread;

    thread = rt_container_of(to, struct rt_thread, sp);

    // restore first task`s fpu context.
    void awos_arch_restore_fpu_status(fpu_context_t *);
    awos_arch_restore_fpu_status(&thread->fdext_ctx);

    void awos_arch_first_task_start(unsigned long to);
    return awos_arch_first_task_start(to);
}

void finish_task_switch(rt_thread_t ARG_UNUSED(last))
{
    unsigned long sstatus;

    sstatus = arch_local_save_flags();
    RT_ASSERT((sstatus & SR_FS) == SR_FS_CLEAN);
}

void awos_arch_save_fpu_status(fpu_context_t *);
void awos_arch_restore_fpu_status(fpu_context_t *);
static inline void awos_switch_to_aux(rt_thread_t prev, rt_thread_t next)
{
    unsigned long sstatus;
    switch_ctx_regs_t *regs_ctx;

    sstatus = arch_local_save_flags();
    if (unlikely(sstatus & SR_SD))
    {
        awos_arch_save_fpu_status(&prev->fdext_ctx);
    }

    regs_ctx = (switch_ctx_regs_t *)next->sp;
    if((regs_ctx->sstatus & SR_FS)!= SR_FS_OFF)
    {
        awos_arch_restore_fpu_status(&next->fdext_ctx);
    }
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  rt_hw_context_switch <swith thread from 'from' task to 'to' task.>
 *
 * @param from, sp of the thread need to sched out.
 * @param to, sp of the thread need to shced in.
 * The rt_hw_context_switch exception is the only execution context in the
 * system that can perform context switching. When an execution context finds
 * out it has to switch contexts, it call this.
 * When called, the decision that a context switch must happen has already been
 * taken. In other words, when it runs, we *know* we have to do context switch.
 */
/* ----------------------------------------------------------------------------*/
void rt_hw_context_switch(rt_ubase_t from, rt_ubase_t to)
{
    rt_ubase_t last;

    rt_thread_t prev = rt_list_entry(from, struct rt_thread, sp);
    rt_thread_t next = rt_list_entry(to, struct rt_thread, sp);

    int irqs_disabled(void);
    if (!irqs_disabled())
    {
        __err("irqmask status error.");
        software_break();
    }

#ifdef TRACE_SWITCH
    __log("-----------------------------------------------------------");
    __log("|high:[%8s][0x%08x]====>[%8s][0x%08x].|", \
          prev->name, prev->sched_o, next->name, next->sched_i);
#endif

    awos_switch_to_aux(prev, next);

    rt_ubase_t awos_arch_task_switch(rt_ubase_t from, rt_ubase_t to);
    last = awos_arch_task_switch(from, to);

    if (!irqs_disabled())
    {
        __err("irqmask status error.");
        software_break();
    }

    RT_ASSERT(rt_thread_self() == prev);

    rt_thread_t task_last = rt_list_entry(last, struct rt_thread, sp);

    finish_task_switch(task_last);

#ifdef TRACE_SWITCH
    __log("|oldr:[%8s][0x%08x]====>[%8s][0x%08x].|", \
          prev->name, prev->sched_o, next->name, next->sched_i);
    __log("|down:[%8s][0x%08x]<====[%8s][0x%08x].|", \
          prev->name, prev->sched_i, task_last->name, task_last->sched_o);
    __log("-----------------------------------------------------------");
#endif
}

/*
 * preempt schedule during exit irq environment.
 */
void schedule_preempt_inirq(void)
{
    // not allowd preemptation either scheduler lock or irqnest
    if (preempt_level() != 0 || hardirq_count() != 0)
    {
        return;
    }

    if (!irqs_disabled())
    {
        __err("irqmask status error.");
        software_break();
    }

    //irq procedure cant change the status of current thread, if so, report bug.
    //RT_ASSERT((rt_thread_self()->stat & RT_THREAD_STAT_MASK) == RT_THREAD_READY);
    /*
     * Notice: The arch_local_irq_enable/arch_lock_irq_disable should be work if
     * Each interrupt source perform the right acknowledge operations during the
     * irqhandler. but it seems did not ready now, so, each victim task(especially idle)
     * should prepare the irqstack by extend the task stack.
     * here, mask the enable/disable operations to avoid this.
     */
    do
    {
        //arch_local_irq_enable();
        rt_schedule();
        //arch_local_irq_disable();
    } while (thread_need_resched());

    if (!irqs_disabled())
    {
        __err("irqmask status error.");
        software_break();
    }

    return;
}


/**
 * This function will initialize thread stack
 *
 * @param tentry the entry of thread
 * @param parameter the parameter of entry
 * @param stack_addr the beginning stack address
 * @param texit the function will be called when thread exit
 *
 * @return stack address
 */
rt_uint8_t *rt_hw_stack_init(void       *tentry,
                             void       *parameter,
                             rt_uint8_t *stack_addr,
                             void       *texit)
{
    uint8_t *awos_arch_stack_init(void *tentry, void *parameter, uint8_t *stack_addr, void *texit);
    return awos_arch_stack_init(tentry, parameter, stack_addr, texit);
}

/** shutdown CPU */
void rt_hw_cpu_shutdown()
{
    rt_kprintf("shutdown...\n");

    rt_hw_interrupt_disable();
    sbi_shutdown();
}
