/*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                               mod_mmp sub-system
*
*                                (c) Copyright 2007-2009, SoftWinners, kevin.z.m
*                                               All Rights Reserved
*
* File   : eLIBs_GetEbookFmt.h
* Version: V1.0
* By     : kevin.z.m, 2009-11-27 10:02
*********************************************************************************************************
*/
#ifndef _ELIBS_EBOOK_FORMAT_H_
#define _ELIBS_EBOOK_FORMAT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
************************************************************************************************************************
*                       GET EBOOK FILE FORMAT
*
*Description: get ebook file format.
*
*Arguments  : file      full path of ebook file.
*             format    file format information.
*
*Return     : result.
*               1:get ebook file format succssed.
*               0:get ebook file format failed.
************************************************************************************************************************
*/
extern __bool eLIBs_GetEbookFormat(const char *file, int *format);

#ifdef __cplusplus
}
#endif

#endif //_ELIBS_EBOOK_FORMAT_H_
