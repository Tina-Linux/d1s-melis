/*
 * This file contains shadow memory manipulation code.
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 * Author: Andrey Ryabinin <ryabinin.a.a@gmail.com>
 *
 * Some code borrowed from https://github.com/xairy/kasan-prototype by
 *        Andrey Konovalov <adech.fo@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <typedef.h>
#include <stdbool.h>
#include <string.h>
#include "kasan.h"

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#define DISABLE_BRANCH_PROFILING
#define _RET_IP_        (unsigned long)__builtin_return_address(0)

#define __alias(symbol) __attribute__((alias(#symbol)))

#define __round_mask(x, y) ((__typeof__(x))((y)-1))
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)
#define round_down(x, y) ((x) & ~__round_mask(x, y))

#define PAGE_SIZE (4096)
#define KASAN_ALIGN(size, align)    (((size) + (align) - 1) & ~((align) - 1))
#define IS_ALIGNED(x, a)  (((x) & ((typeof(x))(a) - 1)) == 0)
#define PageHighMem(x) (0)

typedef struct slab_chunk_debug_magic
{
    char redzone[8];
} __attribute__((aligned(8))) slab_chunk_debug_magic;

/*
 * Poisons the shadow memory for 'size' bytes starting from 'addr'.
 * Memory addresses should be aligned to KASAN_SHADOW_SCALE_SIZE.
 */
static void kasan_poison_shadow(const void *address, size_t size, __u8 value)
{
    void *shadow_start, *shadow_end;

    shadow_start = kasan_mem_to_shadow(address);
    shadow_end = kasan_mem_to_shadow(address + size);

    memset(shadow_start, value, shadow_end - shadow_start);
}

void kasan_unpoison_shadow(const void *address, size_t size)
{
    kasan_poison_shadow(address, size, 0);

    if (size & KASAN_SHADOW_MASK)
    {
        __u8 *shadow = (__u8 *)kasan_mem_to_shadow(address + size);
        *shadow = size & KASAN_SHADOW_MASK;
    }
}

/*
 * Clear all poison for the region between the current SP and a provided
 * watermark value, as is sometimes required prior to hand-crafted asm function
 * returns in the middle of functions.
 */
void kasan_unpoison_stack_above_sp_to(const void *watermark)
{
    const void *sp = __builtin_frame_address(0);
    size_t size = watermark - sp;

    if (sp > watermark)
    {
        return;
    }
    kasan_unpoison_shadow(sp, size);
}

/*
 * All functions below always inlined so compiler could
 * perform better optimizations in each of __asan_loadX/__assn_storeX
 * depending on memory access size X.
 */

static __always_inline bool memory_is_poisoned_1(unsigned long addr)
{
    __s8 shadow_value = *(__s8 *)kasan_mem_to_shadow((void *)addr);

    if (unlikely(shadow_value))
    {
        __s8 last_accessible_byte = addr & KASAN_SHADOW_MASK;
        return unlikely(last_accessible_byte >= shadow_value);
    }

    return false;
}

static __always_inline bool memory_is_poisoned_2(unsigned long addr)
{
    __u8 *shadow_addr = (__u8 *)kasan_mem_to_shadow((void *)addr);

    if (unlikely(*shadow_addr))
    {
        if (memory_is_poisoned_1(addr + 1))
        {
            return true;
        }

        /*
         * If single shadow byte covers 2-byte access, we don't
         * need to do anything more. Otherwise, test the first
         * shadow byte.
         */
        if (likely(((addr + 1) & KASAN_SHADOW_MASK) != 0))
        {
            return false;
        }

        return unlikely(*(__u8 *)shadow_addr);
    }

    return false;
}

static __always_inline bool memory_is_poisoned_4(unsigned long addr)
{
    __u8 *shadow_addr = (__u8 *)kasan_mem_to_shadow((void *)addr);

    if (unlikely(*shadow_addr))
    {
        if (memory_is_poisoned_1(addr + 3))
        {
            return true;
        }

        /*
         * If single shadow byte covers 4-byte access, we don't
         * need to do anything more. Otherwise, test the first
         * shadow byte.
         */
        if (likely(((addr + 3) & KASAN_SHADOW_MASK) >= 3))
        {
            return false;
        }

        return unlikely(*(__u8 *)shadow_addr);
    }

    return false;
}

