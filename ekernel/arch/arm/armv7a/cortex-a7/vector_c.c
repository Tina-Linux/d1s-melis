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
#include <stdio.h>

#include <excep.h>
#include <log.h>
#include <aw_list.h>
#include <ktimer.h>

#include <hal_debug.h>
#include <hal_thread.h>

#ifdef CONFIG_DEBUG_BACKTRACE
#include <backtrace.h>
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

void dump_system_information(void);
void dump_memory(unsigned long* , int);
void panic_goto_cli(void);
void show_thread_info(void *);

#ifdef CONFIG_DEBUG_PANIC_CAUSE
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
#endif

#if defined CONFIG_SUBSYS_KGDB

static LIST_HEAD(undef_hook);
static LIST_HEAD(data_abort_hook);
static LIST_HEAD(instr_prefetch_abort_hook);

void register_undef_hook(struct undef_hook *hook)
{
    unsigned long flags;

    list_add(&hook->node, &undef_hook);
}

void unregister_undef_hook(struct undef_hook *hook)
{
    unsigned long flags;

    list_del(&hook->node);
}

static int call_undef_hook(excep_regs_t *regs)
{
    struct undef_hook *hook;
    unsigned long flags;
    int (*fn)(excep_regs_t *regs) = NULL;
    unsigned long instr;

    if (regs->orig_cpsr & 0x20)
    {
        instr = (*(unsigned short *)regs->abt_lr) & 0xffff;
    }
    else
    {
        instr = *(unsigned long *)regs->abt_lr;
    }

    list_for_each_entry(hook, &undef_hook, node)
    {
        if ((instr & hook->instr_mask) == hook->instr_val)
        {
            fn = hook->fn;
            break;
        }
    }

    return fn ? fn(regs) : 1;
}

void register_data_abort_hook(struct data_abort_hook *hook)
{
    unsigned long flags;

    list_add(&hook->node, &data_abort_hook);
}

void unregister_data_abort_hook(struct data_abort_hook *hook)
{
    unsigned long flags;

    list_del(&hook->node);
}

static int call_data_abort_hook(excep_regs_t *regs)
{
    struct data_abort_hook *hook;
    unsigned long flags;
    int (*fn)(excep_regs_t *regs) = NULL;
    unsigned long abt_dfsr = (unsigned long)regs->abt_dFSR;

    list_for_each_entry(hook, &data_abort_hook, node)
    {
        if ((abt_dfsr & hook->dfsr_mask) == hook->dfsr_val)
        {
            fn = hook->fn;
            break;
        }
    }

    return fn ? fn(regs) : 1;
}

void register_instr_prefetch_abort_hook(struct instr_prefetch_abort_hook *hook)
{
    unsigned long flags;

    list_add(&hook->node, &instr_prefetch_abort_hook);
}

void unregister_instr_prefetch_abort_hook(struct instr_prefetch_abort_hook *hook)
{
    unsigned long flags;

    list_del(&hook->node);
}

static int call_instr_prefetch_abort_hook(excep_regs_t *regs)
{
    struct instr_prefetch_abort_hook *hook;
    unsigned long flags;
    int (*fn)(excep_regs_t *regs) = NULL;
    unsigned long abt_ifsr = (unsigned long)regs->abt_iFSR;

    list_for_each_entry(hook, &instr_prefetch_abort_hook, node)
    {
        if ((abt_ifsr & hook->ifsr_mask) == hook->ifsr_val)
        {
            fn = hook->fn;
            break;
        }
    }

    return fn ? fn(regs) : 1;
}

static int thumb_32bit_code(uint16_t ic)
{
    uint16_t op = (ic >> 11) & 0xFFFF;
    if (op == 0x1D || op == 0x1E || op == 0x1F)
    {
        return 1;
    }
    return 0;
}
#endif

static void show_register(excep_regs_t *regs, uint32_t victim, int32_t type)
{
    show_thread_info(kthread_self());

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

#ifdef CONFIG_DEBUG_PANIC_CAUSE
    printk("\ncause:\n");
    if (type == DABT_TYPE)
    {
        printk("DFSR signs %s.\n",  fsr_info[fsr_fs(regs->abt_dFSR)]);
        void show_pte(unsigned long addr);
        show_pte(regs->abt_dFAR);
    }
    else if (type == IABT_TYPE)
    {
        printk("IFSR signs %s.\n",  fsr_info[fsr_fs(regs->abt_iFSR)]);
    }
    else
    {
        printk("undefind instructions.\n\r");
    }
#endif

    printk("\r\n");

#ifdef CONFIG_DEBUG_BACKTRACE
    backtrace_exception((print_function)printk, regs->orig_cpsr, regs->sp, regs->abt_lr, regs->lr);
#endif

    dump_system_information();

    void dump_register_memory(char *name, unsigned long addr, int32_t len);
    dump_register_memory("stack",regs->sp, 128);

    printk("\r\n");
    printk("================================================================\n");

    panic_goto_cli();
}

