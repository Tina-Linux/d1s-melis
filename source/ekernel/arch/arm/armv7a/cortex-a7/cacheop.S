/*
 * ===========================================================================================
 *
 *       Filename:  cachop.S
 *
 *    Description:  cache op for armv7-a 
 *
 *        Version:  Melis3.0 
 *         Create:  2019-10-31 10:31:51
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-12-16 14:26:06
 *
 * ===========================================================================================
 */

#include <cfi.h>
#include <cpu.h>
#include <kconfig.h>

/*
 * void awos_arch_flush_kern_dcache_area(void *addr, size_t size)
 * Ensure that the data held in the page kaddr is written back
 * to the page in question.
 * - addr  - kernel address
 * - size  - region size.
 */
cfi_debug_info_begin awos_arch_flush_kern_dcache_area
    mrc     p15,0, r3, c0, c0, 1
    lsr     r3, r3, #16         
    and     r3, r3, #15         
    mov     r2, #4              
    lsl     r2, r2, r3          
    add     r1, r0, r1          
    sub     r3, r2, #1          
    bic     r0, r0, r3          

#ifdef CONFIG_SMP
    dsb     sy                  
#else
    nop
#endif

1:
    mcr     p15, 0, r0, c7, c14, 1              @ Clean and invalidate data cache line by MVA to PoC
    add     r0, r0, r2          
    cmp     r0, r1              
    blo     1b
    dsb     st                  
    bx      lr   
cfi_debug_info_end awos_arch_flush_kern_dcache_area
/*
 *
 * void awos_arch_flush_icache_all(void)
 * Flush the whole I-cache. 
 * Registers: r0 is zero.
 */
cfi_debug_info_begin awos_arch_flush_icache_all
   mov     r0, #0
#ifdef CONFIG_SMP
   mcr     p15, 0, r0, c7, c1, 0                @ Instruction cache invalidate all PoU, IS.
#else
   mcr     p15, 0, r0, c7, c5, 0                @ Instruction cache invalidate all PoU
#endif
   mcr     p15, 0, r0, c7, c5, 6                @ Invalidate entire branch predictor array.
   bx      lr
cfi_debug_info_end awos_arch_flush_icache_all

/*
 *
 *  Flush the whole D-cache.
 *  Corrupted registers: r0-r7, r9-r11 (r6 only in Thumb mode).
 *  flush sequence: L1->L2->......
 */
cfi_debug_info_begin __v7_flush_dcache_all
   dmb     sy                        @ ensure ordering with previous memory accesses
   mrc     p15, 1, r0, c0, c0, 1     @ read clidr
   lsr     r3, r0, #23               @ move LoC into position
   ands    r3, r3, #14               @ extract LoC*2 from clidr
   beq     finished                  @ if loc is 0, l2 cache not implemented on the processor, then no need to clean
start_flush_levels:
   mov     r10, #0                   @ start clean at cache level 0
flush_levels:
   add     r2, r10, r10, lsr #1      @ work out 3x current cache level
   mov     r1, r0, lsr r2            @ extract cache type bits from clidr
   and     r1, r1, #7                @ mask of the bits for current cache only
   cmp     r1, #2                    @ see what cache we have at this level
   blt     skip                      @ skip if no cache, or just i-cache

   mrs     r9, cpsr                  @ make cssr&csidr read atomic
   mcr     p15, 2, r10, c0, c0, 0    @ select current cache level in cssr
   isb     sy                        @ isb to sych the new cssr&csidr
   mrc     p15, 1, r1, c0, c0, 0     @ read the new csidr
   msr     cpsr_c, r9                @ recover original cpsr.

   and     r2, r1, #7                @ extract the length of the cache lines
   add     r2, r2, #4                @ add 4 (line length offset)
   movw    r4, #1023    @ 0x3ff      @ 
   ands    r4, r4, r1, lsr #3        @ find maximum number on the way size
   clz     r5, r4                    @ find bit position of way size increment
   movw    r7, #32767   @ 0x7fff     @  
   ands    r7, r7, r1, lsr #13       @ extract max number of the index size
