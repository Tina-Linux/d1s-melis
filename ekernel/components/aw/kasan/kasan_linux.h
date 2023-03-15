#ifndef _LINUX_KASAN_H
#define _LINUX_KASAN_H

#ifdef CONFIG_KASAN

#define VA_BITS 32
#define KASAN_SHADOW_SIZE   (1 << (VA_BITS - 3))

#define KASAN_SHADOW_SCALE_SHIFT (3)

#define VA_START (0x60000000L)

#define KASAN_SHADOW_START      (VA_START)
#define KASAN_SHADOW_END        ((unsigned long)KASAN_SHADOW_START + (unsigned long)KASAN_SHADOW_SIZE)

/*
 * This value is used to map an address to the corresponding shadow
 * address by the following formula:
 *     shadow_addr = (address >> 3) + KASAN_SHADOW_OFFSET;
 *
 * (1 << 61) shadow addresses - [KASAN_SHADOW_OFFSET,KASAN_SHADOW_END]
 * cover all 64-bits of virtual addresses. So KASAN_SHADOW_OFFSET
 * should satisfy the following equation:
 *      KASAN_SHADOW_OFFSET = KASAN_SHADOW_END - (1ULL << 61)
 */
#define KASAN_SHADOW_OFFSET     CONFIG_KASAN_SHADOW_OFFSET

void kasan_init(void);
void kasan_early_init(void);

void kasan_populate_zero_shadow(const void *shadow_start,
                                const void *shadow_end);

static inline void *kasan_mem_to_shadow(const void *addr)
{
    return (void *)((unsigned long)addr >> KASAN_SHADOW_SCALE_SHIFT)
           + KASAN_SHADOW_OFFSET;
}

#else /* CONFIG_KASAN */


#endif /* CONFIG_KASAN */

#endif /* LINUX_KASAN_H */
