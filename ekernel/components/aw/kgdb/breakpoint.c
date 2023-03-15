#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gdb_stub.h"
#include "breakpoint.h"

extern struct gdb_arch arch_gdb_ops;

static struct gdb_bkpt gdb_sw_break[GDB_MAX_SW_BREAKPOINTS] =
{
    [0 ... GDB_MAX_SW_BREAKPOINTS - 1] = { .state = BP_UNDEFINED }
};

static int gdb_hw_break_max;
static int gdb_hw_watch_max;

static struct gdb_bkpt *gdb_hw_break = NULL;
static struct gdb_bkpt *gdb_hw_watch = NULL;

struct bp_type_state_match
{
    int type;
    char *str;
};

static struct bp_type_state_match bp_type_match [] =
{
    {BP_BREAKPOINT, "breakpoint"},
    {BP_HARDWARE_BREAKPOINT, "hardware breakpoint"},
    {BP_WRITE_WATCHPOINT, "write watchpoint"},
    {BP_READ_WATCHPOINT, "read watchpoint"},
    {BP_ACCESS_WATCHPOINT, "access watchpoint"},
    {BP_POKE_BREAKPOINT, "poke breakpoint"},
};

static struct bp_type_state_match bp_state_match[] =
{
    {BP_UNDEFINED, "undefined"},
    {BP_REMOVED, "removed"},
    {BP_SET, "set"},
    {BP_ACTIVE, "active"},
};

static void gdb_flush_swbreak_addr(unsigned long addr)
{
    gdb_flush_icache_range(addr, addr + BREAK_INSTR_SIZE);
}

static int gdb_arch_set_sw_breakpoint(unsigned long addr, int ins_len, char *saved_instr)
{
    if (arch_gdb_ops.set_sw_breakpoint)
    {
        return arch_gdb_ops.set_sw_breakpoint(addr, ins_len, saved_instr);
    }
    return -1;
}

static int gdb_arch_remove_sw_breakpoint(unsigned long addr, char *bundle)
{
    if (arch_gdb_ops.remove_sw_breakpoint)
    {
        return arch_gdb_ops.remove_sw_breakpoint(addr, bundle);
    }
    return -1;
}

int create_hw_break_watch(unsigned int hw_break, unsigned int hw_watch)
{
    int ret = -1;
    int i;

    if (hw_break)
    {
        gdb_hw_break = calloc(hw_break, sizeof(struct gdb_bkpt));
        if (gdb_hw_break)
        {
            gdb_hw_break_max = hw_break;
            struct gdb_bkpt *bkpt = gdb_hw_break;
            for (i = 0; i < hw_break; i++)
            {
                bkpt->state = BP_UNDEFINED;
                bkpt++;
            }
        }
        else
        {
            return -1;
        }
    }

    if (hw_watch)
    {
        gdb_hw_watch = calloc(hw_break, sizeof(struct gdb_bkpt));
        if (gdb_hw_watch)
        {
            gdb_hw_watch_max = hw_watch;
            struct gdb_bkpt *bkpt = gdb_hw_watch;
            for (i = 0; i < hw_break; i++)
            {
                bkpt->state = BP_UNDEFINED;
                bkpt++;
            }
        }
        else
        {
            free(gdb_hw_break);
            gdb_hw_break = NULL;
            return -1;
        }
    }
    return 0;
}

int destory_hw_break_watch(void)
{
    if (gdb_hw_break)
    {
        free(gdb_hw_break);
        gdb_hw_break = NULL;
    }

    if (gdb_hw_watch)
    {
        free(gdb_hw_watch);
        gdb_hw_watch = NULL;
    }
    return 0;
}

/*
 * SW breakpoint management:
 */
static int gdb_validate_break_address(unsigned long addr)
{
    char tmp_variable[BREAK_INSTR_SIZE];
    int err;
    /* Validate setting the breakpoint and then removing it.  In the
     * remove fails, the kernel needs to emit a bad message because we
     * are deep trouble not being able to put things back the way we
     * found them.
     */
    err = gdb_arch_set_sw_breakpoint(addr, 0, tmp_variable);
    if (err)
    {
        return err;
    }
    err = gdb_arch_remove_sw_breakpoint(addr, tmp_variable);
    if (err)
    {
        printf("GDB: Critical breakpoint error,memory destroyed at: %08lx \n", addr);
    }
    return err;
}

