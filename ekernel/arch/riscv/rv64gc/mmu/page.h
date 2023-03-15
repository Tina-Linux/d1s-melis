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
#ifndef _ASM_RISCV_PAGE_H
#define _ASM_RISCV_PAGE_H

#include "pfn.h"
#include <consts.h>

#define PAGE_SHIFT  (12)
#define PAGE_SIZE   (_AC(1, UL) << PAGE_SHIFT)
#define PAGE_MASK   (~(PAGE_SIZE - 1))

#define IS_ALIGNED(x, a)   (((x) & ((typeof(x))(a) - 1)) == 0)
/* test whether an address (unsigned long or pointer) is aligned to PAGE_SIZE */
#define PAGE_ALIGNED(addr) IS_ALIGNED((unsigned long)(addr), PAGE_SIZE)

#define __ALIGN_KERNEL_MASK(x, mask)    (((x) + (mask)) & ~(mask))
#define __ALIGN_KERNEL(x, a)            __ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define ALIGN_TO_PAGE(x, a)             __ALIGN_KERNEL((x), (a))

#ifndef __ASSEMBLY__
#define BUG() do { \
        printk("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__); \
        while(1); \
    } while (0)
#define BUG_ON(condition)       do { if (unlikely(condition)) BUG(); } while (0)
#define offset_in_page(p)       ((unsigned long)(p) & ~PAGE_MASK)

static inline int is_power_of_2(unsigned long n)
{
    return (n != 0 && ((n & (n - 1)) == 0));
}

#endif

#ifdef CONFIG_64BIT
#define HUGE_MAX_HSTATE     2
#else
#define HUGE_MAX_HSTATE     1
#endif
#define HPAGE_SHIFT     PMD_SHIFT
#define HPAGE_SIZE      (_AC(1, UL) << HPAGE_SHIFT)
#define HPAGE_MASK              (~(HPAGE_SIZE - 1))
#define HUGETLB_PAGE_ORDER      (HPAGE_SHIFT - PAGE_SHIFT)

/*
 * PAGE_OFFSET -- the first address of the first page of memory.
 * When not using MMU this corresponds to the first free page in
 * physical memory (aligned on a page boundary).
 */
#define PAGE_OFFSET     _AC(CONFIG_PAGE_OFFSET, UL)

#define KERN_VIRT_SIZE (-PAGE_OFFSET)

#ifndef __ASSEMBLY__

#define PAGE_UP(addr)   (((addr)+((PAGE_SIZE)-1))&(~((PAGE_SIZE)-1)))
#define PAGE_DOWN(addr) ((addr)&(~((PAGE_SIZE)-1)))

/* align addr on a size boundary - adjust address up/down if needed */
#define _ALIGN_UP(addr, size)   (((addr)+((size)-1))&(~((size)-1)))
#define _ALIGN_DOWN(addr, size) ((addr)&(~((size)-1)))

/* align addr on a size boundary - adjust address up if needed */
#define _ALIGN(addr, size)  _ALIGN_UP(addr, size)

#define clear_page(pgaddr)          memset((pgaddr), 0, PAGE_SIZE)
#define copy_page(to, from)         memcpy((to), (from), PAGE_SIZE)

#define clear_user_page(pgaddr, vaddr, page)    memset((pgaddr), 0, PAGE_SIZE)
#define copy_user_page(vto, vfrom, vaddr, topg) \
    memcpy((vto), (vfrom), PAGE_SIZE)

/*
 * Use struct definitions to apply C type checking
 */

/* Page Global Directory entry */
typedef struct
{
    unsigned long pgd;
} pgd_t;

/* Page Table entry */
typedef struct
{
    unsigned long pte;
} pte_t;

typedef struct
{
    unsigned long pgprot;
} pgprot_t;

typedef struct page *pgtable_t;

#define pte_val(x)  ((x).pte)
#define pgd_val(x)  ((x).pgd)
#define pgprot_val(x)   ((x).pgprot)

#define __pte(x)    ((pte_t) { (x) })
#define __pgd(x)    ((pgd_t) { (x) })
#define __pgprot(x) ((pgprot_t) { (x) })

#ifdef CONFIG_64BIT
#define PTE_FMT "%016lx"
#else
#define PTE_FMT "%08lx"
#endif

// VA==PA on riscv melis port.
#define va_pa_offset        ((0))

#define __pa(x)             ((unsigned long)(x) - va_pa_offset)
#define __va(x)             ((void *)((unsigned long) (x) + va_pa_offset))

#define pfn_to_page(pfn)     __va(pfn << PAGE_SHIFT)
#define phys_to_pfn(phys)   (PFN_DOWN(phys))
#define pfn_to_phys(pfn)    (PFN_PHYS(pfn))

#define virt_to_pfn(vaddr)  (phys_to_pfn(__pa(vaddr)))
#define pfn_to_virt(pfn)    (__va(pfn_to_phys(pfn)))

#define virt_to_page(vaddr) (pfn_to_page(virt_to_pfn(vaddr)))
#define page_to_virt(page)  (pfn_to_virt(page_to_pfn(page)))

#define page_to_phys(page)  (pfn_to_phys(page_to_pfn(page)))
#define page_to_bus(page)   (page_to_phys(page))
#define phys_to_page(paddr) (pfn_to_page(phys_to_pfn(paddr)))

#ifdef CONFIG_FLATMEM
#define pfn_valid(pfn) \
    (((pfn) >= pfn_base) && (((pfn)-pfn_base) < max_mapnr))
#endif

#define ARCH_PFN_OFFSET     (pfn_base)

#endif /* __ASSEMBLY__ */

#define virt_addr_valid(vaddr)  (pfn_valid(virt_to_pfn(vaddr)))

#define VM_DATA_DEFAULT_FLAGS   (VM_READ | VM_WRITE | \
                                 VM_MAYREAD | VM_MAYWRITE | VM_MAYEXEC)

#endif /* _ASM_RISCV_PAGE_H */
