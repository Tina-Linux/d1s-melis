/*
 *  arch/arm/include/asm/pgtable.h
 *
 *  Copyright (C) 1995-2002 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef _ASMARM_PGTABLE_H
#define _ASMARM_PGTABLE_H

#include <arch.h>
#include "page_2nd_level.h"

/*
 *   - extended small page/tiny page
 */
#define PTE_EXT_XN              (_AT(pteval_t, 1) << 0)         /* v6 */
#define PTE_EXT_AP_MASK         (_AT(pteval_t, 3) << 4)
#define PTE_EXT_AP0             (_AT(pteval_t, 1) << 4)
#define PTE_EXT_AP1             (_AT(pteval_t, 2) << 4)
#define PTE_EXT_AP_UNO_SRO      (_AT(pteval_t, 0) << 4)
#define PTE_EXT_AP_UNO_SRW      (PTE_EXT_AP0)
#define PTE_EXT_AP_URO_SRW      (PTE_EXT_AP1)
#define PTE_EXT_AP_URW_SRW      (PTE_EXT_AP1|PTE_EXT_AP0)
#define PTE_EXT_TEX(x)          (_AT(pteval_t, (x)) << 6)       /* v5 */
#define PTE_EXT_APX             (_AT(pteval_t, 1) << 9)         /* v6 */
#define PTE_EXT_COHERENT        (_AT(pteval_t, 1) << 9)         /* XScale3 */
#define PTE_EXT_SHARED          (_AT(pteval_t, 1) << 10)        /* v6 */
#define PTE_EXT_NG              (_AT(pteval_t, 1) << 11)        /* v6 */

//#define VMALLOC_OFFSET    (8*1024*1024)
//#define VMALLOC_START     (((unsigned long)high_memory + VMALLOC_OFFSET) & ~(VMALLOC_OFFSET-1))
//#define VMALLOC_END       0xff800000UL

#define LIBRARY_TEXT_START  0x0c000000
#define TASK_SIZE               0xffffffff

#ifndef __ASSEMBLY__
extern void __pte_error(const char *file, int line, pte_t);
extern void __pmd_error(const char *file, int line, pmd_t);
extern void __pgd_error(const char *file, int line, pgd_t);

//#define pte_ERROR(pte)        __pte_error(__FILE__, __LINE__, pte)
//#define pmd_ERROR(pmd)        __pmd_error(__FILE__, __LINE__, pmd)
//#define pgd_ERROR(pgd)        __pgd_error(__FILE__, __LINE__, pgd)

/*
 * This is the lowest virtual address we can permit any user space
 * mapping to be mapped at.  This is particularly important for
 * non-high vector CPUs.
 */
#define FIRST_USER_ADDRESS  (PAGE_SIZE * 2)

/*
 * Use TASK_SIZE as the ceiling argument for free_pgtables() and
 * free_pgd_range() to avoid freeing the modules pmd when LPAE is enabled (pmd
 * page shared between user and kernel).
 */
#ifdef CONFIG_ARM_LPAE
#define USER_PGTABLES_CEILING   TASK_SIZE
#endif

/*
 * The pgprot_* and protection_map entries will be fixed up in runtime
 * to include the cachable and bufferable bits based on memory policy,
 * as well as any architecture dependent bits like global/ASID and SMP
 * shared mapping bits.
 */
#define _L_PTE_DEFAULT  L_PTE_PRESENT | L_PTE_YOUNG

extern pgprot_t     pgprot_user;
extern pgprot_t     pgprot_kernel;
extern pgprot_t     pgprot_hyp_device;
extern pgprot_t     pgprot_s2;
extern pgprot_t     pgprot_s2_device;

#define _MOD_PROT(p, b) __pgprot(pgprot_val(p) | (b))

