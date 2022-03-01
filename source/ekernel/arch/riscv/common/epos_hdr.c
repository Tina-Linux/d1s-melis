/*
 * =====================================================================================
 *
 *       Filename:  melis_hdr.c
 *
 *    Description:  kernel header structure used to keep the boot parameters from boot.
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 15:33:38
 *
 *         Author:  zhijinzeng@allwinnertech.com
 *  Last Modified:  2020-08-03 20:20:48
 *
 * =====================================================================================
 */

#include "epos_hdr.h"
#include <mmu.h>

#define OFFSET(structtype)                    (sizeof(structtype) >> 1)
#define OFFSET_12_19(structtype)              (((OFFSET(structtype) & 0x0007f800) >> 11) << 12)
#define OFFSET_11(structtype)                 (((OFFSET(structtype) & 0x00000400) >> 10) << 20)
#define OFFSET_1_10(structtype)               (((OFFSET(structtype) & 0x000003ff) >> 0 ) << 21)
#define OFFSET_20(structtype)                 (((OFFSET(structtype) & 0x00080000) >> 19) << 31)
#define BOOT_HEAD_SIZE_OFFSET(structtype)     ((OFFSET_12_19(structtype)) | (OFFSET_11(structtype)) | (OFFSET_1_10(structtype)) | (OFFSET_20(structtype)))
#define headjump(structtype)                  (0x0000006f | BOOT_HEAD_SIZE_OFFSET(structtype))

/* melis os header structure definition. */
__attribute__((section(".melis_head"))) const boot_head_t melis_head =
{
    {
        /* jump_instruction */
        headjump(boot_head_t),
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
