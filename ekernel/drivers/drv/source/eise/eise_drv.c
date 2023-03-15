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
#include <sunxi_hal_eise.h>
#include <sunxi_drv_eise.h>
#include <aw_list.h>
#include <rtthread.h>
#include <typedef.h>
#include <log.h>
#include <init.h>

// bring hal layer in.
extern const sunxi_hal_driver_eise_t sunxi_hal_eise_driver;
static sunxi_driver_eise_t eise_drv;

static rt_err_t sunxi_eise_init(struct rt_device *dev)
{
    rt_size_t ret = 0;
    sunxi_driver_eise_t *pusr, *pvfy;
    sunxi_hal_driver_eise_t *hal_drv = NULL;;

    if (dev == NULL)
    {
        while (1);
    }
    pvfy = container_of(dev, sunxi_driver_eise_t, base);
    pusr = (sunxi_driver_eise_t *)dev->user_data;

    hal_assert(pvfy == pusr);

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_eise_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->initialize)
    {
        ret = hal_drv->initialize(pusr->dev_id);
    }
    return ret;
}

static rt_err_t sunxi_eise_open(struct rt_device *dev, rt_uint16_t oflag)
{
    return 0;
}

static rt_err_t sunxi_eise_close(struct rt_device *dev)
{
    rt_size_t ret = 0;
    sunxi_driver_eise_t *pusr = NULL, *pvfy = NULL;
    sunxi_hal_driver_eise_t *hal_drv = NULL;
    if (dev == NULL)
    {
        while (1);
    }

    pvfy = container_of(dev, sunxi_driver_eise_t, base);
    pusr = (sunxi_driver_eise_t *)dev->user_data;

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_eise_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->uninitialize)
    {
        ret = hal_drv->uninitialize(pusr->dev_id);
    }
    return ret;
}

static rt_size_t sunxi_eise_read(struct rt_device *dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    rt_size_t ret = 0;
    sunxi_driver_eise_t *pusr, *pvfy;
    sunxi_hal_driver_eise_t *hal_drv = NULL;

    if (dev == NULL)
    {
        while (1);
    }

    pvfy = container_of(dev, sunxi_driver_eise_t, base);
    pusr = (sunxi_driver_eise_t *)dev->user_data;

    hal_assert(pvfy == pusr);

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_eise_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->send)
    {
        ret = hal_drv->receive(pusr->dev_id, buffer, size);
    }

    return ret;
}

static rt_size_t sunxi_eise_write(struct rt_device *dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    rt_size_t ret = 0;

    sunxi_driver_eise_t *pusr, *pvfy;
    sunxi_hal_driver_eise_t *hal_drv = NULL;

    if (dev == NULL)
    {
        while (1);
    }

    pvfy = container_of(dev, sunxi_driver_eise_t, base);
    pusr = (sunxi_driver_eise_t *)dev->user_data;

    hal_assert(pvfy == pusr);

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_eise_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->send)
    {
        ret = hal_drv->send(pusr->dev_id, buffer, size);
    }

    return ret;
}

static rt_err_t sunxi_eise_control(struct rt_device *dev, int cmd, void *args)
{
    rt_size_t ret = 0;
    sunxi_driver_eise_t *pusr = NULL, *pvfy = NULL;
    sunxi_hal_driver_eise_t *hal_drv = NULL;
    if (dev == NULL)
    {
        while (1);
    }

    pvfy = container_of(dev, sunxi_driver_eise_t, base);
    pusr = (sunxi_driver_eise_t *)dev->user_data;

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_eise_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->control)
    {
        ret = hal_drv->control(pusr->dev_id, cmd, args);
    }

    return ret;
}

//#if defined(RT_USING_POSIX)
#if 0
#include <dfs_file.h>
#include <rtdevice.h>
#include <dfs_poll.h>
#include <string.h>

#define CHECK_POLL_OPS_FUNC_IS_VALAID(drv, func) \
    drv && drv->poll_ops && drv->poll_ops->func

static int eise_wakeup_poll_waitqueue(int32_t dev_id, short key)
{
    rt_device_t dev;
    char dev_name [RT_NAME_MAX];

    memset(dev_name, 0, RT_NAME_MAX);
    sprintf(dev_name, "eise%ld", dev_id);
    dev = rt_device_find(dev_name);
    if (!dev)
    {
        return -EINVAL;
    }

    rt_wqueue_wakeup(&dev->wait_queue, key);
    return 0;
}

