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
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <barrier.h>
#include <aw_list.h>
#include "page_2nd_level.h"
#include "pgtable.h"
#include <debug.h>
#include <log.h>

#include <hal_mem.h>

#define DBG(fmt, ...)     do { printf("%s line %d, "fmt, __func__, __LINE__, ##__VA_ARGS__); } while (0)

static struct list_head root_head = LIST_HEAD_INIT(root_head);
static int __get_vm_area_node(unsigned long size,
                              unsigned long align, unsigned long flags, unsigned long start,
                              unsigned long end, int node, gfp_t gfp_mask, const void *caller)
{
    unsigned int total, used, max_used, aval;

    BUG_ON(in_interrupt());
    size = PAGE_ALIGN(size);
    if (unlikely(!size))
    {
        __err("aligned size is zero. ");
        return 0;
    }

    return size;
}

static void clear_vm_uninitialized_flag(int vmok)
{
    dmb();
    dsb();
    isb();
}

static inline void kmemleak_alloc(const void *ptr, size_t size, int min_count, gfp_t gfp)
{
    return;
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
    flush_cache_all();

    /*
     * set_pte_at() called from vmap_pte_range() does not
     * have a DSB after cleaning the cache line.
     */
    dsb(ishst);
}
/*
 * Allocate one PTE table.
 *
 * This actually allocates two hardware PTE tables, but we wrap this up
 * into one table thus:
 *
 *  +------------+
 *  | Linux pt 0 |
 *  +------------+
 *  | Linux pt 1 |
 *  +------------+
 *  |  h/w pt 0  |
 *  +------------+
 *  |  h/w pt 1  |
 *  +------------+
 */
static inline pte_t *pte_alloc_one_kernel(void *mm, unsigned long addr)
{
    pte_t *pte;

    pte = (pte_t *)rt_page_alloc(1); ;
    if (pte)
    {
        memset(pte, 0x00, PAGE_SIZE);
        clean_pte_table(pte);
    }
    return pte;
}

int __pte_alloc_kernel(pmd_t *pmd, unsigned long address)
{
    pte_t *new = pte_alloc_one_kernel(NULL, address);
    if (!new)
    {
        __err("alloc pte failure.");
        return -1;
    }

    dsb();
    dmb();
    isb();

    if (likely(pmd_none(*pmd)))     /* Has another populated it ? */
    {
        pmd_populate_kernel(NULL, pmd, new);
        new = NULL;
    }

    if (new)
    {
        rt_page_free(new, 1);
        new = NULL;
    }

    return 0;
}

static void set_pte_at(void *mm, unsigned long addr,
                       pte_t *ptep, pte_t pteval)
{
    unsigned long ext = 0;

    __sync_icache_dcache(pteval);

    //did not set the nG bits for kernel shareing, without ASID.
    //all the pagetable pte entry are global for each thread.
    //refer DDI0406C_C_arm_architecture_reference_manual.pdf chapter B3.5.2
    //ext |= PTE_EXT_NG;

    set_pte_ext(ptep, pteval, ext);
}

//final stage of setting MMU table. Set PTE entry by traversing the PTE table.
//pud, pmd are collapse to pgd. 4-level page table map are reduce dimension to
//2-level page table map.
static int vmap_pte_range(pmd_t *pmd, unsigned long addr,
                          unsigned long end, pgprot_t prot, void *pages, int *nr)
{
    pte_t *pte;

    /*
    ¦* nr is a running index into the array which helps higher level
    ¦* callers keep track of where we're up to.
    ¦*/

    pte = pte_alloc_kernel(pmd, addr);
    if (!pte)
    {
        __err("alloc pte failure.");
        return -1;
    }

    //__log("in.");
    do
    {
        void *page = (void *)((uint32_t)pages + *nr * PAGE_SIZE);

        if (WARN_ON(!pte_none(*pte)))
        {
            __err("pte status not right.");
            return -1;
        }
        if (WARN_ON(!page))
        {
            __err("page is null.");
            return -1;
        }
        set_pte_at(NULL, addr, pte, mk_pte(page, prot));
        (*nr)++;
        //DBG("addr = %lx, end = %lx, pte = %p, page %p.\n", addr, end, pte, page);
    } while (pte++, addr += PAGE_SIZE, addr != end);

    //__log("out.");
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
        __err("pmd alloc failure.");
        return -1;
    }

    do
    {
        next = pmd_addr_end(addr, end);
        if (vmap_pte_range(pmd, addr, next, prot, pages, nr))
        {
            __err("pte range map failure.");
            return -1;
        }
    } while (pmd++, addr = next, addr != end);

    return 0;
}

