/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : exfat_cluster.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-16
* Descript: exfat file system cluster handing functions.
* Update  : date                auther      ver     notes
*           2011-3-16 14:21:29  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __EXFAT_CLUSTER_H__
#define __EXFAT_CLUSTER_H__

struct exfat_clus_map
{
    u32 iclusnr;        /* cluster number on data of inode. */
    u32 clusnr;         /* cluster number on disk. */
    u32 len;            /* number of contiguous clusters */
};

int exfat_get_cluster(struct inode *inode, u32 iclusnr, u32 clus_len,
                      struct exfat_clus_map *cmap);
int exfat_alloc_clusters(struct inode *inode, int *cluster,
                         int nr_cluster, u8 *data_flag);
int exfat_free_clusters(struct inode *inode, int cluster,
                        int nr_cluster, u8 data_flag);
int exfat_chain_add(struct inode *inode, int clus,
                    int nr_clus, u8 data_flag);
int exfat_free(struct inode *inode, int skip, int nr_clus);

#endif /* __EXFAT_CLUSTER_H__ */
