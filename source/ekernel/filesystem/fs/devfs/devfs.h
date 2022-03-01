/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : devfs.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-16
* Descript: device file system.
* Update  : date                auther      ver     notes
*           2011-3-16 14:04:51  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __DEVFS_FS_H__
#define __DEVFS_FS_H__

#include "fs.h"
#include "supports.h"

#define DEVFS_ROOT_INO      1
#define DEVFS_SUPER_MAGIC   0x5d55

#define DEVFS_MAX_LINK      1000

struct devfs_mount_options
{
    unsigned short codepage;  /* Codepage for shortname conversions */
    char *iocharset;          /* Charset used for filename input/display */
    unsigned char name_check; /* r = relaxed, n = normal, s = strict */
};

struct devfs_sb_info
{
    const void *dir_ops;             /* Opaque; default directory operations */

    struct devfs_mount_options options;

    struct list_head inode_list;
};

struct devfs_inode_info
{
    __hdle       i_hnode;
    unsigned int i_key;
    struct list_head i_list;    /* hash by i_location */
    struct inode     vfs_inode;
};

typedef  struct DmsNodeInfo_dev dms_node_info_fs_t;
typedef  struct DmsDirInfo_dev  dms_dir_info_fs_t;

static inline struct devfs_sb_info *DEVFS_SB(struct super_block *sb)
{
    return sb->s_fs_info;
}

static inline struct devfs_inode_info *DEVFS_I(struct inode *inode)
{
    return container_of(inode, struct devfs_inode_info, vfs_inode);
}


#endif  /* __DEVFS_FS_H__ */
