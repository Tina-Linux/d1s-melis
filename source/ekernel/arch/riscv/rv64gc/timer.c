/*
 * ===========================================================================================
 *
 *       Filename:  timer.c
 *
 *    Description:  tick impl.
 *
 *        Version:  Melis3.0
 *         Create:  2020-06-29 13:59:33
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-07-09 09:29:08
 *
 * ===========================================================================================
 */
#include <stdio.h>
#include <csr.h>
#include <sbi.h>
#include <timex.h>

//24M HZ
void awos_arch_tick_increase(void);

static unsigned long delta = 24000000 / CONFIG_HZ;

void system_tick_init(void)
{
    csr_set(sie, SIE_STIE);
    sbi_set_timer(get_cycles64() + delta);
}

void riscv_timer_interrupt(void)
{
    csr_clear(sie, SIE_STIE);

    awos_arch_tick_increase();

    sbi_set_timer(get_cycles64() + delta);
    csr_set(sie, SIE_STIE);
}