loop1:
   mov     r9, r7                    @ create working copy of max index
loop2:
   orr     fp, r10, r4, lsl r5
   orr     fp, fp, r9, lsl r2
   mcr     p15, 0, fp, c7, c14, 2    @ Clean and invalidate DCache entry (Set/Way)
   subs    r9, r9, #1                @ sets --
   bge     loop2
   subs    r4, r4, #1                @ ways--
   bge     loop1
skip:
   add     r10, r10, #2              @ increment cache number. CSSELR[1:3],L1,L2... 
   cmp     r3, r10                   @ CSSELR[0]: 0-1/D-I Cache selection. 
   bgt     flush_levels              @ Only flush D Cache.
finished:
   mov     r10, #0                   @ switch back to cache level 0
   mcr     p15, 2, r10, c0, c0, 0    @ select current cache level in cssr
   dsb     st
   isb     sy
   bx      lr
cfi_debug_info_end __v7_flush_dcache_all

/*       
 *  void awos_arch_flush_kern_cache_all(void)                    
 *       
 *  Flush the entire cache system.
 *  The data cache flush is now achieved using atomic clean / invalidates
 *  working outwards from L1 cache. This is done using Set/Way based cache
 *  maintenance instructions.
 *  The instruction cache can still be invalidated back to the point of
 *  unification in a single instruction.
 *       
 */ 
cfi_debug_info_begin awos_arch_flush_kern_cache_all
    push    {r4, r5, r7, r9, sl, fp, lr}
    bl      __v7_flush_dcache_all
    mov     r0, #0     

#ifdef CONFIG_SMP
    mcr     p15, 0, r0, c7, c1, 0                   @ Instruction cache invalidate all PoU, IS.
#else
    mcr     p15, 0, r0, c7, c5, 0                   @ Instruction cache invalidate all PoU
#endif
    pop     {r4, r5, r7, r9, sl, fp, lr}
    bx      lr
cfi_debug_info_end awos_arch_flush_kern_cache_all

cfi_debug_info_begin awos_arch_clean_dcache
    push    {r4, r5, r7, r9, sl, fp, lr}
    bl      __v7_flush_dcache_all
    pop     {r4, r5, r7, r9, sl, fp, lr}
    bx      lr
cfi_debug_info_end awos_arch_clean_dcache

cfi_debug_info_begin awos_arch_clean_flush_dcache
    push    {r4, r5, r7, r9, sl, fp, lr}
    bl      __v7_flush_dcache_all
    pop     {r4, r5, r7, r9, sl, fp, lr}
    bx      lr
cfi_debug_info_end awos_arch_clean_flush_dcache

cfi_debug_info_begin awos_arch_clean_flush_cache
    b       .
cfi_debug_info_end awos_arch_clean_flush_cache

#invalidate icache operation.
cfi_debug_info_begin awos_arch_mems_flush_icache_region
    b       .
cfi_debug_info_end awos_arch_mems_flush_icache_region

cfi_debug_info_begin awos_arch_flush_icache
    mov     r0, #0
#ifdef CONFIG_SMP
    mcr     p15, 0, r0, c7, c1, 0                   @ Instruction cache invalidate all PoU, IS.
#else
    mcr     p15, 0, r0, c7, c5, 0
#endif
    bx      lr
cfi_debug_info_end awos_arch_flush_icache

cfi_debug_info_begin awos_arch_flush_dcache
    push    {r4, r5, r7, r9, sl, fp, lr}
    bl      __v7_flush_dcache_all
    pop     {r4, r5, r7, r9, sl, fp, lr}
    bx      lr
cfi_debug_info_end awos_arch_flush_dcache

