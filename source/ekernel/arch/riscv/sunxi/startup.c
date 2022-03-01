/*
 * ===========================================================================================
 *
 *       Filename:  startup.c
 *
 *    Description:  rtkernel os startup routine.
 *
 *        Version:  Melis3.0
 *         Create:  2020-06-24 10:32:22
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-07-10 17:24:27
 *
 * ===========================================================================================
 */

#include <port.h>
#include <rtthread.h>
#include <version.h>
#include <kconfig.h>
#include <preempt.h>
#include <_newlib_version.h>
#include <debug.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arch.h>
#include <log.h>
#if (CONFIG_LOG_DEFAULT_LEVEL == 0)
#define printk(...)
#endif
static const char melis_banner [] =
{
    "|                   /'\\_/`\\       (_ )  _                       /'_  )    /'_ `\\   /' _`\\                     |\n\r"
    "|                   |     |   __   | | (_)  ___  ______  _   _ (_)_) |   ( (_) |   | ( ) |                    |\n\r"
    "|                   | (_) | /'__`\\ | | | |/',__)(______)( ) ( ) _(_ <     \\__, |   | | | |                    |\n\r"
    "|                   | | | |(  ___/ | | | |\\__, \\        | \\_/ |( )_) | _     | | _ | (_) |                    |\n\r"
    "|                   (_) (_)`\\____)(___)(_)(____/        `\\___/'`\\____)(_)    (_)(_)`\\___/'                    |\n\r"
};

#define MELIS_VERSION_STRING(major, minor, patch) \
    #major"."#minor"."#patch

const char *melis_banner_string(void)
{
    return ("V"MELIS_VERSION_STRING(3, 9, 0));
}

void show_melis_version(void)
{
    //use newlibc printk for long strings.
    printk("\n\r");
    printk("===============================================================================================================\n\r");
    printk("%s", melis_banner);
    printk("|version : %s%s%s\n\r", melis_banner_string(), XPOSTO(110), "|");
    printk("|commitid: %.*s%s%s\n\r", 100, SDK_GIT_VERSION, XPOSTO(110), "|");
#if defined(HAL_GIT_VERSION)
    printk("|halgitid: %.*s%s%s\n\r", 100, HAL_GIT_VERSION, XPOSTO(110), "|");
#endif
    printk("|sunxiver: %x%s%s\n\r", MELIS_VERSION_CODE, XPOSTO(110), "|");
    printk("|timever : %.*s%s%s\n\r", 100, SDK_UTS_VERSION, XPOSTO(110), "|");
    printk("|compiler: %.*s%s%s\n\r", 100, GCC_VERSION, XPOSTO(110), "|");
#ifdef CONFIG_CC_OPTIMIZE_FOR_SIZE
    printk("|optimal : %.*s%s%s\n\r", 100, "-Os -g -gdwarf-2 -gstrict-dwarf", XPOSTO(110), "|");
#else
    printk("|optimal : %.*s%s%s\n\r", 100, "-O0 -g -gdwarf-2 -gstrict-dwarf", XPOSTO(110), "|");
#endif
    printk("|linker  : %.*s%s%s\n\r", 100, LNK_VERSION, XPOSTO(110), "|");
    printk("|newlibc : %.*s%s%s\n\r", 100, _NEWLIB_VERSION, XPOSTO(110), "|");
    printk("|author  : %.*s%s%s\n\r", 100, SDK_COMPILE_BY, XPOSTO(110), "|");
    printk("===============================================================================================================\n\r");
    printk("\n\r");
}

static int rt_application_init(pfun_krnl_init_t app_init)
{
    static rt_thread_t app_init_thread;

    // highest priority to system bringup.
    app_init_thread = rt_thread_create("kstartup", app_init, RT_NULL, 0x4000, 25, 10);
    RT_ASSERT(app_init_thread != RT_NULL);

    rt_thread_startup(app_init_thread);

    return 0;
}

#define HEAP_TOP      (CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_SIZE)

void awos_init_bootstrap(pfun_bsp_init_t rt_system_bsp_init, pfun_krnl_init_t app_init, uint64_t ticks)
{
    void *heap_start, *heap_end;

    extern unsigned long __bss_end[];
    heap_start = (void *)__bss_end;
    heap_end   = (void *)HEAP_TOP;

    rt_system_heap_init(heap_start, heap_end);
    rt_system_timer_init();

    rt_system_scheduler_init();
    rt_system_timer_thread_init();

    void system_time_init(void);
    system_time_init();

    if (rt_system_bsp_init)
    {
        rt_system_bsp_init();
    }

    setbuf(stdout, NULL);
    show_melis_version();

    /* initialize idle thread */
    rt_thread_idle_init();

    rt_application_init(app_init);

    rt_system_scheduler_start();

#ifdef CONFIG_BRINGUP_IDLE
    void idle_entry(void);
    idle_entry();
#endif
    /* never come here */
    CODE_UNREACHABLE;
}
