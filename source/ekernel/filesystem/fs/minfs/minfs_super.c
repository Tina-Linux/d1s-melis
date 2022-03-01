/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_super.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-26
* Descript: Mini rom image file system super blobk handing functions.
* Update  : date                auther      ver     notes
*           2011-3-26 14:34:41  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "minfs_i.h"
#include <port.h>

int minfs_init_sb_info(struct super_block *sb)
{
    struct minfs_sb_info *sbi = MINFS_SB(sb);
    struct buffer_head   *bh;
    __minfs_hdr_t        *pMFSHdr;

    bh = sb_bread(sb, 0);
    if (bh == NULL)
    {
        fs_log_error("minfs: unable to read super block\n");
        return -EIO;
    }

    pMFSHdr = (__minfs_hdr_t *)(bh->b_data);
    if (memcmp(pMFSHdr->Magic, MINFS_MAGIC, MINFS_MAGIC_LEN - 1) != 0)
    {
        fs_log_error("invalid minfs super block\n");
        brelse(bh);
        return -EINVAL;
    }

    //allocate semaphore for this super block
    sbi->Sem = esKRNL_SemCreate(1);;
    if (sbi->Sem == NULL)
    {
        fs_log_error("allocate semaphore for minfs super block failed\n");
        brelse(bh);
        return -EINVAL;
    }
    sbi->Size          = pMFSHdr->Size;
    sbi->DEntryNum     = pMFSHdr->DEntryNum;
    sbi->DEntryLen     = pMFSHdr->DEntryLen;
    sbi->RootDataOffet = pMFSHdr->RootDirOffset;
    sbi->RootDirSize   = pMFSHdr->RootDirSize;
    sbi->SectorSize    = 512;
    sbi->SectorBits    = 9;

    //allocate buffer to cache all dentry metadata,
    //this can improve the dentry operation performances.
    //by sunny at 2011-3-26 15:26:54.
    sbi->pDEntryData = palloc(sbi->DEntryLen);
    if (sbi->pDEntryData == NULL)
    {
        fs_log_error("allocate buffer for minfs dentry data failed\n");
        esKRNL_SemDel(sbi->Sem, 0, NULL);
        brelse(bh);
        return -ENOMEM;
    }

    //initialize minfs cache,
    //maybe should check is ramdisk or not,
    //ramdisk no need to cache any data,
    //all cache will cost-down access performance.
    //by sunny at 2011-3-31.
    minfs_cache_init(sb);

    //load detry data to buffer
    minfs_pread(sb, sbi->pDEntryData, pMFSHdr->RootDirOffset, pMFSHdr->DEntryLen);

    return 0;
}

void minfs_exit_sb_info(struct super_block *sb)
{
    struct minfs_sb_info *sbi = MINFS_SB(sb);

    //exit minfs cache
    minfs_cache_exit(sb);

    if (sbi->Sem)
    {
        esKRNL_SemDel(sbi->Sem, 0, NULL);
        sbi->Sem = NULL;
    }
    if (sbi->pDEntryData)
    {
        pfree(sbi->pDEntryData, sbi->DEntryLen);
        sbi->pDEntryData = NULL;
    }
}

__s32 minfs_fill_super(struct super_block *sb, void *data, __s32 silent)
{
    struct inode *root_inode = NULL;
    struct minfs_sb_info *sbi;

    /* allocate buffer for super block of minfs */
    sbi = (struct minfs_sb_info *)malloc(sizeof(struct minfs_sb_info));
    if (sbi == NULL)
    {
        return -ENOMEM;
    }
    memset(sbi, 0, sizeof(struct minfs_sb_info));

    /* initialize minfs super block information */
    sb_min_blocksize(sb, 512);

    sb->s_fs_info   = sbi;

    if (minfs_init_sb_info(sb))
    {
        free(sbi);
        return -EINVAL;
    }

    /* initialize super block */
    sb->s_flags             |= MS_RDONLY;
    sb->s_magic             = MINFS_SUPER_MAGIC;
    sb->s_op                = &minfs_sops;
    sb->s_blocksize_bits    = 9;
    sb->s_blocksize         = 512;

    root_inode = new_inode(sb);
    if (root_inode == NULL)
    {
        fs_log_error("allocate minfs root inode failed\n");
        free(sbi);
        minfs_exit_sb_info(sb);
        return -EINVAL;
    }

    /* get root inode */
    if (minfs_read_root_inode(root_inode))
    {
        fs_log_error("read minfs root inode failed\n");
        free(sbi);
        minfs_exit_sb_info(sb);
        iput(root_inode);
        return -EINVAL;
    }

    /* get the root dentry */
    sb->s_root = d_alloc_root(root_inode);
    if (sb->s_root == NULL)
    {
        fs_log_error("get root dentry failed\n");
        free(sbi);
        minfs_exit_sb_info(sb);
        iput(root_inode);
        return -EINVAL;
    }

    sb->s_root->d_op = &minfs_dentry_ops;

    strcpy(sb->s_volname, "minfs");

    return 0;
}

static void minfs_put_super(struct super_block *sb)
{
    minfs_exit_sb_info(sb);
}


static int minfs_statfs(struct super_block *sb, struct kstatfs *buf, __u32 flags)
{
    struct minfs_sb_info *sbi = MINFS_SB(sb);

    buf->f_type     = sb->s_magic;
    buf->f_bsize    = sb->s_blocksize;
    buf->f_blocks   = sbi->Size >> sbi->SectorBits;
    buf->f_bfree    = 0;
    buf->f_files    = sbi->DEntryNum;

    /* set fs attribute here */
    if (sb->s_flags & MS_RDONLY)
    {
        buf->f_attribute =  FS_ATTR_READONLY; /* readonly fs */
    }
    else
    {
        buf->f_attribute = 0;
    }

    strncpy(buf->f_fsname,  sb->s_fsname, MAX_FS_NAME_LEN);
    strncpy(buf->f_volname, sb->s_volname, MAX_FS_NAME_LEN);

    return 0;
}

int minfs_lock(struct super_block *sb)
{
    struct minfs_sb_info    *sbi = MINFS_SB(sb);
    __u8                    err = 0;

    //pend minfs super block semaphore
    esKRNL_SemPend(sbi->Sem, 0, &err);
    if (err)
    {
        fs_log_error("Pend sem error, error number:%d\n", err);
        return err;
    }
    return 0;
}

int minfs_unlock(struct super_block *sb)
{
    struct minfs_sb_info *sbi   = MINFS_SB(sb);

    //post minfs super block semaphore
    esKRNL_SemPost(sbi->Sem);
    return 0;
}

static void minfs_drop_inode(struct inode *inode)
{
    //for minfs, inode cache maybe exhaust the page pool,
    //you can't cache inode, so the inode must been completely destroyed,
    //By sunny, 2011-5-9 17:24:53.
    generic_delete_inode(inode);
}

const struct super_operations minfs_sops =
{
    .alloc_inode    = minfs_alloc_inode,
    .destroy_inode  = minfs_destroy_inode,
    .put_super      = minfs_put_super,
    .drop_inode     = minfs_drop_inode,
    .statfs         = minfs_statfs,
};
