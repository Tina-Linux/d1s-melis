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
//*****************************************************************************
//  All Winner Micro, All Right Reserved. 2006-2008 Copyright (c)
//
//  File:               ss_sys.c
//
//  Description:  This file implements system functions for SS (Security System)
//
//  History:
//              2009/04/09      Berg Xing       0.10    Initial version
//
//*****************************************************************************
#include "ss_reg.h"

//*****************************************************************************
//  void ss_sys_open(void)
//  Description:    Request system source for SS
//
//  Arguments:      None
//
//  Return Value:   None
//*****************************************************************************
void ss_sys_open(void)
{
    unsigned long reg_val;

    //enable SS AHB clock
    reg_val = ss_read_w((CCMU_REGS_BASE + 0x0c));
    reg_val |= 0x1 << 2;    //SS AHB clock on
    ss_write_w((CCMU_REGS_BASE + 0x0c), reg_val);
    //enable SS working clock
    reg_val = ss_read_w((CCMU_REGS_BASE + 0x3c));
    reg_val &= ~(0x3 << 16);
    reg_val &= ~(0x3 << 20);
    reg_val |= 0x3 << 16;
    reg_val |= 0x0 << 20;   //0: core PLL, 1: dram PLL source
    reg_val |= 0x1 << 21;   //SS working clock on
    ss_write_w((CCMU_REGS_BASE + 0x3c), reg_val);
}
//*****************************************************************************
//  void ss_sys_close(void)
//  Description:    Release system source for SS
//
//  Arguments:      None
//
//  Return Value:   None
//*****************************************************************************
void ss_sys_close(void)
{
    unsigned long reg_val;

    //disable SS AHB clock
    reg_val = ss_read_w((CCMU_REGS_BASE + 0x0c));
    reg_val &= ~(0x1 << 2);         //SS AHB clock off
    ss_write_w((CCMU_REGS_BASE + 0x0c), reg_val);
    //disable SS working clock
    reg_val = ss_read_w((CCMU_REGS_BASE + 0x3c));
    reg_val &= ~(0x1 << 21);    //SS working clock off
    ss_write_w((CCMU_REGS_BASE + 0x3c), reg_val);
}
