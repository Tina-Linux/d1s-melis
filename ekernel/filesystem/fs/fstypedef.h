/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : fstypedef.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: code is extracted from linux.
* Update  : date                auther      ver     notes
*           2011-3-15 15:43:08  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef     __FS_TYPEDEF_H__
#define     __FS_TYPEDEF_H__


#define     __bitwise


/* fs sub system typedef */
#ifndef DATA_TYPE_X___u64
#define DATA_TYPE_X___u64
typedef     unsigned long long        __u64;
#endif

#ifndef DATA_TYPE_X___u32
#define DATA_TYPE_X___u32
typedef     unsigned int              __u32;
#endif

#ifndef DATA_TYPE_X___u16
#define DATA_TYPE_X___u16
typedef     unsigned short            __u16;
#endif

#ifndef DATA_TYPE_X___u8
#define DATA_TYPE_X___u8
typedef     unsigned char             __u8;
#endif

#ifndef DATA_TYPE_X___s64
#define DATA_TYPE_X___s64
typedef     signed long long          __s64;
#endif

#ifndef DATA_TYPE_X___s32
#define DATA_TYPE_X___s32
typedef     signed int                __s32;
#endif

#ifndef DATA_TYPE_X___s16
#define DATA_TYPE_X___s16
typedef     signed short              __s16;
#endif

#ifndef DATA_TYPE_X___s8
#define DATA_TYPE_X___s8
typedef     signed char               __s8;
#endif


#ifndef DATA_TYPE_X_u64
#define DATA_TYPE_X_u64
typedef     __u64                     u64;
#endif

#ifndef DATA_TYPE_X_u32
#define DATA_TYPE_X_u32
typedef     __u32                     u32;
#endif

#ifndef DATA_TYPE_X_u16
#define DATA_TYPE_X_u16
typedef     __u16                     u16;
#endif

#ifndef DATA_TYPE_X_u8
#define DATA_TYPE_X_u8
typedef     __u8                      u8;
#endif

#ifndef DATA_TYPE_X_s64
#define DATA_TYPE_X_s64
typedef     __s64                     s64;
#endif

#ifndef DATA_TYPE_X_s32
#define DATA_TYPE_X_s32
typedef     __s32                     s32;
#endif

#ifndef DATA_TYPE_X_s16
#define DATA_TYPE_X_s16
typedef     __s16                     s16;
#endif

#ifndef DATA_TYPE_X_s8
#define DATA_TYPE_X_s8
typedef     __s8                      s8;
#endif

#ifndef DATA_TYPE_X___le16
#define DATA_TYPE_X___le16
typedef     short int                 __le16;
#endif

#ifndef DATA_TYPE_X___le32
#define DATA_TYPE_X___le32
typedef     int                       __le32;
#endif

#ifndef DATA_TYPE_X___le64
#define DATA_TYPE_X___le64
typedef     long long                 __le64;
#endif

#ifndef DATA_TYPE_X_le16
#define DATA_TYPE_X_le16
typedef     unsigned short int        le16;
#endif

#ifndef DATA_TYPE_X_le32
#define DATA_TYPE_X_le32
typedef     unsigned int              le32;
#endif

#ifndef DATA_TYPE_X_le64
#define DATA_TYPE_X_le64
typedef     unsigned long long        le64;
#endif


#ifndef DATA_TYPE_X___be16
#define DATA_TYPE_X___be16
typedef     __u16                     __be16;
#endif

#ifndef DATA_TYPE_X___be32
#define DATA_TYPE_X___be32
typedef     __u32                     __be32;
#endif

#ifndef DATA_TYPE_X___be64
#define DATA_TYPE_X___be64
typedef     __u64                     __be64;
#endif

#ifndef DATA_TYPE_X_be16
#define DATA_TYPE_X_be16
typedef     __u16                     be16;
#endif

#ifndef DATA_TYPE_X_be32
#define DATA_TYPE_X_be32
typedef     __u32                     be32;
#endif

#ifndef DATA_TYPE_X_be64
#define DATA_TYPE_X_be64
typedef     __u64                     be64;
#endif



typedef     __u8        u_int8_t;
typedef     __s8        int8_t;
typedef     __u16       u_int16_t;
typedef     __s16       int16_t;
typedef     __u32       u_int32_t;
typedef     __u8        uint8_t;
typedef     __u16       uint16_t;
typedef     __u64       u_int64_t;
#ifdef CONFIG_ARM
typedef     __s64       int64_t;
#endif


//typedef     unsigned long   dev_t;
//typedef     unsigned int    uid_t;
//typedef     unsigned int    gid_t;
typedef     unsigned short  umode_t;
//typedef     unsigned short  mode_t;
#ifdef CONFIG_ARM
typedef     unsigned int    size_t;
#endif
typedef     signed long long  loff_t;
typedef     long            off_t;
//typedef     long            clock_t;

#ifdef      CONFIG_LBD
typedef     u64             sector_t;
#else
typedef     unsigned long   sector_t;
#endif

/* some basic macro define */
#define ARRAY_SIZE(x)       (sizeof(x) / sizeof((x)[0]))
#define DIV_ROUND_UP(n,d)   (((n) + (d) - 1) / (d))

#endif      /* __FS_TYPEDEF_H__ */
