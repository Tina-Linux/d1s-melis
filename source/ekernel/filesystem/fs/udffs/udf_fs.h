/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : udf_fs.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-17
* Descript: OSTA-UDF(tm) = Optical Storage Technology Association
*           Universal Disk Format.
*           This code is based on version 2.50 of the UDF specification,
*           and revision 3 of the ECMA 167 standard [equivalent to ISO 13346].
*           http://www.osta.org/
*           http://www.ecma.ch/
*           http://www.iso.org/
*
* Update  : date                auther      ver     notes
*           2011-3-17 9:00:19   Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __UDF_FS_H__
#define __UDF_FS_H__

#define UDF_PREALLOCATE
#define UDF_DEFAULT_PREALLOC_BLOCKS 8

#define UDFFS_DATE          "2004/29/09"
#define UDFFS_VERSION       "0.9.8.1"

/* udf debug compile option */
//#define UDFFS_DEBUG

#ifdef UDFFS_DEBUG
#define udf_debug(f, a...) \
    { \
        printk("UDF-fs DEBUG %s:%d:%s: ", \
               __FILE__, __LINE__, __FUNCTION__); \
        printk(f, ##a); \
    }

#define udf_info(f, a...) \
    printk("UDF-fs INFO " f, ##a);

#define udf_error(f, a...) \
    { \
        printk("UDF-fs error %s:%d:%s: ", \
               __FILE__, __LINE__, __FUNCTION__); \
        printk(f, ##a); \
    }

#define udf_warning(f, a...) \
    { \
        printk("UDF-fs warning %s:%d:%s: ", \
               __FILE__, __LINE__, __FUNCTION__); \
        printk(f, ##a); \
    }

#else
#define udf_debug(f, a...)
#define udf_info(f, a...)
#define udf_error(f, a...)
#define udf_warning(f, a...)
#endif

#endif /* __UDF_FS_H__ */
