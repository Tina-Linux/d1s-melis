/*
 * g2d_drv.c
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 *
 *
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <init.h>
#include <stdlib.h>
#include <string.h>
#include <rtthread.h>
extern int sunxi_g2d_control(int cmd, void *arg);
extern int sunxi_g2d_close(void);
extern int sunxi_g2d_open(void);
extern int g2d_probe(void);

static rt_err_t sunxi_g2d_init(rt_device_t dev)
{
	return 0;
}

static rt_err_t sunxi_g2d_open_wrap(rt_device_t dev, rt_uint16_t oflag)

{
	return sunxi_g2d_open(); 
}


static rt_err_t sunxi_g2d_close_wrap(rt_device_t dev)
{
	return sunxi_g2d_close();
}

static rt_err_t sunxi_g2d_control_wrap(rt_device_t dev, int cmd, void *args)
{
	return sunxi_g2d_control(cmd, args);
}


static int g2d_probe_wrap(void)
{
	int ret = -1;
	rt_device_t device;

	device = rt_device_create(RT_Device_Class_Graphic, 0);
	if (!device) {
		return ret;
	}
	memset(device, 0, sizeof(struct rt_device));
	

	device->init = sunxi_g2d_init;
	device->open = sunxi_g2d_open_wrap;
	device->close = sunxi_g2d_close_wrap;
	device->control = sunxi_g2d_control_wrap;

	ret = rt_device_register(device, "g2d", RT_DEVICE_FLAG_RDWR);
	if (ret != 0) {
		return ret;
	}

	return g2d_probe();
}

device_initcall(g2d_probe_wrap);