int gdb_set_sw_break(unsigned long addr, int length)
{
    int i;
    int err = -1;
    int breakno = -1;

    err = gdb_validate_break_address(addr);
    if (err)
    {
        return err;
    }

    for (i = 0; i < GDB_MAX_SW_BREAKPOINTS; i++)
    {
        if ((gdb_sw_break[i].state == BP_SET) &&
            (gdb_sw_break[i].bpt_addr == addr))
        {
            return 0;
        }
    }
    for (i = 0; i < GDB_MAX_SW_BREAKPOINTS; i++)
    {
        if (gdb_sw_break[i].state == BP_REMOVED)
        {
            breakno = i;
            break;
        }
    }

    if (breakno == -1)
    {
        for (i = 0; i < GDB_MAX_SW_BREAKPOINTS; i++)
        {
            if (gdb_sw_break[i].state == BP_UNDEFINED)
            {
                breakno = i;
                break;
            }
        }
    }

    if (breakno == -1)
    {
        return -1;
    }

    gdb_sw_break[breakno].state = BP_SET;
    gdb_sw_break[breakno].type = BP_BREAKPOINT;
    gdb_sw_break[breakno].bpt_addr = addr;
    gdb_sw_break[breakno].bpt_len = length;

    return 0;
}


int gdb_remove_sw_break(unsigned long addr, int length)
{
    int i;

    for (i = 0; i < GDB_MAX_SW_BREAKPOINTS; i++)
    {
        if ((gdb_sw_break[i].state == BP_SET) &&
            (gdb_sw_break[i].bpt_addr == addr))
        {
            gdb_sw_break[i].state = BP_REMOVED;
            return 0;
        }
    }
    return -1;
}

int gdb_isremoved_sw_break(unsigned long addr)
{
    int i;

    for (i = 0; i < GDB_MAX_SW_BREAKPOINTS; i++)
    {
        if ((gdb_sw_break[i].state == BP_REMOVED) &&
            (gdb_sw_break[i].bpt_addr == addr))
        {
            return 1;
        }
    }
    return 0;
}

/*
 * HW breakpoint management:
 */
int gdb_set_hw_break(unsigned long addr)
{
    int i;
    int breakno = -1;

    for (i = 0; i < gdb_hw_break_max; i++)
    {
        if ((gdb_hw_break[i].state == BP_SET) &&
            (gdb_hw_break[i].bpt_addr == addr))
        {
            return 0;
        }
    }
    for (i = 0; i < gdb_hw_break_max; i++)
    {
        if (gdb_hw_break[i].state == BP_REMOVED)
        {
            breakno = i;
            break;
        }
    }

    if (breakno == -1)
    {
        for (i = 0; i < gdb_hw_break_max; i++)
        {
            if (gdb_hw_break[i].state == BP_UNDEFINED)
            {
                breakno = i;
                break;
            }
        }
    }

    if (breakno == -1)
    {
        return -1;
    }

    gdb_hw_break[breakno].state = BP_SET;
    gdb_hw_break[breakno].type = BP_HARDWARE_BREAKPOINT;
    gdb_hw_break[breakno].bpt_addr = addr;

    return 0;
}

int gdb_remove_hw_break(unsigned long addr)
{
    int i;

    for (i = 0; i < gdb_hw_break_max; i++)
    {
        if ((gdb_hw_break[i].state == BP_SET) &&
            (gdb_hw_break[i].bpt_addr == addr))
        {
            gdb_hw_break[i].state = BP_REMOVED;
            return 0;
        }
    }
    return -1;
}

int gdb_isremoved_hw_break(unsigned long addr)
{
    int i;

    for (i = 0; i < gdb_hw_break_max; i++)
    {
        if ((gdb_hw_break[i].state == BP_REMOVED) &&
            (gdb_hw_break[i].bpt_addr == addr))
        {
            return 1;
        }
    }
    return 0;
}

/*
 * HW watchpoint management:
 */
int gdb_set_hw_watch(unsigned long addr, enum gdb_bptype type)
{
    int i;
    int breakno = -1;

    for (i = 0; i < gdb_hw_watch_max; i++)
    {
        if ((gdb_hw_watch[i].state == BP_SET) &&
            (gdb_hw_watch[i].bpt_addr == addr))
        {
            return 0;
        }
    }
    for (i = 0; i < gdb_hw_watch_max; i++)
    {
        if (gdb_hw_watch[i].state == BP_REMOVED)
        {
            breakno = i;
            break;
        }
    }

    if (breakno == -1)
    {
        for (i = 0; i < gdb_hw_watch_max; i++)
        {
            if (gdb_hw_watch[i].state == BP_UNDEFINED)
            {
                breakno = i;
                break;
            }
        }
    }

    if (breakno == -1)
    {
        return -1;
    }

    gdb_hw_watch[breakno].state = BP_SET;
    gdb_hw_watch[breakno].type = type;
    gdb_hw_watch[breakno].bpt_addr = addr;

    return 0;
}

