/*
 * =====================================================================================
 *
 *       Filename:  linker.lds
 *
 *    Description:  linker script file, This file describes the memory layout (memory
 *                  blocks) as virtual memory addresses.This linker script is passed 
 *                  to the binutils ld with '-T' options and then to link compiler o-
 *                  utput into elf runable output file. 
 *
 *        Version:  2.0 
 *         Create:  2017-11-02 15:46:44
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-01-14 12:21:09
 *
 * =====================================================================================
 */

MEMORY
{
    /* dram0 area, mmu enabled, 64M size, flat mapping of dram0_0_seg. */
    dram0_0_seg (RWX) :                 org = 0xc2000000, len = 0x4000000

    /* modules area, mmu enabled, 256M size. */
    dram0_1_seg (RWX) :                 org = 0xe0000000, len = 0x10000000

    /* slib driver area, mmu enabled, 64M size, flat mapping of dram0_0_seg. */
    dram0_2_seg (RWX) :                 org = 0xe8000000, len = 0x10000

    /* MAGIC section area. just use in loader. */
    magic_info_seg (R) :                org = 0xffff0000, len = 0x100
}

INCLUDE configuration_riscv.lds

/* must be defined in the code. */
ENTRY(UPDATE_MInit)
INCLUDE section.lds
