#include <stdio.h>
#include <rtthread.h>
#include "kasan.h"

char *get_current_thread_name(void)
{
    rt_thread_t thread = rt_thread_self();
    return (char *) & (thread->name);
}

int get_current_thread_id(void)
{
    return 0;
}

void rt_page_alloc_func_hook(void *ptr, unsigned int npages)
{
    kasan_alloc_pages(ptr, npages);
}

void rt_page_free_func_hook(void *ptr, unsigned int npages)
{
    kasan_free_pages(ptr, npages);
}

void rt_malloc_large_func_hook(void *ptr, unsigned int size)
{
    kasan_malloc_large(ptr, size);
}

void rt_free_large_func_hook(void *ptr, unsigned int size)
{
    kasan_free_large(ptr, size);
}

void rt_malloc_small_func_hook(void *ptr, unsigned int size)
{
    kasan_malloc_small(ptr, size);
}

void rt_free_small_func_hook(void *ptr, unsigned int size)
{
    kasan_free_small(ptr, size);
}

void rt_realloc_small_func_hook(void *ptr, unsigned int size)
{
    kasan_realloc_small(ptr, size);
}

void rt_free_func_hook(void *ptr)
{
    kasan_double_free_check(ptr);
}
