/*
 * ARM GDB support
 * arch-specific portion of GDB stub
 *
 * File      : arch_gdb.h(arm)
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2014-07-04     wzyy2      first version
 * 2019-12-25     ZengZhijin refactor code to support hw breakpoint/watchpoint
 */
#ifndef __ARM_GDB_H__
#define __ARM_GDB_H__

#include <excep.h>

/*
 * By doing this as an undefined instruction trap, we force a mode
 * switch from SVC to UND mode, allowing us to save full kernel state.
 * We also define a GDB_COMPILED_BREAK which can be used to compile
 * in breakpoints.
 */
#define BREAK_INSTR_SIZE    4
#define GDB_BREAKINST       0xe7ffdefe
#define GDB_THUMB_BREAKINST 0x0000defe
#define GDB_COMPILED_BREAK  0xe7ffdeff
#define CACHE_FLUSH_IS_SAFE 1

#define ARM_GP_REGS         16
#define ARM_FP_REGS         8
#define ARM_EXTRA_REGS      2
#define GDB_MAX_REGS        (ARM_GP_REGS + (ARM_FP_REGS * 3) + ARM_EXTRA_REGS)
#define NUMREGBYTES         (GDB_MAX_REGS << 2)

#define SVC_MODE 0x00000013
#define MODE_MASK 0x0000001f

//#define BUFMAX            ((NUMREGBYTES << 1) + 10)
#define BUFMAX          400

enum regnames
{
    GDB_R0,     /*0*/
    GDB_R1,     /*1*/
    GDB_R2,     /*2*/
    GDB_R3,     /*3*/
    GDB_R4,     /*4*/
    GDB_R5,     /*5*/
    GDB_R6,     /*6*/
    GDB_R7,     /*7*/
    GDB_R8,     /*8*/
    GDB_R9,     /*9*/
    GDB_R10,    /*10*/
    GDB_FP,     /*11*/
    GDB_IP,     /*12*/
    GDB_SP,    /*13*/
    GDB_LR,     /*14*/
    GDB_PC,     /*15*/
#if 0
    GDB_DSTART,
    GDB_D0,
    GDB_D1,
    GDB_D2,
    GDB_D3,
    GDB_D4,
    GDB_D5,
    GDB_D6,
    GDB_D7,
    GDB_D8,
    GDB_D9,
    GDB_D10,
    GDB_D11,
    GDB_D12,
    GDB_D13,
    GDB_D14,
    GDB_D15,
    GDB_D16,
    GDB_D17,
    GDB_D18,
    GDB_D19,
    GDB_D20,
    GDB_D21,
    GDB_D22,
    GDB_D23,
    GDB_D24,
    GDB_D25,
    GDB_D26,
    GDB_D27,
    GDB_D28,
    GDB_D29,
    GDB_D30,
    GDB_D31,
    GDB_DNUM,
    GDB_FPSCR,
    GDB_FPEXC,
#endif
    GDB_CPSR = GDB_MAX_REGS - 1
};

struct gdb_regs_t
{
    excep_regs_t regs_t;
};

extern struct gdb_arch      arch_gdb_ops;
void gdb_get_register(unsigned long *gdb_regs);
void gdb_put_register(unsigned long *gdb_regs);
void gdb_set_register(void *hw_regs);
int gdb_arch_handle_exception(char *remcom_in_buffer,
                              char *remcom_out_buffer);
void gdb_flush_icache_range(unsigned long start, unsigned long end);

int gdb_handle_exception(int signo, void *regs);

#endif /* __ARM_GDB_H__ */