static __always_inline bool memory_is_poisoned_8(unsigned long addr)
{
    __u8 *shadow_addr = (__u8 *)kasan_mem_to_shadow((void *)addr);

    if (unlikely(*shadow_addr))
    {
        if (memory_is_poisoned_1(addr + 7))
        {
            return true;
        }

        /*
         * If single shadow byte covers 8-byte access, we don't
         * need to do anything more. Otherwise, test the first
         * shadow byte.
         */
        if (likely(IS_ALIGNED(addr, KASAN_SHADOW_SCALE_SIZE)))
        {
            return false;
        }

        return unlikely(*(__u8 *)shadow_addr);
    }

    return false;
}

static __always_inline bool memory_is_poisoned_16(unsigned long addr)
{
    __u32 *shadow_addr = (__u32 *)kasan_mem_to_shadow((void *)addr);

    if (unlikely(*shadow_addr))
    {
        __u16 shadow_first_bytes = *(__u16 *)shadow_addr;

        if (unlikely(shadow_first_bytes))
        {
            return true;
        }

        /*
         * If two shadow bytes covers 16-byte access, we don't
         * need to do anything more. Otherwise, test the last
         * shadow byte.
         */
        if (likely(IS_ALIGNED(addr, KASAN_SHADOW_SCALE_SIZE)))
        {
            return false;
        }

        return memory_is_poisoned_1(addr + 15);
    }

    return false;
}

static __always_inline unsigned long bytes_is_zero(const __u8 *start,
        size_t size)
{
    while (size)
    {
        if (unlikely(*start))
        {
            return (unsigned long)start;
        }
        start++;
        size--;
    }

    return 0;
}

static __always_inline unsigned long memory_is_zero(const void *start,
        const void *end)
{
    unsigned int words;
    unsigned long ret;
    unsigned int prefix = (unsigned long)start % 8;

    if (end - start <= 16)
    {
        return bytes_is_zero(start, end - start);
    }

    if (prefix)
    {
        prefix = 8 - prefix;
        ret = bytes_is_zero(start, prefix);
        if (unlikely(ret))
        {
            return ret;
        }
        start += prefix;
    }

    words = (end - start) / 8;
    while (words)
    {
        if (unlikely(*(u64 *)start))
        {
            return bytes_is_zero(start, 8);
        }
        start += 8;
        words--;
    }

    return bytes_is_zero(start, (end - start) % 8);
}

static __always_inline bool memory_is_poisoned_n(unsigned long addr,
        size_t size)
{
    unsigned long ret;

    ret = memory_is_zero(kasan_mem_to_shadow((void *)addr),
                         kasan_mem_to_shadow((void *)addr + size - 1) + 1);

    if (unlikely(ret))
    {
        unsigned long last_byte = addr + size - 1;
        __s8 *last_shadow = (__s8 *)kasan_mem_to_shadow((void *)last_byte);

        if (unlikely(ret != (unsigned long)last_shadow ||
                     ((long)(last_byte & KASAN_SHADOW_MASK) >= *last_shadow)))
        {
            return true;
        }
    }
    return false;
}

static __always_inline bool memory_is_poisoned(unsigned long addr, size_t size)
{
    if (__builtin_constant_p(size))
    {
        switch (size)
        {
            case 1:
                return memory_is_poisoned_1(addr);
            case 2:
                return memory_is_poisoned_2(addr);
            case 4:
                return memory_is_poisoned_4(addr);
            case 8:
                return memory_is_poisoned_8(addr);
            case 16:
                return memory_is_poisoned_16(addr);
            default:
                break;
        }
    }

    return memory_is_poisoned_n(addr, size);
}

static __always_inline void check_memory_region_inline(unsigned long addr,
        size_t size, bool write,
        unsigned long ret_ip)
{
    if (unlikely(size == 0))
    {
        return;
    }

    if (addr < KASAN_SHADOW_START)
    {
        return;
    }

    if (unlikely((void *)addr <
                 kasan_shadow_to_mem((void *)KASAN_SHADOW_START)))
    {
        kasan_report(addr, size, write, ret_ip);
        return;
    }

    if (likely(!memory_is_poisoned(addr, size)))
    {
        return;
    }

    kasan_report(addr, size, write, ret_ip);
}

