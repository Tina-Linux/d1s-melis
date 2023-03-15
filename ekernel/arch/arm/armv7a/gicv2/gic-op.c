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
#include <stdio.h>
#include "interrupt.h"
#include "gicv2_reg.h"
#include <log.h>
#include <sunxi_hal_common.h>

uint32_t arm_gic_nlines(void)
{
    uint32_t regval;
    uint32_t field;

    /* Get the number of interrupt lines. */
    regval = hal_readl(GIC_CON_TYPE);
    field = (regval & 0x1f);
    return (field + 1) << 5;
}

static inline void sr32(uint32_t addr, uint32_t start_bit, uint32_t num_bits, uint32_t value)
{
    uint32_t tmp, msk = (1 << num_bits) - 1;

    tmp = (hal_readl(addr) & (~(msk << start_bit)));
    tmp |= (value << start_bit);
    hal_writel(tmp, addr);
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

    base_os = irq_no >> 5;
    base = GIC_SET_EN(base_os);
    bit_os = irq_no & 0x1f;
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

    base_os = irq_no >> 5;
    base = GIC_CLR_EN(base_os);
    bit_os = irq_no & 0x1f;
    hal_writel((1 << bit_os), base);

    return 0;
}

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
        base_os = irq_no >> 2;
        base = GIC_SGI_PROC_TARG(base_os);
        bit_os = (irq_no % 4) << 3;
    }
    else if ((irq_no >= GIC_SRC_PPI0) && (irq_no <= GIC_SRC_PPI15))
    {
        irq_no -= 16;
        base_os = irq_no >> 2;
        base = GIC_PPI_PROC_TARG(base_os);
        bit_os = (irq_no % 4) << 3;
    }
    else if ((irq_no >= 32) && (irq_no < arm_gic_nlines()))
    {
        irq_no -= 32;
        base_os = irq_no >> 2;
        base = GIC_SPI_PROC_TARG(base_os);
        bit_os = (irq_no % 4) << 3;
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

    base_os = id >> 5;
    base = GIC_PEND_CLR(base_os);
    bit_os = id & 0x1f;
    hal_writel(1 << bit_os, base);
}
