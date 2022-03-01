/*
 * ===========================================================================================
 *
 *       Filename:  page_2nd_level.h
 *
 *    Description:  type definiton of pagetable for 2nd level.
 *
 *        Version:  Melis3.0
 *         Create:  2019-10-29 16:41:19
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *                  porting from linux.
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-06-11 17:29:42
 *
 * ===========================================================================================
 */
#ifndef PAGE_2ND_LEVEL_H
#define PAGE_2ND_LEVEL_H

#ifndef __ASSEMBLY__
#include <stdbool.h>
#include <typedef.h>
#include <rtthread.h>
#include <common/list.h>
#include <rtdef.h>
#include <stdio.h>
#include "mmu.h"

typedef struct module_alloc_info
{
    struct list_head    list;
    unsigned long       start;
    unsigned long       end;
    void                *pages;
} module_alloc_info_t;

typedef uint32_t pteval_t;
typedef uint32_t pmdval_t;
typedef uint32_t phys_addr_t;
#endif

#ifdef __ASSEMBLY__
#define _AC(X,Y)        X
#define _AT(T,X)        X
#else
#define __AC(X,Y)       (X##Y)
#define _AC(X,Y)        __AC(X,Y)
#define _AT(T,X)        ((T)(X))
typedef unsigned int    gfp_t;
#endif

#undef STRICT_MM_TYPECHECKS
#ifdef STRICT_MM_TYPECHECKS
/*
 * These are used to make use of C type-checking..
 */
typedef struct
{
    pteval_t pte;
} pte_t;

typedef struct
{
    pmdval_t pmd;
} pmd_t;

typedef struct
{
    pmdval_t pgd[2];
} pgd_t;

typedef struct
{
    pteval_t pgprot;
} pgprot_t;

#define pte_val(x)      ((x).pte)
#define pmd_val(x)      ((x).pmd)
#define pgd_val(x)      ((x).pgd[0])
#define pgprot_val(x)   ((x).pgprot)

#define __pte(x)        ((pte_t) { (x) } )
#define __pmd(x)        ((pmd_t) { (x) } )
#define __pgprot(x)     ((pgprot_t) { (x) } )

#else
/*
 * .. while these make it easier on the compiler
 */
#ifndef __ASSEMBLY__
typedef pteval_t pte_t;
typedef pmdval_t pmd_t;
typedef pmdval_t pgd_t[2];
typedef pteval_t pgprot_t;
#endif

#define pte_val(x)      (x)
#define pmd_val(x)      (x)
#define pgd_val(x)  ((x)[0])
#define pgprot_val(x)   (x)

#define pud_val(x)      (pgd_val((x).pgd))

#define __pte(x)        (x)
#define __pmd(x)        (x)
#define __pgprot(x)     (x)

#endif /* STRICT_MM_TYPECHECKS */

#ifndef __ASSEMBLY__
// pgd_t are collapse to pgd.
typedef struct
{
    pgd_t pgd;
} pud_t;
#endif

#define swapper_pg_dirs  ((pgd_t*)CONFIG_DRAM_VIRTBASE)

/* PAGE_SHIFT determines the page size */
#define PAGE_SHIFT              12
#define PAGE_SIZE               (_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK               (~((1 << PAGE_SHIFT) - 1))

#define __ALIGN_KERNEL_MASK(x, mask)    (((x) + (mask)) & ~(mask))
#define __ALIGN_KERNEL(x, a)            __ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)

#ifdef ALIGN
#undef ALIGN
#endif

#define ALIGN(x, a)                     __ALIGN_KERNEL((x), (a))

/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr)        ALIGN(addr, PAGE_SIZE)
#define IS_ALIGNED(x, a)                (((x) & ((typeof(x))(a) - 1)) == 0)
/* test whether an address (unsigned long or pointer) is aligned to PAGE_SIZE */
#define PAGE_ALIGNED(addr)      IS_ALIGNED((unsigned long)(addr), PAGE_SIZE)

/* bits in flags of vmalloc's vm_struct below */
#define VM_IOREMAP              0x00000001      /* ioremap() and friends */
#define VM_ALLOC                0x00000002      /* vmalloc() */
#define VM_MAP                  0x00000004      /* vmap()ed pages */
#define VM_USERMAP              0x00000008      /* suitable for remap_vmalloc_range */
#define VM_UNINITIALIZED        0x00000020      /* vm_struct is not fully initialized */
#define VM_NO_GUARD             0x00000040      /* don't add guard page */
#define VM_KASAN                0x00000080      /* has allocated kasan shadow memory */

