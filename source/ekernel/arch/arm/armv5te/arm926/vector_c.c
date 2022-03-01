/*
 * =====================================================================================
 *
 *       Filename:  vector_c.c
 *
 *    Description:  rotuine to dealt with exception. based on ARM926EJ-S
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 13:43:10
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-05-15 16:20:17
 *
 * =====================================================================================
 */

#include <typedef.h>
#include <sys_int.h>
#include <excep.h>
#include <kconfig.h>
#include <log.h>

#ifdef CONFIG_KGDB
#include <kgdb.h>
#endif

#ifdef CONFIG_RTTKERNEL
#include <rtthread.h>
#endif

#define DABT_TYPE               (0)
#define IABT_TYPE               (1)
#define UNDF_TYPE               (2)

#define FSR_LNX_PF              (1 << 31)
#define FSR_WRITE               (1 << 11)
#define FSR_FS4                 (1 << 10)
#define FSR_FS3_0               (15)
#define FSR_FS5_0               (0x3f)
#define FSR_FS_AEA              22

static inline int fsr_fs(unsigned int fsr)
{
    return (fsr & FSR_FS3_0) | (fsr & FSR_FS4) >> 6;
}
static char *fsr_info[] =
{
    /*
    ¦* The following are the standard ARMv3 and ARMv4 aborts.  ARMv5
    ¦* defines these to be "precise" aborts.
    ¦*/
    "vector exception",
    "alignment exception",
    "terminal exception",
    "alignment exception",
    "external abort on linefetch",
    "section translation fault",
    "external abort on linefetch",
    "page translation fault",
    "external abort on non-linefetch",
    "section domain fault",
    "external abort on non-linefetch",
    "page domain fault",
    "external abort on translation",
    "section permission fault",
    "external abort on translation",
    "page permission fault",
    "unknown 16",
    "unknown 17",
    "unknown 18",
    "unknown 19",
    "lock abort",                       /* xscale */
    "unknown 21",
    "imprecise external abort",         /* xscale */
    "unknown 23",
    "dcache parity error",              /* xscale */
    "unknown 25",
    "unknown 26",
    "unknown 27",
    "unknown 28",
    "unknown 29",
    "unknown 30",
    "unknown 31",
};

static void dump_memory(uint32_t *buf, int32_t len)
{
    int i;

    printk("\n\rdump stack memory:");
    for (i = 0; i < len; i ++)
    {
        if ((i % 4) == 0)
        {
            printk("\n\r0x%p: ", buf + i);
        }
        printk("0x%08x ", buf[i]);
    }
    printk("\n\r");

    return;
}

void memory_info(void)
{
#ifdef CONFIG_RTTKERNEL
    rt_uint32_t total, used, max_used;

    rt_memory_info(&total, &used, &max_used);

    printk("\n\rmemory info:\n\r");
    printk("    Total  0x%08x\n\r" \
           "    Used   0x%08x\r\n" \
           "    Max    0x%08x\r\n" \
           "    Margin 0x%08x\n\r", \
           total, used, max_used, total - max_used);
#endif
}

/**
 * this function will show registers of CPU
 *
 * @param regs the registers point
 */

static void show_register(excep_regs_t *regs, uint32_t victim, int32_t type)
{
#ifdef CONFIG_RTTKERNEL
    printk("thread: %s, entry: 0x%p, stack_base: 0x%08x\n" \
           "stack_size: 0x%08x.\n", \
           rt_thread_self()->name, \
           rt_thread_self()->entry, \
           rt_thread_self()->stack_addr, \
           rt_thread_self()->stack_size);
#endif
    printk("cpsr: 0x%08x.\n", regs->orig_cpsr);

    printk("\ngprs:\n");
    printk("r00:0x%08x r01:0x%08x r02:0x%08x r03:0x%08x\n",
           regs->r0, regs->r1, regs->r2, regs->r3);
    printk("r04:0x%08x r05:0x%08x r06:0x%08x r07:0x%08x\n",
           regs->r4, regs->r5, regs->r6, regs->r7);
    printk("r08:0x%08x r09:0x%08x r10:0x%08x r11:0x%08x\n",
           regs->r8, regs->r9, regs->r10, regs->r11);
    printk("r12:0x%08x  sp:0x%08x  lr:0x%08x  pc:0x%08x\n",
           regs->r12, regs->sp, regs->lr, victim);

    printk("\ncp15:\n");
    printk("fst_fsid:0x%08x\n" \
           "abt_dfar:0x%08x\n" \
           "abt_ifsr:0x%08x\n" \
           "abt_dfsr:0x%08x\n" \
           "abt_lr  :0x%08x\n" \
           "abt_sp  :0x%p  \n", \
           regs->fcsid, regs->abt_dFAR, regs->abt_iFSR, regs->abt_dFSR, regs->abt_lr, regs);

    printk("\ncause:\n");
    if (type == DABT_TYPE)
    {
        printk("DFSR signs %s.\n",  fsr_info[fsr_fs(regs->abt_dFSR)]);
    }
    else if (type == IABT_TYPE)
    {
        printk("IFSR signs %s.\n",  fsr_info[fsr_fs(regs->abt_iFSR)]);
    }
    else
    {
        printk("undefind instructions.\n\r");
    }

    printk("\r\n");

    dump_memory((uint32_t *)regs->sp, 64);
    memory_info();
    printk("\r\n");

#ifdef CONFIG_RTTKERNEL
    extern void dump_stack_nofp(unsigned int pc, unsigned int sp, int level, unsigned int *last_pointer, int flag);
    extern long list_thread(void);
    //dump_stack_nofp(victim, regs->sp, 0, NULL, 0);
    /*list_thread();*/
#endif
    void jtag_enable_op(void);
    jtag_enable_op();
    printk("================================================================\n");
}

