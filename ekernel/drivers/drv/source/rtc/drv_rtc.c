/*
 * Copyright 2018 Rockchip Electronics Co., Ltd
 *
 */
#include <sunxi_hal_rtc.h>
#include <sunxi_drv_rtc.h>

#include <rtthread.h>
#include <log.h>
#include <init.h>
const sunxi_hal_driver_rtc_t sunxi_hal_rtc_driver;
static sunxi_driver_rtc_t rtc;

static rt_err_t rtc_open(struct rt_device *dev, rt_uint16_t oflag)
{
    rt_size_t ret = 0;
    sunxi_driver_rtc_t *pusr, *pvfy;
    sunxi_hal_driver_rtc_t *hal_drv = NULL;

    if (dev == NULL)
    {
        while (1);
    }

    pvfy = rt_container_of(dev, sunxi_driver_rtc_t, base);
    pusr = (sunxi_driver_rtc_t *)dev->user_data;

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_rtc_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->initialize)
    {
        ret = hal_drv->initialize();
    }

    return ret;
}

static rt_err_t rtc_close(struct rt_device *dev)
{
    rt_size_t ret = 0;
    sunxi_driver_rtc_t *pusr = NULL, *pvfy = NULL;
    sunxi_hal_driver_rtc_t *hal_drv = NULL;

    if (dev == NULL)
    {
        while (1);
    }

    pvfy = rt_container_of(dev, sunxi_driver_rtc_t, base);
    pusr = (sunxi_driver_rtc_t *)dev->user_data;

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_rtc_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->uninitialize)
    {
        ret = hal_drv->uninitialize();
    }

    return ret;
}

static rt_err_t rtc_init(struct rt_device *dev)
{
    int ret;
#if defined(CONFIG_SOC_SUN20IW1)
    ret = hal_rtc_init();
    if (ret)
    {
	__err("fatal error, ret %d, init rtc hal failure.\n", ret);
	return -1;
    }
#endif
    return 0;
}

static rt_size_t rtc_write(struct rt_device *dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
	return 0;
}

static rt_size_t rtc_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
	return 0;
}

static rt_err_t rtc_control(struct rt_device *dev, int cmd, void *args)
{
    rt_size_t ret = 0;
    sunxi_driver_rtc_t *pusr = NULL, *pvfy = NULL;
    sunxi_hal_driver_rtc_t *hal_drv = NULL;

    if (dev == NULL)
    {
        while (1);
    }

    pvfy = rt_container_of(dev, sunxi_driver_rtc_t, base);
    pusr = (sunxi_driver_rtc_t *)dev->user_data;

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_rtc_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->control)
    {
        ret = hal_drv->control(cmd, args);
    }

    return ret;
}

static void init_rtc_device(struct rt_device *dev, void *usr_data, char *dev_name)
{
    rt_err_t ret;

    dev->open       = rtc_open;
    dev->close      = rtc_close;
    dev->init       = rtc_init;
    dev->write      = rtc_write;
    dev->read       = rtc_read;
    dev->control    = rtc_control;
    dev->user_data  = usr_data;

    ret = rt_device_register(dev, dev_name, RT_DEVICE_FLAG_RDWR);
    if (ret != 0)
    {
        __err("fatal error, ret %d, register device to rtsystem failure.\n", ret);
        while (1);
        return;
    }
}

int hal_rtc_control(hal_rtc_transfer_cmd_t cmd, void *args)
{
    struct rtc_time *hal_rtc_time;
    struct rtc_wkalrm *hal_rtc_wkalarm;
    rtc_callback_t user_callback;
    unsigned int *enabled;

    switch (cmd)
    {
#ifdef CONFIG_ARM
        case RTC_GET_TIME:
            hal_rtc_time = (struct rtc_time *)args;
            return hal_rtc_gettime(hal_rtc_time);
        case RTC_SET_TIME:
            hal_rtc_time = (struct rtc_time *)args;
            return hal_rtc_settime(hal_rtc_time);
        case RTC_GET_ALARM:
            hal_rtc_wkalarm = (struct rtc_wkalrm *)args;
            return hal_rtc_getalarm(hal_rtc_wkalarm);
        case RTC_SET_ALARM:
            hal_rtc_wkalarm = (struct rtc_wkalrm *)args;
            return hal_rtc_setalarm(hal_rtc_wkalarm);
        case RTC_CALLBACK:
            user_callback = (rtc_callback_t)args;
            return hal_rtc_register_callback(user_callback);
        case RTC_IRQENABLE:
            enabled = (unsigned int *)args;
            return hal_rtc_alarm_irq_enable(*enabled);
#endif
        default:
            return -1;
    }
}

const sunxi_hal_driver_rtc_t sunxi_hal_rtc_driver =
{
    .initialize     = hal_rtc_init,
    .uninitialize   = hal_rtc_deinit,
    .control        = hal_rtc_control,
};

int sunxi_driver_rtc_init(void)
{
    struct rt_device *device;

    device = &rtc.base;
    rtc.dev_id = 0;
    rtc.hal_drv = &sunxi_hal_rtc_driver;

    init_rtc_device(device, &rtc, "rtc");

    return 0;
}

subsys_initcall(sunxi_driver_rtc_init);



