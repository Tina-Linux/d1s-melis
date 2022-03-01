/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : dev_dir.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-16
* Descript: device file system directory handing functions.
* Update  : date                auther      ver     notes
*           2011-3-16 14:05:49  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "fs.h"
#include "devfs.h"
#include "errno.h"
#include "fsys_debug.h"


/**
  *return value:
  *     1: fill buffer overflow
  *     0: end of dir
  *    <0: ENOMEM error or other fatal error, need stop to go on scaning the directory
  */
static __s32 devfs_readdir(struct file *filp, void *dirent, filldir_t filldir)
{
    struct super_block *sb = filp->f_dentry->d_sb;
    struct inode *inode = filp->f_dentry->d_inode;
    dms_dir_info_fs_t itemInfo;
    dms_node_info_fs_t dinfo;
    __s64  cpos;
    __u64  cino;
    int rc, ret = 0;

    memset(&itemInfo, 0x00, sizeof(dms_dir_info_fs_t));
    memset(&dinfo, 0x00, sizeof(dms_node_info_fs_t));

    cpos = filp->f_pos;

    /* 判断是否是合法的目录节点 */
    if (! S_ISDIR(inode->i_mode))
    {
        fs_log_debug("fail, readdir on a none dir node!\n");
        ret = -ENOTDIR;
        goto out;
    }

    for (;;)
    {
        /* 读取目录下的条目 */
        itemInfo.dir = DEVFS_I(inode)->i_hnode;
        itemInfo.itemPos = cpos;
        ret = esFSYS_pioctrl(sb->s_part, PART_IOC_USR_GETITEM, (long)&itemInfo, &dinfo);
        if (ret == EPDK_FAIL) /* 到达目录末尾 */
        {
            ret = 0;
            goto EOD;
        }

        /* 填充目录结构 */
        cino = cpos + 1;
        rc = filldir(dirent, dinfo.name, strlen(dinfo.name), 0, cpos ++, cino,
                     (dinfo.type == DEVFS_CLASS_TYPE ? FSYS_ATTR_DIRECTORY : 0) | FSYS_ATTR_READONLY | FSYS_ATTR_SYSTEM, 0);
        if (rc)    /* 目录缓存空间不足 */
        {
            ret = 1;
            goto NoEnoughFillBuff;
        }

        filp->f_pos = cpos;
    }

EOD:
NoEnoughFillBuff:

out:
    return ret;
}

/* ------------------- dir ops -------------------------------*/
const struct file_operations devfs_dir_operations =
{
    .readdir    = devfs_readdir,
};

