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

