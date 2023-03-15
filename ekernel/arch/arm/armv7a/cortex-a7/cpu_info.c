/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <const.h>

#define BITREADER(v, o, l) (((v) >> (o)) & ((1 << (l)) - 1))

enum CACHE_TYPE
{
    DCACHE_L1 = 0,
    DCACHE_L2,
    ICACHE_L1,
    ICACHE_L2,
    CACHE_TYPE_NUM,
};

static inline uint32_t read_cache_type(void)
{
    uint32_t retval;

    /* Read Cache Type Register */
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

    /* Read Cache Level ID Register */
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

    /* Read Cache Type Register */
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

static const char *crbits[] = { "M", "A", "C", "", "", "", "", "", "", "",
                                "SW", "Z", "I", "V", "", "", "", "", "",
                                "WXN", "UWXN", "", "", "", "", "EE", "",
                                "", "TRE", "AFE", "TE", ""
                              };

void print_cpu_info(void)
{
    uint32_t cr;
    uint32_t mainid;
    int32_t  i;
    int32_t  cpu_arch;
    char    *isa_arch, *vendor;

    decode_cache_info();

    __asm__ __volatile__(
        "mrc p15, 0, %0, c1, c0, 0\n"
        : "=r"(cr) : : "memory");

    __asm__ __volatile__(
        "mrc p15, 0, %0, c0, c0, 0\n"
        : "=r"(mainid) : : "memory");

    printf("Control register: ");
    for (i = 0; i < sizeof(crbits) / sizeof(crbits[0]); i++)
    {
        if (cr & (1 << i))
        {
            printf("%s ", crbits[i]);
        }
    }

    printf("\n\r");
    switch (mainid >> 24)
    {
        case 0x41:
            vendor = "ARM";
            break;
        case 0x44:
            vendor = "Digital Equipment Corp.";
            break;
        case 0x40:
            vendor = "Motorola - Freescale Semiconductor Inc.";
            break;
        case 0x56:
            vendor = "Marvell Semiconductor Inc.";
            break;
        case 0x69:
            vendor = "Intel Corp.";
            break;
        default:
            vendor = "Unknown";
    }

    if ((mainid & 0x0008f000) == 0)
    {
        cpu_arch = CPU_ARCH_UNKNOWN;
    }
    else if ((mainid & 0x0008f000) == 0x00007000)
    {
        cpu_arch = (mainid & (1 << 23)) ? CPU_ARCH_ARMv4T : CPU_ARCH_ARMv3;
    }
    else if ((mainid & 0x00080000) == 0x00000000)
    {
        cpu_arch = (mainid >> 16) & 7;
        if (cpu_arch)
        {
            cpu_arch += CPU_ARCH_ARMv3;
        }
    }
    else if ((mainid & 0x000f0000) == 0x000f0000)
    {
        uint32_t mmfr0;

        /*
        ¦* Revised CPUID format. Read the Memory Model Feature
        ¦* Register 0 and check for VMSAv7 or PMSAv7
        ¦*/
        asm("mrc p15, 0, %0, c0, c1, 4" : "=r"(mmfr0));
        if ((mmfr0 & 0x0000000f) >= 0x00000003 || (mmfr0 & 0x000000f0) >= 0x00000030)
        {
            cpu_arch = CPU_ARCH_ARMv7;
        }
        else if ((mmfr0 & 0x0000000f) == 0x00000002 || (mmfr0 & 0x000000f0) == 0x00000020)
        {
            cpu_arch = CPU_ARCH_ARMv6;
        }
        else
        {
            cpu_arch = CPU_ARCH_UNKNOWN;
        }
    }
    else
    {
        cpu_arch = CPU_ARCH_UNKNOWN;
    }

    switch (cpu_arch)
    {
        case CPU_ARCH_ARMv3:
            isa_arch = "v3";
            break;
        case CPU_ARCH_ARMv4:
            isa_arch = "v4";
            break;
        case CPU_ARCH_ARMv4T:
            isa_arch = "v4T";
            break;
        case CPU_ARCH_ARMv5:
            isa_arch = "v5";
            break;
        case CPU_ARCH_ARMv5T:
            isa_arch = "v5T";
            break;
        case CPU_ARCH_ARMv5TE:
            isa_arch = "v5TE";
            break;
        case CPU_ARCH_ARMv5TEJ:
            isa_arch = "v5TEJ";
            break;
        case CPU_ARCH_ARMv6:
            isa_arch = "v6";
            break;
        case CPU_ARCH_ARMv7:
            isa_arch = "v7";
            break;
        case CPU_ARCH_UNKNOWN:
        default:
            isa_arch = "Unknown";
    }

    printf("vendor: %s\r\nISA-arch: %s\r\n", vendor, isa_arch);

    if (cpu_arch == CPU_ARCH_ARMv7)
    {
        int32_t major, minor;
        char *part;
        major = (mainid >> 20) & 0xf;
        minor = mainid & 0xf;
        switch (mainid & 0xfff0)
        {
            case ARM_CPU_PART_CORTEX_A5:
                part = "Cortex-A5";
                break;
            case ARM_CPU_PART_CORTEX_A7:
                part = "Cortex-A7";
                break;
            case ARM_CPU_PART_CORTEX_A8:
                part = "Cortex-A8";
                break;
            case ARM_CPU_PART_CORTEX_A9:
                part = "Cortex-A9";
                break;
            case ARM_CPU_PART_CORTEX_A15:
                part = "Cortex-A15";
                break;
            default:
                part = "unknown";
        }
        printf("core: %s r%ldp%ld\r\n", part, major, minor);
    }
    printf("\n\r");
}
