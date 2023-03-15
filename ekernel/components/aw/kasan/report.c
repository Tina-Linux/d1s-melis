/*
 * This file contains error reporting code.
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

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <rtthread.h>

#include <bits.h>
#include <log.h>
#include <typedef.h>
#include <backtrace.h>
#include <libs/hexdump.h>

#include "kasan.h"

extern char *get_current_thread_name(void);
extern int get_current_thread_id(void);

#define PAGE_SIZE (4096)
#define TASK_SIZE 0xffffffff

/* Shadow layout customization. */
#define SHADOW_BYTES_PER_BLOCK 1
#define SHADOW_BLOCKS_PER_ROW 16
#define SHADOW_BYTES_PER_ROW (SHADOW_BLOCKS_PER_ROW * SHADOW_BYTES_PER_BLOCK)
#define SHADOW_ROWS_AROUND_ADDR 2
#define _RET_IP_        (unsigned long)__builtin_return_address(0)

#define __round_mask(x, y) ((__typeof__(x))((y)-1))
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)
#define round_down(x, y) ((x) & ~__round_mask(x, y))

#define KASAN_REPORT_INIT_FLAG (0x00000001)
#define KASAN_REPORT_SHOW_FLAG (0x00000010)
#define KASAN_BIT_MULTI_SHOT   (0x00000100)
#define KASAN_BIT_REPORTED     (0x00001000)

static unsigned long kasan_flags = 0;

static inline bool kasan_report_enabled(void);

void kasan_disable_report(void)
{
    kasan_flags &= ~(KASAN_REPORT_SHOW_FLAG);
}

void kasan_enable_report(void)
{
    kasan_flags |= KASAN_REPORT_SHOW_FLAG;
}

void kasan_init_report(void)
{
    kasan_flags |= KASAN_REPORT_INIT_FLAG;
}

void kasan_deinit_report(void)
{
    kasan_flags &= ~(KASAN_REPORT_INIT_FLAG);
}

int kasan_save_enable_multi_shot(void)
{
    int flag = kasan_flags & KASAN_BIT_MULTI_SHOT;
    kasan_flags |= KASAN_BIT_MULTI_SHOT;
    return kasan_flags;
}

int kasan_restore_multi_shot(int flag)
{
    if (flag & KASAN_BIT_MULTI_SHOT)
    {
        kasan_flags |= flag;
    }
    else
    {
        kasan_flags &= ~(KASAN_BIT_MULTI_SHOT);
    }
    return kasan_flags;
}

int kasan_set_reported_bit(void)
{
    kasan_flags |= KASAN_BIT_REPORTED;
    return kasan_flags;
}

static const void *find_first_bad_addr(const void *addr, size_t size)
{
    __u8 shadow_val = *(__u8 *)kasan_mem_to_shadow(addr);
    const void *first_bad_addr = addr;

    while (!shadow_val && first_bad_addr < addr + size)
    {
        first_bad_addr += KASAN_SHADOW_SCALE_SIZE;
        shadow_val = *(__u8 *)kasan_mem_to_shadow(first_bad_addr);
    }
    return first_bad_addr;
}

static bool addr_has_shadow(struct kasan_access_info *info)
{
    return (info->access_addr >=
            kasan_shadow_to_mem((void *)KASAN_SHADOW_START));
}

static const char *get_shadow_bug_type(struct kasan_access_info *info)
{
    const char *bug_type = "unknown-crash";
    __u8 *shadow_addr;

    info->first_bad_addr = find_first_bad_addr(info->access_addr,
                           info->access_size);

    shadow_addr = (__u8 *)kasan_mem_to_shadow(info->first_bad_addr);

    /*
     * If shadow byte value is in [0, KASAN_SHADOW_SCALE_SIZE) we can look
     * at the next shadow byte to determine the type of the bad access.
     */
    if (*shadow_addr > 0 && *shadow_addr <= KASAN_SHADOW_SCALE_SIZE - 1)
    {
        shadow_addr++;
    }

    switch (*shadow_addr)
    {
        case 0 ... KASAN_SHADOW_SCALE_SIZE - 1:
            /*
             * In theory it's still possible to see these shadow values
             * due to a data race in the kernel code.
             */
            bug_type = "out-of-bounds";
            break;
        case KASAN_PAGE_REDZONE:
        case KASAN_KMALLOC_REDZONE:
            bug_type = "slab-out-of-bounds";
            break;
        case KASAN_GLOBAL_REDZONE:
            bug_type = "global-out-of-bounds";
            break;
        case KASAN_STACK_LEFT:
        case KASAN_STACK_MID:
        case KASAN_STACK_RIGHT:
        case KASAN_STACK_PARTIAL:
            bug_type = "stack-out-of-bounds";
            break;
        case KASAN_FREE_PAGE:
        case KASAN_KMALLOC_FREE:
            bug_type = "use-after-free";
            break;
        case KASAN_USE_AFTER_SCOPE:
            bug_type = "use-after-scope";
            break;
        case KASAN_ALLOCA_LEFT:
        case KASAN_ALLOCA_RIGHT:
            bug_type = "alloca-out-of-bounds";
            break;
    }

    return bug_type;
}

