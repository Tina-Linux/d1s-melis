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
#include "minfs_i.h"

static struct file_system_type minfs_fs_type;

int minfs_identify(__hdle pPart)
{
    int             res;
    int             sctsize = 0;
    unsigned char  *pBuffer;
    __minfs_hdr_t  *pMFSHdr;

    fs_log_trace01("minfs identify enter...\n");

    /* First of all, get the hardware sector size for this device.
     * we can't read data size smaller then hardware sector size.
     * sector size must be a power of two, and between 512 and PAGE_SIZE.
     */
    sctsize = dev_hardsect_size(pPart);
    if (sctsize > PAGE_SIZE || sctsize < 512 || !is_power_of_2(sctsize))
    {
        fs_log_error("invalid hardware sector size.\n");
        res = EPDK_FALSE;
        pBuffer = NULL;
        goto out;
    }
    pBuffer = palloc(sctsize);
    if (pBuffer == NULL)
    {
        fs_log_error("no memory for identify operation.\n");
        res = EPDK_FALSE;
        goto out;
    }

    res = esFSYS_pread((void *)pBuffer, 0, 1, pPart);
    if (res <= 0)
    {
        res = EPDK_FALSE;
        goto out;
    }

    pMFSHdr = (__minfs_hdr_t *)pBuffer;

    if (memcmp(pMFSHdr->Magic, MINFS_MAGIC, MINFS_MAGIC_LEN - 1) != 0)
    {
        res = EPDK_FALSE;
        goto out;
    }

    res = EPDK_TRUE;

out:
    if (pBuffer)
    {
        pfree(pBuffer, sctsize);
    }
    fs_log_trace01("minfs identify return(%x)\n", res);
    return res;
}

int minfs_mount(__hdle part)
{
    int res;

    /* minfs mount for readonly */
    int flags = MS_RDONLY;

    fs_log_trace01("minfs fs mount:\n");

    res = get_sb_bdev(&minfs_fs_type, flags, part, NULL, minfs_fill_super);

    fs_log_trace01("O%d\n", res);

    return !res ? EPDK_OK : EPDK_FAIL;
}

int minfs_unmount(struct super_block *sb, int force)
{
    int res;

    fs_log_trace01("minfs fs unmount...\n");

    res = deactivate_super(sb, force);

    fs_log_trace01("O\n");
    return !res ? EPDK_OK : EPDK_FAIL;
}

static struct file_system_type minfs_fs_type =
{
    .name       = "minfs",
    .mount      = minfs_mount,
    .unmount    = minfs_unmount,
    .identify   = minfs_identify,
    .fs_flags   = FS_REQUIRES_DEV,
};


__s32 fsys_minfs_init(void)
{
    int err = minfs_init_inodecache();
    if (err)
    {
        return err;
    }
    err = esFSYS_fsreg((__hdle)&minfs_fs_type);
    if (err)
    {
        minfs_destroy_inodecache();
        return err;
    }
    return 0;
}

__s32 fsys_minfs_exit(void)
{
    esFSYS_fsunreg((__hdle)&minfs_fs_type);

    minfs_destroy_inodecache();

    return EPDK_OK;
}