static int vmap_pud_range(pgd_t *pgd, unsigned long addr,
                          unsigned long end, pgprot_t prot, void *pages, int *nr)
{
    pud_t *pud;
    unsigned long next;

    pud = pud_alloc(NULL, pgd, addr);
    if (!pud)
    {
        __err("pud alloc failure.");
        return -1;
    }

    do
    {
        next = pud_addr_end(addr, end);
        if (vmap_pmd_range(pud, addr, next, prot, pages, nr))
        {
            __err("pmd map failure.");
            return -1;
        }
    } while (pud++, addr = next, addr != end);

    return 0;
}

/*
 * Set up page tables in kva (addr, end). The ptes shall have prot "prot", and
 * will have pfns corresponding to the "pages" array.
 *
 * Ie. pte at addr+N*PAGE_SIZE shall point to pfn corresponding to pages[N]
 */
static int vmap_page_range_noflush(unsigned long start, unsigned long end,
                                   pgprot_t prot, void *pages)
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
        err = vmap_pud_range(pgd, addr, next, prot, pages, &nr);
        if (err)
        {
            __err("pud map failure.");
            return err;
        }
    } while (pgd++, addr = next, addr != end);

    return nr;
}

static int vmap_page_range(unsigned long start, unsigned long end,
                           pgprot_t prot, void *pages)
{
    int ret;

    ret = vmap_page_range_noflush(start, end, prot, pages);
    flush_cache_vmap(start, end);
    return ret;
}

int map_vm_area(int size, pgprot_t prot, void *pages, unsigned long target_vmaddr)
{
    unsigned long addr = (unsigned long)target_vmaddr;
    unsigned long end = addr + size;
    int err;

    err = vmap_page_range(addr, end, prot, pages);
    return err > 0 ? 0 : err;
}

static void *__vmalloc_area_node(int area, gfp_t gfp_mask,
                                 pgprot_t prot, int node, unsigned long target_vmaddr)
{
    void *pages;

    pages = rt_page_alloc(area / PAGE_SIZE);
    if (pages == NULL)
    {
        __err("page alloc failure.");
        return NULL;
    }

    memset(pages, 0x00, area);
    if (map_vm_area(area, prot, pages, target_vmaddr))
    {
        __err("mmu map failure.");
        goto fail;
    }

    void add_alloc_info(unsigned long start, unsigned long end, void *pages);
    add_alloc_info((unsigned long)target_vmaddr, (unsigned long)target_vmaddr + area, pages);

    return (void *)target_vmaddr;

fail:
    rt_page_free(pages, area / PAGE_SIZE);
    return NULL;
}

void add_alloc_info(unsigned long start, unsigned long end, void *pages)
{
    module_alloc_info_t *p;

    p = hal_malloc(sizeof(module_alloc_info_t));
    if (!p)
    {
        BUG();
    }

    memset(p, 0x00, sizeof(module_alloc_info_t));
    p->start  = start;
    p->end    = end;
    p->pages  = pages;
    p->npages = (end - start) / PAGE_SIZE;

    INIT_LIST_HEAD(&p->list);
    list_add(&p->list, &root_head);
}

module_alloc_info_t *find_alloc_info(unsigned long start, unsigned long end)
{
    struct list_head    *pos;
    struct list_head    *q;
    module_alloc_info_t *tmp;

    list_for_each_safe(pos, q, &root_head)
    {
        tmp = list_entry(pos, module_alloc_info_t, list);
        if ((tmp->start <= start) && (tmp->end >= end))
        {
            tmp->npages -=  (end - start) / PAGE_SIZE;
            if(tmp->npages == 0)
            {
                list_del(pos);
            }
            return tmp;
        }
    }
    return NULL;
}

// max alloc 2M for module.
static unsigned long totalram_pages = (2 * 1024 * 1024) / PAGE_SIZE;
/**
 *  __vmalloc_node_range  -  allocate virtually contiguous memory
 *  @size:      allocation size
 *  @align:     desired alignment
 *  @start:     vm area range start
 *  @end:       vm area range end
 *  @gfp_mask:  flags for the page level allocator
 *  @prot:      protection mask for the allocated pages
 *  @vm_flags:  additional vm area flags (e.g. %VM_NO_GUARD)
 *  @node:      node to use for allocation or NUMA_NO_NODE
 *  @caller:    caller's return address
 *  @target_vmaddr: the virtual address need map to.
 *
 *  Allocate enough pages to cover @size from the page level
 *  allocator with @gfp_mask flags.  Map them into contiguous
 *  kernel virtual space, using a pagetable protection of @prot.
 */
