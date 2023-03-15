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
#include <stdlib.h>
#include <string.h>
#include "pgtable.h"
#include "page_2nd_level.h"
#include "mmu.h"

#include <arch.h>
#include <port.h>
#include <kasan_linux.h>
#include <log.h>
#include <debug.h>

#include <rtthread.h>

#define SEC1M          (0<<18)
#define SEC16M         (1<<18)
#define DESC_SEC       (0x2)

#ifndef CONFIG_KASAN_SHADOW_OFFSET
#define CONFIG_KASAN_SHADOW_OFFSET (0x54000000)
#endif

#define dsb(option)             __asm__ __volatile__ ("dsb " #option : : : "memory")
#define cpu_tlb_invalidate_all  awos_arch_tlb_invalidate_all

uint32_t awos_arch_virt_to_phys(uint32_t virtaddr);
#define __page_aligned_bss __attribute__((aligned (4096)));
#define ALIGN_MASK(align_size)  ((align_size) - 1)
#define KASAN_ALIGN(size, align)    (((size) + (align) - 1) & ~((align) - 1))

void esKRNL_SchedLock(void);
void esKRNL_SchedUnlock(void);

static unsigned char kasan_zero_page[PAGE_SIZE] __page_aligned_bss;
static pte_t kasan_zero_pte[PTRS_PER_PTE] __page_aligned_bss;

/* 1M aligned memory, located in .bss segement */
static unsigned char kasan_shadow_memory_area[CONFIG_DRAM_SIZE >> 3] __attribute__((aligned(1048576)));
static unsigned char kasan_shadow_memory_area_bitmap[CONFIG_DRAM_SIZE >> 3 >> 20];

static void *kasan_section_malloc(int sections)
{
    int i;
    int length = 0;

    for (i = 0; i < sizeof(kasan_shadow_memory_area_bitmap); i++)
    {
        if (kasan_shadow_memory_area_bitmap[i] == 0)
        {
            length ++;
            if (length >= sections)
            {
                break;
            }
        }
        else
        {
            length = 0;
        }
    }

    if (length < sections)
    {
        return NULL;
    }
    else
    {
        int j;
        for (j = 0; j < sections; j++)
        {
            if (kasan_shadow_memory_area_bitmap[j + i] == 0)
            {
                kasan_shadow_memory_area_bitmap[j + i] = 1;
            }
            else
            {
                printf("%s, %d, fatal error! bitmap is wrong!\n", __func__, __LINE__);
            }
        }
        return &kasan_shadow_memory_area[i * (1024 * 1024)];
    }
}

static void kasan_section_free(void *ptr, int sections)
{
    int index;
    int i;
    index = ((char *)ptr - (char *)&kasan_shadow_memory_area) >> 20;
    for (i = 0; i < sections; i++)
    {
        if (kasan_shadow_memory_area_bitmap[i + index] == 1)
        {
            kasan_shadow_memory_area_bitmap[i + index] = 0;
        }
        else
        {
            printf("%s, %d, fatal error! bitmap is wrong!\n", __func__, __LINE__);
        }
    }
}

static int32_t early_vmem_create_1m(uint32_t vmaddr, uint32_t npte, uint8_t domain, uint32_t *nalloc)
{
    uint32_t   index;
    uint32_t   page_paddr;
    uint32_t *l2tbl_addr;

    if ((tlb_vbase[vmaddr >> 20] & 3) == 0)
    {
        l2tbl_addr = (uint32_t *)&kasan_zero_pte;
        tlb_vbase[vmaddr >> 20] |= (awos_arch_virt_to_phys((uint32_t)l2tbl_addr) & 0xFFFFFC00)
                                   | MEMS_TLB_L1_PTE_COARSE
                                   | (0 << 2)
                                   | (0 << 3)
                                   | (0 << 4)
                                   | (0 << 9)
                                   | DOMAIN0;

        page_paddr = awos_arch_virt_to_phys((uint32_t)&kasan_zero_page);
        for (index = 0; index < 256; index++)
        {
            l2tbl_addr[index] = page_paddr
                                | MEMS_TLB_L2_PTE_SMALL
                                | (0 << 0) // XN bit
                                | (3 << 2) // C/B bit
                                | (3 << 5) // AP[1:0]
                                | (0 << 9) // AP[2]
                                | (1 << 10) // S bit
                                | (0 << 11) //nG bit
                                | (1 << 6) // TEX
                                ;
        }
    }
    return 0;
}

