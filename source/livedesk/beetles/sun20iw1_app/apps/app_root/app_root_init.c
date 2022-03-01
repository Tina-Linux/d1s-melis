/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : live_init.c
* By        : Andy.zhang
* Func      : Live main res init
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include <log.h>
#include "beetles_app.h"

GUI_FONT    *SWFFont = NULL;

/**
 * 初始化 系统资源
 */
int32_t app_root_init_res(void)
{
    int32_t     ret;

    rat_init();

	SWFFont     = GUI_SFT_CreateFont(BEETLES_FONT_SIZE_INT, BEETLES_APP_ROOT"res\\fonts\\font16.sft");
    if (SWFFont == NULL)
    {
        __err("create font fail...\n");
        return EPDK_FAIL;
    }

    GUI_SetFont(SWFFont);
    return EPDK_OK;
}

/**
 * 释放 系统资源
 */
int32_t app_root_deinit_res(void)
{
    rat_deinit();

    if (SWFFont)
    {
        GUI_SetFont(GUI_GetDefaultFont());
        GUI_SFT_ReleaseFont(SWFFont);
        SWFFont = NULL;
    }

    return EPDK_OK;
}
