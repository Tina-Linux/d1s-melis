/*
 * ===========================================================================================
 *
 *       Filename:  arch_init.c
 *
 *    Description:  arch specifi init routine.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-20 15:23:16
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-11-21 17:42:22
 *
 * ===========================================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

int awos_arch_specific_init(void)
{
    // Init generic timer and set setup the tick interrupt.
    void arm_generic_timer_init(int irq, unsigned int freq_override);
    int32_t interrupt_init(void);

    interrupt_init();
    arm_generic_timer_init(0, 0);

    return 0;
}
