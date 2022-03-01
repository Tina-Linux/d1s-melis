/*
 * =====================================================================================
 *
 *       Filename:  critical.S
 *
 *    Description:  interrupt disable and enable.
 *
 *        Version:  Melis3.0 
 *         Create:  2018-01-17 17:33:26
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2018-01-17 17:35:40
 *
 * =====================================================================================
 */
#include <cfi.h>

#define INT_MASK    0xc0 

.global awos_arch_lock_irq
.global awos_arch_unlock_irq

cfi_debug_info_begin awos_arch_lock_irq
    mrs     r0, cpsr
    orr     r1, r0, #INT_MASK       @ set irq and fiq bits in cpsr to disable all interrupts
    msr     cpsr_c, r1
    mrs     r1, cpsr                @ confirm that cpsr contains the proper interrupt disable flags  
    and     r1, r1, #INT_MASK
    cmp     r1, #INT_MASK
    bne     awos_arch_lock_irq      @ not properly disabled (try again)
    bx      lr                      @ disabled, return the original cpsr contents in r0
cfi_debug_info_end awos_arch_lock_irq

cfi_debug_info_begin awos_arch_unlock_irq
    msr     cpsr_c, r0
    mov     pc, lr
cfi_debug_info_end awos_arch_unlock_irq