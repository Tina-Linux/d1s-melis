/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_IRQDESC_H
#define _LINUX_IRQDESC_H

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "irq_internal.h"

/*
 * Core internal functions to deal with irq descriptors
 */

//struct irq_affinity_notify;
//struct proc_dir_entry;
//struct module;
struct irq_desc;
//struct irq_domain;
//struct pt_regs;

/**
 * struct irq_desc - interrupt descriptor
 * @irq_common_data:    per irq and chip data passed down to chip functions
 * @kstat_irqs:     irq stats per cpu
 * @handle_irq:     highlevel irq-events handler
 * @preflow_handler:    handler called before the flow handler (currently used by sparc)
 * @action:     the irq action chain
 * @status:     status information
 * @core_internal_state__do_not_mess_with_it: core internal status information
 * @depth:      disable-depth, for nested irq_disable() calls
 * @wake_depth:     enable depth, for multiple irq_set_irq_wake() callers
 * @tot_count:      stats field for non-percpu irqs
 * @irq_count:      stats field to detect stalled irqs
 * @last_unhandled: aging timer for unhandled count
 * @irqs_unhandled: stats field for spurious unhandled interrupts
 * @threads_handled:    stats field for deferred spurious detection of threaded handlers
 * @threads_handled_last: comparator field for deferred spurious detection of theraded handlers
 * @lock:       locking for SMP
 * @affinity_hint:  hint to user space for preferred irq affinity
 * @affinity_notify:    context for notification of affinity changes
 * @pending_mask:   pending rebalanced interrupts
 * @threads_oneshot:    bitfield to handle shared oneshot threads
 * @threads_active: number of irqaction threads currently running
 * @wait_for_threads:   wait queue for sync_irq to wait for threaded handlers
 * @nr_actions:     number of installed actions on this descriptor
 * @no_suspend_depth:   number of irqactions on a irq descriptor with
 *          IRQF_NO_SUSPEND set
 * @force_resume_depth: number of irqactions on a irq descriptor with
 *          IRQF_FORCE_RESUME set
 * @rcu:        rcu head for delayed free
 * @kobj:       kobject used to represent this struct in sysfs
 * @request_mutex:  mutex to protect request/free before locking desc->lock
 * @dir:        /proc/irq/ procfs entry
 * @debugfs_file:   dentry for the debugfs file
 * @name:       flow handler name for /proc/interrupts output
 */
struct irq_desc
{
    struct irq_data     irq_data;
    irq_flow_handler_t  handle_irq;
    struct irqaction    *action;    /* IRQ action list */
    unsigned int        number;     /* nested irq disables */
    unsigned int        nr_actions;
    const char          *dev_name;
    const char          *name;
} ____cacheline_internodealigned_in_smp;

#ifdef CONFIG_SPARSE_IRQ
extern void irq_lock_sparse(void);
extern void irq_unlock_sparse(void);
#else
#endif

/*
 * Architectures call this to let the generic IRQ layer
 * handle an interrupt.
 */
static inline void generic_handle_irq_desc(struct irq_desc *desc)
{
    desc->handle_irq(desc);
}

int generic_handle_irq(unsigned int irq);

/* Test to see if a driver has successfully requested an irq */
static inline int irq_desc_has_action(struct irq_desc *desc)
{
    return desc->action != NULL;
}

#define for_each_action_of_desc(desc, act)                      \
    for (act = desc->action; act; act = act->next)


#endif
