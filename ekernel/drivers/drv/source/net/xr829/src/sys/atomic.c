#include "rthw.h"

#include "compiler.h"

#include "atomic.h"

__nonxip_text
int atomic_add_return(int i, atomic_t *v)
{
	unsigned long flags;
	int val;

	flags = rt_hw_interrupt_disable();
	val = v->counter;
	v->counter = val += i;
	rt_hw_interrupt_enable(flags);

	return val;
}

__nonxip_text
int atomic_sub_return(int i, atomic_t *v)
{
	unsigned long flags;
	int val;

	flags = rt_hw_interrupt_disable();
	val = v->counter;
	v->counter = val -= i;
	rt_hw_interrupt_enable(flags);

	return val;
}

__nonxip_text
int atomic_cmpxchg(atomic_t *v, int old, int new_v)
{
	int ret;
	unsigned long flags;

	flags = rt_hw_interrupt_disable();
	ret = v->counter;
	if (ret == old)
		v->counter = new_v;
	rt_hw_interrupt_enable(flags);

	return ret;
}

__nonxip_text
void atomic_clear_mask(unsigned long mask, unsigned long *addr)
{
	unsigned long flags;

	flags = rt_hw_interrupt_disable();
	*addr &= ~mask;
	rt_hw_interrupt_enable(flags);
}
