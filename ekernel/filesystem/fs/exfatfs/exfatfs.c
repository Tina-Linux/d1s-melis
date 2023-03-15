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
#include "exfatfs.h"
#include "exfat_cache.h"
#include "endians.h"
#include "blk_dev.h"
#include "fsys_debug.h"
#include "fsys_libs.h"
#include "fs.h"

struct file_system_type exfat_fs_type;

int exfat_format(__hdle p_part, __hdle fmtpara)
{
    int res;

    fs_log_trace01("exfat fomat enter...\n");

    /* exfat not support format now */
    res = EPDK_FALSE;

    fs_log_trace01("exfat fomat return(%x)\n", res);

    return res;
}

int exfat_boot_sector_is_exfat(void *boot_sector)
{
    char *message;
    struct exfat_super_block *exsb;

    /* can't set boot_sector to exfat sb type
     * can't be sure, must rethink this
     */
    exsb = (struct exfat_super_block *)boot_sector;

    /* first : check oem_id */
    if (memcmp(exsb->oem_id, EXFAT_OEM_ID, sizeof(exsb->oem_id)))
    {
        message = "invalid OEM ID";
        goto not_exfat;
    }
    /* second : check boot sector signature */
    if (exsb->signature != cpu_to_le16(EXFAT_BOOT_SIG))
    {
        message = "invalid boot block signature";
        goto not_exfat;
    }
    /* not find any error */
    return 0;

not_exfat:
    fs_log_info("exFAT: %s\n", message);
    return 1;
}

int exfat_identify(__hdle pPart)
{
    int res;
    int sctsize;

    unsigned char *buffer;

    fs_log_trace01("exfat identify enter...\n");

    /* First of all, get the hardware sector size for this device.
     * we can't read data size smaller then hardware sector size.
     * sector size must be a power of two, and between 512 and PAGE_SIZE.
     */
    sctsize = dev_hardsect_size(pPart);
    if (sctsize > PAGE_SIZE || sctsize < 512 || !is_power_of_2(sctsize))
    {
        fs_log_error("invalid hardware sector size.\n");
        res = EPDK_FALSE;
        buffer = NULL;
        goto out;
    }

    buffer = palloc(sctsize);
    if (!buffer)
    {
        fs_log_error("no memory for exfat identify operation.\n");
        res = EPDK_FALSE;
        goto out;
    }

    res = esFSYS_pread((void *)buffer, 0, 1, pPart);
    if (res <= 0)
    {
        res = EPDK_FALSE;
        goto out;
    }

    /* part identify really */
    if (exfat_boot_sector_is_exfat(buffer))
    {
        res = EPDK_FALSE;
        goto out;
    }
    /* should do other things or not, must rethinlk this carefully */
    res = EPDK_TRUE;
out:
    if (buffer)
    {
        pfree(buffer, sctsize);
    }
    fs_log_trace01("exfat identify return(%x)\n", res);
    return res;
}

int exfat_mount(__hdle part)
{
    int res;

#if defined CONFIG_FSYS_EXFAT_RW
    int flags = 0;
#else
    int flags = MS_RDONLY;
#endif

    fs_log_trace01("exfat fs mount:\n");

    res = get_sb_bdev(&exfat_fs_type, flags, part, NULL, exfat_fill_super);

    fs_log_trace01("O%d\n", res);

    return !res ? EPDK_OK : EPDK_FAIL;
}

int exfat_unmount(struct super_block *sb, int force)
{
    int res;

    fs_log_trace01("exfat fs unmount...\n");

    res = deactivate_super(sb, force);

    fs_log_trace01("O\n");
    return !res ? EPDK_OK : EPDK_FAIL;
}

struct file_system_type exfat_fs_type =
{
    .name       = "exfat",
    .mount      = exfat_mount,
    .unmount    = exfat_unmount,
    .identify   = exfat_identify,
    .format     = exfat_format,
    .fs_flags   = FS_REQUIRES_DEV,
};

/* ----------------------- init && exit --------------------*/
__s32 fsys_exfatfs_init(void)
{
    int err;

    err = exfat_init_cache_cachep();
    if (err)
    {
        goto out;
    }

    err = exfat_init_inodecache();
    if (err)
    {
        goto error_cache;
    }

    err = esFSYS_fsreg((__hdle)&exfat_fs_type);
    if (err)
    {
        goto error_inode;
    }

    goto out;

error_inode:
    exfat_destroy_inodecache();
error_cache:
    exfat_destroy_cache_cachep();
out:
    return err;
}

__s32 fsys_exfatfs_exit(void)
{
    esFSYS_fsunreg((__hdle)&exfat_fs_type);

    exfat_destroy_inodecache();

    exfat_destroy_cache_cachep();

    return EPDK_OK;
}