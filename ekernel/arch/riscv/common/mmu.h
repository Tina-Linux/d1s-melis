/*
 * ===========================================================================================
 *
 *       Filename:  mmu.h
 *
 *    Description:
 *
 *        Version:  Melis3.0
 *         Create:  2018-05-23 18:09:29
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  Zeng Zhijin
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-06-13 13:28:39
 *
 * ===========================================================================================
 */

#ifndef __MMU_I_H__
#define __MMU_I_H__
#include <typedef.h>
#include <kconfig.h>

#include "page.h"
#include "pgtable.h"

typedef struct mem_desc
{
    unsigned long vaddr_start;
    unsigned long vaddr_end;
    unsigned long paddr_start;
    pgprot_t attr;
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

#if 0
#define  __tlb_init_code            __attribute__((section(".tlb.code")))
#define  __tlb_init_data            __attribute__((section(".tlb.data")))
#else
#define  __tlb_init_code
#define  __tlb_init_data
#endif

#define  CSP_GLOBAL_MODULE_NR       80
#define  CSP_GLOBAL_MOD_TYPE_NULL   0x00
#define  CSP_GLOBAL_MOD_TYPE_IO     0x01
#define  CSP_GLOBAL_MOD_TYPE_MEMORY 0x02

#define tlb_pbase                   ((uint32_t*)CONFIG_DRAM_PHYBASE)
#define tlb_vbase                   ((uint32_t*)CONFIG_DRAM_VIRTBASE)
#define tlb_size                    (16 * 1024)

#define MEMS_ALIGN(val, align)      (((val) + ((align) - 1)) & ~((align) - 1))
#define MEMS_MIN(a, b)              ((a) < (b) ? (a) : (b))

#endif
