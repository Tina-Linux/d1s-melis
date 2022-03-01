#ifndef _ASM_RISCV_TLBFLUSH_H
#define _ASM_RISCV_TLBFLUSH_H

static inline void local_flush_tlb_all(void)
{
    __asm__ __volatile__("sfence.vma" : : : "memory");
}

/* Flush one page from local TLB */
static inline void local_flush_tlb_page(unsigned long addr)
{
    __asm__ __volatile__("sfence.vma %0" : : "r"(addr) : "memory");
}

#ifdef CONFIG_SMP
void flush_tlb_all(void);
void flush_tlb_mm(struct mm_struct *mm);
void flush_tlb_page(struct vm_area_struct *vma, unsigned long addr);
void flush_tlb_range(struct vm_area_struct *vma, unsigned long start,
                     unsigned long end);
#else /* CONFIG_SMP */
#define flush_tlb_all() local_flush_tlb_all()
#define flush_tlb_page(vma, addr) local_flush_tlb_page(addr)

static inline void flush_tlb_range(void *vma,
                                   unsigned long start, unsigned long end)
{
    local_flush_tlb_all();
}

#define flush_tlb_mm(mm) flush_tlb_all()
#endif /* CONFIG_SMP */

/* Flush a range of kernel pages */
static inline void flush_tlb_kernel_range(unsigned long start, unsigned long end)
{
    start &= PAGE_MASK;
    end   += PAGE_SIZE - 1;
    end   &= PAGE_MASK;

    while (start < end)
    {
        __asm__ __volatile__("sfence.vma %0" : : "r"(start) : "memory");
        start += PAGE_SIZE;
    }
}

#endif /* _ASM_RISCV_TLBFLUSH_H */
