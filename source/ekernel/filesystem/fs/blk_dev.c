/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : blk_dev.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: block device operations.
* Update  : date                auther      ver     notes
*           2011-3-15 15:27:17  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "fs.h"
#include "blk_dev.h"
#include "page_pool.h"
#include "buffer_head.h"
#include "errno.h"

int dev_hardsect_size(__hdle part)
{
    int size = 0;

    if (esFSYS_pioctrl(part, PART_IOC_USR_GETSCTSIZE, 0, &size) != EPDK_OK)
    {

        /* default sector size is 512 Byte */
        return 512;
    }
    /* invalid sector size, default is 512 byte */
    if (size > PAGE_SIZE || size < 512 || !is_power_of_2(size))
    {
        return 512;
    }
    return size;
}

/* assumes size > 256 */
int blksize_bits(int size)
{
    int bits = 8;
    do
    {
        bits++;
        size >>= 1;
    } while (size > 256);
    return bits;
}

int set_blocksize(__hdle part, int size)
{
    int hd_sct_size = 0;

    /* Size must be a power of two, and between 512 and PAGE_SIZE */
    if (size > PAGE_SIZE || size < 512 || !is_power_of_2(size))
    {
        return -EINVAL;
    }

    /* get hardare device sector size */
    hd_sct_size = dev_hardsect_size(part);
    if (hd_sct_size <= 0)
    {
        return -EINVAL;
    }

    /* Size cannot be smaller than the size supported by the device */
    if (size < hd_sct_size)
    {
        return -EINVAL;
    }

    /* Don't change the size if it is same as current */
    //  if (bdev->bd_block_size != size) {
    //      sync_blockdev(bdev);
    //      bdev->bd_block_size = size;
    //      bdev->bd_inode->i_blkbits = blksize_bits(size);
    //      kill_bdev(bdev);
    //  }
    return 0;
}

int sb_set_blocksize(struct super_block *sb, int size)
{
    if (set_blocksize(sb->s_part, size))
    {
        return 0;
    }

    /* invalid sb associate buffers */
    invalidate_buffers(sb, 1, 0);

    /* If we get here, we know size is power of two
     * and it's value is between 512 and PAGE_SIZE,
     * and the old buffers invalid.
     */
    sb->s_blocksize = size;
    sb->s_blocksize_bits = blksize_bits(size);
    return sb->s_blocksize;
}

int sb_min_blocksize(struct super_block *sb, int size)
{
    int minsize = dev_hardsect_size(sb->s_part);
    if (size < minsize)
    {
        size = minsize;
    }
    return sb_set_blocksize(sb, size);
}
