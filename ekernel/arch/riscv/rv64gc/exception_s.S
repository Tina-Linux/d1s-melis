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
.section .text.entry, "ax", @progbits
.align 8
.option norvc
.global handle_exception
handle_exception:
    # save interrupted context to current thread stack
    addi sp, sp, -34 * 8

    sd   x1,   1 * 8(sp)
    sd   x3,   3 * 8(sp)
    sd   x4,   4 * 8(sp)
    sd   x5,   5 * 8(sp)
    sd   x6,   6 * 8(sp)
    sd   x7,   7 * 8(sp)
    sd   x8,   8 * 8(sp)
    sd   x9,   9 * 8(sp)
    sd   x10, 10 * 8(sp)
    sd   x11, 11 * 8(sp)
    sd   x12, 12 * 8(sp)
    sd   x13, 13 * 8(sp)
    sd   x14, 14 * 8(sp)
    sd   x15, 15 * 8(sp)
    sd   x16, 16 * 8(sp)
    sd   x17, 17 * 8(sp)
    sd   x18, 18 * 8(sp)
    sd   x19, 19 * 8(sp)
    sd   x20, 20 * 8(sp)
    sd   x21, 21 * 8(sp)
    sd   x22, 22 * 8(sp)
    sd   x23, 23 * 8(sp)
    sd   x24, 24 * 8(sp)
    sd   x25, 25 * 8(sp)
    sd   x26, 26 * 8(sp)
    sd   x27, 27 * 8(sp)
    sd   x28, 28 * 8(sp)
    sd   x29, 29 * 8(sp)
    sd   x30, 30 * 8(sp)
    sd   x31, 31 * 8(sp)

    csrr a0, sepc
    sd   a0, 0 * 8(sp)
    csrr a1, sstatus
    sd   a1, 32 * 8(sp)
    csrr a2, sscratch
    sd   a2, 33 * 8(sp)

    # backup stack pointer for later check.
    move  s0, sp
    move  a0, sp
    addi  a0, a0, 34 * 8
    sd    a0, 2 * 8(sp)

    csrr  a0, sstatus
    call  fpu_save_inirq

    # re-backup the sstatus in chance clean the fpustatus.
    csrr s1, sstatus
    sd   s1, 32 * 8(s0)

    # check bit[63], 0:interrupt, 1:exceptions
    csrr  a0, scause
    bge   a0, zero, .exception

.interrupt:
    # handle interrupts.
    # set sp to interrupt stack
    mv    a0, sp
    la    sp, __interrupt_stack_end__
    addi  sp, sp, -1 * 8
    sd    a0, 1 * 8(sp)

    call  hal_interrupt_enter

    csrr  a0, scause
    csrr  a1, sepc
    csrr  a2, stval
    move  a3, s0
    call  riscv_cpu_handle_interrupt

    call  hal_interrupt_leave
    # restore thread context stack
    ld    a0, 1 * 8(sp)
    mv    sp, a0
    j     .restore_all

.exception:
    # handle syscall and exceptions.
    csrr  a0, scause
    csrr  a1, sepc
    csrr  a2, stval
    move  a3, s0
    call  riscv_cpu_handle_exception
    nop

.restore_all:
    csrr a0, sstatus
    call fpu_restore_inirq

    # guanatee SPP bit was set, means we are from supervisor mode.
    csrr a0, sstatus
    ori  a0, a0, 0x100
    csrw sstatus, a0
    # keep sstatus not change inirq.
0:
    bne  a0, s1, 0b

    # keep stack balance.
1:
    bne  sp, s0, 1b

#.option push
#.option norelax
#    la    a0, __global_pointer$
#.option pop

.gpldr:
    auipc a0, %pcrel_hi(__global_pointer$)
    addi  a0, a0,%pcrel_lo(.gpldr)

2:  # x3 is gp in abi
    bne   gp, a0,2b

    jal  schedule_preempt_inirq

    # resotre interrupted context
    ld   a0,   0 * 8(sp)
    csrw sepc, a0

    ld   x1,   1 * 8(sp)

    ld   a0,   32 * 8(sp)
    csrw sstatus, a0

    ld   a0,   33 * 8(sp)
    csrw sscratch, a0

    ld   x3,   3 * 8(sp)
    ld   x4,   4 * 8(sp)
    ld   x5,   5 * 8(sp)
    ld   x6,   6 * 8(sp)
    ld   x7,   7 * 8(sp)
    ld   x8,   8 * 8(sp)
    ld   x9,   9 * 8(sp)
    ld   x10, 10 * 8(sp)
    ld   x11, 11 * 8(sp)
    ld   x12, 12 * 8(sp)
    ld   x13, 13 * 8(sp)
    ld   x14, 14 * 8(sp)
    ld   x15, 15 * 8(sp)
    ld   x16, 16 * 8(sp)
    ld   x17, 17 * 8(sp)
    ld   x18, 18 * 8(sp)
    ld   x19, 19 * 8(sp)
    ld   x20, 20 * 8(sp)
    ld   x21, 21 * 8(sp)
    ld   x22, 22 * 8(sp)
    ld   x23, 23 * 8(sp)
    ld   x24, 24 * 8(sp)
    ld   x25, 25 * 8(sp)
    ld   x26, 26 * 8(sp)
    ld   x27, 27 * 8(sp)
    ld   x28, 28 * 8(sp)
    ld   x29, 29 * 8(sp)
    ld   x30, 30 * 8(sp)
    ld   x31, 31 * 8(sp)

    addi sp,  sp, 34 * 8
    sret

.end
