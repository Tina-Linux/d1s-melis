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
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <barrier.h>
#include "page_2nd_level.h"
#include "pgtable.h"
#include <ktimer.h>
#include <log.h>

void show_pte(unsigned long addr)
{
    pgd_t *pgd;
    pgd_t *pgd_base;

    pgd_base = pgd_offset(NULL, 0UL);
    printf("pgdbase = 0x%08lx\n", (unsigned long)pgd_base);

    pgd = pgd_offset(NULL, addr);
    printf("[0x%08lx] *pgd=0x%08llx",
           addr, (long long)pgd_val(*pgd));

    do
    {
        pud_t *pud;
        pmd_t *pmd;
        pte_t *pte;

        if (pgd_none(*pgd))
        {
            break;
        }

        if (pgd_bad(*pgd))
        {
            printf("(bad)");
            break;
        }

        pud = pud_offset(pgd, addr);
        if (PTRS_PER_PUD != 1)
        {
            printf(", *pud=%08llx", (long long)pud_val(*pud));
        }

        if (pud_none(*pud))
        {
            break;
        }

        if (pud_bad(*pud))
        {
            printf("(bad)");
            break;
        }

        pmd = pmd_offset(pud, addr);
        if (PTRS_PER_PMD != 1)
        {
            printf(", *pmd=%08llx", (long long)pmd_val(*pmd));
        }

        if (pmd_none(*pmd))
        {
            break;
        }

        if (pmd_bad(*pmd))
        {
            printf("(bad)");
            break;
        }

        pte = pte_offset_map(pmd, addr);
        printf(", *pte=%08llx", (long long)pte_val(*pte));
        printf(", *ppte=%08llx",
               (long long)pte_val(pte[PTE_HWTABLE_PTRS]));
        pte_unmap(pte);
    } while (0);

    printf("\n");
}
