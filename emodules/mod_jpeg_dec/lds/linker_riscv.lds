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
 *  Last Modified:  2017-11-02 15:49:12
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
    dram0_2_seg (RWX) :                 org = 0xe4500000, len = 0x100000

    /* MAGIC section area. just use in loader. */
    magic_info_seg (R) :                org = 0xffff0000, len = 0x100
}

INCLUDE configuration_riscv.lds

/* must be defined in the code. */
ENTRY(MInit)

__DRAM0_MOD_BASE = ORIGIN(dram0_2_seg);
__DRAM0_INF_BASE = ORIGIN(magic_info_seg);

SECTIONS
{
    . = __DRAM0_MOD_BASE;
    .mod.text __DRAM0_MOD_BASE : AT(__DRAM0_MOD_BASE)
    {
        *(.text)
        *(.text.*)
        *(.stub)
        *(.gnu.warning)
        *(.gnu.linkonce.t*)
        . = ALIGN(4);
    } > dram0_2_seg 
    
    .mod.rodata  ADDR(.mod.text) + SIZEOF(.mod.text) : AT(LOADADDR(.mod.text) + SIZEOF(.mod.text))
    {
        *(.rodata)
        *(.rodata.*)
        *(.gnu.linkonce.r*)
        . = ALIGN(4);
    } > dram0_2_seg
    
    .mod.data  ADDR(.mod.rodata) + SIZEOF(.mod.rodata) : AT(LOADADDR(.mod.rodata) + SIZEOF(.mod.rodata))
    {
        *(.data) 
        *(.data.*) 
        *(.gnu.linkonce.d.*)
        *(.sdata) 
        *(.sdata.*) 
        *(.gnu.linkonce.s.*)
        *(.sdata2) 
        *(.sdata2.*) 
        *(.gnu.linkonce.s2.*)
        . = ALIGN(4);
    } > dram0_2_seg
    
    .mod.bss ADDR(.mod.data) + SIZEOF(.mod.data) (NOLOAD) :
    {
        __bss_start = ABSOLUTE(.);
        *(.bss)
        *(.bss.*)
        *(.gnu.linkonce.b.*)
        *(.sbss)
        *(.sbss.*)
        *(.gnu.linkonce.sb.*)
        *(.sbss2)
        *(.sbss2.*)
        *(.gnu.linkonce.sb2.*)
        *(COMMON)
        *(.scommon)
        . = ALIGN(4);
    } > dram0_2_seg

    __bss_end = ABSOLUTE(.);
    _end = ABSOLUTE(.);

    MAGIC (INFO) :
    {
        KEEP(*magic.o(.magic))
    } > magic_info_seg

    .note 0 : { *(.note) }
    .stab 0 : { *(.stab)  }
    .stabstr 0 : { *(.stabstr)  }
    .stab.excl 0 : { *(.stab.excl)  }
    .stab.exclstr 0 : { *(.stab.exclstr)  }
    .stab.index 0 : { *(.stab.index)  }
    .stab.indexstr 0 : { *(.stab.indexstr)  }
    .mdebug  0 : { *(.mdebug) }
    .reginfo 0 : { *(.reginfo) }
    .comment 0 : { *(.comment) }
    .ARM.attributes 0 : { *(.ARM.attributes) }
    /DISCARD/ : { *(.note.GNU-stack) *(.ARM.exidx* .gnu.linkonce.armexidx.*) }
}
