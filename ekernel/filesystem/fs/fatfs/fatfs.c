/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : fatfs.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-16
* Descript: fat file system.
* Update  : date                auther      ver     notes
*           2011-3-16 15:51:38  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "fatfs.h"
#include "page_pool.h"
#include "fsys_debug.h"
#include "blk_dev.h"
#include "err.h"

struct file_system_type fat_fs_type;
int _FS_FAT_AutoFormat(__hdle pPart, __hdle Aux);
int fat_init_inodecache(void);
void fat_destroy_inodecache(void);

int fat_format(__hdle p_part, __hdle fmtpara)
{
#if FSYS_FAT_FORMAT_EN
    int res;

    fs_log_trace01("fat fomat enter...\n");

    res = _FS_FAT_AutoFormat(p_part, fmtpara);

    fs_log_trace01("fat fomat return(%x)\n", res);

    return res;
#else
    fs_log_error("fat fomat is disable\n");
    return -EPERM;
#endif
}

int fat_identify(__hdle pPart)
{
    int res;
    int sctsize = 0;

    unsigned char *buffer;

    fs_log_trace01("lfs identify enter...\n");

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
        fs_log_error("no memory for identify operation.\n");
        res = EPDK_FALSE;
        goto out;
    }

    memset(buffer, 0x00, sctsize);
    res = esFSYS_pread((void *)buffer, 0, 1, pPart);
    if (res <= 0)
    {
        res = EPDK_FALSE;
        goto out;
    }

    if ((buffer[512 - 2] + 256 * buffer[512 - 1] != 0xaa55) ||
        (buffer[16] != 2))
    {
        res = EPDK_FALSE;
        goto out;
    }

    res = EPDK_TRUE;
out:
    if (buffer)
    {
        pfree(buffer, sctsize);
    }
    fs_log_trace01("lfs identify return(%x)\n", res);
    return res;
}

int fat_mount(__hdle part)
{
    int res;

#if defined CONFIG_FSYS_FAT_RW
    int flags = 0;
#else
    int flags = MS_RDONLY;
#endif

    fs_log_trace01("fsmount:\n");

    res = get_sb_bdev(&fat_fs_type, flags, part, NULL, fat_fill_super);

    fs_log_trace01("O%d\n", res);

    return !res ? EPDK_OK : EPDK_FAIL;
}

int fat_unmount(struct super_block *sb, int force)
{
    int res;

    fs_log_trace01("fat unmount...\n");

    res = deactivate_super(sb, force);

    fs_log_trace01("O\n");
    return !res ? EPDK_OK : EPDK_FAIL;
}

struct file_system_type fat_fs_type =
{
    .name       = "fat",
    .mount      = fat_mount,
    .unmount    = fat_unmount,
    .identify   = fat_identify,
    .format     = fat_format,
    .fs_flags   = FS_REQUIRES_DEV,
};

/* ----------------------- init && exit --------------------*/
__s32 fsys_fatfs_init(void)
{
    int err;

    err = fat_cache_init();
    if (err)
    {
        goto out;
    }

    err = fat_init_inodecache();
    if (err)
    {
        goto free2;
    }

    err = esFSYS_fsreg((__hdle)&fat_fs_type);
    if (err)
    {
        goto free1;
    }
    goto out;

free1:
    fat_destroy_inodecache();
free2:
    fat_cache_destroy();
out:
    return err;
}

__s32 fsys_fatfs_exit(void)
{
    esFSYS_fsunreg((__hdle)&fat_fs_type);

    fat_cache_destroy();
    fat_destroy_inodecache();

    return EPDK_OK;
}

