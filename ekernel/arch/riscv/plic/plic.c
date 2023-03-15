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
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <excep.h>
#include <irqflags.h>
#include <hal_debug.h>
#include <csr.h>
#include <io.h>
#include <log.h>
#include "irq_internal.h"
#include <interrupt.h>

#define C906_PLIC_PHY_ADDR              (0x10000000)
#define NR_IRQS       (207)
#define C906_PLIC_NR_EXT_IRQS           (NR_IRQS)
#define C906_NR_CPUS                    (1)

#define C906_NR_CONTEXT                 (2)

#define MAX_DEVICES                     1024
#define MAX_CONTEXTS                    15872

#define PRIORITY_BASE                   0
#define PRIORITY_PER_ID                 4

#define ENABLE_BASE                     0x2000
#define ENABLE_PER_HART                 0x80

#define CONTEXT_BASE                    0x200000
#define CONTEXT_PER_HART                0x1000
#define CONTEXT_THRESHOLD               0x00
#define CONTEXT_CLAIM                   0x04


struct plic_handler
{
    bool   present;
    void  *hart_base;
    void  *enable_base;
};

static inline void plic_toggle(struct plic_handler *handler, int hwirq, int enable);

struct plic_handler c906_plic_handlers[C906_NR_CPUS];

static void *c906_plic_regs = NULL;

static inline void plic_irq_toggle(int hwirq, int enable)
{
    int cpu = 0;

    writel(enable, c906_plic_regs + PRIORITY_BASE + hwirq * PRIORITY_PER_ID);
    struct plic_handler *handler = &c906_plic_handlers[cpu];

    if (handler->present)
    {
        plic_toggle(handler, hwirq, enable);
    }
}

static void plic_irq_mask(struct irq_data *d)
{
    plic_irq_toggle(d->irq, 0);
    return;
}

static void plic_irq_unmask(struct irq_data *d)
{
    plic_irq_toggle(d->irq, 1);
    return;
}

static void plic_irq_eoi(struct irq_data *d)
{
    int cpu = 0;
    struct plic_handler *handler = &c906_plic_handlers[cpu];

    writel(d->irq, handler->hart_base + CONTEXT_CLAIM);
    return;
}

struct irq_chip plic_chip =
{
    .name           = "SiFive PLIC",
    .irq_mask       = plic_irq_mask,
    .irq_mask_ack   = NULL,
    .irq_unmask     = plic_irq_unmask,
    .irq_eoi        = plic_irq_eoi,
};

int irq_disable(unsigned int irq_no)
{
    plic_irq_toggle(irq_no, 0);
    return 0;
}

int irq_enable(unsigned int irq_no)
{
    plic_irq_toggle(irq_no, 1);
    return 0;
}

void plic_handle_irq(irq_regs_t *regs)
{
    int cpu = 0;
    unsigned int irq;

    struct plic_handler *handler = &c906_plic_handlers[cpu];
    void *claim = handler->hart_base + CONTEXT_CLAIM;

    if (c906_plic_regs == NULL || !handler->present)
    {
        __err("plic state not initialized.");
        hal_sys_abort();
        return;
    }

    csr_clear(sie, SIE_SEIE);

    while ((irq = readl(claim)))
    {
        /* ID0 is diabled permantually from spec. */
        if (irq == 0)
        {
            __err("irq no is zero.");
            hal_sys_abort();
        }
        else
        {
            generic_handle_irq(irq);
        }
    }
    csr_set(sie, SIE_SEIE);
    return;
}

static inline void plic_toggle(struct plic_handler *handler, int hwirq, int enable)
{
    uint32_t  *reg = handler->enable_base + (hwirq / 32) * sizeof(uint32_t);
    uint32_t hwirq_mask = 1 << (hwirq % 32);

    if (enable)
    {
        writel(readl(reg) | hwirq_mask, reg);
    }
    else
    {
        writel(readl(reg) & ~hwirq_mask, reg);
    }
}

void plic_init(void)
{
    int nr_irqs;
    int nr_context;
    int i;
    unsigned long hwirq;
    int cpu = 0;

    if (c906_plic_regs)
    {
        __err("plic already initialized!");
        return;
    }

    nr_context = C906_NR_CONTEXT;

    c906_plic_regs = (void *)C906_PLIC_PHY_ADDR;
    if (!c906_plic_regs)
    {
        __err("fatal error, plic is reg space is null.");
        hal_sys_abort();
        return;
    }

    nr_irqs = C906_PLIC_NR_EXT_IRQS;

    for (i = 0; i < nr_context; i ++)
    {
        struct plic_handler *handler;
        uint32_t threshold = 0;

        cpu = 0;

        /* skip contexts other than supervisor external interrupt */
        if (i == 0)
        {
            continue;
        }

        // we always use CPU0 M-mode target register.
        handler = &c906_plic_handlers[cpu];
        if (handler->present)
        {
            threshold  = 0xffffffff;
            goto done;
        }

        handler->present = true;
        handler->hart_base = c906_plic_regs + CONTEXT_BASE + i * CONTEXT_PER_HART;
        handler->enable_base = c906_plic_regs + ENABLE_BASE + i * ENABLE_PER_HART;

done:
        /* priority must be > threshold to trigger an interrupt */
        writel(threshold, handler->hart_base + CONTEXT_THRESHOLD);
        for (hwirq = 1; hwirq <= nr_irqs; hwirq++)
        {
            plic_toggle(handler, hwirq, 0);
        }
    }

    return;
}
