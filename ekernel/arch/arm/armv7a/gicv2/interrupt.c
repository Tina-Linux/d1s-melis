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
#include <stdlib.h>
#include "interrupt.h"
#include "irqdesc.h"
#include "irq_internal.h"
#include "gicv2_reg.h"
#include <barrier.h>
#include <excep.h>
#include <log.h>
#include <arch.h>
#include <sunxi_hal_common.h>

#include <hal_interrupt.h>
#include <hal_mem.h>

#define GICC_IAR_INT_ID_MASK    (0x3ff)
#define NR_IRQS (256)

/*
 * Generic no controller implementation
 */
struct irq_chip no_irq_chip =
{
    .name           = "none",
};

static void handle_level_irq(struct irq_desc *desc);
struct irq_desc irq_desc[NR_IRQS]  =
{
    [0 ... NR_IRQS - 1] = {
        .handle_irq     = handle_level_irq,
        .action         = NULL,
        .number         = 0,
        .nr_actions     = 0,
        .dev_name       = '\0',
        .name           = '\0',
    }
};

void handle_level_irq(struct irq_desc *desc)
{
    unsigned int irq = desc->irq_data.irq;
    struct irqaction *action;
    hal_irqreturn_t res;

    action = desc->action;
    res = action->handler(action->dev_id);
    action->irq_nums ++;

    switch (res)
    {
        case HAL_IRQ_OK:
        /* Fall through - to add to randomness */
        case HAL_IRQ_ERR:
            break;
        default:
            break;
    }
}

int32_t interrupt_init(void)
{
    int irqno = 0;

    for (irqno = 0; irqno < NR_IRQS; irqno ++)
    {
        irq_desc[irqno].irq_data.mask       = 0xffffffff;
        irq_desc[irqno].irq_data.irq        = irqno;
        irq_desc[irqno].irq_data.chip       = &no_irq_chip;
        irq_desc[irqno].irq_data.chip_data  = NULL;
    }

    return 0;
}

struct irq_desc *irq_to_desc(unsigned int irq)
{
    extern uint32_t arm_gic_nlines(void);
    return ((irq < NR_IRQS) && (irq < arm_gic_nlines())) ? irq_desc + irq : NULL;
}

static int __setup_irq(unsigned int irq, struct irq_desc *desc, struct irqaction *new)
{
    struct irqaction *old, **old_ptr;
    unsigned long flags, thread_mask = 0;
    int ret, nested;

    if (!desc)
    {
        __err("no desc for this irq %d.", irq);
        return -1;
    }

    if (desc->irq_data.chip == NULL)
    {
        __err("irq chip is null.");
        return -1;
    }

    new->irq = irq;
    desc->dev_name = (const char *)&(new->name);
    desc->name = desc->dev_name;
    desc->number = 0;

    /*
     * Drivers are often written to work w/o knowledge about the
     * underlying irq chip implementation, so a request for a
     * threaded irq without a primary hard irq context handler
     * requires the ONESHOT flag to be set. Some irq chips like
     * MSI based interrupts are per se one shot safe. Check the
     * chip flags, so we can avoid the unmask dance at the end of
     * the threaded handler for those.
     */
    if (desc->irq_data.chip->flags & IRQCHIP_ONESHOT_SAFE)
    {
        new->flags &= ~IRQF_ONESHOT;
    }

    if (desc->action != NULL) {
        if (desc->action->handler != new->handler) {
            __err("irq has been bound\n \
                \t\ttry to bind %s to irq %d \n \
                irq already bind to %s func:%p.",
                new->name, irq, desc->name,
                desc->action->handler);
                return -1;
        } else {
            __wrn("repeat binding irq %d", irq);
            free(desc->action);
            desc->action = NULL;
        }
    }

    /*
     * The following block of code has to be executed atomically
     * protected against a concurrent interrupt and any of the other
     * management calls which are not serialized via
     * desc->request_mutex or the optional bus lock.
     */
    flags = hal_interrupt_save();

    desc->action = new;
    void enable_irq(unsigned int irq);
    enable_irq(irq);

    hal_interrupt_restore(flags);

    return 0;
}

