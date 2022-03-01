/*
 * ===========================================================================================
 *
 *       Filename:  mmu.h
 *
 *    Description:  type define of TLB unit target to ARM926EJ-S.
 *
 *        Version:  Melis3.0
 *         Create:  2018-05-23 18:09:29
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-03-24 13:28:39
 *
 * ===========================================================================================
 */

#ifndef __MMU_I_H__
#define __MMU_I_H__
#include <typedef.h>
#include <kconfig.h>

#define DESC_SEC       (0x2)
#define MEMWBWAYC      ((1<<12)|(3<<2))     /* write back, write allocate,cache enable */

// Out and Inner Non-cacheable, refer DDI0406C_C_arm_architecture_reference_manual.pdf
// B3.8.2 Short-descriptor format memory region attributes, without TEX remap
#define MEMWBWANC      ((1<<12)|(0<<2))     /* write back, write allocate,cache disable */
#define MEMWB          (3<<2)  /* write back, no write allocate */
#define MEMWT          (2<<2)  /* write through, no write allocate */
#define SHAREDEVICE    (1<<2)  /* shared device */
#define STRONGORDER    (0<<2)  /* strong ordered */
#define XN             (1<<4)  /* eXecute Never */
#define AP_RW          (3<<10) /* supervisor=RW, user=RW */
#define AP_RO          (2<<10) /* supervisor=RW, user=RO */
#define SHARED         (1<<16) /* shareable */
#define NS             (1<<19) /* access in ns mode. */

#define DOMAIN_FAULT   (0x0)
#define DOMAIN_CHK     (0x1)
#define DOMAIN_NOTCHK  (0x3)
#define DOMAIN0        (0x0<<5)
#define DOMAIN1        (0x1<<5)
#define DOMAIN15       (0xf<<5)

#define DOMAIN0_ATTR   (DOMAIN_CHK<<0)
#define DOMAIN1_ATTR   (DOMAIN_FAULT<<2)

#define NORMAL_MEM_CACHED   (SHARED|AP_RW|DOMAIN0|MEMWBWAYC|DESC_SEC)
#define NORMAL_MEM_UNCACHED (SHARED|AP_RW|DOMAIN0|MEMWBWANC|DESC_SEC)
#define NORMAL_MEM_UNCACHED_COHERENT (SHARED|AP_RW|DOMAIN0|MEMWBWANC|DESC_SEC|XN)

/* device mapping type */
#define DEVICE_MEM     (SHARED|AP_RW|DOMAIN0|SHAREDEVICE|DESC_SEC|XN)

/* normal memory mapping type */
#ifndef CONFIG_CPU_DCACHE_DISABLE
#define NORMAL_MEM     (NORMAL_MEM_CACHED)
#else
#define NORMAL_MEM     (NORMAL_MEM_UNCACHED)
#endif

/* sram memory mapping type */
#define SRAM_MEM        (NORMAL_MEM)

typedef struct mem_desc
{
    uint32_t vaddr_start;
    uint32_t vaddr_end;
    uint32_t paddr_start;
    uint32_t attr;
} mem_desc_t;

typedef struct module_info_item
{
    uint32_t phy_base_addr;            //module`s io memory base address
    uint32_t phy_io_size;              //io memory space
    uint32_t mod_type;                 //such as CSP_GLOBAL_MOD_TYPE_NULL
} module_info_item_t;

typedef struct ic_module_info
{
    uint32_t module_nr;                //module number
    struct module_info_item *items;
} ic_module_info_t;

#define  __tlb_init_code            __attribute__((section(".tlb.code")))
#define  __tlb_init_data            __attribute__((section(".tlb.data")))

#define  CSP_GLOBAL_MODULE_NR       80
#define  CSP_GLOBAL_MOD_TYPE_NULL   0x00
#define  CSP_GLOBAL_MOD_TYPE_IO     0x01
#define  CSP_GLOBAL_MOD_TYPE_MEMORY 0x02

#define CACHE_LINE_SIZE             32
#define tlb_pbase                   ((uint32_t*)CONFIG_DRAM_PHYBASE)
#define tlb_vbase                   ((uint32_t*)CONFIG_DRAM_VIRTBASE)
#define tlb_size                    (16 * 1024)

// 页表项类型
#define MEMS_TLB_L1_PTE_SECTION     0x00000002
#define MEMS_TLB_L1_PTE_COARSE      0x00000001
#define MEMS_TLB_L1_PTE_FINE        0x00000003
#define MEMS_TLB_L1_PTE_ERR         0x00000000
#define MEMS_TLB_L2_PTE_LARGE       0x00000001
#define MEMS_TLB_L2_PTE_SMALL       0x00000002
#define MEMS_TLB_L2_PTE_TINY        0x00000003
#define MEMS_TLB_L2_PTE_ERR         0x00000000

// AP(读写位)
#define MEMS_TLB_L1_AP00            0x00000000
#define MEMS_TLB_L1_AP01            0x00000400
#define MEMS_TLB_L1_AP10            0x00000800
#define MEMS_TLB_L1_AP11            0x00000c00
#define MEMS_TLB_L2_AP00            0x00000000
#define MEMS_TLB_L2_AP01            0x00000010
#define MEMS_TLB_L2_AP10            0x00000020
#define MEMS_TLB_L2_AP11            0x00000030

#define MEMS_TLB_WBUF_ON            0x00000004
#define MEMS_TLB_CACHE_ON           0x00000008
#define MEMS_TLB_DOMAIN15           0x000001e0
#define MEMS_TLB_DOMAIN14           0x000001c0

#if defined CONFIG_KGDB
#define MEMS_DOMAIN_ACCESS_VALUE    0xf0000000    // 设置D15 and D14为管理者
#else
#define MEMS_DOMAIN_ACCESS_VALUE    0xd0000000    // 设置D15为管理者, d14 is client
#endif

// 粗页表AP
#define MEMS_TLB_COASE_L2_AP00      0x00000000
#define MEMS_TLB_COASE_L2_AP01      0x00000550
#define MEMS_TLB_COASE_L2_AP10      0x00000aa0
#define MEMS_TLB_COASE_L2_AP11      0x00000ff0

#define MEMS_DOMAIN_ACCESS_VALU     0xc0000000    // 设置D15为管理者
#define MEMS_ALIGN(val, align)      (((val) + ((align) - 1)) & ~((align) - 1))
#define MEMS_MIN(a, b)              ((a) < (b) ? (a) : (b))

#endif
