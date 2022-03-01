/*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                               mod_mmp sub-system
*
*                                (c) Copyright 2007-2009, SoftWinners,
*                                               All Rights Reserved
*
* File   : eLIBs_GetVideoFmt.h
* Version: V1.0
* By     : robert_yuan  2009-1-8
* Modify : kevin.z.m, 2009-11-26 20:31
*********************************************************************************************************
*/
#ifndef _ELIBS_VIDEO_FORMAT_H_
#define _ELIBS_VIDEO_FORMAT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
************************************************************************************************************************
*                       GET VIDEO FILE FORMAT
*
*Description: get video file format.
*
*Arguments  : file      full path of video file.
*             format    file format information.
*
*Return     : result.
*               1:get video file format succssed.
*               0:get video file format failed.
************************************************************************************************************************
*/
extern __bool eLIBs_GetVideoFormat(const char *file, int *format);

#ifdef __cplusplus
}
#endif

#endif //_ELIBS_VIDEO_FORMAT_H_
