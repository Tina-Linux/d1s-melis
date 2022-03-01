/*
 * =====================================================================================
 *
 *       Filename:  mmu.c
 *
 *    Description:  routine dealt with tlb.
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 13:29:32
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-05-29 14:20:16
 *
 * =====================================================================================
 */

#include "mmu.h"
#include "epos_hdr.h"
#include <kapi.h>
#include <debug.h>
#include <log.h>
#include <port.h>
#include <barrier.h>

#define ALIGN_MASK(align_size)  ((align_size) - 1)
// in global .data section.
const __epos_para_t kernel_param;

static struct ic_module_info soc_io_info __tlb_init_data =
{
    .module_nr = 0,
    .items     = NULL
};

static __epos_para_t boot_param __tlb_init_data =
{
    {1, 0},
    {0},
    {0x80000000, 0x04000000, 0, 0},
    {0x80300000, 0x01000000, 0, 0},
    {0, 0, 0, 0},
    0
};

__tlb_init_code void mmu_enable(void)
{
    asm volatile
    (
        "mrc p15, 0, r0, c1, c0, 0 \n"
        "orr r0, r0, #0x1 \n"
        "mcr p15, 0, r0, c1, c0, 0 \n"
        :::"r0"
    );
}

__tlb_init_code void mmu_disable(void)
{
    asm volatile
    (
        "mrc p15, 0, r0, c1, c0, 0 \n"
        "bic r0, r0, #0x1 \n"
        "mcr p15, 0, r0, c1, c0, 0 \n"
        :::"r0"
    );
}

__tlb_init_code void mmu_enable_icache(void)
{
    asm volatile
    (
        "mrc p15, 0, r0, c1, c0, 0 \n"
        "orr r0, r0, #(1<<12) \n"
        "mcr p15, 0, r0, c1, c0, 0 \n"
        :::"r0"
    );
}

__tlb_init_code void mmu_enable_dcache(void)
{
    asm volatile
    (
        "mrc p15, 0, r0, c1, c0, 0 \n"
        "orr r0, r0, #(1<<2) \n"
        "mcr p15, 0, r0, c1, c0, 0 \n"
        :::"r0"
    );
}

__tlb_init_code void mmu_disable_icache(void)
{
    asm volatile
    (
        "mrc p15, 0, r0, c1, c0, 0 \n"
        "bic r0, r0, #(1<<12) \n"
        "mcr p15, 0, r0, c1, c0, 0 \n"
        :::"r0"
    );

}

__tlb_init_code void mmu_disable_dcache(void)
{
    asm volatile
    (
        "mrc p15, 0, r0, c1, c0, 0 \n"
        "bic r0, r0, #(1<<2) \n"
        "mcr p15, 0, r0, c1, c0, 0 \n"
        :::"r0"
    );
}

__tlb_init_code void mmu_enable_alignfault(void)
{
    asm volatile
    (
        "mrc p15, 0, r0, c1, c0, 0 \n"
        "orr r0, r0, #1 \n"
        "mcr p15, 0, r0, c1, c0, 0 \n"
        :::"r0"
    );
}

__tlb_init_code void mmu_disable_alignfault(void)
{
    asm volatile
    (
        "mrc p15, 0, r0, c1, c0, 0 \n"
        "bic r0, r0, #1 \n"
        "mcr p15, 0, r0, c1, c0, 0 \n"
        :::"r0"
    );
}

__tlb_init_code void mmu_enable_rom_protection(void)
{
    asm volatile
    (
        "mrc p15, 0, r0, c1, c0, 0 \n"
        "orr r0, r0, #0x200 \n"
        "mcr p15, 0, r0, c1, c0, 0 \n"
        :::"r0"
    );
}

__tlb_init_code void mmu_enable_system_protection(void)
{
    asm volatile
    (
        "mrc p15, 0, r0, c1, c0, 0 \n"
        "orr r0, r0, #0x100 \n"
        "mcr p15, 0, r0, c1, c0, 0 \n"
        :::"r0"
    );
}

__tlb_init_code void mmu_clean_invalidated_cache_index(int index)
{
    asm volatile("mcr p15, 0, %0, c7, c14, 2": :"r"(index));
}

__tlb_init_code void mmu_clean_invalidated_dcache(uint32_t buffer, uint32_t size)
{
    unsigned int ptr;

    ptr = buffer & ~(CACHE_LINE_SIZE - 1);

    while (ptr < buffer + size)
    {
        asm volatile("mcr p15, 0, %0, c7, c14, 1": :"r"(ptr));

        ptr += CACHE_LINE_SIZE;
    }
}

__tlb_init_code void mmu_clean_dcache(uint32_t buffer, uint32_t size)
{
    unsigned int ptr;

    ptr = buffer & ~(CACHE_LINE_SIZE - 1);

    while (ptr < buffer + size)
    {
        asm volatile("mcr p15, 0, %0, c7, c10, 1": :"r"(ptr));

        ptr += CACHE_LINE_SIZE;
    }
}

__tlb_init_code void mmu_invalidate_dcache(uint32_t buffer, uint32_t size)
{
    unsigned int ptr;

    ptr = buffer & ~(CACHE_LINE_SIZE - 1);

    while (ptr < buffer + size)
    {
        asm volatile("mcr p15, 0, %0, c7, c6, 1": :"r"(ptr));

        ptr += CACHE_LINE_SIZE;
    }
}

