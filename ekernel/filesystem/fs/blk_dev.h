/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : blk_dev.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: block device operations.
* Update  : date                auther      ver     notes
*           2011-3-15 15:27:17  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __BLK_DEV_H__
#define __BLK_DEV_H__

int dev_hardsect_size(__hdle part);
int blksize_bits(int size);
int set_blocksize(__hdle part, int size);
int sb_set_blocksize(struct super_block *sb, int size);
int sb_min_blocksize(struct super_block *sb, int size);

#endif  /* __BLK_DEV_H__ */
