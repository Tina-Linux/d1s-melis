/*
 * Allwinnertech rtos arch timer file.
 * Copyright (C) 2019  Allwinnertech Co., Ltd. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "interrupt.h"
#include <of/of.h>
#include <kconfig.h>
#include <stdio.h>
#include <log.h>

static unsigned int timer_reload_val;

#define ARCH_TIMER_CTRL_ENABLE          (1 << 0)
#define ARCH_TIMER_CTRL_IT_MASK         (1 << 1)
#define ARCH_TIMER_CTRL_IT_STAT         (1 << 2)

static unsigned int read_cntfrq(void)
{
    unsigned int cntfrq;
    asm volatile("mrc  p15, 0, %0, c14, c0, 0" : "=r"(cntfrq));
    return cntfrq;
}

static void write_cntp_tval(unsigned int cntp_tval)
{
    asm volatile("mcr p15, 0, %0, c14, c2, 0" :: "r"(cntp_tval));
}

static void write_cntp_ctl(unsigned int cntp_ctl)
{
    asm volatile("mcr p15, 0, %0, c14, c2, 1" ::"r"(cntp_ctl));
    asm volatile("isb");
}

static unsigned int read_cntp_ctl(void)
{
    unsigned int val = 0;
    asm volatile("mrc p15, 0, %0, c14, c2, 1" : "=r"(val));
    asm volatile("isb");
    return val;
}

static void set_next_event(unsigned int reload)
{
    unsigned int ctl;

    ctl = read_cntp_ctl();
    ctl |= ARCH_TIMER_CTRL_ENABLE;
    ctl &= ~ARCH_TIMER_CTRL_IT_MASK;
    write_cntp_tval(reload);
    write_cntp_ctl(ctl); /* enable timer */
}

static irqreturn_t platform_tick(int no, void *para)
{
    unsigned int ctl;

    ctl = read_cntp_ctl();

    if (ctl & ARCH_TIMER_CTRL_IT_STAT)
    {
        //mask now.
        ctl |= ARCH_TIMER_CTRL_IT_MASK;
        write_cntp_ctl(ctl); /* disable timer */

        void awos_arch_tick_increase(void);
        awos_arch_tick_increase();

        set_next_event(timer_reload_val);
        return IRQ_HANDLED;
    }

    return IRQ_NONE;
}

uint32_t g_irq_arg[3] = { 0 };
void arm_generic_timer_init(int irq, unsigned int freq_override)
{
    unsigned int cntfrq = 0, *rate = NULL;
    int ret, len, irqnum;
    uint32_t intsize = 0;

#ifdef CONFIG_OF
    struct device_node *np = NULL, *nc = NULL;
    nc = of_find_node_by_name(NULL, "arch-timer");
    if (nc)
    {
        int i, res;

        rate = (unsigned int *)of_get_property(nc, "clock-frequency", &len);
        if (!rate || len != 4)
        {
            __err("cant get arch-timer properity value.");
            return;
        }

        np = of_irq_find_parent(nc);
        if (np == NULL)
        {
            __err("cant get arch-timer device_node parent.");
            return;
        }
        of_property_read_u32(np, "#interrupt-cells", &intsize);
        //__log("clock-freq = %d, intsize %d.nw->full_name %s.", __builtin_bswap32(*rate), intsize, np->full_name);
        // cell0 is security mode, and cell1 is ns mode.
        for (i = 0; i < intsize; i ++)
        {
            res = of_property_read_u32_index(nc, "interrupts",
                                             (0 * intsize) + i,
                                             g_irq_arg + i);
            if (res)
            {
                __err("get arch-timer interrupts paramter failure.");
                return;
            }
        }
        irqnum = g_irq_arg[1];
        cntfrq = __builtin_bswap32(*rate);

        if (g_irq_arg[0]) // PPI interrupt
        {
            irqnum += 16;
        }
    }
    else
#endif
    {
        //ns:0 secuirty mode. ns:1 normal mode
        int ns = 0;

        if (freq_override == 0)
        {
            cntfrq = read_cntfrq(); /* default: 24MHz */
            if (!cntfrq)
            {
                cntfrq = 24000000;
            }
        }
        else
        {
            cntfrq = freq_override;
        }

        //normal world, irqnum is 30
        if (ns == 1)
        {
            irqnum = 30;
        }
        else
        {
            irqnum = 29;
        }
    }

    timer_reload_val = cntfrq / CONFIG_HZ;
    set_next_event(timer_reload_val);

    ret = request_irq(irqnum, platform_tick, 0, "tick", NULL);
    if (ret != 0)
    {
        __err("fatal error, request irq failure.");
        while (1)
            ;
        return;
    }
}