static const char *get_wild_bug_type(struct kasan_access_info *info)
{
    const char *bug_type = "unknown-crash";

    if ((unsigned long)info->access_addr < PAGE_SIZE)
    {
        bug_type = "null-ptr-deref";
    }
    else if ((unsigned long)info->access_addr < TASK_SIZE)
    {
        bug_type = "user-memory-access";
    }
    else
    {
        bug_type = "wild-memory-access";
    }

    return bug_type;
}

static const char *get_bug_type(struct kasan_access_info *info)
{
    if (addr_has_shadow(info))
    {
        return get_shadow_bug_type(info);
    }
    return get_wild_bug_type(info);
}

static void print_error_description(struct kasan_access_info *info)
{
    const char *bug_type = get_bug_type(info);

    rt_kprintf("BUG: KASAN: %s in %p\n",
           bug_type, (void *)info->ip);
    rt_kprintf("%s of size %u at addr %p by task %s/%d\n",
           info->is_write ? "Write" : "Read", info->access_size,
           info->access_addr, get_current_thread_name(), get_current_thread_id());
}

static inline bool kernel_or_module_addr(const void *addr)
{
#if 0
    if (addr >= (void *)_stext && addr < (void *)_end)
    {
        return true;
    }
    if (is_module_address((unsigned long)addr))
    {
        return true;
    }
    return false;
#endif
    return false;
}

static inline bool init_task_stack_addr(const void *addr)
{
    return false;
}

static void kasan_start_report(unsigned long *flags)
{
    /*
     * Make sure we don't end up in loop.
     */
    uint32_t hal_spin_lock_irqsave(void);
    *flags = hal_spin_lock_irqsave();
    kasan_disable_report();
    rt_kprintf("==================================================================\n");
}

static void kasan_end_report(unsigned long *flags)
{
    rt_kprintf("==================================================================\n");
    kasan_enable_report();
    void hal_spin_unlock_irqrestore(uint32_t __cpsr);
    hal_spin_unlock_irqrestore(*flags);
}

static void print_track(struct kasan_track *track, const char *prefix)
{
#if 0
    rt_kprintf("%s by task %u:\n", prefix, track->pid);
    if (track->stack)
    {
        struct stack_trace trace;

        depot_fetch_stack(track->stack, &trace);
        print_stack_trace(&trace, 0);
    }
    else
    {
        rt_kprintf("(stack is not available)\n");
    }
#endif
}

static void print_address_description(void *addr)
{
    void print_memory_address_description(void *addr);
    print_memory_address_description(addr);
}

static bool row_is_guilty(const void *row, const void *guilty)
{
    return (row <= guilty) && (guilty < row + SHADOW_BYTES_PER_ROW);
}

static int shadow_pointer_offset(const void *row, const void *shadow)
{
    /* The length of ">ff00ff00ff00ff00: " is
     *    3 + (BITS_PER_LONG/8)*2 chars.
     */
    return 3 + (BITS_PER_LONG / 8) * 2 + (shadow - row) * 2 +
           (shadow - row) / SHADOW_BYTES_PER_BLOCK + 1;
}

