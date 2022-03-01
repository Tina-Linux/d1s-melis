/*
 * ===========================================================================================
 *
 *       Filename:  arm_fpu.S
 *
 *    Description:  arm neon/vfp process file.
 *
 *        Version:  Melis3.0 
 *         Create:  2019-12-02 13:49:07
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-05-18 16:24:36
 *
 * ===========================================================================================
 */

#include <cfi.h>
#include <cpu.h>
#include <kconfig.h>

// void awos_arch_fpu_init(void)
cfi_debug_info_begin awos_arch_fpu_init
    mrc     p15, 0, r0, c1, c0, 2    @ Read CP Access register
    ldr     r0, =(0xf << 20)         @ Enable full access to NEON/VFP (Coprocessors 10 and 11)
    mcr     p15, 0, r0, c1, c0, 2    @ Write CP Access register

    vmrs    r0, fpexc                @ Switch on the VFP and NEON hardware
    orr     r0, r0, #0x40000000      @ Set EN bit in FPEXC
    vmsr    fpexc, r0
    bx      lr
cfi_debug_info_end awos_arch_fpu_init

// void enable_vfp_early(void)
cfi_tlb_info_begin enable_vfp_early
    mrc     p15, 0, r0, c1, c0, 2    @ Read CP Access register
    ldr     r0, =(0xf << 20)         @ Enable full access to NEON/VFP (Coprocessors 10 and 11)
    mcr     p15, 0, r0, c1, c0, 2    @ Write CP Access register

    vmrs    r0, fpexc                @ Switch on the VFP and NEON hardware
    orr     r0, r0, #0x40000000      @ Set EN bit in FPEXC
    vmsr    fpexc, r0
    bx      lr
cfi_tlb_info_end enable_vfp_early
