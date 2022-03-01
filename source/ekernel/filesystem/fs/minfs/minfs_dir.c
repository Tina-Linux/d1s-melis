/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_dir.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-26
* Descript: minfs directory handing functions.
* Update  : date                auther      ver     notes
*           2011-3-26 16:42:41  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "minfs_i.h"

static __s32 minfs_readdir(struct file *filp, void *dirent, filldir_t filldir)
{
    struct inode            *dir = filp->f_dentry->d_inode;
    struct super_block      *sb  = dir->i_sb;
    struct minfs_sb_info    *sbi = MINFS_SB(sb);
    struct minfs_inode_info *exi = MINFS_I(dir);
    __minfs_dentry_t        *pdentry;
    __u32                   offset;
    unsigned int            dt_type;
    int                     inode_number;

    /* position offset must small then directory size */
    if (filp->f_pos >= exi->DataLen)
    {
        return -ENOENT;
    }

    //read directory from current offset position
    offset  = filp->f_pos;
    pdentry = (__minfs_dentry_t *)(sbi->pDEntryData + exi->DataOffset - sbi->RootDataOffet + offset);

    while (offset < exi->DataLen)
    {
        if (pdentry->Attribute & MINFS_ATTR_DIR)
        {
            dt_type = FSYS_ATTR_DIRECTORY;
        }
        else
        {
            dt_type = 0;
        }
        inode_number = exi->DataOffset + offset;

        //fill this dentry
        if (filldir(dirent, pdentry->MetaData, pdentry->NameLen, 0, filp->f_pos, inode_number, dt_type, pdentry->Size))
        {
            //buffer overflow
            break;
        }

        //next dentry
        filp->f_pos        += (pdentry->RecLen);
        offset             += (pdentry->RecLen);
        pdentry = (__minfs_dentry_t *)((uintptr_t)(pdentry) + pdentry->RecLen);
    }

    //readdir finished
    return 0;
}

const struct file_operations minfs_dir_operations =
{
    .readdir = minfs_readdir,
};
