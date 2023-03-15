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
#include "mmu.h"
#include "epos_hdr.h"
#include <kapi.h>
#include <debug.h>
#include <port.h>
#include <barrier.h>
#include "page_2nd_level.h"
#include <log.h>
#include <compiler.h>

// in global .data section.
const boot_head_t kernel_param;

static struct ic_module_info soc_io_info __tlb_init_data =
{
    .module_nr = 0,
    .items     = NULL
};

static boot_head_t boot_param __tlb_init_data =
{
    {
        0,
        { 0 },
        0,
        0,
        0,
        0,
        { 0 },
        { 0 },
        { 0 }
    },
    {
        { 0 },
        0,
        0,
        0,
        {
            {0}, {0}
        },
        0,
        {
            {0}, {0}
        },
        0,
        0,
        { {0} },
        { 0 },
        { {0} },
        { 0 },
        0,
        0,
        0,
        {0},
        0,
        0,
        0,
        0,
        {0},
        0,
        0,
        0,
        0,
        0,
        { 0 }
    },
    {
        { {0} },
        { {0} },
        { {0} },
        { {0} }
    },
};

__tlb_init_code static void mmu_zero_pgdirs(void)
{
    uint32_t i;

    for (i = 0; i < tlb_size / 4; i ++)
    {
        tlb_pbase[i] = 0;
    }
}

__tlb_init_code static int32_t mmu_build_highmap_area(void)
{
    uint32_t *l2;
    uint32_t pte_vec;
    uint32_t pte_clk;
    uint32_t kern_pgprot;
    uint32_t vector_base;
    int32_t  i;

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
#define L_PTE_AP_0              (1 << 4)
#define L_PTE_AP_1              (1 << 5)
#define L_PTE_TEX               (1 << 6)
#define L_PTE_AP_2              (1 << 9)
#define L_PTE_CB_S              (3 << 2)
#define L_PTE_CB_1              (1 << 2)
#define L_PTE_CB_2              (1 << 3)
#define L_PTE_XN_0              (1 << 0)

    kern_pgprot = L_PTE_MT_WRITEBACK;
    kern_pgprot |= L_PTE_SHARED;
    //kern_pgprot |= (L_PTE_PRESENT | L_PTE_YOUNG | L_PTE_DIRTY);
    kern_pgprot |= (L_PTE_TEX | 0x02);

    // Out and Inner Non-cacheable, refer DDI0406C_C_arm_architecture_reference_manual.pdf
    // B3.8.2 Short-descriptor format memory region attributes, without TEX remap
    // and Short-descriptor translation table second-level descriptor formats
    // B3.5.1 Short-descriptor translation table format descriptors
    /*
     * if SCTLR.AFE sets to 0, AP0,AP1,AP2 all effective,
     * AP[2]----AP[1-0]       PL1 Access       Unpriviledged access.
     *  0         00           No access        No access
     *  0         01           R/W              No access
     *  0         10           R/W              RO
     *  0         11           R/W              R/W
     *  1         00           --------         ---------
     *  1         01           RO               No access
     *  1         10           RO               RO
     *  1         11           RO               RO
     */

    //PL1 R/W, Unpriviledge not permitted
    kern_pgprot |= L_PTE_AP_0;
    kern_pgprot &= ~(L_PTE_AP_1 | L_PTE_AP_2);

    // Must Clear Set the XN Bits Because of We need excuted inst.
    // on this area. !!!!!!!!! refer DDI0406C_C_arm_architecture_reference_manual.pdf
    // B3.5.2 Memory attributes in the Short-descriptor translation
    // table format descriptors
    kern_pgprot &= ~(L_PTE_XN_0);

#ifdef CONFIG_CPU_DCACHE_DISABLE
    kern_pgprot &= ~(L_PTE_CB_S);
    kern_pgprot |= L_PTE_TEX;
#endif

#define IRQ_PAGE1_4K_OFFSET             (0x0000)
#define IRQ_PAGE2_4K_OFFSET             (0x1000)
#define CLK_PAGE1_4K_OFFSET             (0x2000)
#define CLK_PAGE2_4K_OFFSET             (0x3000)
#define CLK_PAGE3_4K_OFFSET             (0x4000)
#define CLK_PAGE4_4K_OFFSET             (0x5000)
#define CLK_PAGE5_4K_OFFSET             (0x6000)
#define L2_OFFSET                       (0x7000)

    extern uint8_t __head_2ndtlb_high_start[];
    l2 = (uint32_t *)&__head_2ndtlb_high_start[L2_OFFSET];

    if (((uint32_t)l2 & 0x3ff) || \
        (__head_2ndtlb_high_start[IRQ_PAGE1_4K_OFFSET] & 0xfff) || \
        (__head_2ndtlb_high_start[IRQ_PAGE2_4K_OFFSET] & 0xfff) || \
        (__head_2ndtlb_high_start[CLK_PAGE1_4K_OFFSET] & 0xfff) || \
        (__head_2ndtlb_high_start[CLK_PAGE2_4K_OFFSET] & 0xfff) || \
        (__head_2ndtlb_high_start[CLK_PAGE3_4K_OFFSET] & 0xfff) || \
        (__head_2ndtlb_high_start[CLK_PAGE4_4K_OFFSET] & 0xfff) || \
        (__head_2ndtlb_high_start[CLK_PAGE5_4K_OFFSET] & 0xfff))
    {
        while (1);
    }

    for (i = 0; i < 256; i ++)
    {
        l2[i] = 0;
    }

    //create exception vector code and data area, 4k bytes enough for excepton vectors.
#ifdef CONFIG_ARMCPU_HIGH_VECTOR_ADDRESS
    vector_base = 0xFFFF0000;
#else
    vector_base = 0x00000000;
#endif

    pte_vec = (vector_base >> 12) & (256 - 1);
    l2[pte_vec ++] = (uint32_t)&__head_2ndtlb_high_start[IRQ_PAGE1_4K_OFFSET] | kern_pgprot;
    l2[pte_vec ++] = (uint32_t)&__head_2ndtlb_high_start[IRQ_PAGE2_4K_OFFSET] | kern_pgprot;

    //create clk code and data area, 20k bytes enough.
    pte_clk = (CONFIG_CLK_TUNING_VBASE >> 12) & (256 - 1);
    if (pte_clk != pte_vec)
    {
        while (1);
    }

    l2[pte_clk ++] = (uint32_t)&__head_2ndtlb_high_start[CLK_PAGE1_4K_OFFSET] | kern_pgprot;
    l2[pte_clk ++] = (uint32_t)&__head_2ndtlb_high_start[CLK_PAGE2_4K_OFFSET] | kern_pgprot;
    l2[pte_clk ++] = (uint32_t)&__head_2ndtlb_high_start[CLK_PAGE3_4K_OFFSET] | kern_pgprot;
    l2[pte_clk ++] = (uint32_t)&__head_2ndtlb_high_start[CLK_PAGE4_4K_OFFSET] | kern_pgprot;
    l2[pte_clk ++] = (uint32_t)&__head_2ndtlb_high_start[CLK_PAGE5_4K_OFFSET] | kern_pgprot;

    // link l2 page table to l1 page directory, page table entry format, domain15.
    tlb_pbase[vector_base >> 20] = (uint32_t)l2 | DOMAIN0 | (1 << 0);
    return 0;
}

