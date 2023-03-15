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