#define BUG() do { \
        printf("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__); \
        while(1); \
    } while (0)

#define BUG_ON(condition)       do { if (unlikely(condition)) BUG(); } while (0)
#define WARN_ON(condition)  ({                                                    \
        int __ret_warn_on = !!(condition);                            \
        unlikely(__ret_warn_on);                                      \
    })
#define WARN(condition, format...) ({                                   \
        int __ret_warn_on = !!(condition);                              \
        if(__ret_warn_on)                                               \
            printf(format);                                             \
        unlikely(__ret_warn_on);                                        \
    })
#define WARN_ONCE(condition, format...) ({                      \
        static bool __warned;                                  \
        int __ret_warn_once = !!(condition);                    \
        \
        if (unlikely(__ret_warn_once && !__warned)) {           \
            __warned = true;                                \
            WARN(1, format);                                \
        }                                                       \
        unlikely(__ret_warn_once);                              \
    })
#define in_interrupt(...)       rt_interrupt_get_nest()
#define in_nmi(...)             (0)

/*
 * Hardware-wise, we have a two level page table structure, where the first
 * level has 4096 entries, and the second level has 256 entries.  Each entry
 * is one 32-bit word.  Most of the bits in the second level entry are used
 * by hardware, and there aren't any "accessed" and "dirty" bits.
 *
 * Linux on the other hand has a three level page table structure, which can
 * be wrapped to fit a two level page table structure easily - using the PGD
 * and PTE only.  However, Linux also expects one "PTE" table per page, and
 * at least a "dirty" bit.
 *
 * Therefore, we tweak the implementation slightly - we tell Linux that we
 * have 2048 entries in the first level, each of which is 8 bytes (iow, two
 * hardware pointers to the second level.)  The second level contains two
 * hardware PTE tables arranged contiguously, preceded by Linux versions
 * which contain the state information Linux needs.  We, therefore, end up
 * with 512 entries in the "PTE" level.
 *
 * This leads to the page tables having the following layout:
 *
 *    pgd             pte
 * |        |
 * +--------+
 * |        |       +------------+ +0
 * +- - - - +       | Linux pt 0 |
 * |        |       +------------+ +1024
 * +--------+ +0    | Linux pt 1 |
 * |        |-----> +------------+ +2048
 * +- - - - + +4    |  h/w pt 0  |
 * |        |-----> +------------+ +3072
 * +--------+ +8    |  h/w pt 1  |
 * |        |       +------------+ +4096
 *
 * See L_PTE_xxx below for definitions of bits in the "Linux pt", and
 * PTE_xxx for definitions of bits appearing in the "h/w pt".
 *
 * PMD_xxx definitions refer to bits in the first level page table.
 *
 * The "dirty" bit is emulated by only granting hardware write permission
 * iff the page is marked "writable" and "dirty" in the Linux PTE.  This
 * means that a write to a clean page will cause a permission fault, and
 * the Linux MM layer will mark the page dirty via handle_pte_fault().
 * For the hardware to notice the permission change, the TLB entry must
 * be flushed, and ptep_set_access_flags() does that for us.
 *
 * The "accessed" or "young" bit is emulated by a similar method; we only
 * allow accesses to the page if the "young" bit is set.  Accesses to the
 * page will cause a fault, and handle_pte_fault() will set the young bit
 * for us as long as the page is marked present in the corresponding Linux
 * PTE entry.  Again, ptep_set_access_flags() will ensure that the TLB is
 * up to date.
 *
 * However, when the "young" bit is cleared, we deny access to the page
 * by clearing the hardware PTE.  Currently Linux does not flush the TLB
 * for us in this case, which means the TLB will retain the transation
 * until either the TLB entry is evicted under pressure, or a context
 * switch which changes the user space mapping occurs.
 */

#define PTRS_PER_PTE        512
#define PTRS_PER_PMD        1
#define PTRS_PER_PGD        2048

#define PTE_HWTABLE_PTRS    (PTRS_PER_PTE)
#define PTE_HWTABLE_OFF     (PTE_HWTABLE_PTRS * sizeof(pte_t))
#define PTE_HWTABLE_SIZE    (PTRS_PER_PTE * sizeof(uint32_t))