int request_threaded_irq(unsigned int irq, hal_irq_handler_t handler,
                         hal_irq_handler_t thread_fn, unsigned long irqflags,
                         const char *devname, void *dev_id)
{
    struct irqaction *action;
    struct irq_desc *desc;
    int retval;

    if (irq == IRQ_NOTCONNECTED)
    {
        __err("irq invalided!");
        return -1;
    }

    /*
    ¦* Sanity-check: shared interrupts must pass in a real dev-ID,
    ¦* otherwise we'll have trouble later trying to figure out
    ¦* which interrupt is which (messes up the interrupt freeing
    ¦* logic etc).
    ¦*
    ¦* Also IRQF_COND_SUSPEND only makes sense for shared interrupts and
    ¦* it cannot be set along with IRQF_NO_SUSPEND.
    ¦*/
    if (((irqflags & IRQF_SHARED) && !dev_id) ||
        (!(irqflags & IRQF_SHARED) && (irqflags & IRQF_COND_SUSPEND)) ||
        ((irqflags & IRQF_NO_SUSPEND) && (irqflags & IRQF_COND_SUSPEND)))
    {
        __err("parameter invalided!");
        return -1;
    }

    desc = irq_to_desc(irq);
    if (!desc)
    {
        __err("no desc for this irq %d.!", irq);
        return -1;
    }

    if (!handler)
    {
        __err("handler parameter is null");
        return -1;
    }

    action = hal_malloc(sizeof(struct irqaction));
    if (!action)
    {
        __err("no free buffer.");
        return -1;
    }

    action->handler = handler;
    action->flags = irqflags;
    if (devname)
    {
       strncpy((char *)action->name, devname, IRQACTION_NAME_MAX - 1);
    }
    action->dev_id = dev_id;
    action->irq_nums = 0;

    retval = __setup_irq(irq, desc, action);
    if (retval)
    {
        hal_free(action);
        action = NULL;
    }

    return retval;
}

int generic_handle_irq(unsigned int irq)
{
    struct irq_desc *desc = irq_to_desc(irq);
    if (!desc)
    {
        return -1;
    }
    generic_handle_irq_desc(desc);
    return 0;
}


static void ack_bad_irq(unsigned int irq)
{
    __err("PANIC: %s line %d, fata error.", __func__, __LINE__);
    while (1);
}

int __handle_domain_irq(void *domain, unsigned int hwirq,
                        bool lookup, irq_regs_t *regs)
{
    int ret = 0;
    unsigned irq = hwirq;
    int32_t nr_irqs = NR_IRQS;
    /*
    ¦* Some hardware gives randomly wrong interrupts.  Rather
    ¦* than crashing, do something sensible.
    ¦*/
    if (unlikely(!irq || irq >= nr_irqs))
    {
        ack_bad_irq(irq);
        ret = -1;
    }
    else
    {
        generic_handle_irq(irq);
    }

    return ret;
}

static inline int handle_domain_irq(void *domain, unsigned int hwirq, irq_regs_t *regs)
{
    return __handle_domain_irq(domain, hwirq, true, regs);
}

void gic_handle_irq(irq_regs_t *regs)
{
    uint32_t irqstat, irqnr;

    do
    {
        irqstat = hal_readl(GIC_CPU_AIAR_REG);
        irqnr = irqstat & GICC_IAR_INT_ID_MASK;

        if (likely(irqnr > 15 && irqnr < 1020))
        {
            hal_writel(irqstat, GIC_CPU_AEOI_REG);
            isb();
            handle_domain_irq(NULL, irqnr, regs);
            continue;
        }
        if (irqnr < 16)
        {
            hal_writel(irqstat, GIC_CPU_AEOI_REG);
            hal_writel(irqstat, GIC_CPU_DEACTIVATE);
#ifdef RT_USING_SMP
            /*
            ¦* Ensure any shared data written by the CPU sending
            ¦* the IPI is read after we've read the ACK register
            ¦* on the GIC.
            ¦*
            ¦* Pairs with the write barrier in gic_raise_softirq
            ¦*/
            smp_rmb();
            handle_IPI(irqnr, regs);
#endif
            continue;
        }
        break;
    } while (1);
}

/*
 * Internal function to unregister an irqaction - used to free
 * regular and special interrupts that are part of the architecture.
 */
static struct irqaction *__free_irq(struct irq_desc *desc)
{
    unsigned irq = desc->irq_data.irq;
    struct irqaction *action = desc->action;
    unsigned long flags;

    flags = awos_arch_lock_irq();

