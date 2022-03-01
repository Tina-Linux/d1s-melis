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
