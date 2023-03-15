#include <stdlib.h>
#include <stdio.h>

#include <gdb_stub.h>
#include <arch_gdb.h>
#include <excep.h>
#include <arch.h>
#include <breakpoint.h>
#include "hardware_wbp.h"

extern int compiled_break;

static int kgdb_brk_fn(excep_regs_t *regs)
{
    gdb_handle_exception(SIGTRAP, regs);
    return 0;
}

static int kgdb_compiled_brk_fn(excep_regs_t *regs)
{
    compiled_break = 1;
    gdb_handle_exception(SIGTRAP, regs);
    return 0;
}

static struct undef_hook kgdb_brkpt_hook =
{
    .instr_mask     = 0xffffffff,
    .instr_val      = GDB_BREAKINST,
    .cpsr_mask      = MODE_MASK,
    .cpsr_val       = SVC_MODE,
    .fn         = kgdb_brk_fn
};

static struct undef_hook kgdb_compiled_brkpt_hook =
{
    .instr_mask     = 0xffffffff,
    .instr_val      = GDB_COMPILED_BREAK,
    .cpsr_mask      = MODE_MASK,
    .cpsr_val       = SVC_MODE,
    .fn         = kgdb_compiled_brk_fn
};

static struct undef_hook kgdb_thumb_brkpt_hook =
{
    .instr_mask     = 0x0000ffff,
    .instr_val      = GDB_THUMB_BREAKINST,
    .cpsr_mask      = MODE_MASK,
    .cpsr_val       = SVC_MODE,
    .fn         = kgdb_brk_fn
};

static int kgdb_hw_watchpoint_fn(excep_regs_t *regs)
{
    gdb_handle_exception(SIGTRAP, regs);
    return 0;
}

static int kgdb_hw_breakpoint_fn(excep_regs_t *regs)
{
    gdb_handle_exception(SIGTRAP, regs);
    return 0;
}

static struct data_abort_hook kgdb_hw_watch_hook =
{
    .dfsr_mask      = 0x1f,
    .dfsr_val       = 0x2,
    .cpsr_mask      = MODE_MASK,
    .cpsr_val       = SVC_MODE,
    .fn         = kgdb_hw_watchpoint_fn
};

static struct instr_prefetch_abort_hook kgdb_hw_break_hook =
{
    .ifsr_mask      = 0x1f,
    .ifsr_val       = 0x2,
    .cpsr_mask      = MODE_MASK,
    .cpsr_val       = SVC_MODE,
    .fn         = kgdb_hw_breakpoint_fn
};

int gdb_instr_prefetch_abort_hook(void *regs)
{
    struct gdb_regs_t *tmp_reg = (struct gdb_regs_t *)regs;
    unsigned long abt_status = (unsigned long)tmp_reg->regs_t.abt_iFSR;

    if ((abt_status & 0x1F) == 2)
    {
        gdb_handle_exception(SIGTRAP, regs);
        return 1;
    }

    return 0;
}

int gdb_data_abort_hook(void *regs)
{
    struct gdb_regs_t *tmp_reg = (struct gdb_regs_t *)regs;
    unsigned long abt_addr = (unsigned long)tmp_reg->regs_t.abt_dFAR;
    unsigned long abt_status = (unsigned long)tmp_reg->regs_t.abt_dFSR;

    if ((abt_status & 0x1F) == 2)
    {
        gdb_handle_exception(SIGTRAP, regs);
        return 1;
    }

    return 0;
}

static int debug_breakpoint_init_flag = 0;
static int kgdb_arch_init_flag = 0;

int arm_kgdb_arch_init(void)
{
    int ret = -1;
    unsigned int brp = 0;
    unsigned int wrp = 0;

    if (debug_breakpoint_init_flag == 1)
    {
        printf("you has set breakpoint/watchpoint, so can not enter kgdb debug mode\n");
        return -1;
    }

    if (kgdb_arch_init_flag > 0)
    {
        return 0;
    }

    if (!monitor_mode_enabled())
    {
        ret = enable_monitor_mode();
        if (ret)
        {
            printf("enter monitor mode failed!\n");
            return -1;
        }
    }

    brp = get_num_brp_resources();
    wrp = get_num_wrp_resources();

    if (create_hw_break_watch(brp, wrp))
    {
        return -1;
    }

    register_undef_hook(&kgdb_brkpt_hook);
    register_undef_hook(&kgdb_thumb_brkpt_hook);
    register_undef_hook(&kgdb_compiled_brkpt_hook);

    register_data_abort_hook(&kgdb_hw_watch_hook);
    register_instr_prefetch_abort_hook(&kgdb_hw_break_hook);

    kgdb_arch_init_flag = 1;
    return ret;
}

int debug_breakpoint_init(void)
{
    int ret = -1;
    unsigned int brp = 0;
    unsigned int wrp = 0;

    if (kgdb_arch_init_flag == 1)
    {
        printf("you has enter kgdb debug mode, so can not set breakpoint/watchpoint\n");
        return -1;
    }

    if (debug_breakpoint_init_flag > 0)
    {
        return 0;
    }

    if (!monitor_mode_enabled())
    {
        ret = enable_monitor_mode();
        if (ret)
        {
            printf("enter monitor mode failed!\n");
            return -1;
        }
    }

    brp = get_num_brp_resources();
    wrp = get_num_wrp_resources();

    if (create_hw_break_watch(brp, wrp))
    {
        return -1;
    }

    debug_breakpoint_init_flag = 1;
    return ret;
}

void arm_kgdb_arch_exit(void)
{
    unregister_undef_hook(&kgdb_brkpt_hook);
    unregister_undef_hook(&kgdb_compiled_brkpt_hook);
    unregister_data_abort_hook(&kgdb_hw_watch_hook);
    unregister_instr_prefetch_abort_hook(&kgdb_hw_break_hook);

    destory_hw_break_watch();
}
