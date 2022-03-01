/*
 * =====================================================================================
 *
 *       Filename:  port_s.S
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
 *  Last Modified:  2019-05-05 09:39:07
 *
 * =====================================================================================
 */

#include <cpu.h>
#include <cfi.h>

.extern melis_kernel_running
.extern melis_pend_sched_flag
.extern melis_sched_to_thread
.extern melis_sched_from_thread

/*
 * the cache line size of the I and D cache
 */
#define CACHE_DLINESIZE 32
#define CACHE_DLIMIT    32768

/*
 * uint32_t awos_arch_lock_irq(void);
 */
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

/*
 * void awos_arch_unlock_irq(uint32_t cpu_sr);
 */
cfi_debug_info_begin awos_arch_unlock_irq
    msr     cpsr_c, r0
    bx      lr
cfi_debug_info_end awos_arch_unlock_irq

 @ void awos_arch_set_vector_base(uint32_t newvector);
 @ only ARM11 and cortex-a can modify the vector address.
 @ ARM7,ARM9,ARM10 only fixed at 0x0 or 0xffff0000. 
cfi_debug_info_begin awos_arch_set_vector_base
    mrc     p15, #0, r1, c1, c0, #0            @ Read CP15 control register.
    bic     r1,  r1, #C1_V_BIT                 @ Clear V bit 12 to low vector 0x0~0x1c
    mcr     p15, #0, r1, c1, c0, #0            @ Write CP15
    mcr     p15, #0, r0, c12, c0, #0           @ Set new vector to cp12
    bx      lr
cfi_debug_info_end awos_arch_set_vector_base

/*
 * void awos_arch_task_switch(rt_uint32 from, rt_uint32 to);
 * r0 --> from
 * r1 --> to
 */
cfi_debug_info_begin awos_arch_task_switch
    stmfd   sp!, {lr}                @ push pc (lr should be pushed in place of pc)
    stmfd   sp!, {r0-r12, lr}        @ push lr & register file
    mrs     r4,  cpsr
    stmfd   sp!, {r4}                @ push cpsr
    mrc     p15, 0, r4, c13, c0, 0
    stmfd   sp!, {r4} 
    str     sp,  [r0]                @ store sp in preempted tasks tcb
    ldr     sp,  [r1]                @ get new task stack pointer
    ldmfd   sp!, {r4}
    mcr     p15, 0, r4, c13, c0, 0   @ restore new task fcse
    ldmfd   sp!, {r4}                @ pop new task spsr
    msr     spsr_cxsf, r4
    ldmfd   sp!, {r0-r12, lr, pc}^   @ pop new task r0-r12, lr & pc
cfi_debug_info_end awos_arch_task_switch

@uint32_t awos_arch_chksched_start(void);
cfi_debug_info_begin awos_arch_chksched_start
    ldr     r0, =melis_kernel_running
    ldrb    r0, [r0]
    bx      lr
cfi_debug_info_end awos_arch_chksched_start

/*
 * void awos_arch_first_task_start(rt_uint32 to);
 * r0 --> to
 */
