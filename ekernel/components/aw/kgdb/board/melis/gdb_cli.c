#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "breakpoint.h"
#include <kgdb.h>
#include <gdb_stub.h>

#include <rtthread.h>

//extern int kgdb_arch_init(void);
extern int debug_breakpoint_init(void);

static int cmd_kgdb(int argc, const char *argv[])
{
    int ret = -1;
    ret = kgdb_arch_init();
    if (ret)
    {
        printf("init kgdb failed!\n");
        return -1;
    }
    gdb_start();
    return 0;
}

static void show_watchpoint_help(void)
{
    debug_dump_all_breaks_info();
    printf("\nUsage: watchpoint [write | read | access | remove] addr\n");
}

static int cmd_watchpoint(int argc, const char *argv[])
{
    int ret = -1;
    char *err = NULL;
    unsigned int addr = 0;

    if (argc < 3)
    {
        show_watchpoint_help();
        return -1;
    }

    ret = debug_breakpoint_init();
    if (ret)
    {
        printf("init kgdb failed!\n");
        return -1;
    }

    addr = strtoul(argv[2], &err, 0);

    gdb_deactivate_all_break_watch_points();

    if (!strcmp(argv[1], "write"))
    {
        if (gdb_set_hw_watch(addr, BP_WRITE_WATCHPOINT))
        {
            printf("set write watchpoint failed!");
        }
    }
    else if (!strcmp(argv[1], "read"))
    {
        if (gdb_set_hw_watch(addr, BP_READ_WATCHPOINT))
        {
            printf("set read watchpoint failed!");
        }
    }
    else if (!strcmp(argv[1], "access"))
    {
        if (gdb_set_hw_watch(addr, BP_ACCESS_WATCHPOINT))
        {
            printf("set access watchpoint failed!");
        }
    }
    else if (!strcmp(argv[1], "remove"))
    {
        if (gdb_remove_hw_watch(addr))
        {
            printf("remove watchpoint failed!");
        }
    }
    else
    {
        show_watchpoint_help();
    }

    gdb_activate_all_break_watch_points();
    return 0;
}

static void show_breakpoint_help(void)
{
    debug_dump_all_breaks_info();
    printf("\nUsage: breakpoint [set | remove] addr\n");
}

static int cmd_breakpoint(int argc, const char *argv[])
{
    unsigned long addr = 0;
    char *err = NULL;
    int ret = 0;

    if (argc < 3)
    {
        show_breakpoint_help();
        return -1;
    }

    ret = debug_breakpoint_init();
    if (ret)
    {
        printf("init kgdb failed!\n");
        return -1;
    }

    addr = strtoul(argv[2], &err, 0);
    if (*err != 0)
    {
        printf("addr error\n");
        return -1;
    }

    gdb_deactivate_all_break_watch_points();

    if (!strcmp(argv[1], "set"))
    {
        if (gdb_set_hw_break(addr))
        {
            printf("set breakpoint failed!");
        }
    }
    else if (!strcmp(argv[1], "remove"))
    {
        if (gdb_remove_hw_break(addr))
        {
            printf("remove breakpoint failed!");
        }
    }
    else
    {
        show_breakpoint_help();
    }
    gdb_activate_all_break_watch_points();
    return -1;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_kgdb, __cmd_kgdb, KGDB debug mode);
FINSH_FUNCTION_EXPORT_ALIAS(cmd_watchpoint, __cmd_watchpoint, Set watchpoint);
FINSH_FUNCTION_EXPORT_ALIAS(cmd_breakpoint, __cmd_breakpoint, Set breakpoint);
