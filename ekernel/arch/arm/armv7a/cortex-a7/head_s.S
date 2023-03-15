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
#include <cpu.h>
#include <cfi.h>
#include <kconfig.h>

@ Stack Offset of Init TLB routine, from 0x80f00000 area.
.equ    MELIS_MMUINIT_STACK_OFFSET,   (15 * 1024 * 1024)

@======================================================================================
@                            EXTERN PARAMETERS
@======================================================================================

.extern mmu_init
.extern mmu_unmap
.extern start_kernel
.extern armv_vector_table
#ifdef CONFIG_KASAN
.extern kasan_early_init
#endif
.global melis_exit
.global melis_trampoline
.global _start

.balign 4
@======================================================================================
@    _start，系统入口，完成不同模式下寄存器的初始化工作， 初始化MMU, 在实地址运行.
@======================================================================================
cfi_debug_info_begin _start
    mrrc   p15, 0, r7, r1, c14
    ldr     r1,  =0xdeadbead

    mrs     r0,  cpsr
    orr     r0,  r0, #INT_MASK

    bic     r0,  r0, #ARMV7A_MODE_MASK
    orr     r0,  r0, #ARMV7A_ABT_MODE
    msr     cpsr_c,  r0
    mov     lr,  r1

    bic     r0,  r0, #ARMV7A_MODE_MASK
    orr     r0,  r0, #ARMV7A_FIQ_MODE
    msr     cpsr_c,  r0
    mov     lr,  r1

    bic     r0,  r0, #ARMV7A_MODE_MASK
    orr     r0,  r0, #ARMV7A_IRQ_MODE
    msr     cpsr_c,  r0
    mov     lr,  r1

    bic     r0,  r0, #ARMV7A_MODE_MASK
    orr     r0,  r0, #ARMV7A_SYS_MODE
    msr     cpsr_c,  r0
    mov     lr,  r1

    bic     r0,  r0, #ARMV7A_MODE_MASK
    orr     r0,  r0, #ARMV7A_ABT_MODE
    msr     cpsr_c,  r0
    mov     lr,  r1

    bic     r0,  r0, #ARMV7A_MODE_MASK
    orr     r0,  r0, #ARMV7A_UND_MODE
    msr     cpsr_c,  r0
    mov     lr,  r1

    bic     r0,  r0, #ARMV7A_MODE_MASK
    orr     r0,  r0, #ARMV7A_MODE_MON
    msr     cpsr_c,  r0
    mov     lr,  r1

    bic     r0,  r0, #ARMV7A_MODE_MASK
    orr     r0,  r0, #ARMV7A_SVC_MODE
    msr     cpsr_c,  r0
    mov     lr,  r1

    mov     fp,  #0                  @ fp is alias of r11
    mov     ip,  #0                  @ ip is alias of r12

exitsmp:
    @ SMP status is controlled by bit 6 of the CP15 Aux Ctrl Reg
    @mrc     p15, 0, r0, c1, c0, 1   @ Read ACTLR
    @bic     r0, r0, #0x040          @ Clear bit 6
    @mcr     p15, 0, r0, c1, c0, 1   @ Write ACTLR 

enablesmp:
    @ SMP status is controlled by bit 6 of the CP15 Aux Ctrl Reg, enabled to use cache and ldrex/strex.
    @ Even in a single core Cortex-A7 processor the ACTLR.SMP bit must be set. The caches are disabled when
    @ ACTLR.SMP is set to 0 regardless of the value of the cache enable bit (SCTLR.C).
    @ It must ensure the ACTLR.SMP bit is set to 1 before the caches and MMU are enabled
    @ or any cache and TLB maintenance operations are performed. The only time this bit is set to 0 is
    @ during a processor power-down sequence.

    mrc     p15, 0, r0, c1, c0, 1   @ Read ACTLR
    orr     r0, r0, #0x040          @ enable bit 6 .SMP
    mcr     p15, 0, r0, c1, c0, 1   @ update ACTLR

    nop
    isb

    @ disable datalignment check before exception handler install.
    mrc      p15, 0, r1, c1, c0, 0
    bic      r1,  #(1<<1)
    mcr      p15, 0, r1, c1, c0, 0

    @ If N is set to 0, always use TTBR0. This is the default case at reset for the
    @ Secure version of this register. It is backwards compatible with ARMv5 and
    @ earlier processors
    @ If N is set greater than 0, and bits [31:32-N] of the VA are all zeros, use TTBR0
    @ otherwise use TTBR1. N must be in the range 0-7.
    @ Here Set to Zero, means use ttbr0 only.

    mov      r0,   #0x0
    mcr      p15,  0, r0, c2, c0, 2

    mrc     p15, #0, r0, c1, c0, #0            @ Read CP15 control register.
    orr     r0,  r0, #C1_V_BIT                 @ high excep vector, 0xffff0000.
    bic     r0,  r0, #C1_AFE_BIT               @ use ap0-ap1-ap2 all in pte entry.
    mcr     p15, #0, r0, c1, c0, #0            @ Write CP15

    @ If N is set to 0, always use TTBR0. This is the default case
    @ at reset for the Secure version of this register.
    @ It is backwards compatible with ARMv5 and earlier processors.
    @ If N is set greater than 0, and bits [31:32-N] of the VA are all zeros, use TTBR0,
    @ otherwise use TTBR1. N must be in the range 0-7.
    @ So set to zero means use TTBR0 only.
    mov     r0,  #0x0
    mcr     p15, 0, r0, c2, c0, 2

