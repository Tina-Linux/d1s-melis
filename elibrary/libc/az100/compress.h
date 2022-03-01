/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                           az100 compress libary
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : compress.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-4-11
* Descript: compress wapper interfaces.
* Update  : date                auther      ver     notes
*           2011-4-11 14:27:39  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __COMPRESS_H__
#define __COMPRESS_H__

#define MAX_EXPAND_RATIO 4

__s32 AZ100_DataCompress(__u8 *pDst, __u32 *DstLen, __u8 *pSrc, __u32 SrcLen);
__s32 AZ100_DataUncompress(__u8 *pDst, unsigned long DstLen, __u8 *pSrcData, unsigned long SrcLen);

#endif  // __COMPRESS_H__
