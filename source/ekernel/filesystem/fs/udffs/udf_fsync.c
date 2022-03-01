/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : udf_fsync.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-17
* Descript: Fsync handling routines for the OSTA-UDF(tm) filesystem.
* Update  : date                auther      ver     notes
*           2011-3-17 9:04:15   Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "udf_decl.h"
#include "fs.h"
#include "udf_fs.h"

#if defined CONFIG_FSYS_UDFFS_RW

static int udf_fsync_inode(struct inode *, int);

/*
 *  File may be NULL when we are called. Perhaps we shouldn't
 *  even pass file to fsync ?
 */

int udf_fsync_file(struct file *file, struct dentry *dentry, int datasync)
{
    struct inode *inode = dentry->d_inode;

    return udf_fsync_inode(inode, datasync);
}

static int udf_fsync_inode(struct inode *inode, int datasync)
{
    int err = 0;

    /* FIXME : ?? maybe update inode buffer heads here */
    //err = sync_mapping_buffers(inode->i_mapping);
    if (!(inode->i_state & I_DIRTY))
    {
        return err;
    }
    if (datasync && !(inode->i_state & I_DIRTY_DATASYNC))
    {
        return err;
    }

    err |= udf_sync_inode(inode);

    return err ? -EIO : 0;
}

#endif  /* FSYS_UDF_RW */

