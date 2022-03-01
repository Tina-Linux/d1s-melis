/*
************************************************************************************************************************
*                                                        CUCKOO
*                                                   the Audio Player
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : mem_dev.c
*
* Author      :
*
* Version     : 1.1.0
*
* Date        :
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*
*/
#include <log.h>
#include "mem_dev.h"

//不适合UI多线程
//create 和 delete必须成对调用

static GUI_MEMDEV_Handle g_memdev_hdl = NULL;

static __s32 __com_memdev_create(H_LYR hlyr, __s32 x, __s32 y, __s32 w, __s32 h)
{
    if (g_memdev_hdl)
    {
        __err("memdev already exist...\n");
        return EPDK_FAIL;
    }

    if (!hlyr)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(hlyr);
    g_memdev_hdl = GUI_MEMDEV_Create(x, y, w, h);

    if (!g_memdev_hdl)
    {
        return EPDK_FAIL;
    }

    GUI_MEMDEV_Select(g_memdev_hdl);
    return EPDK_OK;
}

__s32 com_memdev_create(H_LYR hlyr)
{
    __s32 ret = 0;
    RECT rect = {0};
    GUI_LyrWinGetScnWindow(hlyr, &rect);
    rect.x = 0;
    rect.y = 0;
    ret = __com_memdev_create(hlyr, rect.x, rect.y, rect.width, rect.height);
    return ret;
}

__s32 com_memdev_create_ex(H_LYR hlyr, __s32 x, __s32 y, __s32 w, __s32 h)
{
    __s32 ret;
    ret = __com_memdev_create(hlyr, x, y, w, h);
    return ret;
}

__s32 com_memdev_delete(void)
{
    if (!g_memdev_hdl)
    {
        //////__err("g_memdev_hdl is null...\n");   ///112350
        return EPDK_FAIL;
    }

    GUI_MEMDEV_CopyToLCD(g_memdev_hdl);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(g_memdev_hdl);
    g_memdev_hdl = NULL;
    return EPDK_OK;
}

