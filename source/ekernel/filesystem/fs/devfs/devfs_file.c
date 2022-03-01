/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : dev_file.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-16
* Descript: device file system file handing functions.
* Update  : date                auther      ver     notes
*           2011-3-16 14:06:16  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "devfs.h"
#include "errno.h"
#include "fsys_debug.h"
#include <sys_device.h>

__s32 devfs_generic_ioctl(struct inode *inode, struct file *filp,
                          __u32 cmd, __u32 aux, void *pBuffer)
{
    return esDEV_Ioctl(filp->private_data, cmd, aux, pBuffer);
}

static __s32 devfs_file_read(struct file *filp, char *buf, __u32 len, __s64 *ppos)
{
    return esDEV_Read(buf, *ppos, len, filp->private_data);
}

static __s32 devfs_file_write(struct file *filp, const char *buf, __u32 len, __s64 *ppos)
{
    return esDEV_Write(buf, *ppos, len, filp->private_data);
}

static __s32 devfs_file_open(struct inode *finode, struct file *filp)
{
    struct devfs_inode_info *devi = DEVFS_I(finode);

    filp->f_dev = 1;
    filp->private_data = esDEV_Open(devi->i_hnode, 0);                    /* 打开设备                                                     */
    if (filp->private_data == 0)
    {
        fs_log_warning("dev cannot opened!\n");
        return -EIO;
    }

    return 0;
}

static __s32 devfs_file_close(struct file *filp)
{
    return esDEV_Close(filp->private_data);
}

static __s32 devfs_file_fsync(struct file *filp, struct dentry *dentry, __s32 datasync)
{
    return 0;
}

/* ------------------- file ops -------------------------------*/
const struct file_operations devfs_file_operations =
{
    .read       = devfs_file_read,
    .write      = devfs_file_write,
    .ioctl      = devfs_generic_ioctl,
    .open       = devfs_file_open,
    .close      = devfs_file_close,
    .fsync      = devfs_file_fsync,
};

const struct inode_operations devfs_file_inode_operations =
{
};

