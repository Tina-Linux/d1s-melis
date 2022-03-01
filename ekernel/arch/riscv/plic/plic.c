#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <excep.h>
#include <irqflags.h>
#include <debug.h>
#include <csr.h>
#include <io.h>
#include <log.h>
#include "irq_internal.h"
#include <interrupt.h>

#define C906_PLIC_PHY_ADDR              (0x10000000)
#define C906_PLIC_NR_EXT_IRQS           (NR_IRQS)
#define C906_NR_CPUS                    (NR_CPUS)

/* M and S mode context. */
#define C906_NR_CONTEXT                 (2)

#define MAX_DEVICES                     1024
#define MAX_CONTEXTS                    15872

/*
 *  Each interrupt source has a priority register associated with it.
 *  We always hardwire it to one in Linux.
 */
#define PRIORITY_BASE                   0
#define PRIORITY_PER_ID                 4

/*
 *  Each hart context has a vector of interrupt enable bits associated with it.
 *  There's one bit for each interrupt source.
 */
#define ENABLE_BASE                     0x2000
#define ENABLE_PER_HART                 0x80

/*
 *  Each hart context has a set of control registers associated with it.  Right
 *  now there's only two: a source priority threshold over which the hart will
 *  take an interrupt, and a register to claim interrupts.
 */
#define CONTEXT_BASE                    0x200000
#define CONTEXT_PER_HART                0x1000
#define CONTEXT_THRESHOLD               0x00
#define CONTEXT_CLAIM                   0x04

static void *c906_plic_regs = NULL;

struct plic_handler
{
    bool   present;
    void  *hart_base;
    void  *enable_base;
};

static inline void plic_toggle(struct plic_handler *handler, int hwirq, int enable);
struct plic_handler c906_plic_handlers[C906_NR_CPUS];

static inline void plic_irq_toggle(int hwirq, int enable)
{
    int cpu = 0;

    /* set priority of interrupt, interrupt 0 is zero. */
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

/*
 * Handling an interrupt is a two-step process: first you claim the interrupt
 * by reading the claim register, then you complete the interrupt by writing
 * that source ID back to the same claim register.  This automatically enables
 * and disables the interrupt, so there's nothing else to do.
 */
void plic_handle_irq(irq_regs_t *regs)
{
    int cpu = 0;
    unsigned int irq;

    struct plic_handler *handler = &c906_plic_handlers[cpu];
    void *claim = handler->hart_base + CONTEXT_CLAIM;

    if (c906_plic_regs == NULL || !handler->present)
    {
        __err("plic state not initialized.");
        software_break();
        return;
    }

    csr_clear(sie, SIE_SEIE);

    while ((irq = readl(claim)))
    {
        /* ID0 is diabled permantually from spec. */
        if (irq == 0)
        {
            __err("irq no is zero.");
            software_break();
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
        software_break();
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
