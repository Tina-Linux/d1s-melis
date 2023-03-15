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
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <typedef.h>
#include <kapi.h>
#include <hal_mem.h>
#include <sunxi_drv_spinor.h>
#include <rtthread.h>
#include <blkpart.h>
#include <part_efi.h>
#include <debug.h>
#include <sys_device.h>
#include <hal_timer.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
#endif

#define ALIGN_DOWN(x, a) __ALIGN_KERNEL((x) - ((a) - 1), (a))
#define __ALIGN_KERNEL(x, a) __ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

static struct blkpart norblk;

struct syspart
{
    char name[MAX_BLKNAME_LEN];
    u32 offset;
    u32 bytes;
};
static const struct syspart syspart[] =
{
    {"boot0", 0, 48 * 1024},
	{"gpt", 48 * 1024, 16 * 1024},
};

static struct part all_part[16];

static int nor_get_gpt(rt_device_t dev, char *buf, int len)
{
    int ret = 0;
    if (len < GPT_TABLE_SIZE)
    {
        pr_err("buf too small for gpt\n");
        return -EINVAL;
    }

    pr_debug("read gpt from 0x%x\n", GPT_ADDRESS);
    ret = dev->read(dev, GPT_ADDRESS, buf, GPT_TABLE_SIZE);
    return ret <= 0 ? -EIO : 0;
}

#if defined(RT_USING_POSIX)
#include <dfs_file.h>
#include <rtdevice.h>
#include <string.h>

static int part_fops_open(struct dfs_fd *fd)
{
    rt_device_t dev;
    uint32_t total_bytes = 0;

    dev = (rt_device_t) fd->data;
    dev->control(dev, DEVICE_PART_CMD_GET_TOTAL_SIZE, &total_bytes);

    fd->size = total_bytes;
    fd->type  = FT_DEVICE;

    return 0;
}

static int part_fops_close(struct dfs_fd *fd)
{
    return 0;
}

static int part_fops_ioctl(struct dfs_fd *fd, int cmd, void *args)
{
    rt_device_t dev;
    dev = (rt_device_t) fd->data;

    return dev->control(dev, cmd, args);
}

static int part_fops_read(struct dfs_fd *fd, void *buf, size_t count)
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

static int part_fops_write(struct dfs_fd *fd, const void *buf, size_t count)
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

static int part_fops_lseek(struct dfs_fd *fd, off_t offset)
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

static struct dfs_file_ops part_device_fops =
{
    .open = part_fops_open,
    .close = part_fops_close,
    .ioctl = part_fops_ioctl,
    .lseek = part_fops_lseek,
    .read = part_fops_read,
    .write = part_fops_write,
};
#endif

#ifdef CONFIG_MELIS_LEGACY_DRIVER_MAN
typedef struct __SPINOR_DEV
{
    __u8            used;
    __u32           offset;
    char            major_name[24];
    char            minor_name[24];
    __hdle          h_reg;
    __dev_blkinfo_t info;
    void           *device;
} __spinor_dev_t;

typedef struct tag_CRC32_DATA
{
	__u32 CRC;             
	__u32 CRC_32_Tbl[256];
} CRC32_DATA_t;

#define MAX_PART_COUNT 4

static __spinor_dev_t pndev[MAX_PART_COUNT];

static __u32 calc_crc32(const unsigned char *buffer, __u32 length)
{
	__u32 i, j;
	CRC32_DATA_t crc32;
	__u32 CRC32 = 0xffffffff;  /* 设置初始值 */
	crc32.CRC = 0;

	for ( i = 0; i < 256; ++i) /* 用++i以提高效率 */
	{
		crc32.CRC = i;
		for ( j = 0; j < 8 ; ++j)
		{
			if (crc32.CRC & 1)
				crc32.CRC = (crc32.CRC >> 1) ^ 0xEDB88320;
			else
				crc32.CRC >>= 1;
		}
		crc32.CRC_32_Tbl[i] = crc32.CRC;
	}

	CRC32 = 0xffffffff; /* 设置初始值 */
    for ( i = 0; i < length; ++i)
    {
        CRC32 = crc32.CRC_32_Tbl[(CRC32^((unsigned char*)buffer)[i]) & 0xff] ^ (CRC32 >> 8);
    }

	return CRC32^0xffffffff;
}

