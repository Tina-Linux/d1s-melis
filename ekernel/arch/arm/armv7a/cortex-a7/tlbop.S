/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <cfi.h>
#include <cpu.h>
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
