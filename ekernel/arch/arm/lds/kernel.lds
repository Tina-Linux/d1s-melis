/*
 * =====================================================================================
 *
 *       Filename:  kernel.lds
 *
 *    Description:  linker script file, This file describes the memory layout (memory
 *                  blocks) as virtual memory addresses.the script should be passed 
 *                  to binutils linker ld with '-T' options and then linker objectes
 *                  output into elf runable output file. 
 *
 *        Version:  Melis3.0 
 *         Create:  2017-11-02 15:38:28
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-03-06 08:55:13
 *
 * =====================================================================================
 */

MEMORY
{
    /* ddr dram0 area, mmu disabled, 64M size, permanet mappings. */
    dram0_0_seg (rwx) :                 org = 0x40000000, len = 0x4000000

    /* ddr dram0 area, mmu disabled, 1M size, used during kernel bootstrap stage. */
    dram0_1_seg (rwx) :                 org = 0x40800000, len = 0x100000
  
    /* dram0 area, mmu enabled, 64M size, flat mapping of dram0_0_seg. */
    dram0_2_seg (rwx) :                 org = 0xc2000000, len = 0x4000000

    /* dram0 area, mmu enabled, 64M size, flat mapping of dram0_0_seg. */
    dram0_3_seg (rwx) :                 org = 0xc2000000, len = 0x4000000

    /* cpu vector table, code and stack area, mmu enabled.mapped to dram0. */  
    dram0_4_seg (rwx) :                 org = 0xffff0000, len = 0x2000

    /* code area used for kernel adjust clock. mapped to dram0. */  
    dram0_5_seg (rwx) :                 org = 0xffff4000, len = 0x4800

    /* 
     * code area used for adjust dram clock, mapped to sram area. refer object
     * g_module_info_items in kernel source code. sram0 include three part. 
     * 1. native cpu. 2. borrowed from VE, 3. borrowed from DE, 0xc000 bytes 
     * totally. 
     */  
    sram0_0_seg (rwx) :                 org = 0xf0000000, len = 0xc000

    /* 
     * io map area used for io register operation. 16M, mmu mapped. refer 
     * 'g_module_info_items' object in kernel source code. 
     */  
    io_seg (RW) :                       org = 0xf1000000, len = 0x1000000
}

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SEARCH_DIR(".");

PHDRS
{
    boot    PT_LOAD FLAGS(5); /* PF_R|PF_X */
    text    PT_LOAD FLAGS(5); /* PF_R|PF_X */
    rodata  PT_LOAD FLAGS(4); /* PF_R */
    data    PT_LOAD FLAGS(6); /* PF_R|PF_W */
    mmucode PT_LOAD FLAGS(5); /* PF_R|PF_X */
    irqs    PT_LOAD FLAGS(5); /* PF_R|PF_X */
    note    PT_NOTE FLAGS(4); /* PF_R */
    debug   PT_NOTE FLAGS(0); /* PF_R */
}
/* _start must be defined in the code. */
ENTRY(_start)

__DRAM0_PHY_BASE = ORIGIN(dram0_0_seg);
__DRAM0_VIR_BASE = ORIGIN(dram0_2_seg);
__DRAM0_IRQ_BASE = ORIGIN(dram0_4_seg);

