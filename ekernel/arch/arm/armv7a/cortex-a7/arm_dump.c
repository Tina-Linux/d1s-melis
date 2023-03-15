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
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <barrier.h>
#include <init.h>
#include <aw_list.h>
#include "page_2nd_level.h"
#include "pgtable-2level-hwdef.h"
#include "pgtable.h"
#include <log.h>

#define DOMAIN_KRNL     0
#define DOMAIN_USER     1
#define DOMAIN_IO       2
#define DOMAIN_VECTORS  3

struct addr_marker
{
    unsigned long start_address;
    char *name;
};

struct ptdump_info
{
    void                            *mm;
    const struct addr_marker        *markers;
    unsigned long                   base_addr;
};

static struct addr_marker address_markers[] =
{
    { 0x00000000,          "Sram" },
    { 0x01900000,          "LDevice" },
    { 0x03000000,          "LDevice" },
    { 0x40000000,          "Physical" },
    { 0xf1900000,          "HDevice" },
    { 0xf3000000,          "HDevice" },
    { 0xf5000000,          "HDevice" },
    { 0xe0000000,          "Modules" },
    { 0xc2000000,          "Kernel Mapping" },
    { -1,                   NULL },
};

struct pg_state
{
    const void *seq;
    const struct addr_marker *marker;
    unsigned long start_address;
    unsigned level;
    uint64_t current_prot;
    bool check_wx;
    unsigned long wx_pages;
    const char *current_domain;
};

struct prot_bits
{
    uint64_t    mask;
    uint64_t    val;
    const char  *set;
    const char  *clear;
    bool        ro_bit;
    bool        nx_bit;
};

static const struct prot_bits pte_bits[] =
{
    {
        .mask   = L_PTE_USER,
        .val    = L_PTE_USER,
        .set    = "USR",
        .clear  = "   ",
    }, {
        .mask   = L_PTE_RDONLY,
        .val    = L_PTE_RDONLY,
        .set    = "ro",
        .clear  = "RW",
        .ro_bit = true,
    }, {
        .mask   = L_PTE_XN,
        .val    = L_PTE_XN,
        .set    = "NX",
        .clear  = "x ",
        .nx_bit = true,
    }, {
        .mask   = L_PTE_SHARED,
        .val    = L_PTE_SHARED,
        .set    = "SHD",
        .clear  = "   ",
    }, {
        .mask   = L_PTE_MT_MASK,
        .val    = L_PTE_MT_UNCACHED,
        .set    = "SO/UNCACHED",
    }, {
        .mask   = L_PTE_MT_MASK,
        .val    = L_PTE_MT_BUFFERABLE,
        .set    = "MEM/BUFFERABLE/WC",
    }, {
        .mask   = L_PTE_MT_MASK,
        .val    = L_PTE_MT_WRITETHROUGH,
        .set    = "MEM/CACHED/WT",
    }, {
        .mask   = L_PTE_MT_MASK,
        .val    = L_PTE_MT_WRITEBACK,
        .set    = "MEM/CACHED/WBRA",
    }, {
        .mask   = L_PTE_MT_MASK,
        .val    = L_PTE_MT_MINICACHE,
        .set    = "MEM/MINICACHE",
    }, {
        .mask   = L_PTE_MT_MASK,
        .val    = L_PTE_MT_WRITEALLOC,
        .set    = "MEM/CACHED/WBWA",
    }, {
        .mask   = L_PTE_MT_MASK,
        .val    = L_PTE_MT_DEV_SHARED,
        .set    = "DEV/SHARED",
    }, {
        .mask   = L_PTE_MT_MASK,
        .val    = L_PTE_MT_DEV_NONSHARED,
        .set    = "DEV/NONSHARED",
    }, {
        .mask   = L_PTE_MT_MASK,
        .val    = L_PTE_MT_DEV_WC,
        .set    = "DEV/WC",
    }, {
        .mask   = L_PTE_MT_MASK,
        .val    = L_PTE_MT_DEV_CACHED,
        .set    = "DEV/CACHED",
    },
};

