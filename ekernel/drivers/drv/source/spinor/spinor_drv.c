#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <rtthread.h>
#include <init.h>
#include <sunxi_drv_spinor.h>
#include <sunxi_hal_spinor.h>
#include <blkpart.h>
#include <kapi.h>

static rt_err_t sunxi_spinor_init(rt_device_t dev)
{
    sunxi_hal_driver_spinor_t *spinor_driver;

    rt_err_t ret = -1;
    if (!dev)
    {
        return -EINVAL;
    }

    spinor_driver = (sunxi_hal_driver_spinor_t *)(dev->user_data);
    if (spinor_driver && spinor_driver->initialize)
    {
        ret = spinor_driver->initialize(0);
    }
    return ret;
}

static rt_size_t sunxi_spinor_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    sunxi_hal_driver_spinor_t *spinor_driver;
    rt_err_t ret = -1;

    if (!dev)
    {
        return -EINVAL;
    }

    spinor_driver = (sunxi_hal_driver_spinor_t *)(dev->user_data);


    if (spinor_driver && spinor_driver->read_data)
    {
        ret = spinor_driver->read_data(pos, buffer, size);
    }

    return ret ? ret : size;
}

static rt_err_t sunxi_spinor_open(rt_device_t dev, rt_uint16_t oflag)
{
    return 0;
}

static rt_err_t sunxi_spinor_close(rt_device_t dev)
{
    return 0;
}

static rt_size_t sunxi_spinor_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    int ret = -1;
    sunxi_hal_driver_spinor_t *spinor_driver;

    if (!dev)
    {
        return -EINVAL;
    }

    spinor_driver = (sunxi_hal_driver_spinor_t *)(dev->user_data);

    if (spinor_driver && spinor_driver->program_data)
    {
        ret = spinor_driver->program_data(pos, buffer, size);
    }

    return ret ? ret : size;
}

static rt_err_t sunxi_spinor_control(rt_device_t dev, int cmd, void *args)
{
    int ret = 0;
    blk_dev_erase_t *erase_sector;
    sunxi_hal_spinor_info *spinor_info;
    struct rt_device_blk_geometry *geometry;
    sunxi_hal_driver_spinor_t *spinor_driver;

    if (!dev)
    {
        return -EINVAL;
    }

    spinor_driver = (sunxi_hal_driver_spinor_t *)(dev->user_data);
    if (!spinor_driver)
    {
        return ret;
    }

    switch (cmd)
    {
        case BLOCK_DEVICE_CMD_ERASE_ALL:
            if (spinor_driver->erase_chip)
            {
                ret = spinor_driver->erase_chip();
            }
            break;
        case BLOCK_DEVICE_CMD_ERASE_SECTOR:
            erase_sector = (blk_dev_erase_t *)(args);

            if (spinor_driver->erase_sector)
            {
                ret = spinor_driver->erase_sector(erase_sector->addr, erase_sector->len);
            }
            break;
        case BLOCK_DEVICE_CMD_GET_TOTAL_SIZE:
            if (spinor_driver->get_info)
            {
                spinor_info = spinor_driver->get_info();
                if (spinor_info)
                {
                    *(uint32_t *)args = spinor_info->sector_size * spinor_info->sector_count;
                }
                else
                {
                    ret = -EIO;
                    *(uint32_t *)args = 0;
                }
            }
            break;
        case BLOCK_DEVICE_CMD_GET_PAGE_SIZE:
            if (spinor_driver->get_info)
            {
                spinor_info = spinor_driver->get_info();
                if (spinor_info)
                {
                    *(uint32_t *)args = spinor_info->page_size;
                }
                else
                {
                    ret = -EIO;
                    *(uint32_t *)args = 0;
                }
            }
            break;
        case BLOCK_DEVICE_CMD_GET_BLOCK_SIZE:
            if (spinor_driver->get_info)
            {
                spinor_info = spinor_driver->get_info();
                if (spinor_info)
                {
                    *(uint32_t *)args = spinor_info->sector_size;
                }
                else
                {
                    ret = -EIO;
                    *(uint32_t *)args = 0;
                }
            }
            break;
        case RT_DEVICE_CTRL_BLK_GETGEOME:
            geometry = (struct rt_device_blk_geometry *)args;
            memset(geometry, 0, sizeof(struct rt_device_blk_geometry));
            if (spinor_driver->get_info)
            {
                spinor_info = spinor_driver->get_info();
                if (spinor_info)
                {
                    geometry->block_size = spinor_info->sector_size;
                    geometry->bytes_per_sector = 512;
                    geometry->sector_count = (spinor_info->sector_count * spinor_info->sector_size) / geometry->bytes_per_sector;
                }
                else
                {
                    ret = -EIO;
                }
            }
            break;

        default:
            break;
    }

    return ret;
}

#if defined(RT_USING_POSIX)
#include <dfs_file.h>
#include <rtdevice.h>

static int sunxi_spinor_fops_open(struct dfs_fd *fd)
{
    rt_device_t dev;
    uint32_t total_bytes = 0;

    dev = (rt_device_t) fd->data;
    dev->control(dev, BLOCK_DEVICE_CMD_GET_TOTAL_SIZE, &total_bytes);

    fd->size = total_bytes;
    fd->type  = FT_DEVICE;

    return sunxi_spinor_open(dev, 0);
}