#define PAGE_NONE       _MOD_PROT(pgprot_user, L_PTE_XN | L_PTE_RDONLY | L_PTE_NONE)
#define PAGE_SHARED     _MOD_PROT(pgprot_user, L_PTE_USER | L_PTE_XN)
#define PAGE_SHARED_EXEC    _MOD_PROT(pgprot_user, L_PTE_USER)
#define PAGE_COPY       _MOD_PROT(pgprot_user, L_PTE_USER | L_PTE_RDONLY | L_PTE_XN)
#define PAGE_COPY_EXEC      _MOD_PROT(pgprot_user, L_PTE_USER | L_PTE_RDONLY)
#define PAGE_READONLY       _MOD_PROT(pgprot_user, L_PTE_USER | L_PTE_RDONLY | L_PTE_XN)
#define PAGE_READONLY_EXEC  _MOD_PROT(pgprot_user, L_PTE_USER | L_PTE_RDONLY)
#define PAGE_KERNEL     _MOD_PROT(pgprot_kernel, L_PTE_XN)
#define PAGE_KERNEL_EXEC    pgprot_kernel
#define PAGE_HYP        _MOD_PROT(pgprot_kernel, L_PTE_HYP | L_PTE_XN)
#define PAGE_HYP_EXEC       _MOD_PROT(pgprot_kernel, L_PTE_HYP | L_PTE_RDONLY)
#define PAGE_HYP_RO     _MOD_PROT(pgprot_kernel, L_PTE_HYP | L_PTE_RDONLY | L_PTE_XN)
#define PAGE_HYP_DEVICE     _MOD_PROT(pgprot_hyp_device, L_PTE_HYP)
#define PAGE_S2         _MOD_PROT(pgprot_s2, L_PTE_S2_RDONLY)
#define PAGE_S2_DEVICE      _MOD_PROT(pgprot_s2_device, L_PTE_S2_RDONLY)

#define __PAGE_NONE     __pgprot(_L_PTE_DEFAULT | L_PTE_RDONLY | L_PTE_XN | L_PTE_NONE)
#define __PAGE_SHARED       __pgprot(_L_PTE_DEFAULT | L_PTE_USER | L_PTE_XN)
#define __PAGE_SHARED_EXEC  __pgprot(_L_PTE_DEFAULT | L_PTE_USER)
#define __PAGE_COPY     __pgprot(_L_PTE_DEFAULT | L_PTE_USER | L_PTE_RDONLY | L_PTE_XN)
#define __PAGE_COPY_EXEC    __pgprot(_L_PTE_DEFAULT | L_PTE_USER | L_PTE_RDONLY)
#define __PAGE_READONLY     __pgprot(_L_PTE_DEFAULT | L_PTE_USER | L_PTE_RDONLY | L_PTE_XN)
#define __PAGE_READONLY_EXEC    __pgprot(_L_PTE_DEFAULT | L_PTE_USER | L_PTE_RDONLY)

#define __pgprot_modify(prot,mask,bits)     \
    __pgprot((pgprot_val(prot) & ~(mask)) | (bits))

#define pgprot_noncached(prot) \
    __pgprot_modify(prot, L_PTE_MT_MASK, L_PTE_MT_UNCACHED)

#define pgprot_writecombine(prot) \
    __pgprot_modify(prot, L_PTE_MT_MASK, L_PTE_MT_BUFFERABLE)

#define pgprot_stronglyordered(prot) \
    __pgprot_modify(prot, L_PTE_MT_MASK, L_PTE_MT_UNCACHED)

#ifdef CONFIG_ARM_DMA_MEM_BUFFERABLE
#define pgprot_dmacoherent(prot) \
    __pgprot_modify(prot, L_PTE_MT_MASK, L_PTE_MT_BUFFERABLE | L_PTE_XN)
#define __HAVE_PHYS_MEM_ACCESS_PROT
struct file;
extern pgprot_t phys_mem_access_prot(struct file *file, unsigned long pfn,
                                     unsigned long size, pgprot_t vma_prot);
