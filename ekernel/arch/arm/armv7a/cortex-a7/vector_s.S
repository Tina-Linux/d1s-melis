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

#ifdef CONFIG_ZEPHYRKERNEL
#include <zephyr/offsets.h>

/* lowest value of _thread_base.preempt at which a thread is non-preemptible */
#define _NON_PREEMPT_THRESHOLD 0x0080

/* highest value of _thread_base.preempt at which a thread is preemptible */
#define _PREEMPT_THRESHOLD (_NON_PREEMPT_THRESHOLD - 1)
#endif

.extern awos_arch_und_trap_entry
.extern awos_arch_iabt_trap_entry
.extern awos_arch_dabt_trap_entry
.extern awos_arch_irq_trap_entry
.extern awos_arch_irq_trap_enter
.extern awos_arch_irq_trap_leave
.extern awos_arch_swc_mode_error
.extern awos_arch_swc_func_error
.extern awos_arch_fiq_trap_entry
.extern awos_arch_rst_trap_entry
.extern awos_arch_hyp_trap_entry
.extern awos_arch_irqstack_balance_error
.extern awos_arch_bad_switch_error
.extern melis_syscall_table
.extern schedule_preempt_inirq
.extern rt_current_thread
.global armv_rst_handler
.global armv_und_handler
.global armv_swc_handler
.global armv_iabt_handler
.global armv_dabt_handler
.global armv_hyp_handler
.global _interrupt_return_address

#ifdef CONFIG_RTTKERNEL
.global armv_irq_handler_rtthr
#elif defined CONFIG_ZEPHYRKERNEL
.global armv_irq_handler_zephyr
#endif 
.global armv_fiq_handler

#ifdef CONFIG_ZEPHYRKERNEL
.extern _k_neg_eagain
.extern _kernel
.extern _update_time_slice_before_swap
.global __swap
#endif

.global cpu_recover_from_exception
.balign 4
.macro common_error_handler  handler
    stmfd   sp!, {r0-r12}
#ifdef CONFIG_SUBSYS_KGDB
    saveneonvfp r2, r3
#endif

    @ get the lr and sp of prev. mode
    mrs     r0, spsr
    mrs     r1, cpsr
    orr     r0, r0, #INT_MASK
    msr     cpsr_c, r0
    mov     r2, sp                      @ get spsr mode sp
    mov     r3, lr                      @ get spsr mode lr
    msr     cpsr_c, r1
    
    @ push sp,orig_lr,cpsr, abt_lr.
    stmfd   sp!, {r2}
    stmfd   sp!, {r3}
    mrs     r0,  spsr
    stmfd   sp!, {r0}
    stmfd   sp!, {lr}
 
    @ prepare parameter for c code
    mrc     p15, 0,  r4, c5, c0, 0      @ get DFSR, cause.
    stmfd   sp!, {r4}
    mrc     p15, 0,  r4, c5, c0, 1      @ get IFSR.
    stmfd   sp!, {r4}
    mrc     p15, 0,  r4, c6, c0, 0      @ get DFAR, badvadr.
    stmfd   sp!, {r4}
    mrc     p15, 0, r2, c13, c0, 0      @ get FCSEID.
    stmfd   sp!, {r2}
    mov     r0,  sp
    bl      \handler                    @ jumpto specify procedure.
    ldmfd   sp!, {r0-r3}                @ popup fcseid, dfar, ifsr, dfsr.
    ldmfd   sp!, {lr}
    ldmfd   sp!, {r0-r2}
#ifdef CONFIG_SUBSYS_KGDB
    restoreneonvfp r2, r3
#endif
    ldmfd   sp!, {r0-r12}
    movs    pc,  lr
.endm

