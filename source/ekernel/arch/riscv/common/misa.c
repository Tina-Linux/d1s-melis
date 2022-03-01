/*
 * =====================================================================================
 *
 *       Filename:  stack_init.c
 *
 *    Description:  RISCV Processor C-ABI thread stack init.
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 11:58:23
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-06-28 19:59:14
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <string.h>
#include <log.h>
#include <misa.h>

void decode_misa(unsigned long misa)
{
    unsigned long mxl_val;
    char *misa_mxl[] =
    {
        "INVAL_ISA",
        "32BIT_ISA",
        "64BIT_ISA",
        "128BIT_ISA"
    };
#ifdef CONFIG_64BIT
    mxl_val = misa >> 62;
#else
    mxl_val = misa >> 30;
#endif
    printk("%s Support:\n\r", misa_mxl[mxl_val]);

    if (misa & RVI)
    {
        printk("I ");
    }

    if (misa & RVE)
    {
        printk("E ");
    }

    if (misa & RVM)
    {
        printk("M ");
    }

    if (misa & RVA)
    {
        printk("A ");
    }

    if (misa & RVF)
    {
        printk("F ");
    }

    if (misa & RVD)
    {
        printk("D ");
    }

    if (misa & RVC)
    {
        printk("C ");
    }

    if (misa & RVS)
    {
        printk("S ");
    }
    if (misa & RVU)
    {
        printk("U ");
    }
    if (misa & RVH)
    {
        printk("H ");
    }
    printk("\n\r");
}
