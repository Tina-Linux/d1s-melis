/*
 * This provides the functions that GDB needs to share between
 * different portions.
 *
 * GDB stub.
 *
 * Migarte form linux to rt-thread by Wzyy2
 * Original edition : KGDB stub
 *
 * File      : gdb_stub.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2014-07-04     Wzyy2      first version
 * 2019-12-25     ZengZhijin refactor to support hw breakpoint/watchpoint
 */
#ifndef __GDB_STUB_H__
#define __GDB_STUB_H__

#include <arch_gdb.h>

#define SIGTRAP 5       /* trace trap (not reset when caught) */

enum gdb_bptype
{
    BP_BREAKPOINT = 0,
    BP_HARDWARE_BREAKPOINT,
    BP_WRITE_WATCHPOINT,
    BP_READ_WATCHPOINT,
    BP_ACCESS_WATCHPOINT,
    BP_POKE_BREAKPOINT,
};

enum gdb_bpstate
{
    BP_UNDEFINED = 0,
    BP_REMOVED,
    BP_SET,
    BP_ACTIVE
};

struct gdb_bkpt
{
    unsigned long       bpt_addr;
    unsigned char       saved_instr[BREAK_INSTR_SIZE];
    int                 bpt_len;
    enum gdb_bptype type;
    enum gdb_bpstate    state;
};

/**
 * struct gdb_arch - Describe architecture specific values.
 * @gdb_bpt_instr: The instruction to trigger a breakpoint.
 * @flags:
 * @set_sw_breakpoint: Set a software breakpoint
 * @remove_sw_breakpoint: Remove a software breakpoint
 * @set_hw_breakpoint: Set a hardware breakpoint.
 * @remove_hw_breakpoint: Remove a hardware breakpoint.
 * @set_hw_watchpoint: Set a hardware watchpoint.
 * @remove_hw_watchpoint: Remove a hardware watchpoint.
 */
struct gdb_arch
{
    unsigned char gdb_bpt_instr[BREAK_INSTR_SIZE];
    unsigned long flags;

    int (*kgdb_init)(void);
    int (*kgdb_exit)(void);
    void (*set_compiled_breakpoint)(void);
    int (*check_data_access)(unsigned long addr);

    int (*set_sw_breakpoint)(unsigned long addr, int ins_len, char *saved_instr);
    int (*remove_sw_breakpoint)(unsigned long addr, char *bundle);
    int (*set_hw_breakpoint)(int, unsigned long);
    int (*remove_hw_breakpoint)(int, unsigned long);
    int (*set_hw_watchpoint)(enum gdb_bptype, int, unsigned long);
    int (*remove_hw_watchpoint)(enum gdb_bptype, int, unsigned long);
};

/**
 * struct gdb_io - Describe the interface for an I/O driver to talk with KGDB.
 * @read_char: Pointer to a function that will return one char.
 * @write_char: Pointer to a function that will write one char.
 * @flush: Pointer to a function that will flush any pending writes.
 * @init: Pointer to a function that will initialize the device.
 */
struct gdb_io
{
    int (*read_char)(void);
    void (*write_char)(char);
    void (*flush)(void);
    int (*init)(void);
};

void gdb_console_write(const char *s, unsigned count, void *private_data);
int gdb_handle_exception(int signo, void *regs);

extern void gdb_set_register(void *hw_regs);
extern void gdb_get_register(unsigned long *gdb_regs);
extern void gdb_put_register(unsigned long *gdb_regs);

extern int kgdb_arch_init(void);
extern int kgdb_arch_exit(void);
extern int kgdb_permit_data_access(unsigned long addr, int length);

extern struct gdb_io        gdb_io_ops;

#endif /* __GDB_STUB_H__ */
