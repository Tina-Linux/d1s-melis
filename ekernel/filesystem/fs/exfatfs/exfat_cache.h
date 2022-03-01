/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : exfat_cache.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-16
* Descript: exfat file system cache handing functions.
* Update  : date                auther      ver     notes
*           2011-3-16 14:21:29  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef _EXFAT_CACHE_H_
#define _EXFAT_CACHE_H_

#define EXFAT_CACHE_VALID       0   /* special case for valid cache */

/* This must be > 0. */
#define EXFAT_MAX_CACHE         8

#define exfat_cache_entry(x)    list_entry(x, struct exfat_cache, cache_list)

struct exfat_cache
{
    struct list_head cache_list;
    u32 iclusnr;        /* cluster number on data of inode. */
    u32 clusnr;         /* cluster number on disk. */
    u32 len;            /* number of contiguous clusters */
};

struct exfat_cache_id
{
    u32 id;         /* cache generation id */
    u32 iclusnr;        /* cluster number on data of inode. */
    u32 clusnr;         /* cluster number on disk. */
    u32 len;            /* number of contiguous clusters */
};

static inline void exfat_cache_init(struct exfat_cache_id *cid,
                                    u32 iclusnr, u32 clusnr)
{
    cid->id = EXFAT_CACHE_VALID;
    cid->iclusnr = iclusnr;
    cid->clusnr = clusnr;
    cid->len = 1;
}

static inline int exfat_cache_contiguous(struct exfat_cache_id *cid, u32 clusnr)
{
    return (cid->clusnr + cid->len) == clusnr;
}

int  exfat_init_cache_cachep(void);
void exfat_destroy_cache_cachep(void);
int  exfat_cache_lookup(struct inode *inode, u32 iclusnr,
                        struct exfat_cache_id *cid);
void exfat_cache_add(struct inode *inode, struct exfat_cache_id *new);
void exfat_cache_inval(struct inode *inode);
void exfat_cache_inode_init_once(struct exfat_inode_info *exi);

#endif /* _EXFAT_CACHE_H_ */
