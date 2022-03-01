/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : exfatfs.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-16
* Descript: exfat file system.
* Update  : date                auther      ver     notes
*           2011-3-16 14:30:32  Sunny       1.0     Create this file.
*********************************************************************************************************
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

