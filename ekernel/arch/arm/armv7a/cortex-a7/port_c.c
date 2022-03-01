/*
 * =====================================================================================
 *
 *       Filename:  port_c.c
 *
 *    Description:  porinting layer of arm processor platform.
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 11:58:23
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-05-19 11:58:35
 *
 * =====================================================================================
 */

#include <stddef.h>
#include <stdint.h>
#include <kconfig.h>
#include <rtthread.h>
#include <sys/prctl.h>
#include <debug.h>
#include <log.h>

/*****************************/
/* CPU Mode                  */
/*****************************/
#define ARM_IRQ_MASK    0x00000080
#define ARM_FIQ_MASK    0x00000040
#define USERMODE        0x10
#define FIQMODE         0x11
#define IRQMODE         0x12
#define SVCMODE         0x13
#define SYSMODE         0x1f
#define ABORTMODE       0x17
#define UNDEFMODE       0x1b
#define MODEMASK        0x1f
#define NOINT           0xc0
#define ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

#ifdef CONFIG_CONSISTENT_CONTEXT_SWITCH
void ret_from_create_s(void);
void kthread_enable_fpu(void);
void kthread_stop_fpu(void);
void rt_thread_newborn(void)
{
    kthread_enable_fpu();
    //kthread_stop_fpu();
    return;
}

void ret_from_create_c(void)
{
    register long r4 __asm__("r4");
    register long r5 __asm__("r5");

    void (*entry)(void *) = (void (*)(void *))r5;
    if (entry)
    {
        rt_thread_newborn();

        if ((uint32_t)entry & 0x01)
        {
            entry((void *)r4);
        }
        else
        {
            entry((void *)r4);
        }
    }

    return;
}
#endif
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
uint8_t *awos_arch_stack_init(void *tentry, void *parameter,
                              uint8_t *stack_addr, void *texit)
{
    uint32_t *stk;
    uint32_t cpsr;
    uint32_t wrapper_entry;

    stack_addr += sizeof(uint32_t);
    stack_addr = (uint8_t *)ALIGN_DOWN((uint32_t)stack_addr, 8);
    stk        = (uint32_t *)stack_addr;

#ifdef CONFIG_CONSISTENT_CONTEXT_SWITCH
#if 1
    wrapper_entry = (uint32_t)ret_from_create_c;
#else
    wrapper_entry = (uint32_t)ret_from_create_s;
#endif

    *(--stk) = wrapper_entry;               /* entry point */
    *(--stk) = (uint32_t)texit;             /* lr */
    *(--stk) = 0xdeadbeef;                  /* r12 */
    *(--stk) = 0xdeadbeef;                  /* r11 */
    *(--stk) = 0xdeadbeef;                  /* r10 */
    *(--stk) = 0xdeadbeef;                  /* r9 */
    *(--stk) = 0xdeadbeef;                  /* r8 */
    *(--stk) = 0xdeadbeef;                  /* r7 */
    *(--stk) = 0xdeadbeef;                  /* r6 */
    *(--stk) = (uint32_t)tentry;            /* r5 : entry*/
    *(--stk) = (uint32_t)parameter;         /* r4 : argument */

    /*
     * r0-r4 are caller registes, need not initialize
     * because rt_schedule would always be called
     * from c code.
     */
#else
    wrapper_entry = (uint32_t)tentry;

    *(--stk) = wrapper_entry;               /* entry point */
    *(--stk) = (uint32_t)texit;             /* lr */
    *(--stk) = 0xdeadbeef;                  /* r12 */
    *(--stk) = 0xdeadbeef;                  /* r11 */
    *(--stk) = 0xdeadbeef;                  /* r10 */
    *(--stk) = 0xdeadbeef;                  /* r9 */
    *(--stk) = 0xdeadbeef;                  /* r8 */
    *(--stk) = 0xdeadbeef;                  /* r7 */
    *(--stk) = 0xdeadbeef;                  /* r6 */
    *(--stk) = 0xdeadbeef;                  /* r5 */
    *(--stk) = 0xdeadbeef;                  /* r4 */
    *(--stk) = 0xdeadbeef;                  /* r3 */
    *(--stk) = 0xdeadbeef;                  /* r2 */
    *(--stk) = 0xdeadbeef;                  /* r1 */
    *(--stk) = (uint32_t)parameter;         /* r0 : argument */
#endif
    /* cpsr setting, clear N,Z,C,V,I. disable fiq capacity use SVC mode. */
    if (wrapper_entry & 0x01)
    {
        cpsr = (SVCMODE | ARM_FIQ_MASK | 0x20);    /* thumb mode */
    }
    else
    {
        cpsr = (SVCMODE | ARM_FIQ_MASK);           /* arm mode   */
    }

    *(--stk) = cpsr;

    /*  FCSE did not enabled at present. */
    *(--stk) = 0x0;

    /*
     *  The task will start without a floating point context. A task that
     *  need to use the neon&vfp hardwore must be enable the function expicity.
     */
    *(--stk) = 0x0;
    /* return task's current stack address */
    return (uint8_t *)stk;
}

/**
 * This function finds the first bit set (beginning with the least significant bit)
 * in value and return the index of that bit.
 *
 * Bits are numbered starting at 1 (the least significant bit).  A return value of
 * zero from any of these functions means that the argument was zero.
 *
 * @return return the index of the first bit set. If value is 0, then this function
 * shall return 0.
 */
int32_t awos_arch_ffs(int32_t value)
{
    register uint32_t x;

    if (value == 0)
    {
        return 0;
    }

    __asm__ __volatile__
    (
        "rsb %[temp], %[val], #0\n"
        "and %[temp], %[temp], %[val]\n"
        "clz %[temp], %[temp]\n"
        "rsb %[temp], %[temp], #32\n"
        :[temp] "=r"(x)
        :[val] "r"(value)
    );
    return x;
}

static int get_task_comm(char *comm, rt_thread_t thread)
{
    return 0;
}

static int set_task_comm(char *comm, rt_thread_t thread)
{
    char tmp[RT_NAME_MAX] = {0};

    rt_strncpy(tmp, comm, RT_NAME_MAX - 1);
    rt_thread_nameset(thread, tmp);
    return 0;
}

int prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5)
{
    rt_thread_t self = rt_thread_self();
    if (self == RT_NULL)
    {
        __err("scheduler not startup.");
        software_break();
    }

    switch (option)
    {
        case PR_GET_NAME:
        {
            get_task_comm((char *)arg2, self);
            break;
        }
        case PR_SET_NAME:
        {
            set_task_comm((char *)arg2, self);
            break;
        }
    }

    return 0;
}