/**
 * When ARM7TDMI comes across an instruction which it cannot handle,
 * it takes the undefined instruction trap.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void awos_arch_und_trap_entry(excep_regs_t *regs)
{
    volatile uint8_t  i = 1;
    uint32_t victim = regs->abt_lr - 0;

#ifdef CONFIG_KGDB
    regs->abt_lr -= 4;
    if (gdb_undef_hook(regs))
    {
        return;
    }
#endif

    printk("================================================================\n");
    printk("                  undefined instructions trap                   \n");
    printk("================================================================\n");

    show_register(regs, victim, UNDF_TYPE);
    regs->abt_lr = victim;

    while (i);
}

/**
 * An abort indicates that the current memory access cannot be completed,
 * which occurs during an instruction prefetch.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */

void awos_arch_iabt_trap_entry(excep_regs_t *regs)
{
    volatile uint8_t  i = 1;
    uint32_t victim = regs->abt_lr - 4;
#define readmem(addr)     (*(const volatile uint32_t *)addr)
#define accessable(addr)  ((((addr & 0xf0000000) >> 24) == 0xc0) || \
                           (((addr & 0xf0000000) >> 24) == 0xe0))

    /*makesure in code range and check whether it was the 'bkpt' instructions.*/
    if (accessable(victim) && ((readmem(victim) & 0xfff00070) == 0xe1200070))
    {
        static uint32_t loop = 0;

        /* A proper intrval for print */
        if ((loop ++ & 0x007fffff) == 0)
        {
            printk("softbreak, connect the debugger.sp: 0x%p, lr: 0x%08x, abt_lr: 0x%08x...\n", \
                   regs, regs->lr, regs->abt_lr);
        }

        regs->abt_lr = victim;

        /*resume to connect the DS5*/
        return;
    }

    printk("================================================================\n");
    printk("                      memory access abort(IF)                   \n");
    printk("================================================================\n");

    show_register(regs, victim, IABT_TYPE);
    regs->abt_lr = victim;

    while (i);
}


/**
 * An abort indicates that the current memory access cannot be completed,
 * which occurs during a data access.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void awos_arch_dabt_trap_entry(excep_regs_t *regs)
{
    volatile uint8_t  i = 1;
    uint32_t victim = regs->abt_lr - 8;

    printk("================================================================\n");
    printk("                      memory access abort(MA)                   \n");
    printk("================================================================\n");

    show_register(regs, victim, DABT_TYPE);
    regs->abt_lr = victim;

    while (i);
}

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_irq_trap_entry, IRQ routine. */
/* ----------------------------------------------------------------------------*/
void awos_arch_irq_trap_entry(void)
{
    __s32 irqno;

    irqno = INT_GetCurIrq();
    INT_SwitchMask(irqno);
    INT_EnterIsr(irqno);
    INT_RestoreMask();

    return;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_swc_func_error, wrong syscall report.
 *
 * @param[in] spsr
 * @param[in] cpsr
 * @param[in] swino
 * @param[in] swihander
 * @param[in] caller_lr
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_swc_func_error(uint32_t spsr, uint32_t cpsr, uint32_t swino, uint32_t swihander, uint32_t caller_lr)
{
    volatile uint8_t  i = 1;

    printk("syscall error: invalid syscall handler.\n");
    printk("================================================================\n");
    printk("                      syscall error                             \n");
    printk("================================================================\n");

    printk("spsr      :0x%08x\n" \
           "cpsr      :0x%08x\n" \
           "swino     :0x%08x\n" \
           "swihandler:0x%08x\n" \
           "caller_lr :0x%08x\n", spsr, cpsr, swino, swihander, caller_lr);

    while (i);
}


/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_bad_stack_error <irq stack corrupted happened, stack balan-
 *         ce destroyed.>
 *
 * @param[in] act_sp, actually stack pointer.
 * @param[in] exp_sp, expected stack pointer.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_bad_stack_error(uint32_t act_sp, uint32_t exp_sp)
{
    volatile uint8_t  i = 1;

    printk("irq stack corrupted: invalid stack pointer.\n");
    printk("================================================================\n");
    printk("                      irq error                                 \n");
    printk("================================================================\n");

    printk("act_sp      :0x%08x\n" \
           "exp_sp      :0x%08x\n",  act_sp, exp_sp);

    while (i);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_swc_mode_error <wrong cpu mode happed>
 *
 * @param spsr
 * @param cpsr
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_swc_mode_error(uint32_t spsr, uint32_t cpsr)
{
    volatile uint8_t  i = 1;

    printk("syscall error: invalid caller cpu mode.\n");
    printk("================================================================\n");
    printk("                        syscall error                           \n");
    printk("================================================================\n");

    printk("spsr:0x%08x\ncpsr:0x%08x\n", spsr, cpsr);

    while (i);
}

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_fiq_trap_entry <FIQ not support.> */
/* ----------------------------------------------------------------------------*/
void awos_arch_fiq_trap_entry(void)
{
    volatile uint8_t  i = 1;

    printk("fiq trap: fast interrupt did not support!\n");

    while (i);
}

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_rst_trap_entry <reset mode> */
/* ----------------------------------------------------------------------------*/
void awos_arch_rst_trap_entry(void)
{
    volatile uint8_t  i = 1;

    printk("reset trap: should never happen, brom stage.\n");

    while (i);
}

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_rsv_trap_entry <reserved exception, should never happend> */
/* ----------------------------------------------------------------------------*/
void awos_arch_rsv_trap_entry(void)
{
    volatile uint8_t  i = 1;

    printk("reserve trap: should never happen.\n");

    while (i);
}
