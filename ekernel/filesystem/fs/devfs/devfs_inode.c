/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : dev_inode.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-16
* Descript: device file system inode handing functions.
* Update  : date                auther      ver     notes
*           2011-3-16 14:05:49  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "devfs.h"
#include "errno.h"
#include "slab.h"
#include "nls.h"
#include "fsys_debug.h"

extern struct inode_operations devfs_dir_inode_operations;
extern struct file_operations devfs_dir_operations;
extern struct inode_operations devfs_file_inode_operations;
extern struct file_operations devfs_file_operations;
extern struct dentry_operations devfs_dentry_ops[2];

static int devfs_read_root(struct inode *inode)
{
    struct super_block *sb = inode->i_sb;
    struct devfs_sb_info *sbi = DEVFS_SB(sb);

    inode->i_version++;
    inode->i_generation = 0;
    inode->i_mode = S_IFDIR;
    inode->i_op = sbi->dir_ops;
    inode->i_fop = &devfs_dir_operations;

    inode->i_nlink = 2;

    esFSYS_pioctrl(inode->i_sb->s_part, PART_IOC_USR_GETDEVCLSROOT, 0,
                   &(DEVFS_I(inode)->i_hnode));

    if (!DEVFS_I(inode)->i_hnode)
    {
        return -EIO;
    }

    return 0;
}

static void init_once(void *foo, kmem_cache_t *cachep, unsigned long flags)
{
    struct devfs_inode_info *ei = (struct devfs_inode_info *)foo;

    INIT_LIST_HEAD(&ei->i_list);
    inode_init_once(&ei->vfs_inode);
}

static kmem_cache_t *devfs_inode_cachep;
int devfs_init_inodecache(void)
{
    devfs_inode_cachep = kmem_cache_create("devfs_inode_cache",
                                           sizeof(struct devfs_inode_info),
                                           0, SLAB_HWCACHE_ALIGN,
                                           init_once, NULL);
    if (devfs_inode_cachep == NULL)
    {
        return -ENOMEM;
    }
    return 0;
}

static void devfs_destroy_inodecache(void)
{
    kmem_cache_destroy(devfs_inode_cachep);
}

static struct inode *devfs_alloc_inode(struct super_block *sb)
{
    struct devfs_inode_info *ei;

    ei = kmem_cache_alloc(devfs_inode_cachep, GFP_KERNEL);
    if (!ei)
    {
        return NULL;
    }

    return &ei->vfs_inode;
}

struct inode *devfs_iget(struct super_block *sb, unsigned int key)
{
    struct devfs_sb_info *sbi = DEVFS_SB(sb);
    struct devfs_inode_info *i;
    struct inode *inode = NULL;

    /* 在私有超级块的设备inode链中顺序比较查找inode节点是否存在 */
    list_for_each_entry(i, &(sbi->inode_list), i_list)
    {
        BUG_ON(i->vfs_inode.i_sb != sb);
        if (i->i_key != key)
        {
            continue;
        }
        inode = igrab(&i->vfs_inode);
        if (inode)
        {
            break;
        }
    }
    return inode;
}

static int devfs_fill_inode(struct inode *inode, dms_node_info_fs_t *dinfo)
{
    struct devfs_sb_info *sbi = DEVFS_SB(inode->i_sb);

    DEVFS_I(inode)->i_hnode = dinfo->hnode;
    inode->i_version++;
    inode->i_generation = 0;//get_seconds();

    /* 填充设备类节点 */
    if ((dinfo->type == DEVFS_CLASS_TYPE))
    {
        inode->i_generation &= ~1;
        inode->i_mode = S_IFDIR;
        inode->i_op = sbi->dir_ops;
        inode->i_fop = &devfs_dir_operations;

        inode->i_nlink = 2;
    }
    /* 填充设备节点 */
    else   /* not a directory */
    {
        inode->i_generation |= 1;
        inode->i_mode = S_IFREG;
        inode->i_op = &devfs_file_inode_operations;
        inode->i_fop = &devfs_file_operations;

        inode->i_nlink = 1;
    }

    return 0;
}

struct inode *devfs_build_inode(struct super_block *sb, dms_node_info_fs_t *dinfo)
{
    struct inode *inode;
    struct devfs_inode_info *devi;
    int err;

    /* 扫描设备inode链，看待创建的inode是否已经被建立 */
    inode = devfs_iget(sb, dinfo->key);
    if (inode)
    {
        goto out;
    }

