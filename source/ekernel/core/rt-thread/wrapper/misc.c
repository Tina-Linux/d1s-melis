#include <rtthread.h>
#include <rthw.h>
#include <sunxi_hal_rtc.h>

#ifdef CONFIG_DYNAMIC_LOG_LEVEL_SUPPORT
#ifndef CONFIG_DYNAMIC_LOG_DEFAULT_LEVEL
#define CONFIG_DYNAMIC_LOG_DEFAULT_LEVEL 5
#endif
static volatile int g_log_level = CONFIG_DYNAMIC_LOG_DEFAULT_LEVEL;
#endif

#ifdef CONFIG_LOG_LEVEL_STORAGE_RTC

static void save_log_level_to_stroage(int level)
{
    unsigned int rtc_log_level = 0;

    level += 1;
    rtc_log_level = hal_rtc_read_data(RTC_LOG_LEVEL_INDEX);
    rtc_log_level &= 0xfffffff0;
    rtc_log_level |= level & 0xf;
    hal_rtc_write_data(RTC_LOG_LEVEL_INDEX, rtc_log_level);
}

static int get_log_level_from_stroage(void)
{
    unsigned int rtc_log_level = 0;

    rtc_log_level = hal_rtc_read_data(RTC_LOG_LEVEL_INDEX);
    if (rtc_log_level & 0xf)
    {
        g_log_level = (rtc_log_level & 0xf) - 1;
    }
    return g_log_level;
}
#elif defined(CONFIG_LOG_LEVEL_STORAGE_NONE)
static void save_log_level_to_stroage(int level)
{
    g_log_level = level;
}
static int get_log_level_from_stroage(void)
{
    return g_log_level;
}
#endif

/*
 * rtc register 5: 0-3bit for log level
 * rtc log level value : 0, rtc default value, no output log level
 * rtc log level value : 1, system log level is 0
 * rtc log level value : 2, system log level is 1
 */
#ifdef CONFIG_DYNAMIC_LOG_LEVEL_SUPPORT
int get_log_level(void)
{
    int level;
    register rt_base_t temp;

    temp = rt_hw_interrupt_disable();

    level = get_log_level_from_stroage();

    rt_hw_interrupt_enable(temp);
    return level;
}

void set_log_level(int level)
{
    register rt_base_t temp;

    temp = rt_hw_interrupt_disable();

    save_log_level_to_stroage(level);
    g_log_level = level;

    rt_hw_interrupt_enable(temp);
}
#endif

