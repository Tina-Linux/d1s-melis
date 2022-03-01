/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : platform_adapter.h
* By        : Andy.zhang
* Func      : platform_adapter
* Version   : v1.0
* ============================================================================================================
* 2010-9-3 9:46:25  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
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
