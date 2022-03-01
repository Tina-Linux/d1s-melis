/*
 * ===========================================================================================
 *
 *       Filename:  module.c
 *
 *    Description:  dynamic memory implemetation.
 *
 *        Version:  Melis3.0
 *         Create:  2020-07-07 15:19:59
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-07-21 12:03:08
 *
 * ===========================================================================================
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "page.h"
#include "pgtable.h"
#include <rtthread.h>
#include <barrier.h>
#include <sbi.h>
#include <arch.h>
#include <log.h>

static pmd_t system_pmd_tbl[PTRS_PER_PMD] __aligned(PAGE_SIZE);
// max alloc 2M for module.
static unsigned long totalram_pages = (2 * 1024 * 1024) / PAGE_SIZE;

void flush_icache_all(void)
{
    sbi_remote_fence_i(NULL);
}

void flush_icache_pte(pte_t pte)
{
    flush_icache_all();
}

static unsigned long __get_vm_area_node(unsigned long size)
{
    unsigned int total, used, max_used, aval;

    BUG_ON(in_interrupt());

    size = ALIGN_TO_PAGE(size, PAGE_SIZE);
    if (unlikely(!size))
    {
        return 0;
    }

    rt_memory_info(&total, &used, &max_used);
    aval = total - used;

    if (aval < size)
    {
        return 0;
    }

    return size;
}

static void clear_vm_uninitialized_flag(int vmok)
{
    barrier();
    wmb();
}

static void kmemleak_alloc(const void *ptr, size_t size, int min_count)
{
    barrier();
    wmb();
}

static inline pmd_t *pmd_alloc_one(void *mm, unsigned long addr)
{
    // valid check, dynamic area only allowed in range [0xc0000000, 0xffffffff]
    if ((addr < 0xc0000000) || (addr > 0xffffffff))
    {
        __err("fatal error, dynamic area range should be [0xc0000000, 0xffffffff].");
        return NULL;
    }

    // one pmd is enough for 1G map.
    return system_pmd_tbl;
}

int __pmd_alloc(void *mm, pud_t *pud, unsigned long address)
{
    unsigned long cpu_sr;

    pmd_t *new = pmd_alloc_one(mm, address);
    if (!new)
    {
        __err("fatal error, alloc pmd failed.");
        return -1;
    }

    barrier();
    wmb();

    cpu_sr = awos_arch_lock_irq();

    if (!pud_present(*pud))
    {
        pud_populate(mm, pud, new);
    }
    else if (pud_page_vaddr(*pud) != (unsigned long)system_pmd_tbl)
    {
        __err("pud corruptted, not match the pmd items.");
        return -1;
    }

    awos_arch_unlock_irq(cpu_sr);

    return 0;
}

static inline pte_t *pte_alloc_one_kernel(void *mm)
{
    pte_t *pte;

    pte = (pte_t *)rt_page_alloc(1); ;
    if (pte)
    {
        memset(pte, 0x00, PAGE_SIZE);
    }

    barrier();
    wmb();
    return pte;
}

int __pte_alloc_kernel(pmd_t *pmd)
{
    unsigned long cpu_sr;

    pte_t *new = pte_alloc_one_kernel(NULL);
    if (!new)
    {
        return -1;
    }

    barrier();
    wmb();

    cpu_sr = awos_arch_lock_irq();
    if (likely(pmd_none(*pmd)))     /* Has another populated it ? */
    {
        pmd_populate_kernel(NULL, pmd, new);
        new = NULL;
    }
    awos_arch_unlock_irq(cpu_sr);

    if (new)
    {
        rt_page_free(new, 1);
    }

    return 0;
}


