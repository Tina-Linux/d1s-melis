/*
 * =====================================================================================
 *
 *       Filename:  context_switch.S
 *
 *    Description:  this file contain the arch dependent routines. common layer for both
 *                  RT-Thread and Zephyr Kernel.
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 10:10:33
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-07-26 16:37:26
 *
 * =====================================================================================
 */

#include <csr.h>
#include <excep.h>
/*
 * unsigned long awos_arch_task_switch(unsigned long from, unsigned long to);
 */

.global awos_arch_task_switch
awos_arch_task_switch:
   addi  sp,  sp, -14 * 8
   sd    ra,  0*8(sp)

   csrr  t0,  sstatus 
   li    t1,  SR_SPIE
   not   t1,  t1
   li    t2,  SR_SPP
   and   t0,  t0, t1
   or    t0,  t0, t2
   sd    t0,  1*8(sp)

   sd    s0,  2*8(sp)
   sd    s1,  3*8(sp)
   sd    s2,  4*8(sp)
   sd    s3,  5*8(sp)
   sd    s4,  6*8(sp)
   sd    s5,  7*8(sp)
   sd    s6,  8*8(sp)
   sd    s7,  9*8(sp)
   sd    s8,  10*8(sp)
   sd    s9,  11*8(sp)
   sd    s10, 12*8(sp)
   sd    s11, 13*8(sp)

   sd    sp,  0(a0)
   ld    sp,  0(a1)

   jal   check_gp_balance

   # load context switch registers.
   ld    ra,  0*8(sp)

   # load sstatus register.
   ld    t0,  1*8(sp)
   csrw  sstatus,t0
   csrw  sepc,ra

   ld    s0,  2*8(sp)
   ld    s1,  3*8(sp)
   ld    s2,  4*8(sp)
   ld    s3,  5*8(sp)
   ld    s4,  6*8(sp)
   ld    s5,  7*8(sp)
   ld    s6,  8*8(sp)
   ld    s7,  9*8(sp)
   ld    s8,  10*8(sp)
   ld    s9,  11*8(sp)
   ld    s10, 12*8(sp)
   ld    s11, 13*8(sp)
   addi  sp,  sp, 8*14

   move  tp,  a1
#if 1
   sret
#else
   #pseudo of 'jalr zero, 0(ra)'
   ret
#endif

/*
 * void awos_arch_first_task_start(unsigned long to);
 * r0 --> to
 */
.global awos_arch_first_task_start
awos_arch_first_task_start:
   la    t0,  melis_kernel_running
   li    t1,  1
   sb    t1,  (t0)

   ld    sp,  0(a0)

   # load context switch registers.
   ld    ra,  0*8(sp)

   #load sstatus register.
   ld    t0,  1*8(sp)
   csrw  sstatus,t0
   csrw  sepc,ra

   ld    s0,  2*8(sp)
   ld    s1,  3*8(sp)
   ld    s2,  4*8(sp)
   ld    s3,  5*8(sp)
   ld    s4,  6*8(sp)
   ld    s5,  7*8(sp)
   ld    s6,  8*8(sp)
   ld    s7,  9*8(sp)
   ld    s8,  10*8(sp)
   ld    s9,  11*8(sp)
   ld    s10, 12*8(sp)
   ld    s11, 13*8(sp)
   addi  sp,  sp, 8*14

   move  tp,  a0
   move  a1,  a0
   move  a0,  zero

   # task top exit entry to ra.
   move  ra,  s3

#if 1
   sret
#else
   #pseudo of 'jalr zero, 0(ra)'
   ret
#endif

/*
 * uint32_t awos_arch_chksched_start(void);
 */
.global awos_arch_chksched_start
awos_arch_chksched_start:
   la    t0,  melis_kernel_running
   lbu   a0, (t0)
   ret

/*
 * void awos_arch_save_fpu_status(fpu_context_t *);
 */
