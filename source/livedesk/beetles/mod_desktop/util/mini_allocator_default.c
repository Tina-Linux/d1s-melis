/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : mini_allocater_default.h
* By        : Andy.zhang
* Func      : platform_adapter
* Version   : v1.0
* ============================================================================================================
* 2010-9-3 9:46:25  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/


#include "mini_allocator_default.h"

void *mini_allocator_default_alloc(MiniAllocator *thiz, size_t size)
{
    return esMEMS_Malloc(0, size);
}

void *mini_allocator_default_realloc(MiniAllocator *thiz, void *ptr, size_t new_size)
{
    return esMEMS_Realloc(0, ptr, new_size);
}

void  mini_allocator_default_free(MiniAllocator *thiz, void *ptr)
{
    esMEMS_Mfree(0, ptr);
    return;
}

void  mini_allocator_default_destroy(MiniAllocator *thiz)
{
    return;
}

static const MiniAllocator g_default_allocator =
{
    mini_allocator_default_alloc,
    mini_allocator_default_realloc,
    mini_allocator_default_free,
    mini_allocator_default_destroy,
    {0}
};

MiniAllocator *mini_allocator_default_create(void)
{
    return (MiniAllocator *)&g_default_allocator;
}