/*#define GROUP_FOLLOWS_AT(where) AT > where*/
SECTIONS
{
    .head.text __DRAM0_PHY_BASE : AT(__DRAM0_PHY_BASE)
    {
        KEEP(*.o(.melis_head))
        KEEP(*.o(.text._start))
        . = ALIGN(0x4000);
    } > dram0_0_seg :boot

    ASSERT(SIZEOF(.head.text) == 0x4000, "Error: kernel head(furture 1st-tlb) section length wrong.")

    .head.2ndtlb __DRAM0_PHY_BASE + SIZEOF(.head.text) : AT(__DRAM0_PHY_BASE + SIZEOF(.head.text))
    {
        PROVIDE(__head_2ndtlb_high_start = ABSOLUTE(.));
        KEEP(*(.2ndtlb-high))
        PROVIDE(__head_2ndtlb_krnl_start = ABSOLUTE(.));
        KEEP(*(.2ndtlb-kern))
        . = ALIGN(0x4000);
        PROVIDE(__head_2ndtlb_end = ABSOLUTE(.));
    } > dram0_0_seg :boot

    ASSERT(SIZEOF(.head.2ndtlb) == 0x8000, "Error: length wrong, should be fixed to 0x8000.")

    /* adjust the current address to virtual area with the same offset */
    . = . + (__DRAM0_VIR_BASE - __DRAM0_PHY_BASE);

    _mmu_text_start = .;
    /* cacluate the kernel running address */
    PROVIDE(__kernel_entry_start = __DRAM0_VIR_BASE + SIZEOF(.head.text) + SIZEOF(.head.2ndtlb));

    /* the kernel code should be at offset 0xb400 of physical memory */
    ASSERT(. == __kernel_entry_start , "Error: kernel base address invalid.")
    
    PROVIDE(__readonly_area_start = .);
    .dram0.text __kernel_entry_start : AT(LOADADDR(.head.2ndtlb) + SIZEOF(.head.2ndtlb))
    {
        KEEP(*.o(.direct_syscall_table))
        PROVIDE(__code_start = ABSOLUTE(.));
        *(.text)
        *(.text.*)
        *(.stub)
        *(.gnu.warning)
        *(.gnu.linkonce.t*)
        PROVIDE(__code_end = ABSOLUTE(.));
        . = ALIGN(4);
    } > dram0_2_seg :text

    .dram0.rodata ADDR(.dram0.text) + SIZEOF(.dram0.text) : AT(LOADADDR(.dram0.text) + SIZEOF(.dram0.text))
    {
        . = ALIGN(8);
        *(.rodata)
        *(.rodata.*)
        *(.gnu.linkonce.r*)
        KEEP(*.o(.openocd_support))
        KEEP(*.o(.blobdata))
        . = ALIGN(4);
    } > dram0_2_seg :rodata

    .dram0.initcall ADDR(.dram0.rodata) + SIZEOF(.dram0.rodata) : AT(LOADADDR(.dram0.rodata) + SIZEOF(.dram0.rodata))
    {
        __initcall_start = .;
        KEEP(*(.initcallearly.init))
        __initcall0_start = .;
        KEEP(*(.initcall0.init))
        KEEP(*(.initcall0s.init))
        __initcall1_start = .;
        KEEP(*(.initcall1.init))
        KEEP(*(.initcall1s.init))
        __initcall2_start = .;
        KEEP(*(.initcall2.init))
        KEEP(*(.initcall2s.init))
        __initcall3_start = .;
        KEEP(*(.initcall3.init))
        KEEP(*(.initcall3s.init))
        __initcall4_start = .;
        KEEP(*(.initcall4.init))
        KEEP(*(.initcall4s.init))
        __initcall5_start = .;
        KEEP(*(.initcall5.init))
        KEEP(*(.initcall5s.init))
        __initcallrootfs_start = .;
        KEEP(*(.initcallrootfs.init))
        KEEP(*(.initcallrootfss.init))
        __initcall6_start = .;
        KEEP(*(.initcall6.init))
        KEEP(*(.initcall6s.init))
        __initcall7_start = .;
        KEEP(*(.initcall7.init))
        KEEP(*(.initcall7s.init))
        __initcall_end = .;
        __con_initcall_start = .;
        KEEP(*(.con_initcall.init))
        __con_initcall_end = .;
        . = ALIGN(4);
    } > dram0_2_seg :rodata

    .note.gnu.build-id ADDR(.dram0.initcall) + SIZEOF(.dram0.initcall) : AT(LOADADDR(.dram0.initcall) + SIZEOF(.dram0.initcall))
    { 
        *(.note.gnu.build-id)  
    } > dram0_2_seg :rodata

    .dram0.ctors ADDR(.note.gnu.build-id) + SIZEOF(.note.gnu.build-id) : AT(LOADADDR(.note.gnu.build-id) + SIZEOF(.note.gnu.build-id))
    {
        . = ALIGN(4);
        PROVIDE(__ctors_start__ = .);
        KEEP(*(SORT(.ctors.*)))
        KEEP(*(SORT(.ctors)))
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        PROVIDE(__ctors_end__ = .);
    } > dram0_2_seg :rodata

    .dram0.dtors ADDR(.dram0.ctors) + SIZEOF(.dram0.ctors) : AT(LOADADDR(.dram0.ctors) + SIZEOF(.dram0.ctors))
    {
        . = ALIGN(4);
        PROVIDE(__dtors_start__ = .);
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(SORT(.dtors)))
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array))
        PROVIDE(__dtors_end__ = .);
    } > dram0_2_seg :rodata

    __exidx_start = .;
    .ARM.exidx ADDR(.dram0.dtors) + SIZEOF(.dram0.dtors) : AT(LOADADDR(.dram0.dtors) + SIZEOF(.dram0.dtors))
    {
        __start_unwind_idx = .;
        *(.ARM.exidx*)
        __stop_unwind_idx = .;
        *(.gnu.linkonce.armexidx.*)
    } > dram0_2_seg :rodata
    __exidx_end = .;

    .ARM.extab ADDR(.ARM.exidx) + SIZEOF(.ARM.exidx) : AT(LOADADDR(.ARM.exidx) + SIZEOF(.ARM.exidx))
    {
        __start_unwind_tab = .;
        *(.ARM.extab*)
        __stop_unwind_tab = .;
    } > dram0_2_seg :rodata

    PROVIDE(__readonly_area_end = .);

    .dram0.data ADDR(.ARM.extab) + SIZEOF(.ARM.extab) : AT(LOADADDR(.ARM.extab) + SIZEOF(.ARM.extab))
    { 
        . = ALIGN(8);
        *(.data) 
        *(.data.*) 
        *(.gnu.linkonce.d.*)
        *(.sdata) 
        *(.sdata.*) 
        *(.gnu.linkonce.s.*)
        *(.sdata2) 
        *(.sdata2.*) 
        *(.gnu.linkonce.s2.*)
        *(.got.plt) 
        *(.got)  

        . = ALIGN(8);
        /* noinit used for zephyr kernel meta structure. */ 
        *(.noinit.*)
        . = ALIGN(4);
        _k_mem_pool_list_start = .;
        KEEP(*(SORT_BY_NAME("._k_mem_pool.static.*")))
        _k_mem_pool_list_end = .;
        . = ALIGN(4);
        _k_sem_list_start = .;
        KEEP(*(SORT_BY_NAME("._k_sem.static.*")))
        _k_sem_list_end = .;
        . = ALIGN(4);
        _k_mutex_list_start = .;
        KEEP(*(SORT_BY_NAME("._k_mutex.static.*")))
        _k_mutex_list_end = .;
        . = ALIGN(4);
        __tdata_start = .;
        *(.tdata)
        *(.tdata.*)
        *(.gnu.linkonce.td.*)
        __tdata_end = .;

        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;
        . = ALIGN(4);

        . = ALIGN(4);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;
        . = ALIGN(4);

        . = ALIGN(4);
    } > dram0_2_seg :data

    _mmu_text_end = .;

    .dram0.bss ADDR(.dram0.data) + SIZEOF(.dram0.data) (NOLOAD) : 
    {
        . = ALIGN(8);
        *(.bss)
        *(.bss.*)
        *(.gnu.linkonce.b.*)
        *(.sbss)
        *(.sbss.*)
        *(.gnu.linkonce.sb.*)
        *(.sbss2)
        *(.sbss2.*)
        *(.gnu.linkonce.sb2.*)
        *(.dynbss)
        *(COMMON)
        *(.scommon)
        . = ALIGN(4);
        __tbss_start = .;
        *(.tbss)
        *(.tbss.*)
        *(.gnu.linkonce.tb.*)
        __tbss_end = .;
        . = ALIGN(4);
    } > dram0_2_seg :data
    
    PROVIDE(__bss_start = ADDR(.dram0.bss));
    PROVIDE(__bss_end = ABSOLUTE(.));
    PROVIDE(_end = ABSOLUTE(.));
    ASSERT(_end < 0xc6000000, "binary too large!")
    
    /* 
     * adjust current address to the physical area, let the mmu tempory code overlapped with .bss 
     * the region would be cleared during the .bss clean_loop.
     */
    . = LOADADDR(.dram0.data) + SIZEOF(.dram0.data);
    .tlb.init ALIGN(4) : AT(LOADADDR(.dram0.bss))
    {
        PROVIDE(__tlb_init_begin = ABSOLUTE(.));
        KEEP(*(.tlb.code))
        . = ALIGN(8);
        KEEP(*(.tlb.data))
        . = ALIGN(4);
        PROVIDE(__tlb_init_end = ABSOLUTE(.));
    } > dram0_0_seg :mmucode

    .cpu.irqs __DRAM0_IRQ_BASE : AT(LOADADDR(.tlb.init) + SIZEOF(.tlb.init))
    {
        PROVIDE(__cpu_irqs_begin = ABSOLUTE(.));
        __exception_text_start = .;
        KEEP(*(.irq.code))
        __exception_text_end = .;
        . = ALIGN(8);
        KEEP(*(.irq.rodata))
        . = ALIGN(4);
        PROVIDE(__cpu_irqs_end = ABSOLUTE(.));
    } > dram0_4_seg :irqs

    PROVIDE(__cpu_irqs_load_begin = LOADADDR(.cpu.irqs));

    /* keep the exception code & data area in rang[0xffff0000, 0xffff0600]. left enough room for exception stack. */
    ASSERT(SIZEOF(.cpu.irqs) <= 0x600, "Error: no room left for exception and irq stack.")

    .note 0 : { *(.note)  } :note
    .stab 0 : { *(.stab)  } :note
    .stabstr 0 : { *(.stabstr)  } :note
    .stab.excl 0 : { *(.stab.excl)  } :note
    .stab.exclstr 0 : { *(.stab.exclstr)  } :note
    .stab.index 0 : { *(.stab.index)  } :note
    .stab.indexstr 0 : { *(.stab.indexstr)  } :note
    .reginfo 0 : { *(.reginfo) } :note
    .comment 0 : { *(.comment) } :note
    .mdebug  0 : { *(.mdebug)  } :note
    .debug_abbrev 0 : { *(.debug_abbrev) }
    .debug_info 0 : { *(.debug_info) }
    .debug_line 0 : { *(.debug_line) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    .debug_aranges 0 : { *(.debug_aranges) }
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
    /* DWARF 3 */
    .debug_pubtypes 0 : { *(.debug_pubtypes) }
    .debug_ranges   0 : { *(.debug_ranges) }
    .ARM.attributes 0 : { KEEP (*(.ARM.attributes)) KEEP (*(.gnu.attributes)) }
    /DISCARD/ : { *(.note.GNU-stack) }
}
