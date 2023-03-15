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
#include <simd.h>
#include <kconfig.h>

.extern melis_kernel_running
.extern melis_arch_use_fpu

/*
 * uint32_t awos_arch_lock_irq(void);
 */
cfi_debug_info_begin awos_arch_lock_irq
    mrs     r0, cpsr
    orr     r1, r0, #INT_MASK       @ set irq and fiq bits in cpsr to disable all interrupts
    msr     cpsr_c, r1
    mrs     r1, cpsr                @ confirm that cpsr contains the proper interrupt disable flags  
    and     r1, r1, #INT_MASK
    cmp     r1, #INT_MASK
    bne     awos_arch_lock_irq      @ not properly disabled (try again)
    bx      lr                      @ disabled, return the original cpsr contents in r0
cfi_debug_info_end awos_arch_lock_irq

/*
 * void awos_arch_unlock_irq(uint32_t cpu_sr);
 */
cfi_debug_info_begin awos_arch_unlock_irq
    msr     cpsr_c, r0
    bx      lr
cfi_debug_info_end awos_arch_unlock_irq

 @ void awos_arch_set_vector_base(uint32_t newvector);
 @ only ARM11 and cortex-a can modify the vector address.
 @ ARM7,ARM9,ARM10 only fixed at 0x0 or 0xffff0000. 
cfi_debug_info_begin awos_arch_set_vector_base
    mrc     p15, #0, r1, c1, c0, #0            @ Read CP15 control register.
    bic     r1,  r1, #C1_V_BIT                 @ Clear V bit 12 to low vector 0x0~0x1c
    mcr     p15, #0, r1, c1, c0, #0            @ Write CP15
    mcr     p15, #0, r0, c12, c0, #0           @ Set new vector to cp12
    bx      lr
cfi_debug_info_end awos_arch_set_vector_base

/*
 * int awos_arch_task_switch(rt_uint32 from, rt_uint32 to);
 * r0 --> from
 * r1 --> to
 * the value of the other volatile registers don't matter -- r1-r3, ip).
 * This function is called through the normal C calling conventions and the values of
 * these registers cannot be assumed at the point of return.
 * Registers:
 * r0-r3: are the argument and scratch registers; r0-r1 are also the result registers
 * r4-r8: callee saved registers.
 * r9-r11:callee saved registers
 * r12: alias ip, special register, not callee saved, free to use for callee.
 * r13: alias sp, called saved register, keep same after function call.
 * r14: not callee saved, special register.
 * r15: pc, special register.
 * r2-r3 are free to use here.
 */
cfi_debug_info_begin awos_arch_task_switch
    clrex
    @DBGSTR3 "register %08x %08x %08x", r0, r1, r2
    stmfd   sp!, {lr}                @ push pc (lr should be pushed in place of pc)
#ifdef CONFIG_CONSISTENT_CONTEXT_SWITCH
    stmfd   sp!, {r4-r12, lr}        @ push lr & register file
#else
    stmfd   sp!, {r0-r12, lr}        @ push lr & register file
#endif

    mrs     r2,  cpsr
    tst     lr,  #0x01
    orrne   r2, r2, #0x20            @ thumb mode
    stmfd   sp!, {r2}                @ push cpsr

    @ get fcse
    mrc     p15, #0, r2, c13, c0, #0
    stmfd   sp!, {r2} 

    mov     r5,  r0

    saveneonvfp r2, r3

    str     sp,  [r0]                @ store sp in preempted tasks tcb
    ldr     sp,  [r1]                @ get new task stack pointer

    restoreneonvfp r2, r3

    ldmfd   sp!, {r2}
    mcr     p15, #0, r2, c13, c0, #0 @ restore new task fcse
    ldmfd   sp!, {r2}                @ pop new task spsr
    msr     spsr_cxsf, r2
    mov     r0, r5
#ifdef CONFIG_CONSISTENT_CONTEXT_SWITCH
    ldmfd   sp!, {r4-r12, lr, pc}^   @ pop new task r4-r12, lr & pc
#else
    ldmfd   sp!, {r0-r12, lr, pc}^   @ pop new task r4-r12, lr & pc
#endif
cfi_debug_info_end awos_arch_task_switch

@uint32_t awos_arch_chksched_start(void);
cfi_debug_info_begin awos_arch_chksched_start
    ldr     r0, =melis_kernel_running
    ldrb    r0, [r0]
    bx      lr
cfi_debug_info_end awos_arch_chksched_start

/*
 * void awos_arch_first_task_start(rt_uint32 to);
 * r0 --> to
 */
cfi_debug_info_begin awos_arch_first_task_start
    ldr     r1, =melis_kernel_running
    mov     r2, #1
    strb    r2, [r1]

    ldr     sp, [r0]                 @ get new task stack pointer

    restoreneonvfp  r0, r4

    ldmfd   sp!, {r0}
    mcr     p15, #0, r0, c13, c0, #0 @ restore new task fcse

    ldmfd   sp!, {r0}                @ pop new task spsr