    /* 申请设备inode空间 */
    inode = new_inode(sb);
    if (!inode)
    {
        inode = ERR_PTR(-ENOMEM);
        goto out;
    }
    inode->i_ino = iunique(sb, DEVFS_ROOT_INO);
    inode->i_version = 1;

    /* 填充inode内容 */
    err = devfs_fill_inode(inode, dinfo);
    if (err)
    {
        iput(inode);
        inode = ERR_PTR(err);
        goto out;
    }

    /* 加入新建立的设备inode到超级块的inode链中 */
    list_add(&(DEVFS_I(inode)->i_list), &(DEVFS_SB(sb)->inode_list));
    /* 加入新建立的vfsinode到虚拟文件系统的hash表中 */
    insert_inode_hash(inode);

out:
    return inode;
}

static int devfs_statfs(struct super_block *sb, struct kstatfs *buf, __u32 flags)
{
    /* flags nouse under devfs */

    buf->f_type = sb->s_magic;
    buf->f_bsize = 0;
    buf->f_blocks = 0;
    buf->f_bfree = 0;
    buf->f_namelen = 32;
    strncpy(buf->f_fsname, sb->s_fsname, MAX_FS_NAME_LEN);
    strncpy(buf->f_volname, sb->s_volname, MAX_FS_NAME_LEN);

    return 0;
}

static void devfs_destroy_inode(struct inode *inode)
{
    kmem_cache_free(devfs_inode_cachep, DEVFS_I(inode));
}

static void devfs_clear_inode(struct inode *inode)
{
    struct devfs_sb_info *sbi = DEVFS_SB(inode->i_sb);

    if (is_bad_inode(inode))
    {
        return;
    }
    list_del_init(&DEVFS_I(inode)->i_list);
}

static void devfs_drop_inode(struct inode *inode)
{
    /* in devfs, you can't cache inode,
     * so the inode must been completely destroyed,
     * By sunny, 2009.8.19 */
    generic_delete_inode(inode);
}

/* ------------------- sb ops -------------------------------*/
static const struct super_operations devfs_sops =
{
    .alloc_inode    = devfs_alloc_inode,
    .destroy_inode  = devfs_destroy_inode,
    .clear_inode    = devfs_clear_inode,
    .drop_inode     = devfs_drop_inode,
    .statfs         = devfs_statfs,
};

static int devfs_default_codepage = 936;
static char devfs_default_iocharset[] = "cp936";
__s32 devfs_fill_super(struct super_block *sb, void *data, __s32 silent)
{
    struct inode *root_inode = NULL;
    struct devfs_sb_info *sbi;
    unsigned int media;
    long error;
    char buf[50];

    /* 申请私有超级块空间 */
    sbi = calloc(sizeof(struct devfs_sb_info), 1);
    if (!sbi)
    {
        return -ENOMEM;
    }
    sb->s_fs_info = sbi;

    /* 初始化超级块 */
    sb->s_flags |= MS_NODIRATIME;// | MS_SYNCHRONOUS;   //debug for MS_SYNCHRONOUS
    sb->s_magic = DEVFS_SUPER_MAGIC;
    sb->s_op = &devfs_sops;
    sb->s_blocksize_bits = 9;
    sb->s_blocksize = 512;
    sbi->dir_ops = &devfs_dir_inode_operations;
    INIT_LIST_HEAD(&sbi->inode_list);

    /* 挂接字符集 */

    /* 建立根目录inode节点 */
    error = -ENOMEM;
    root_inode = new_inode(sb);
    if (!root_inode)
    {
        goto out_fail;
    }
    root_inode->i_ino = 1;
    root_inode->i_version = 1;
    error = devfs_read_root(root_inode);
    if (error < 0)
    {
        goto out_fail;
    }

    /* 建立根目录dentry节点 */
    error = -ENOMEM;
    insert_inode_hash(root_inode);
    sb->s_root = d_alloc_root(root_inode);
    if (!sb->s_root)
    {
        fs_log_error("DEVFS: get root inode failed\n");
        goto out_fail;
    }
    sb->s_root->d_op = &devfs_dentry_ops[0];

    return 0;

    error = -EINVAL;
    if (!silent)
        fs_log_info("VFS: Can't find a valid DEVFS filesystem"
                    " on dev %c.\n", sb->s_letter);

out_fail:
    if (root_inode)
    {
        iput(root_inode);
    }
    free(sbi);
    sb->s_fs_info = NULL;
    return error;
}



