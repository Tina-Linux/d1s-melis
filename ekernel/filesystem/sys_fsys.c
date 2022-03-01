/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : sys_fsys.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-15
* Descript: file system sevice interfaces.
* Update  : date                auther      ver     notes
*           2011-3-15 14:38:27  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "sys_fsys_i.h"
#include <kconfig.h>

/*
*********************************************************************************************************
*                                           FSYS INIT
*
* Description: file system initialize.
*
* Arguments  : NONE
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
uint32_t fsys_init(void)
{
    fsys_vpart_init();

    fsys_vfs_init();
    /* initialize dev file system */
    fsys_devfs_init();

    /* initialize cdfs file system */
#ifdef CONFIG_CDFS
    fsys_cdfs_init();
#endif

    /* initialize udffs file system */
#ifdef CONFIG_UDFFS
    fsys_udffs_init();
#endif

    /* initialize exFAT file system */
#ifdef CONFIG_EXFAT
    fsys_exfatfs_init();
#endif

    /* initialize ntfs file system */
#ifdef CONFIG_NTFS
    fsys_ntfs_init();
#endif

    /* initialize fat file system */
#ifdef CONFIG_FAT
    fsys_fatfs_init();
#endif

    /* initialize mini file system */
#ifdef CONFIG_MINFS
    fsys_minfs_init();
#endif
    fsys_dmspart_init();
    fsys_rawpart_init();
    fsys_dospart_init();

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                           FSYS EXIT
*
* Description: file system exit.
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__u32 FSYS_Exit(void)
{
    fsys_dospart_exit();
    fsys_rawpart_exit();
    fsys_dmspart_exit();

    /* exit udf file system */
#ifdef CONFIG_UDFFS
    fsys_udffs_exit();
#endif

    /* exit cdfs file system */
#ifdef CONFIG_CDFS
    fsys_cdfs_exit();
#endif

    /* exit exfat file system */
#ifdef CONFIG_EXFAT
    fsys_exfatfs_exit();
#endif

    /* exit ntfs file system */
#ifdef CONFIG_NTFS
    fsys_ntfs_exit();
#endif

    /* exit fat file system */
#ifdef CONFIG_FAT
    fsys_fatfs_exit();
#endif

    /* exit mini file system */
    fsys_minfs_exit();

    /* exit dev file system */
    fsys_devfs_exit();

    fsys_vfs_exit();
    fsys_vpart_exit();

    return EPDK_OK;
}