/*
 * Ref A3.5.1 Memory types, Examples of memory technologies
 * for which the Normal memory attribute is appropriate are:
 *     -Programmed Flash ROM.
 *     -ROM
 *     -SRAM
 *     -DRAM And DDR memory.
 */
static mem_desc_t sun8iw19p1_memory_layout[] __tlb_init_data = \
{
#if 1
    // avoid mapt directly to NULL for error detect.
    // Attribute C(acheable),S(hare),X(executeable)
    {0x20000000, 0x200FFFFF, 0x00000000, SRAM_MEM  },
#else
    // directly equal map.
    {0x00000000, 0x000FFFFF, 0x00000000, SRAM_MEM  },
#endif

    // directly equal-map.
    { 0x01000000, 0x013FFFFF, 0x01000000, DEVICE_MEM},
    { 0x01480000, 0x0153FFFF, 0x01480000, DEVICE_MEM},
    { 0x01900000, 0x090FFFFF, 0x01900000, DEVICE_MEM},
    { 0x03000000, 0x030FFFFF, 0x03000000, DEVICE_MEM},
    { 0x04000000, 0x040FFFFF, 0x04000000, DEVICE_MEM},
    { 0x05000000, 0x050FFFFF, 0x05000000, DEVICE_MEM},

    // Main Memory Map.
    {
        CONFIG_DRAM_PHYBASE, \
        CONFIG_DRAM_PHYBASE + CONFIG_DRAM_SIZE - 1, \
        CONFIG_DRAM_PHYBASE, NORMAL_MEM
    },

    {
        CONFIG_COHERENT_START_ADDR, \
        CONFIG_COHERENT_START_ADDR + CONFIG_COHERENT_SIZE - 1, \
        CONFIG_DRAM_PHYBASE + CONFIG_DRAM_SIZE - CONFIG_COHERENT_SIZE, \
        NORMAL_MEM_UNCACHED_COHERENT
    },

    // Kernel map, including os code, data, stack and heap area.
    {
        CONFIG_DRAM_VIRTBASE, \
        CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_SIZE - 1, \
        CONFIG_DRAM_PHYBASE, NORMAL_MEM
    },

    // directly offset-map.
    { 0xF1000000, 0xF13FFFFF, 0x01000000, DEVICE_MEM },
    { 0xF1480000, 0xF153FFFF, 0x01480000, DEVICE_MEM },
    { 0xF1900000, 0xF90FFFFF, 0x01900000, DEVICE_MEM },
    { 0xF3000000, 0xF30FFFFF, 0x03000000, DEVICE_MEM },
    { 0xF4000000, 0xF40FFFFF, 0x04000000, DEVICE_MEM },
    { 0xF5000000, 0xF50FFFFF, 0x05000000, DEVICE_MEM },

    // The Last 1M Of Memory. Not use eventally, just takeup a slot
    // The actuall built of the exception area is in function 'mmu_build_highmap_area'
    //{0xFFF00000, 0xFFFFFFFF, 0x44000000, NORMAL_MEM}
};