static int32_t early_vmem_map_read_only(unsigned long vmaddr)
{
    uint32_t index;
    unsigned long page_paddr;
    unsigned long *l2tbl_addr;

    if ((tlb_vbase[vmaddr >> 20] & 3) == MEMS_TLB_L1_PTE_SECTION)
    {
        return 0;
    }

    if (((tlb_vbase[vmaddr >> 20] & 3) == MEMS_TLB_L1_PTE_COARSE) &&
        (awos_arch_phys_to_virt((tlb_vbase[vmaddr >> 20] & 0xfffffc00)) == (unsigned long)&kasan_zero_pte))
    {
        l2tbl_addr = (unsigned long *)&kasan_zero_pte;
        for (index = 0; index < 256; index++)
        {
            l2tbl_addr[index] |= (1 << 9);
        }
    }
    return 0;
}

static void kasan_map_early_shadow_read_only(void)
{
    unsigned long addr = KASAN_SHADOW_START;
    unsigned long end = KASAN_SHADOW_END;
    unsigned long next;

    do
    {
        next = addr + 1 * 1024 * 1024;
        early_vmem_map_read_only(addr);
        addr = next;
    } while (addr != end);

    dsb();
    cpu_tlb_invalidate_all();
    awos_arch_dcache_clean_flush_all();
    awos_arch_flush_icache_all();
}

static void kasan_map_early_shadow(void)
{
    unsigned long addr = KASAN_SHADOW_START;
    unsigned long end = KASAN_SHADOW_END;
    unsigned long next;

    do
    {
        next = addr + 1 * 1024 * 1024;
        early_vmem_create_1m(addr, 0, 0, NULL);
        addr = next;
    } while (addr != end);

    dsb();
    cpu_tlb_invalidate_all();
    awos_arch_dcache_clean_flush_all();
    awos_arch_flush_icache_all();
}

static int32_t create_page_small(unsigned long vmaddr, unsigned long *l2tbl_addr, uint32_t npte, uint32_t *nalloc)
{
    uint32_t ptestart;
    unsigned long page_vaddr;
    unsigned long page_paddr;

    ptestart = (vmaddr >> 12) & (256 - 1);

    for (; ;)
    {
        if (npte == 0)
        {
            return 0;
        }

        if (l2tbl_addr[ptestart] == 0)
        {
            page_vaddr = (unsigned long)k_malloc_align(4096, 4096);
            if (page_vaddr == 0 || (page_vaddr & ALIGN_MASK(4096)))
            {
                __err("alloc memory failure, page_vaddr = 0x%08x.", page_vaddr);
                software_break();
                break;
            }
            memset((void *)page_vaddr, 0, 4096);

            page_paddr = awos_arch_virt_to_phys(page_vaddr);
            l2tbl_addr[ptestart] = page_paddr
                                   | MEMS_TLB_L2_PTE_SMALL
                                   | (0 << 0) // XN bit
                                   | (3 << 2) // C/B bit
                                   | (3 << 5) // AP[1:0]
                                   | (0 << 9) // AP[2]
                                   | (1 << 10) // S bit
                                   | (0 << 11) //nG bit
                                   | (1 << 6) // TEX
                                   ;
            (*nalloc)++;
        }

        ptestart++;
        npte--;
    }

    return -1;
}

