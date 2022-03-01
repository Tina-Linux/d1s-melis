/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                          az100 compress libary
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : az100_i.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-4-11
* Descript: az100 compress libary internal header file.
* Update  : date                auther      ver     notes
*           2011-4-11 14:49:16  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __AZ100_I_H__
#define __AZ100_I_H__
#include <typedef.h>
#include "compress.h"

//the magic header of az100 compressed file
#define  AZ100_MAGIC        "AZ10"
#define  AZ100_MAGIC_LEN    4
#define  AZ100_HEADER_LEN   (4 + 4) //magic + uncompress_len

#endif  //__AZ100_I_H__
