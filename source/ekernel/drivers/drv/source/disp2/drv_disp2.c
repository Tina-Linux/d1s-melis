#include <init.h>
#include <rtthread.h>
#include <log.h>
extern int disp_probe(void);
extern int disp_ioctl(int cmd, void *arg);
extern int disp_release(void);
extern int disp_open(void);

static rt_err_t disp_init_wrap(rt_device_t dev)
{
	return 0;
}

static rt_err_t disp_open_wrap(rt_device_t dev, rt_uint16_t oflag)
{
	return disp_open();
}

static rt_err_t disp_release_wrap(rt_device_t dev)
{
	return disp_release();
}

static rt_err_t disp_ioctl_wrap(rt_device_t dev, int cmd, void *arg)
{
	return disp_ioctl(cmd, arg);
}
struct rt_device disp_engine = {
	.init = disp_init_wrap,
	.open = disp_open_wrap,
	.close = disp_release_wrap,
	.control = disp_ioctl_wrap
};

int disp_module_init(void)
{
	int ret = -1;

	ret = rt_device_register(&disp_engine, "disp", RT_DEVICE_FLAG_RDWR);
	if (ret) {
		__err("Faile to register disp device");
		goto OUT;
	}
	ret = disp_probe();
OUT:
	return ret;
}

device_initcall(disp_module_init);
