#ifndef _IRQDESC_H
#define _IRQDESC_H

#include <stdio.h>
#include <stdint.h>
#include "irq_internal.h"


struct irq_desc
{
    struct irq_data     irq_data;
    irq_flow_handler_t  handle_irq;
    struct irqaction    *action;    /* IRQ action list */
    unsigned int        number;     /* nested irq disables */
    unsigned int        nr_actions;
    const char          *dev_name;
    const char          *name;
};

/*
 * Architectures call this to let the generic IRQ layer
 * handle an interrupt.
 */
static inline void generic_handle_irq_desc(struct irq_desc *desc)
{
    desc->handle_irq(desc);
}

int generic_handle_irq(unsigned int irq);

#define for_each_action_of_desc(desc, act)                      \
    for (act = desc->action; act; act = act->next)

#endif
