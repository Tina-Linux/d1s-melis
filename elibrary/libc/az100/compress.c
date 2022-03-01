/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                           az100 compress libary
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : compress.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-4-11
* Descript: compress wapper interfaces.
* Update  : date                auther      ver     notes
*           2011-4-11 14:29:31  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "az100_i.h"
#include "lzma/LzmaLib.h"
#include <log.h>

/*
*********************************************************************************************************
*                                       FULL FUNCTION NAME
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__s32 AZ100_DataUncompress(__u8 *pDst, unsigned long DstLen, __u8 *pSrcData, unsigned long SrcLen)
{
    __u8   *pCompressData;
    //uncompress data
    pCompressData  = pSrcData + LZMA_PROPS_SIZE;
    SrcLen        -= (LZMA_PROPS_SIZE);

    if (LzmaUncompress(pDst, (size_t *)&DstLen, pCompressData, (size_t *)&SrcLen, pSrcData, LZMA_PROPS_SIZE))
    {
        __msg("lzma uncompress data failed\n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}