static __hdle spinor_dev_open(void *arg, __u32 mode)
{
    __s32          cpu_sr;
    __spinor_dev_t *pdev = (__spinor_dev_t *)arg;

    if (pdev->used)
    {
        __wrn("cannot open spinor, used by someone else\n");
        return 0;
    }

    pdev->device = rt_device_find(pdev->minor_name);
    if (rt_device_open(pdev->device, RT_DEVICE_FLAG_RDWR))
    {
        return NULL;
    }

    pdev->used = 1;
    return (__hdle)pdev;
}

static __s32 spinor_dev_close(__hdle hdev)
{
    __s32          cpu_sr;
    __spinor_dev_t *pdev = (__spinor_dev_t *)hdev;

    if (!pdev->used)
    {
        return EPDK_FAIL;
    }

    if (pdev->device)
    {
        rt_device_close(pdev->device);
    }
    pdev->used = 0;
    return EPDK_OK;
}

static __u32 spinor_dev_read(void *pbuffer, __u32 block, __u32 n, __hdle hdev)
{
    __u8           err;
    __spinor_dev_t *pndev = (__spinor_dev_t *)hdev;

    if (block >= pndev->info.partsize)
    {
        __wrn("out of device capacity\n");
        goto exit;
    }

    if (block + n > pndev->info.partsize)
    {
        __wrn("out of device capacity\n");
        __wrn("pndev->major_name=%s\n", pndev->major_name);
        __wrn("pndev->minor_name=%s\n", pndev->minor_name);
        __wrn("pndev->offset=%d\n", pndev->offset);
        __wrn("pndev->info.partsize=%d\n", pndev->info.partsize);
        __wrn("block=%d\n", block);
        __wrn("n=%d\n", n);
        n = pndev->info.partsize - block;
    }


    if (rt_device_read(pndev->device, block * pndev->info.secsize, pbuffer, n * pndev->info.secsize) != (n * pndev->info.secsize))
    {
        goto exit;
    }

    return n;
exit:

    return 0;
}

static __u32 spinor_dev_write(const void *pbuffer, __u32 block, __u32 n, __hdle hdev)
{
    __u8           err;
    __spinor_dev_t *pndev = (__spinor_dev_t *)hdev;

    if (block >= pndev->info.partsize)
    {
        __err("out of device capacity, block = %d, partsize = %d\n", block, pndev->info.partsize);
        goto exit;
    }

    if (block + n > pndev->info.partsize)
    {
        __err("out of device capacity\n");
        __err("pndev->major_name=%s\n", pndev->major_name);
        __err("pndev->minor_name=%s\n", pndev->minor_name);
        __err("pndev->offset=%d\n", pndev->offset);
        __err("pndev->info.partsize=%d\n", pndev->info.partsize);
        __err("block=%d\n", block);
        __err("n=%d\n", n);
        n = pndev->info.partsize - block;
        goto exit;
    }

    if (rt_device_write(pndev->device, block * pndev->info.secsize, pbuffer, n * pndev->info.secsize) != (n * pndev->info.secsize))
    {
        goto exit;
    }

    return n;
exit:
    return 0;
}

static int32_t spinor_dev_ioctrl(__hdle hdev, uint32_t cmd, long aux, void *pbuffer)
{
    __spinor_dev_t *pndev = (__spinor_dev_t *)hdev;

    switch (cmd)
    {
        case DEV_CMD_GET_INFO:
        {
            if (!pbuffer)
            {
                return EPDK_FAIL;
            }

            *((__dev_blkinfo_t *)pbuffer) = pndev->info;
            return EPDK_OK;
        }

        case DEV_CMD_GET_STATUS:
            return EPDK_OK;

        case DEV_CMD_GET_OFFSET:
            *((__u32 *)pbuffer) = pndev->offset;
            return EPDK_OK;
        default:
            return EPDK_OK;
    }
}