#else
#define pgprot_dmacoherent(prot) \
    __pgprot_modify(prot, L_PTE_MT_MASK, L_PTE_MT_UNCACHED | L_PTE_XN)
#endif

#endif /* __ASSEMBLY__ */

/*
 * The table below defines the page protection levels that we insert into our
 * Linux page table version.  These get translated into the best that the
 * architecture can perform.  Note that on most ARM hardware:
 *  1) We cannot do execute protection
 *  2) If we could do execute protection, then read is implied
 *  3) write implies read permissions
 */
#define __P000  __PAGE_NONE
#define __P001  __PAGE_READONLY
#define __P010  __PAGE_COPY
#define __P011  __PAGE_COPY
#define __P100  __PAGE_READONLY_EXEC
#define __P101  __PAGE_READONLY_EXEC
#define __P110  __PAGE_COPY_EXEC
#define __P111  __PAGE_COPY_EXEC

#define __S000  __PAGE_NONE
#define __S001  __PAGE_READONLY
#define __S010  __PAGE_SHARED
#define __S011  __PAGE_SHARED
#define __S100  __PAGE_READONLY_EXEC
#define __S101  __PAGE_READONLY_EXEC
#define __S110  __PAGE_SHARED_EXEC
#define __S111  __PAGE_SHARED_EXEC

#ifndef __ASSEMBLY__
/*
 * ZERO_PAGE is a global shared page that is always zero: used
 * for zero-mapped memory areas etc..
 */
extern struct page *empty_zero_page;
#define ZERO_PAGE(vaddr)    (empty_zero_page)

//extern pgd_t swapper_pg_dir[PTRS_PER_PGD];
/* to find an entry in a page-table-directory */
#define pgd_index(addr)     ((addr) >> PGDIR_SHIFT)

#define pgd_offset(mm, addr) (swapper_pg_dirs + pgd_index(addr))

/* to find an entry in a kernel page-table-directory */
#define pgd_offset_k(addr)  pgd_offset(NULL, addr)

#define PHYS_MASK           (~0UL)
#define pmd_none(pmd)       (!pmd_val(pmd))
#define __va(phyaddr)       ((void*)((uint32_t)(phyaddr) + ((CONFIG_DRAM_VIRTBASE) - (CONFIG_DRAM_PHYBASE))))
#define __pa(viraddr)       ((uint32_t)((uint32_t)(viraddr) - ((CONFIG_DRAM_VIRTBASE) - (CONFIG_DRAM_PHYBASE))))

static inline pte_t *pmd_page_vaddr(pmd_t pmd)
{
    return __va(pmd_val(pmd) & PHYS_MASK & (s32)PAGE_MASK);
}

#define pfn_to_page(pfn)    __va(pfn << PAGE_SHIFT)
#define pmd_page(pmd)       pfn_to_page(__phys_to_pfn(pmd_val(pmd) & PHYS_MASK))

#ifndef CONFIG_HIGHPTE
#define __pte_map(pmd)      pmd_page_vaddr(*(pmd))
#define __pte_unmap(pte)    do { } while (0)
#else
#define __pte_map(pmd)      (pte_t *)kmap_atomic(pmd_page(*(pmd)))
#define __pte_unmap(pte)    kunmap_atomic(pte)
#endif

#define pte_index(addr)     (((addr) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))
#define pte_offset_kernel(pmd,addr) (pmd_page_vaddr(*(pmd)) + pte_index(addr))
#define pte_offset_map(pmd,addr)    (__pte_map(pmd) + pte_index(addr))
#define pte_unmap(pte)      __pte_unmap(pte)
#define pte_pfn(pte)        ((pte_val(pte) & PHYS_MASK) >> PAGE_SHIFT)
#define pfn_pte(pfn,prot)   __pte(pfn | pgprot_val(prot))
#define pte_page(pte)       pfn_to_page(pte_pfn(pte))
#define mk_pte(page,prot)   pfn_pte(__pa(page), prot)
#define pte_clear(mm,addr,ptep) set_pte_ext(ptep, __pte(0), 0)
#define pte_isset(pte, val) ((uint32_t)(val) == (val) ? pte_val(pte) & (val) \
                             : !!(pte_val(pte) & (val)))
