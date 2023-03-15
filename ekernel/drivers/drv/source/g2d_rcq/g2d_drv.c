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