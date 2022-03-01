/*
 * =====================================================================================
 *
 *       Filename:  irqs_gnu.S
 *
 *    Description:  exception routine, common glue layer, support both 
 *                  RT-Thread and Zephyr Kernel.
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 10:09:09
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-07-22 20:10:12
 *
 * =====================================================================================
 */

#include <cpu.h>
#include <cfi.h>

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
.extern awos_arch_rsv_trap_entry
.extern awos_arch_bad_stack_error      
.extern melis_pend_sched_flag
.extern melis_sched_from_thread
.extern melis_sched_to_thread
.extern melis_syscall_table

.global armv5te_rst_handler
.global armv5te_und_handler
.global armv5te_swc_handler
.global armv5te_iabt_handler
.global armv5te_dabt_handler
.global armv5te_rsv_handler

#ifdef CONFIG_RTTKERNEL
.global armv5te_irq_handler_rtthr
#elif defined CONFIG_ZEPHYRKERNEL
.global armv5te_irq_handler_zephyr
#endif 

.global armv5te_fiq_handler
.global jtag_enable_op

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
    ldmfd   sp!, {r0-r12}
    movs    pc,  lr
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

cfi_debug_info_begin  armv5te_rst_handler
    b       awos_arch_rst_trap_entry
    b       .
cfi_debug_info_end armv5te_rst_handler

cfi_debug_info_begin  armv5te_rsv_handler
    b       awos_arch_rsv_trap_entry
    b       .
cfi_debug_info_end armv5te_rsv_handler

cfi_debug_info_begin  armv5te_und_handler
    common_error_handler  awos_arch_und_trap_entry
cfi_debug_info_end armv5te_und_handler

cfi_debug_info_begin  armv5te_swc_handler
    @ state: arm mode, irq disable.
    stmfd   sp!, {r0-r3, r7, lr} 
    mrs     r0,  spsr
    stmfd   sp!, {r0}
    and     r1,  r0, #ARM9_MODE_MASK
    cmp     r1,  #ARM9_SVC_MODE
    beq     do_syscall

    @processor cant not be user&syst mode present.
    cmp     r1,  #ARM9_USR_MODE
    beq     error_cpu_mode
    cmp     r1,  #ARM9_SYS_MODE
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

.armcc:
    mov     r7,  r0

.gnucc:
    ldr     r2,  =melis_syscall_table
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
cfi_debug_info_end  armv5te_swc_handler

cfi_debug_info_begin  armv5te_iabt_handler
    common_error_handler  awos_arch_iabt_trap_entry
cfi_debug_info_end armv5te_iabt_handler

cfi_debug_info_begin  armv5te_dabt_handler
    common_error_handler  awos_arch_dabt_trap_entry
cfi_debug_info_end armv5te_dabt_handler

#if defined CONFIG_RTTKERNEL
cfi_debug_info_begin  armv5te_irq_handler_rtthr
    stmfd   sp!, {r0-r12,lr}

    @Be Care!!! backup previous mode(svc) sp, lr, and spsr register.
    @in case the svc mode re-entrance during the irq procedure.
    mrs     r0,  cpsr
    mrs     r1,  spsr
    orr     r1,  #INT_MASK
    msr     cpsr_c, r1
    mov     r1,  sp
    mov     r2,  lr
    mrs     r3,  spsr
    msr     cpsr_c, r0
    stmfd   sp!, {r1-r3}

    @--------------------------------------------------------------    
    bl      awos_arch_irq_trap_enter
    bl      awos_arch_irq_trap_entry
    bl      awos_arch_irq_trap_leave
    @--------------------------------------------------------------    

    @restore previous mode(svc) sp, lr and spsr register.
    @so we can return normally from the irq context.
    ldmfd   sp!, {r1-r3}
    mrs     r0,  cpsr
    mrs     r4,  spsr
    orr     r4,  #INT_MASK
    msr     cpsr_c, r4
    mov     sp,  r1
    mov     lr,  r2
    msr     spsr_cxsf, r3
    msr     cpsr_c, r0

    @check irq sp is balance.
    mov     r0,  sp
    ldr     r1,  =EPOS_STACK_IRQ_SAFE_CHK
    cmp     r0,  r1
    beq     1f
    b       awos_arch_bad_stack_error @ irq stack corruption. 

1: 
    @If melis_pend_sched_flag set,
    @jump to do_schedule and never return
    ldr     r0,  =melis_pend_sched_flag
    ldr     r1,  [r0]
    cmp     r1,  #1
    beq     do_schedule

    ldmfd   sp!, {r0-r12,lr}
    subs    pc,  lr, #4