static const struct prot_bits section_bits[] =
{
    {
        .mask   = PMD_SECT_APX | PMD_SECT_AP_READ | PMD_SECT_AP_WRITE,
        .val    = PMD_SECT_APX | PMD_SECT_AP_WRITE,
        .set    = "    ro",
        .ro_bit = true,
    }, {
        .mask   = PMD_SECT_APX | PMD_SECT_AP_READ | PMD_SECT_AP_WRITE,
        .val    = PMD_SECT_AP_WRITE,
        .set    = "    RW",
    }, {
        .mask   = PMD_SECT_APX | PMD_SECT_AP_READ | PMD_SECT_AP_WRITE,
        .val    = PMD_SECT_AP_READ,
        .set    = "USR ro",
    }, {
        .mask   = PMD_SECT_APX | PMD_SECT_AP_READ | PMD_SECT_AP_WRITE,
        .val    = PMD_SECT_AP_READ | PMD_SECT_AP_WRITE,
        .set    = "USR RW",
    }, {
        .mask   = PMD_SECT_XN,
        .val    = PMD_SECT_XN,
        .set    = "NX",
        .clear  = "x ",
        .nx_bit = true,
    }, {
        .mask   = PMD_SECT_S,
        .val    = PMD_SECT_S,
        .set    = "SHD",
        .clear  = "   ",
    },
};

struct pg_level
{
    const struct prot_bits *bits;
    size_t num;
    uint64_t mask;
    const struct prot_bits *ro_bit;
    const struct prot_bits *nx_bit;
};

static struct pg_level pg_level[] =
{
    {
    }, { /* pgd */
    }, { /* pud */
    }, { /* pmd */
        .bits   = section_bits,
        .num    = ARRAY_SIZE(section_bits),
    }, { /* pte */
        .bits   = pte_bits,
        .num    = ARRAY_SIZE(pte_bits),
    },
};

static void dump_prot(struct pg_state *st, const struct prot_bits *bits, size_t num)
{
    unsigned i;

    for (i = 0; i < num; i++, bits++)
    {
        const char *s;

        if ((st->current_prot & bits->mask) == bits->val)
        {
            s = bits->set;
        }
        else
        {
            s = bits->clear;
        }

        if (s)
        {
            printf("\t %s", s);
        }
    }
}

static void note_prot_wx(struct pg_state *st, unsigned long addr)
{
    if (!st->check_wx)
    {
        return;
    }
    if ((st->current_prot & pg_level[st->level].ro_bit->mask) ==
        pg_level[st->level].ro_bit->val)
    {
        return;
    }
    if ((st->current_prot & pg_level[st->level].nx_bit->mask) ==
        pg_level[st->level].nx_bit->val)
    {
        return;
    }

    WARN_ONCE(1, "arm/mm: Found insecure W+X mapping at address %pS\n",
              (void *)st->start_address);

    st->wx_pages += (addr - st->start_address) / PAGE_SIZE;
}

static void note_page(struct pg_state *st, unsigned long addr,
                      unsigned int level, uint64_t val, const char *domain)
{
    static const char units[] = "KMGTPE";
    uint64_t prot = val & pg_level[level].mask;

    if (!st->level)
    {
        st->level = level;
        st->current_prot = prot;
        st->current_domain = domain;
        printf("\t---[ %s ]---\n", st->marker->name);
    }
    else if (prot != st->current_prot || level != st->level ||
             domain != st->current_domain ||
             addr >= st->marker[1].start_address)
    {
        const char *unit = units;
        unsigned long delta;

        if (st->current_prot)
        {
            note_prot_wx(st, addr);
            printf("\t0x%08lx-0x%08lx   ",
                   st->start_address, addr);

            delta = (addr - st->start_address) >> 10;
            while (!(delta & 1023) && unit[1])
            {
                delta >>= 10;
                unit++;
            }
            printf("\t%9lu%c", delta, *unit);
            if (st->current_domain)
            {
                printf("\t %s", st->current_domain);
            }
            if (pg_level[st->level].bits)
            {
                dump_prot(st, pg_level[st->level].bits, pg_level[st->level].num);
            }
            printf("\n");
        }

        if (addr >= st->marker[1].start_address)
        {
            st->marker++;
            printf("\t---[ %s ]---\n", st->marker->name);
        }
        st->start_address = addr;
        st->current_prot = prot;
        st->current_domain = domain;
        st->level = level;
    }
}

static void walk_pte(struct pg_state *st, pmd_t *pmd, unsigned long start,
                     const char *domain)
{
    pte_t *pte = pte_offset_kernel(pmd, 0);
    unsigned long addr;
    unsigned i;

    for (i = 0; i < PTRS_PER_PTE; i++, pte++)
    {
        addr = start + i * PAGE_SIZE;
        note_page(st, addr, 4, pte_val(*pte), domain);
    }
}

