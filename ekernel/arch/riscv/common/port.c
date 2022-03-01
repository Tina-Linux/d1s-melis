/*
 * =====================================================================================
 *
 *       Filename:  port.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2020年06月08日 18时47分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zeng Zhijin
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <rtthread.h>
#include <port.h>
#include <csr.h>
#include <irqflags.h>

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_task_create <create task with specify parameter>
 *
 * @param name, thread name,
 * @param entry, entry function point of the task.
 * @param parameter, parameter for  'entry'
 * @param stack_size, stack size for stack.
 * @param priority, prority of the thread.
 * @param tick, time slice of the task, round-robin scheduler.
 *
 * @return
 *    -task handle if success.
 *    -NULL if faulre.
 */
/* ----------------------------------------------------------------------------*/
__hdle awos_task_create(const char *name, void (*entry)(void *parameter), \
                        void *parameter, uint32_t stack_size, uint8_t priority, \
                        uint32_t tick)
{
    rt_thread_t thr;

    thr = rt_thread_create(name, entry, parameter, stack_size, priority, tick);

    RT_ASSERT(thr != RT_NULL);
    rt_thread_startup(thr);

    return (__hdle)thr;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_task_delete <delete a task from system scheduler structure.>
 *
 * @param thread, handle for specify thread.
 *
 * @return
 *    -0 if success.
 */
/* ----------------------------------------------------------------------------*/
int32_t awos_task_delete(__hdle thread)
{
    RT_ASSERT(thread != RT_NULL);
    if (thread == rt_thread_self())
    {
        void rt_thread_exit(void);
        rt_thread_exit();
        CODE_UNREACHABLE;
    }
    else
    {
        rt_thread_delete((rt_thread_t)thread);
    }

    return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_task_self <return thread hanlde of current context.>
 *
 * @return
 *    -[hdl], hdl of current context thread handle.
 *    -NULL, if faiulre.
 */
/* ----------------------------------------------------------------------------*/
__hdle awos_task_self(void)
{
    return (__hdle)rt_thread_self();
}

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_irq_trap_enter <enter interrupt log.> */
/* ----------------------------------------------------------------------------*/
void awos_arch_irq_trap_enter(void)
{
    rt_interrupt_enter();
}

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_irq_trap_leave <leave interrupt log.> */
/* ----------------------------------------------------------------------------*/
void awos_arch_irq_trap_leave(void)
{
    rt_interrupt_leave();
}

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_tick_increase <system tick increase, must be invokded in
 *          timier irq context.>
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_tick_increase(void)
{
    rt_tick_increase();
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_isin_irq <judge whether in irq context>
 *
 * @return
 *   -0: in task environment
 *   -1: in interrupt environment.
 */
/* ----------------------------------------------------------------------------*/
uint8_t awos_arch_isin_irq(void)
{
    return rt_interrupt_get_nest() ?  1 : 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  k_malloc_align <alloc memory meet specify alignment.>
 *
 * @param[in] size, memory size need to alloc.
 * @param[in] align, aligment needs.
 *
 * @return
 *    -NULL, if faulre.
 *    -[ptr], address if success.
 */
/* ----------------------------------------------------------------------------*/
void *k_malloc_align(uint32_t size, uint32_t align)
{
    return rt_malloc_align(size, align);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  k_free_align <free align>
 *
 * @param[in] ptr, the memory start address need to free.
 */
/* ----------------------------------------------------------------------------*/
void k_free_align(void *ptr, uint32_t size)
{
    return rt_free_align(ptr);
}

unsigned long awos_arch_lock_irq(void)
{
    return arch_local_irq_save();
}

void awos_arch_unlock_irq(unsigned long cpu_sr)
{
    arch_local_irq_restore(cpu_sr);
}

rt_base_t rt_hw_interrupt_disable(void)
{
    return arch_local_irq_save();
}
RTM_EXPORT(rt_hw_interrupt_disable);

void rt_hw_interrupt_enable(rt_base_t level)
{
    arch_local_irq_restore(level);
}
RTM_EXPORT(rt_hw_interrupt_enable);

int irqs_disabled(void)
{
    return arch_irqs_disabled();
}

void local_irq_enable(void)
{
    arch_local_irq_enable();
}

void local_irq_disable(void)
{
    arch_local_irq_disable();
}
