#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <rtthread.h>
#include <hal_cfg.h>
#include <init.h>
#include <sunxi_hal_thermal.h>
#include <sunxi_hal_thermal.h>
#include <sunxi_drv_thermal.h>

static temperature_control_strategy_t temperature_control_strategy = TEMPERATURE_CONTROL_NONE;
static rt_err_t sunxi_thermal_init(rt_device_t dev)
{
    return 0;
}
static rt_err_t sunxi_thermal_open(rt_device_t dev, rt_uint16_t oflag)
{
     return 0;
}

static rt_err_t sunxi_thermal_close(rt_device_t dev)
{
    return 0;
}

static rt_size_t sunxi_thermal_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
	hal_ths_get_temp(0, buffer);
    return 0;
}
static rt_size_t sunxi_thermal_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{

    return 0;
}
static rt_err_t sunxi_thermal_control(rt_device_t dev, int cmd, void *args)
{
    int ret = 0;
	
//	printf("%s %d %s cmd:%d\n", __FILE__, __LINE__, __func__, cmd);
    switch (cmd)
    {
        case THERMAL_GET_STRATEGY:
//			printf("%s %d %s *args:%d\n", __FILE__, __LINE__, __func__, temperature_control_strategy);
			*((temperature_control_strategy_t *)args) = temperature_control_strategy;
        break;
		case THERMAL_SET_STRATEGY:
			temperature_control_strategy = *((temperature_control_strategy_t *)args);
//			printf("%s %d %s *args:%d\n", __FILE__, __LINE__, __func__, temperature_control_strategy);
		default:
			break;
    }

//	printf("%s %d %s *args:%d\n", __FILE__, __LINE__, __func__, *((temperature_control_strategy_t *)args));
    return ret;
}

int sunxi_driver_thermal_init(void)
{
    int ret = 0;
    struct rt_device *device;
	hal_ths_init();
	
    device = rt_device_create(RT_Device_Class_Block, 0);
    if (!device)
    {
        return ret;
    }
    device->init = sunxi_thermal_init;
    device->open = sunxi_thermal_open;
    device->close = sunxi_thermal_close;
    device->read = sunxi_thermal_read;
    device->write = sunxi_thermal_write;
    device->control = sunxi_thermal_control;

    ret = rt_device_register(device, "thermal", RT_DEVICE_FLAG_RDWR);
    if (ret != RT_EOK)
    {
        rt_device_destroy(device);
    }
	return ret;
}

device_initcall(sunxi_driver_thermal_init);