#invalid total whole cache system.
cfi_debug_info_begin awos_arch_flush_cache
    b       awos_arch_flush_kern_cache_all
cfi_debug_info_end awos_arch_flush_cache

#invalidate dcache operation.
cfi_debug_info_begin awos_arch_mems_flush_dcache_region
    add     r1, r0, r1
    b       awos_arch_mems_dma_inv_range
cfi_debug_info_end awos_arch_mems_flush_dcache_region

#invalidate dcache operation.
cfi_debug_info_begin awos_arch_mems_flush_cache_region
    add     r1, r0, r1
    b       awos_arch_mems_dma_inv_range
cfi_debug_info_end awos_arch_mems_flush_cache_region

/*                           
 * void awos_arch_mems_clean_dcache_region(void *start, ssize_t size)
 * - start   - virtual start address of region
 * - size    - start + size is virtual end address of region
 */           
cfi_debug_info_begin awos_arch_mems_clean_dcache_region
    mrc     p15, 0, r3, c0, c0, 1
    lsr     r3, r3, #16 
    and     r3, r3, #15 
    mov     r2, #4     
    lsl     r2, r2, r3 
    
    add     r1, r0, r1
    sub     r3, r2, #1 
    bic     r0, r0, r3 

1:
    mcr     p15, 0, r0, c7, c10, 1                        @ Clean data cache line to PoC by MVA.
    add     r0, r0, r2 
    cmp     r0, r1     
    bcc     1b
    dsb     st         
    bx      lr 
cfi_debug_info_end awos_arch_mems_clean_dcache_region

/*
 * void awos_arch_mems_clean_flush_dcache_region(void *addr, size_t size)
 *
 * Ensure that the data held in the page kaddr is written back
 * to the page in question.
 *
 * - addr  - kernel address
 * - size  - region size
 * this function would write the dirty data back to memory. including
 * the margin area of [start, end].
 */
cfi_debug_info_begin awos_arch_mems_clean_flush_dcache_region
    mrc     p15, 0x0, r3, c0, c0, 1
    lsr     r3, r3, #16
    and     r3, r3, #15
    mov     r2, #4
    lsl     r2, r2, r3

    add     r1, r0, r1 
    sub     r3, r2, #1
    bic     r0, r0, r3

1:
    mcr     p15, 0x0, r0, c7, c14, 1             @ Clean and invalidate data cache line by MVA to PoC.
    add     r0, r0, r2
    cmp     r0, r1
    bcc     1b
    dsb     st
    bx      lr
cfi_debug_info_end awos_arch_mems_clean_flush_dcache_region

/*
 * void awos_arch_mems_clean_flush_cache_region(void *addr, size_t size)
 *
 * Ensure that the data held in the page kaddr is written back
 * to the page in question.
 *
 * - addr  - kernel address
 * - size  - region size
 * this function would write the dirty data back to memory. including
 * the margin area of [start, end].
 * THIS FUNCTION WOULD INVALIDATE ICACHE LINE specified. 
 */
cfi_debug_info_begin awos_arch_mems_clean_flush_cache_region
    mrc     p15, 0x0, r3, c0, c0, 1
    lsr     r3, r3, #16
    and     r3, r3, #15
    mov     r2, #4
    lsl     r2, r2, r3

    add     r1, r0, r1 
    sub     r3, r2, #1
    bic     r0, r0, r3

1:
    mcr     p15, 0x0, r0, c7, c14, 1             @ Clean and invalidate data cache line by MVA to PoC.
    mcr     p15, 0x0, r0, c7, c5,  1             @ Invalidate instruction cache by MVA to PoU.
    add     r0, r0, r2
    cmp     r0, r1
    bcc     1b
    dsb     st
    bx      lr
cfi_debug_info_end awos_arch_mems_clean_flush_cache_region