__tlb_init_code void mmu_invalidate_tlb(void)
{
    asm volatile("mcr p15, 0, %0, c8, c7, 0": :"r"(0));
}

__tlb_init_code void mmu_invalidate_icache(void)
{
    asm volatile("mcr p15, 0, %0, c7, c5, 0": :"r"(0));
}

__tlb_init_code void mmu_invalidate_dcache_all(void)
{
    asm volatile("mcr p15, 0, %0, c7, c6, 0": :"r"(0));
}

static void __mmu_set_domain(register uint32_t i)
{
    asm volatile("mcr p15,0, %0, c3, c0,  0": :"r"(i));
}

void mmu_set_domain(void)
{
    __mmu_set_domain(MEMS_DOMAIN_ACCESS_VALUE);
}

__tlb_init_code void mmu_test_clean_flush_cache(void)
{
    asm volatile
    (
        // write back data cache
        "1:  mrc p15, 0, r15, c7, c14, 3                    \n\t"
        "    bne 1b                                         \n\t"
        // drain write buffer
        "    mcr p15, 0, %0, c7, c7, 0                      \n\t"
        "    mcr p15, 0, %0, c7, c10, 4                     \n\t"
        : :
        "r"(0)
    );
}

__tlb_init_code void mmu_setttbase(register uint32_t i)
{
    register uint32_t value;

    /* Invalidates all TLBs.Domain access is selected as
     * client by configuring domain access register,
     * in that case access controlled by permission value
     * set by page table entry
     */
    value = 0;

    asm volatile("mcr p15, 0, %0, c8, c7, 0"::"r"(value));
    value = 0x55555555;
    asm volatile("mcr p15, 0, %0, c3, c0, 0"::"r"(value));
    asm volatile("mcr p15, 0, %0, c2, c0, 0"::"r"(i));
}

__tlb_init_code static void mmu_setmtt(uint32_t vaddr_begin, uint32_t vaddr_end,
                                       uint32_t paddr_begin, uint32_t attr)
{
    volatile uint32_t *ptt;
    volatile int nsec;
    int i = 0;

    ptt = tlb_pbase + (vaddr_begin >> 20);
    nsec = (vaddr_end >> 20) - (vaddr_begin >> 20);

    for (i = 0; i < nsec; i ++)
    {
        *ptt = attr | (((paddr_begin >> 20) + i) << 20);
        ptt ++;
    }

    return;
}

__tlb_init_code static void mmu_zero_tlb(void)
{
    uint32_t i;

    for (i = 0; i < tlb_size / 4; i ++)
    {
        tlb_pbase[i] = 0;
    }
}

__tlb_init_code static void mmu_build_direct_page_table(void)
{
    int32_t i;

    /*直接将sdram从物理地址空间直接MAP到地址相同的逻辑地址空间，*/
    /*用于初始化代码的地址映射，避免程序跑飞，使用1M的段项*/
    mmu_setmtt(CONFIG_DRAM_PHYBASE, CONFIG_DRAM_PHYBASE + CONFIG_DRAM_SIZE, CONFIG_DRAM_PHYBASE, \
               MEMS_TLB_L1_PTE_SECTION | 0x00000010 | MEMS_TLB_L1_AP11 | MEMS_TLB_DOMAIN15);

    mmu_setmtt(CONFIG_DRAM_VIRTBASE, CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_SIZE, CONFIG_DRAM_PHYBASE, \
               MEMS_TLB_L1_PTE_SECTION | 0x00000010 | MEMS_TLB_L1_AP11 | MEMS_TLB_WBUF_ON | MEMS_TLB_CACHE_ON | MEMS_TLB_DOMAIN15);
    dsb();
}

#ifdef CONFIG_SOC_SUN3IW2
static struct module_info_item g_module_info_items[] __tlb_init_data =
{
    {0x00000000,   0x4000,   CSP_GLOBAL_MOD_TYPE_MEMORY},       //SRAM 16K
    {0x00004000,   0x8000,   CSP_GLOBAL_MOD_TYPE_MEMORY},       //SRAM C 32K

