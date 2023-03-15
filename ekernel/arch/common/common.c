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
#include <_ansi.h>
#include <reent.h>
#include <stdio.h>
#include <stdarg.h>
#include <version.h>

#include <rtthread.h>

#include <log.h>
#include <epos.h>
#include <hal_interrupt.h>
#include <hal_mem.h>
#include <hal_thread.h>
#include <hal_mutex.h>
#include <hal_atomic.h>

int _printf_r(struct _reent *ptr,
              const char *__restrict fmt, ...)
{
    int ret;
    va_list ap;

    _REENT_SMALL_CHECK_INIT(ptr);
    va_start(ap, fmt);
    ret = _vfprintf_r(ptr, _stdout_r(ptr), fmt, ap);
    va_end(ap);
    return ret;
}

#ifdef _NANO_FORMATTED_IO
int _iprintf_r(struct _reent *, const char *, ...) _ATTRIBUTE((__alias__("_printf_r")));
#endif

#ifndef _REENT_ONLY

#if defined(CONFIG_UART_CLI_USE_SPINLOCK)
static hal_spinlock_t io_lock;
#elif defined(CONFIG_UART_CLI_USE_MUTEX)
static hal_mutex io_mutex;
#endif

static unsigned long enter_io_critical(void)
{
#if defined(CONFIG_UART_CLI_USE_SPINLOCK)
    return hal_spin_lock_irqsave(&io_lock);
#elif defined(CONFIG_UART_CLI_USE_MUTEX)
    if (!kthread_in_critical_context())
    {
        hal_mutex_lock(&io_mutex);
    }
#endif
    return 0;
}

static void exit_io_critical(unsigned long cpsr)
{
#if defined(CONFIG_UART_CLI_USE_SPINLOCK)
    hal_spin_unlock_irqrestore(&io_lock, cpsr);
#elif defined(CONFIG_UART_CLI_USE_MUTEX)
    (void)cpsr;
    if (!kthread_in_critical_context())
    {
        hal_mutex_unlock(&io_mutex);
    }
#endif
}

int printf(const char *__restrict fmt, ...)
{
    int ret = 0;
    unsigned long flags;


    flags = enter_io_critical();

    va_list ap;
    struct _reent *ptr = _REENT;

    _REENT_SMALL_CHECK_INIT(ptr);
    va_start(ap, fmt);
    ret = _vfprintf_r(ptr, _stdout_r(ptr), fmt, ap);
    va_end(ap);

    exit_io_critical(flags);

    return ret;
}

#ifdef _NANO_FORMATTED_IO
int iprintf(const char *, ...) _ATTRIBUTE((__alias__("printf")));
#endif
#endif /* ! _REENT_ONLY */

void dump_memory(uint32_t *buf, int32_t len)
{
    int i;

    for (i = 0; i < len; i ++)
    {
        if ((i % 4) == 0)
        {
            printk("\n\r0x%p: ", buf + i);
        }
        printk("0x%08x ", buf[i]);
    }
    printk("\n\r");

    return;
}

void dump_register_memory(char *name, unsigned long addr, int32_t len)
{
    int i;
    int32_t check_virtual_address(unsigned long vaddr);

    if (check_virtual_address(addr) && check_virtual_address(addr + len * sizeof(int)))
    {
        printk("\n\rdump %s memory:", name);
        dump_memory((uint32_t *)addr, len);
    }
    else
    {
        printk("\n\r%s register corrupted!!\n", name);
    }

    return;
}

int g_cli_direct_read = 0;

void panic_goto_cli(void)
{
    if (g_cli_direct_read > 0)
    {
        printk("%s can not be reentrant!\n", __func__);
        return;
    }
    g_cli_direct_read = 1;
#ifdef CONFIG_RT_USING_FINSH
    void finsh_thread_entry(void *parameter);
    finsh_thread_entry(NULL);
#endif
}

#define MELIS_VERSION_STRING(major, minor, patch) \
    #major"."#minor"."#patch

const char *melis_banner_string(void)
{
    return ("V"MELIS_VERSION_STRING(3, 9, 0));
}

static const char melis_banner [] =
{
    "|                  /'\\_/`\\       (_ )  _                      ( )( )     /' _`\\    /' _`\\ \n\r"
    "|                  |     |   __   | | (_)  ___  ______  _   _ | || |     | ( ) |   | ( ) |    \n\r"
    "|                  | (_) | /'__`\\ | | | |/',__)(______)( ) ( )| || |_    | | | |   | | | |   \n\r"
    "|                  | | | |(  ___/ | | | |\\__, \\        | \\_/ |(__ ,__) _ | (_) | _ | (_) | \n\r"
    "|                  (_) (_)`\\____)(___)(_)(____/        `\\___/'   (_)  (_)`\\___/'(_)`\\___/'\n\r"
};

#if (CONFIG_LOG_DEFAULT_LEVEL == 0)
#define printk(...)
#endif

void show_melis_version(void)
{
#ifdef CONFIG_SHOW_FULL_VERSION
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
    printk("|optimal : %.*s%d%s%s%s\n\r", 100, "-O", CONFIG_CC_OPTIMIZE_LEVEL, " -g -gdwarf-2 -gstrict-dwarf", XPOSTO(110), "|");
#endif
    printk("|linker  : %.*s%s%s\n\r", 100, LNK_VERSION, XPOSTO(110), "|");
    printk("|newlibc : %.*s%s%s\n\r", 100, _NEWLIB_VERSION, XPOSTO(110), "|");
    printk("|author  : %.*s%s%s\n\r", 100, SDK_COMPILE_BY, XPOSTO(110), "|");
    printk("===============================================================================================================\n\r");
#else
    printk("\n\r");
    printk("|commitid: %.*s\n\r", 40, SDK_GIT_SHORT_VERSION);
    printk("|halgitid: %.*s\n\r", 40, HAL_GIT_SHORT_VERSION);
    printk("|timever : %.*s\n\r", 40, SDK_UTS_VERSION);
#endif
    printk("\n\r");
}

void common_init(void)
{
#ifndef _REENT_ONLY
#if defined(CONFIG_UART_CLI_USE_SPINLOCK)
    hal_spin_lock_init(&io_lock);
#elif defined(CONFIG_UART_CLI_USE_MUTEX)
    hal_mutex_init(&io_mutex);
#endif
#endif
}