void awos_arch_und_trap_entry(excep_regs_t *regs)
{
    volatile uint8_t  i = 1;
    uint32_t victim = regs->abt_lr - 0;

#if defined CONFIG_SUBSYS_KGDB
    if (regs->orig_cpsr & 0x20)
    {
        uint16_t nextins;
        regs->abt_lr -= 2;
        nextins = (*(unsigned short *)(victim - 4)) & 0xffff;
        if (thumb_32bit_code(nextins) == 1)
        {
            regs->abt_lr -= 2;
        }
    }
    else
#endif
    {
        regs->abt_lr -= 4;
    }
#if defined CONFIG_SUBSYS_KGDB
    if (!call_undef_hook(regs))
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

void awos_arch_iabt_trap_entry(excep_regs_t *regs)
{
    volatile uint8_t  i = 1;
    uint32_t victim = regs->abt_lr;
#define readmem(addr)     (*(const volatile uint32_t *)addr)
#define readmem_thumb(addr)     (*(const volatile uint16_t *)addr)
#define accessable(addr)  ((((addr & 0xf0000000) >> 24) == 0xc0) || \
                           (((addr & 0xf0000000) >> 24) == 0xe0))

    regs->abt_lr -= 4;
#if defined CONFIG_SUBSYS_KGDB
    if (!call_instr_prefetch_abort_hook(regs))
    {
        return;
    }
#endif

    //arm mode.
    if (!(regs->orig_cpsr & ARMV7A_THB_MASK))
    {
        /*makesure in code range and check whether it was the 'bkpt' instructions.*/
        if (accessable(victim) && ((readmem(victim) & 0xfff00070) == 0xe1200070))
        {
            static uint32_t loop = 0;

            /* A proper intrval for print */
            if ((loop ++ & 0x007fffff) == 0)
            {
                printk("arm:softbreak, connect the debugger.sp: 0x%p, lr: 0x%08x, abt_lr: 0x%08x...\n", \
                           regs, regs->lr, regs->abt_lr);
#ifdef CONFIG_DEBUG_BACKTRACE
                backtrace_exception((print_function)printk, regs->orig_cpsr, regs->sp, regs->abt_lr, regs->lr);
#endif
            }

            /*resume to connect the DS5*/
            return;
        }
    }
    else  //thumb mode.
    {
        /*makesure in code range and check whether it was the 'bkpt' instructions.*/
        if (accessable(victim) && ((readmem_thumb(victim) & 0xff00) == 0xbe00))
        {
            static uint32_t loop = 0;

            /* A proper intrval for print */
            if ((loop ++ & 0x007fffff) == 0)
            {
                printk("thumb:softbreak, connect the debugger.sp: 0x%p, lr: 0x%08x, abt_lr: 0x%08x...\n", \
                           regs, regs->lr, regs->abt_lr);
#ifdef CONFIG_DEBUG_BACKTRACE
                backtrace_exception((print_function)printk, regs->orig_cpsr, regs->sp, regs->abt_lr, regs->lr);
#endif
            }
            //force to thumb state, can be comment for cpsr already in the right state.
            regs->abt_lr |= 1;
            /*resume to connect the DS5*/
            return;
        }
    }
    printk("================================================================\n");
    printk("                      memory access abort(IF)                   \n");
    printk("================================================================\n");

    show_register(regs, victim, IABT_TYPE);
    regs->abt_lr = victim;

    while (i);
}

void awos_arch_dabt_trap_entry(excep_regs_t *regs)
{
    volatile uint8_t  i = 1;
    uint32_t victim = regs->abt_lr - 8;

    regs->abt_lr -= 8;
#if defined CONFIG_SUBSYS_KGDB
    if (!call_data_abort_hook(regs))
    {
        return;
    }
#endif

    printk("================================================================\n");
    printk("                      memory access abort(MA)                   \n");
    printk("================================================================\n");

    show_register(regs, victim, DABT_TYPE);
    regs->abt_lr = victim;

    while (i);
}

void awos_arch_irq_trap_entry(irq_regs_t *regs)
{
    void gic_handle_irq(irq_regs_t *regs);
    if (!irqs_disabled())
    {
        __err("Do you wanna nest irq?");
        hal_sys_abort();
    }

    gic_handle_irq(regs);

    if (!irqs_disabled())
    {
        __err("Irq opend by interrupt handler.");
        hal_sys_abort();
    }
}

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

void awos_arch_irqstack_balance_error(void)
{
    volatile uint8_t  i = 1;

    printk("irq stack corrupted: invalid stack pointer.\n");
    printk("================================================================\n");
    printk("                      irq error                                 \n");
    printk("================================================================\n");

    while (i);
}

void awos_arch_bad_switch_error(rt_thread_t old, rt_thread_t new)
{
    volatile uint8_t  i = 1;

    printk("bad irq switch.\n");
    printk("================================================================\n");
    printk("                      bad irq switch                            \n");
    printk("================================================================\n");

    printk("old thread :%s\n" \
               "new thread :%s\n",  old->name, new->name);

    while (i);
}

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

void awos_arch_fiq_trap_entry(void)
{
    volatile uint8_t  i = 1;

    printk("fiq trap: fast interrupt did not support!\n");

    while (i);
}

void awos_arch_rst_trap_entry(void)
{
    volatile uint8_t  i = 1;

    printk("reset trap: should never happen, brom stage.\n");

    while (i);
}

void awos_arch_hyp_trap_entry(void)
{
    volatile uint8_t  i = 1;

    printk("reserve trap: should never happen.\n");

    while (i);
}
