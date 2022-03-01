/*
 * Allwinnertech rtos generic interrupt controller file.
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
#include "gicv2_reg.h"
#include <stdio.h>
#include <log.h>

uint32_t arm_gic_nlines(void)
{
    uint32_t regval;
    uint32_t field;

    /* Get the number of interrupt lines. */
    regval = readl(GIC_CON_TYPE);
    field = (regval & 0x1f);
    return (field + 1) << 5;
}

static inline void sr32(uint32_t addr, uint32_t start_bit, uint32_t num_bits, uint32_t value)
{
    uint32_t tmp, msk = (1 << num_bits) - 1;

    tmp = (readl(addr) & (~(msk << start_bit)));
    tmp |= (value << start_bit);
    writel(tmp, addr);
}

int32_t irq_enable(uint32_t irq_no)
{
    uint32_t base;
    uint32_t base_os;
    uint32_t bit_os;

    if (irq_no >= arm_gic_nlines())
    {
        __err("exceed the max irq number supported!");
        return -1;
    }

    base_os = irq_no >> 5; // 除32
    base = GIC_SET_EN(base_os);
    bit_os = irq_no & 0x1f; // %32
    sr32(base, bit_os, 1, 1);

    return 0;
}

int32_t irq_disable(uint32_t irq_no)
{
    uint32_t base;
    uint32_t base_os;
    uint32_t bit_os;

    if (irq_no >= arm_gic_nlines())
    {
        __err("exceed the max irq number supported!");
        return -1;
    }

    base_os = irq_no >> 5; // 除32
    base = GIC_CLR_EN(base_os);
    bit_os = irq_no & 0x1f; // %32
#if 0
    sr32(base, bit_os, 1, 1);
#else
    writel((1 << bit_os), base);
#endif

    return 0;
}

/*
 * Set irq to target CPUs; one irq has max 8 target CPUs
 * One Interrupt Processor Targets Register contains 4 interrupts configuration
 * One bit means one relevant CPU interface
 * 0bxxxxxxx1 means CPU interface 0; 0bxxxxxx1x means CPU interface 1
 */
int32_t set_irq_target(uint32_t irq_no, uint32_t target)
{
    uint32_t base;
    uint32_t base_os;
    uint32_t bit_os;

    if (irq_no >= arm_gic_nlines())
    {
        return -1;
    }

    if (irq_no <= GIC_SRC_SGI15)
    {
        base_os = irq_no >> 2; // 除4
        base = GIC_SGI_PROC_TARG(base_os);
        bit_os = (irq_no % 4) << 3; // %4
    }
    else if ((irq_no >= GIC_SRC_PPI0) && (irq_no <= GIC_SRC_PPI15))
    {
        irq_no -= 16;
        base_os = irq_no >> 2; // 除4
        base = GIC_PPI_PROC_TARG(base_os);
        bit_os = (irq_no % 4) << 3; // %4
    }
    else if ((irq_no >= 32) && (irq_no < arm_gic_nlines()))
    {
        irq_no -= 32;
        base_os = irq_no >> 2; // 除4
        base = GIC_SPI_PROC_TARG(base_os);
        bit_os = (irq_no % 4) << 3; // %4
    }
    else
    {
        return 0;
    }

    sr32(base, bit_os, 8, target);

    return 0;
}

void gic_clear_pending(uint32_t id)
{
    uint32_t base;
    uint32_t base_os;
    uint32_t bit_os;

    base_os = id >> 5; // 除32
    base = GIC_PEND_CLR(base_os);
    bit_os = id & 0x1f; // %32
    writel(1 << bit_os, base);
}
