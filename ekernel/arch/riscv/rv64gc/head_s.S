/*
 * ===========================================================================================
 *
 *       Filename:  startup_gcc.S
 *
 *    Description:  bringup for f133.
 *
 *        Version:  Melis3.0 
 *         Create:  2020-06-24 08:58:38
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2021-01-12 10:15:01
 *
 * ===========================================================================================
 */
#include <csr.h>
#include "mmu/page.h"

.section ".start", "ax", @progbits
.global	_start
.option norvc
_start:
    # mask all the interrupts.
    csrw  sie, 0
    csrw  sip, 0

    # x0 hardcoded to zero.
    li    x1,  0
    li    x2,  0
    li    x3,  0
    li    x4,  0
    li    x5,  0
    li    x6,  0
    li    x7,  0
    li    x8,  0
    li    x9,  0
    li    x10, 0
    li    x11, 0
    li    x12, 0
    li    x13, 0
    li    x14, 0
    li    x15, 0
    li    x16, 0
    li    x17, 0
    li    x18, 0
    li    x19, 0
    li    x20, 0
    li    x21, 0
    li    x22, 0
    li    x23, 0
    li    x24, 0
    li    x25, 0
    li    x26, 0
    li    x27, 0
    li    x28, 0
    li    x29, 0
    li    x30, 0
    li    x31, 0

    # Kind of the chicken/egg first problem
    # the initalization of gp needs to be protected from relaxation otherwise 
    # the initialization of gp itself will try to use a garbage 
    # gp with disastrous results.
    # without this norelax option, la gp, __global_pointer$
    # will be emitted with something like  "addi gp, gp, 0".
.option push # push the following setting of all .options to a stack. so that subsequent .option pop will restore their value.
.option norelax
    la    gp, __global_pointer$
.option pop  # pop the optios stack, restore all .options to the setting of the last .option push time.

    # auipc gp, %pcrel_hi(__global_pointer$)
    # addi  gp, gp, %pcrel_lo(1b)

    la    t0, handle_exception
    csrw  stvec, t0

1:
    /* make sure that stvec is updated */
    csrr    t1, stvec
    bne     t1, t0, 1b

    # clear SD temporaly.disable and interrupt.
    li    t0, SR_FS_DIRTY
    csrc  sstatus, t0

    # initial status to set the fpu registers.
    li    t0, SR_FS_INITIAL
    csrs  sstatus, t0

    # follwing change of fpu registers
    # would set sr_sd again.

    # clear the floating-point swap control and status register.
    # abandon the original value.
    fscsr   x0, x0

    # clear the GPRS of FPU.
    fmv.d.x f0, x0
    fmv.d.x f1, x0
    fmv.d.x f2, x0
    fmv.d.x f3, x0
    fmv.d.x f4, x0
    fmv.d.x f5, x0
    fmv.d.x f6, x0
    fmv.d.x f7, x0
    fmv.d.x f8, x0
    fmv.d.x f9, x0
    fmv.d.x f10,x0
    fmv.d.x f11,x0
    fmv.d.x f12,x0
    fmv.d.x f13,x0
    fmv.d.x f14,x0
    fmv.d.x f15,x0
    fmv.d.x f16,x0
    fmv.d.x f17,x0
    fmv.d.x f18,x0
    fmv.d.x f19,x0
    fmv.d.x f20,x0
    fmv.d.x f21,x0
    fmv.d.x f22,x0
    fmv.d.x f23,x0
    fmv.d.x f24,x0
    fmv.d.x f25,x0
    fmv.d.x f26,x0
    fmv.d.x f27,x0
    fmv.d.x f28,x0
    fmv.d.x f29,x0
    fmv.d.x f30,x0
    fmv.d.x f31,x0

    # clear sscratch register.
    csrw   sscratch,zero
    # clear SD temporaly. and disable interrupt.
    li    t0, SR_FS_DIRTY
    csrc  sstatus, t0

    # re-initial status to initial status.
    li    t0, SR_FS_INITIAL
    csrs  sstatus, t0

    la   t0, __bss_start
    la   t1, __bss_end
    ble  t1, t0, clear_bss_done

clear_bss:
    sd   zero, (t0)
    add  t0, t0, 8
    blt  t0, t1, clear_bss

clear_bss_done:
    la    t0, __init_process_stack_start__
    la    t1, __init_process_stack_end__
    # fill stack with '#'
    li    t2, 0x2323232323232323

1:
    sd    t2, 0(t0)
    add   t0, t0, 8
    blt   t0, t1, 1b

    # prepare stack
    la    sp, __init_process_stack_end__
page_init:
    # mmu init
    la t0, mmu_init
    jalr  t0

    # jump to running address from load address by exception if va!=pa.
    la    a0, running_address
    csrw  stvec, a0

    /* make sure that stvec is updated */
1:  csrr    a1, stvec
    bne     a1, a0, 1b

    # prepare the SATP for latter setting.
    # mode: SV39
    # ASID: we dont use ASID mechanism, so always set to 0.refer to spec riscv-privileged page 67.
    # PPN: system_pg_dir >> 12.
    la    t0, system_pg_dir
    srl   t1, t0, PAGE_SHIFT
    li    t2, SATP_MODE
    or    t1, t1, t2

    # We need a full fence here because above just wrote these PTEs and we need
    # to ensure the new translations are in use.
    sfence.vma
    csrw satp, t1
    sfence.vma

    # trigger exception deliberately.
    unimp
    nop

/* at this point we are running from link address */
running_address:
    nop
    # again in-case distort by above.
    la    t0, handle_exception
    csrw  stvec, t0

    /* make sure that stvec is updated */
1:  csrr    t1, stvec
    bne     t1, t0, 1b

.option rvc
to_c:
    # prepare jump to c code.
    # set stack pointer.
    # boot cost 0 nanoseconds just for dummy
    li    a0,  0
    # report misa to system software.
    # but cant read on s-mode, need report from sbi.
    # csrr a1,  misa
    la  a2, __sbi_binary_start
    la  a3, __sbi_binary_end
    sfence.vma
    la    t0, start_kernel
    jalr  zero, t0
    nop
    nop

.align 3

_load_start:
    .dword 0x0

_link_start:
    .dword _firmware_start

_link_end:
    .dword _firmware_end

.end
