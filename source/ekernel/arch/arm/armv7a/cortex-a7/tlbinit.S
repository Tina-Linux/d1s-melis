/*
 * =====================================================================================
 *
 *       Filename:  tlbinit.S
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
 *  Last Modified:  2020-05-18 16:36:31
 *
 * =====================================================================================
 */

#include <cpu.h>
#include <cfi.h>

cfi_tlb_info_begin awos_arch_dcache_enable
    mrc     p15, #0, r0, c1, c0, #0
    orr     r0,  r0, #0x00000004
    mcr     p15, #0, r0, c1, c0, #0
    bx      lr
cfi_tlb_info_end awos_arch_dcache_enable

cfi_tlb_info_begin awos_arch_icache_enable
    mrc     p15, #0, r0, c1, c0, #0
    orr     r0,  r0, #0x00001000
    mcr     p15, #0, r0, c1, c0, #0
    bx      lr
cfi_tlb_info_end awos_arch_icache_enable

.section .tlb.code, "ax"
_FLD_MAX_WAY:
   .word  0x3ff
_FLD_MAX_IDX:
   .word  0x7fff
cfi_tlb_info_begin awos_arch_dcache_clean_flush
    push    {r4-r11}
    dmb
    mrc     p15, #1, r0, c0, c0, #1  @ read clid register
    ands    r3, r0, #0x7000000       @ get level of coherency
    mov     r3, r3, lsr #23
    beq     finished
    mov     r10, #0
loop1:
    add     r2, r10, r10, lsr #1
    mov     r1, r0, lsr r2
    and     r1, r1, #7
    cmp     r1, #2
    blt     skip
    mcr     p15, #2, r10, c0, c0, #0
    isb
    mrc     p15, #1, r1, c0, c0, #0
    and     r2, r1, #7
    add     r2, r2, #4
    ldr     r4, _FLD_MAX_WAY
    ands    r4, r4, r1, lsr #3
    clz     r5, r4
    ldr     r7, _FLD_MAX_IDX
    ands    r7, r7, r1, lsr #13
loop2:
    mov     r9, r4
loop3:
    orr     r11, r10, r9, lsl r5
    orr     r11, r11, r7, lsl r2
    mcr     p15, #0, r11, c7, c14, #2
    subs    r9, r9, #1
    bge     loop3
    subs    r7, r7, #1
    bge     loop2
skip:
    add     r10, r10, #2
    cmp     r3, r10
    bgt     loop1

finished:
    dsb
    isb
    pop     {r4-r11}
    bx      lr
cfi_tlb_info_end awos_arch_dcache_clean_flush

cfi_tlb_info_begin awos_arch_icache_flush
    mov     r0, #0
    mcr     p15, 0, r0, c7, c5, 0       @ I+BTB cache invalidate
    dsb
    isb
    bx      lr
cfi_tlb_info_end awos_arch_icache_flush

cfi_tlb_info_begin awos_arch_dcache_disable
    push    {r4-r11, lr}
    bl      awos_arch_dcache_clean_flush
    mrc     p15, #0, r0, c1, c0, #0
    bic     r0,  r0, #0x00000004
    mcr     p15, #0, r0, c1, c0, #0
    pop     {r4-r11, lr}
    bx      lr
cfi_tlb_info_end awos_arch_dcache_disable

cfi_tlb_info_begin awos_arch_icache_disable
    mrc     p15, #0, r0, c1, c0, #0
    bic     r0,  r0, #0x00001000
    mcr     p15, #0, r0, c1, c0, #0
    bx      lr
cfi_tlb_info_end awos_arch_icache_disable

cfi_tlb_info_begin awos_arch_mmu_disable
    mrc     p15, #0, r0, c1, c0, #0
    bic     r0, r0, #1
    mcr     p15, #0, r0, c1, c0, #0    @ clear mmu bit
    dsb
    bx      lr
cfi_tlb_info_end awos_arch_mmu_disable

cfi_tlb_info_begin awos_arch_mmu_enable
    mrc     p15, #0, r0, c1, c0, #0
    orr     r0, r0, #0x001
    mcr     p15, #0, r0, c1, c0, #0    @ set mmu enable bit
    dsb
    bx      lr
cfi_tlb_info_end awos_arch_mmu_enable

cfi_tlb_info_begin awos_arch_tlb_set
    mcr     p15, #0, r0, c2, c0, #0
    isb     sy

    ldr     r0, =0x0
    mcr     p15, #0, r0, c2, c0, #1
    isb     sy

    dmb    
    bx      lr
cfi_tlb_info_end awos_arch_tlb_set

    .end
