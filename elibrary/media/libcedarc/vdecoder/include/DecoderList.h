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
#ifndef CDX_LIST_H
#define CDX_LIST_H
#include "DecoderTypes.h"

#define CDX_LIST_POISON1  ((void *) 0x00700700)
#define CDX_LIST_POISON2  ((void *) 0x00900900)

static inline void DecoderPrefetchIon(const void *x)
{
    (void)x;
}

struct DecoderListNodeS
{
    struct DecoderListNodeS *next;
    struct DecoderListNodeS *prev;
};

struct DecoderListS
{
    struct DecoderListNodeS *head;
    struct DecoderListNodeS *tail;
};

#define DecoderListInit(list) do { \
    (list)->head = (list)->tail = (struct DecoderListNodeS *)(list);\
    }while (0)

#define DecoderListNodeInit(node) do { \
    (node)->next = (node)->prev = (node);\
    }while (0)

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef AWP_DEBUG

void DecoderListAdd(struct DecoderListNodeS *newList, struct DecoderListS *list);

void DecoderListAddBefore(struct DecoderListNodeS *newList, struct DecoderListNodeS *pos);

void DecoderListAddAfter(struct DecoderListNodeS *newList, struct DecoderListNodeS *pos);

void DecoderListAddTail(struct DecoderListNodeS *newList, struct DecoderListS *list);

void DecoderListDel(struct DecoderListNodeS *node);

int DecoderListEmpty(const struct DecoderListS *list);

#else
#include <DecoderListFunc.h>
#endif

#ifdef __cplusplus
}
#endif

#define DecoderListEntry(ptr, type, member) \
    DecoderContainerOf(ptr, type, member)

#define DecoderListFirstEntry(ptr, type, member) \
    DecoderListEntry((ptr)->head, type, member)

#define DecoderListForEach(pos, list) \
    for (pos = (list)->head; \
            pos != (struct DecoderListNodeS *)(list);\
            pos = pos->next)

#define DecoderListForEachPrev(pos, list) \
    for (pos = (list)->tail; \
        pos != (struct DecoderListNodeS *)(list); \
        pos = pos->prev)

#define DecoderListForEachSafe(pos, n, list) \
    for (pos = (list)->head, n = pos->next; \
        pos != (struct DecoderListNodeS *)(list); \
        pos = n, n = pos->next)

#define DecoderListForEachSafeIon(pos, n, list) \
        for (pos = (list)->next, n = pos->next; pos != (list); \
        pos = n, n = pos->next)

#define DecoderListForEachPrevSafe(pos, n, list) \
    for (pos = (list)->tail, n = pos->prev; \
         pos != (struct DecoderListNodeS *)(list); \
         pos = n, n = pos->prev)

#define DecoderListForEachEntry(pos, list, member)                \
    for (pos = DecoderListEntry((list)->head, typeof(*pos), member);    \
         &pos->member != (struct DecoderListNodeS *)(list);     \
         pos = DecoderListEntry(pos->member.next, typeof(*pos), member))

#define DecoderListForEachEntryIon(pos, list, member) \
for (pos = DecoderListEntry((list)->next, typeof(*pos), member); \
     DecoderPrefetchIon(pos->member.next), &pos->member != (list);  \
     pos = DecoderListEntry(pos->member.next, typeof(*pos), member))

#define DecoderListForEachEntryReverse(pos, list, member)            \
    for (pos = DecoderListEntry((list)->tail, typeof(*pos), member);    \
         &pos->member != (struct DecoderListNodeS *)(list);     \
         pos = DecoderListEntry(pos->member.prev, typeof(*pos), member))

#define DecoderListForEachEntrySafe(pos, n, list, member)            \
    for (pos = DecoderListEntry((list)->head, typeof(*pos), member),    \
        n = DecoderListEntry(pos->member.next, typeof(*pos), member);    \
         &pos->member != (struct DecoderListNodeS *)(list);                     \
         pos = n, n = DecoderListEntry(n->member.next, typeof(*n), member))

#define DecoderListForEachEntrySafeReverse(pos, n, list, member)        \
    for (pos = DecoderListEntry((list)->prev, typeof(*pos), member),    \
        n = DecoderListEntry(pos->member.prev, typeof(*pos), member);    \
         &pos->member != (struct DecoderListNodeS *)(list);                     \
         pos = n, n = DecoderListEntry(n->member.prev, typeof(*n), member))

#endif
