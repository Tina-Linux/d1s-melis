/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                          az100 compress libary
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : az100.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-4-11
* Descript: az100 compress libary extern header file.
* Update  : date                auther      ver     notes
*           2011-4-11 14:41:02  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __ELIBS_AZ100_H__
#define __ELIBS_AZ100_H__

/*
*********************************************************************************************************
*                                    CHECK DATA BUFFER IS COMPRESSED OR NOT
*
* Description: check a input buffer is use compressed by az100 or not.
*
* Arguments  : pBuffer  :  the pointer of input buffer,
*              Size     :  the size of input buffer.
*
* Returns    : return EPDK_TRUE if compressed by az100, return EPDK_FALSE if not.
*
* Note       :
*********************************************************************************************************
*/
__bool AZ100_IsCompress(void *pBuffer, __u32 Size);

/*
*********************************************************************************************************
*                                       GET UNCOMPRESS SIZE
*
* Description: get the uncompress size of a az100 compressed buffer.
*
* Arguments  : pBuffer  :  the pointer of input buffer,
*              Size     :  the size of input buffer.
*
* Returns    : the size of uncompress buffer, 0 if failed.
*
* Note       :
*********************************************************************************************************
*/
__u32 AZ100_GetUncompressSize(void *pBuffer, __u32 Size);

/*
*********************************************************************************************************
*                                     UNCOMPRESS DATA BUFFER
*
* Description: uncompress a az100 compressed buffer.
*
* Arguments  : pSource      :  the pointer of input buffer,
*              SourceSize   :  the size of input buffer,
*              pDest        :  the pointer of output buffer,
*              DestSize     :  the size of output buffer.
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__s32 AZ100_Uncompress(void *pSource, __u32 SourceSize, void *pDest, __u32 DestSize);

#endif  //__ELIBS_AZ100_H__
