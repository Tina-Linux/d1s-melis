/*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                               mod_mmp sub-system
*
*                                (c) Copyright 2007-2009, SoftWinners,
*                                               All Rights Reserved
*
* File   : eLIBs_GetFlashFmt.h
* Version: V1.0
* By     : kevin
* Date   : 2011-1-4 19:39
*********************************************************************************************************
*/
#ifndef _ELIBS_FLASH_FORMAT_H_
#define _ELIBS_FLASH_FORMAT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
************************************************************************************************************************
*                       GET FLASH FILE FORMAT
*
*Description: get flash file format.
*
*Arguments  : file      full path of flash file.
*             format    file format information.
*
*Return     : result.
*               1:get flash file format succssed.
*               0:get flash file format failed.
************************************************************************************************************************
*/
extern __bool eLIBs_GetFlashFormat(const char *file, int *format);

#ifdef __cplusplus
}
#endif

#endif //_ELIBS_FLASH_FORMAT_H_