static const char *get_domain_name(pmd_t *pmd)
{
    switch (pmd_val(*pmd) & PMD_DOMAIN_MASK)
    {
        case PMD_DOMAIN(DOMAIN_KRNL):
            return "KERNEL ";
        case PMD_DOMAIN(DOMAIN_USER):
            return "USER   ";
        case PMD_DOMAIN(DOMAIN_IO):
            return "IO     ";
        case PMD_DOMAIN(DOMAIN_VECTORS):
            return "VECTORS";
        default:
            return "modules";
    }
    return NULL;
}

static void walk_pmd(struct pg_state *st, pud_t *pud, unsigned long start)
{
    pmd_t *pmd = pmd_offset(pud, 0);
    unsigned long addr;
    unsigned i;
    const char *domain;

    for (i = 0; i < PTRS_PER_PMD; i++, pmd++)
    {
        addr = start + i * PMD_SIZE;
        domain = get_domain_name(pmd);
        if (pmd_none(*pmd) || pmd_large(*pmd) || !pmd_present(*pmd))
        {
            note_page(st, addr, 3, pmd_val(*pmd), domain);
        }
        else
        {
            walk_pte(st, pmd, addr, domain);
        }

        if (SECTION_SIZE < PMD_SIZE && pmd_large(pmd[1]))
        {
            addr += SECTION_SIZE;
            pmd++;
            domain = get_domain_name(pmd);
            note_page(st, addr, 3, pmd_val(*pmd), domain);
        }
    }
}

static void walk_pud(struct pg_state *st, pgd_t *pgd, unsigned long start)
{
    pud_t *pud = pud_offset(pgd, 0);
    unsigned long addr;
    unsigned i;

    for (i = 0; i < PTRS_PER_PUD; i++, pud++)
    {
        addr = start + i * PUD_SIZE;
        if (!pud_none(*pud))
        {
            walk_pmd(st, pud, addr);
        }
        else
        {
            note_page(st, addr, 2, pud_val(*pud), NULL);
        }
    }
}

static void walk_pgd(struct pg_state *st, const void *mm,
                     unsigned long start)
{
    pgd_t *pgd = pgd_offset(mm, 0UL);
    unsigned i;
    unsigned long addr;

    for (i = 0; i < PTRS_PER_PGD; i++, pgd++)
    {
        addr = start + i * PGDIR_SIZE;
        if (!pgd_none(*pgd))
        {
            walk_pud(st, pgd, addr);
        }
        else
        {
            note_page(st, addr, 1, pgd_val(*pgd), NULL);
        }
    }
}

static void ptdump_initialize(void)
{
    unsigned i, j;

    for (i = 0; i < ARRAY_SIZE(pg_level); i++)
    {
        if (pg_level[i].bits)
        {
            for (j = 0; j < pg_level[i].num; j++)
            {
                pg_level[i].mask |= pg_level[i].bits[j].mask;
                if (pg_level[i].bits[j].ro_bit)
                {
                    pg_level[i].ro_bit = &pg_level[i].bits[j];
                }
                if (pg_level[i].bits[j].nx_bit)
                {
                    pg_level[i].nx_bit = &pg_level[i].bits[j];
                }
            }
        }
    }
}

static struct ptdump_info kernel_ptdump_info =
{
    .mm = NULL,
    .markers = address_markers,
    .base_addr = 0,
};

void ptdump_walk_pgd(const void *m, struct ptdump_info *info)
{
    static bool __inited;

    if (unlikely(!__inited))
    {
        ptdump_initialize();
        __inited = true;
    }

    struct pg_state st =
    {
        .seq = m,
        .marker = address_markers,
        .check_wx = false,
    };

    walk_pgd(&st, NULL, 0x00000000);
    note_page(&st, 0, 0, 0, NULL);
}

void dump_page_table(void)
{
    ptdump_walk_pgd(NULL, NULL);
}

void ptdump_check_wx(void)
{
    struct pg_state st =
    {
        .seq = NULL,
        .marker = (struct addr_marker[])
        {
            { 0, NULL},
            { -1, NULL},
        },
        .check_wx = true,
    };

    walk_pgd(&st, NULL, 0);
    note_page(&st, 0, 0, 0, NULL);
    if (st.wx_pages)
    {
        pr_warn("Checked W+X mappings: FAILED, %lu W+X pages found\n",
                st.wx_pages);
    }
    else
    {
        pr_info("Checked W+X mappings: passed, no W+X pages found\n");
    }
}

static int show_memory_layout(void)
{
    printf("\n===========================================================DUMP MMU Table===========================================================\n");
    dump_page_table();
    printf("====================================================================================================================================\r\n");
    return 0;
}
