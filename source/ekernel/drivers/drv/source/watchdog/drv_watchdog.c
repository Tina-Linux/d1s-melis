/*
 * Copyright 2018 Rockchip Electronics Co., Ltd
 *
 */
#include <init.h>
#include <stdio.h>
#include <sunxi_hal_watchdog.h>
#include <sunxi_drv_watchdog.h>
#include <standby/standby.h>

void drv_watchdog_info()
{
    hal_watchdog_info();
}

void drv_watchdog_disable(void)
{
    hal_watchdog_disable();
}

void drv_watchdog_reset(int timeout)
{
    hal_watchdog_reset(timeout);
}

void reset_cpu(void)
{
    hal_watchdog_restart();
}

void drv_watchdog_feed(void)
{
    hal_watchdog_feed();
}

int drv_watchdog_suspend(void *timeout)
{
	hal_watchdog_suspend(3);

	return 0;
}

int drv_watchdog_resume(void *timeout)
{
	hal_watchdog_resume(3);

	return 0;
}

int sunxi_wdt_init(void)
{
    hal_watchdog_init();
#ifdef CONFIG_STANDBY
    register_pm_dev_notify(drv_watchdog_suspend, drv_watchdog_resume, NULL);
#endif
    return 0;
}

late_initcall(sunxi_wdt_init);