void *__vmalloc_node_range(unsigned long size, unsigned long align,
                           unsigned long start, unsigned long end, gfp_t gfp_mask,
                           pgprot_t prot, unsigned long vm_flags, int node,
                           const void *caller)
{
    int area;
    void *addr;
    unsigned long real_size = size;

    __log("size 0x%08lx,start = 0x%08lx, end = 0x%08lx.", size, start, end);
    BUG_ON(!size);
    BUG_ON(offset_in_page(size));
    BUG_ON(!is_power_of_2(align));

    size = PAGE_ALIGN(size);
    if (!size || (size >> PAGE_SHIFT) > totalram_pages)
    {
        __err("too much for one single module");
        goto fail;
    }

    area = __get_vm_area_node(size, align, VM_ALLOC | VM_UNINITIALIZED |
                              vm_flags, start, end, node, gfp_mask, caller);
    if (!area)
    {
        __err("area: create vm failure.");
        goto fail;
    }

    addr = __vmalloc_area_node(area, gfp_mask, prot, node, start);
    if (!addr)
    {
        __err("addr: create vm failure.");
        return NULL;
    }

    /*
     * In this function, newly allocated vm_struct has VM_UNINITIALIZED
     * flag. It means that vm_struct is not fully initialized.
     * Now, it is fully initialized, so remove this flag here.
     */
    clear_vm_uninitialized_flag(area);

    /*
     * A ref_count = 2 is needed because vm_struct allocated in
     * __get_vm_area_node() contains a reference to the virtual address of
     * the vmalloc'ed block.
     */
    kmemleak_alloc(addr, real_size, 2, gfp_mask);

    return addr;

fail:
    BUG();
    return NULL;
}

static inline void kmemleak_free(const void *ptr)
{
    return;
}

void *find_vmap_area(const void *addr)
{
    return (void *)1;
}

static inline size_t get_vm_area_size(const void *area)
{
    /*
     *if (!(area->flags & VM_NO_GUARD))
     *    [> return actual size without guard page <]
     *    return area->size - PAGE_SIZE;
     *else
     *    return area->size;
     */

    return 0;
}

static inline void debug_check_no_locks_freed(const void *from, unsigned long len)
{
    return;
}

static inline void debug_check_no_obj_freed(const void *address, unsigned long size)
{
    return;
}