cfi_debug_info_begin awos_arch_first_task_start
    ldr     r1, =melis_kernel_running
    mov     r2, #1
    strb    r2, [r1]

    ldr     sp, [r0]                 @ get new task stack pointer

    ldmfd   sp!, {r0}
    mcr     p15, 0, r0, c13, c0, 0   @ restore new task fcse

    ldmfd   sp!, {r0}                @ pop new task spsr
    ldr     r1, [sp, #56]            @ get pc
    tst     r1, #1
    orrne   r0, #32                  @ if pc is thumb mode, set spsr to thumb
    msr     spsr_cxsf, r0
    ldmfd   sp!, {r0-r12, lr, pc}^   @ pop new task r0-r12, lr & pc
cfi_debug_info_end awos_arch_first_task_start

cfi_debug_info_begin awos_arch_dbg_task_switch_to
    mov     sp,  r0                  @ get new task stack pointer
    ldmfd   sp!, {r4}
    mcr     p15, 0, r4, c13, c0, 0   @ restore new task fcse
    ldmfd   sp!, {r4}                @ pop new task spsr
    msr     spsr_cxsf, r4
    ldmfd   sp!, {r0-r12, lr, pc}^   @ pop new task r0-r12, lr & pc
cfi_debug_info_end awos_arch_dbg_task_switch_to

/*
 * void awos_arch_task_switch_inirq(uint32 from, uint32 to);
 */

cfi_debug_info_begin awos_arch_task_switch_inirq
    ldr     r2, =melis_pend_sched_flag
    ldr     r3, [r2]
    cmp     r3, #1
    beq     _reswitch
    mov     r3, #1                          @ set flag to 1
    str     r3, [r2]
    ldr     r2, =melis_sched_from_thread    @ set rt_interrupt_from_thread
    str     r0, [r2]
_reswitch:
    ldr     r2, =melis_sched_to_thread      @ set rt_interrupt_to_thread
    str     r1, [r2]
    bx      lr
    nop
cfi_debug_info_end awos_arch_task_switch_inirq

cfi_debug_info_begin awos_arch_clean_dcache
    mrc     p15, 0x0, r15, c7, c10, 3
    bne     awos_arch_clean_dcache
    mcr     p15, 0, r0, c7, c10, 4          @ drain WB  
    mov     pc, lr
cfi_debug_info_end awos_arch_clean_dcache

cfi_debug_info_begin awos_arch_clean_flush_dcache
    mrc     p15, 0x0, r15, c7, c14, 3
    bne     awos_arch_clean_flush_dcache
    mcr     p15, 0, r0, c7, c10, 4
    mov     pc, lr
cfi_debug_info_end awos_arch_clean_flush_dcache

cfi_debug_info_begin awos_arch_clean_flush_cache
    mrc     p15, 0x0, r15, c7, c14, 3
    bne     awos_arch_clean_flush_cache
    mcr     p15, 0x0, r0, c7, c5, 0
    mcr     p15, 0, r0, c7, c10, 4          @ drain WB
    mov     pc, lr
cfi_debug_info_end awos_arch_clean_flush_cache

cfi_debug_info_begin awos_arch_flush_icache
    mov     r0, #0
    mcr     p15, 0, r0, c7, c5, 0
    mcr     p15, 0, r0, c7, c10, 4          @ drain WB
    mov     pc, lr
cfi_debug_info_end awos_arch_flush_icache

cfi_debug_info_begin awos_arch_flush_dcache
    mov     r0, #0
    mcr     p15, 0x0, r0, c7, c6, 0
    mcr     p15, 0, r0, c7, c10, 4          @ drain WB
    mov     pc, lr
cfi_debug_info_end awos_arch_flush_dcache

cfi_debug_info_begin awos_arch_flush_cache
    mov      r0,#0
    mcr      p15,0x0,r0,c7,c7,0
    mcr      p15, 0, r0, c7, c10, 4          @ drain WB
    mov      pc,lr
cfi_debug_info_end awos_arch_flush_cache

#invalidate icache operation.
cfi_debug_info_begin awos_arch_mems_flush_icache_region
    add     r1, r0, r1
1:
    mcr     p15, 0x0, r0, c7, c5, 1
    add     r0, r0, #CACHE_DLINESIZE
    cmp     r0, r1
    blo     1b  
    mcr     p15, 0, r0, c7, c10, 4          @ drain WB
    mov     pc, lr
cfi_debug_info_end awos_arch_mems_flush_icache_region

#invalidate dcache operation.
cfi_debug_info_begin awos_arch_mems_flush_dcache_region
    add     r1, r0, r1
1:
    mcr     p15, 0x0, r0, c7, c6, 1
    add     r0, r0, #CACHE_DLINESIZE
    cmp     r0, r1
    blo     1b  
    mcr     p15, 0, r0, c7, c10, 4          @ drain WB
    mov     pc, lr
cfi_debug_info_end awos_arch_mems_flush_dcache_region

cfi_debug_info_begin awos_arch_mems_flush_cache_region
    add     r1, r0, r1
1:
    mcr     p15, 0x0, r0, c7, c5, 1
    mcr     p15, 0x0, r0, c7, c6, 1
    add     r0, r0, #CACHE_DLINESIZE
    cmp     r0, r1
    blo     1b  
    mcr     p15, 0, r0, c7, c10, 4          @ drain WB
    mov     pc, lr
cfi_debug_info_end awos_arch_mems_flush_cache_region

cfi_debug_info_begin awos_arch_mems_clean_dcache_region
    add     r1, r0, r1
1:
    mcr     p15, 0x0, r0, c7, c10, 1
    add     r0, r0, #CACHE_DLINESIZE
    cmp     r0, r1
    blo     1b  
    mcr     p15, 0, r0, c7, c10, 4          @ drain WB
    mov     pc, lr
cfi_debug_info_end awos_arch_mems_clean_dcache_region

cfi_debug_info_begin awos_arch_mems_clean_flush_dcache_region
    add     r1, r0, r1
1:
    mcr     p15, 0x0, r0, c7, c14, 1        @ clean+invalidate D entry
    add     r0, r0, #CACHE_DLINESIZE
    cmp     r0, r1
    blo     1b  
    mcr     p15, 0, r0, c7, c10, 4          @ drain WB
    mov     pc, lr
cfi_debug_info_end awos_arch_mems_clean_flush_dcache_region


cfi_debug_info_begin awos_arch_mems_clean_flush_cache_region
    add     r1, r0, r1
1:
    mcr     p15, 0x0, r0, c7, c14, 1
    mcr     p15, 0x0, r0, c7, c5, 1
    add     r0, r0, #CACHE_DLINESIZE
    cmp     r0, r1
    blo     1b  
    mcr     p15, 0, r0, c7, c10, 4          @ drain WB
    mov     pc, lr
cfi_debug_info_end awos_arch_mems_clean_flush_cache_region


/*
 * void cpu_arm926_tlb_invalidate_all(void)
 *
 * Invalidate all TLB entries
 */
cfi_debug_info_begin cpu_arm926_tlb_invalidate_all
    mov r0, #0
    mcr p15, 0, r0, c7, c10, 4      @ drain WB
    mcr p15, 0, r0, c8, c7, 0       @ invalidate I & D TLBs
    mov pc, lr
cfi_debug_info_end cpu_arm926_tlb_invalidate_all

/*
 * cpu_arm926_cache_clean_invalidate_all()
 *
 * clean and invalidate all cache lines
 *
 * Note:
 *  1. we should preserve r0 at all times
 */
.align  5
cfi_debug_info_begin cpu_arm926_cache_clean_invalidate_all
    mov r2, #4
    mov ip, #0
__flush_whole_cache:
#ifdef CONFIG_CPU_DCACHE_WRITETHROUGH
    mcr p15, 0, ip, c7, c6, 0       @ invalidate D cache
#else
1:  mrc p15, 0, r15, c7, c14, 3     @ test,clean,invalidate
    bne 1b
#endif
    tst r2, #4
    mcrne   p15, 0, ip, c7, c5, 0   @ invalidate I cache
    mcrne   p15, 0, ip, c7, c10, 4  @ drain WB
    mov pc, lr
cfi_debug_info_end cpu_arm926_cache_clean_invalidate_all

.globl awos_arch_test_clean_flush_cache
.set   awos_arch_test_clean_flush_cache, cpu_arm926_cache_clean_invalidate_all

cfi_debug_info_begin arm926_dma_clean_range
    bic     r0, r0, #CACHE_DLINESIZE - 1
1:  mcr     p15, 0, r0, c7, c10, 1          @ clean D entry
    add     r0, r0, #CACHE_DLINESIZE
    cmp     r0, r1
    blo     1b
    mcr     p15, 0, r0, c7, c10, 4          @ drain WB
    bx      lr
cfi_debug_info_end arm926_dma_clean_range

cfi_debug_info_begin arm926_dma_inv_range
    tst     r0, #CACHE_DLINESIZE - 1
    mcrne   p15, 0, r0, c7, c10, 1          @ clean D entry
    tst     r1, #CACHE_DLINESIZE - 1
    mcrne   p15, 0, r1, c7, c10, 1          @ clean D entry
    bic     r0, r0, #CACHE_DLINESIZE - 1
1:  mcr     p15, 0, r0, c7, c6, 1           @ invalidate D entry
    add     r0, r0, #CACHE_DLINESIZE
    cmp     r0, r1
    blo     1b
    mcr     p15, 0, r0, c7, c10, 4          @ drain WB
    bx      lr
cfi_debug_info_end arm926_dma_inv_range

cfi_debug_info_begin arm926_dma_flush_range
    bic     r0, r0, #CACHE_DLINESIZE - 1
1:       
#ifndef CONFIG_CPU_DCACHE_WRITETHROUGH
    mcr     p15, 0, r0, c7, c14, 1          @ clean+invalidate D entry
#else    
    mcr     p15, 0, r0, c7, c6, 1           @ invalidate D entry
#endif   
    add     r0, r0, #CACHE_DLINESIZE
    cmp     r0, r1
    blo     1b
    mcr     p15, 0, r0, c7, c10, 4          @ drain WB
    bx      lr 
cfi_debug_info_end arm926_dma_flush_range

/*                                                            
 *  dma_map_area(start, size)                       
 *  - start - kernel virtual start address                  
 *  - size  - size of region 
 */                                     
cfi_debug_info_begin cpu_arm926_dma_map_area
    push    {lr}
    add     r1, r1, r0 
    bl      arm926_dma_clean_range 
    bl      arm926_dma_inv_range
    bl      arm926_dma_flush_range
    pop     {pc}
cfi_debug_info_end cpu_arm926_dma_map_area

cfi_debug_info_begin arm926_flush_icache_all
    mov     r0, #0
    mcr     p15, 0, r0, c7, c5, 0           @ invalidate I cache
    bx      lr
cfi_debug_info_end arm926_flush_icache_all

/* uint8_t awos_arch_set_fsce_id(uint8_t fsceid); */
cfi_debug_info_begin awos_arch_set_fsce_id
    push    {lr} 
    mov     r1, r0
    mrs     r2, cpsr
    orr     lr, r2, #0xc0
    msr     cpsr_c, lr 
    mrc     p15, 0x0, r3, c13, c0, 0
    lsl     r0, r1, #25
    mcr     p15, 0x0, r0, c13, c0, 0
    msr     cpsr_c, r2
    lsr     r0, r3, #25
    pop     {pc}
cfi_debug_info_end awos_arch_set_fsce_id

@int setjmp(jmp_buf env);
cfi_debug_info_begin setjmp
    stmia   r0!, {r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr}
    mov     r0, #0
    tst     lr, #1
    moveq   pc, lr
    .word   0xe12fff1e
cfi_debug_info_end setjmp

@void longjmp(jmp_buf env,int val);
cfi_debug_info_begin longjmp
    ldmfd   r0!, {r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr}
    movs    r0,  r1
    moveq   r0,  #1 
    tst     lr,  #1
    moveq   pc,  lr
    .word   0xe12fff1e
cfi_debug_info_end longjmp

 /* 
  * void enable_fpu(void); Unfortunately, In ARMv5TEJ (which the ARM926EJ-S implements) there *
  * is no ID register to report the presence of the FPU. Therefore the only way to determine  *
  * whether it is there to try to enable it.  This will succeed if it is present, or generate *
  * an UNDEF exception if if it is not. it is not supported by default on SUN3IW2(D100) project.
  */
@void aw_hal_enable_vfp(void);
@.global aw_hal_enable_vfp
@aw_hal_enable_vfp:
    @push    {lr}
    @mrc     p15, 0, r0, c1, c0, 2
    @orr     r0, r0, #0x300000            @ single precision
    @orr     r0, r0, #0xC00000            @ double precision
    @mcr     p15, 0, r0, c1, c0, 2
    @mov     r0, #0x40000000
    @fmxr    fpexc,r0
    @pop     {pc}

    
.section .tlb.code, "ax"
.global awos_arch_lock_tlb_zone
awos_arch_lock_tlb_zone:
    mov     r1, r0                        @ save r0(lockAddr) to r1
    mcr     p15,0,r1,c8,c7,1              @ invalidate TLB single entry to ensure that lockAddr is not already in the TLB
    mrc     p15,0,r0,c10,c0,0             @ read lockdown register
    orr     r0, r0,#1                     @ set preserve bit
    mcr     p15,0,r0,c10,c0,0             @ write lockdown register
    ldr     r1, [r1]                      @ cause TLB miss to load TLB entry
    mrc     p15,0,r0,c10,c0,0             @ read lockdown register
    bic     r0, r0,#1                     @ clear preserve bit
    mcr     p15,0,r0,c10,c0,0             @ write lockdown register
    mov     pc, lr                        @ tlb lockdown return

/*
 * cpu_arm926_set_pgd(pgd)
 * Set the translation base pointer to be as described by pgd.
 * pgd: new page tables
 */
.section .tlb.code, "ax"
.global cpu_arm926_set_pgd
cpu_arm926_set_pgd:
    mov     ip,  #0
1:  
    mrc     p15, 0, r15, c7, c14, 3     @ test,clean,invalidate whole DCache
    bne     1b

    mcr     p15, 0, ip, c7, c5, 0       @ invalidate I cache
    mcr     p15, 0, ip, c7, c10,4       @ drain WB
    mcr     p15, 0, r0, c2, c0, 0       @ load page table pointer
    mcr     p15, 0, ip, c8, c7, 0       @ invalidate I & D TLBs
    mov     pc, lr
.end
