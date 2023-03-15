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