static void create_page_large(unsigned long vmaddr, unsigned long *l2tbl_addr, uint32_t *npte, uint32_t *nalloc)
{
    uint32_t   ptestart;
    unsigned long page_vaddr;
    unsigned long page_paddr;
    uint32_t   PTEIndex;

    ptestart = (vmaddr >> 12) & (256 - 1);

    for (; ;)
    {
        if (*npte < 16)
        {
            return;
        }

        page_vaddr = (unsigned long)k_malloc_align(64 * 1024, 64 * 1024);
        if ((page_vaddr == 0) || (page_vaddr & ALIGN_MASK(64 * 1024)))
        {
            __err("alloc memory failure. page_vaddr = 0x%08x", page_vaddr & 0xffffffff);
            void memory_info(void);
            memory_info();
            software_break();
            return;
        }
        memset((void *)page_vaddr, 0, 64 * 1024);

        if ((l2tbl_addr[ptestart] & 3) == MEMS_TLB_L2_PTE_SMALL)
        {
            uint32_t i;

            for (i = 1; i < 16; i++)
            {
                if ((l2tbl_addr[ptestart + i] & 3) != MEMS_TLB_L2_PTE_SMALL)
                {
                    k_free_align((void *)page_vaddr, 64 * 1024);
                    return;
                }
            }
        }

        if (l2tbl_addr[ptestart] == 0)
        {
            page_paddr = awos_arch_virt_to_phys(page_vaddr);
            for (PTEIndex = 0; PTEIndex < 16; PTEIndex++)
            {
                l2tbl_addr[ptestart + PTEIndex] = page_paddr
                                                  | MEMS_TLB_L2_PTE_LARGE
                                                  | (0 << 9) // AP[2]
                                                  | (3 << 4) // AP_RW AP[1:0]
                                                  | (1 << 2) // B bit
                                                  | (1 << 3) // C bit
                                                  | MEMWBWAYC
                                                  | (0 << 11) //nG bit
                                                  | (0 << 6) // SBZ
                                                  | (1 << 10); // S bit
            }
            (*nalloc) += 16;
        }

        vmaddr   += (64 * 1024);
        (*npte)  -= 16;
        ptestart += 16;

        if ((*npte) < 16)
        {
            return;
        }
    }
}

static int32_t create_l2pte(unsigned long vmaddr, unsigned long *l2tbl_addr, uint32_t npte, uint32_t *nalloc)
{
    uint32_t   ptestart;
    uint32_t   npte_s;
    uint32_t   npte_l;

    ptestart = (vmaddr >> 12) & (256 - 1);

    if (vmaddr & (64 * 1024 - 1))
    {
        npte_s = (MEMS_ALIGN(vmaddr, 64 * 1024) - vmaddr) >> 12;
        npte_s = MEMS_MIN(npte_s, npte);
        if (create_page_small(vmaddr, l2tbl_addr, npte_s, nalloc) != 0)
        {
            __err("create VM [%x] L2 page table failed", vmaddr);
            return -1;
        }
        vmaddr    = MEMS_ALIGN(vmaddr, 64 * 1024);
        npte     -= npte_s;
        ptestart += npte_s;
    }

    /* try to create 64k aligned as large page */
    if (npte >= 16)
    {
        npte_l = npte;
        create_page_large(vmaddr, l2tbl_addr, &npte_l, nalloc);
        npte_l = npte - npte_l;
        vmaddr = vmaddr + npte_l * (4 * 1024);
        npte  -= npte_l;
    }

    /* create the tail unligned pages as small pages */
    if (npte)
    {
        if (create_page_small(vmaddr, l2tbl_addr, npte, nalloc) != 0)
        {
            __err("create VM [%x] L2 page table failed", vmaddr);
            return -1;
        }
    }

    return 0;
}

