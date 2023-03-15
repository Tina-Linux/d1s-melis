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
#ifndef _IRQ_INTERNAL_H
#define _IRQ_INTERNAL_H

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

struct irq_chip;
struct irq_desc;

typedef void (*irq_flow_handler_t)(struct irq_desc *desc);
int generic_handle_irq(unsigned int irq);

struct irq_data
{
    uint32_t            mask;
    unsigned int        irq;
    struct irq_chip     *chip;
    void                *chip_data;
};

enum
{
    IRQCHIP_SET_TYPE_MASKED     = (1 <<  0),
    IRQCHIP_EOI_IF_HANDLED      = (1 <<  1),
    IRQCHIP_MASK_ON_SUSPEND     = (1 <<  2),
    IRQCHIP_ONOFFLINE_ENABLED   = (1 <<  3),
    IRQCHIP_SKIP_SET_WAKE       = (1 <<  4),
    IRQCHIP_ONESHOT_SAFE        = (1 <<  5),
    IRQCHIP_EOI_THREADED        = (1 <<  6),
    IRQCHIP_SUPPORTS_LEVEL_MSI  = (1 <<  7),
    IRQCHIP_SUPPORTS_NMI        = (1 <<  8),
};

struct irq_chip
{
    const char  *name;
    void (*irq_mask)(struct irq_data *data);
    void (*irq_mask_ack)(struct irq_data *data);
    void (*irq_unmask)(struct irq_data *data);
    void (*irq_eoi)(struct irq_data *data);

    unsigned long   flags;
};

#define in_interrupt(...)       hal_interrupt_get_nest()

#endif /* _IRQ_INTERNAL_H */