void pgd_clear_bad(pgd_t *pgd)
{
    pgd_ERROR(*pgd);
    pgd_clear(pgd);
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
    if (unlikely(pgd_bad(*pgd)))
    {
        pgd_clear_bad(pgd);
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

static inline pte_t ptep_get_and_clear(void *mm,
                                       unsigned long address,
                                       pte_t *ptep)
{
    pte_t pte = *ptep;
    pte_clear(mm, address, ptep);
    return pte;
}

/*** Page table manipulation functions ***/
static void vunmap_pte_range(pmd_t *pmd, unsigned long addr, unsigned long end)
{
    pte_t *pte;

    pte = pte_offset_kernel(pmd, addr);
    do
    {
        pte_t ptent = ptep_get_and_clear(NULL, addr, pte);
        WARN_ON(!pte_none(ptent) && !pte_present(ptent));
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

static void vunmap_pud_range(pgd_t *pgd, unsigned long addr, unsigned long end)
{
    pud_t *pud;
    unsigned long next;

    pud = pud_offset(pgd, addr);
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
        vunmap_pud_range(pgd, addr, next);
    } while (pgd++, addr = next, addr != end);
}

/*
 * Clear the pagetable entries of a given vmap_area
 */
static void unmap_vmap_area(unsigned long start, unsigned long end)
{
    vunmap_page_range(start, end);
}

void free_vmap_area_noflush(unsigned long start, unsigned long end)
{
    awos_arch_flush_kern_tlb_range(start, end);
}
/*
 * Free and unmap a vmap area, caller ensuring flush_cache_vunmap had been
 * called for the correct range previously.
 */
static void free_unmap_vmap_area_noflush(unsigned long start, unsigned long end)
{
    unmap_vmap_area(start, end);
    free_vmap_area_noflush(start, end);
}

static inline void flush_cache_vunmap(unsigned long start, unsigned long end)
{
    //if (!cache_is_vipt_nonaliasing())
    flush_cache_all();
}
/*
 * Free and unmap a vmap area
 */
static void free_unmap_vmap_area(const void *addr, unsigned long size)
{
    flush_cache_vunmap((unsigned long)addr, (unsigned long)addr + size);
    free_unmap_vmap_area_noflush((unsigned long)addr, (unsigned long)addr + size);
}

void *remove_vm_area(const void *addr, unsigned long size)
{
    //TODO: need add a manager layer with module alloc area.
    //struct vmap_area *va;

    //va = find_vmap_area((unsigned long)addr);
    //if (va && va->flags & VM_VM_AREA)
    {
        //struct vm_struct *vm = va->vm;

        //spin_lock(&vmap_area_lock);
        //va->vm = NULL;
        //va->flags &= ~VM_VM_AREA;
        //spin_unlock(&vmap_area_lock);

        //vmap_debug_free_range(va->va_start, va->va_end);
        //kasan_free_shadow(vm);

        free_unmap_vmap_area(addr, size);

        //return vm;
    }
    return NULL;
}

static void __vunmap(const void *addr, unsigned long size, int deallocate_pages)
{
    void *area;

    if (!addr)
    {
        return;
    }

    if (WARN(!PAGE_ALIGNED(addr), "Trying to vfree() bad address (%p)\n",
             addr))
    {
        return;
    }

    area = find_vmap_area(addr);
    if (unlikely(!area))
    {
        WARN(1, "ERR:Trying to vfree() nonexistent vm area (%p)\n",
             addr);
        return;
    }

    debug_check_no_locks_freed(addr, get_vm_area_size(area));
    debug_check_no_obj_freed(addr, get_vm_area_size(area));

    remove_vm_area(addr, size);

    module_alloc_info_t *module = NULL;
    if (deallocate_pages)
    {
        module = find_alloc_info((uint32_t)addr, (uint32_t)addr + size);
        if (!module)
        {
            BUG();
        }

        rt_page_free((void*)((unsigned long)module->pages + (addr - module->start)), size >> PAGE_SHIFT);
    }

    if (module && module->npages == 0)
    {
        hal_free(module);
    }
    return;
}

/**
 *      __vmalloc_node  -  allocate virtually contiguous memory
 *      @size:          allocation size
 *      @align:         desired alignment
 *      @gfp_mask:      flags for the page level allocator
 *      @prot:          protection mask for the allocated pages
 *      @node:          node to use for allocation or NUMA_NO_NODE
 *      @caller:        caller's return address
 *
 *      Allocate enough pages to cover @size from the page level
 *      allocator with @gfp_mask flags.  Map them into contiguous
 *      kernel virtual space, using a pagetable protection of @prot.
 */
static void *__vmalloc_node(unsigned long size, unsigned long align,
                            gfp_t gfp_mask, pgprot_t prot,
                            int node, const void *caller)
{
    return __vmalloc_node_range(size, align, VMALLOC_START, VMALLOC_END,
                                gfp_mask, prot, 0, node, caller);
}

static inline void *__vmalloc_node_flags(unsigned long size,
        int node, gfp_t flags)
{
    return __vmalloc_node(size, 1, flags, PAGE_KERNEL,
                          node, __builtin_return_address(0));
}

/**
 *      vmalloc  -  allocate virtually contiguous memory
 *      @size:          allocation size
 *      Allocate enough pages to cover @size from the page level
 *      allocator and map them into contiguous kernel virtual space.
 *
 *      For tight control over page level allocator and protection flags
 *      use __vmalloc() instead.
 */
void *vmalloc(unsigned long size)
{
    return __vmalloc_node_flags(size, NUMA_NO_NODE, 0);
}
/**
 *      vfree  -  release memory allocated by vmalloc()
 *      @addr:          memory base address
 *      @size:          size need to free in bytes.
 *
 *      Free the virtually continuous memory area starting at @addr, as
 *      obtained from vmalloc(), vmalloc_32() or __vmalloc(). If @addr is
 *      NULL, no operation is performed.
 *
 *      Must not be called in NMI context (strictly speaking, only if we don't
 *      have CONFIG_ARCH_HAVE_NMI_SAFE_CMPXCHG, but making the calling
 *      conventions for vfree() arch-depenedent would be a really bad idea)
 *
 *      NOTE: assumes that the object at *addr has a size >= sizeof(llist_node)
 */
int vfree(const void *addr, unsigned long size)
{
    BUG_ON(in_nmi());

    kmemleak_free(addr);

    if (!addr)
    {
        return 0;
    }

    __vunmap(addr, size, 1);

    return 0;
}

unsigned long pgprot_kernel;
void *module_alloc(void *target_vmaddr, unsigned long map_size)
{
    void *addr;
    unsigned long align = PAGE_SIZE;
    unsigned long start = (unsigned long)target_vmaddr;
    unsigned long end   = start + map_size;

    if (map_size == 0)
    {
        __err("parameter error.");
        return NULL;
    }

    // only allowed map to [0xe2000000,0xf0000000)
    if ((start < 0xe2000000) || (end >= 0xf0000000))
    {
        __err("module only can be created in range [0xe2000000, 0xf0000000].");
        return NULL;
    }

    __log("vm %p, size 0x%08x.", target_vmaddr, map_size);
#define L_PTE_AP_0          (1 << 4)
#define L_PTE_AP_1          (1 << 5)
#define L_PTE_AP_2          (1 << 9)
#define L_PTE_XN_0          (1 << 0)
    pgprot_kernel  = L_PTE_MT_WRITEBACK;
    pgprot_kernel |= L_PTE_SHARED;
    pgprot_kernel |= (L_PTE_PRESENT | L_PTE_YOUNG | L_PTE_DIRTY);
    pgprot_kernel |= L_PTE_AP_0;
    pgprot_kernel &= ~(L_PTE_AP_1 | L_PTE_AP_2);

    // Must Clear Set the XN Bits Because of We need excuted inst.
    // on this area. !!!!!!!!! refer DDI0406C_C_arm_architecture_reference_manual.pdf
    // B3.5.2 Memory attributes in the Short-descriptor translation
    // table format descriptors
    // cant set here like in highmemory map does, for the bit 0 of pgprot_kernel has
    // represent 'L_PTE_PRESENT'
    // pgprot_kernel &= ~(L_PTE_XN_0);

    pgprot_kernel &= ~(L_PTE_XN);

#define PAGE_KERNEL_EXEC        pgprot_kernel
    BUG_ON(in_interrupt());

    hal_enter_critical();
    addr = __vmalloc_node_range(map_size, align, start, end, 0, PAGE_KERNEL_EXEC, 0, 0, __builtin_return_address(0));
    hal_exit_critical();

    return addr;
}

int module_free(void *target_vmaddr, unsigned long free_size)
{
    int err;

    if (target_vmaddr == NULL || free_size == 0)
    {
        return 0;
    }

    BUG_ON(in_interrupt());

    hal_enter_critical();
    err = vfree(target_vmaddr, free_size);
    hal_exit_critical();

    if (err)
    {
        BUG();
        return err;
    }

    return 0;
}

void show_pte(unsigned long addr)
{
    pgd_t *pgd;
    pgd_t *pgd_base;

    pgd_base = pgd_offset(NULL, 0UL);
    printf("pgdbase = 0x%08lx\n", (unsigned long)pgd_base);

    pgd = pgd_offset(NULL, addr);
    printf("[0x%08lx] *pgd=0x%08llx",
           addr, (long long)pgd_val(*pgd));

    do
    {
        pud_t *pud;
        pmd_t *pmd;
        pte_t *pte;

        if (pgd_none(*pgd))
        {
            break;
        }

        if (pgd_bad(*pgd))
        {
            printf("(bad)");
            break;
        }

        pud = pud_offset(pgd, addr);
        if (PTRS_PER_PUD != 1)
        {
            printf(", *pud=%08llx", (long long)pud_val(*pud));
        }

        if (pud_none(*pud))
        {
            break;
        }

        if (pud_bad(*pud))
        {
            printf("(bad)");
            break;
        }

        pmd = pmd_offset(pud, addr);
        if (PTRS_PER_PMD != 1)
        {
            printf(", *pmd=%08llx", (long long)pmd_val(*pmd));
        }

        if (pmd_none(*pmd))
        {
            break;
        }

        if (pmd_bad(*pmd))
        {
            printf("(bad)");
            break;
        }

        pte = pte_offset_map(pmd, addr);
        printf(", *pte=%08llx", (long long)pte_val(*pte));
        printf(", *ppte=%08llx",
               (long long)pte_val(pte[PTE_HWTABLE_PTRS]));
        pte_unmap(pte);
    } while (0);

    printf("\n");
}