clear_afe_and_tre:
    mrc     p15, #0, r0, c1, c0, #0
    bic     r0, r0, #3<<28
    mcr     p15, #0, r0, c1, c0, #0    @ clear SCTLR.AFE and SCTLR.TRE bit
    isb

openneon:
    mrc     p15, 0, r0, c1, c0, 2              @ Read CP Access register
    ldr     r0, =(0xf << 20)                   @ Enable full access to NEON/VFP (Coprocessors 10 and 11)
    mcr     p15, 0, r0, c1, c0, 2              @ Write CP Access register

    # flush the pipeline to avoid the process already have decoded the vmsr/vmrs as
    # an undefined instructions.before upper mcr is executed.
    # the isb ensure the update of the cacr is seen by processor when decoding the vmsr.
    isb

    vmrs    r0, fpexc                          @ Switch on the VFP and NEON hardware
    orr     r0, r0, #0x40000000                @ Set EN bit in FPEXC
    vmsr    fpexc, r0

@ prepare for C code, be sure stack area is preserved.
    ldr     sp,  =(CONFIG_DRAM_PHYBASE + MELIS_MMUINIT_STACK_OFFSET)
    ldr     lr,  =melis_trampoline
    ldr     r0,  =mmu_init
    mov     pc,  r0
cfi_debug_info_end _start


@ ----------------------------------------------------------------------------------------------
@ Bringup the OS, Prepare the C runtime environment for Next Stage, the key flow are:
@      1.Destory the Initizlize stage code, in section ".tlb.code" and ".tlb.data"
@      2.Initlize the IRQ Stack Sp of each cpu mode.
@      3.copy the frequecy tunning and exception hander code to high momory(compat with arm9)
@      4.execute the handler in section "ctor".
@      5.step into start_kernel to prepare the OS.
@ ---------------------------------------------------------------------------------------------

cfi_debug_info_begin melis_trampoline
    mov     r0,  #0                               @clear mmu_init routine code and data.
    ldr     r1,  =__tlb_init_begin
    ldr     r2,  =__tlb_init_end

clear_code_loop:
    cmp     r1,  r2
    strlo   r0,  [r1], #4
    blo     clear_code_loop

@ set abt mode stack
    mrs     r0,  cpsr
    bic     r0,  r0, #ARMV7A_MODE_MASK
    orr     r0,  r0, #ARMV7A_ABT_MODE
    msr     cpsr_c, r0
    ldr     sp,  =EPOS_STACK_ABT

@ set undef mode stack
    mrs     r0,  cpsr
    bic     r0,  r0, #ARMV7A_MODE_MASK
    orr     r0,  r0, #ARMV7A_UND_MODE
    msr     cpsr_c,  r0
    ldr     sp,  =EPOS_STACK_UNDEF

@ set fiq mode stack
    mrs     r0,  cpsr
    bic     r0,  r0, #ARMV7A_MODE_MASK
    orr     r0,  r0, #ARMV7A_FIQ_MODE
    msr     cpsr_c, r0
    ldr     sp,  =EPOS_STACK_FIQ

@ set system mode stack
    mrs     r0,  cpsr
    bic     r0,  r0, #ARMV7A_MODE_MASK
    orr     r0,  r0, #ARMV7A_SYS_MODE
    msr     cpsr_c, r0
    ldr     sp,  =EPOS_STACK_SYS

@ set irq mode stack
    mrs     r0,  cpsr
    bic     r0,  r0, #ARMV7A_MODE_MASK
    orr     r0,  r0, #ARMV7A_IRQ_MODE
    msr     cpsr_c, r0
    ldr     sp,  =EPOS_STACK_IRQ

@ set monitor mode stack
    mrs     r0,  cpsr
    bic     r0,  r0, #ARMV7A_MODE_MASK
    orr     r0,  r0, #ARMV7A_MODE_MON
    msr     cpsr_c, r0
    ldr     sp,  =EPOS_STACK_MON

