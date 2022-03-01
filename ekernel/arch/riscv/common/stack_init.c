/*
 * =====================================================================================
 *
 *       Filename:  stack_init.c
 *
 *    Description:  RISCV Processor C-ABI thread stack init.
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 11:58:23
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-07-02 10:04:02
 *
 * =====================================================================================
 */
#include <excep.h>
#include <rtthread.h>
#include <cpuport.h>
#include <typedef.h>
#include <irqflags.h>
#include <debug.h>
#include <rtdef.h>
#include <csr.h>
#if (CONFIG_LOG_DEFAULT_LEVEL == 0)
#define rt_kprintf(...)
#endif
void finish_task_switch(rt_thread_t ARG_UNUSED(last));
void ret_from_create_c(unsigned long *frsp, unsigned long *tosp)
{
    rt_thread_t last =  RT_NULL;
    rt_thread_t curr =  RT_NULL;

    // thread fn of entry.
    register long s1 __asm__("s1");
    // thread paramter of entry.
    register long s2 __asm__("s2");
    // thread exit entry.
    register long s3 __asm__("s3");

    void (*entry)(void *) = (void (*)(void *))s1;
    void (*exity)(void)   = (void (*)(void))s3;
    void *parameter       = (void *)s2;

    if (frsp == RT_NULL)
    {
        rt_kprintf("scheduler startup\n");
    }
    else
    {
        last = rt_container_of(frsp, struct rt_thread, sp);
    }

    finish_task_switch(last);

    curr = rt_container_of(tosp, struct rt_thread, sp);
    if (last == rt_thread_self())
    {
        software_break();
    }
    else if (curr != rt_thread_self())
    {
        software_break();
    }

    // enable local irq.
    //arch_local_irq_enable();
    if (irqs_disabled())
    {
        software_break();
    }

    if (exity != rt_thread_exit)
    {
        software_break();
    }

    if (entry)
    {
        entry(parameter);
    }
    else
    {
        software_break();
    }

    exity();

    /* never come here */
    CODE_UNREACHABLE;
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
uint8_t *awos_arch_stack_init(void *tentry, void *parameter, uint8_t *stack_addr, void *texit)
{
    rt_uint8_t         *stk;
    switch_ctx_regs_t *frame;

    stk  = stack_addr + sizeof(rt_ubase_t);
    stk  = (rt_uint8_t *)RT_ALIGN_DOWN((rt_ubase_t)stk, REGBYTES);
    stk -= sizeof(switch_ctx_regs_t);

    frame = (switch_ctx_regs_t *)stk;

    frame->ra      = (rt_ubase_t)ret_from_create_c;

    // ABI take as FP, so cant pass parameter by S0.
    frame->s[0]    = (rt_ubase_t) -1;
    frame->s[1]    = (rt_ubase_t)tentry;
    frame->s[2]    = (rt_ubase_t)parameter;
    frame->s[3]    = (rt_ubase_t)texit;
    frame->s[4]    = (rt_ubase_t)0xdeadbeef;
    frame->s[5]    = (rt_ubase_t)0xdeadbeef;
    frame->s[6]    = (rt_ubase_t)0xdeadbeef;
    frame->s[7]    = (rt_ubase_t)0xdeadbeef;
    frame->s[8]    = (rt_ubase_t)0xdeadbeef;
    frame->s[9]    = (rt_ubase_t)0xdeadbeef;
    frame->s[10]   = (rt_ubase_t)0xdeadbeef;
    frame->s[11]   = (rt_ubase_t)0xdeadbeef;

    // Restore Previous Interrupt Enable and Previous Privledge level status.
    //frame->sstatus = SR_SPP | SR_SPIE; /*  Supervisor, irqs on, Initial fpu for ready to use */
    frame->sstatus = SR_SPP | SR_SPIE | SR_FS_CLEAN; /*  Supervisor, irqs on, Initial fpu for ready to use */

    return stk;
}