.macro switch_to_svcmode
    sub     lr, lr, #4

    @ Save r0, lr_<exception> (parent PC) and spsr_<exception>
    @ (parent CPSR)
    stmfd   sp, {r0, lr}            @ Save r0-lr
    mrs     lr, spsr
    str     lr, [sp, #-12]          @ Save spsr
  
    @ check irq stack is balance.
    ldr     r0,  =EPOS_STACK_IRQ
    cmp     r0,  sp 
    bne     awos_arch_irqstack_balance_error @ irq stack corruption. 
    nop

    @ Prepare for SVC32 mode.  IRQs remain disabled.
    mrs     r0, cpsr
    eor     r0, r0, #(ARMV7A_IRQ_MODE ^ ARMV7A_SVC_MODE)
    msr     spsr_cxsf, r0 

    sub     r0, sp, #12             @ pass lr, r0, spsr registers.  
    ldr     lr, =.Lsvc_mode
    movs    pc, lr
    nop

    @ enter svc mode from irq in order to do the context switch.
.Lsvc_mode:
    @ align to 8 bytes.
    tst     sp, #4
    subeq   sp, sp, #4

    sub     sp, sp, #4              @ placeholder for irq return pc(value of irq lr.)
    stmfd   sp!,{lr}                @ push old tasks lr 
    sub     sp, sp, #4              @ placeholder for R13, (svc sp(sp_svc))
    add     lr, sp, #12 
    addeq   lr, #4                  @ restore the origin sp_svc that before aligned to 8
    str     lr, [sp]                @ backup origin(svc) R13(sp stackpointer)
    stmfd   sp!,{r1-r12}            @ push old tasks LR,R1-R12

    
    ldmfd   r0, {r1-r3}             @ get lr, r0, spsr registers.
                                    @ r1:spsr
                                    @ r2:r0
                                    @ r3:lr
    stmfd   sp!,{r2}                @ push origin r0
    str     r3, [sp, #4*15]         @ save lr_exception(pc) to the placeholder.
    stmfd   sp!,{r1}                @ push irq spsr, which means spsr_exceptinon. 
.endm

cfi_debug_info_begin cpu_recover_from_exception
    mov     lr,  r0                  @ get new task stack pointer
    ldr     r0,  [lr, #0x20]
    ldr     r1,  [lr, #0x24]
    ldr     r2,  [lr, #0x28]
    ldr     r3,  [lr, #0x2c]
    ldr     r4,  [lr, #0x30]
    ldr     r5,  [lr, #0x34]
    ldr     r6,  [lr, #0x38]
    ldr     r7,  [lr, #0x3c]
    ldr     r8,  [lr, #0x40]
    ldr     r9,  [lr, #0x44]
    ldr     r10, [lr, #0x48]
    ldr     r11, [lr, #0x4c]
    ldr     r12, [lr, #0x50]
    ldr     lr,  [lr, #0x10]         @ recover exception lr address.
    add     sp,  sp,  #(21*4)
    movs    pc,  lr
cfi_debug_info_end cpu_recover_from_exception

cfi_debug_info_begin  armv_rst_handler
    b       awos_arch_rst_trap_entry
    b       .
cfi_debug_info_end armv_rst_handler

cfi_debug_info_begin  armv_hyp_handler
    b       awos_arch_hyp_trap_entry
    b       .
cfi_debug_info_end armv_hyp_handler

cfi_debug_info_begin  armv_und_handler
    common_error_handler  awos_arch_und_trap_entry
cfi_debug_info_end armv_und_handler

cfi_debug_info_begin  armv_swc_handler
    @state: arm mode, irq disable.
    stmfd   sp!, {r0-r3, r7, lr} 
    mrs     r0,  spsr
    stmfd   sp!, {r0}
    and     r1,  r0, #ARMV7A_MODE_MASK
    cmp     r1,  #ARMV7A_SVC_MODE
    beq     do_syscall

    @processor cant not be user&syst mode present.
    cmp     r1,  #ARMV7A_USR_MODE
    beq     error_cpu_mode
    cmp     r1,  #ARMV7A_SYS_MODE
    beq     error_cpu_mode

@old state maybe irq, fiq, abt, und or sys.
copy_lr_2_prev_state:
    mrs     r1,  cpsr
    mov     r2,  lr
    orr     r0,  #INT_MASK
    msr     cpsr_c, r0
    mov     lr,  r2
    msr     cpsr_c, r1
   
@find the entry and jump into, ^used to switch mode from svc to prev.
do_syscall:
    ldr     r0,  [lr, #-4]
    bic     r0,  r0, #0xff000000
    ldr     r1,  =GCC_SWINO 
    cmp     r0,  r1
    beq     .gnucc

@ move the syscall. no to r7 in armcc eabi
.armcc:
    mov     r7,  r0

.gnucc:
#ifdef CONFIG_SYSCALL
    ldr     r2,  =melis_syscall_table
#else
    mov     r2,  #0
#endif
    bic     r3,  r7, #0xffff00ff
    ldr     r2,  [r2, r3, lsr #6]
    cmp     r2,  #0 
    beq     error_syscall
    and     r3,  r7, #0x000000ff
    ldr     r2,  [r2, r3, lsl #2]       @fetch syscall entry address
    cmn     r2,  #1                     @valid test with -1
    beq     error_syscall
    cmp     r2,  #0                     @valid test with NULL
    beq     error_syscall
    add     r0,  sp, #(4*7)
    stmfd   r0,  {r2}                   @replace syscall entry into lr slot.
    ldmfd   sp!, {r0} 
    msr     spsr_cxsf, r0               @recover irq flag.
    ldmfd   sp!, {r0-r3,r7, pc}^        @keep stack balance, ^copy spsr to cpsr.

error_cpu_mode:
    mrs     r0,  spsr
    mrs     r1,  cpsr
    bl      awos_arch_swc_mode_error
    ldmfd   sp!, {r0}                   @pop spsr
    ldmfd   sp!, {r0-r3, r7, lr}        @pop up, keep stack balance.

error_syscall:
    mrs     r0,  spsr
    mrs     r1,  cpsr
    mov     r3,  r2
    mov     r2,  r7
    stmfd   sp!, {lr}
    bl      awos_arch_swc_func_error
    ldmfd   sp!, {lr}
    ldmfd   sp!, {r0}                   @pop spsr
    ldmfd   sp!, {r0-r3, r7, lr}        @pop up, keep stack balance.
    b       .
cfi_debug_info_end  armv_swc_handler

cfi_debug_info_begin  armv_iabt_handler
    common_error_handler  awos_arch_iabt_trap_entry
cfi_debug_info_end armv_iabt_handler

cfi_debug_info_begin  armv_dabt_handler
    common_error_handler  awos_arch_dabt_trap_entry
cfi_debug_info_end armv_dabt_handler

/************************************************************************************
 * Name: armv_irq_handler_rtthr
 *
 * Description:
 *   Interrupt exception. Entered in IRQ mode with spsr = SVC CPSR, lr = SVC PC
 *
 ************************************************************************************/
#if defined CONFIG_RTTKERNEL
cfi_debug_info_begin  armv_irq_handler_rtthr
    clrex
#ifdef CONFIG_NEST_INTERRUPT
    switch_to_svcmode
#else
    @ !CONFIG_NEST_INTERRUPT
    stmfd   sp!, {r0-r12,lr}
               
    @ Be Care!!! backup previous mode(svc) sp, lr, and spsr register.
    @ in case the svc mode re-entrance during the irq procedure.
    mrs     r0,  cpsr
    mrs     r1,  spsr
    mov     r4,  r1
    orr     r1,  #INT_MASK
    msr     cpsr_c, r1
    mov     r1,  sp
    mov     r2,  lr
    mrs     r3,  spsr
    msr     cpsr,r0
    stmfd   sp!, {r1-r4}
#endif
    saveneonvfp r0, r4

    @ --------------------------------------------------------------    
    bl      awos_arch_irq_trap_enter
    mov     r0,  sp
    bl      awos_arch_irq_trap_entry
_interrupt_return_address:
    bl      awos_arch_irq_trap_leave
    @ --------------------------------------------------------------    

#ifndef CONFIG_NEST_INTERRUPT
    restoreneonvfp r0, r4

    @ restore previous mode(svc) sp, lr and spsr register.
    @ so we can return normally from the irq context.
    ldmfd   sp!, {r1-r4}
    mrs     r0,  cpsr
    orr     r4,  #INT_MASK
    msr     cpsr_c, r4
    mov     sp,  r1
    mov     lr,  r2
    msr     spsr,r3
    msr     cpsr,r0                                                                                                                                                                            
    ldmfd   sp!, {r0-r12,lr}        @ Reload saved registers
    switch_to_svcmode
    saveneonvfp r0, r4
#endif
    
@------------ void do_schedule(void) -----------------
@ MUST BE execution in SVC mode.
do_schedule:
    @b       .                      @ debug useage

    @ldr     r8, =rt_current_thread
    @ldr     r8, [r8]

    ldr     r0, =schedule_preempt_inirq
    blx     r0

    @ldr     r1, =rt_current_thread
    @ldr     r1, [r1]

    @mov     r0, r8
    @cmp     r0, r1
    @bne     awos_arch_bad_switch_error

    restoreneonvfp r0, r4

    ldmfd   sp!, {r4}               @ Pop new tasks SPSR
    msr     spsr_cxsf, r4
    ldmfd   sp, {r0-r13,lr,pc}^     @ restore irq context r0-r13,lr & pc. spsr ---> cpsr 
cfi_debug_info_end armv_irq_handler_rtthr

#elif defined CONFIG_ZEPHYRKERNEL

cfi_debug_info_begin armv_irq_handler_zephyr
    stmfd   sp!, {r0-r3, ip, lr}    @ Bakeup caller registers follow aapcs-eabi.

@-------------------------------------------------------------------    
    bl      awos_arch_irq_trap_enter
    bl      awos_arch_irq_trap_entry
    bl      awos_arch_irq_trap_leave
@-------------------------------------------------------------------    

    ldr     r0, =_kernel
@ get nested.
    ldr     r1, [r0, #___kernel_t_nested_OFFSET]
    cmp     r1, #0
    bne     no_reschedule

@ get current.
    ldr     r1, [r0, #___kernel_t_current_OFFSET]
    
    add     r2, r1, #___thread_t_base_OFFSET
    ldrh    r2, [r2, #___thread_base_t_preempt_OFFSET] 
    cmp     r2, #_PREEMPT_THRESHOLD 
    bhi     no_reschedule

@ get incoming thread.
    ldr     r0, [r0, #___kernel_t_ready_q_OFFSET]
    cmp     r0, r1
    beq     no_reschedule
    nop
    b       do_schedule

no_reschedule:
    ldmfd   sp!, {r0-r3, ip, lr}
    subs    pc,  lr, #4

@--------------- void do_schedule(void) -------------------
do_schedule:
    ldmfd   sp!, {r0-r3, ip, lr}    @ reload saved registers
    stmfd   sp,  {r0-r2}            @ save r0-r2, dont use '!' to keep irq stack balance.
    sub     r1,  sp, #4*3           @ save old tasks sp to r1
    sub     r2,  lr, #4             @ save old tasks pc to r2

    mrs     r0,  spsr               @ get cpsr of interrupt thread
    msr     cpsr_c, #(INT_MASK | ARMV7A_SVC_MODE)    @ Switch to SVC mode and no interrupt

    stmfd   sp!, {r2}               @ Push old tasks PC
    stmfd   sp!, {r3, ip, lr}       @ Push old tasks R3, R12, LR
    ldmfd   r1,  {r1-r3}            @ Get backed R0-R2
    stmfd   sp!, {r1-r3}            @ Push old tasks R0-R2
    stmfd   sp!, {r0}               @ Push old tasks CPSR
   
#ifdef CONFIG_TIMESLICING           
    ldr     r0,  =_update_time_slice_before_swap
    blx     r0                      @ Reset zephyr kernel timeslice.
#endif

    mrs     r0,  cpsr
    ldr     r1,  =__swap
    blx     r1                      @ Switch to the incoming thread.

    ldmfd   sp!, {r0}
    msr     spsr_cxsf, r0
    ldmfd   sp!, {r0-r3, ip, lr, pc}^
cfi_debug_info_end armv_irq_handler_zephyr
#else
    #error "os not support"
#endif

cfi_debug_info_begin  armv_fiq_handler
    stmfd   sp!, {r0-r7, lr}
    bl      awos_arch_fiq_trap_entry
    ldmfd   sp!, {r0-r7, lr}
    subs    pc,  lr, #4
cfi_debug_info_end armv_fiq_handler

cfi_debug_info_begin awos_arch_dcache_clean_flush_all
    push    {r4-r11}
    dmb
    mrc     p15, #1, r0, c0, c0, #1  @ read clid register
    ands    r3, r0, #0x7000000       @ get level of coherency
    mov     r3, r3, lsr #23
    beq     finished
    mov     r10, #0
loop1:
    add     r2, r10, r10, lsr #1
    mov     r1, r0, lsr r2
    and     r1, r1, #7
    cmp     r1, #2
    blt     skip
    mcr     p15, #2, r10, c0, c0, #0
    isb
    mrc     p15, #1, r1, c0, c0, #0
    and     r2, r1, #7
    add     r2, r2, #4
    ldr     r4, _MAX_WAY
    ands    r4, r4, r1, lsr #3
    clz     r5, r4
    ldr     r7, _MAX_IDX
    ands    r7, r7, r1, lsr #13
loop2:
    mov     r9, r4
loop3:
    orr     r11, r10, r9, lsl r5
    orr     r11, r11, r7, lsl r2
    mcr     p15, #0, r11, c7, c14, #2
    subs    r9, r9, #1
    bge     loop3
    subs    r7, r7, #1
    bge     loop2
skip:
    add     r10, r10, #2
    cmp     r3, r10
    bgt     loop1

finished:
    dsb
    isb
    pop     {r4-r11}
    bx      lr

_MAX_WAY:
   .word  0x3ff
_MAX_IDX:
   .word  0x7fff
cfi_debug_info_end awos_arch_dcache_clean_flush_all

cfi_debug_info_begin cpu_do_idle
    dsb
    wfi
    mov     pc, lr
cfi_debug_info_end cpu_do_idle

.end
