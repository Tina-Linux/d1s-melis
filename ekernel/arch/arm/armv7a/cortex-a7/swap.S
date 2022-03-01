/*
 * =====================================================================================
 *
 *       Filename:  swap.S
 *
 *    Description:  nesscessary for zehpyr thread context switching.
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-18 15:38:00
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-04-20 09:47:25
 *
 * =====================================================================================
 */

#include <cfi.h>
#include <zephyr/offsets.h>

.extern rt_thread_newborn
#ifdef CONFIG_ZEPHYRKERNEL
.extern _k_neg_eagain
.extern _kernel
.extern _thread_entry
.global _thread_entry_wrapper
.global __swap
@ unsigned int __swap(unsigned int key);
cfi_debug_info_begin  __swap
    stmfd   sp!, {r4-r12, lr}
    stmfd   sp!, {r0}
    mrc     p15, 0, r0, c13, c0, 0
    stmfd   sp!, {r0}
     
    ldr     r0, =_kernel
    ldr     r1, [r0, #___kernel_t_current_OFFSET]
    add     r2, r1, #___thread_t_callee_saved_OFFSET
    str     sp, [r2, #___callee_saved_t_sp_OFFSET]

    ldr     r2, =_k_neg_eagain
    ldr     r2, [r2]
    add     r3, r1, #___thread_t_arch_OFFSET
    str     r2, [r3, #___thread_arch_t_swap_return_value_OFFSET]

    ldr     r1, [r0, #___kernel_t_ready_q_OFFSET]
    str     r1, [r0, #___kernel_t_current_OFFSET]

    add     r0, r1, #___thread_t_arch_OFFSET
    ldr     r0, [r0, #___thread_arch_t_swap_return_value_OFFSET]

    add     r2, r1, #___thread_t_callee_saved_OFFSET
    ldr     sp, [r2, #___callee_saved_t_sp_OFFSET]

    ldmfd   sp!, {r1}
    mcr     p15, 0, r1, c13, c0, 0
    ldmfd   sp!, {r1}
#if 1
    msr     spsr_cxsf, r1
    ldmfd   sp!, {r4-r12, pc}^
#else
    msr     cpsr_cxsf, r1
    ldmfd   sp!, {r4-r12, pc}
#endif
cfi_debug_info_end  __swap

@ void _thread_entry_wrapper(_thread_entry_t, void *, void *, void *);
cfi_debug_info_begin _thread_entry_wrapper
    @ This all corresponds to struct init_stack_frame defined in
    @ _new_thread. We need to take this stuff off the stack and 
    @ put it in the apporpriate registers, r0-r3 as parameter.

    ldmfd   sp!, {r0-r3}

    @ Can't return from here, just put infinite loop rtn to lr
    mov     lr,  pc 
    ldr     pc,  =_thread_entry
    b       .
cfi_debug_info_end _thread_entry_wrapper

#else

#ifdef CONFIG_CONSISTENT_CONTEXT_SWITCH
@ void ret_from_create_s(void);
cfi_debug_info_begin ret_from_create_s
    @ SP必须在公共接口处对齐8字节.
    push    {ip,lr}
    
    bl      rt_thread_newborn

    cmp     r5, #0
    movne   r0, r4
    addne   lr, pc, #4
    movne   pc, r5

    b       .          @ dead here.
1:  
    pop    {ip, pc} 

cfi_debug_info_end ret_from_create_s
#endif
#endif

cfi_debug_info_begin mcount
    nop
    bx lr
cfi_debug_info_end mcount

cfi_debug_info_begin __gnu_mcount_nc
    push    {r0, r1, r2, r3, lr}
    subs    r1, lr, #0                  @ callee - current lr
    ldr     r0, [sp, #20]               @ caller - at the top of the stack
    bl      mcount
    pop	    {r0, r1, r2, r3, ip, lr}
    bx	    ip
cfi_debug_info_end __gnu_mcount_nc

/*------------------------------------------------------------------------
 * pause or halt  -  Place the processor in a hard loop
 *------------------------------------------------------------------------
 */
.global halt
.global pause
halt:
pause:
    cpsid   i               @ Disable interrupts
dloop:
    b       dloop           @ Dead loop

.end
