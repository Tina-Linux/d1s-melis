#include <stdio.h>
#include <rtthread.h>
#include <sunxi_drv_rtc.h>
#include <sunxi_hal_rtc.h>

struct rtc_time rtc_tm;
struct rtc_wkalrm wkalrm;

int callback(void)
{
    printf("alarm interrupt\n");
    return 0;
}

static int cmd_drv_rtc(int argc, const char **argv)
{
    rt_device_t fd;
    unsigned int enable = 0;

    fd = rt_device_find("rtc");
    if (fd == RT_NULL)
    {
        return -1;
    }

    rt_device_open(fd, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_STREAM);
    rt_device_control(fd, RTC_CALLBACK, callback);
    rt_device_control(fd, RTC_GET_TIME, &rtc_tm);

    wkalrm.enabled = 1;
    wkalrm.time = rtc_tm;
    rtc_tm.tm_min -= 1;

    printf("alarm time %04d-%02d-%02d %02d:%02d:%02d\n",
           wkalrm.time.tm_year + 1900, wkalrm.time.tm_mon + 1, wkalrm.time.tm_mday,
           wkalrm.time.tm_hour, wkalrm.time.tm_min, wkalrm.time.tm_sec);

    rt_device_control(fd, RTC_SET_TIME, &rtc_tm);
    rt_device_control(fd, RTC_SET_ALARM, &wkalrm);
    rt_device_control(fd, RTC_GET_ALARM, &wkalrm);

    rt_device_control(fd, RTC_GET_TIME, &rtc_tm);
    rt_device_control(fd, RTC_IRQENABLE, &enable);
    rt_device_close(fd);
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_drv_rtc, __cmd_drvrtc, rtthread rtc test code);

static int cmd_hal_rtc(int argc, const char **argv)
{
    unsigned int enable = 0;

    hal_rtc_init();

    hal_rtc_register_callback(callback);

    if (hal_rtc_gettime(&rtc_tm))
    {
        printf("sunxi rtc gettime error\n");
    }

    wkalrm.enabled = 1;
    wkalrm.time = rtc_tm;
    if (rtc_tm.tm_min > 0)
        rtc_tm.tm_min -= 1;
    else
        wkalrm.time.tm_min += 1;

    printf("alarm time %04d-%02d-%02d %02d:%02d:%02d\n",
           wkalrm.time.tm_year + 1900, wkalrm.time.tm_mon + 1, wkalrm.time.tm_mday,
           wkalrm.time.tm_hour, wkalrm.time.tm_min, wkalrm.time.tm_sec);

    if (hal_rtc_settime(&rtc_tm))
    {
        printf("sunxi rtc settime error\n");
    }

    if (hal_rtc_setalarm(&wkalrm))
    {
        printf("sunxi rtc setalarm error\n");
    }

    if (hal_rtc_getalarm(&wkalrm))
    {
        printf("sunxi rtc getalarm error\n");
    }

    if (hal_rtc_gettime(&rtc_tm))
    {
        printf("sunxi rtc gettime error\n");
    }

    //if do hal_rtc_alarm_irq_enable and hal_rtc_uninit, alarm will not work
    hal_rtc_alarm_irq_enable(enable);

    hal_rtc_deinit();
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_hal_rtc, cmd_rtc, hal rtc test code);
