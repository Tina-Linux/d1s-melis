/*
 * =====================================================================================
 *
 *       Filename:  syscall.S
 *
 *    Description:  syscall layer for user mode.
 *
 *        Version:  Melis3.0 
 *         Create:  2017-11-06 19:09:12
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2018-09-07 11:16:39
 *
 * =====================================================================================
 */

 #include <cfi.h>

@porting from musl libc, take r7 as syscall number.
@syscall entry with parameter number less than 4.

.syntax unified
.global __syscallx
.global __syscall5
.global __syscall6

/* ----------------------------------------------------------------------------*/
/** @brief  <cant be more than 4 parameters, r0-r3 for pass parameter.> */
/* ----------------------------------------------------------------------------*/
cfi_debug_info_begin  __syscallx
__syscallx:
    stmfd   sp!, {r7-r8, lr}
    mov     r7,  r0
    mov     r0,  r1
    mov     r1,  r2
    mov     r2,  r3
    ldr     r3,  [sp, #8]
    mrs     lr,  spsr
    stmfd   sp!, {lr}
    mrs     r8,  cpsr
    svc     0
    msr     cpsr_cxsf, r8 
    ldmfd   sp!, {lr}
    msr     spsr_cxsf, lr
    ldmfd   sp!, {r7-r8, lr}
    bx      lr
cfi_debug_info_end  __syscallx

/* ----------------------------------------------------------------------------*/
/** @brief  <syscall with 5 parameter, the 5th parameter passed by stack.> */
/* ----------------------------------------------------------------------------*/
cfi_debug_info_begin  __syscall5
__syscall5:
    stmfd   sp!, {r4,r7-r8,lr}
    mov     r7,  r0
    mov     r0,  r1
    mov     r1,  r2
    mov     r2,  r3
    ldr     r3,  [sp, #12]
    ldr     r4,  [sp, #16]
    mrs     lr,  spsr
    stmfd   sp!, {lr}
    stmfd   sp!, {r4}
    mrs     r8,  cpsr
    svc     0
    msr     cpsr_cxsf, r8 
    ldmfd   sp!, {r4}
    ldmfd   sp!, {lr}
    msr     spsr_cxsf, lr
    ldmfd   sp!, {r4,r7-r8,lr}
    bx      lr
cfi_debug_info_end  __syscall5

/* ----------------------------------------------------------------------------*/
/** @brief  <syscall with 6 parameter, the 5th, 6th parameter passed by stack.>*/
/* ----------------------------------------------------------------------------*/
cfi_debug_info_begin  __syscall6
__syscall6:
    stmfd   sp!, {r4-r5,r7-r8,lr}
    mov     r7,  r0
    mov     r0,  r1
    mov     r1,  r2
    mov     r2,  r3
    ldr     r3,  [sp, #16]
    ldr     r4,  [sp, #20]
    ldr     r5,  [sp, #24]
    mrs     lr,  spsr
    stmfd   sp!, {lr}
    stmfd   sp!, {r4-r5}
    mrs     r8,  cpsr
    svc     0
    msr     cpsr_cxsf, r8 
    ldmfd   sp!, {r4-r5}
    ldmfd   sp!, {lr}
    msr     spsr_cxsf, lr
    ldmfd   sp!, {r4-r5,r7-r8,lr}
    bx      lr
cfi_debug_info_end  __syscall6

.end
