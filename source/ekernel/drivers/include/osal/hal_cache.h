#ifndef SUNXI_HAL_CACHE_H
#define SUNXI_HAL_CACHE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>
#ifdef CONFIG_KERNEL_FREERTOS
#ifndef CONFIG_CORE_DSP0
#include <mmu_cache.h>
#include <armv7.h>
#endif
#else
#include <arch.h>
#endif

void hal_dcache_clean(unsigned long vaddr_start, unsigned long size);
void hal_dcache_invalidate(unsigned long vaddr_start, unsigned long size);
void hal_dcache_clean_invalidate(unsigned long vaddr_start, unsigned long size);
void hal_icache_invalidate_all(void);
void hal_dcache_invalidate_all(void);
void hal_dcache_clean_all(void);

#ifdef __cplusplus
}
#endif
#endif