static void check_memory_region(unsigned long addr,
                                size_t size, bool write,
                                unsigned long ret_ip)
{
    check_memory_region_inline(addr, size, write, ret_ip);
}

void kasan_check_read(const void *p, unsigned int size)
{
    check_memory_region((unsigned long)p, size, false, _RET_IP_);
}

void kasan_check_write(const void *p, unsigned int size)
{
    check_memory_region((unsigned long)p, size, true, _RET_IP_);
}

void *__memset(void *addr, int c, size_t len)
{
    int i = 0;
    char *dst = addr;

    if (len <= 0)
    {
        return addr;
    }

    for (i = 0; i < len; i++)
    {
        *(dst + i) = c;
    }
    return addr;
}

void *__memcpy(void *dest, const void *src, size_t count)
{
    char *d;
    const char *s;
    size_t size = count;

    if ((dest > (src + size)) || (dest < src))
    {
        d = dest;
        s = src;
        while (count--)
        {
            *d++ = *s++;
        }
    }
    else
    {
        d = (char *)(dest + count - 1);
        s = (char *)(src + count - 1);
        while (count --)
        {
            *d-- = *s--;
        }
    }
    return dest;
}

void *__memmove(void *dst, const void *src, size_t count)
{
    void *ret = dst;

    if (dst <= src || (char *)dst >= ((char *)src + count))
    {
        while (count--)
        {
            *(char *)dst = *(char *)src;
            dst = (char *)dst + 1;
            src = (char *)src + 1;
        }
    }
    else
    {
        dst = (char *)dst + count - 1;
        src = (char *)src + count - 1;
        while (count--)
        {
            *(char *)dst = *(char *)src;
            dst = (char *)dst - 1;
            src = (char *)src - 1;
        }
    }
    return ret;
}

#undef memset
void *memset(void *addr, int c, size_t len)
{
    check_memory_region((unsigned long)addr, len, true, _RET_IP_);

    return __memset(addr, c, len);
}

#undef memmove
void *__memmove(void *dest, const void *src, size_t len);
void *memmove(void *dest, const void *src, size_t len)
{
    check_memory_region((unsigned long)src, len, false, _RET_IP_);
    check_memory_region((unsigned long)dest, len, true, _RET_IP_);

    return __memmove(dest, src, len);
}

#undef memcpy
void *__memcpy(void *dest, const void *src, size_t len);
void *memcpy(void *dest, const void *src, size_t len)
{
    check_memory_region((unsigned long)src, len, false, _RET_IP_);
    check_memory_region((unsigned long)dest, len, true, _RET_IP_);

    return __memcpy(dest, src, len);
}


void kasan_alloc_pages(void *page, unsigned int npages)
{
    char *left_rz;
    char *right_rz;
    char *left_magic;
    char *right_magic;

    slab_chunk_debug_magic debug_magic;

    if (!page)
    {
        return;
    }

    if (likely(!PageHighMem(page)))
    {
        kasan_unpoison_shadow(page, PAGE_SIZE * npages);
    }

    left_rz = ((char *)page) - sizeof(debug_magic.redzone);
    kasan_poison_shadow(left_rz, sizeof(debug_magic.redzone), KASAN_PAGE_REDZONE);

    right_rz = ((char *)page) + npages * PAGE_SIZE;
    kasan_poison_shadow(right_rz, sizeof(debug_magic.redzone), KASAN_PAGE_REDZONE);
}

void kasan_free_pages(void *page, unsigned int npages)
{
    __s8 shadow_byte;

    if (!page)
    {
        return;
    }

    shadow_byte = *(__s8 *)kasan_mem_to_shadow(page);
    if (shadow_byte < 0 || shadow_byte >= KASAN_SHADOW_SCALE_SIZE)
    {
        kasan_report_double_free(PAGE_SIZE * npages, page, __builtin_return_address(0));
        return;
    }

    if (likely(!PageHighMem(page)))
    {
        kasan_poison_shadow(page, PAGE_SIZE * npages, KASAN_FREE_PAGE);
    }
}