int gdb_remove_hw_watch(unsigned long addr)
{
    int i;

    for (i = 0; i < gdb_hw_watch_max; i++)
    {
        if ((gdb_hw_watch[i].state == BP_SET) &&
            (gdb_hw_break[i].bpt_addr == addr))
        {
            gdb_hw_watch[i].state = BP_REMOVED;
            return 0;
        }
    }
    return -1;
}

int gdb_isremoved_hw_watch(unsigned long addr)
{
    int i;

    for (i = 0; i < gdb_hw_watch_max; i++)
    {
        if ((gdb_hw_watch[i].state == BP_REMOVED) &&
            (gdb_hw_watch[i].bpt_addr == addr))
        {
            return 1;
        }
    }
    return 0;
}


static int gdb_arch_set_hw_breakpoint(unsigned int no, unsigned long addr)
{
    if (arch_gdb_ops.set_hw_breakpoint)
    {
        return arch_gdb_ops.set_hw_breakpoint(no, addr);
    }
    return -1;
}

static int gdb_arch_remove_hw_breakpoint(unsigned int no, unsigned long addr)
{
    if (arch_gdb_ops.remove_hw_breakpoint)
    {
        return arch_gdb_ops.remove_hw_breakpoint(no, addr);
    }
    return -1;
}

static int gdb_arch_set_hw_watchpoint(enum gdb_bptype type, unsigned int no, unsigned long addr)
{
    if (arch_gdb_ops.set_hw_watchpoint)
    {
        return arch_gdb_ops.set_hw_watchpoint(type, no, addr);
    }
    return -1;
}

static int gdb_arch_remove_hw_watchpoint(enum gdb_bptype type, unsigned int no, unsigned long addr)
{
    if (arch_gdb_ops.set_hw_watchpoint)
    {
        return arch_gdb_ops.set_hw_watchpoint(type, no, addr);
    }
    return -1;
}

static int gdb_deactivate_sw_breakpoints(void)
{
    unsigned long addr;
    int error = 0;
    int i;

    for (i = 0; i < GDB_MAX_SW_BREAKPOINTS; i++)
    {
        if (gdb_sw_break[i].state != BP_ACTIVE)
        {
            continue;
        }
        addr = gdb_sw_break[i].bpt_addr;
        error = gdb_arch_remove_sw_breakpoint(addr,
                                              (char *)(gdb_sw_break[i].saved_instr));
        if (error)
        {
            return error;
        }

        gdb_flush_swbreak_addr(addr);
        gdb_sw_break[i].state = BP_SET;
    }
    return 0;
}

static int gdb_deactivate_hw_breakpoints(void)
{
    unsigned long addr;
    int error = 0;
    int i;

    for (i = 0; i < gdb_hw_break_max; i++)
    {
        if (gdb_hw_break[i].state != BP_ACTIVE)
        {
            continue;
        }
        addr = gdb_hw_break[i].bpt_addr;
        error = gdb_arch_remove_hw_breakpoint(i, addr);
        if (error)
        {
            return error;
        }

        gdb_hw_break[i].state = BP_SET;
    }
    return 0;
}

static int gdb_deactivate_hw_watchpoints(void)
{
    unsigned long addr;
    int error = 0;
    int i;

    for (i = 0; i < gdb_hw_watch_max; i++)
    {
        if (gdb_hw_watch[i].state != BP_ACTIVE)
        {
            continue;
        }
        addr = gdb_hw_watch[i].bpt_addr;
        error = gdb_arch_remove_hw_watchpoint(gdb_hw_watch[i].type, i, addr);
        if (error)
        {
            return error;
        }

        gdb_hw_watch[i].state = BP_SET;
    }
    return 0;
}

static int gdb_activate_sw_breakpoints(void)
{
    unsigned long addr;
    int error = 0;
    int i;
    int ins_len;

    for (i = 0; i < GDB_MAX_SW_BREAKPOINTS; i++)
    {
        if (gdb_sw_break[i].state != BP_SET)
        {
            continue;
        }

        addr = gdb_sw_break[i].bpt_addr;
        ins_len = gdb_sw_break[i].bpt_len;
        error = gdb_arch_set_sw_breakpoint(addr,
                                           ins_len,
                                           (char *)(gdb_sw_break[i].saved_instr));
        if (error)
        {
            return error;
        }

        gdb_flush_swbreak_addr(addr);
        gdb_sw_break[i].state = BP_ACTIVE;
    }
    return 0;
}