/* clean data cache line by MVA */
__tlb_init_code static void cp15_clean_dcache_bymva(unsigned int va)
{
    asm volatile("mcr p15, 0, %0, c7, c10, 1\n" /* DCCMVAC */
                 : : "r"(va) : "memory");
}

__tlb_init_code static void mmu_setpgd(uint32_t vaddr_start, uint32_t vaddr_end, uint32_t paddr_start, uint32_t attr)
{
    volatile uint32_t *pdirs;
    volatile int i, nsec;

    pdirs  = (uint32_t *)tlb_pbase + (vaddr_start >> 20);
    nsec = (vaddr_end >> 20) - (vaddr_start >> 20);
    for (i = 0; i <= nsec; i++)
    {
        *pdirs = attr | (((paddr_start >> 20) + i) << 20);
        cp15_clean_dcache_bymva((unsigned int)pdirs);
        pdirs++;
    }

    isb();
    dsb();
    dmb();
}

__tlb_init_code static void mmu_build_page_dirs(void)
{
    int i;
    for (i = 0; i < sizeof(sun8iw19p1_memory_layout) / sizeof(sun8iw19p1_memory_layout[0]); i ++)
    {
        mmu_setpgd(sun8iw19p1_memory_layout[i].vaddr_start, \
                   sun8iw19p1_memory_layout[i].vaddr_end, \
                   sun8iw19p1_memory_layout[i].paddr_start, \
                   sun8iw19p1_memory_layout[i].attr);
    }

    isb();
    dsb();
    dmb();
}

__attribute__((target("arm"))) __tlb_init_code noinline static uint32_t mmu_set_domain(uint32_t domain_val)
{
    uint32_t old_domain;

    asm volatile("mrc p15, 0, %0, c3, c0\n" : "=r"(old_domain));
    asm volatile("mcr p15, 0, %0, c3, c0\n" : :"r"(domain_val) : "memory");

    return old_domain;
}

__tlb_init_code noinline static int32_t mmu_create_memory_map(void)
{
    mmu_zero_pgdirs();
    mmu_build_page_dirs();
    mmu_build_highmap_area();

    awos_arch_dcache_clean_flush();
    awos_arch_icache_flush();
    awos_arch_dcache_disable();
    awos_arch_icache_disable();
    awos_arch_mmu_disable();

    /*
     * Domain Access Control, D0-D15,Bits[(2n+1):2n]:
     * Domain n access permission, where n = 0 to 15. Permitted values are:
     * -0b00 No access. Any access to the domain generates a Domain fault
     * -0b01 Client. Accesses are checked against the permission bits in the translation tables.
     * -0b10 Reserved, effect is UNPREDICTABLE.
     * -0b11 Manager. Accesses are not checked against the permission bits in the translation table.
     * Here the setting is:
     *  D0-D14: Client, All DEVICE_MEM and NORMAL_MEM belongs to this domain.
     *       from [0x0000000, 0x000fffff]
     *            [0x0190000, 0x090fffff]
     *            [0xc200000, 0xc3ffffff]
     *            [0xf190000, 0xf90fffff]
     *  D15: Manager, only plugin modules and cpu vectors use this preset,
     *       from [0xffff0000, 0xffff6fff]
     *            [0xe0000000, 0xXXXXXXXX]
     *  the reason needs a separate D15 manager domain is that: exception hander and driver plugin work
     *  in 2nd page level, the pte bits[0] XN field only can be set work without abort when domain
     *  field set to manager. detail see arm_archtechture_reference_mannual about "XN, Execute-never"
     *  setting, chapter B3.7.2.
     */

    //from now, D15 domain is not nessary, because the XN bit was clear.
    mmu_set_domain(0x55555555);

    awos_arch_tlb_set((uint32_t)tlb_pbase);
    awos_arch_mmu_enable();

    awos_arch_icache_enable();
    awos_arch_dcache_enable();
    awos_arch_flush_icache_all();

    dmb();
    dsb();
    isb();
    return 0;
}