static int sunxi_eise_fops_poll(struct dfs_fd *fd, struct rt_pollreq *req)
{
    printf("sunxi_eise_fops_poll\n");
    rt_device_t dev;
    int mask = 0;
    short key;
    int ret = -1;
    sunxi_driver_eise_t *pusr, *pvfy;
    sunxi_hal_driver_eise_t *hal_drv;

    dev = (rt_device_t)fd->data;
    if (!dev)
    {
        return mask;
    }

    pvfy = container_of(dev, sunxi_driver_eise_t, base);
    pusr = (sunxi_driver_eise_t *)dev->user_data;

    hal_assert(pvfy == pusr);

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_eise_t *)pusr->hal_drv;
    }

    key = req->_key;

    if (CHECK_POLL_OPS_FUNC_IS_VALAID(hal_drv, check_poll_state))
    {
        mask = hal_drv->poll_ops->check_poll_state(pusr->dev_id, key);
    }

    if (!mask && req->_proc != NULL)
    {
        if (CHECK_POLL_OPS_FUNC_IS_VALAID(hal_drv, register_poll_wakeup))
        {
            hal_drv->poll_ops->register_poll_wakeup(eise_wakeup_poll_waitqueue);
        }
        rt_poll_add(&dev->wait_queue, req);
    }
    /*
     * TODO:
     * Maybe it need to unregister poll_wakeup function, but need to take care of
     * select the same fd as the same time
     * */
    return mask;
}

static int sunxi_eise_fops_open(struct dfs_fd *fd)
{
    printf("sunxi_eise_fops_open\n");
    rt_device_t dev;
    dev = (rt_device_t) fd->data;

    return sunxi_eise_open(dev, 0);
}

static int sunxi_eise_fops_close(struct dfs_fd *fd)
{
    rt_device_t dev;
    dev = (rt_device_t) fd->data;

    return sunxi_eise_close(dev);
}

static int sunxi_eise_fops_ioctl(struct dfs_fd *fd, int cmd, void *args)
{
    rt_device_t dev;
    dev = (rt_device_t) fd->data;
    return sunxi_eise_control(dev, cmd, args);
}

static int sunxi_eise_fops_read(struct dfs_fd *fd, void *buf, size_t count)
{
    rt_device_t dev;
    dev = (rt_device_t) fd->data;

    return sunxi_eise_read(dev, fd->pos, buf, count);
}

static int sunxi_eise_fops_write(struct dfs_fd *fd, void *buf, size_t count)
{
    rt_device_t dev;
    dev = (rt_device_t) fd->data;

    return sunxi_eise_write(dev, fd->pos, buf, count);
}

static int sunxi_eise_fops_flush(struct dfs_fd *fd)
{
    return 0;
}

static int sunxi_eise_fops_lseek(struct dfs_fd *fd, off_t offset)
{
    return 0;
}

static int sunxi_eise_fops_getdents(struct dfs_fd *fd, struct dirent *dirp, uint32_t count)
{
    return 0;
}



static struct dfs_file_ops eise_device_fops =
{
    .open = sunxi_eise_fops_open,
    .close = sunxi_eise_fops_close,
    .flush = sunxi_eise_fops_flush,
    .ioctl = sunxi_eise_fops_ioctl,
    .lseek = sunxi_eise_fops_lseek,
    .read = sunxi_eise_fops_read,
    .write = sunxi_eise_fops_write,
    .getdents = sunxi_eise_fops_getdents,
    .poll = sunxi_eise_fops_poll,
};

#endif

static void init_eise_device(struct rt_device *dev, void *usr_data, char *dev_name)
{
    rt_err_t ret;
    dev->init        = sunxi_eise_init;
    dev->open        = sunxi_eise_open;
    dev->close       = sunxi_eise_close;
    dev->read        = sunxi_eise_read;
    dev->write       = sunxi_eise_write;
    dev->control     = sunxi_eise_control;
    dev->user_data   = usr_data;
    ret = rt_device_register(dev, dev_name, RT_DEVICE_FLAG_RDWR);
    if (ret != 0)
    {
        __err("fatal error, ret %d, register device to rtsystem failure.\n", ret);
        while (1);
        return;
    }
#if 0
//#if defined(RT_USING_POSIX)
    dev->fops = &eise_device_fops;
#endif
}

int sunxi_driver_eise_init(void)
{
    struct rt_device *device;

    device = &eise_drv.base;
    eise_drv.dev_id = 0;
    eise_drv.hal_drv = &sunxi_hal_eise_driver;
    //__log("sunxi_driver_eise_init");
    init_eise_device(device, &eise_drv, "sunxi_eise");
    return 0;
}

late_initcall(sunxi_driver_eise_init);