/*
 * PMD_SHIFT determines the size of the area a second-level page table can map
 * PGDIR_SHIFT determines what a third-level page table entry can map
 */
#define PMD_SHIFT       21
#define PGDIR_SHIFT     21

#define PMD_SIZE        (1UL << PMD_SHIFT)
#define PMD_MASK        (~(PMD_SIZE-1))
#define PGDIR_SIZE      (1UL << PGDIR_SHIFT)
#define PGDIR_MASK      (~(PGDIR_SIZE-1))

/*
 * section address mask and size definitions.
 */
#define SECTION_SHIFT       20
#define SECTION_SIZE        (1UL << SECTION_SHIFT)
#define SECTION_MASK        (~(SECTION_SIZE-1))

/*
 * ARMv6 supersection address mask and size definitions.
 */
#define SUPERSECTION_SHIFT  24
#define SUPERSECTION_SIZE   (1UL << SUPERSECTION_SHIFT)
#define SUPERSECTION_MASK   (~(SUPERSECTION_SIZE-1))

#define USER_PTRS_PER_PGD   (TASK_SIZE / PGDIR_SIZE)

/*
 * "Linux" PTE definitions.
 *
 * We keep two sets of PTEs - the hardware and the linux version.
 * This allows greater flexibility in the way we map the Linux bits
 * onto the hardware tables, and allows us to have YOUNG and DIRTY
 * bits.
 *
 * The PTE table pointer refers to the hardware entries; the "Linux"
 * entries are stored 1024 bytes below.
 */
#define L_PTE_VALID     (_AT(pteval_t, 1) << 0)     /* Valid */
#define L_PTE_PRESENT   (_AT(pteval_t, 1) << 0)
#define L_PTE_YOUNG     (_AT(pteval_t, 1) << 1)
#define L_PTE_DIRTY     (_AT(pteval_t, 1) << 6)
#define L_PTE_RDONLY    (_AT(pteval_t, 1) << 7)
#define L_PTE_USER      (_AT(pteval_t, 1) << 8)
#define L_PTE_XN        (_AT(pteval_t, 1) << 9)
#define L_PTE_SHARED    (_AT(pteval_t, 1) << 10)    /* shared(v6), coherent(xsc3) */
#define L_PTE_NONE      (_AT(pteval_t, 1) << 11)

/*
 * These are the memory types, defined to be compatible with
 * pre-ARMv6 CPUs cacheable and bufferable bits: n/a,n/a,C,B
 * ARMv6+ without TEX remapping, they are a table index.
 * ARMv6+ with TEX remapping, they correspond to n/a,TEX(0),C,B
 *
 * MT type      Pre-ARMv6   ARMv6+ type / cacheable status
 * UNCACHED     Uncached    Strongly ordered
 * BUFFERABLE       Bufferable  Normal memory / non-cacheable
 * WRITETHROUGH     Writethrough    Normal memory / write through
 * WRITEBACK        Writeback   Normal memory / write back, read alloc
 * MINICACHE        Minicache   N/A
 * WRITEALLOC       Writeback   Normal memory / write back, write alloc
 * DEV_SHARED       Uncached    Device memory (shared)
 * DEV_NONSHARED    Uncached    Device memory (non-shared)
 * DEV_WC       Bufferable  Normal memory / non-cacheable
 * DEV_CACHED       Writeback   Normal memory / write back, read alloc
 * VECTORS      Variable    Normal memory / variable
 *
 * All normal memory mappings have the following properties:
 * - reads can be repeated with no side effects
 * - repeated reads return the last value written
 * - reads can fetch additional locations without side effects
 * - writes can be repeated (in certain cases) with no side effects
 * - writes can be merged before accessing the target
 * - unaligned accesses can be supported
 *
 * All device mappings have the following properties:
 * - no access speculation
 * - no repetition (eg, on return from an exception)
 * - number, order and size of accesses are maintained
 * - unaligned accesses are "unpredictable"
 */