void kasan_malloc_large(void *page, unsigned int size)
{
    char *left_rz;
    char *right_rz;
    char *left_magic;
    char *right_magic;

    slab_chunk_debug_magic debug_magic;

    if (!page)
    {
        return;
    }

    if (likely(!PageHighMem(page)))
    {
        kasan_unpoison_shadow(page, size);
    }

    left_rz = ((char *)page) - sizeof(debug_magic.redzone);
    kasan_poison_shadow(left_rz, sizeof(debug_magic.redzone), KASAN_PAGE_REDZONE);

    right_rz = ((char *)page) + KASAN_ALIGN(size, 8);
    kasan_poison_shadow(right_rz, sizeof(debug_magic.redzone), KASAN_PAGE_REDZONE);
}

void kasan_free_large(void *page, unsigned int size)
{
    if (!page)
    {
        return;
    }

    if (likely(!PageHighMem(page)))
    {
        size = KASAN_ALIGN(size, 8);
        kasan_poison_shadow(page, size, KASAN_FREE_PAGE);
    }
}

void kasan_malloc_small(void *ptr, unsigned int size)
{
    char *left_rz;
    char *right_rz;
    char *left_magic;
    char *right_magic;

    slab_chunk_debug_magic debug_magic;

    if (!ptr)
    {
        return;
    }

    if (likely(!PageHighMem(ptr)))
    {
        kasan_unpoison_shadow(ptr, size);
    }

    left_rz = ((char *)ptr) - sizeof(debug_magic.redzone);
    kasan_poison_shadow(left_rz, sizeof(debug_magic.redzone), KASAN_KMALLOC_REDZONE);

    right_rz = ((char *)ptr) + KASAN_ALIGN(size, 8);
    kasan_poison_shadow(right_rz, sizeof(debug_magic.redzone), KASAN_KMALLOC_REDZONE);
}

void kasan_free_small(void *ptr, unsigned int size)
{
    if (!ptr)
    {
        return;
    }

    if (likely(!PageHighMem(ptr)))
    {
        size = KASAN_ALIGN(size, 8);
        kasan_poison_shadow(ptr, size, KASAN_FREE_PAGE);
    }
}

void kasan_realloc_small(void *ptr, unsigned int size)
{
    char *left_rz;
    char *right_rz;
    char *left_magic;
    char *right_magic;

    slab_chunk_debug_magic debug_magic;

    if (!ptr)
    {
        return;
    }

    if (likely(!PageHighMem(ptr)))
    {
        kasan_unpoison_shadow(ptr, size);
    }

    left_rz = ((char *)ptr) - sizeof(debug_magic.redzone);
    kasan_poison_shadow(left_rz, sizeof(debug_magic.redzone), KASAN_KMALLOC_REDZONE);

    right_rz = ((char *)ptr) + KASAN_ALIGN(size, 8);
    kasan_poison_shadow(right_rz, sizeof(debug_magic.redzone), KASAN_KMALLOC_REDZONE);
}

void kasan_double_free_check(void *ptr)
{
    __s8 shadow_byte;

    if (!ptr)
    {
        return;
    }

    shadow_byte = *(__s8 *)kasan_mem_to_shadow(ptr);
    if (shadow_byte < 0 || shadow_byte >= KASAN_SHADOW_SCALE_SIZE)
    {
        kasan_report_double_free(0, ptr, __builtin_return_address(0));
    }
}

/*
 * Adaptive redzone policy taken from the userspace AddressSanitizer runtime.
 * For larger allocations larger redzones are used.
 */
static size_t optimal_redzone(size_t object_size)
{
    int rz =
        object_size <= 64        - 16   ? 16 :
        object_size <= 128       - 32   ? 32 :
        object_size <= 512       - 64   ? 64 :
        object_size <= 4096      - 128  ? 128 :
        object_size <= (1 << 14) - 256  ? 256 :
        object_size <= (1 << 15) - 512  ? 512 :
        object_size <= (1 << 16) - 1024 ? 1024 : 2048;
    return rz;
}

static void register_global(struct kasan_global *global)
{
    size_t aligned_size = round_up(global->size, KASAN_SHADOW_SCALE_SIZE);

    kasan_unpoison_shadow(global->beg, global->size);

    kasan_poison_shadow(global->beg + aligned_size,
                        global->size_with_redzone - aligned_size,
                        KASAN_GLOBAL_REDZONE);
}

void __asan_register_globals(struct kasan_global *globals, size_t size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        register_global(&globals[i]);
    }
}

void __asan_unregister_globals(struct kasan_global *globals, size_t size)
{
}

