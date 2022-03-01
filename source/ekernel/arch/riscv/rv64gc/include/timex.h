#ifndef _ASM_RISCV_TIMEX_H
#define _ASM_RISCV_TIMEX_H

#include <stdint.h>
#include <stddef.h>
#include "csr.h"

typedef unsigned long cycles_t;

static inline cycles_t get_cycles(void)
{
    return csr_read(CSR_TIME);
}
#define get_cycles get_cycles

#ifdef CONFIG_64BIT
static inline uint64_t get_cycles64(void)
{
    return get_cycles();
}
#else /* CONFIG_64BIT */
static inline u32 get_cycles_hi(void)
{
    return csr_read(CSR_TIMEH);
}

static inline uint64_t get_cycles64(void)
{
    u32 hi, lo;

    do
    {
        hi = get_cycles_hi();
        lo = get_cycles();
    } while (hi != get_cycles_hi());

    return ((uint64_t)hi << 32) | lo;
}
#endif /* CONFIG_64BIT */

#define ARCH_HAS_READ_CURRENT_TIMER
static inline int read_current_timer(unsigned long *timer_val)
{
    *timer_val = get_cycles();
    return 0;
}

#endif /* _ASM_RISCV_TIMEX_H */