static int32_t vmem_create_section(unsigned long vmaddr, uint8_t Domain, uint32_t *nalloc)
{
    unsigned long page_vaddr;
    unsigned long page_paddr;

    /* alloc 1M memory aligned by 1M. */
    page_vaddr = (unsigned long)kasan_section_malloc(1);
    if ((page_vaddr == 0) || (page_vaddr & ALIGN_MASK(1 * 1024 * 1024)))
    {
        __err("alloc memory failure.");
        software_break();
        return -1;
    }

    memset((void *)page_vaddr, 0, 1 * 1024 * 1024);

    page_paddr = awos_arch_virt_to_phys(page_vaddr);
    tlb_vbase[vmaddr >> 20] = page_paddr
                              | MEMS_TLB_L1_PTE_SECTION
                              | SEC1M
                              | NORMAL_MEM;

    (*nalloc) += 256;

    return 0;
}

static int32_t vmem_create_1m(unsigned long vmaddr, uint32_t npte, uint8_t domain, uint32_t *nalloc)
{
    uint32_t   index;
    unsigned long *l2tbl_addr;
    int32_t    res;

    esKRNL_SchedLock();

    if (((tlb_vbase[vmaddr >> 20] & 3) == 0) ||
        (awos_arch_phys_to_virt((tlb_vbase[vmaddr >> 20] & 0xfffffc00)) == (unsigned long)&kasan_zero_pte))
    {
        if (((vmaddr & (1024 * 1024 - 1)) == 0) && (npte == 256))
        {
            if (vmem_create_section(vmaddr, domain, nalloc) == 0)
            {
                esKRNL_SchedUnlock();
                return 0;
            }
        }

        l2tbl_addr = (unsigned long *)k_malloc_align(1024, 1024);
        if (l2tbl_addr == NULL || (((unsigned long)l2tbl_addr) & ALIGN_MASK(1024)))
        {
            esKRNL_SchedUnlock();
            __err("alloc memory failure.");
            software_break();
            return -1;
        }

        tlb_vbase[vmaddr >> 20] = (awos_arch_virt_to_phys((unsigned long)l2tbl_addr) & 0xFFFFFC00)
                                  | MEMS_TLB_L1_PTE_COARSE
                                  | (0 << 2)
                                  | (0 << 3)
                                  | (0 << 4)
                                  | (0 << 9)
                                  | DOMAIN0;

        for (index = 0; index < 256; index++)
        {
            l2tbl_addr[index] = 0;
        }
    }
    else if ((tlb_vbase[vmaddr >> 20] & 3) == MEMS_TLB_L1_PTE_SECTION)
    {
        if ((tlb_vbase[vmaddr >> 20] & 0x000001e0) != (domain << 5))
        {
            __err("ERR:domain conflict");
            esKRNL_SchedUnlock();
            return -1;
        }
        esKRNL_SchedUnlock();
        return 0;
    }
    else
    {
        l2tbl_addr = (unsigned long *)(awos_arch_phys_to_virt(tlb_vbase[vmaddr >> 20] & 0xfffffc00));
    }

    res = create_l2pte(vmaddr, l2tbl_addr, npte, nalloc);

    esKRNL_SchedUnlock();
    return res;
}

static int32_t vmem_delete_section(unsigned long vmaddr)
{
    uint32_t  phy_addr;

    phy_addr = tlb_vbase[vmaddr >> 20] & 0xfff00000;
    kasan_section_free((void *)awos_arch_phys_to_virt(phy_addr), 1);

    tlb_vbase[vmaddr >> 20] = 0;

    awos_arch_mems_clean_flush_dcache_region((unsigned long)tlb_vbase, tlb_size);
    return 0;
}