void __asan_before_dynamic_init(const char *module_name)
{

}

void __asan_after_dynamic_init(void)
{

}

#define DEFINE_ASAN_LOAD_STORE(size)                    \
    void __asan_load##size(unsigned long addr)          \
    {                               \
        check_memory_region_inline(addr, size, false, _RET_IP_);\
    }                               \
    __alias(__asan_load##size)                  \
    void __asan_load##size##_noabort(unsigned long);        \
    void __asan_store##size(unsigned long addr)         \
    {                               \
        check_memory_region_inline(addr, size, true, _RET_IP_); \
    }                               \
    __alias(__asan_store##size)                 \
    void __asan_store##size##_noabort(unsigned long);       \

DEFINE_ASAN_LOAD_STORE(1);
DEFINE_ASAN_LOAD_STORE(2);
DEFINE_ASAN_LOAD_STORE(4);
DEFINE_ASAN_LOAD_STORE(8);
DEFINE_ASAN_LOAD_STORE(16);

void __asan_loadN(unsigned long addr, size_t size)
{
    check_memory_region(addr, size, false, _RET_IP_);
}

__alias(__asan_loadN)
void __asan_loadN_noabort(unsigned long, size_t);

void __asan_storeN(unsigned long addr, size_t size)
{
    check_memory_region(addr, size, true, _RET_IP_);
}

__alias(__asan_storeN)
void __asan_storeN_noabort(unsigned long, size_t);

/* to shut up compiler complaints */
void __asan_handle_no_return(void) {}

/* Emitted by compiler to poison large objects when they go out of scope. */
void __asan_poison_stack_memory(const void *addr, size_t size)
{
    /*
     * Addr is KASAN_SHADOW_SCALE_SIZE-aligned and the object is surrounded
     * by redzones, so we simply round up size to simplify logic.
     */
    kasan_poison_shadow(addr, round_up(size, KASAN_SHADOW_SCALE_SIZE),
                        KASAN_USE_AFTER_SCOPE);
}

/* Emitted by compiler to unpoison large objects when they go into scope. */
void __asan_unpoison_stack_memory(const void *addr, size_t size)
{
    kasan_unpoison_shadow(addr, size);
}

/* Emitted by compiler to poison alloca()ed objects. */
void __asan_alloca_poison(unsigned long addr, size_t size)
{
    size_t rounded_up_size = round_up(size, KASAN_SHADOW_SCALE_SIZE);
    size_t padding_size = round_up(size, KASAN_ALLOCA_REDZONE_SIZE) -
                          rounded_up_size;
    size_t rounded_down_size = round_down(size, KASAN_SHADOW_SCALE_SIZE);

    const void *left_redzone = (const void *)(addr -
                               KASAN_ALLOCA_REDZONE_SIZE);
    const void *right_redzone = (const void *)(addr + rounded_up_size);

    kasan_unpoison_shadow((const void *)(addr + rounded_down_size),
                          size - rounded_down_size);
    kasan_poison_shadow(left_redzone, KASAN_ALLOCA_REDZONE_SIZE,
                        KASAN_ALLOCA_LEFT);
    kasan_poison_shadow(right_redzone,
                        padding_size + KASAN_ALLOCA_REDZONE_SIZE,
                        KASAN_ALLOCA_RIGHT);
}

/* Emitted by compiler to unpoison alloca()ed areas when the stack unwinds. */
void __asan_allocas_unpoison(const void *stack_top, const void *stack_bottom)
{
    if (unlikely(!stack_top || stack_top > stack_bottom))
    {
        return;
    }

    kasan_unpoison_shadow(stack_top, stack_bottom - stack_top);
}

/* Emitted by the compiler to [un]poison local variables. */
#define DEFINE_ASAN_SET_SHADOW(byte) \
    void __asan_set_shadow_##byte(const void *addr, size_t size)    \
    {                               \
        __memset((void *)addr, 0x##byte, size); \
    }                               \


DEFINE_ASAN_SET_SHADOW(00);
DEFINE_ASAN_SET_SHADOW(f1);
DEFINE_ASAN_SET_SHADOW(f2);
DEFINE_ASAN_SET_SHADOW(f3);
DEFINE_ASAN_SET_SHADOW(f5);
DEFINE_ASAN_SET_SHADOW(f8);
