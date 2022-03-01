/*
************************************************************************************************************************
*                                                                auto off
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                       All Rights Reserved
*
* File Name   : dsk_auto_off.c
*
* Author      : Jackie.chen
*
* Version     : 1.0
*
* Date        : 2010.11.30
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
*Jackie.chen      2010.11.30        1.0         build the file
*
************************************************************************************************************************
*/

#include "functions_i.h"


__s32 dsk_set_auto_off_time(__u32 time)
{
    __gui_msg_t msg;
    __s32       ret;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_SET_AUTO_OFF;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = (__u32)time * 1000;
    msg.dwAddData2  = (__u32)0;

    ret = GUI_SendNotifyMessage(&msg);
    return ret;
}

__s32 dsk_power_off(void)
{
    __gui_msg_t msg;
    __s32       ret;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_POWER_OFF;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = (__u32)0;
    msg.dwAddData2  = (__u32)0;

    ret = GUI_SendNotifyMessage(&msg);
    return ret;
}