.global awos_arch_save_fpu_status
awos_arch_save_fpu_status:
   li  t0, SR_FS
   csrs sstatus, t0
   frcsr    t1
   fsd f0,  FPU_CTX_F0_F0(a0)
   fsd f1,  FPU_CTX_F1_F0(a0)
   fsd f2,  FPU_CTX_F2_F0(a0)
   fsd f3,  FPU_CTX_F3_F0(a0)
   fsd f4,  FPU_CTX_F4_F0(a0)
   fsd f5,  FPU_CTX_F5_F0(a0)
   fsd f6,  FPU_CTX_F6_F0(a0)
   fsd f7,  FPU_CTX_F7_F0(a0)
   fsd f8,  FPU_CTX_F8_F0(a0)
   fsd f9,  FPU_CTX_F9_F0(a0)
   fsd f10, FPU_CTX_F10_F0(a0)
   fsd f11, FPU_CTX_F11_F0(a0)
   fsd f12, FPU_CTX_F12_F0(a0)
   fsd f13, FPU_CTX_F13_F0(a0)
   fsd f14, FPU_CTX_F14_F0(a0)
   fsd f15, FPU_CTX_F15_F0(a0)
   fsd f16, FPU_CTX_F16_F0(a0)
   fsd f17, FPU_CTX_F17_F0(a0)
   fsd f18, FPU_CTX_F18_F0(a0)
   fsd f19, FPU_CTX_F19_F0(a0)
   fsd f20, FPU_CTX_F20_F0(a0)
   fsd f21, FPU_CTX_F21_F0(a0)
   fsd f22, FPU_CTX_F22_F0(a0)
   fsd f23, FPU_CTX_F23_F0(a0)
   fsd f24, FPU_CTX_F24_F0(a0)
   fsd f25, FPU_CTX_F25_F0(a0)
   fsd f26, FPU_CTX_F26_F0(a0)
   fsd f27, FPU_CTX_F27_F0(a0)
   fsd f28, FPU_CTX_F28_F0(a0)
   fsd f29, FPU_CTX_F29_F0(a0)
   fsd f30, FPU_CTX_F30_F0(a0)
   fsd f31, FPU_CTX_F31_F0(a0)
   sw  t1,  FPU_CTX_FCSR_F0(a0)

   # clr FS domain
   csrc sstatus, t0

   # clean status would clr sr_sd;
   li t0, SR_FS_CLEAN
   csrs sstatus, t0
   ret

/*
 * void awos_arch_restore_fpu_status(fpu_context_t *);
 */

.global awos_arch_restore_fpu_status
awos_arch_restore_fpu_status:
   li  t0, SR_FS
   lw  t1, FPU_CTX_FCSR_F0(a0)
   csrs sstatus, t0
   fld f0, FPU_CTX_F0_F0(a0)
   fld f1, FPU_CTX_F1_F0(a0)
   fld f2, FPU_CTX_F2_F0(a0)
   fld f3, FPU_CTX_F3_F0(a0)
   fld f4, FPU_CTX_F4_F0(a0)
   fld f5, FPU_CTX_F5_F0(a0)
   fld f6, FPU_CTX_F6_F0(a0)
   fld f7, FPU_CTX_F7_F0(a0)
   fld f8, FPU_CTX_F8_F0(a0)
   fld f9, FPU_CTX_F9_F0(a0)
   fld f10,FPU_CTX_F10_F0(a0)
   fld f11,FPU_CTX_F11_F0(a0)
   fld f12,FPU_CTX_F12_F0(a0)
   fld f13,FPU_CTX_F13_F0(a0)
   fld f14,FPU_CTX_F14_F0(a0)
   fld f15,FPU_CTX_F15_F0(a0)
   fld f16,FPU_CTX_F16_F0(a0)
   fld f17,FPU_CTX_F17_F0(a0)
   fld f18,FPU_CTX_F18_F0(a0)
   fld f19,FPU_CTX_F19_F0(a0)
   fld f20,FPU_CTX_F20_F0(a0)
   fld f21,FPU_CTX_F21_F0(a0)
   fld f22,FPU_CTX_F22_F0(a0)
   fld f23,FPU_CTX_F23_F0(a0)
   fld f24,FPU_CTX_F24_F0(a0)
   fld f25,FPU_CTX_F25_F0(a0)
   fld f26,FPU_CTX_F26_F0(a0)
   fld f27,FPU_CTX_F27_F0(a0)
   fld f28,FPU_CTX_F28_F0(a0)
   fld f29,FPU_CTX_F29_F0(a0)
   fld f30,FPU_CTX_F30_F0(a0)
   fld f31,FPU_CTX_F31_F0(a0)
   fscsr t1

   # clr FS domain
   csrc sstatus, t0

   # clean status would clr sr_sd;
   li t0, SR_FS_CLEAN
   csrs sstatus, t0
   ret

/*
 * void check_gp_balance(void);
 */
.global check_gp_balance
check_gp_balance:
.gpldr:
    auipc a2, %pcrel_hi(__global_pointer$)
    addi  a2, a2,%pcrel_lo(.gpldr)

1:  # x3 is gp in abi
    bne   gp, a2,1b
    ret

/*
 * void delay_10insn(void);
 */
.global delay_10insn
delay_10insn:
   li     a0, 10
   beqz   a0, 2f
1:
   addi   a0, a0, -1
   .rept  5
   nop
   .endr
   bnez a0, 1b
2:
   ret

.end