static int sunxi_spinor_fops_close(struct dfs_fd *fd)
{
    rt_device_t dev;
    dev = (rt_device_t) fd->data;

    return sunxi_spinor_close(dev);
}

static int sunxi_spinor_fops_ioctl(struct dfs_fd *fd, int cmd, void *args)
{
    rt_device_t dev;
    dev = (rt_device_t) fd->data;

    return sunxi_spinor_control(dev, cmd, args);
}

static int sunxi_spinor_fops_read(struct dfs_fd *fd, void *buf, size_t count)
{
    rt_device_t dev;
    int ret = -1;
    dev = (rt_device_t) fd->data;

    ret = dev->read(dev, fd->pos, buf, count);
    if (ret >= 0)
    {
        fd->pos += ret;
    }

    return ret;
}

static int sunxi_spinor_fops_write(struct dfs_fd *fd, const void *buf, size_t count)
{
    rt_device_t dev;
    int ret = -1;
    dev = (rt_device_t) fd->data;

    ret = dev->write(dev, fd->pos, buf, count);
    if (ret >= 0)
    {
        fd->pos += ret;
    }

    return ret;
}

static int sunxi_spinor_fops_lseek(struct dfs_fd *fd, off_t offset)
{
    int ret = -1;

    if (fd->flags & O_DIRECTORY)
    {
        ret = -1;
    }
    else
    {
        if (offset > fd->size)
        {
            ret = -1;
        }
        else
        {
            fd->pos = offset;
            ret = fd->pos;
        }
    }
    return ret;
}

static struct dfs_file_ops spinor_device_fops =
{
    .open = sunxi_spinor_fops_open,
    .close = sunxi_spinor_fops_close,
    .ioctl = sunxi_spinor_fops_ioctl,
    .lseek = sunxi_spinor_fops_lseek,
    .read = sunxi_spinor_fops_read,
    .write = sunxi_spinor_fops_write,
};
#endif

static const sunxi_hal_driver_spinor_t sunxi_hal_spinor_driver =
{
    .get_version  = hal_spinor_get_version,
    .get_capabilities = hal_spinor_get_capabilities,
    .initialize = hal_spinor_init,
    .uninitialize = hal_spinor_deinit,
    .power_control = hal_spinor_power_control,
    .read_data = hal_spinor_read_data,
    .program_data = hal_spinor_program_data,
    .erase_sector = hal_spinor_erase_sector,
    .erase_chip = hal_spinor_erase_chip,
    .get_status = hal_spinor_get_status,
    .get_info = hal_spinor_get_info,
    .signal_event = hal_spinor_signal_event,
    .control = hal_spinor_control,
};

int sunxi_driver_spinor_deinit(void)
{
    int ret = -1;
    struct rt_device *device;
    sunxi_hal_driver_spinor_t *spinor_driver;

    device = rt_device_find("spinor");
    if (device == NULL)
    {
        return ret;
    }

    spinor_driver = (sunxi_hal_driver_spinor_t *)(device->user_data);
    if (spinor_driver && spinor_driver->uninitialize)
    {
        ret = spinor_driver->uninitialize();
    }

    return ret;
}

int sunxi_driver_spinor_init(void)
{
    struct rt_device *device;
    int ret = -1, value = 0;

    ret = esCFG_GetKeyValue("target", "storage_type", (int32_t *)&value, 1);
	if (value != STORAGE_NOR)
	{
		printk("value:%d, ret:%d\n", value, ret);
		printk("not boot from spinor!\n");
		value = -1;
		ret = esCFG_GetKeyValue("card_product", "card_product_used", (int32_t *)&value, 1);
		if (ret != 0 || value == 0)
		{
			printk("%s %d not card product!\n", __FILE__, __LINE__);
			return -1;
		} else if (value) {
			ret = esCFG_GetKeyValue("card_product", "card_product_storage", (int32_t *)&value, 1);
			if (value != STORAGE_NOR) {
				printk("value:%d, ret:%d\n", value, ret);
				printk("card product but target not spinor, will return!\n");
				return -1;
			}
		}
	}

    device = rt_device_create(RT_Device_Class_Block, 0);
    if (!device)
    {
        return ret;
    }

    device->init = sunxi_spinor_init;
    device->open = sunxi_spinor_open;
    device->close = sunxi_spinor_close;
    device->read = sunxi_spinor_read;
    device->write = sunxi_spinor_write;
    device->control = sunxi_spinor_control;
    device->user_data = (void *)&sunxi_hal_spinor_driver;

    ret = rt_device_register(device, "spinor", RT_DEVICE_FLAG_RDWR);
    if (ret != RT_EOK)
    {
        rt_device_destroy(device);
        return ret;
    }

#if defined(RT_USING_POSIX)
    device->fops = &spinor_device_fops;
#endif

    int nor_blkpart_init(const char *name);
    ret = nor_blkpart_init("spinor");
    return ret;
}
device_initcall(sunxi_driver_spinor_init);