static __dev_devop_t melis_spinor_dev_op =
{
    spinor_dev_open,
    spinor_dev_close,
    spinor_dev_read,
    spinor_dev_write,
    spinor_dev_ioctrl
};

static unsigned char *udisk_buff = NULL;
static unsigned char *udisk_buff_backup = NULL;

typedef struct udisk_sector_manager
{
	int sector;
	int drity;
	struct udisk_sector_manager *next;
} udisk_manager_t;

static udisk_manager_t sector_header;

static __s32 melis_udisk_sector_manager_init(__spinor_dev_t *pDev)
{
	u32 udisk_part_size = 0, index = 0, udisk_part_crc_size = 0;
	udisk_manager_t *next_node = &sector_header, *tmp = NULL;
	
	sector_header.drity = 0;
	sector_header.sector = 0;
	
	udisk_part_size = pDev->info.partsize*pDev->info.secsize;
	udisk_part_crc_size = norblk.blk_bytes;
	for (index = 1; index < (udisk_part_size + udisk_part_crc_size)/norblk.blk_bytes; index++)
	{
		tmp = hal_malloc(sizeof(udisk_manager_t));
		if (tmp == NULL)
		{
			__err("melis spinor udisk_part_check fail!\n");
			return -1;
		}
		memset(tmp, 0x00, sizeof(udisk_manager_t));
		tmp->next = NULL;
		tmp->sector = index;
		tmp->drity = 0;
		next_node->next = tmp;
		next_node = tmp;
	}
	return 0;
}

/*比较udisk和udisk backup中那些扇区数据不一致，设置为dirty*/
static __s32 melis_udisk_part_mark_dirty_data(__spinor_dev_t *pDev)
{
	u32 offset = 0;
	udisk_manager_t *next_node = &sector_header;
	
	while (next_node)
	{
		offset = next_node->sector*norblk.blk_bytes;
		if (memcmp(udisk_buff + offset, udisk_buff_backup + offset, norblk.blk_bytes) != 0)
		{
			next_node->drity = 1;
		}
		next_node = next_node->next;
	}
	return 0;
}
/*
将 udisk和udisk backup中数据不一致的扇区写回到flash中
pbuffer:数据源
offset:udisk 或者udisk backup的偏移
*/
static __s32 melis_udisk_part_flush_dirty_data(__spinor_dev_t *pDev, unsigned char *pbuffer, __u32 offset)
{
	rt_device_t dev;
	udisk_manager_t *next_node = &sector_header;
    __spinor_dev_t *pndev = (__spinor_dev_t *)pDev;
	blk_dev_erase_t erase_sector;	
	u32 write_sector_start = 0, flag = 0, write_sector_num = 0, udisk_crc_offset = 0;
	
	u32 total = 0;
	udisk_crc_offset = pDev->info.partsize*pDev->info.secsize;
	dev = rt_device_find("spinor");
	if (rt_device_open(dev, RT_DEVICE_FLAG_RDWR))
	{
		return -1;
	}
	while (next_node) {
		if (next_node->drity == 1)
		{
			if (flag == 0)
			{
				write_sector_start = next_node->sector;
				flag = 1;
			}			
			write_sector_num++;
			next_node->drity = 0;
		}
		else
		{
			if (flag == 1)
			{
				total += write_sector_num;
				erase_sector.len = write_sector_num*norblk.blk_bytes;
				erase_sector.addr = pDev->offset + write_sector_start*norblk.blk_bytes + offset;
				rt_device_control(dev, BLOCK_DEVICE_CMD_ERASE_SECTOR, &erase_sector);				
				rt_device_write(dev, erase_sector.addr, pbuffer + write_sector_start*norblk.blk_bytes, erase_sector.len);
				flag = 0;
				write_sector_start = 0;
				write_sector_num = 0;
			}
		}
		next_node = next_node->next;
	}
	/*如果最后的扇区为dirty，上面的操作可能没能正确flush，此处需要再次判断*/
	if (flag == 1)
	{
		total += write_sector_num;
		erase_sector.len = write_sector_num*norblk.blk_bytes;
		erase_sector.addr = pDev->offset + write_sector_start*norblk.blk_bytes + offset;
		rt_device_control(dev, BLOCK_DEVICE_CMD_ERASE_SECTOR, &erase_sector);
		rt_device_write(dev, erase_sector.addr, pbuffer + write_sector_start*norblk.blk_bytes, erase_sector.len);
		flag = 0;
		write_sector_start = 0;
		write_sector_num = 0;
	}
	/*CRC PART*/
	rt_device_close(dev);
	return 0;
}