#define pte_isclear(pte, val)   (!(pte_val(pte) & (val)))
#define pte_none(pte)       (!pte_val(pte))
#define pte_present(pte)    (pte_isset((pte), L_PTE_PRESENT))
#define pte_valid(pte)      (pte_isset((pte), L_PTE_VALID))
#define pte_accessible(mm, pte) (mm_tlb_flush_pending(mm) ? pte_present(pte) : pte_valid(pte))
#define pte_write(pte)      (pte_isclear((pte), L_PTE_RDONLY))
#define pte_dirty(pte)      (pte_isset((pte), L_PTE_DIRTY))
#define pte_young(pte)      (pte_isset((pte), L_PTE_YOUNG))
#define pte_exec(pte)       (pte_isclear((pte), L_PTE_XN))

#define pte_valid_user(pte) \
    (pte_valid(pte) && pte_isset((pte), L_PTE_USER) && pte_young(pte))

void awos_arch_flush_kern_dcache_area(void *addr, unsigned long size);
void awos_arch_flush_icache_all(void);
static inline void __sync_icache_dcache(pte_t pteval)
{
    void *addr;
    unsigned long pfn;

    pfn = pte_pfn(pteval);
    addr = pfn_to_page(pfn);

    awos_arch_flush_kern_dcache_area(addr, PAGE_SIZE);
    awos_arch_flush_icache_all();
}

static inline pte_t clear_pte_bit(pte_t pte, pgprot_t prot)
{
    pte_val(pte) &= ~pgprot_val(prot);
    return pte;
}

static inline pte_t set_pte_bit(pte_t pte, pgprot_t prot)
{
    pte_val(pte) |= pgprot_val(prot);
    return pte;
}

static inline pte_t pte_wrprotect(pte_t pte)
{
    return set_pte_bit(pte, __pgprot(L_PTE_RDONLY));
}

static inline pte_t pte_mkwrite(pte_t pte)
{
    return clear_pte_bit(pte, __pgprot(L_PTE_RDONLY));
}

static inline pte_t pte_mkclean(pte_t pte)
{
    return clear_pte_bit(pte, __pgprot(L_PTE_DIRTY));
}

static inline pte_t pte_mkdirty(pte_t pte)
{
    return set_pte_bit(pte, __pgprot(L_PTE_DIRTY));
}

static inline pte_t pte_mkold(pte_t pte)
{
    return clear_pte_bit(pte, __pgprot(L_PTE_YOUNG));
}

static inline pte_t pte_mkyoung(pte_t pte)
{
    return set_pte_bit(pte, __pgprot(L_PTE_YOUNG));
}

static inline pte_t pte_mkexec(pte_t pte)
{
    return clear_pte_bit(pte, __pgprot(L_PTE_XN));
}

static inline pte_t pte_mknexec(pte_t pte)
{
    return set_pte_bit(pte, __pgprot(L_PTE_XN));
}

static inline pte_t pte_modify(pte_t pte, pgprot_t newprot)
{
    const pteval_t mask = L_PTE_XN | L_PTE_RDONLY | L_PTE_USER |
                          L_PTE_NONE | L_PTE_VALID;
    pte_val(pte) = (pte_val(pte) & ~mask) | (pgprot_val(newprot) & mask);
    return pte;
}

/*
 * Encode and decode a swap entry.  Swap entries are stored in the Linux
 * page tables as follows:
 *
 *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
 *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *   <--------------- offset ------------------------> < type -> 0 0
 *
 * This gives us up to 31 swap files and 128GB per swap file.  Note that
 * the offset field is always non-zero.
 */