#ifdef CONFIG_CONSISTENT_CONTEXT_SWITCH
    ldr     r1, [sp, #40]            @ get pc
#else
    ldr     r1, [sp, #56]            @ get pc
#endif
    tst     r1, #1
    orrne   r0, #32                  @ if pc is thumb mode, set spsr to thumb
    msr     spsr_cxsf, r0
#ifdef CONFIG_CONSISTENT_CONTEXT_SWITCH
    ldmfd   sp!, {r4-r12, lr, pc}^   @ pop new task r0-r12, lr & pc
#else
    ldmfd   sp!, {r0-r12, lr, pc}^   @ pop new task r0-r12, lr & pc
#endif
cfi_debug_info_end awos_arch_first_task_start

cfi_debug_info_begin awos_arch_dbg_task_switch_to
    mov     sp,  r0                  @ get new task stack pointer

    restoreneonvfp r0, r4

    ldmfd   sp!, {r4}
    mcr     p15, #0, r4, c13, c0, #0 @ restore new task fcse
    ldmfd   sp!, {r4}                @ pop new task spsr
    msr     spsr_cxsf, r4
#ifdef CONFIG_CONSISTENT_CONTEXT_SWITCH
    ldmfd   sp!, {r4-r12, lr, pc}^   @ pop new task r4-r12, lr & pc
#else
    ldmfd   sp!, {r0-r12, lr, pc}^   @ pop new task r4-r12, lr & pc
#endif
cfi_debug_info_end awos_arch_dbg_task_switch_to

.align  5
@int setjmp(jmp_buf env);
cfi_debug_info_begin setjmp
    stmia   r0!, {r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr}
    mov     r0, #0
    tst     lr, #1
    moveq   pc, lr
    .word   0xe12fff1e
cfi_debug_info_end setjmp

@void longjmp(jmp_buf env,int val);
cfi_debug_info_begin longjmp
    ldmfd   r0!, {r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr}
    movs    r0,  r1
    moveq   r0,  #1 
    tst     lr,  #1
    moveq   pc,  lr
    .word   0xe12fff1e
cfi_debug_info_end longjmp

@ Get the TPIDRPRW registor, TPIDRPRW can be R/W in priliedge mode.
@ unsigned long awos_arch_prvdata_rd(void);
cfi_debug_info_begin awos_arch_prvdata_rd
    mrc     p15, #0, r0, c13, c0, #4
    mov     pc,  lr
    .word   0xe12fff1e
cfi_debug_info_end awos_arch_prvdata_rd

@ Set the TPIDRPRW registor, TPIDRPRW can be R/W in priliedge mode.
@ void awos_arch_prvdata_wt(unsigned long ptr);
cfi_debug_info_begin awos_arch_prvdata_wt
    mcr     p15, #0, r0, c13, c0, #4   @ set tpidrprw register.
    mov     pc,  lr
    .word   0xe12fff1e
cfi_debug_info_end awos_arch_prvdata_wt

@ Get the TPIDRURW registor, TPIDRURW can be R/W in user mode.
@ unsigned long awos_arch_usrdata_rd(void);
cfi_debug_info_begin awos_arch_usrdata_rd
    mrc     p15, #0, r0, c13, c0, #2
    mov     pc,  lr
    .word   0xe12fff1e
cfi_debug_info_end awos_arch_usrdata_rd

@ Set the TPIDRURW registor, TPIDRURW can be R/W in user mode.
@ void awos_arch_usrdata_wt(unsigned long ptr);
cfi_debug_info_begin awos_arch_usrdata_wt
    mcr     p15, #0, r0, c13, c0, #2  @ set tpidrurw register.
    mov     pc,  lr
    .word   0xe12fff1e
cfi_debug_info_end awos_arch_usrdata_wt

@ Get the TPIDRURO registor, TPIDRURO only can be R in user mode.
@ unsigned long awos_arch_usrdataro_rd(void);
cfi_debug_info_begin awos_arch_usrdataro_rd
    mrc     p15, #0, r0, c13, c0, #3
    mov     pc,  lr
    .word   0xe12fff1e
cfi_debug_info_end awos_arch_usrdataro_rd

@ Set the TPIDRURO registor, TPIDRURO only can be R in user mode.
@ void awos_arch_usrdataro_wt(unsigned long ptr);
cfi_debug_info_begin awos_arch_usrdataro_wt
    mcr     p15, #0, r0, c13, c0, #3 @ set tpidruro register.
    mov     pc,  lr
    .word   0xe12fff1e
cfi_debug_info_end awos_arch_usrdataro_wt

.end
