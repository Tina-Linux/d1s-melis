/*
 * ===========================================================================================
 *
 *       Filename:  vfp.c
 *
 *    Description:  neon unit driver.
 *
 *        Version:  Melis3.0
 *         Create:  2020-03-13 12:18:48
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-03-14 11:15:10
 *
 * ===========================================================================================
 */
#include <rtthread.h>
#include <kconfig.h>
#include <debug.h>
#include <stdio.h>
#include <arch.h>
#include <log.h>

uint32_t kernel_neon_begin(void)
{
    uint32_t fpexc_saved, fpexc;

    RT_ASSERT(rt_interrupt_get_nest() == 0);

    rt_enter_critical();

    fpexc_saved = fmrx(FPEXC);
    fpexc = fpexc_saved | FPEXC_EN;
    fmxr(FPEXC, fpexc);

    return fpexc_saved;
}

void kernel_neon_end(uint32_t saved_fpexc)
{
    RT_ASSERT(rt_interrupt_get_nest() == 0);

    fmxr(FPEXC, saved_fpexc);
    rt_exit_critical();
}