static __s32 melis_udisk_part_check(__spinor_dev_t *pDev)
{
	rt_device_t dev = NULL;
	u32 udisk_part_size = 0, udisk_part_crc_size = 0, udisk_part_backup_offset = 0;
	u32 actual_crc = 0, actual_crc_backup = 0, crc32 = 0, crc32_backup = 0;
	blk_dev_erase_t erase_sector;	

	dev = rt_device_find("spinor");
	if (rt_device_open(dev, RT_DEVICE_FLAG_RDWR))
	{
		return -1;
	}
	udisk_part_size = pDev->info.partsize*pDev->info.secsize;
	udisk_part_crc_size = norblk.blk_bytes;
	udisk_part_backup_offset = udisk_part_size + udisk_part_crc_size;

	udisk_buff = hal_malloc(udisk_part_size + udisk_part_crc_size);
    if (udisk_buff == NULL)
    {
        __err("melis spinor udisk_part_check fail!\n");
		rt_device_close(dev);
        return -1;
    }
	udisk_buff_backup = hal_malloc(udisk_part_size + udisk_part_crc_size);
    if (udisk_buff_backup == NULL)
    {
        __err("melis spinor udisk_part_check fail!\n");
		hal_free(udisk_buff_backup);
		rt_device_close(dev);
        return -1;
    }

	melis_udisk_sector_manager_init(pDev);
	
	memset(udisk_buff, 0x00, udisk_part_size + udisk_part_crc_size);
	rt_device_read(dev, pDev->offset, udisk_buff, udisk_part_size + udisk_part_crc_size);
	memset(udisk_buff_backup, 0x00, udisk_part_size + udisk_part_crc_size);
	rt_device_read(dev, pDev->offset + udisk_part_backup_offset, udisk_buff_backup, udisk_part_size + udisk_part_crc_size);

	actual_crc = calc_crc32(udisk_buff, udisk_part_size);
	crc32 = *(__u32 *)(udisk_buff + udisk_part_size);		

	actual_crc_backup = calc_crc32(udisk_buff_backup, udisk_part_size);
	crc32_backup = *(__u32 *)(udisk_buff_backup + udisk_part_size);

	if (actual_crc != crc32 && actual_crc_backup == crc32_backup) 
	{
		melis_udisk_part_mark_dirty_data(pDev);
		melis_udisk_part_flush_dirty_data(pDev, udisk_buff_backup, 0);
		memcpy(udisk_buff, udisk_buff_backup, udisk_part_size + udisk_part_crc_size);
		__log("%s %d copy udisk_buff_backup\n", __func__, __LINE__);
	}
	else {		
		melis_udisk_part_mark_dirty_data(pDev);
		melis_udisk_part_flush_dirty_data(pDev, udisk_buff, udisk_part_backup_offset);
		memcpy(udisk_buff_backup, udisk_buff, udisk_part_size + udisk_part_crc_size);
		__log("%s %d copy udisk_buff\n", __func__, __LINE__);
	}
	rt_device_close(dev);
	return 0;
}
static __s32 melis_udisk_part_flush(__spinor_dev_t *pDev)
{
	blk_dev_erase_t erase_sector;	
	u32 actual_crc = 0, actual_crc_backup = 0;
	u32 udisk_part_size = 0, udisk_part_crc_size = 0, udisk_part_backup_offset = 0;

	rt_device_t dev;
	dev = rt_device_find("spinor");
	if (rt_device_open(dev, RT_DEVICE_FLAG_RDWR))
	{
		return -1;
	}

	udisk_part_size = pDev->info.partsize*pDev->info.secsize;
	udisk_part_crc_size = norblk.blk_bytes;
	udisk_part_backup_offset = udisk_part_size + udisk_part_crc_size;

	actual_crc = calc_crc32(udisk_buff, udisk_part_size);
	if (memcmp(udisk_buff,udisk_buff_backup,udisk_part_backup_offset))
	{
		*(__u32 *)(udisk_buff + udisk_part_size) = actual_crc;

		/*先写udisk part*/
		melis_udisk_part_mark_dirty_data(pDev);
		melis_udisk_part_flush_dirty_data(pDev, udisk_buff, 0);

		/*再写udisk backup part*/
		melis_udisk_part_mark_dirty_data(pDev);
		melis_udisk_part_flush_dirty_data(pDev, udisk_buff, udisk_part_backup_offset);
	}
	memcpy(udisk_buff_backup, udisk_buff, udisk_part_size + udisk_part_crc_size);
	rt_device_close(dev);
	return 0;
}
static __u32 udisk_dev_virtual_read(void *pbuffer, __u32 block, __u32 n, __hdle hdev)
{
    __u8           err;
    __spinor_dev_t *pndev = (__spinor_dev_t *)hdev;

    if (block >= pndev->info.partsize)
    {
        __wrn("out of device capacity\n");
        goto exit;
    }

    if (block + n > pndev->info.partsize)
    {
        __wrn("out of device capacity\n");
        __wrn("pndev->major_name=%s\n", pndev->major_name);
        __wrn("pndev->minor_name=%s\n", pndev->minor_name);
        __wrn("pndev->offset=%d\n", pndev->offset);
        __wrn("pndev->info.partsize=%d\n", pndev->info.partsize);
        __wrn("block=%d\n", block);
        __wrn("n=%d\n", n);
        n = pndev->info.partsize - block;
    }

	memcpy(pbuffer, udisk_buff + (block * pndev->info.secsize), (n * pndev->info.secsize));
    return n;
exit:

    return 0;
}
static uint32_t udisk_dev_virtual_write(const void *pbuffer, uint32_t block, uint32_t n, __hdle hdev)
{
    __spinor_dev_t *pndev = (__spinor_dev_t *)hdev;

    if (block >= pndev->info.partsize)
    {
        __err("out of device capacity, block = %d, partsize = %d\n", block, pndev->info.partsize);
		return 0;
    }

    if (block + n > pndev->info.partsize)
    {
        __err("out of device capacity\n");
        __err("pndev->major_name=%s\n", pndev->major_name);
        __err("pndev->minor_name=%s\n", pndev->minor_name);
        __err("pndev->offset=%d\n", pndev->offset);
        __err("pndev->info.partsize=%d\n", pndev->info.partsize);
        __err("block=%d\n", block);
        __err("n=%d\n", n);
        n = pndev->info.partsize - block;
    }

	memcpy(udisk_buff + (block * pndev->info.secsize), pbuffer, (n * pndev->info.secsize));
    return n;
}