    void disable_irq(unsigned int irq);
    disable_irq(irq);
    desc->action = NULL;

    awos_arch_unlock_irq(flags);

    return action;
}

const void *free_irq(int32_t irq)
{
    struct irq_desc *desc = irq_to_desc(irq);
    struct irqaction *action;
    const char *devname;

    if (!desc)
    {
        __err("no desc for irq %d.", irq);
        return NULL;
    }

    action = __free_irq(desc);

    if (!action)
    {
        __err("find no desc for irq %d.", irq);
        return NULL;
    }

    devname = action->name;
    hal_free(action);
    return devname;
}

void remove_irq(unsigned int irq)
{
    struct irq_desc *desc = irq_to_desc(irq);

    if (desc)
    {
        __free_irq(desc);
    }
}

static void __disable_irq(struct irq_desc *desc)
{
    int32_t irq_disable(uint32_t irq_no);

    desc->number--;
    irq_disable(desc->irq_data.irq);
}

static int __disable_irq_nosync(unsigned int irq)
{
    unsigned long flags;

    flags = awos_arch_lock_irq();
    struct irq_desc *desc = irq_to_desc(irq);
    if (!desc)
    {
        awos_arch_unlock_irq(flags);
        return -1;
    }
    __disable_irq(desc);
    awos_arch_unlock_irq(flags);

    return 0;
}

void disable_irq(unsigned int irq)
{
    __disable_irq_nosync(irq);
}

void __enable_irq(struct irq_desc *desc)
{
    int32_t irq_enable(uint32_t irq_no);

    desc->number++;
    irq_enable(desc->irq_data.irq);
}

void enable_irq(unsigned int irq)
{
    unsigned long flags;

    flags = awos_arch_lock_irq();

    struct irq_desc *desc = irq_to_desc(irq);
    if (!desc)
    {
        __err("cant find the desc of irq %d.", irq);
        awos_arch_unlock_irq(flags);
        return;
    }
    __enable_irq(desc);

    awos_arch_unlock_irq(flags);
}

int setup_irq(unsigned int irq, struct irqaction *act)
{
    int retval;
    struct irq_desc *desc = irq_to_desc(irq);

    if (!desc || !act)
    {
        __err("cant find irq descriptor.");
        return -1;
    }

    retval = __setup_irq(irq, desc, act);

    return retval;
}

void irq_suspend(void)
{
	struct irq_desc *desc;
	struct irqaction *action;
	unsigned long l;
	int i;

	l = awos_arch_lock_irq();
	for (i = 0; i < NR_IRQS; i++) {
		desc = &irq_desc[i];

		for_each_action_of_desc(desc, action) {
			if (desc->number && !(action->flags & IRQF_NO_SUSPEND)) {
				int32_t irq_disable(uint32_t irq_no);
				irq_disable(desc->irq_data.irq);
				break;
			}
		}
	}
	awos_arch_unlock_irq(l);
}

void irq_resume(void)
{
	struct irq_desc *desc;
	struct irqaction *action;
	unsigned long l;
	int i;

	l = awos_arch_lock_irq();
	for (i = 0; i < NR_IRQS; i++) {
		desc = &irq_desc[i];

		for_each_action_of_desc(desc, action) {
			if (desc->number && !(action->flags & IRQF_NO_SUSPEND)) {
				int32_t irq_enable(uint32_t irq_no);
				irq_enable(desc->irq_data.irq);
				break;
			}
		}
	}
	awos_arch_unlock_irq(l);
}


void show_irqs(void)
{
    int i;
    struct irq_desc *desc = NULL;
    struct irqaction *action = NULL;;

    printf(" irqno    devname     name     irqname          handler        active.\n");
    printf(" ---------------------------------------------------------------------\n");
    for (i = 0; i < NR_IRQS; i ++)
    {
        int line = 0;
        desc = irq_to_desc(i);
        if (!desc || desc->action == NULL)
        {
            continue;
        }

        printf("%6d%11s%9s", i, desc->dev_name, desc->name);
        for_each_action_of_desc(desc, action)
        {
            if (line !=  0)
            {
                printf("%-*.s", 26, "");
            }
            line ++;
            printf("%*.*s       0x%lx     0x%08lx\n", 12, 10, action->name, (unsigned long)action->handler, action->irq_nums);
        }
    }
    printf(" ---------------------------------------------------------------------\n");
}
