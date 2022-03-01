/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : cdfs_compress.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-16
* Descript: iso9660(cdfs) file system decompression.
* Update  : date                auther      ver     notes
*           2011-3-16 13:32:35  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

/*
 * Prototypes for functions exported from the compressed isofs subsystem
 */
#ifndef     __CDFS_COMPRESS_H__
#define     __CDFS_COMPRESS_H__

#ifdef CONFIG_ZISOFS
extern const struct address_space_operations zisofs_aops;
extern int  zisofs_init(void);
extern void zisofs_cleanup(void);
#endif  /* CONFIG_ZISOFS */

#endif      /* __CDFS_COMPRESS_H__ */