@ set svc mode stack
    mrs     r0,  cpsr
    bic     r0,  r0, #ARMV7A_MODE_MASK
    orr     r0,  r0, #ARMV7A_SVC_MODE
    msr     cpsr_c, r0
    ldr     sp,  =EPOS_STACK_SVC

@ fill irq stack
    ldr     r0,  =EPOS_INTV_BASEv
    ldr     r1,  =EPOS_STACK_IRQ
@ fill with softbreak inst.
    ldr     r2,  =0xe1200070

fill_stack_loop:
    cmp     r0,  r1
    strcc   r2,  [r0], #4
    bcc     fill_stack_loop

    ldr      r0,  =armv_vector_table
    mcr      p15, 0, r0, c12, c0, 0

@ copy the vector to the correct address.
    ldr     r0,  =__cpu_irqs_load_begin
    ldr     r1,  =__cpu_irqs_begin
    ldr     r2,  =__cpu_irqs_end

    ldr     r3,  =armv_vector_table
    mrc     p15, 0, r4, c1, c0, 0
    ands    r4,  r4, #C1_V_BIT
    ldreq   r5,  =0x00000000
    ldrne   r5,  =0xffff0000

    cmp     r1, r3
    beq     1f
    bl      melis_exit
1:
    cmp     r1, r5
    beq     load_irqs_loop
    bl      melis_exit
load_irqs_loop:
    cmp     r1,  r2
    ldr     r3,  [r0]
    strlo   r3,  [r1], #4
    add     r0,  r0, #4
    blo     load_irqs_loop

@ TODO, need relocate clk,standby code and data.

    mov     r0,  #0
    ldr     r1,  =__bss_start
    ldr     r2,  =__bss_end

bss_clear_loop:
    cmp     r1,  r2
    strlo   r0,  [r1], #4
    blo     bss_clear_loop

#ifdef CONFIG_KASAN
    bl      kasan_early_init
#endif

@ set sp to mapped sram0 top address.
    ldr     sp,  =EPOS_STACK_SVC
    ldr     r0,  =mmu_unmap
    blx     r0
    nop

@ call c/c++ constructors of global objects
@    ldr     r0, =__ctors_start__
@    ldr     r1, =__ctors_end__
@
@ctor_loop:
@    cmp     r0, r1
@    beq     set_generic_timer
@    ldr     r2, [r0], #4
@    stmfd   sp!, {r0-r1}
@    mov     lr, pc
@    cmp     r2, #0x0
@    bxne    r2
@    ldmfd   sp!, {r0-r1}
@    b       ctor_loop

set_generic_timer:
    @ set cntfrq to 24M, generic timer as the tick source.
    ldr     r1,  =24000000
    mcr     p15, 0, r1, c14, c0, 0

    ldr     sp,  =EPOS_STACK_SVC
    bl      main_gic_init

    @ enable datalignment check after exception handler install.
    @ mrc      p15, 0, r1, c1, c0, 0
    @ orr      r1,  #(1<<1)
    @ mcr      p15, 0, r1, c1, c0, 0

@ operation system bringup.
    ldr     sp,  =EPOS_STACK_SVC
    ldr     lr,  =melis_exit
    mov     r0,  r7
    mov     r1,  r8
    ldr     r2,  =start_kernel
    mov     pc,  r2
    b       .
cfi_debug_info_end melis_trampoline

cfi_debug_info_begin melis_exit
    b       .
cfi_debug_info_end melis_exit

@cfi_debug_info_begin arch_writeback
    @ldr    sp,  [r0]!
    @ldr    sp,  [r0]!
    @ldr    sp,  [r0]
    @ldr    sp,  [r0]
    @ldmfd  sp!, {r0}
    @ldmfd  sp!, {r0-r1}
    @ldmfd  sp!, {r0-r2}
    @ldmfd  sp!, {r0-r3}
    @ldmfd  sp!, {r0-r4}
    @ldmfd  sp!, {r0-r5}
    @ldmfd  sp!, {r0-r6}
    @ldmfd  sp!, {r0-r7}
    @ldmfd  sp!, {r0-r8}
    @ldmfd  sp!, {r0-r9}
    @ldmfd  sp!, {r0-r10}
    @ldmfd  sp!, {r0-r11}
    @ldmfd  sp!, {r0-r12}
    @ldmfd  sp!, {r0-r13}
    @ldmfd  sp!, {r0-r14}
    @ldmfd  sp!, {r0-r15}
    @ldmfd  sp!, {r0-r15}^
    @b       .
@cfi_debug_info_end arch_writeback

.end
