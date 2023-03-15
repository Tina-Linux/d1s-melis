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

.global armv_rst_handler
.global armv_und_handler
.global armv_swc_handler
.global armv_iabt_handler
.global armv_dabt_handler
.global armv_hyp_handler
#ifdef CONFIG_RTTKERNEL
.global armv_irq_handler_rtthr
#elif defined CONFIG_ZEPHYRKERNEL
.global armv_irq_handler_zephyr
#endif 
.global armv_fiq_handler

.section .irq.code, "ax", %progbits; 
.code 32

//#define ASM_DEBUG_EXCEPTION
@-------------------------------------------vector table-------------------------------------------------
/* These will be relocated to VECTOR_BASE. */ 
//void armv_vector_table(void);
.global armv_vector_table
armv_vector_table:
#ifndef ASM_DEBUG_EXCEPTION
    ldr     pc, vector_rst                           @ use brom reset default.
    ldr     pc, vector_und                           @ meet undefined instruction. 
    ldr     pc, vector_swc                           @ triggerd by syscall instruction
    ldr     pc, vector_iabt                          @ instruction fetch abort.
    ldr     pc, vector_dabt                          @ data access abort.
    ldr     pc, vector_hyp                           @ hypervisor mode handler.
    ldr     pc, vector_irq                           @ common interrupt request.
    ldr     pc, vector_fiq                           @ fast interrupt request.
#else
    ldr     pc, vector_rst                           @ use brom reset default.
    b       .                                        @ meet undefined instruction. 
    b       .                                        @ triggerd by syscall instruction
    b       .                                        @ instruction fetch abort.
    b       .                                        @ data access abort.
    b       .                                        @ hypervisor mode handler.
    ldr     pc, vector_irq                           @ common interrupt request.
    ldr     pc, vector_fiq                           @ fast interrupt request.
#endif

vector_rst:
    .word   armv_rst_handler

vector_und:
    .word   armv_und_handler
    
vector_swc:
    .word   armv_swc_handler
    
vector_iabt:
    .word   armv_iabt_handler
    
vector_dabt:
    .word   armv_dabt_handler
    
vector_hyp: 
    .word   armv_hyp_handler
    
vector_irq:
#if defined CONFIG_RTTKERNEL
    .word   armv_irq_handler_rtthr
#elif defined CONFIG_ZEPHYRKERNEL
    .word   armv_irq_handler_zephyr
#else
#error "os not support."
#endif
    
vector_fiq:
    .word   armv_fiq_handler
.size   armv_vector_table, . - armv_vector_table

@cpu exception vector data area, register system global object for statistic.
.section .irq.rodata
trap_und:
    .word  awos_arch_und_trap_entry
trap_iabt:
    .word  awos_arch_iabt_trap_entry
trap_dabt:
    .word  awos_arch_dabt_trap_entry
trap_irq:
    .word  awos_arch_irq_trap_entry
trap_fiq:
    .word  awos_arch_fiq_trap_entry
irq_enter:
    .word  awos_arch_irq_trap_enter
irq_exit:
    .word  awos_arch_irq_trap_leave
kernel_running:
    .word  melis_kernel_running

.balignl 16, 0xdeadbeef
.end
