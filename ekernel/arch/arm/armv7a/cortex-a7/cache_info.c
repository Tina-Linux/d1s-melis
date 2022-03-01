/*
 * ===========================================================================================
 *
 *       Filename:  cache_info.c
 *
 *    Description:  cache information for cortex-a7.
 *
 *        Version:  Melis3.0
 *         Create:  2019-12-09 11:27:48
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-12-09 14:47:16
 *
 * ===========================================================================================
 */
#include <rtthread.h>
#include <version.h>
#include <kconfig.h>
#include <debug.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arch.h>
#include <const.h>
#include <log.h>

#define BITREADER(v, o, l) (((v) >> (o)) & ((1 << (l)) - 1))

enum CACHE_TYPE
{
    DCACHE_L1 = 0,
    DCACHE_L2,
    ICACHE_L1,
    ICACHE_L2,
    __CACHE_ALL_TYPE,
};

static inline uint32_t read_cache_type(void)
{
    uint32_t retval;

    // Read Cache Type Register
    __asm__ __volatile__("mrc p15, 0, %0, c0, c0, 1\n" : "=r"(retval) :: "memory");

    return retval;
}

static inline uint32_t read_cache_cfg(enum CACHE_TYPE etype)
{
    uint32_t cache_size_sel_reg;
    uint32_t retval;

    switch (etype)
    {
        case DCACHE_L1:
            cache_size_sel_reg = 0;
            break;
        case DCACHE_L2:
            cache_size_sel_reg = 2;
            break;
        case ICACHE_L1:
            cache_size_sel_reg = 1;
            break;
        case ICACHE_L2:
            cache_size_sel_reg = 3;
            break;
        default:
            cache_size_sel_reg = 0;
            break;
    }

    __asm__ __volatile__("mcr p15, 2, %0, c0, c0, 0\n" : :"r"(cache_size_sel_reg) : "memory");
    __asm__ __volatile__("mrc p15, 1, %0, c0, c0, 0\n" : "=r"(retval) : : "memory");

    return retval;
}

static inline void decode_ccssidr(char *cache, uint32_t reg)
{
    int32_t linesize = BITREADER(reg, 0, 3);
    int32_t associat = BITREADER(reg, 3, 10);
    int32_t numsets  = BITREADER(reg, 13, 15);
    int32_t wa = BITREADER(reg, 28, 1);
    int32_t ra = BITREADER(reg, 29, 1);
    int32_t wb = BITREADER(reg, 30, 1);
    int32_t wt = BITREADER(reg, 31, 1);

    linesize  = (1 << (linesize + 2)) * 4;
    numsets  += 1;
    associat += 1;

    int32_t cache_size = (numsets) * (associat) * linesize;
    cache_size /= 1024;

    printf("%s size %ld KB:\n", cache, cache_size);
    printf("\tlinesize %ld bytes\n", linesize);
    printf("\tassociaty %ld\n", associat);
    printf("\tnumbsets %ld\n", numsets);
    printf("\twrite allocation %s\n", wa ? "support" : "not support");
    printf("\tread allocation %s\n", ra ? "support" : "not support");
    printf("\twrite back %s\n", wb ? "support" : "not support");
    printf("\twrite through %s\n", wt ? "support" : "not support");
}

static inline void decode_clidr_reg(void)
{
    uint32_t clidr;

    // Read Cache Level ID Register
    __asm__ __volatile__("mrc p15, 1, %0, c0, c0, 1\n" : "=r"(clidr) :: "memory");

    int32_t ctype1 = BITREADER(clidr, 0, 3);
    int32_t ctype2 = BITREADER(clidr, 3, 3);
    int32_t louis  = BITREADER(clidr, 21, 3);
    int32_t loc    = BITREADER(clidr, 24, 3);
    int32_t louu   = BITREADER(clidr, 27, 3);

    printf("cache-attribute:\n");
    printf("\t%s\n", (ctype1 == 0x3) ? "L1 Separate instruction and data caches" : "L1 Unify caches.");
    printf("\t%s\n", (ctype2 == 0x00) ? "L2 cache not implemented on the processor" : ((ctype2 == 0x4) ? \
            "L2 Unified instruction and data caches" : "L2 Separate I/D L2 cache"));
    printf("\t%s\n", (louis == 0x1) ? "L2 Unification Inner Shareable for the cache hierarchy is L2 cache." : "L2 null");
    printf("\t%s\n", (loc == 0x1) ? "L2 cache not implemented on the processor" : ((ctype2 == 0x2) ? \
            "L2 cache coherency" : "L2 null"));
    printf("\t%s\n", (louu == 0x1) ? "L2 the Level of Unification Uniprocessor for the cache hierarchy is L2 cache." : "L2 null");
}

void decode_cache_info(void)
{
    uint32_t cache_type_reg;
    uint32_t l1_dcache_info;
    uint32_t l2_dcache_info;
    uint32_t l1_icache_info;
    uint32_t l2_icache_info;

    // Read Cache Type Register
    cache_type_reg = read_cache_type();
    l1_dcache_info = read_cache_cfg(DCACHE_L1);
    l2_dcache_info = read_cache_cfg(DCACHE_L2);
    l1_icache_info = read_cache_cfg(ICACHE_L1);
    l2_icache_info = read_cache_cfg(ICACHE_L2);

    int32_t format = BITREADER(cache_type_reg, 29, 3);
    int32_t cwg = BITREADER(cache_type_reg, 24, 4);
    int32_t erg = BITREADER(cache_type_reg, 20, 4);
    int32_t dminline = BITREADER(cache_type_reg, 16, 4);
    int32_t l1ip = BITREADER(cache_type_reg, 14, 2);
    int32_t iminline = BITREADER(cache_type_reg, 0, 4);

    printf("Cache-info:\n");
    printf("\tilinesize %d bytes\n", (1 << iminline) * 4);
    printf("\tdlinesize %d bytes\n", (1 << dminline) * 4);
    printf("\tl1-icache policy %s.\n", (l1ip == 2) ? "vipt" : "not vipt");
    printf("\tcwg granule %d.\n", (1 << cwg) * 4);
    printf("\terg granule %d.\n", (1 << erg) * 4);
    printf("\tformat %s.\n", (format == 0x4) ? "ARMv7" : "Not ARMv7");

    decode_ccssidr("L1_dcache_info", l1_dcache_info);
    decode_ccssidr("L1_icache_info", l1_icache_info);
    decode_ccssidr("L2_dcache_info", l2_dcache_info);
    decode_ccssidr("L2_icache_info", l2_icache_info);

    decode_clidr_reg();

    return;
}
