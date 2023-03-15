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
#ifndef __MINI_ALLOCATER_H__
#define __MINI_ALLOCATER_H__

#include "platform_adapter.h"

__BEGIN_DECLS__

struct _MiniAllocator;
typedef struct _MiniAllocator MiniAllocator;

typedef void *(*MiniAllocatorAlloc)(MiniAllocator *thiz, size_t size);
typedef void *(*MiniAllocatorRealloc)(MiniAllocator *thiz, void *ptr, size_t new_size);
typedef void (*MiniAllocatorFree)(MiniAllocator *thiz, void *ptr);
typedef void (*MiniAllocatorDestroy)(MiniAllocator *thiz);

struct _MiniAllocator
{
    MiniAllocatorAlloc      alloc;
    MiniAllocatorRealloc    realloc;
    MiniAllocatorFree       free;
    MiniAllocatorDestroy    destroy;

    char priv[1];
};

static inline void *mini_allocator_alloc(MiniAllocator *thiz, size_t size)
{
    return_val_if_fail(thiz != NULL, NULL);

    return thiz->alloc(thiz, size);
}

static inline void *mini_allocator_realloc(MiniAllocator *thiz, void *ptr, size_t new_size)
{
    return_val_if_fail(thiz != NULL, NULL);

    return thiz->realloc(thiz, ptr, new_size);
}

static inline void  mini_allocator_free(MiniAllocator *thiz, void *ptr)
{
    return_if_fail(thiz != NULL);

    if (ptr == NULL)
    {
        return;
    }

    thiz->free(thiz, ptr);
    return;
}

static inline void  mini_allocator_destroy(MiniAllocator *thiz)
{
    if (thiz != NULL && thiz->destroy != NULL)
    {
        thiz->destroy(thiz);
    }

    return;
}

static inline void *mini_allocator_zalloc(MiniAllocator *thiz, size_t size)
{
    void *ptr = mini_allocator_alloc(thiz, size);

    return_val_if_fail(ptr != NULL, NULL);

    memset(ptr, 0x00, size);

    return ptr;
}

static inline void  mini_allocator_zfree(MiniAllocator *thiz, void *ptr, size_t size)
{
    return_if_fail(thiz != NULL && ptr != NULL);

    memset(ptr, 0x00, size);

    mini_allocator_free(thiz, ptr);

    return;
}

//
//char* mini_strdup(const char* str)
//{
//  char* new_str = NULL;
//  return_val_if_fail(str != NULL, NULL);
//  new_str = mini_allocator_alloc(mini_default_allocator(), strlen(str) + 1);
//
//  if(new_str != NULL)
//  {
//      strcpy(new_str, str);
//  }
//
//  return new_str;
//}

__END_DECLS__

#endif /* __MINI_ALLOCATER_H__ */
