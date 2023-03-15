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