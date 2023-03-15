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
#ifndef _PGTABLE_NOP4D_H
#define _PGTABLE_NOP4D_H

#ifndef __ASSEMBLY__

#define __PAGETABLE_P4D_FOLDED 1

typedef struct
{
    pgd_t pgd;
} p4d_t;

#define P4D_SHIFT       PGDIR_SHIFT
#define MAX_PTRS_PER_P4D    1
#define PTRS_PER_P4D        1
#define P4D_SIZE        (1UL << P4D_SHIFT)
#define P4D_MASK        (~(P4D_SIZE-1))

static inline int pgd_none(pgd_t pgd)
{
    return 0;
}
static inline int pgd_bad(pgd_t pgd)
{
    return 0;
}
static inline int pgd_present(pgd_t pgd)
{
    return 1;
}
static inline void pgd_clear(pgd_t *pgd)    { }
#define p4d_ERROR(p4d)              (pgd_ERROR((p4d).pgd))

#define pgd_populate(mm, pgd, p4d)      do { } while (0)
#define pgd_populate_safe(mm, pgd, p4d)     do { } while (0)
/*
 * (p4ds are folded into pgds so this doesn't get actually called,
 * but the define is needed for a generic inline function.)
 */
#define set_pgd(pgdptr, pgdval) set_p4d((p4d_t *)(pgdptr), (p4d_t) { pgdval })

static inline p4d_t *p4d_offset(pgd_t *pgd, unsigned long address)
{
    return (p4d_t *)pgd;
}

#define p4d_val(x)              (pgd_val((x).pgd))
#define __p4d(x)                ((p4d_t) { __pgd(x) })

#define pgd_page(pgd)               (p4d_page((p4d_t){ pgd }))
#define pgd_page_vaddr(pgd)         (p4d_page_vaddr((p4d_t){ pgd }))

/*
 * allocating and freeing a p4d is trivial: the 1-entry p4d is
 * inside the pgd, so has no extra memory associated with it.
 */
#define p4d_alloc_one(mm, address)      NULL
#define p4d_free(mm, x)             do { } while (0)
#define __p4d_free_tlb(tlb, x, a)       do { } while (0)

#undef  p4d_addr_end
#define p4d_addr_end(addr, end)         (end)

#endif /* __ASSEMBLY__ */
#endif /* _PGTABLE_NOP4D_H */