static void print_shadow_for_address(const void *addr)
{
    int i;
    const void *shadow = kasan_mem_to_shadow(addr);
    const void *shadow_row;

    shadow_row = (void *)round_down((unsigned long)shadow,
                                    SHADOW_BYTES_PER_ROW)
                 - SHADOW_ROWS_AROUND_ADDR * SHADOW_BYTES_PER_ROW;

    rt_kprintf("Memory state around the buggy address:\n");

    for (i = -SHADOW_ROWS_AROUND_ADDR; i <= SHADOW_ROWS_AROUND_ADDR; i++)
    {
        const void *kaddr = kasan_shadow_to_mem(shadow_row);
        char buffer[4 + (BITS_PER_LONG / 8) * 2 + 4];
        char shadow_buf[SHADOW_BYTES_PER_ROW];

        memset(buffer, 0, sizeof(buffer));
        rt_snprintf(buffer, sizeof(buffer),
                 (i == 0) ? ">%p: " : " %p: ", kaddr);
        /*
         * We should not pass a shadow pointer to generic
         * function, because generic functions may try to
         * access kasan mapping for the passed address.
         */
        memcpy(shadow_buf, shadow_row, SHADOW_BYTES_PER_ROW);
        print_hex_dump(KERN_ERR, buffer,
                       DUMP_PREFIX_NONE, SHADOW_BYTES_PER_ROW, 1,
                       shadow_buf, SHADOW_BYTES_PER_ROW, 0);

        if (row_is_guilty(shadow_row, shadow))
            rt_kprintf("%*c\n",
                   shadow_pointer_offset(shadow_row, shadow),
                   '^');

        shadow_row += SHADOW_BYTES_PER_ROW;
    }
}

void kasan_report_double_free(unsigned int size, void *object,
                              void *ip)
{
    unsigned long flags;

    if (likely(!kasan_report_enabled()))
    {
        return;
    }

    kasan_start_report(&flags);
    rt_kprintf("BUG: KASAN: double-free or invalid-free in %p\n", ip);
    rt_kprintf("\n");
    print_address_description(object);
    rt_kprintf("\n");
    print_shadow_for_address(object);
    rt_kprintf("\nCaller Backtrace:\n");
    backtrace(NULL, NULL, 0, 0, rt_kprintf);

    kasan_end_report(&flags);
}

static void kasan_report_error(struct kasan_access_info *info)
{
    unsigned long flags;

    kasan_start_report(&flags);

    print_error_description(info);
    rt_kprintf("\n");

    if (!addr_has_shadow(info))
    {
        backtrace(NULL, NULL, 0, 0, rt_kprintf);
    }
    else
    {
        print_address_description((void *)info->access_addr);
        rt_kprintf("\n");
        /* print_shadow_for_address(info->first_bad_addr); */
        rt_kprintf("\nCaller Backtrace:\n");
        backtrace(NULL, NULL, 0, 0, rt_kprintf);
    }
#ifndef CONFIG_KASAN_REPEAT_REPORT_ERROR
    kasan_set_reported_bit();
#endif
    kasan_end_report(&flags);
}

static inline bool kasan_report_enabled(void)
{
    if (!(kasan_flags & KASAN_REPORT_INIT_FLAG))
    {
        return false;
    }
    if (!(kasan_flags & KASAN_REPORT_SHOW_FLAG))
    {
        return false;
    }
    if (kasan_flags & KASAN_BIT_MULTI_SHOT)
    {
        return true;
    }
    if (kasan_flags & KASAN_BIT_REPORTED)
    {
        return false;
    }
    return true;
}

void kasan_report(unsigned long addr, size_t size,
                  bool is_write, unsigned long ip)
{
    struct kasan_access_info info;

    if (likely(!kasan_report_enabled()))
    {
        return;
    }

    info.access_addr = (void *)addr;
    info.first_bad_addr = (void *)addr;
    info.access_size = size;
    info.is_write = is_write;
    info.ip = ip;

    kasan_report_error(&info);
}


#define DEFINE_ASAN_REPORT_LOAD(size)                     \
    void __asan_report_load##size##_noabort(unsigned long addr) \
    {                                                         \
        kasan_report(addr, size, false, _RET_IP_);    \
    }                                                         \

#define DEFINE_ASAN_REPORT_STORE(size)                     \
    void __asan_report_store##size##_noabort(unsigned long addr) \
    {                                                          \
        kasan_report(addr, size, true, _RET_IP_);      \
    }                                                          \

DEFINE_ASAN_REPORT_LOAD(1);
DEFINE_ASAN_REPORT_LOAD(2);
DEFINE_ASAN_REPORT_LOAD(4);
DEFINE_ASAN_REPORT_LOAD(8);
DEFINE_ASAN_REPORT_LOAD(16);
DEFINE_ASAN_REPORT_STORE(1);
DEFINE_ASAN_REPORT_STORE(2);
DEFINE_ASAN_REPORT_STORE(4);
DEFINE_ASAN_REPORT_STORE(8);
DEFINE_ASAN_REPORT_STORE(16);

void __asan_report_load_n_noabort(unsigned long addr, size_t size)
{
    kasan_report(addr, size, false, _RET_IP_);
}

void __asan_report_store_n_noabort(unsigned long addr, size_t size)
{
    kasan_report(addr, size, true, _RET_IP_);
}
