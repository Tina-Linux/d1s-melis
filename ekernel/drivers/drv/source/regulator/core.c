/*
 * Copyright (c) 2020 Allwinner Technology Co., Ltd. ALL rights reserved.
 */

#include <init.h>
#include <log.h>
#include <rtdef.h>
#include <rtthread.h>
#include <stdint.h>
#include <stdio.h>
#include <aw_list.h>

static void regulator_hal_init(void)
{

}

static rt_err_t regulator_control(struct rt_device *dev, int cmd, void *args)
{
	return 0;
}

static struct rt_device regulator = {
	.control = regulator_control,
};

int regulator_core_init(void)
{
	struct rt_device *dev = &regulator;
	rt_err_t ret;

	regulator_hal_init();

	ret = rt_device_register(dev, "regulator", RT_DEVICE_FLAG_RDWR);
	if (ret != RT_EOK)
		rt_device_destroy(dev);

	return ret;
}

subsys_initcall(regulator_core_init);
