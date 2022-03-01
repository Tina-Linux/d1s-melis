/*
************************************************************************************************************************
*                                                     close screen
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_close_screen.c
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
#ifndef  __G_CLOSE_SCREEN_C__
#define  __G_CLOSE_SCREEN_C__

#include <log.h>
#include "../mod_init_i.h"
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
__s32 g_enable_close_scn(void)
{
    __gui_msg_t msg;
    __s32       ret;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_CLOSE_SCN_GATE_ON;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = 0;
    msg.dwAddData2  = 0;

    ret = GUI_SendNotifyMessage(&msg);
    return ret;
}



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
__s32 g_disable_close_scn(void)
{
    __gui_msg_t msg;
    __s32       ret;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_CLOSE_SCN_GATE_OFF;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = 0;
    msg.dwAddData2  = 0;

    ret = GUI_SendNotifyMessage(&msg);
    return ret;
}



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
__s32 g_set_close_scn_time(__u32 time)
{
    __gui_msg_t msg;
    __s32       ret;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_SET_SCN_CLOSE;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = (__u32)time * 1000;
    msg.dwAddData2  = (__u32)0;

    ret = GUI_SendNotifyMessage(&msg);

    __msg("g_set_close_scn_time, time=%d\n", time);
    return ret;
}

__s32 g_get_close_scn_time(void)
{
    __gui_msg_t msg;
    __s32       ret;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_GET_SCN_CLOSE;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = 0;
    msg.dwAddData2  = 0;

    ret = GUI_SendMessage(&msg);

    __msg("g_get_close_scn_time, time=%d\n", ret);
    return ret;
}

__s32 g_close_screen(void)
{
    __gui_msg_t msg;
    __s32       ret;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_SCREEN_CLOSE;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = (__u32)0;
    msg.dwAddData2  = (__u32)0;

    ret = GUI_SendNotifyMessage(&msg);
    return ret;
}

void g_enable_standby(void)
{
    __gui_msg_t msg;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_ENABLE_STANDBY;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = (__u32)0;
    msg.dwAddData2  = (__u32)0;

    GUI_SendMessage(&msg);
}

void g_disable_standby(void)
{
    __gui_msg_t msg;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_DISABLE_STANDBY;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = (__u32)0;
    msg.dwAddData2  = (__u32)0;

    GUI_SendMessage(&msg);
}

__bool g_is_enable_standby(void)
{
    __gui_msg_t msg;
    __s32       ret;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_GET_STANDBY_FLAG;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = (__u32)0;
    msg.dwAddData2  = (__u32)0;

    ret = GUI_SendMessage(&msg);
    return ret;
}

#endif     //  ifndef __DSK_CLOSE_SCREEN_C__

/* end of dsk_close_screen.c */