#define L_PTE_MT_UNCACHED   (_AT(pteval_t, 0x00) << 2)  /* 0000 */
#define L_PTE_MT_BUFFERABLE (_AT(pteval_t, 0x01) << 2)  /* 0001 */
#define L_PTE_MT_WRITETHROUGH   (_AT(pteval_t, 0x02) << 2)  /* 0010 */
#define L_PTE_MT_WRITEBACK  (_AT(pteval_t, 0x03) << 2)  /* 0011 */
#define L_PTE_MT_MINICACHE  (_AT(pteval_t, 0x06) << 2)  /* 0110 (sa1100, xscale) */
#define L_PTE_MT_WRITEALLOC (_AT(pteval_t, 0x07) << 2)  /* 0111 */
#define L_PTE_MT_DEV_SHARED (_AT(pteval_t, 0x04) << 2)  /* 0100 */
#define L_PTE_MT_DEV_NONSHARED  (_AT(pteval_t, 0x0c) << 2)  /* 1100 */
#define L_PTE_MT_DEV_WC     (_AT(pteval_t, 0x09) << 2)  /* 1001 */
#define L_PTE_MT_DEV_CACHED (_AT(pteval_t, 0x0b) << 2)  /* 1011 */
#define L_PTE_MT_VECTORS    (_AT(pteval_t, 0x0f) << 2)  /* 1111 */
#define L_PTE_MT_MASK       (_AT(pteval_t, 0x0f) << 2)

#ifndef __ASSEMBLY__

#define pte_ERROR(pte)          do { printf("%s line %d, pte error.\n", __func__, __LINE__); } while(0)
#define pmd_ERROR(pmd)          do { printf("%s line %d, pmd error.\n", __func__, __LINE__); } while(0)
#define pgd_ERROR(pgd)          do { printf("%s line %d, pgd error.\n", __func__, __LINE__); } while(0)
#define pud_ERROR(pud)          do { printf("%s line %d, pud error.\n", __func__, __LINE__); } while(0)

/*
 * The "pud_xxx()" functions here are trivial when the pmd is folded into
 * the pud: the pud entry is never bad, always exists, and can't be set or
 * cleared.
 */
#define pud_none(pud)       (0)
#define pud_bad(pud)        (0)
#define pud_present(pud)    (1)
#define pud_clear(pudp)     do { } while (0)
#define set_pud(pud,pudp)   do { } while (0)

void awos_arch_flush_pmd_entry(uint32_t pmd);
void awos_arch_clean_pmd_entry(uint32_t pmd);
void awos_arch_flush_kern_cache_all(void);
#define flush_pmd_entry(pmd)    awos_arch_flush_pmd_entry((uint32_t)pmd)
#define clean_pmd_entry(pmd)    awos_arch_clean_pmd_entry((uint32_t)pmd)
#define flush_cache_all         awos_arch_flush_kern_cache_all

static inline pmd_t *pmd_offset(pud_t *pud, unsigned long addr)
{
    return (pmd_t *)pud;
}

#define pmd_large(pmd)      (pmd_val(pmd) & 2)
#define pmd_bad(pmd)        (pmd_val(pmd) & 2)
#define pmd_present(pmd)    (pmd_val(pmd))

#define copy_pmd(pmdpd,pmdps)       \
    do {                \
        pmdpd[0] = pmdps[0];    \
        pmdpd[1] = pmdps[1];    \
        flush_pmd_entry(pmdpd); \
    } while (0)

#define pmd_clear(pmdp)         \
    do {                \
        pmdp[0] = __pmd(0); \
        pmdp[1] = __pmd(0); \
        clean_pmd_entry(pmdp);  \
    } while (0)

/* we don't need complex calculations here as the pmd is folded into the pgd */
#define pmd_addr_end(addr,end) (end)

void awos_arch_set_pte_ext(void *ptep, uint32_t pte, uint32_t ext);
#define set_pte_ext(ptep,pte,ext)  awos_arch_set_pte_ext(ptep,pte,ext)
#define pte_special(pte)    (0)
static inline pte_t pte_mkspecial(pte_t pte)
{
    return pte;
}

/*
 * We don't have huge page support for short descriptors, for the moment
 * define empty stubs for use by pin_page_for_write.
 */
#define pmd_hugewillfault(pmd)  (0)
#define pmd_thp_or_huge(pmd)    (0)
#endif

/* Are two types/vars the same type (ignoring qualifiers)? */
#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#define BUILD_BUG_ON_ZERO(e) (0)
/* &a[0] degrades to a pointer: a different type from an array */
#define __must_be_array(a)      BUILD_BUG_ON_ZERO(__same_type((a), &(a)[0]))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))

#ifndef pr_fmt
#define pr_fmt(fmt) fmt
#endif

#endif  /*PAGE_2ND_LEVEL_H*/
