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