static int32_t udisk_dev_ioctrl(__hdle hdev, uint32_t cmd, long aux, void *pbuffer)
{
    __spinor_dev_t *pndev = (__spinor_dev_t *)hdev;

    switch (cmd)
    {
        case DEV_CMD_GET_INFO:
        {
            if (!pbuffer)
            {
                return EPDK_FAIL;
            }

            *((__dev_blkinfo_t *)pbuffer) = pndev->info;
            return EPDK_OK;
        }

        case DEV_CMD_GET_STATUS:
            return EPDK_OK;

        case DEV_CMD_GET_OFFSET:
            *((__u32 *)pbuffer) = pndev->offset;
            return EPDK_OK;
		case DEV_IOC_USR_FLUSH_CACHE:
		{
			return EPDK_OK;
		}
        default:
            return EPDK_OK;
    }
}

static __dev_devop_t melis_udisk_dev_op =
{
    spinor_dev_open,
    spinor_dev_close,
    udisk_dev_virtual_read,
    udisk_dev_virtual_write,
    udisk_dev_ioctrl
};
static rt_err_t sunxi_udisk_flush_open(rt_device_t dev, rt_uint16_t oflag)
{
    return 0;
}

static rt_err_t sunxi_udisk_flush_close(rt_device_t dev)
{
    return 0;
}