@------ void do_schedule(uint32_t flag) -----------------
do_schedule:
    mov     r1,  #0                 @ Clear flag
    str     r1,  [r0]               @ Save to flag variable

    ldmfd   sp!, {r0-r12,lr}        @ Reload saved registers
    stmfd   sp,  {r0-r2}            @ Save R0-R2
    sub     r1,  sp, #4*3           @ Save old tasks SP to R1
    sub     r2,  lr, #4             @ Save old tasks PC to R2

    mrs     r0,  spsr               @ Get CPSR of interrupt thread
    msr     cpsr_c, #(INT_MASK | ARM9_SVC_MODE)    @Switch to SVC mode and no interrupt

    stmfd   sp!, {r2}               @ Push old tasks PC
    stmfd   sp!, {r3-r12,lr}        @ Push old tasks LR,R12-R3
    ldmfd   r1,  {r1-r3}
    stmfd   sp!, {r1-r3}            @ Push old tasks R2-R0
    stmfd   sp!, {r0}               @ Push old tasks CPSR
    mrc     p15, 0, r4, c13, c0, 0
    stmfd   sp!, {r4} 

    ldr     r4,  =melis_sched_from_thread
    ldr     r5,  [r4]               @ R5 = stack ptr in old taskss TCB
    str     sp,  [r5]               @ Store SP in preempted taskss TCB

    ldr     r6,  =melis_sched_to_thread
    ldr     r6,  [r6]               @ R6 = stack ptr in new taskss TCB
    ldr     sp,  [r6]               @ Get new tasks stack pointer

    ldmfd   sp!, {r4}
    mcr     p15, 0, r4, c13, c0, 0  @ restore new task fcse
    ldmfd   sp!, {r4}               @ Pop new tasks SPSR
    msr     spsr_cxsf, r4
    ldmfd   sp!, {r0-r12,lr,pc}^    @ pop new tasks R0-R12,LR & PC SPSR to CPSR
cfi_debug_info_end armv5te_irq_handler_rtthr

#elif defined CONFIG_ZEPHYRKERNEL

cfi_debug_info_begin armv5te_irq_handler_zephyr
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
    msr     cpsr_c, #(INT_MASK | ARM9_SVC_MODE)    @ Switch to SVC mode and no interrupt

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
cfi_debug_info_end armv5te_irq_handler_zephyr
#else
    #error "os not support"
#endif

cfi_debug_info_begin  armv5te_fiq_handler
    stmfd   sp!, {r0-r7, lr}
    bl      awos_arch_fiq_trap_entry
    ldmfd   sp!, {r0-r7, lr}
    subs    pc,  lr, #4
cfi_debug_info_end armv5te_fiq_handler

cfi_debug_info_begin  jtag_enable_op
    ldr     r0,  =0xf1c208b4
    ldr     r1,  =0x00444444
    str     r1,  [r0]
    bx      lr
cfi_debug_info_end  jtag_enable_op

/*
 * cpu_arm926_proc_fin()
 */
cfi_debug_info_begin cpu_arm926_proc_fin
    stmfd   sp!, {lr}
    mov ip, #INT_MASK | ARM9_SVC_MODE
    msr cpsr_c, ip
    bl  cpu_arm926_cache_clean_invalidate_all
    mrc p15, 0, r0, c1, c0, 0       @ ctrl register
    bic r0, r0, #0x1000             @ ...i............
    bic r0, r0, #0x000e             @ ............wca.
    mcr p15, 0, r0, c1, c0, 0       @ disable caches
    ldmfd   sp!, {pc}
cfi_debug_info_end cpu_arm926_proc_fin

/*
 * cpu_arm926_reset(loc)
 * Perform a soft reset of the system.  Put the CPU into the
 * same state as it would be if it had been reset, and branch
 * to what would be the reset vector.
 * loc: location to jump to for soft reset
 */

.align  5
cfi_debug_info_begin  cpu_arm926_reset
    mov     ip, #0
    mcr     p15, 0, ip, c7, c7, 0       @ invalidate I,D caches
    mcr     p15, 0, ip, c7, c10, 4      @ drain WB
    mcr     p15, 0, ip, c8, c7, 0       @ invalidate I & D TLBs
    mrc     p15, 0, ip, c1, c0, 0       @ ctrl register
    bic     ip, ip, #0x000f             @ ............wcam
    bic     ip, ip, #0x1100             @ ...i...s........
    mcr     p15, 0, ip, c1, c0, 0       @ ctrl register
    mov     pc, r0
cfi_debug_info_end  cpu_arm926_reset

/*
 * cpu_arm926_do_idle(void)
 */
.align  5
cfi_debug_info_begin cpu_arm926_do_idle
    mov     r0, #0
    @mrc    p15, 0, r1, c1, c0, 0       @ Read control register
    mcr     p15, 0, r0, c7, c10, 4      @ Drain write buffer
    @bic    r2, r1, #1 << 12
    @mrs    r3, cpsr                    @ Disable FIQs while Icache
    @orr    ip, r3, #0x40               @ is disabled
    @msr    cpsr_c, ip
    @mcr    p15, 0, r2, c1, c0, 0       @ Disable I cache
    mcr     p15, 0, r0, c7, c0, 4       @ Wait for interrupt
    @mcr    p15, 0, r1, c1, c0, 0       @ Restore ICache enable
    @msr    cpsr_c, r3                  @ Restore FIQ state
    mov     pc, lr
cfi_debug_info_end cpu_arm926_do_idle
.end
