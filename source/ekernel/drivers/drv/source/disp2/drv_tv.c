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