/*
 * awos_arch_mems_dma_inv_range(void *start, void *end);
 *
 * Invalidate the data cache within the specified region; we will
 * be performing a DMA operation in this region and we want to
 * purge old data in the cache.
 *
 * - start   - virtual start address of region
 * - end     - virtual end address of region
 * this function would purge old data in the cache, except the margin data
 * of 'start' and 'end' in area [start, end]
 */
cfi_debug_info_begin awos_arch_mems_dma_inv_range
    mrc     p15, 0x0, r3, c0, c0, 1
    lsr     r3, r3, #16
    and     r3, r3, #15
    mov     r2, #4
    lsl     r2, r2, r3
    sub     r3, r2, #1
    tst     r0, r3
    bic     r0, r0, r3
    mcrne   p15, 0x0, r0, c7, c14, 1             @ Clean and invalidate data cache line by MVA to PoC. 
    tst     r1, r3
    bic     r1, r1, r3
    mcrne   p15, 0x0, r1, c7, c14, 1             @ Clean and invalidate data cache line by MVA to PoC. 

1:
    mcr     p15, 0x0, r0, c7, c6, 1              @ Invalidate data cache line by MVA to PoC.
    add     r0, r0, r2
    cmp     r0, r1
    bcc     1b
    dsb     st
    bx      lr
cfi_debug_info_end awos_arch_mems_dma_inv_range

/*
 *
 * v7_dma_clean_range(start,end)
 * - start   - virtual start address of region
 * - end     - virtual end address of region
 */
cfi_debug_info_begin v7_dma_clean_range
    mrc     p15, 0, r3, c0, c0, 1
    lsr     r3, r3, #16
    and     r3, r3, #15
    mov     r2, #4
    lsl     r2, r2, r3
    sub     r3, r2, #1
    bic     r0, r0, r3
1:
    mcr     p15, 0, r0, c7, c10, 1               @ Clean data cache line to PoC by MVA.
    add     r0, r0, r2
    cmp     r0, r1
    blo     1b
    dsb     st
    bx      lr
cfi_debug_info_end v7_dma_clean_range

/*
 * v7_dma_flush_range(start,end)
 * - start   - virtual start address of region
 * - end     - virtual end address of region
 */
cfi_debug_info_begin v7_dma_flush_range
    mrc     p15, 0, r3, c0, c0, 1
    lsr     r3, r3, #16
    and     r3, r3, #15
    mov     r2, #4
    lsl     r2, r2, r3
    sub     r3, r2, #1
    bic     r0, r0, r3
1:
    mcr     p15, 0, r0, c7, c14, 1           @ Clean and invalidate data cache line by MVA to PoC
    add     r0, r0, r2
    cmp     r0, r1
    bcc     1b
    dsb     st
    bx      lr
cfi_debug_info_end v7_dma_flush_range

/*
 * dma_map_area(start, size, dir)
 * - start - kernel virtual start address
 * - size  - size of region
 * - dir   - DMA direction
 */
cfi_debug_info_begin  dma_map_area
    b       v7_dma_map_area
cfi_debug_info_end  dma_map_area

/*
 *  dma_unmap_area(start, size, dir)
 *  - start - kernel virtual start address
 *  - size  - size of region
 *  - dir   - DMA direction
 */
cfi_debug_info_begin  dma_unmap_area
    b       v7_dma_unmap_area
cfi_debug_info_end  dma_unmap_area

/*
 * dma_map_area(start, size, dir)
 * - start - kernel virtual start address
 * - size  - size of region
 * - dir   - DMA direction
 */
cfi_debug_info_begin v7_dma_map_area
    add     r1, r1, r0
    teq     r2, #DMA_FROM_DEVICE
    beq     awos_arch_mems_dma_inv_range
    b       v7_dma_clean_range
cfi_debug_info_end v7_dma_map_area

/*
 *  dma_unmap_area(start, size, dir)
 *  - start - kernel virtual start address
 *  - size  - size of region
 *  - dir   - DMA direction
 */
