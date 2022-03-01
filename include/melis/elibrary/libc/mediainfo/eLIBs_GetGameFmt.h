/*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                               mod_mmp sub-system
*
*                                (c) Copyright 2007-2009, SoftWinners, kevin.z
*                                               All Rights Reserved
*
* File   : eLIBs_GetGameFmt.h
* Version: V1.0
* By     : kevin.z.m, 2009-11-27 10:03
*********************************************************************************************************
*/
#ifndef _ELIBS_GAME_FORMAT_H_
#define _ELIBS_GAME_FORMAT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
************************************************************************************************************************
*                       GET GAME FILE FORMAT
*
*Description: get game file format.
*
*Arguments  : file      full path of game file.
*             format    file format information.
*
*Return     : result.
*               1:get game file format succssed.
*               0:get game file format failed.
************************************************************************************************************************
*/
extern __bool eLIBs_GetGameFormat(const char *file, int *format);

#ifdef __cplusplus
}
#endif

#endif //_ELIBS_GAME_FORMAT_H_