static int32_t delete_l2pte(unsigned long vmaddr, unsigned long *l2ptbl_addr, uint32_t npte)
{
    uint32_t  l2pte_idx;
    unsigned long  pte_phyaddr;
    uint32_t  i;

    l2pte_idx = (vmaddr >> 12) & (256 - 1);
    while (npte)
    {
        if (l2ptbl_addr[l2pte_idx] & 3)
        {
            if ((l2ptbl_addr[l2pte_idx] & 3) == MEMS_TLB_L2_PTE_LARGE)
            {
                pte_phyaddr = l2ptbl_addr[l2pte_idx] & 0xffff0000;

                k_free_align((void *)awos_arch_phys_to_virt(pte_phyaddr), 64 * 1024);

                for (i = 0; npte && (i < 16); i++, npte--, l2pte_idx++)
                {
                    l2ptbl_addr[l2pte_idx] = 0;
                }
            }
            else if ((l2ptbl_addr[l2pte_idx] & 3) == MEMS_TLB_L2_PTE_SMALL)
            {
                pte_phyaddr = l2ptbl_addr[l2pte_idx] & 0xfffff000;

                k_free_align((void *)awos_arch_phys_to_virt(pte_phyaddr), 4096);
                l2ptbl_addr[l2pte_idx] = 0;

                l2pte_idx++;
                npte--;
            }
            else
            {
                __err("invalid L2 page table entry to delete");
                return -1;
            }
        }
        else
        {
            l2pte_idx++;
            npte--;
        }
    }

    return 0;
}

static void vmem_delete_1m(unsigned long vmaddr, uint32_t npte)
{
    uint32_t  l2pte_idx;
    uint32_t  l2pte_tmp;
    unsigned long *l2tbl_addr;

    esKRNL_SchedLock();

    switch ((tlb_vbase[vmaddr >> 20] & 3))
    {
        case  MEMS_TLB_L1_PTE_ERR:
        {
            esKRNL_SchedUnlock();
            return;
        }
        case MEMS_TLB_L1_PTE_SECTION:
        {
            if ((vmaddr & (1024 * 1024 - 1)) || (npte != 256))
            {
                __err("invalid 1m virtual memory space to delete");
                esKRNL_SchedUnlock();
                return;
            }

            vmem_delete_section(vmaddr);

            esKRNL_SchedUnlock();
            return;
        }
        case MEMS_TLB_L1_PTE_COARSE:
        {
            l2tbl_addr = (uint32_t *)awos_arch_phys_to_virt((tlb_vbase[vmaddr >> 20] & 0xfffffc00));
            break;
        }
        default:
        {
            __err("1M section and fine page not support");
            esKRNL_SchedUnlock();
            return;
        }
    }

    delete_l2pte(vmaddr, l2tbl_addr, npte);

    l2pte_tmp = 0;
    for (l2pte_idx = 0; l2pte_idx < 256; l2pte_idx++)
    {
        l2pte_tmp |= l2tbl_addr[l2pte_idx];
    }

    awos_arch_mems_clean_flush_dcache_region((unsigned long)l2tbl_addr, 1024);

    if (l2pte_tmp == 0)
    {
        k_free_align((void *)l2tbl_addr, 1024);

        tlb_vbase[(vmaddr) >> 20] = 0;
    }

    awos_arch_mems_clean_flush_dcache_region((unsigned long)tlb_vbase, tlb_size);

    esKRNL_SchedUnlock();
    return;
}

static void kasan_awos_arch_vmem_delete(unsigned long *virtaddr, uint32_t npage)
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

    ptenum   = (npage + 4 - 1) >> 2;

    ptestart = (vmaddr >> 12) & (256 - 1);

    if ((ptestart + ptenum) <= 256)
    {
        vmem_delete_1m(vmaddr, ptenum);
    }
    else
    {
        //delete the first unaligned 1M section space.
        vmem_delete_1m(vmaddr, (256 - ptestart));

        ptenum -= (256 - ptestart);
        vmaddr  = (vmaddr & 0xfff00000) + 1 * 1024 * 1024;

        //delete aligned 1M section space.
        while (ptenum > 256)
        {
            vmem_delete_1m(vmaddr, 256);
            ptenum -= 256;
            vmaddr += (1 * 1024 * 1024);
        }

        //delete the last unaligned 1M section space.
        vmem_delete_1m(vmaddr, ptenum);
    }

    dsb();
    cpu_tlb_invalidate_all();
    awos_arch_dcache_clean_flush_all();
    awos_arch_flush_icache_all();
    return;
}

