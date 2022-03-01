/* Atomic operations usable in machine independent code */
#ifndef _LINUX_ATOMIC_H
#define _LINUX_ATOMIC_H

/**
 * atomic_read - read atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically reads the value of @v.
 */
#define atomic_read(v)	((v)->counter)

/**
 * atomic_set - set atomic variable
 * @v: pointer of type atomic_t
 * @i: required value
 *
 * Atomically sets the value of @v to @i.
 */
#define atomic_set(v, i) ((v)->counter = i)

#define atomic_inc(v)  ((v)->counter++)

#define atomic_dec(v)  ((v)->counter--)

#define atomic_inc_return(v) ({ \
		(v)->counter++; \
		(v)->counter; \
		})

static inline int atomic_add_return(int i, atomic_t *v)
{
    unsigned long flags;
    int val;

    val = v->counter;
    v->counter = val += i;

    return val;
}

static inline int atomic_sub_return(int i, atomic_t *v)
{
    unsigned long flags;
    int val;

    val = v->counter;
    v->counter = val -= i;

    return val;
}

#define atomic_inc_and_test(v)  (atomic_add_return(1, v) == 0)
#define atomic_dec_and_test(v)  (atomic_sub_return(1, v) == 0)

#endif /* _LINUX_ATOMIC_H */