static int vmap_pte_range(pmd_t *pmd, unsigned long addr, unsigned long end, pgprot_t prot, void *pages, int *nr)
{
    pte_t *pte;

    /*
     * nr is a running index into the array which helps higher level
     * callers keep track of where we're up to.
     */
    pte = pte_alloc_kernel(pmd, addr);
    if (!pte)
    {
        return -1;
    }

    do
    {
        if (pte_none(*pte))/*if pte is NULL, set the the page address as new*/
        {
#ifdef CONFIG_SLAB_DEBUG
            void *page = (void *)rt_page_alloc(PAGE_SIZE/4096);
#else
            void *page = (void *)rt_malloc(PAGE_SIZE);
#endif
            if(RT_NULL == page)
            {
                __log("vmap_pte_ent request failed!\r\n");
                return -1;
            }
            if (!page)
            {
                return -1;
            }
            set_pte_at(NULL, addr, pte, mk_pte(page, prot));
        }
        else
        {
            __inf("pte ent has been set before\r\n");
        }
        (*nr)++;
    } while (pte++, addr += PAGE_SIZE, addr != end);
    return 0;
}

static int vmap_pmd_range(pud_t *pud, unsigned long addr,
                          unsigned long end, pgprot_t prot, void *pages, int *nr)
{
    pmd_t *pmd;
    unsigned long next;

    pmd = pmd_alloc(NULL, pud, addr);
    if (!pmd)
    {
        return -1;
    }

    do
    {
        next = pmd_addr_end(addr, end);
        if (vmap_pte_range(pmd, addr, next, prot, pages, nr))
        {
            return -1;
        }
    } while (pmd++, addr = next, addr != end);

    return 0;
}

static int vmap_pud_range(p4d_t *p4d, unsigned long addr,
                          unsigned long end, pgprot_t prot, struct page **pages, int *nr)
{
    pud_t *pud;
    unsigned long next;

    pud = pud_alloc(NULL, p4d, addr);
    if (!pud)
    {
        return -1;
    }

    do
    {
        next = pud_addr_end(addr, end);
        if (vmap_pmd_range(pud, addr, next, prot, pages, nr))
        {
            return -1;
        }
    } while (pud++, addr = next, addr != end);
    return 0;
}


static int vmap_p4d_range(pgd_t *pgd, unsigned long addr, unsigned long end, pgprot_t prot, struct page **pages, int *nr)
{
    p4d_t *p4d;
    unsigned long next;

    // no p4d, so p4d just equal pgd.
    p4d = p4d_alloc(NULL, pgd, addr);
    if (!p4d)
    {
        return -1;
    }

    do
    {
        next = p4d_addr_end(addr, end);
        if (vmap_pud_range(p4d, addr, next, prot, pages, nr))
        {
            return -1;
        }
    } while (p4d++, addr = next, addr != end);
    return 0;
}


/*
 * Set up page tables in kva (addr, end). The ptes shall have prot "prot", and
 * will have pfns corresponding to the "pages" array.
 *
 * Ie. pte at addr+N*PAGE_SIZE shall point to pfn corresponding to pages[N]
 */
static int vmap_page_range_noflush(unsigned long start, unsigned long end, pgprot_t prot, void *pages)
{
    pgd_t *pgd;
    unsigned long next;
    unsigned long addr = start;
    int err = 0;
    int nr = 0;

    BUG_ON(addr >= end);
    pgd = pgd_offset_k(addr);
    do
    {
        next = pgd_addr_end(addr, end);
        err = vmap_p4d_range(pgd, addr, next, prot, pages, &nr);
        if (err)
        {
            return err;
        }
    } while (pgd++, addr = next, addr != end);

    return nr;
}


/*
 * flush_cache_vmap() is used when creating mappings (eg, via vmap,
 * vmalloc, ioremap etc) in kernel space for pages.  On non-VIPT
 * caches, since the direct-mappings of these pages may contain cached
 * data, we need to do a full cache flush to ensure that writebacks
 * don't corrupt data placed into these pages via the new mappings.
 */
static inline void flush_cache_vmap(unsigned long start, unsigned long end)
{
    barrier();
    wmb();
    rmb();
    mb();
}

