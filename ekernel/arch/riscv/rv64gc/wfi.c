/*
 * =====================================================================================
 *
 *       Filename:  context_switch.S
 *
 *    Description:  this file contain the arch dependent routines. common layer for both
 *                  RT-Thread and Zephyr Kernel.
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 10:10:33
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-06-28 14:06:21
 *
 * =====================================================================================
 */

#include <csr.h>
#include <irqflags.h>

static void arch_cpu_idle(void)
{
    arch_local_irq_disable();
    wait_for_interrupt();
    arch_local_irq_enable();
}

void cpu_do_idle(void)
{
    arch_cpu_idle();
}
