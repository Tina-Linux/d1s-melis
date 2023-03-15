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
#include <stdlib.h>

#include "interrupt.h"
#include "irqdesc.h"
#include "irq_internal.h"
#include <irqflags.h>

#include <excep.h>
#include <csr.h>
#include <log.h>

#include <hal_debug.h>

#define NR_IRQS       (207)

extern struct irq_chip plic_chip;
static void handle_level_irq(struct irq_desc *desc);

void plic_handle_irq(irq_regs_t *);
void riscv_pmu_handle_irq(void);
void riscv_timer_interrupt(void);

static struct irq_chip no_irq_chip =
{
    .name           = "none",
    .irq_mask       = NULL,
    .irq_mask_ack   = NULL,
    .irq_unmask     = NULL,
    .irq_eoi        = NULL,
};

static struct irq_desc irq_desc[NR_IRQS]  =
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

static void handle_level_irq(struct irq_desc *desc)
{
    struct irqaction *action;
    hal_irqreturn_t res;

    struct irq_chip *chip = desc->irq_data.chip;
    unsigned int irq = desc->irq_data.irq;

    action = desc->action;
    res = action->handler(action->dev_id);
    action->irq_nums ++;

    switch (res)
    {
        case HAL_IRQ_OK:
        case HAL_IRQ_ERR:
            break;
        default:
            break;
    }

    /* ack eoi. */
    chip->irq_eoi(&desc->irq_data);
}

int interrupt_init(void)
{
    int irqno = 0;

    for (irqno = 0; irqno < NR_IRQS; irqno ++)
    {
        irq_desc[irqno].irq_data.mask       = 0xffffffff;
        irq_desc[irqno].irq_data.irq        = irqno;
        irq_desc[irqno].irq_data.chip       = &plic_chip;
        irq_desc[irqno].irq_data.chip_data  = NULL;
    }

    /* Enable all interrupts */
#ifdef CONFIG_RV_MACHINE_MODE
    csr_write(CSR_MIE, -1);
#else
    csr_write(CSR_SIE, -1);
#endif
    return 0;
}

struct irq_desc *irq_to_desc(unsigned int irq)
{
    return (irq < NR_IRQS) ? irq_desc + irq : NULL;
}

/*
 * Internal function to register an irqaction - typically used to
 * allocate special interrupts that are part of the architecture.
 */
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

    new->irq = irq;
    desc->dev_name = (char *)&(new->name);
    desc->name = desc->dev_name;
    desc->number = 0;

    flags = awos_arch_lock_irq();

    desc->action = new;
    void enable_irq(unsigned int irq);
    enable_irq(irq);

    awos_arch_unlock_irq(flags);

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

    action = calloc(sizeof(struct irqaction), 1);
    if (!action)
    {
        __err("no free buffer.");
        return -1;
    }

    action->handler = handler;
    action->flags = irqflags;
    strncpy((char *)action->name, devname, IRQACTION_NAME_MAX - 1);
    action->dev_id = dev_id;
    action->irq_nums = 0;

    retval = __setup_irq(irq, desc, action);
    if (retval)
    {
        free(action);
        action = NULL;
    }

    return retval;
}

int generic_handle_irq(unsigned int irq)
{
    struct irq_desc *desc = irq_to_desc(irq);
    if (!desc)
    {
        __err("fatal error, desc is null for irq %d.", irq);
        hal_sys_abort();
        return -1;
    }
    generic_handle_irq_desc(desc);
    return 0;
}

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
    free(action);
    return devname;
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

void handle_arch_irq(irq_regs_t *regs)
{
    unsigned long sip = csr_read(CSR_SIP);

    if (!(sip & (SIE_STIE | SIE_SEIE)))
    {
        __err("sip status error.");
        hal_sys_abort();
    }

    plic_handle_irq(regs);

    return;
}

unsigned long riscv_cpu_handle_interrupt(unsigned long scause, unsigned long sepc, unsigned long stval, irq_regs_t *regs)
{
    if (!(scause & SCAUSE_IRQ_FLAG))
    {
        __err("fatal error, scause corruption.");
        hal_sys_abort();
        return 1;
    }

    switch (scause & ~SCAUSE_IRQ_FLAG)
    {
        case IRQ_U_SOFT:
        case IRQ_M_SOFT:
        case IRQ_U_TIMER:
        case IRQ_M_TIMER:
        case IRQ_U_EXT:
        case IRQ_M_EXT:
            {
                __err("unexpected interrupt cause 0x%lx", scause);
                hal_sys_abort();
                break;
            }
        case IRQ_S_SOFT:
            {
                __err("i dont know how todo becasue only smp support IPI use this, cause 0x%lx", scause);
                hal_sys_abort();
                break;
            }
        case IRQ_S_PMU:
            break;

        case IRQ_S_TIMER:
            riscv_timer_interrupt();
            break;

        case IRQ_S_EXT:
            handle_arch_irq(regs);
            break;

        default:
            __err("unexpected interrupt cause 0x%lx", scause);
            hal_sys_abort();
            break;
    }

    return 0;
}

unsigned long arch_irq_is_disable(void)
{
    return arch_irqs_disabled_flags(arch_local_save_flags());
}
