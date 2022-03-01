/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_i.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-26
* Descript: Mini rom image file system internal header file.
* Update  : date                auther      ver     notes
*           2011-3-26 14:15:28  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#ifndef __MINFS_I_H__
#define __MINFS_I_H__
#include "fs.h"
#include "errno.h"
#include "blk_dev.h"
#include "fstypedef.h"
#include "fsys_debug.h"
#include "minfs.h"
#include "slab.h"
#include "nls.h"
#include <common/romldr.h>

//mini filesystem super block magic
#define MINFS_SUPER_MAGIC       0x138F          //magic of minfs

#define MINFS_CACHE_MAXNUM      (8)             //max cache frame number
#define MINFS_CACHE_SIZE        (4096)          //one cache frame size
#define MINFS_CACHE_BITS        (12)            //one cache frame size bits
#define MINFS_INVALID_SECTOR    (0xffffffffUL)  //invalid sector number

#define MINFS_ALIGN(val, align) (((val) + ((align) - 1)) & ~((align) - 1))

typedef struct __MINFS_CACHE_ITEM
{
    __u32    Sector;
    __u8    *pData;
} __minfs_cache_item_t;

typedef struct __MINFS_CACHE
{
    __hdle               Sem;                           //semaphore controlling the lock
    __u32                CacheItemSectors;              //the sector number of one cache item
    __u32                MaxCacheNum;                   //cache数组个数
    __u32                CacheIndex;                    //cache轮换编号
    __u8                *pCacheBuffer;                  //cache缓冲区
    __minfs_cache_item_t CacheItems[MINFS_CACHE_MAXNUM];//cache数组
} __minfs_cache_t;

/*
 * minfs super-block data in memory
 */
struct minfs_sb_info
{
    __hdle          Sem;            //semaphore controlling the lock
    __u32           SectorSize;     //sector size
    __u32           SectorBits;     //sector size bits
    __u32           Size;           //the total size of minfs image
    __u32           RootDataOffet;  //root data area offset
    __u32           RootDirSize;    //the byte size of root directory
    __u32           DEntryNum;      //dentry number
    __u32           DEntryLen;      //dentry data area size
    __u8           *pDEntryData;    //buffer to store all dentrys data
    __minfs_cache_t Cache;          //device cache
};


/*
 * minfs inode data in memory
 */
struct minfs_inode_info
{
    __u32           Attribute;
    __u32           DEntryOffset;
    __u32           DataOffset;
    __u32           DataLen;
    struct inode    vfs_inode;
};

static inline struct minfs_sb_info *MINFS_SB(struct super_block *sb)
{
    return (struct minfs_sb_info *)(sb->s_fs_info);
}

static inline struct minfs_inode_info *MINFS_I(struct inode *inode)
{
    return list_entry(inode, struct minfs_inode_info, vfs_inode);
}

extern const struct inode_operations minfs_dir_inode_operations;
extern const struct file_operations  minfs_dir_operations;
extern const struct file_operations  minfs_file_operations;
extern const struct super_operations minfs_sops;
extern struct dentry_operations minfs_dentry_ops;

//minfs_inode.c
struct inode *minfs_iget(struct super_block *sb, __minfs_dentry_t *pdentry, __u32 dentry_offset);
int     minfs_read_root_inode(struct inode *inode);
void    minfs_destroy_inodecache(void);
int     minfs_init_inodecache(void);
struct inode *minfs_alloc_inode(struct super_block *sb);
void    minfs_destroy_inode(struct inode *inode);

//mifs_super.c
__s32   minfs_fill_super(struct super_block *sb, void *data, __s32 silent);
int     minfs_lock(struct super_block *sb);
int     minfs_unlock(struct super_block *sb);

//minfs_device.c
int     minfs_cache_init(struct super_block *sb);
int     minfs_cache_exit(struct super_block *sb);
__u32   minfs_pread(struct super_block *sb, void *buffer, __u32 pos, __u32 count);
void    *minfs_allocate_temp_buffer(struct super_block *sb, unsigned long len);
void    minfs_free_temp_buffer(struct super_block *sb, void *buffer, unsigned long len);
int     minfs_cache_lock(struct super_block *sb);
int     minfs_cache_unlock(struct super_block *sb);

//minfs_compress.c
__s32   minfs_uncompress(__u8 *pdst, unsigned long *dstLen, __u8 *psrcdata, unsigned long *srclen);


#endif  //__MINFS_I_H__