#define __SWP_TYPE_SHIFT    2
#define __SWP_TYPE_BITS     5
#define __SWP_TYPE_MASK     ((1 << __SWP_TYPE_BITS) - 1)
#define __SWP_OFFSET_SHIFT  (__SWP_TYPE_BITS + __SWP_TYPE_SHIFT)

#define __swp_type(x)       (((x).val >> __SWP_TYPE_SHIFT) & __SWP_TYPE_MASK)
#define __swp_offset(x)     ((x).val >> __SWP_OFFSET_SHIFT)
#define __swp_entry(type,offset) ((swp_entry_t) { ((type) << __SWP_TYPE_SHIFT) | ((offset) << __SWP_OFFSET_SHIFT) })

#define __pte_to_swp_entry(pte) ((swp_entry_t) { pte_val(pte) })
#define __swp_entry_to_pte(swp) ((pte_t) { (swp).val })

/*
 * It is an error for the kernel to have more swap files than we can
 * encode in the PTEs.  This ensures that we know when MAX_SWAPFILES
 * is increased beyond what we presently support.
 */
#define MAX_SWAPFILES_CHECK() BUILD_BUG_ON(MAX_SWAPFILES_SHIFT > __SWP_TYPE_BITS)

/* Needs to be defined here and not in linux/mm.h, as it is arch dependent */
/* FIXME: this is not correct */
#define kern_addr_valid(addr)   (1)

//#include <asm-generic/pgtable.h>

/*
 * We provide our own arch_get_unmapped_area to cope with VIPT caches.
 */
#define HAVE_ARCH_UNMAPPED_AREA
#define HAVE_ARCH_UNMAPPED_AREA_TOPDOWN

#define pgtable_cache_init() do { } while (0)

static inline int pgd_none(pgd_t pgd)
{
    return 0;
}
static inline int pgd_bad(pgd_t pgd)
{
    return 0;
}
static inline int pgd_present(pgd_t pgd)
{
    return 1;
}
static inline void pgd_clear(pgd_t *pgd)        { }

static inline pud_t *pud_offset(pgd_t *pgd, unsigned long address)
{
    return (pud_t *)pgd;
}

static inline int __pud_alloc(void *mm, pgd_t *pgd,
                              unsigned long address)
{
    return 0;
}

static inline int __pmd_alloc(void *mm, pud_t *pud,
                              unsigned long address)
{
    return 0;
}
static inline pud_t *pud_alloc(void *mm, pgd_t *pgd, unsigned long address)
{
    return (unlikely(pgd_none(*pgd)) && __pud_alloc(mm, pgd, address)) ?
           NULL : pud_offset(pgd, address);
}

static inline pmd_t *pmd_alloc(void *mm, pud_t *pud, unsigned long address)
{
    return (unlikely(pud_none(*pud)) && __pmd_alloc(mm, pud, address)) ?
           NULL : pmd_offset(pud, address);
}

#define PUD_SHIFT       PGDIR_SHIFT
#define PTRS_PER_PUD    1
#define PUD_SIZE        (1UL << PUD_SHIFT)
#define PUD_MASK        (~(PUD_SIZE-1))
/*
 * When walking page tables, get the address of the next boundary,
 * or the end address of the range if that comes earlier.  Although no
 * vma end wraps to 0, rounded up __boundary may wrap to 0 throughout.
 */

#define pgd_addr_end(addr, end)                                         \
    ({      unsigned long __boundary = ((addr) + PGDIR_SIZE) & PGDIR_MASK;  \
        (__boundary - 1 < (end) - 1)? __boundary: (end);                \
    })

#ifndef pud_addr_end
#define pud_addr_end(addr, end)                                         \
    ({      unsigned long __boundary = ((addr) + PUD_SIZE) & PUD_MASK;      \
        (__boundary - 1 < (end) - 1)? __boundary: (end);                \
    })
#endif

#ifndef pmd_addr_end
#define pmd_addr_end(addr, end)                                         \
    ({      unsigned long __boundary = ((addr) + PMD_SIZE) & PMD_MASK;      \
        (__boundary - 1 < (end) - 1)? __boundary: (end);                \
    })