static int vmap_page_range(unsigned long start, unsigned long end,
                           pgprot_t prot, void *pages)
{
    int ret;

    ret = vmap_page_range_noflush(start, end, prot, pages);
    flush_cache_vmap(start, end);
    return ret;
}

int map_vm_area(unsigned long size, pgprot_t prot, void *pages, unsigned long target_vmaddr)
{
    unsigned long addr = (unsigned long)target_vmaddr;
    unsigned long end = addr + size;
    int err;

    err = vmap_page_range(addr, end, prot, pages);

    return err > 0 ? 0 : err;
}

static void *__vmalloc_area_node(unsigned long area_sz, pgprot_t prot, unsigned long target_vmaddr)
{
    void *pages = NULL;
    if (map_vm_area(area_sz, prot, pages, target_vmaddr))
    {
        __err("falure to map.");
        goto fail;
    }

    return (void *)target_vmaddr;

fail:
    return NULL;
}

// just one node, which is the rtos system memory.
void *__vmalloc_node_range(unsigned long start, unsigned long size, pgprot_t prot)
{
    unsigned long area_size;
    void *addr;
    unsigned long real_size = size;

    BUG_ON(!size);
    BUG_ON(offset_in_page(size));

    size = ALIGN_TO_PAGE(size, PAGE_SIZE);
    if (!size || (size >> PAGE_SHIFT) > totalram_pages)
    {
        goto fail;
    }

    area_size = __get_vm_area_node(size);
    if (!area_size)
    {
        goto fail;
    }

    RT_ASSERT(area_size == size);

    addr = __vmalloc_area_node(area_size, prot, start);
    if (!addr)
    {
        return NULL;
    }

    /*
     * In this function, newly allocated vm_struct has VM_UNINITIALIZED
     * flag. It means that vm_struct is not fully initialized.
     * Now, it is fully initialized, so remove this flag here.
     */
    clear_vm_uninitialized_flag(area_size);

    /*
     * A ref_count = 2 is needed because vm_struct allocated in
     * __get_vm_area_node() contains a reference to the virtual address of
     * the vmalloc'ed block.
     */
    kmemleak_alloc(addr, real_size, 2);

    return addr;

fail:
    BUG();
    return NULL;
}

static void *__vmalloc_node(unsigned long vmaddr, unsigned long size, pgprot_t prot)
{
    unsigned long start = vmaddr;

    return __vmalloc_node_range(start, size, prot);
}

static void *__vmalloc_node_flags(unsigned long vmaddr, unsigned long size, pgprot_t prot)
{
    return __vmalloc_node(vmaddr, size, prot);
}

static void *vmalloc_prot(unsigned long vmaddr, unsigned long size, pgprot_t prot)
{
    void *ptr;

    ptr =  __vmalloc_node_flags(vmaddr, size, prot);
    if (ptr != (void *)vmaddr)
    {
        __err("map failure. return.");
        return NULL;
    }

    return ptr;
}

static inline void flush_cache_vunmap(unsigned long start, unsigned long end)
{

}

/*
 * If a p?d_bad entry is found while walking page tables, report
 * the error, before resetting entry to p?d_none.  Usually (but
 * very seldom) called out from the p?d_none_or_clear_bad macros.
 */
void pgd_clear_bad(pgd_t *pgd)
{
    pgd_ERROR(*pgd);
    pgd_clear(pgd);
}

void p4d_clear_bad(p4d_t *p4d)
{
    p4d_ERROR(*p4d);
    p4d_clear(p4d);
}

void pud_clear_bad(pud_t *pud)
{
    pud_ERROR(*pud);
    pud_clear(pud);
}

void pmd_clear_bad(pmd_t *pmd)
{
    pmd_ERROR(*pmd);
    pmd_clear(pmd);
}

