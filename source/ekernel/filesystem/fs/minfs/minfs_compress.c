/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_compress.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-29
* Descript: Mini rom image file system uncompress handing fucntion.
* Update  : date                auther      ver     notes
*           2011-3-29 22:12:04  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "minfs_i.h"
#include "lzma/LzmaLib.h"

__s32 minfs_uncompress(__u8 *pdst, unsigned long *dstLen, __u8 *psrcdata, unsigned long *srclen)
{
    __u8   *pcompressdata;

    //uncompress data
    pcompressdata   = psrcdata + 5;
    (*srclen)       -= (5);

    if (LzmaUncompress(pdst, (size_t *)dstLen, pcompressdata, (size_t *)srclen, psrcdata, 5))
    {
        fs_log_error("lzma uncompress data failed\n");
        return -EIO;
    }
    return 0;
}
