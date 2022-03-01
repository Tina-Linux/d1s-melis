/*
************************************************************************************************************************
*                                                     update firmware
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_fw_update.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.09.01
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2010.09.01        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_FW_UPDATE_H__
#define  __DSK_FW_UPDATE_H__

#ifdef __cplusplus
extern "C" {
#endif


/*
****************************************************************************************************
*Name        : dsk_fw_update
*Prototype   : __s32 dsk_fw_update( const char *file )
*Arguments   : file        input. the fireware file's full path.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : update system with the fireware file.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_fw_update(void);

extern __s32 dsk_start_fw_update(const char *path);

#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_FW_UPDATE_H__

/* end of dsk_fw_update.h */
