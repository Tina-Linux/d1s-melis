/*
 * =====================================================================================
 *
 *       Filename:  c906_cache.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2021年01月22日 11时49分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zeng Zhijin
 *   Organization:
 *
 * =====================================================================================
 */

#define L1_CACHE_BYTES (64)

static void dcache_wb_range(unsigned long start, unsigned long end)
{
    unsigned long i = start & ~(L1_CACHE_BYTES - 1);

    for (; i < end; i += L1_CACHE_BYTES)
    {
        asm volatile("dcache.cva %0\n"::"r"(i):"memory");
    }
    asm volatile(".long 0x01b0000b");
}

static void dcache_inv_range(unsigned long start, unsigned long end)
{
    unsigned long i = start & ~(L1_CACHE_BYTES - 1);

    for (; i < end; i += L1_CACHE_BYTES)
    {
        asm volatile("dcache.iva %0\n"::"r"(i):"memory");
    }
    asm volatile(".long 0x01b0000b");
}

static void dcache_wbinv_range(unsigned long start, unsigned long end)
{
    unsigned long i = start & ~(L1_CACHE_BYTES - 1);

    for (; i < end; i += L1_CACHE_BYTES)
    {
        asm volatile("dcache.civa %0\n"::"r"(i):"memory");
    }
    asm volatile(".long 0x01b0000b");
}

static void icache_inv_range(unsigned long start, unsigned long end)
{
    unsigned long i = start & ~(L1_CACHE_BYTES - 1);

    for (; i < end; i += L1_CACHE_BYTES)
    {
        asm volatile("icache.iva %0\n"::"r"(i):"memory");
    }
    asm volatile(".long 0x01b0000b");
}

void awos_arch_clean_dcache(void)
{
    asm volatile("dcache.call\n":::"memory");
}

void awos_arch_clean_flush_dcache(void)
{
    asm volatile("dcache.ciall\n":::"memory");
}

void awos_arch_flush_dcache(void)
{
    asm volatile("dcache.iall\n":::"memory");
}

void awos_arch_flush_icache(void)
{
    asm volatile("icache.iall\n":::"memory");
}

void awos_arch_mems_flush_icache_region(unsigned long start, unsigned long len)
{
    icache_inv_range(start, start + len);
}

void awos_arch_mems_clean_dcache_region(unsigned long start, unsigned long len)
{
    dcache_wb_range(start, start + len);
}

void awos_arch_mems_clean_flush_dcache_region(unsigned long start, unsigned long len)
{
    dcache_wbinv_range(start, start + len);
}

void awos_arch_mems_flush_dcache_region(unsigned long start, unsigned long len)
{
    dcache_inv_range(start, start + len);
}

void awos_arch_clean_flush_cache(void)
{
    awos_arch_clean_flush_dcache();
    awos_arch_flush_icache();
}

void awos_arch_clean_flush_cache_region(unsigned long start, unsigned long len)
{
    awos_arch_mems_clean_flush_dcache_region(start, len);
    awos_arch_mems_flush_icache_region(start, len);
}

void awos_arch_flush_cache(void)
{
    awos_arch_flush_dcache();
    awos_arch_flush_icache();
}