static inline int pgd_none_or_clear_bad(pgd_t *pgd)
{
    if (pgd_none(*pgd))
    {
        return 1;
    }
    if (pgd_bad(*pgd))
    {
        pgd_clear_bad(pgd);
        return 1;
    }
    return 0;
}

static inline int p4d_none_or_clear_bad(p4d_t *p4d)
{
    if (p4d_none(*p4d))
    {
        return 1;
    }
    if (unlikely(p4d_bad(*p4d)))
    {
        p4d_clear_bad(p4d);
        return 1;
    }
    return 0;
}

static inline int pud_none_or_clear_bad(pud_t *pud)
{
    if (pud_none(*pud))
    {
        return 1;
    }
    if (unlikely(pud_bad(*pud)))
    {
        pud_clear_bad(pud);
        return 1;
    }
    return 0;
}

static inline int pmd_none_or_clear_bad(pmd_t *pmd)
{
    if (pmd_none(*pmd))
    {
        return 1;
    }
    if (unlikely(pmd_bad(*pmd)))
    {
        pmd_clear_bad(pmd);
        return 1;
    }
    return 0;
}

/*** Page table manipulation functions ***/

static void vunmap_pte_range(pmd_t *pmd, unsigned long addr, unsigned long end)
{
    pte_t *pte;

    pte = pte_offset_kernel(pmd, addr);
    do
    {
        pte_t ptent = ptep_get_and_clear(NULL, addr, pte);
        if (!pte_none(ptent) && !pte_present(ptent))
        {
            __err("fatal pte status.");
            return;
        }
        else if (!pte_none(ptent))
        {
            void *page = pte_page(ptent);
#ifdef CONFIG_SLAB_DEBUG
            rt_page_free(page, 1);
#else
            rt_free(page);
#endif
        }
    } while (pte++, addr += PAGE_SIZE, addr != end);
}


static void vunmap_pmd_range(pud_t *pud, unsigned long addr, unsigned long end)
{
    pmd_t *pmd;
    unsigned long next;

    pmd = pmd_offset(pud, addr);
    do
    {
        next = pmd_addr_end(addr, end);
        if (pmd_clear_huge(pmd))
        {
            continue;
        }
        if (pmd_none_or_clear_bad(pmd))
        {
            continue;
        }
        vunmap_pte_range(pmd, addr, next);
    } while (pmd++, addr = next, addr != end);
}

static void vunmap_pud_range(p4d_t *p4d, unsigned long addr, unsigned long end)
{
    pud_t *pud;
    unsigned long next;

    pud = pud_offset(p4d, addr);
    do
    {
        next = pud_addr_end(addr, end);
        if (pud_clear_huge(pud))
        {
            continue;
        }
        if (pud_none_or_clear_bad(pud))
        {
            continue;
        }
        vunmap_pmd_range(pud, addr, next);
    } while (pud++, addr = next, addr != end);
}


static void vunmap_p4d_range(pgd_t *pgd, unsigned long addr, unsigned long end)
{
    p4d_t *p4d;
    unsigned long next;

    p4d = p4d_offset(pgd, addr);
    do
    {
        next = p4d_addr_end(addr, end);
        if (p4d_clear_huge(p4d))
        {
            continue;
        }

        if (p4d_none_or_clear_bad(p4d))
        {
            continue;
        }
        vunmap_pud_range(p4d, addr, next);
    } while (p4d++, addr = next, addr != end);
}


static void vunmap_page_range(unsigned long addr, unsigned long end)
{
    pgd_t *pgd;
    unsigned long next;

    BUG_ON(addr >= end);
    pgd = pgd_offset_k(addr);
    do
    {
        next = pgd_addr_end(addr, end);
        if (pgd_none_or_clear_bad(pgd))
        {
            continue;
        }
        vunmap_p4d_range(pgd, addr, next);
    } while (pgd++, addr = next, addr != end);
}

static void unmap_vmap_area(unsigned long vmaddr, unsigned long free_sz)
{
    vunmap_page_range(vmaddr, vmaddr + free_sz);
}