static int32_t kasan_awos_arch_vmem_create(unsigned long *virtaddr, uint32_t npage, uint8_t domain)
{
    unsigned long      vmaddr;
    uint32_t           ptenum;
    uint32_t           ptestart;
    uint32_t           nalloc;

    if (awos_arch_isin_irq() != 0)
    {
        __err("cannot call vmem_create from isr!");
        return -1;
    }
    vmaddr = (unsigned long)virtaddr;
    if (vmaddr & ((4 * 1024) - 1))
    {
        __err("invalid adress for vm ctreate, must aligned by 4K");
        return -1;
    }

    ptenum   = (npage + 4 - 1) >> 2;

    ptestart = (vmaddr >> 12) & (256 - 1);

    if (ptenum == 0)
    {
        return 0;
    }

    if ((ptestart + ptenum) <= 256)
    {
        if (vmem_create_1m(vmaddr, ptenum, domain, &nalloc) != 0)
        {
            kasan_awos_arch_vmem_delete((unsigned long *)vmaddr, nalloc * 4);
            return -1;
        }
    }
    else
    {
        if (vmem_create_1m(vmaddr, (256 - ptestart), domain, &nalloc) != 0)
        {
            kasan_awos_arch_vmem_delete((unsigned long *)vmaddr, nalloc * 4);
            return -1;
        }

        ptenum -= (256 - ptestart);
        vmaddr  = (vmaddr & 0xfff00000) + 1 * 1024 * 1024;

        while (ptenum > 256)
        {
            if (vmem_create_1m(vmaddr, 256, domain, &nalloc) != 0)
            {
                kasan_awos_arch_vmem_delete((unsigned long *)vmaddr, nalloc * 4);
                return -1;
            }

            ptenum -= 256;
            vmaddr += (1 * 1024 * 1024);
        }

        if (ptenum)
        {
            if (vmem_create_1m(vmaddr, ptenum, domain, &nalloc) != 0)
            {
                kasan_awos_arch_vmem_delete((unsigned long *)vmaddr, nalloc * 4);
                return -1;
            }
        }
    }

    dsb();
    cpu_tlb_invalidate_all();
    awos_arch_dcache_clean_flush_all();
    awos_arch_flush_icache_all();
    return 0;
}

static int kasan_map_area(unsigned long start, unsigned long end)
{
    int page_1k;
    page_1k = (KASAN_ALIGN((end - start), 4096)) >> 10;
    return kasan_awos_arch_vmem_create((unsigned long *)start, page_1k, 0);
}

static void kasan_map_shadow(void)
{
    unsigned long addr;
    unsigned long end;
    addr = (CONFIG_DRAM_VIRTBASE >> 3) + CONFIG_KASAN_SHADOW_OFFSET;
    end = ((CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_SIZE - CONFIG_COHERENT_SIZE) >> 3) + CONFIG_KASAN_SHADOW_OFFSET;
    kasan_map_area(addr, end);

    addr = (CONFIG_COHERENT_START_ADDR >> 3) + CONFIG_KASAN_SHADOW_OFFSET;
    end = ((CONFIG_COHERENT_START_ADDR + CONFIG_COHERENT_SIZE) >> 3) + CONFIG_KASAN_SHADOW_OFFSET;
    kasan_map_area(addr, end);

    addr = (0xFFFF0000 >> 3) + CONFIG_KASAN_SHADOW_OFFSET;
    end = (0xFFFFFFFF >> 3) + CONFIG_KASAN_SHADOW_OFFSET;
    kasan_map_area(addr, end);

    memset(&kasan_zero_page, 0, sizeof(kasan_zero_page));
    kasan_map_early_shadow_read_only();
}

void kasan_early_init(void)
{
    kasan_map_early_shadow();
}

void kasan_init(void)
{
    kasan_map_shadow();
    void kasan_init_report(void);
    kasan_init_report();
}