__tlb_init_code noinline static void copy_boot_para(const boot_head_t *to, const boot_head_t *from)
{
    uint8_t *src;
    uint8_t *dst;
    uint32_t count = sizeof(boot_head_t);

    src = (uint8_t *)from;
    dst = (uint8_t *)to;

    while (count --)
    {
        *dst ++ = *src ++;
    }

    dsb();
    return;
}

/*
 * The Short-descriptor translation table format supports a memory map based on memory sections or pages:
 * -Supersections Consist of 16MB blocks of memory. Support for Supersections is optional, except that an
 *  implementation that includes the Large Physical Address Extension and supports more that 32 bits
 *  of Physical Address must also support Supersections to provide access to the entire Physical
 *  Address space.
 * -Sections Consist of 1MB blocks of memory.
 * -Large pages Consist of 64KB blocks of memory.
 * -Small pages Consist of 4KB blocks of memory.
 * Supersections, Sections and Large pages map large regions of memory using only a single TLB entry.
 */
__tlb_init_code void mmu_init(void)
{
    copy_boot_para(&boot_param, melis_head_para_get());
    mmu_create_memory_map();
    copy_boot_para(&kernel_param, &boot_param);
}

void mmu_unmap(void)
{
    uint32_t i;

    for (i = 0; i < (CONFIG_DRAM_SIZE >> 20); i ++)
    {
        tlb_vbase[i + (CONFIG_DRAM_PHYBASE >> 20)] = 0;
    }

    awos_arch_dcache_clean_flush_all();
    awos_arch_flush_kern_tlb_range(CONFIG_DRAM_PHYBASE, CONFIG_DRAM_PHYBASE + CONFIG_DRAM_SIZE);
    awos_arch_v7_invalidate_l1();
    awos_arch_tlb_invalidate_all();
    awos_arch_flush_icache_all();
    dsb();
    isb();
    dmb();
}

unsigned long awos_arch_virt_to_phys(unsigned long virtaddr)
{
    return virtaddr - CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_PHYBASE;
}

unsigned long awos_arch_phys_to_virt(unsigned long phyaddr)
{
    return phyaddr + (CONFIG_DRAM_VIRTBASE - CONFIG_DRAM_PHYBASE);
}

unsigned long awos_arch_bus_to_virt(unsigned long phyaddr)
{
    return (CONFIG_IOREGS_VBASE + phyaddr);
}

void awos_arch_vmem_delete(uint8_t *virtaddr, uint32_t npage)
{
    uint32_t   ptenum;
    uint32_t   ptestart;
    uint32_t   vmaddr;

    //See if called from ISR ...
    if (awos_arch_isin_irq() != 0)
    {
        __err("cannot call vmem_create from isr!");
        return;
    }

    vmaddr = (uint32_t)virtaddr;
    if (npage == 0)
    {
        __err("0 page number to vm delete");
        return;
    }

    int module_free(void *target_vmaddr, unsigned long free_size);
    uint32_t size = ALIGN(npage * PAGE_SIZE, PAGE_SIZE);
    module_free((void *)virtaddr, size);

    return;
}

void *module_alloc(void *target_vmaddr, unsigned long map_size);
int32_t awos_arch_vmem_create(uint8_t *virtaddr, uint32_t npage, uint8_t domain)
{
    //See if called from ISR ...
    if (awos_arch_isin_irq() != 0)
    {
        __err("cannot call vmem_create from isr!");
        return -1;
    }

    uint32_t size = ALIGN(npage * PAGE_SIZE, PAGE_SIZE);
    module_alloc((void *)virtaddr, size);
    return 0;
}

