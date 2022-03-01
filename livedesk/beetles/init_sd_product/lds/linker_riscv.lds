/*
* =====================================================================================
*
*       Filename:  linker.lds
*
*    Description:  This file describes the memory layout (memory blocks) as virtual
*                  memory addresses.This linker script is passed to the binutils ld
*                  with '-T' options and then to link compiler output into elf run-
*                  able output file.
*                  
*
*        Version:  1.0
*        Created:  2017年08月26日 11时52分57秒
*       Revision:  none
*
*         Author:  czl
*   Organization: AllWinner Technology.
*
* =====================================================================================
*/

MEMORY
{
    /* dram0 area, mmu enabled, 64M size, flat mapping of dram0_0_seg. */
    dram0_0_seg (RWX) :                 org = 0x40000000, len = 0x04000000

    /* modules area, mmu enabled, 256M size. */
    dram0_1_seg (RWX) :                 org = 0xe0000000, len = 0x10000000

    /* iic driver area, mmu enabled, 64M size, flat mapping of dram0_0_seg. */
    dram0_2_seg (RWX) :                 org = 0xe1700000, len = 0x00100000

    /* MAGIC section area. just use in loader. */
    magic_info_seg (R) :                org = 0xffff0000, len = 0x00000100
}

INCLUDE configuration_riscv.lds

/* must be defined in the code. */
ENTRY(INIT_MInit)

SECTIONS
{
    INIT_API_TBL __DRAM0_MOD_BASE : AT(__DRAM0_MOD_BASE)
    {
       KEEP(*init_api.o(INIT_API_TBL))
    } > dram0_2_seg

   __DRAM0_MOD_BASE = ABSOLUTE(.);
}

INCLUDE section.lds
