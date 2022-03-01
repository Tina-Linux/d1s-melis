/*
 * ===========================================================================================
 *
 *       Filename:  tlbop.S
 *
 *    Description:  tlb operations 
 *
 *        Version:  Melis3.0 
 *         Create:  2019-12-10 09:08:00
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-06-10 20:07:07
 *
 * ===========================================================================================
 */
#include <cfi.h>
#include <cpu.h>
#include <kconfig.h>
#include "page_2nd_level.h"
#include "pgtable-2level-hwdef.h"

/*         
 *awos_arch_flush_pmd_entry
 *Flush a PMD entry (word aligned, or double-word aligned) to
 *RAM if the TLB for the CPU we are running on requires this.
 *This is typically used when we are creating PMD entries.
 *   
 *awos_arch_clean_pmd_entry
 *Clean (but don't drain the write buffer) if the CPU requires
 *these operations.  This is typically used when we are removing
 *PMD entries.
 */

//void awos_arch_flush_pmd_entry(uint32_t pmd);
cfi_debug_info_begin awos_arch_flush_pmd_entry
    mcr     p15, 0, r0, c7, c10, 1
    mov     r3, #0  
    mcr     p15, 0, r3, c7, c10, 4
    bx      lr
cfi_debug_info_end awos_arch_flush_pmd_entry

//void awos_arch_clean_pmd_entry(uint32_t pmd);
cfi_debug_info_begin awos_arch_clean_pmd_entry
    mcr     p15, 0, r0, c7, c10, 1     
    bx      lr             
cfi_debug_info_end awos_arch_clean_pmd_entry

/*
 *void awos_arch_set_pte_ext(ptep, pte, ext)
 *Set a level 2 translation table entry.
 *- ptep  - pointer to level 2 translation table entry
 *          (hardware version is stored at +2048 bytes)
 *- pte   - PTE value to store
 *- ext   - value for extended PTE bits
 */
cfi_debug_info_begin awos_arch_set_pte_ext
    str     r1, [r0]
    bic     r3, r1, #0x000003f0
    bic     r3, r3, #PTE_TYPE_MASK
    orr     r3, r3, r2
    orr     r3, r3, #PTE_EXT_AP0 | 2

    tst     r1, #1 << 4
    orrne   r3, r3, #PTE_EXT_TEX(1)

    eor     r1, r1, #L_PTE_DIRTY
    tst     r1, #L_PTE_RDONLY | L_PTE_DIRTY
    orrne   r3, r3, #PTE_EXT_APX

    tst     r1, #L_PTE_USER
    orrne   r3, r3, #PTE_EXT_AP1

    tst     r1, #L_PTE_XN
    orrne   r3, r3, #PTE_EXT_XN

    tst     r1, #L_PTE_YOUNG
    tstne   r1, #L_PTE_VALID
    eorne   r1, r1, #L_PTE_NONE
    tstne   r1, #L_PTE_NONE
    moveq   r3, #0

    str     r3, [r0, #2048]!
    nop
    mcr     p15, 0, r0, c7, c10, 1         @ flush_pte
    bx      lr
cfi_debug_info_end awos_arch_set_pte_ext

cfi_debug_info_begin awos_arch_tlb_invalidate_all
    dsb
    mov     r0, #0
    mcr     p15, 0, r0, c8, c5, 0    @ I-TLB invalidation
    mcr     p15, 0, r0, c8, c6, 0    @ D-TLB invalidation
    mcr     p15, 0, r0, c8, c7, 0    @ Unified-TLB invalidation
    dsb
    isb
    bx      lr
cfi_debug_info_end awos_arch_tlb_invalidate_all

//void awos_arch_flush_kern_tlb_range(uint32_t start, uint32_t end);
cfi_debug_info_begin awos_arch_flush_kern_tlb_range
   dsb     ish
   lsr     r0, r0, #12
   lsr     r1, r1, #12
   lsl     r0, r0, #12
   lsl     r1, r1, #12

1:
#ifdef CONFIG_SMP
   mcr     p15, 0, r0, c8, c3, 1
#else
   mcr     p15, 0, r0, c8, c7, 1
#endif
   add     r0, r0, #4096   @ 0x1000
   cmp     r0, r1
   bcc     1b
   dsb     ish
   isb     sy
   bx      lr
cfi_debug_info_end awos_arch_flush_kern_tlb_range

   .end
