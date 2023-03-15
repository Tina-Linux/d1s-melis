/*
 * irqflags.h
 *
 * IRQ flags tracing: follow the state of the hardirq and softirq flags and
 * provide callbacks for transitions between ON and OFF states.
 *
 * This file gets included from lowlevel asm headers too, to provide
 * wrapped versions of the local_irq_*() APIs, based on the
 * raw_local_irq_*() macros from the lowlevel headers.
 */
#ifndef _LINUX_TRACE_IRQFLAGS_H
#define _LINUX_TRACE_IRQFLAGS_H

#ifdef __cplusplus
extern "C" {
#endif
#include <os/include/sys/interrupt.h>

#define raw_local_irq_save(flags)	\
	do {				\
		flags = xr_irq_save();	\
	} while (0)

#define raw_local_irq_restore(flags)	\
	do {				\
		xr_irq_restore(flags);	\
	} while (0)

#define local_irq_save(flags)		raw_local_irq_save(flags)
#define local_irq_restore(flags)	raw_local_irq_restore(flags)

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_TRACE_IRQFLAGS_SUPPORT */
