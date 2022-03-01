/*
 * =====================================================================================
 *
 *       Filename:  simd.h
 *
 *    Description:  support NEON-SIMD macro definition.
 *
 *        Version:  1.0
 *        Created:  2020年03月10日 11时07分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __SIMD_H__
#define __SIMD_H__

.macro saveneonvfp, reg1, reg2
    ldr     \reg1,  =melis_arch_use_fpu
    ldr     \reg1,  [\reg1]

    cmp     \reg1,  #0
    vpushne {d0-d15}
    vpushne {d16-d31}
    fmrxne  \reg2,  fpscr
    pushne  {\reg2}
    fmrxne  \reg2,  fpexc
    pushne  {\reg2}
    @fmrxne  \reg2,  fpinst             @ FPINST (only if FPEXC.EX is set), Impl def.
    @pushne  {\reg2}
    @fmrxne  \reg2,  fpinst2            @ FPINST2 if needed (and present), Impl def.
    @pushne  {\reg2}
    mrc     p15, 0, \reg2, c0, c0, 5
    and     \reg2, \reg2, #15
    cmp     \reg1,  #0
    pushne  {\reg2}
    stmfd   sp!, {\reg1}
.endm 

.macro restoreneonvfp, reg1, reg2
    ldmfd   sp!, {\reg1}
    ldr     \reg2,  =melis_arch_use_fpu
    str     \reg1,  [\reg2]

    cmp     \reg1,  #0
    popne   {\reg2}                     @ pop cpuid if exist.
    @popne   {\reg2}
    @vmsrne  fpinst2, \reg2
    @popne   {\reg2}
    @vmsrne  fpinst,  \reg2
    popne   {\reg2}
    vmsrne  fpexc, \reg2
    popne   {\reg2}
    vmsrne  fpscr, \reg2
    vpopne  {d16-d31}
    vpopne  {d0-d15}
.endm

.extern printk
.macro  DBGSTR, str
    stmfd   sp!, {r0-r3, ip, lr}
    ldr     r0, =1f
    bl      printk
    ldmfd   sp!, {r0-r3, ip, lr}

    .pushsection .rodata, "a"
1:  .ascii  "kern: \str\n"
    .byte   0
    .previous
.endm
 
.macro  DBGSTR1, str, arg
    stmfd   sp!, {r0-r3, ip, lr}
    mov     r1, \arg
    ldr     r0, =1f
    bl      printk
    ldmfd   sp!, {r0-r3, ip, lr}
  
    .pushsection .rodata, "a"
1:  .ascii  "kern: \str\n"
    .byte   0
    .previous
.endm
 
.macro  DBGSTR3, str, arg1, arg2, arg3
    stmfd   sp!, {r0-r3, ip, lr}
    mov     r3, \arg3
    mov     r2, \arg2
    mov     r1, \arg1
    ldr     r0, =1f
    bl      printk
    ldmfd   sp!, {r0-r3, ip, lr}

    .pushsection .rodata, "a"
1:  .ascii  "kern: \str\n"
    .byte   0
    .previous
.endm

#endif  /*SIMD_H*/