    {0x01000000,   0x400000,   CSP_GLOBAL_MOD_TYPE_IO},         //DE 4M
    {0x01c00000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //SYS CONTROL 4K
    {0x01c02000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //DMA 4K
    {0x01c03000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //TS 4K
    {0x01c05000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //SPI0 4K
    {0x01c06000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //SPI1 4K
    {0x01c0e000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //VE 4K
    {0x01c0f000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //SMHC0 4K
    {0x01c10000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //SMHC1 4K
    {0x01c13000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //USB-OTG 4K
    {0x01c14000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //USB-EHIC0/OHIC0 4K
    {0x01C15000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //CE 4K
    {0x01C16000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //SID 4K
    {0x01C18000,   0x8000,   CSP_GLOBAL_MOD_TYPE_IO},           //DTMB 32K
    {0x01C20000,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //CCM 1K
    {0x01C20400,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //INTC 1K
    {0x01C20800,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //GPIO 1K
    {0x01C20C00,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //TIMER 1K
    {0x01C21000,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //PWM 1K
    {0x01C21400,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //RTC 1K
    {0x01C22000,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //DAUDIO0 1K
    {0x01C2C000,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //CIR 1K
    {0x01C23400,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //KEYADC 1K
    {0x01C23C00,   0x800,   CSP_GLOBAL_MOD_TYPE_IO},            //AUDIO CODEC 2K
    {0x01C24400,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //RTP 1K
    {0x01C25000,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //UART0 1K
    {0x01C25400,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //UART1 1K
    {0x01C25800,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //UART2 1K
    {0x01C27000,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //TWI0 1K
    {0x01C27400,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //TWI1 1K
    {0x01C27800,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //TWI2 1K
    {0x01C27C00,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //SCR 1K
    {0x01C30000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //TVD_TOP 4K
    {0x01C31000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //TVD 4K
    {0x01C60000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //HSTMR 4K
    {0x01C62000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //DRAMCOM 4K
    {0x01C63000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //DRAM_CTRL 4K
    {0x01C70000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //TCON_TOP 4K
    {0x01C71000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //TCON_LCD0 4K
    {0x01C73000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //TCON_TV0 4K
    {0x01C90000,   0x4000,   CSP_GLOBAL_MOD_TYPE_IO},           //TVE_TOP 16K
    {0x01C94000,   0x4000,   CSP_GLOBAL_MOD_TYPE_IO},           //TVE 16K
    {0x01CA0000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //MIPI_DSI 4K
    {0x01CA1000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //MIPI DSI-PHY 4K
    {0x01CA2000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //DTMB_ADC 4K
    {0x01E70000,   0x20000,   CSP_GLOBAL_MOD_TYPE_IO},          //DE INTERLACE 128K
};

#elif defined CONFIG_SOC_SUN3IW1
static struct module_info_item g_module_info_items[] __tlb_init_data =
{
    {0x00000000,   0x2000,   CSP_GLOBAL_MOD_TYPE_MEMORY},       //sram A
    {0x00008000,   0x800,   CSP_GLOBAL_MOD_TYPE_MEMORY},        //USB SRam

    /*AHB controller begin*/
    {0x01c00000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //sram
    {0x01c01000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //sdram
    {0x01c02000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //dma
    {0x01c03000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //nfc
    //{0x01c04000   ,   0x1000  ,   CSP_GLOBAL_MOD_TYPE_IO},    //TSC
    {0x01c05000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //SPI0
    {0x01c06000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //SPI1
    //{0x01c07000   ,   0x1000  ,   CSP_GLOBAL_MOD_TYPE_IO},    //MSC
    //{0x01c08000   ,   0x1000  ,   CSP_GLOBAL_MOD_TYPE_IO},    //reserved
    //{0x01c09000   ,   0x1000  ,   CSP_GLOBAL_MOD_TYPE_IO},    //CSI0
    {0x01c0A000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //TVENC
    {0x01c0B000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //TVD
    {0x01c0C000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //TCON0
    //{0x01c0D000   ,   0x1000  ,   CSP_GLOBAL_MOD_TYPE_IO},    //reserved
    {0x01c0E000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //VE
    {0x01c0F000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //SDC0
    //{0x01c10000   ,   0x1000  ,   CSP_GLOBAL_MOD_TYPE_IO},    //SDC1
    {0x01c11000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //SDC2
    //{0x01c12000   ,   0x1000  ,   CSP_GLOBAL_MOD_TYPE_IO},    //SDC3
    {0x01C13000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //USB0

    {0x01E00000,   0x20000,   CSP_GLOBAL_MOD_TYPE_IO},          //DE_FE 128K
    //{0x01E20000   ,   0x20000 ,   CSP_GLOBAL_MOD_TYPE_IO},    //DE_SC1 128K
    //{0x01E40000   ,   0x20000 ,   CSP_GLOBAL_MOD_TYPE_IO},    //DE_IM1 128K
    {0x01E60000,   0x20000,   CSP_GLOBAL_MOD_TYPE_IO},          //DE_BE 128K
    /*AHB controller End*/

    /*Apb controller begin*/
    {0x01c20000,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //CCM/PWM
    {0x01c20400,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //INTC
    {0x01c20800,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //PORTC
    {0x01c20C00,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //TMRC
    {0x01c21000,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //UART0
    {0x01c21400,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //UART1
    {0x01c21800,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //UART2
    //{0x01c21C00   ,   0x400   ,   CSP_GLOBAL_MOD_TYPE_IO},    //UART3
    {0x01c22000,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //DAUDIO interface
    //{0x01c22400   ,   0x400   ,   CSP_GLOBAL_MOD_TYPE_IO},    //PS2 IF
    //{0x01c12800   ,   0x400   ,   CSP_GLOBAL_MOD_TYPE_IO},    //AC97
    {0x01c22C00,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //IRC
    //{0x01c13000   ,   0x400   ,   CSP_GLOBAL_MOD_TYPE_IO},    //I2S IF
    {0x01c23400,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //LRADC
    //{0x01c13800   ,   0x400   ,   CSP_GLOBAL_MOD_TYPE_IO},    //reserved
    {0x01c23C00,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //Audio Codec
    {0x01c24800,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //TPC
    {0x01c25000,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //UART0
    {0x01c25400,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //UART1
    {0x01c25800,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //UART2

    {0x01c27000,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //TWI0
    {0x01c27400,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //TWI1
    {0x01c27800,   0x400,   CSP_GLOBAL_MOD_TYPE_IO},            //TWI2
    {0x01cb0000,   0x1000,   CSP_GLOBAL_MOD_TYPE_IO},           //csi
    {0x01e70000,   0x10000,   CSP_GLOBAL_MOD_TYPE_IO},          //deinterlace
    /*APB controller End*/
};
#endif

static __tlb_init_code struct ic_module_info *get_io_map(void)
{
    soc_io_info.module_nr = sizeof(g_module_info_items) / sizeof(g_module_info_items[0]);
    soc_io_info.items = g_module_info_items;

    return &soc_io_info;
}

static __tlb_init_code void mmu_build_ioarea(void)
{
    int32_t          i;
    ic_module_info_t    *tmpIoArea;
    module_info_item_t  *tmpIoItem;
    uint32_t          virt_start;
    uint32_t          virt_size;
    uint32_t          phy_start;

    //get io area information from csp module
    tmpIoArea = get_io_map();

    for (i = 0; i < tmpIoArea->module_nr; i++)
    {
        tmpIoItem = &tmpIoArea->items[i];
        if (tmpIoItem->mod_type == CSP_GLOBAL_MOD_TYPE_IO)
        {
            virt_start = tmpIoItem->phy_base_addr + CONFIG_IOREGS_VBASE;
            virt_size = (((tmpIoItem->phy_io_size + 0xfffff) >> 20) << 20);
            phy_start = tmpIoItem->phy_base_addr;

            mmu_setmtt(virt_start, virt_start + virt_size, phy_start, \
                       MEMS_TLB_L1_PTE_SECTION | MEMS_TLB_L1_AP11 | 0x00000010 | MEMS_TLB_DOMAIN15);

        }
        else if (tmpIoItem->mod_type == CSP_GLOBAL_MOD_TYPE_MEMORY)
        {
            virt_start = tmpIoItem->phy_base_addr + CONFIG_SRAM_VBASE;
            virt_size = (((tmpIoItem->phy_io_size + 0xfffff) >> 20) << 20);
            phy_start = tmpIoItem->phy_base_addr;

            mmu_setmtt(virt_start, virt_start + virt_size, phy_start, \
                       MEMS_TLB_L1_PTE_SECTION | MEMS_TLB_L1_AP11 | 0x00000010 | MEMS_TLB_DOMAIN15);
        }
    }

    dsb();
    return;
}

__tlb_init_code static int32_t mmu_build_highmem_area(void)
{
    uint32_t *l2;
    uint32_t pte_vec;
    uint32_t pte_clk;
    uint32_t attr1, attr2;
    uint32_t vector_base;
    int32_t  i;

    extern uint8_t __head_2ndtlb_high_start[];
#define IRQ_PAGE1_4K_OFFSET  (0x0000)
#define IRQ_PAGE2_4K_OFFSET  (0x1000)
#define CLK_PAGE1_4K_OFFSET  (0x2000)
#define CLK_PAGE2_4K_OFFSET  (0x3000)
#define CLK_PAGE3_4K_OFFSET  (0x4000)
#define CLK_PAGE4_4K_OFFSET  (0x5000)
#define CLK_PAGE5_4K_OFFSET  (0x6000)
#define L2_OFFSET            (0x7000)

    attr1 =  MEMS_TLB_L2_PTE_SMALL | MEMS_TLB_COASE_L2_AP00 | MEMS_TLB_WBUF_ON | MEMS_TLB_CACHE_ON;
    attr2 =  MEMS_TLB_L2_PTE_SMALL | MEMS_TLB_COASE_L2_AP11 | MEMS_TLB_WBUF_ON | MEMS_TLB_CACHE_ON;

    l2 = (uint32_t *)&__head_2ndtlb_high_start[L2_OFFSET];

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
    l2[pte_vec + 0] = (uint32_t)&__head_2ndtlb_high_start[IRQ_PAGE1_4K_OFFSET] | attr1;
    l2[pte_vec + 1] = (uint32_t)&__head_2ndtlb_high_start[IRQ_PAGE2_4K_OFFSET] | attr2;

    //create clk code and data area, 20k bytes enough.
    pte_clk = (CONFIG_CLK_TUNING_VBASE >> 12) & (256 - 1);
    l2[pte_clk + 0] = (uint32_t)&__head_2ndtlb_high_start[CLK_PAGE1_4K_OFFSET] | attr2;
    l2[pte_clk + 1] = (uint32_t)&__head_2ndtlb_high_start[CLK_PAGE2_4K_OFFSET] | attr2;
    l2[pte_clk + 2] = (uint32_t)&__head_2ndtlb_high_start[CLK_PAGE3_4K_OFFSET] | attr2;
    l2[pte_clk + 3] = (uint32_t)&__head_2ndtlb_high_start[CLK_PAGE4_4K_OFFSET] | attr2;
    l2[pte_clk + 4] = (uint32_t)&__head_2ndtlb_high_start[CLK_PAGE5_4K_OFFSET] | attr2;

    // link l2 page table to l1 page directory.
    tlb_pbase[vector_base >> 20] = (uint32_t)l2 | \
                                   MEMS_TLB_L1_PTE_COARSE | \
                                   MEMS_TLB_DOMAIN14 | (1 << 4);
    return 0;
}

extern uint8_t __readonly_area_start[];
extern uint8_t __readonly_area_end[];
extern uint32_t __head_2ndtlb_krnl_start[];
__tlb_init_code void mmu_build_readonlyarea(void)
{
    int32_t i;
    int32_t sindex, eindex;

    if ((uint32_t)&__readonly_area_start[0] & 0xfff)
    {
        while (1);
    }

    if ((uint32_t)&__readonly_area_end[0] & 0xfff)
    {
        while (1);
    }

    if ((uint32_t)&__head_2ndtlb_krnl_start[0] & 0x3ff)
    {
        while (1);
    }

    for (i = 0; i < 256; i ++)
    {
        __head_2ndtlb_krnl_start[i] = 0;
    }

    for (i = 0; i < 256; i ++)
    {
        /* fill the l2 descriptor */
        __head_2ndtlb_krnl_start[i] = (((CONFIG_DRAM_PHYBASE >> 12) + i) << 12) |
                                      MEMS_TLB_L2_PTE_SMALL | MEMS_TLB_WBUF_ON | MEMS_TLB_CACHE_ON | MEMS_TLB_COASE_L2_AP11;
    }

    /*  just first 1m area need to be protected. */
    sindex = (((uint32_t)&__readonly_area_start[0] - CONFIG_DRAM_VIRTBASE) >> 12);
    eindex = (((uint32_t)&__readonly_area_end[0] - CONFIG_DRAM_VIRTBASE) >> 12);

    for (i = sindex; i < eindex; i ++)
    {
        /* modify the l2 descriptor */
        __head_2ndtlb_krnl_start[i] = (((CONFIG_DRAM_PHYBASE >> 12) + i) << 12) |
                                      MEMS_TLB_L2_PTE_SMALL | MEMS_TLB_WBUF_ON | MEMS_TLB_CACHE_ON | MEMS_TLB_COASE_L2_AP00;
    }
    /* modify the first 1m section descriptor to new l1 corse descriptor */
    tlb_pbase[CONFIG_DRAM_VIRTBASE >> 20] = MEMS_TLB_L1_PTE_COARSE | 0x00000010 | MEMS_TLB_DOMAIN14 | ((((uint32_t)&__head_2ndtlb_krnl_start[0]) >> 10) << 10);
}

__tlb_init_code int32_t mmu_create_map(void)
{
    uint32_t domain = 0xf0000000;
    mmu_zero_tlb();

    mmu_build_direct_page_table();
    mmu_build_ioarea();
    mmu_build_highmem_area();
    mmu_build_readonlyarea();

    asm volatile("mcr p15,0, %0, c3, c0,  0": :"r"(domain));
    cpu_set_ttbr((uint32_t)tlb_pbase);

    mmu_enable_rom_protection();
    mmu_enable();
    mmu_enable_icache();
    mmu_enable_dcache();
    mmu_enable_alignfault();

    mmu_invalidate_tlb();
    mmu_invalidate_icache();
    mmu_invalidate_dcache_all();
    mmu_test_clean_flush_cache();

    awos_arch_lock_tlb_zone(CONFIG_DRAM_VIRTBASE + 0x00000000);
    awos_arch_lock_tlb_zone(CONFIG_DRAM_VIRTBASE + 0x00100000);
    awos_arch_lock_tlb_zone(CONFIG_DRAM_VIRTBASE + 0x00200000);
    cpu_cache_clean_inv();

    dsb();
    return 0;
}

__tlb_init_code static void copy_boot_para(const __epos_para_t *to, const __epos_para_t *from)
{
    uint8_t *src;
    uint8_t *dst;
    uint32_t count = sizeof(__epos_para_t);

    src = (uint8_t *)from;
    dst = (uint8_t *)to;

    while (count --)
    {
        *dst ++ = *src ++;
    }

    dsb();
    return;
}

__tlb_init_code void mmu_init(void)
{
    copy_boot_para(&boot_param, melis_head_para_get());
    mmu_create_map();
    copy_boot_para(&kernel_param, &boot_param);
}

static void mmu_reload_tlb(void)
{
    register uint32_t value;

    value = 0;
    asm volatile("mcr p15, 0, %0, c7, c10, 4": :"r"(value));

    value = 0;
    asm volatile("mcr p15, 0, %0, c8, c7, 0": :"r"(value));
}

void mmu_unmap(void)
{
    uint32_t i;

    for (i = 0; i < (CONFIG_DRAM_SIZE >> 20); i ++)
    {
        tlb_vbase[i + (CONFIG_DRAM_PHYBASE >> 20)] = 0;
    }

    cpu_tlb_invalidate_all();
    cpu_cache_clean_inv();
    dsb();
}

static void (*const ctors [])(void) __attribute__((used, section(".ctors"), aligned(sizeof(void *)))) = \
{
    mmu_unmap,
    mmu_set_domain
};

static void (*const dtors [])(void) __attribute__((used, section(".dtors"), aligned(sizeof(void *)))) = \
{
    NULL
};

static uint32_t awos_arch_virt_to_phys(uint32_t virtaddr)
{
    return virtaddr - CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_PHYBASE;
}

uint32_t awos_arch_phys_to_virt(uint32_t phyaddr)
{
    return phyaddr + (CONFIG_DRAM_VIRTBASE - CONFIG_DRAM_PHYBASE);
}

uint32_t awos_arch_bus_to_virt(uint32_t phyaddr)
{
    return (CONFIG_IOREGS_VBASE + phyaddr);
}

static int32_t create_page_small(uint32_t vmaddr, uint32_t *l2tbl_addr, uint32_t npte, uint32_t *nalloc)
{
    uint32_t   ptestart;       //start of page table entry
    uint32_t   page_vaddr;      //page virtual address
    uint32_t   page_paddr;      //page physical address

    //the first page table entry

    ptestart = (vmaddr >> 12) & (256 - 1);

    //in order to improve the vm create performance,
    //operate system page pool directly.
    //check if the area is valid
    for (; ;)
    {
        if (npte == 0)
        {
            //create L2 small page table succeeded.
            return 0;
        }

        if (l2tbl_addr[ptestart] == 0)
        {
            page_vaddr = (uint32_t)k_malloc_align(4096, 4096);
            if (page_vaddr == 0 || (page_vaddr & ALIGN_MASK(4096)))
            {
                __err("alloc memory failure, page_vaddr = 0x%08x.", page_vaddr);
                software_break();
                break;
            }

            page_paddr = awos_arch_virt_to_phys(page_vaddr);
            l2tbl_addr[ptestart] = page_paddr
                                   | MEMS_TLB_L2_PTE_SMALL
                                   | MEMS_TLB_COASE_L2_AP11
                                   | MEMS_TLB_WBUF_ON
                                   | MEMS_TLB_CACHE_ON;

            //allocate one page table entry
            (*nalloc)++;
        }

        //next page table entry
        ptestart++;
        npte--;
    }

    return -1;
}

void memory_info(void);
static void create_page_large(uint32_t vmaddr, uint32_t *l2tbl_addr, uint32_t *npte, uint32_t *nalloc)
{
    uint32_t   ptestart;       //start of page table entry
    uint32_t   page_vaddr;      //page virtual address
    uint32_t   page_paddr;      //page phyical address
    uint32_t   PTEIndex;

    //the first page table entry
    ptestart = (vmaddr >> 12) & (256 - 1);

    //scan aligned 64k free pages
    for (; ;)
    {
        if (*npte < 16)
        {
            //create L2 small page table succeeded.
            return;
        }

        page_vaddr = (uint32_t)k_malloc_align(64 * 1024, 64 * 1024);
        if ((page_vaddr == 0) || (page_vaddr & ALIGN_MASK(64 * 1024)))
        {
            __err("alloc memory failure. page_vaddr = 0x%08x", page_vaddr);
            memory_info();
            software_break();
            return;
        }

        if ((l2tbl_addr[ptestart] & 3) == MEMS_TLB_L2_PTE_SMALL)
        {
            uint32_t i;

            //find one 4k page table entry,
            //check the next 60k is created as small page.
            for (i = 1; i < 16; i++)
            {
                if ((l2tbl_addr[ptestart + i] & 3) != MEMS_TLB_L2_PTE_SMALL)
                {
                    //find none 4k page table entry,
                    //we must create the left pages as 4k small pages.
                    //Free 64k
                    k_free_align((void *)page_vaddr, 64 * 1024);
                    return;
                }
            }
        }

        if (l2tbl_addr[ptestart] == 0)
        {
            //create one large page table entry.
            page_paddr = awos_arch_virt_to_phys(page_vaddr);
            for (PTEIndex = 0; PTEIndex < 16; PTEIndex++)
            {
                l2tbl_addr[ptestart + PTEIndex] = page_paddr
                                                  | MEMS_TLB_L2_PTE_LARGE
                                                  | MEMS_TLB_COASE_L2_AP11
                                                  | MEMS_TLB_WBUF_ON
                                                  | MEMS_TLB_CACHE_ON;
            }
            (*nalloc) += 16;
        }

        vmaddr   += (64 * 1024);
        (*npte)  -= 16;
        ptestart += 16;

        if ((*npte) < 16)
        {
            //large PTE create succeeded
            return;
        }
    }
}

static int32_t create_l2pte(uint32_t vmaddr, uint32_t *l2tbl_addr, uint32_t npte, uint32_t *nalloc)
{
    uint32_t   ptestart;
    uint32_t   npte_s;
    uint32_t   npte_l;


    //start index of page table entry
    ptestart = (vmaddr >> 12) & (256 - 1);

    //create the head unaligned pages as small page
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

    //try to create 64k aligned as large page
    if (npte >= 16)
    {
        npte_l = npte;
        create_page_large(vmaddr, l2tbl_addr, &npte_l, nalloc);
        npte_l = npte - npte_l;
        vmaddr = vmaddr + npte_l * (4 * 1024);
        npte  -= npte_l;
    }

    //create the tail unligned pages as small pages
    if (npte)
    {
        if (create_page_small(vmaddr, l2tbl_addr, npte, nalloc) != 0)
        {
            __err("create VM [%x] L2 page table failed", vmaddr);
            return -1;
        }
    }

    //create L2 page table entry succeeded
    return 0;
}

static int32_t vmem_create_section(uint32_t vmaddr, uint8_t Domain, uint32_t *nalloc)
{
    uint32_t page_vaddr;
    uint32_t page_paddr;

    /*software_break();  // Too waste of memory, stop at this point.*/

    //alloc 1M memory aligned by 1M.
    page_vaddr = (uint32_t)k_malloc_align(1 * 1024 * 1024, 1 * 1024 * 1024);
    if ((page_vaddr == 0) || (page_vaddr & ALIGN_MASK(1 * 1024 * 1024)))
    {
        __err("alloc memory failure.");
        software_break();
        return -1;
    }

    page_paddr = awos_arch_virt_to_phys(page_vaddr);
    tlb_vbase[vmaddr >> 20] = page_paddr
                              | MEMS_TLB_L1_PTE_SECTION
                              | 0x00000010
                              | MEMS_TLB_L2_AP11
                              | MEMS_TLB_WBUF_ON
                              | MEMS_TLB_CACHE_ON
                              | MEMS_TLB_DOMAIN15;
    (*nalloc) += 256;

    return 0;
}

static int32_t vmem_create_1m(uint32_t vmaddr, uint32_t npte, uint8_t domain, uint32_t *nalloc)
{
    uint32_t   index;
    uint32_t   *l2tbl_addr;
    int32_t    res;

    //lock system shed
    esKRNL_SchedLock();

    //try to create the L1 page table entry
    if ((tlb_vbase[vmaddr >> 20] & 3) == 0)
    {
        if (((vmaddr & (1024 * 1024 - 1)) == 0) && (npte == 256))
        {
            //1m aligned virtual space to create,
            //try to create 1m section.
            if (vmem_create_section(vmaddr, domain, nalloc) == 0)
            {
                //create 1m section succeeded
                esKRNL_SchedUnlock();
                return 0;
            }

            //create 1m section failed,
            //try to create 4k small page table.
        }

        //the level 1 page table entry created already;
        //allocate memory for L2 page table entry;
        //single L2 page table entry size = 1K, 256 entries.
        l2tbl_addr = (uint32_t *)k_malloc(1024);
        if (l2tbl_addr == NULL || (((uint32_t)l2tbl_addr) & ALIGN_MASK(1024)))
        {
            esKRNL_SchedUnlock();
            __err("alloc memory failure.");
            software_break();
            return -1;
        }

        //link L2 page table address to L1 page table entry
        tlb_vbase[vmaddr >> 20] |= awos_arch_virt_to_phys((uint32_t)l2tbl_addr)
                                   | MEMS_TLB_L1_PTE_COARSE
                                   | (domain << 5)
                                   | (1 << 4);

        //initialize L2 page table entry
        for (index = 0; index < 256; index++)
        {
            l2tbl_addr[index] = 0;
        }
    }
    else if ((tlb_vbase[vmaddr >> 20] & 3) == MEMS_TLB_L1_PTE_SECTION)
    {
        //1m virtual space created already,
        //check the domain conflict or not.
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
        //check the domain conflict or not
        if ((tlb_vbase[vmaddr >> 20] & 0x000001e0) != (domain << 5))
        {
            __err("ERR:domain conflict, vmaddr = 0x%08x.", vmaddr);
            esKRNL_SchedUnlock();
            return -1;
        }
        //get the L2 page table physical address,
        //convert to virtual adress.
        //system L2 page table type : FINE PAGE.
        l2tbl_addr = (uint32_t *)(awos_arch_phys_to_virt(tlb_vbase[vmaddr >> 20] & 0xfffffc00));
    }

    //construct L2 page table
    res = create_l2pte(vmaddr, l2tbl_addr, npte, nalloc);
    esKRNL_SchedUnlock();

    return res;
}

int32_t vmem_delete_section(uint32_t vmaddr)
{
    uint32_t  phy_addr;

    //release allocated pages to system page pool.
    phy_addr = tlb_vbase[vmaddr >> 20] & 0xfff00000;
    //Free 1M
    k_free_align((void *)awos_arch_phys_to_virt(phy_addr), 1 * 1024 * 1024);

    //invalid L1 page table entry
    tlb_vbase[vmaddr >> 20] = 0;

    //1m section delete succeeded
    return 0;
}

int32_t delete_l2pte(uint32_t vmaddr, uint32_t *l2ptbl_addr, uint32_t npte)
{
    uint32_t  l2pte_idx;
    uint32_t  pte_phyaddr;
    uint32_t  i;

    //delete L2 page table entries
    l2pte_idx = (vmaddr >> 12) & (256 - 1);
    while (npte)
    {
        if (l2ptbl_addr[l2pte_idx] & 3)
        {
            if ((l2ptbl_addr[l2pte_idx] & 3) == MEMS_TLB_L2_PTE_LARGE)
            {
                //large page table entry,
                //release allocated pages to system page pool.
                pte_phyaddr = l2ptbl_addr[l2pte_idx] & 0xffff0000;

                //free 64K
                k_free_align((void *)awos_arch_phys_to_virt(pte_phyaddr), 64 * 1024);

                //invalid L2 page table entry
                for (i = 0; npte && (i < 16); i++, npte--, l2pte_idx++)
                {
                    l2ptbl_addr[l2pte_idx] = 0;
                }
            }
            else if ((l2ptbl_addr[l2pte_idx] & 3) == MEMS_TLB_L2_PTE_SMALL)
            {
                //small page table entry,
                //release allocated pages to system page pool.
                pte_phyaddr = l2ptbl_addr[l2pte_idx] & 0xfffff000;

                //free 4K
                k_free_align((void *)awos_arch_phys_to_virt(pte_phyaddr), 4096);
                //invalid L2 page table entry
                l2ptbl_addr[l2pte_idx] = 0;

                l2pte_idx++;
                npte--;
            }
            else
            {
                //large and tiny page not support
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

    //L2 page tabl entry delete succeeded
    return 0;
}

static void vmem_delete_1m(uint32_t vmaddr, uint32_t npte)
{
    uint32_t  l2pte_idx;
    uint32_t  l2pte_tmp;
    uint32_t *l2tbl_addr;

    esKRNL_SchedLock();

    //get the L2 page table address by L1 page table entry.
    switch ((tlb_vbase[vmaddr >> 20] & 3))
    {
        case  MEMS_TLB_L1_PTE_ERR:
        {
            //fault section
            esKRNL_SchedUnlock();
            return;
        }
        case MEMS_TLB_L1_PTE_SECTION:
        {
            //section to delete, virtual address must aligned by 1m,
            //1m virtual space shoudl delete.
            if ((vmaddr & (1024 * 1024 - 1)) || (npte != 256))
            {
                __err("invalid 1m virtual memory space to delete");
                esKRNL_SchedUnlock();
                return;
            }

            //delete 1m section
            vmem_delete_section(vmaddr);

            esKRNL_SchedUnlock();
            return;
        }
        case MEMS_TLB_L1_PTE_COARSE:
        {
            //coarse page type,
            //get L2 page table virtual adress
            l2tbl_addr = (uint32_t *)awos_arch_phys_to_virt((tlb_vbase[vmaddr >> 20] & 0xfffffc00));
            break;
        }
        default:
        {
            //1M section
            __err("1M section and fine page not support");
            esKRNL_SchedUnlock();
            return;
        }
    }

    //delete L2 page table entries
    delete_l2pte(vmaddr, l2tbl_addr, npte);

    //check L1 1M section is used or not
    l2pte_tmp = 0;
    for (l2pte_idx = 0; l2pte_idx < 256; l2pte_idx++)
    {
        l2pte_tmp |= l2tbl_addr[l2pte_idx];
    }

    if (l2pte_tmp == 0)
    {
        //the L1 1M section not used by anyone,
        //delete L2 table.
        //Free 1K
        k_free((void *)l2tbl_addr);

        //delete L1 item in the tlb
        tlb_vbase[(vmaddr) >> 20] = 0;
    }

    esKRNL_SchedUnlock();
    return;
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

    //calc the page table entry number,
    //parameter npage unit : 1k.
    ptenum   = (npage + 4 - 1) >> 2;

    //calc start of L2 page table entry
    ptestart = (vmaddr >> 12) & (256 - 1);

    if ((ptestart + ptenum) <= 256)
    {
        //the delete virtual space not span 1M boundary,
        //delete it directly once.
        vmem_delete_1m(vmaddr, ptenum);
        return;
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
    //virtual space delete succeeded.
    awos_arch_test_clean_flush_cache();
    cpu_tlb_invalidate_all();
    return;
}

int32_t awos_arch_vmem_create(uint8_t *virtaddr, uint32_t npage, uint8_t domain)
{
    uint32_t           vmaddr;
    uint32_t           ptenum;
    uint32_t           ptestart;
    uint32_t           nalloc;

    if (awos_arch_isin_irq() != 0)
    {
        __err("cannot call vmem_create from isr!");
        return -1;
    }

    vmaddr = (uint32_t)virtaddr;
    if (vmaddr & ((4 * 1024) - 1))
    {
        __err("invalid adress for vm ctreate, must aligned by 4K");
        return -1;
    }

    //calc the page table entry number,
    //parameter npage unit : 1k.
    ptenum   = (npage + 4 - 1) >> 2;

    //calc start of L2 page table entry
    ptestart = (vmaddr >> 12) & (256 - 1);

    if ((ptestart + ptenum) <= 256)
    {
        //the virtual space not exceed 1M boundary, create once.
        if (vmem_create_1m(vmaddr, ptenum, domain, &nalloc) != 0)
        {
            //create 1M space failed, release allocated memory.
            awos_arch_vmem_delete((uint8_t *)vmaddr, nalloc * 4);
            return -1;
        }
    }
    else
    {
        //create the first unaligned 1M virtual space
        if (vmem_create_1m(vmaddr, (256 - ptestart), domain, &nalloc) != 0)
        {
            //create 1M space failed, release allocated memory.
            awos_arch_vmem_delete((uint8_t *)vmaddr, nalloc * 4);
            return -1;
        }

        //1M aligned virtual space
        ptenum -= (256 - ptestart);
        vmaddr  = (vmaddr & 0xfff00000) + 1 * 1024 * 1024;

        //create the aligned 1M virtual space
        while (ptenum > 256)
        {
            if (vmem_create_1m(vmaddr, 256, domain, &nalloc) != 0)
            {
                //create 1M space failed, release allocated memory.
                awos_arch_vmem_delete((uint8_t *)vmaddr, nalloc * 4);
                return -1;
            }

            //the next aligned 1M
            ptenum -= 256;
            vmaddr += (1 * 1024 * 1024);
        }

        //create the end 1M virtual space
        if (ptenum)
        {
            if (vmem_create_1m(vmaddr, ptenum, domain, &nalloc) != 0)
            {
                //create 1M space failed, release allocated memory.
                awos_arch_vmem_delete((uint8_t *)vmaddr, nalloc * 4);
                return -1;
            }
        }
    }

    dsb();
    awos_arch_test_clean_flush_cache();
    cpu_tlb_invalidate_all();
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
