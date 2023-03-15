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
#include <cpu.h>

// void awos_arch_fpu_init(void)
cfi_debug_info_begin awos_arch_fpu_init
    mrc     p15, 0, r0, c1, c0, 2    @ Read CP Access register
    ldr     r0, =(0xf << 20)         @ Enable full access to NEON/VFP (Coprocessors 10 and 11)
    mcr     p15, 0, r0, c1, c0, 2    @ Write CP Access register

    vmrs    r0, fpexc                @ Switch on the VFP and NEON hardware
    orr     r0, r0, #0x40000000      @ Set EN bit in FPEXC
    vmsr    fpexc, r0
    bx      lr
cfi_debug_info_end awos_arch_fpu_init

// void enable_vfp_early(void)
cfi_tlb_info_begin enable_vfp_early
    mrc     p15, 0, r0, c1, c0, 2    @ Read CP Access register
    ldr     r0, =(0xf << 20)         @ Enable full access to NEON/VFP (Coprocessors 10 and 11)
    mcr     p15, 0, r0, c1, c0, 2    @ Write CP Access register

    vmrs    r0, fpexc                @ Switch on the VFP and NEON hardware
    orr     r0, r0, #0x40000000      @ Set EN bit in FPEXC
    vmsr    fpexc, r0
    bx      lr
cfi_tlb_info_end enable_vfp_early