static int gdb_activate_hw_breakpoints(void)
{
    int i;
    unsigned long addr;
    int error = 0;

    for (i = 0; i < gdb_hw_break_max; i++)
    {
        if (gdb_hw_break[i].state != BP_SET)
        {
            continue;
        }

        addr = gdb_hw_break[i].bpt_addr;
        error = gdb_arch_set_hw_breakpoint(i, addr);
        if (error)
        {
            return error;
        }

        gdb_hw_break[i].state = BP_ACTIVE;
    }
    return 0;
}

static int gdb_activate_hw_watchpoints(void)
{
    unsigned long addr;
    int error = 0;
    int i;

    for (i = 0; i < gdb_hw_watch_max; i++)
    {
        if (gdb_hw_watch[i].state != BP_SET)
        {
            continue;
        }

        addr = gdb_hw_watch[i].bpt_addr;
        error = gdb_arch_set_hw_watchpoint(gdb_hw_watch[i].type, i, addr);
        if (error)
        {
            return error;
        }

        gdb_hw_watch[i].state = BP_ACTIVE;
    }
    return 0;
}

int gdb_deactivate_all_break_watch_points(void)
{
    gdb_deactivate_sw_breakpoints();
    gdb_deactivate_hw_breakpoints();
    gdb_deactivate_hw_watchpoints();

    return 0;
}

int gdb_activate_all_break_watch_points(void)
{
    gdb_activate_sw_breakpoints();
    gdb_activate_hw_breakpoints();
    gdb_activate_hw_watchpoints();

    return 0;
}

int remove_all_break_watch_points(void)
{
    unsigned long addr;
    int error = 0;
    int i;

    for (i = 0; i < GDB_MAX_SW_BREAKPOINTS; i++)
    {
        if (gdb_sw_break[i].state != BP_ACTIVE)
        {
            goto sw_break_setundefined;
        }
        addr = gdb_sw_break[i].bpt_addr;
        error = gdb_arch_remove_sw_breakpoint(addr,
                                              (char *)gdb_sw_break[i].saved_instr);
        if (error)
        {
            printf("GDB: breakpoint remove failed: %lx\n", addr);
        }
sw_break_setundefined:
        gdb_sw_break[i].state = BP_UNDEFINED;
    }

    for (i = 0; i < gdb_hw_break_max; i++)
    {
        if (gdb_hw_break[i].state != BP_ACTIVE)
        {
            goto hw_break_setundefined;
        }
        addr = gdb_hw_break[i].bpt_addr;
        error = gdb_arch_remove_hw_breakpoint(i, addr);
        if (error)
        {
            printf("GDB: breakpoint remove failed: %lx\n", addr);
        }
hw_break_setundefined:
        gdb_hw_break[i].state = BP_UNDEFINED;
    }

    for (i = 0; i < gdb_hw_watch_max; i++)
    {
        if (gdb_hw_watch[i].state != BP_ACTIVE)
        {
            goto hw_watch_setundefined;
        }
        addr = gdb_hw_watch[i].bpt_addr;
        error = gdb_arch_remove_hw_watchpoint(gdb_hw_watch[i].type, i, addr);
        if (error)
        {
            printf("GDB: breakpoint remove failed: %lx\n", addr);
        }
hw_watch_setundefined:
        gdb_hw_watch[i].state = BP_UNDEFINED;
    }

    return 0;
}


static char *get_bp_type_str(enum gdb_bptype type)
{
    if (type < sizeof(bp_type_match) / sizeof(bp_type_match[0]))
    {
        return bp_type_match[type].str;
    }
    return NULL;
}

static char *get_bp_state_str(enum gdb_bpstate state)
{
    if (state < sizeof(bp_state_match) / sizeof(bp_state_match[0]))
    {
        return bp_state_match[state].str;
    }
    return NULL;
}

void debug_dump_all_breaks_info(void)
{
    int i;

    int debug_breakpoint_init(void);
    if (debug_breakpoint_init() != 0)
    {
        printf("watchpoint debug not init!\n");
        return;
    }

    printf("watchpoint num = %d:\n", gdb_hw_watch_max);
    printf("Id    Addr    State      Type\n");

    for (i = 0; i < gdb_hw_watch_max; i++)
    {
        printf("%d  0x%08lx  %s      %s\n", i,
               gdb_hw_watch[i].bpt_addr,
               get_bp_state_str(gdb_hw_watch[i].state),
               get_bp_type_str(gdb_hw_watch[i].type));
    }

    printf("\nbreakpoint num = %d:\n", gdb_hw_break_max);
    printf("Id    Addr    State      Type\n");

    for (i = 0; i < gdb_hw_break_max; i++)
    {
        printf("%d  0x%08lx  %s      %s\n", i,
               gdb_hw_break[i].bpt_addr,
               get_bp_state_str(gdb_hw_break[i].state),
               get_bp_type_str(gdb_hw_break[i].type));
    }
}
