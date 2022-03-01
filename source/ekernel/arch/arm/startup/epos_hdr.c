/*
 * =====================================================================================
 *
 *       Filename:  melis_hdr.c
 *
 *    Description:  kernel header structure used to keep the boot parameters from boot.
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 15:33:38
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-03-16 14:05:46
 *
 * =====================================================================================
 */

#include "epos_hdr.h"
//#include <boot/boot.h>
#include <kconfig.h>
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