static void free_unmap_vmap_area(unsigned long vmaddr, unsigned long free_sz)
{
    flush_cache_vunmap(vmaddr, free_sz);
    unmap_vmap_area(vmaddr, free_sz);
    flush_tlb_kernel_range(vmaddr, vmaddr + free_sz);
}

static void remove_vm_area(unsigned long vmaddr, unsigned long free_sz)
{
    free_unmap_vmap_area(vmaddr, free_sz);
}

static void vmem_remove_mappings(unsigned long vmaddr, unsigned long free_sz)
{
    remove_vm_area(vmaddr, free_sz);

    flush_tlb_kernel_range(vmaddr, vmaddr + free_sz);
    local_flush_tlb_all();
}

static int vmem_unmap(unsigned long start, unsigned long free_sz)
{
    // valid check, dynamic area only allowed in range [0xc0000000, 0xffffffff]
    if ((start < 0xc0000000) || ((start + free_sz) > 0xffffffff))
    {
        __err("fatal error, dynamic area range should be [0xc0000000, 0xffffffff].");
        return -1;
    }

    vmem_remove_mappings(start, free_sz);

    return 0;
}

int module_alloc(void *target_vmaddr, unsigned long map_size, bool executable)
{
    void *ptr;
    pgprot_t prot;

    if (!target_vmaddr || !map_size)
    {
        __err("invalid parameter, return.");
        return -1;
    }

    if (PAGE_ALIGNED(target_vmaddr) == 0)
    {
        __err("mapped address not page aligned, return.");
        return -1;
    }

    map_size = ALIGN_TO_PAGE(map_size, PAGE_SIZE);

    if (executable)
    {
        prot = PAGE_KERNEL_EXEC;
    }
    else
    {
        prot = PAGE_KERNEL;
    }

    // keep atomic of map.
    rt_enter_critical();
    // executeable page for dynamic module.
    ptr = vmalloc_prot((unsigned long)target_vmaddr, map_size, prot);
    rt_exit_critical();

    if (ptr != target_vmaddr)
    {
        __err("map failure. return.");
        return -1;
    }
    return 0;
}

int module_free(void *target_vmaddr, unsigned long free_size)
{
    unsigned long start = (unsigned long)target_vmaddr;
    unsigned long free_sz = free_size;

    if (!target_vmaddr || !free_size)
    {
        __err("invalid parameter, return.");
        return -1;
    }

    if (PAGE_ALIGNED(start) == 0)
    {
        __err("free address not page aligned, return.");
        return -1;
    }

    free_sz = ALIGN_TO_PAGE(free_sz, PAGE_SIZE);

    // keep atomic of map.
    rt_enter_critical();
    vmem_unmap(start, free_sz);
    rt_exit_critical();

    return 0;
}

int32_t awos_arch_vmem_create(uint8_t *virtaddr, uint32_t npage, uint8_t domain)
{
    if (awos_arch_isin_irq() != 0)
    {
        __err("cannot call vmem_create from isr!");
        return -1;
    }

    uint32_t size = ALIGN_TO_PAGE(npage * PAGE_SIZE, PAGE_SIZE);
    module_alloc((void *)virtaddr, size, 1);
    return 0;
}

void awos_arch_vmem_delete(uint8_t *virtaddr, uint32_t npage)
{
    uint32_t   ptenum;
    uint32_t   ptestart;
    unsigned long vmaddr;

    if (awos_arch_isin_irq() != 0)
    {
        __err("cannot call vmem_create from isr!");
        return;
    }

    vmaddr = (unsigned long)virtaddr;
    if (npage == 0)
    {
        __err("0 page number to vm delete");
        return;
    }

    uint32_t size = ALIGN_TO_PAGE(npage * PAGE_SIZE, PAGE_SIZE);
    module_free((void *)virtaddr, size);

    return;
}

