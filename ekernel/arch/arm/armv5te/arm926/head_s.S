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
.extern armv5te_vector_table

.global melis_exit
.global melis_trampoline
.global _start

.balign 4
@======================================================================================
@    _start，系统入口，完成不同模式下寄存器的初始化工作， 初始化MMU, 在实地址运行.
@======================================================================================
cfi_debug_info_begin _start
    ldr     r1,  =0xdeadbead

    mrs     r0,  cpsr                                                                                       
    orr     r0,  r0, #INT_MASK

    bic     r0,  r0, #ARM9_MODE_MASK                                                                             
    orr     r0,  r0, #ARM9_ABT_MODE                                                                  
    msr     cpsr_c,  r0                                                                                     
    mov     lr,  r1

    bic     r0,  r0, #ARM9_MODE_MASK                                                                             
    orr     r0,  r0, #ARM9_FIQ_MODE                                                                  
    msr     cpsr_c,  r0                                                                                     
    mov     lr,  r1

    bic     r0,  r0, #ARM9_MODE_MASK                                                                             
    orr     r0,  r0, #ARM9_IRQ_MODE                                                                  
    msr     cpsr_c,  r0                                                                                     
    mov     lr,  r1

    bic     r0,  r0, #ARM9_MODE_MASK                                                                             
    orr     r0,  r0, #ARM9_SYS_MODE                                                                  
    msr     cpsr_c,  r0                                                                                     
    mov     lr,  r1

    bic     r0,  r0, #ARM9_MODE_MASK                                                                             
    orr     r0,  r0, #ARM9_ABT_MODE                                                                  
    msr     cpsr_c,  r0                                                                                     
    mov     lr,  r1

    bic     r0,  r0, #ARM9_MODE_MASK                                                                             
    orr     r0,  r0, #ARM9_UND_MODE                                                                  
    msr     cpsr_c,  r0                                                                                     
    mov     lr,  r1

    bic     r0,  r0, #ARM9_MODE_MASK                                                                             
    orr     r0,  r0, #ARM9_SVC_MODE                                                                  
    msr     cpsr_c,  r0                                                                                     
    mov     lr,  r1

    mov     fp,  #0                  @ fp is alias of r11
    mov     ip,  #0                  @ ip is alias of r12

    @ Set CO-Processor c1.
    mrc     p15, #0, r0, c1, c0, #0            @ Read CP15 control register.
    bic     r0,  r0, #C1_I_BIT                 @ Clear I bit 12 to disable I Cache
    bic     r0,  r0, #C1_B_BIT                 @ Clear B bit  7 to enable little endian.
    bic     r0,  r0, #C1_C_BIT                 @ Clear C bit  2 to disable D Cache
    bic     r0,  r0, #C1_M_BIT                 @ Clear M bit  0 to disable MMU
    bic     r0,  r0, #C1_Z_BIT                 @ Clear Z bit 11 to disable branch prediction
    orr     r0,  r0, #C1_V_BIT                 @ high excep vector, 0xffff0000.
    bic     r0,  r0, #C1_S_BIT                 @ clear S(system protection) and enable R (rom protection)
    orr     r0,  r0, #C1_R_BIT                 @ ref dd10198e-arm926ejs-r0p5-trm.pdf 3.4 for detail.
    mcr     p15, #0, r0, c1, c0, #0            @ Write CP15

@ prepare for C code, be sure stack area is preserved.
    ldr     sp,  =(CONFIG_DRAM_PHYBASE + MELIS_MMUINIT_STACK_OFFSET)
    ldr     lr,  =melis_trampoline
    ldr     r0,  =mmu_init
    @bx     r0
    mov     pc, r0
    nop
cfi_debug_info_end _start


@**********************************************************************************************
@         启动操作系统：跳转到内核启动部分, 在虚拟地址上运行,主要完成:
@         1.销毁初始阶段代码
@         2.初始化模式栈
@         3.搬运调频，异常处理代码到运行地址
@         4.执行ctor段代码.
@         5.进入start_kernel启动操作系统
@**********************************************************************************************

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
    bic     r0,  r0, #ARM9_MODE_MASK                                                                             
    orr     r0,  r0, #ARM9_ABT_MODE                                                                  
    msr     cpsr_c, r0                                                                                     
    ldr     sp,  =EPOS_STACK_ABT
    
@ set undef mode stack
    mrs     r0,  cpsr                                                                                       
    bic     r0,  r0, #ARM9_MODE_MASK                                                                             
    orr     r0,  r0, #ARM9_UND_MODE                                                                 
    msr     cpsr_c,  r0                                                                                     
    ldr     sp,  =EPOS_STACK_UNDEF
    
@ set fiq mode stack
    mrs     r0,  cpsr
    bic     r0,  r0, #ARM9_MODE_MASK
    orr     r0,  r0, #ARM9_FIQ_MODE
    msr     cpsr_c, r0
    ldr     sp,  =EPOS_STACK_FIQ

@ set system mode stack
    mrs     r0,  cpsr
    bic     r0,  r0, #ARM9_MODE_MASK
    orr     r0,  r0, #ARM9_SYS_MODE
    msr     cpsr_c, r0
    ldr     sp,  =EPOS_STACK_SYS
    
@ set irq mode stack
    mrs     r0,  cpsr
    bic     r0,  r0, #ARM9_MODE_MASK
    orr     r0,  r0, #ARM9_IRQ_MODE
    msr     cpsr_c, r0
    ldr     sp,  =EPOS_STACK_IRQ

@ set svc mode stack
    mrs     r0,  cpsr
    bic     r0,  r0, #ARM9_MODE_MASK
    orr     r0,  r0, #ARM9_SVC_MODE
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

@copy the vector to the correct address.
    ldr     r0,  =__cpu_irqs_load_begin
    ldr     r1,  =__cpu_irqs_begin
    ldr     r2,  =__cpu_irqs_end

    ldr     r3,  =armv5te_vector_table
    mrc     p15, 0, r4, c1, c0, 0
    ands    r4,  r4, #C1_V_BIT
    ldreq   r5,  =0x00000000
    ldrne   r5,  =0xffff0000
    
    cmp     r1, r3
    beq     1f
    b       melis_exit
1:
    cmp     r1, r5
    beq     load_irqs_loop
    b       melis_exit
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

@ set sp to mapped sram0 top address.
    ldr     sp,  =EPOS_SRAM0_VADDR

    ;ldr     r0,  =mmu_unmap
    ;blx     r0

@ call c/c++ constructors of global objects
    ldr     r0, =__ctors_start__
    ldr     r1, =__ctors_end__

ctor_loop:
    cmp     r0, r1
    beq     ctor_end
    ldr     r2, [r0], #4
    stmfd   sp!, {r0-r1}
    mov     lr, pc
    bx      r2
    ldmfd   sp!, {r0-r1}
    b       ctor_loop
ctor_end:

@ operation system bringup.
    ldr     lr,  =melis_exit
    ldr     r0,  =start_kernel
    mov     pc,  r0
    nop
cfi_debug_info_end melis_trampoline

cfi_debug_info_begin melis_exit
    b       .
cfi_debug_info_end melis_exit

.end
