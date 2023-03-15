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
#include "slab.h"
#include "err.h"
#include "fsys_debug.h"
#include "blk_dev.h"
#include "cdfs.h"

static struct file_system_type cdfs_fs_type;

int cdfs_format(__hdle p_part, __hdle fmtpara)
{
    int res;

    fs_log_trace01("cdfs fomat enter...\n");

    /* not support format now */
    res = EPDK_FALSE;

    fs_log_trace01("cdfs fomat return(%x)\n", res);

    return res;
}

static int cdfs_check_valid(struct super_block *sb)
{
    struct buffer_head *bh;
    struct hs_volume_descriptor *hdp;
    struct iso_volume_descriptor  *vdp;
    unsigned int vol_desc_start;
    unsigned int iso_blknum, block;
    int find_iso, find_high_sierra;

    /*
     * For multi session CD-ROM,
     * volume descriptors start from first block of last session
     */
    vol_desc_start = isofs_get_last_session(sb);

    /* we don't find any volume type now  */
    find_high_sierra = 0;
    find_iso = 0;

    /* For better performance,
     * just try 32 blocks.
     * By sunny,  2010.3.24
     */
    for (iso_blknum = vol_desc_start + 16;
         iso_blknum < vol_desc_start + 32; iso_blknum++)
    {

        block = iso_blknum << (ISOFS_BLOCK_BITS - sb->s_blocksize_bits);
        if (!(bh = sb_bread(sb, block)))
        {
            return -EIO;
        }
        vdp = (struct iso_volume_descriptor *)bh->b_data;
        hdp = (struct hs_volume_descriptor *)bh->b_data;
        /*
         * Due to the overlapping physical location of the descriptors,
         * ISO CDs can match hdp->id==HS_STANDARD_ID as well. To ensure
         * proper identification in this case, we first check for ISO.
         */
        if (strncmp(vdp->id, ISO_STANDARD_ID, sizeof vdp->id) == 0)
        {

            /* maybe check primary descriptor here */
            find_iso = 1;
            break;
        }
        else
        {
            if (strncmp(hdp->id, HS_STANDARD_ID, sizeof hdp->id) == 0)
            {

                /* maybe check primary descriptor here */
                find_high_sierra = 1;
                break;
            }
        }
        /* Just skip any volume descriptors we don't recognize */
        brelse(bh);
        bh = NULL;
    }
    /*
     * If we fall through, either no volume descriptor was found,
     * or else we passed a primary descriptor looking for others.
     */
    if (bh)
    {
        brelse(bh);
    }

    if (find_iso)
    {
        fs_log_trace01("CDFS find ISO9660 type volume\n");
        return 0;
    }
    if (find_high_sierra)
    {
        fs_log_trace01("CDFS find high sierra type volume\n");
        return 0;
    }
    /* unkown volume type */
    return 1;
}

int cdfs_identify(__hdle pPart)
{
    __s32 res = EPDK_TRUE;
    struct super_block *sb;

    fs_log_trace01("cdfs identify enter...\n");

    sb  = kmalloc(sizeof(struct super_block), GFP_KERNEL);
    if (!sb)
    {
        return EPDK_FALSE;
    }
    memset(sb,  0x00, sizeof(struct super_block));

    /* set super block useful field for identify */
    sb->s_blocksize = 2048;
    sb->s_blocksize_bits = 11;
    sb->s_part = pPart;

    /*
     * First of all, get the hardware blocksize for this device.
     * If we don't know what it is, or the hardware blocksize is
     * larger than the blocksize the user specified, then use
     * that value.
     */
    sb->s_blocksize = sb_min_blocksize(sb, sb->s_blocksize);

    /* read volume recognition sequences */
    if (cdfs_check_valid(sb))
    {
        fs_log_trace01("cdfs-fs: VRS not found\n");
        res = EPDK_FALSE;
    }
    fs_log_trace01("cdfs identify return(%x)\n", res);
    kfree(sb);
    return res;
}

int cdfs_mount(__hdle part)
{
    int res;
    int flags = MS_RDONLY;              /* cdfs mount for readonly */

    fs_log_trace01("cdfs fs mount:\n");

    res = get_sb_bdev(&cdfs_fs_type, flags, part, NULL, isofs_fill_super);

    if (!res)
    {
        char *partName;
        esFSYS_pioctrl(part, PART_IOC_SYS_GETNAME, 0, &partName);
    }

    fs_log_trace01("O%d\n", res);

    return !res ? EPDK_OK : EPDK_FAIL;
}

int cdfs_unmount(struct super_block *sb, int force)
{
    int res;

    fs_log_trace01("cdfs fs unmount...\n");

    res = deactivate_super(sb, force);

    fs_log_trace01("O\n");
    return !res ? EPDK_OK : EPDK_FAIL;
}

static struct file_system_type cdfs_fs_type =
{
    .name       = "cdfs",
    .mount      = cdfs_mount,
    .unmount    = cdfs_unmount,
    .identify   = cdfs_identify,
    .format     = cdfs_format,
    .fs_flags   = FS_REQUIRES_DEV,
};

/* ----------------------- init && exit --------------------*/
__s32 fsys_cdfs_init(void)
{
    int err = isofs_init_inodecache();
    if (err)
    {
        goto out;
    }

#ifdef CONFIG_ZISOFS
    err = zisofs_init();
    if (err)
    {
        goto out1;
    }
#endif

    err = esFSYS_fsreg((__hdle)&cdfs_fs_type);
    if (err)
    {
        goto out2;
    }

    return 0;

out2:
#ifdef CONFIG_ZISOFS
    zisofs_cleanup();
out1:
#endif
    isofs_destroy_inodecache();
out:
    return err;
}

__s32 fsys_cdfs_exit(void)
{
    esFSYS_fsunreg((__hdle)&cdfs_fs_type);

#ifdef CONFIG_ZISOFS
    zisofs_cleanup();
#endif

    isofs_destroy_inodecache();

    return EPDK_OK;
}