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
#include <rtthread.h>
#include <log.h>

extern int tv_probe(void);
extern int tv_ioctl(int cmd, void *arg);
extern int tv_release(void);
extern int tv_open(void);

static rt_err_t tv_init_wrap(rt_device_t dev)
{
	return 0;
}

static rt_err_t tv_open_wrap(rt_device_t dev, rt_uint16_t oflag)
{
	return tv_open();
}

static rt_err_t tv_release_wrap(rt_device_t dev)
{
	return tv_release();
}

static rt_err_t tv_ioctl_wrap(rt_device_t dev, int cmd, void *arg)
{
	return tv_ioctl(cmd, arg);
}
struct rt_device tv_disp_engine = {
	.init = tv_init_wrap,
	.open = tv_open_wrap,
	.close = tv_release_wrap,
	.control = tv_ioctl_wrap
};

int tv_drv_module_init(void)
{
	int ret = -1;

	ret = rt_device_register(&tv_disp_engine, "tv", RT_DEVICE_FLAG_RDWR);
	if (ret) {
		__err("Faile to register disp device");
		goto OUT;
	}
	ret = tv_probe();
OUT:
	return ret;
}

late_initcall(tv_drv_module_init);
