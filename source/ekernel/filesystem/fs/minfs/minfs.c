/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-26
* Descript: Mini rom image file system.
* Update  : date                auther      ver     notes
*           2011-3-26 14:16:48  Sunny       1.0     Create this file.
*********************************************************************************************************
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