static rt_err_t sunxi_udisk_flush_control(rt_device_t dev, int cmd, void *args)
{
	__spinor_dev_t *pDev = (__spinor_dev_t *)dev->user_data;
	switch (cmd)
	{
		default:
		case DEV_IOC_USR_FLUSH_CACHE:
		{
			melis_udisk_part_flush(pDev);
			return EPDK_OK;
		}
		break;
	}

}

static int register_udisk_flush(__spinor_dev_t *pDev)
{
    int ret = -1;
    struct rt_device *device;

    device = rt_device_create(RT_Device_Class_Block, 0);
    if (!device)
    {
        return ret;
    }

    device->user_data = pDev;
    device->open = sunxi_udisk_flush_open;
    device->close = sunxi_udisk_flush_close;
    device->control = sunxi_udisk_flush_control;
    ret = rt_device_register(device, "udisk_flush", RT_DEVICE_FLAG_RDWR);
    if (ret != 0)
    {
        free(device);
        return ret;
    }
    return 0;
}

int melis_spinor_part_init(struct part *part)
{
    __spinor_dev_t *pDev = NULL;
    static int id = 0;

    if (id >= MAX_PART_COUNT)
    {
        __err("melis spinor part overflow!\n");
        return -1;
    }

    pDev = &pndev[id];
    memset(pDev, 0, sizeof(__spinor_dev_t));
    pDev->offset = part->off;
    snprintf(pDev->major_name, sizeof(pDev->major_name), "%s", "DISK");
    snprintf(pDev->minor_name, sizeof(pDev->minor_name), "%s", part->name);
    pDev->info.hiddennum = 0;
    pDev->info.secnum = 4;
    pDev->info.partsize = part->bytes / 512;
    pDev->info.secsize = 512;

	if (strcmp(part->name, "UDISK") == 0)
	{
		melis_udisk_part_check(pDev);
		register_udisk_flush(pDev);
		pDev->h_reg = esDEV_DevReg(pDev->major_name, pDev->minor_name, &melis_udisk_dev_op, (void *)pDev);
	}
	else
	{
	    pDev->h_reg = esDEV_DevReg(pDev->major_name, pDev->minor_name, &melis_spinor_dev_op, (void *)pDev);
	}
    if (!pDev->h_reg)
    {
        memset(pDev, 0, sizeof(__spinor_dev_t));
    }
    else
    {
        id ++;
    }
    return 0;
}
#endif

