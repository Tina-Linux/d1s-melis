/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : devfs.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-16
* Descript: device file system.
* Update  : date                auther      ver     notes
*           2011-3-16 14:07:38  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "devfs.h"
#include "fsys_debug.h"
#include <sys_device.h>
#include <string.h>

struct list_head            devfs_suppers;
struct file_system_type     devfs_fs_type;

extern int32_t  devfs_fill_super(struct super_block *, void *, int32_t);
extern int32_t  devfs_init_inodecache(void);

int32_t devfs_format(__hdle p_part, __hdle fmtpara)
{
    return EPDK_FAIL;
}

int32_t devfs_identify(__hdle pPart)
{
    int32_t     res = EPDK_FALSE;
    char        *clsname;
    __hdle      dev;

    fs_log_trace01("devfs identify...");

    if (esFSYS_pioctrl(pPart, PART_IOC_SYS_GETDEV, 0, &dev) == EPDK_OK)
    {
        if (esDEV_Ioctl((__hdle)dev, DEV_IOC_SYS_GET_CLSNAME, 0, &clsname) == EPDK_OK)
        {
            if (!strcmp(clsname, DEV_CLASS_DMS))
            {
                res = EPDK_TRUE;
            }
        }
    }
    fs_log_trace01("O%d\n", res);

    return res;
}

int32_t devfs_mount(__hdle part)
{
    int32_t     res;

    fs_log_trace01("devfs mount...");

    res = get_sb_bdev(&devfs_fs_type, 0, part, NULL, devfs_fill_super);

    fs_log_trace01("O%d\n", res);

    return !res ? EPDK_OK : EPDK_FAIL;
}

int32_t devfs_unmount(struct super_block *sb, int force)
{
    int32_t     res;

    fs_log_trace01("devfs unmount...");

    res = deactivate_super(sb, force);

    fs_log_trace01("O\n");
    return !res ? EPDK_OK : EPDK_FAIL;
}

struct file_system_type devfs_fs_type =
{
    .name       = "devfs",
    .mount      = devfs_mount,
    .unmount    = devfs_unmount,
    .identify   = devfs_identify,
    .format     = devfs_format,
    .fs_flags   = FS_REQUIRES_DEV,
};

/* ----------------------- init && exit --------------------*/
int32_t fsys_devfs_init(void)
{
    int32_t err = 0;

    err = devfs_init_inodecache();
    if (err)
    {
        goto out;
    }

    err = esFSYS_fsreg((__hdle)&devfs_fs_type);

out:
    return err;
}

int32_t fsys_devfs_exit(void)
{
    esFSYS_fsunreg((__hdle)&devfs_fs_type);

    return EPDK_OK;
}
