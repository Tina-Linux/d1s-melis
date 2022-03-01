/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : rawpart.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-15
* Descript: raw partition system.
* Update  : date                auther      ver     notes
*           2011-3-15 15:02:30  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __RAWPART_H__
#define __RAWPART_H__

#include "part.h"

typedef struct __FSYS_RAWPART_P
{
    uint32_t            partaddr;      /* 分区在设备中的地址   */
    uint32_t            partseccnt;    /* 分区扇区数目大小     */
    uint32_t            partsecsize;   /* 分区扇区大小         */
} __fsys_rawpart_p_t;

#endif  /* __RAWPART_H__ */
