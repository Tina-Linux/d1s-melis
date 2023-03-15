#include "os/include/sys/interrupt.h"
#include "atomic.h"

int atomic_add_return(int i, atomic_t *v)
{
	unsigned long flags;
	int val;

	flags = arch_irq_save();
	val = v->counter;
	v->counter = val += i;
	arch_irq_restore(flags);

	return val;
}

int atomic_sub_return(int i, atomic_t *v)
{
	unsigned long flags;
	int val;

	flags = arch_irq_save();
	val = v->counter;
	v->counter = val -= i;
	arch_irq_restore(flags);

	return val;
}

int atomic_cmpxchg(atomic_t *v, int old, int new_v)
{
	int ret;
	unsigned long flags;

	flags = arch_irq_save();
	ret = v->counter;
	if (ret == old)
		v->counter = new_v;
	arch_irq_restore(flags);

	return ret;
}

void atomic_clear_mask(unsigned long mask, unsigned long *addr)
{
	unsigned long flags;

	flags = arch_irq_save();
	*addr &= ~mask;
	arch_irq_restore(flags);
}