static int register_part(rt_device_t dev, struct part *part)
{
    int ret = -1;
    struct rt_device *device;

    device = rt_device_create(RT_Device_Class_Block, 0);
    if (!device)
    {
        return ret;
    }

    device->read = part_read;
    device->control = part_control;
    if (part->erase_flag)
    {
        device->write = part_erase_before_write;
    }
    else
    {
        device->write = part_erase_without_write;
    }

    device->user_data = part;

    ret = rt_device_register(device, part->name, RT_DEVICE_FLAG_RDWR);
    if (ret != 0)
    {
        rt_device_destroy(device);
        return ret;
    }

#if defined(RT_USING_POSIX)
    device->fops = &part_device_fops;
#endif

    return 0;
}
static int register_blk_device(rt_device_t dev)
{
    int ret = -1, index = 0;
    char *gpt_buf;
    struct gpt_part *gpt_part;
    struct part *part;
    unsigned int offset = 0;
    unsigned int *pint64;

    gpt_buf = malloc(GPT_TABLE_SIZE);
    if (!gpt_buf)
    {
        ret = -ENOMEM;
        goto err;
    }
    memset(gpt_buf, 0, GPT_TABLE_SIZE);

    dev->control(dev, BLOCK_DEVICE_CMD_GET_PAGE_SIZE, &norblk.page_bytes);
    dev->control(dev, BLOCK_DEVICE_CMD_GET_TOTAL_SIZE, &norblk.total_bytes);
    dev->control(dev, BLOCK_DEVICE_CMD_GET_BLOCK_SIZE, &norblk.blk_bytes);
    norblk.dev = dev;

    pint64 = (unsigned int *)&norblk.total_bytes;
    pr_debug("total_bytes = 0x%08x%08x\n", pint64[1], pint64[0]);
    pr_debug("blk_bytes = %d\n", norblk.blk_bytes);
    pr_debug("page_bytes = %d\n", norblk.page_bytes);

    ret = nor_get_gpt(dev, gpt_buf, GPT_TABLE_SIZE);
    if (ret)
    {
        pr_err("get gpt from nor flash failed - %d\n", ret);
        goto err;
    }

#ifdef DEBUG
    show_gpt_part(gpt_buf);
#endif

    ret = gpt_part_cnt(gpt_buf);
    if (ret < 0)
    {
        pr_err("get part count from gpt failed\n");
        goto err;
    }

    norblk.n_parts = ret + ARRAY_SIZE(syspart);
    norblk.parts = malloc(sizeof(struct part) * norblk.n_parts);
    if (norblk.parts == NULL)
    {
        pr_err("allocate part array failed.\n");
        goto err;
    }
    memset(norblk.parts, 0, sizeof(struct part) * norblk.n_parts);

    for (index = 0; index < ARRAY_SIZE(syspart); index++)
    {
        part = &norblk.parts[index];
        part->blk = &norblk;
        part->bytes = syspart[index].bytes;
        part->off = syspart[index].offset;
        snprintf(part->name, MAX_BLKNAME_LEN, "%s", syspart[index].name);
        offset += part->bytes;
        part->erase_flag = 1;
    }

    foreach_gpt_part(gpt_buf, gpt_part)
    {
        part = &norblk.parts[index++];
        part->blk = &norblk;
        part->bytes = gpt_part->sects << SECTOR_SHIFT;
        part->off = offset;
        snprintf(part->name, MAX_BLKNAME_LEN, "%s", gpt_part->name);
        if (likely(strcmp(part->name, "ROOTFS")))
        {
            part->erase_flag = 1;
        }
        else
        {
            part->erase_flag = 0;
        }
		if (unlikely(strcmp(part->name, "UDISK") == 0))
		{
			part->bytes = (norblk.total_bytes - offset - 2*norblk.blk_bytes)/2;
		}
        offset += part->bytes;
    }
    for (index = 0; index < norblk.n_parts; index++)
    {
        part = &norblk.parts[index];
        if (part->bytes % norblk.blk_bytes)
        {
            pr_err("part %s with bytes %u should align to block size %u\n",
                   part->name, part->bytes, norblk.blk_bytes);
        }
        else
        {
			register_part(dev, part);
#ifdef CONFIG_MELIS_LEGACY_DRIVER_MAN
            if (!strcmp(part->name, "ROOTFS"))
            {
                int melis_spinor_part_init(struct part *);
                melis_spinor_part_init(part);
            } 
            else if (!strcmp(part->name, "UDISK"))
            {
                int melis_spinor_part_init(struct part *);
                melis_spinor_part_init(part);
            }
#endif			
        }
    }
    blkpart_add_list(&norblk);
err:
    free(gpt_buf);
    return ret;
}

int nor_blkpart_init(const char *spinor_device_name)
{
    int ret = -1, value = 0;
    rt_device_t dev = rt_device_find(spinor_device_name);
    if (!dev)
    {
        pr_err("the spinor device %s is not exist!\n", spinor_device_name);
        return ret;
    }

    if (dev->init)
    {
        dev->init(dev);
    }
	ret = esCFG_GetKeyValue("card_product", "card_product_used", (int32_t *)&value, 1);
	if (ret == 0 && value) {
		ret = esCFG_GetKeyValue("card_product", "card_product_storage", (int32_t *)&value, 1);
		if (value == STORAGE_NOR) {
			printk("%s %d card product target is spinor, spinor init ok, not register blkdev!\n", __FILE__, __LINE__);
			return -1;
		}
	}
    ret = register_blk_device(dev);
    return ret;
}
