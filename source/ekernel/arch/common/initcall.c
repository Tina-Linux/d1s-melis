/*
 * ===========================================================================================
 *
 *       Filename:  initcall.c
 *
 *    Description:  initcall routine
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-22 10:25:56
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-04-13 15:19:08
 *
 * ===========================================================================================
 */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <init.h>
#include <log.h>

#define VERBOSE_INITCALL  1

extern initcall_entry_t __initcall_start[];
extern initcall_entry_t __initcall0_start[];
extern initcall_entry_t __initcall1_start[];
extern initcall_entry_t __initcall2_start[];
extern initcall_entry_t __initcall3_start[];
extern initcall_entry_t __initcall4_start[];
extern initcall_entry_t __initcall5_start[];
extern initcall_entry_t __initcall6_start[];
extern initcall_entry_t __initcall7_start[];
extern initcall_entry_t __initcall_end[];

static initcall_entry_t *initcall_levels[] =
{
    __initcall0_start,
    __initcall1_start,
    __initcall2_start,
    __initcall3_start,
    __initcall4_start,
    __initcall5_start,
    __initcall6_start,
    __initcall7_start,
    __initcall_end,
};

/* Keep these in sync with initcalls in include/linux/init.h */
static const char *initcall_level_names[] =
{
    "pure",
    "core",
    "postcore",
    "arch",
    "subsys",
    "fs",
    "device",
    "late",
};

void do_one_initcall(initcall_t fn)
{
    if (fn)
    {
        fn();
    }
}

static void do_initcall_level(int level)
{
    initcall_entry_t *fn;
    initcall_t callfn;

    for (fn = initcall_levels[level]; fn < initcall_levels[level + 1]; fn++)
    {
        callfn = initcall_from_entry(fn);
#if(VERBOSE_INITCALL)
        __wrn("initcall: 0x%p.", callfn);
#endif
        do_one_initcall(callfn);
    }
}

void do_initcalls(void)
{
    int level;

    for (level = 0; level < (sizeof(initcall_levels) / sizeof(initcall_levels[0])) - 1; level++)
    {
        do_initcall_level(level);
    }
}
