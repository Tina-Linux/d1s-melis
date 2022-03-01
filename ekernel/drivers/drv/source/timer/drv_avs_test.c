#include <log.h>
#include <rtthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sunxi_drv_avs.h>

static int cmd_drv_avs(int argc, const char **argv)
{
    rt_device_t fd;
    fd = rt_device_find("avs0");
    if (fd == RT_NULL)
	return -1;
    rt_device_open(fd, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_STREAM);
    rt_device_control(fd, AVS_ENABLE, NULL);
    rt_device_control(fd, AVS_PAUSE, NULL);
    rt_device_control(fd, AVS_CONTINUE, NULL);
    rt_device_close(fd);
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_drv_avs, __cmd_drvavs, rtthread avs test code);