uint32_t awos_arch_vaddr_map2_phys(uint32_t vaddr)
{
    uint32_t  dram_size;
    uint32_t  phyadr;
    uint32_t  tlbidx;

    if (vaddr == 0)
    {
        return 0;
    }

    //linear map zone, just cal with the offset.
    if ((vaddr >= CONFIG_DRAM_VIRTBASE) && (vaddr < CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_SIZE))
    {
        return awos_arch_virt_to_phys(vaddr);
    }

    dram_size = CONFIG_DRAM_SIZE;
    phyadr = 0;
    switch (tlb_vbase[((uint32_t)vaddr) >> 20] & 3)
    {
        case MEMS_TLB_L1_PTE_SECTION:
        {
            //1M section
            tlbidx = ((uint32_t)vaddr >> 20);
            phyadr = ((tlb_vbase[tlbidx] & 0xfff00000) \
                      | ((uint32_t)vaddr & 0x000fffff));
            break;
        }
        case MEMS_TLB_L1_PTE_COARSE:
        {
            uint32_t *l2vadr;

            //coarse page table entry
            //get L2 page table phyical address from L1 page table entry
            l2vadr = (uint32_t *)((tlb_vbase[(uint32_t)vaddr >> 20] & 0xfffffc00));
            if (((uint32_t)l2vadr >= CONFIG_DRAM_PHYBASE) && \
                ((uint32_t)l2vadr < (CONFIG_DRAM_PHYBASE + dram_size)))
            {
                //valid dram address, convert physical address to virtual address
                l2vadr = (uint32_t *)((uint32_t)l2vadr - CONFIG_DRAM_PHYBASE + CONFIG_DRAM_VIRTBASE);
            }
            else
            {
                //invalid virtual address for physical address convert,
                //device and sram all use 1M section page table entry
                __err("invalid region for VA to PA, addr = %x", vaddr);
                break;
            }

            //L2 page table entry
            tlbidx = ((uint32_t)vaddr >> 12) & (256 - 1);
            if ((l2vadr[tlbidx] & 3) == MEMS_TLB_L2_PTE_LARGE)
            {
                phyadr = ((l2vadr[tlbidx] & 0xffff0000) \
                          | ((uint32_t)vaddr & ((64 * 1024) - 1)));
            }
            else if ((l2vadr[tlbidx] & 3) == MEMS_TLB_L2_PTE_SMALL)
            {
                phyadr = ((l2vadr[tlbidx] & 0xfffff000) \
                          | ((uint32_t)vaddr & ((4 * 1024) - 1)));
            }
            else
            {
                //invalid L2 page table entry
                __err("invalid L2 PTE type for VA to PA, addr = %x", vaddr);
                break;
            }
            break;
        }
        default:
        {
            //invalid L1 page table entry type
            __err("invalid L1 PTE type for VA to PA, addr = %x", vaddr);
            software_break();
            break;
        }
    }

    return phyadr;
}

int32_t check_virtual_address(uint32_t vaddr)
{
    int ret = 0;
    uint32_t  dram_size;
    uint32_t  tlbidx;

    /* linear map zone, just cal with the offset. */
    if ((vaddr >= CONFIG_DRAM_VIRTBASE) && (vaddr < CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_SIZE))
    {
        return 1;
    }

    dram_size = CONFIG_DRAM_SIZE;
    switch (tlb_vbase[((uint32_t)vaddr) >> 20] & 3)
    {
        case MEMS_TLB_L1_PTE_SECTION:
        {
            /* 1M section */
            ret = 1;
            break;
        }
        case MEMS_TLB_L1_PTE_COARSE:
        {
            uint32_t *l2vadr;

            /* coarse page table entry */
            /* get L2 page table phyical address from L1 page table entry */
            l2vadr = (uint32_t *)((tlb_vbase[(uint32_t)vaddr >> 20] & 0xfffffc00));
            if (((uint32_t)l2vadr >= CONFIG_DRAM_PHYBASE) && \
                ((uint32_t)l2vadr < (CONFIG_DRAM_PHYBASE + dram_size)))
            {
                /* valid dram address, convert physical address to virtual address */
                l2vadr = (uint32_t *)((uint32_t)l2vadr - CONFIG_DRAM_PHYBASE + CONFIG_DRAM_VIRTBASE);
            }
            else
            {
                /* invalid virtual address for physical address convert,
                 * device and sram all use 1M section page table entry
                 */
                break;
            }

            /* L2 page table entry */
            tlbidx = ((uint32_t)vaddr >> 12) & (256 - 1);
            if ((l2vadr[tlbidx] & 3) == MEMS_TLB_L2_PTE_LARGE ||
                (l2vadr[tlbidx] & 3) == MEMS_TLB_L2_PTE_SMALL)
            {
                ret = 1;
            }
            break;
        }
        default:
        {
            /* invalid L1 page table entry type */
            break;
        }
    }

    return ret;
}