#endif

#endif /* !__ASSEMBLY__ */

#define pte_alloc_kernel(pmd, address)                  \
    ((unlikely(pmd_none(*(pmd))) && __pte_alloc_kernel(pmd, address))? \
     NULL: pte_offset_kernel(pmd, address))

static inline int pud_set_huge(pud_t *pud, phys_addr_t addr, pgprot_t prot)
{
    return 0;
}
static inline int pmd_set_huge(pmd_t *pmd, phys_addr_t addr, pgprot_t prot)
{
    return 0;
}
static inline int pud_clear_huge(pud_t *pud)
{
    return 0;
}
static inline int pmd_clear_huge(pmd_t *pmd)
{
    return 0;
}
static inline int pud_free_pmd_page(pud_t *pud)
{
    return 0;
}
static inline int pmd_free_pte_page(pmd_t *pmd)
{
    return 0;
}

static inline void clean_pte_table(pte_t *pte)
{
    awos_arch_mems_clean_dcache_region((unsigned long)(pte + PTE_HWTABLE_PTRS), PTE_HWTABLE_SIZE);
}

static inline void __pmd_populate(pmd_t *pmdp, phys_addr_t pte,
                                  pmdval_t prot)
{
    pmdval_t pmdval = (pte + PTE_HWTABLE_OFF) | prot;
    pmdp[0] = __pmd(pmdval);
    pmdp[1] = __pmd(pmdval + 256 * sizeof(pte_t));
    flush_pmd_entry(pmdp);
}

/*
 * Hardware page table definitions.
 *
 * + Level 1 descriptor (PMD)
 *   - common
 */
#define PMD_TYPE_MASK           (_AT(pmdval_t, 3) << 0)
#define PMD_TYPE_FAULT          (_AT(pmdval_t, 0) << 0)
#define PMD_TYPE_TABLE          (_AT(pmdval_t, 1) << 0)
#define PMD_TYPE_SECT           (_AT(pmdval_t, 2) << 0)
#define PMD_PXNTABLE            (_AT(pmdval_t, 1) << 2)     /* v7 */
#define PMD_BIT4                (_AT(pmdval_t, 1) << 4)
#define PMD_DOMAIN(x)           (_AT(pmdval_t, (x)) << 5)
#define PMD_DOMAIN_MASK         PMD_DOMAIN(0x0f)
#define PMD_PROTECTION          (_AT(pmdval_t, 1) << 9)         /* v5 */

#define DOMAIN_KERNEL           (DOMAIN0 >> 5)
#define _PAGE_USER_TABLE        (PMD_TYPE_TABLE | PMD_BIT4 | PMD_DOMAIN(DOMAIN_USER))
#define _PAGE_KERNEL_TABLE      (PMD_TYPE_TABLE | PMD_BIT4 | PMD_DOMAIN(DOMAIN_KERNEL))

#define NUMA_NO_NODE            (-1)

//128M vmalloc area, from [0xe8000000, 0xf0000000).
#define VMALLOC_START           (0xe8000000)
#define VMALLOC_END             (0xf0000000)
/*
 * Populate the pmdp entry with a pointer to the pte.  This pmd is part
 * of the mm address space.
 *
 * Ensure that we always set both PMD entries.
 */
static inline void pmd_populate_kernel(void *mm, pmd_t *pmdp, pte_t *ptep)
{
    /*
    ¦* The pmd must be loaded with the physical address of the PTE table
    ¦*/
    __pmd_populate(pmdp, __pa(ptep), _PAGE_KERNEL_TABLE);
}

#define offset_in_page(p)       ((unsigned long)(p) & ~PAGE_MASK)

static inline int is_power_of_2(unsigned long n)
{
    return (n != 0 && ((n & (n - 1)) == 0));
}

#endif /* _ASMARM_PGTABLE_H */
