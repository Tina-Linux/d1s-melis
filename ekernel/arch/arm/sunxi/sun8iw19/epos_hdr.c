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
#include "epos_hdr.h"
#include <mmu.h>

//the L2 tlb table and fixed high area region map page.
__attribute__((section(".2ndtlb-high"))) uint8_t high_map_area[HIGH_MEMORY_AND_L2_SIZE] = {0};
__attribute__((section(".2ndtlb-kern"))) uint8_t kern_map_area[KERN_MEMORY_AND_L2_SIZE] = {0};

//melis os header structure definition.
__attribute__((section(".melis_head"))) const boot_head_t melis_head =
{
    {
        /* jump_instruction */
        (0xEA000000 | (((sizeof(struct spare_boot_head_t) + sizeof(int) - 1) / sizeof(int) - 2) & 0x00FFFFFF)),
        {'E', 'P', 'O', 'S', '-', 'I', 'M', 'G'},           //unsigned char magic[8];
        0,
        0,
        0,
        0,
        MELIS_VERSION,
        MELIS_PLATFORM,
        {0x40000000}
    },
    {
        { 0 },     //dram para
        1008,      //run core clock
        1200,      //run core vol
        0,         //uart port
        {
            //uart gpio
            {0}, {0}
        },
        0,         //twi port
        {
            //twi gpio
            {0}, {0}
        },
        0,         //work mode
        0,         //storage mode
        { {0} },   //nand gpio
        { 0 },     //nand info
        { {0} },   //sdcard gpio
        { 0 },     //sdcard info
        0,         //secure os
        0,         //monitor
        0,         /* see enum UBOOT_FUNC_MASK_EN */
        {0},       //reserved data
        0,         //OTA flag
        0,         //dtb offset
        0,         //boot_package_size
        0,         //dram_scan_size
        {0},       //reserved data
        0,         //pmu_type;
        0,         //uart_input;
        0,         //key_input;
        0,         //secure_mode
        0,         //debug_mode
        { 0 }      //reserved2[2]
    },
    {
        { {0} },
        { {0} },
        { {0} },
        { {0} }
    },
};

__tlb_init_code const boot_head_t *melis_head_para_get(void)
{
    return  &melis_head;
}
