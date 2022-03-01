/*
************************************************************************************************************************
*                                                     close screen
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : g_close_screen.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.25
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
* Gary.Wang      2010.10.25        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __G_CLOSE_SCREEN_H__
#define  __G_CLOSE_SCREEN_H__

#include <kconfig.h>
/*
****************************************************************************************************
*Name        : dsk_enable_close_scn
*Prototype   : __s32 dsk_enable_close_scn( void )
*Arguments   : void
*Return      : EPDK_OK    succeed
*              EDPK_FAIL  fail
*Description : allow closing screen.
*Other       :
****************************************************************************************************
*/
__s32 g_enable_close_scn(void);

/*
****************************************************************************************************
*Name        : dsk_disable_close_scn
*Prototype   : __s32 dsk_disable_close_scn( void )
*Arguments   : void
*Return      : EPDK_OK    succeed
*              EDPK_FAIL  fail
*Description : forbid closing screen.
*Other       :
****************************************************************************************************
*/
__s32 g_disable_close_scn(void);

/*
****************************************************************************************************
*Name        : dsk_set_close_scn_time
*Prototype   : __s32 dsk_set_close_scn_time(__u32 time)
*Arguments   : time       input. time, unit:s.
*Return      : EPDK_OK    succeed
*              EDPK_FAIL  fail
*Description : set the time for closing screen.
*Other       :
****************************************************************************************************
*/
__s32 g_set_close_scn_time(__u32 time);

__s32 g_get_close_scn_time(void);

__s32 g_close_screen(void);

void g_enable_standby(void);

void g_disable_standby(void);

__bool g_is_enable_standby(void);

#endif     //  ifndef __DSK_CLOSE_SCREEN_C__

/* end of dsk_close_screen.c */
