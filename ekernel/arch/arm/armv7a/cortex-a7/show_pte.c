/*
 * ===========================================================================================
 *
 *       Filename:  show_pte.c
 *
 *    Description:  used to do memory map for 2nd level page table on ARMV7-A.
 *
 *        Version:  Melis3.0
 *         Create:  2019-10-29 16:19:44
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *                  porting from linux 4.9.
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-06-02 14:00:14
 *
 * ===========================================================================================
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
