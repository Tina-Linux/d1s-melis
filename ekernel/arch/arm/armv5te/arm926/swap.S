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
#include <cfi.h>
#include <zephyr/offsets.h>

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

.end