cfi_debug_info_begin v7_dma_unmap_area
    add     r1, r1, r0
    teq     r2, #DMA_TO_DEVICE 
    bne     awos_arch_mems_dma_inv_range
    bx      lr
cfi_debug_info_end v7_dma_unmap_area

/*         
 *  v7_coherent_range(unsigned long start, unsigned long end)
 *     
 *  Ensure that the I and D caches are coherent within specified
 *  region.  This is typically used when code has been written to
 *  a memory region, and will be executed.
 *     
 *  - start   - virtual start address of region
 *  - end     - virtual end address of region
 *     
 *  It is assumed that:
 *  - the Icache does not read data from the write buffer
 */ 
cfi_debug_info_begin v7_coherent_range
    mrc     p15, 0, r3, c0, c0, 1
    lsr     r3, r3, #16
    and     r3, r3, #15
    mov     r2, #4
    lsl     r2, r2, r3
    sub     r3, r2, #1
    bic     ip, r0, r3
1:
    mcr     p15, 0, ip, c7, c11, 1
    add     ip, ip, r2
    cmp     ip, r1
    bcc     1b
    dsb     ishst
    
    @ get icache line size.
    mrc     p15, 0, r3, c0, c0, 1
    and     r3, r3, #15
    mov     r2, #4
    lsl     r2, r2, r3

    sub     r3, r2, #1
    bic     ip, r0, r3
2:
    mcr     p15, 0, ip, c7, c5, 1
    add     ip, ip, r2
    cmp     ip, r1
    bcc     2b
    mov     r0, #0

#ifdef CONFIG_SMP
    mcr     p15, 0, r0, c7, c1, 6       @ Branch predictor invalidate all IS
#else
    mcr     p15, 0, r0, c7, c5, 6       @ Branch predictor invalidate all
#endif

    dsb     ishst
    isb     sy
    bx      lr
    mvn     r0, #13
    bx      lr
cfi_debug_info_end v7_coherent_range

/*
 * The secondary kernel init calls v7_flush_dcache_all before it enables
 * the L1; however, the L1 comes out of reset in an undefined state, so
 * the clean + invalidate performed by v7_flush_dcache_all causes a bunch
 * of cache lines with uninitialized data and uninitialized tags to get
 * written out to memory, which does really unpleasant things to the main
 * processor.  We fix this by performing an invalidate, rather than a
 * clean + invalidate, before jumping into the kernel.
 *     
 * This function is cloned from arch/arm/mach-tegra/headsmp.S, and needs
 * to be called for both secondary cores startup and primary core resume
 * procedures.
 */    
//void awos_arch_v7_invalidate_l1(void);
cfi_debug_info_begin awos_arch_v7_invalidate_l1
    mov     r0, #0
    mcr     p15, 2, r0, c0, c0, 0
    mrc     p15, 1, r0, c0, c0, 0
 
    movw    r1, #0x7fff
    and     r2, r1, r0, lsr #13
 
    movw    r1, #0x3ff
 
    and     r3, r1, r0, lsr #3      @ NumWays - 1
    add     r2, r2, #1              @ NumSets
 
    and     r0, r0, #0x7
    add     r0, r0, #4      @ SetShift
 
    clz     r1, r3          @ WayShift
    add     r4, r3, #1      @ NumWays
1:  sub     r2, r2, #1      @ NumSets--
    mov     r3, r4          @ Temp = NumWays
2:  subs    r3, r3, #1      @ Temp--
    mov     r5, r3, lsl r1
    mov     r6, r2, lsl r0
    orr     r5, r5, r6      @ Reg = (Temp<<WayShift)|(NumSets<<SetShift)
    mcr     p15, 0, r5, c7, c6, 2   @ Invalidate DCache single entry (Set/Way)
    bgt     2b
    cmp     r2, #0
    bgt     1b
    dsb     st
    isb
    bx      lr
cfi_debug_info_end awos_arch_v7_invalidate_l1

   .end
