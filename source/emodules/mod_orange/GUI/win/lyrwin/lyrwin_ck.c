/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              orange Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : win
* File    : lyr_ck.c
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-4 11:24:32
**********************************************************************************************************************
*/
#include "lyrwin_i.h"
#include <kconfig.h>
#include <log.h>
#ifdef CONFIG_SOC_SUN3IW1
#include <mod_display.h>
#endif


/*
**********************************************************************************************************************
*                                               GUI_LyrCKOn
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 GUI_LyrWinCKOn(H_LYR h_lyr)
{
    __s32                    ret = EPDK_FAIL;
#ifdef CONFIG_SOC_SUN3IW1
    __gui_lyrwin_t           *lyr_attr   = (__gui_lyrwin_t *)h_lyr;
    __hdle                   dispbuffer[3] = {NULL, };
#if GUI_CHECK_ARG_EN > 0

    if (!lyr_attr)
    {
        __wrn("h_lyr empty!");
        return EPDK_FAIL;
    }

#endif
    GUI_LyrLock();

    if (lyr_attr->h_hwlyr)
    {
        /*modified by Derek,2010-12-9 19:07:13*/
        dispbuffer[0] = lyr_attr->h_hwlyr;
        dispbuffer[1] = NULL;
        dispbuffer[2] = NULL;
        ret = eLIBs_fioctrl(lyr_attr->devfp, DISP_CMD_LAYER_CK_ON, SEL_SCREEN, (void *)dispbuffer); //end modified

        if (ret != EPDK_OK)
        {
            __wrn("layer set top failed!");
        }
        else
        {
            lyr_attr->lyrpara.ck_mode = EPDK_TRUE;
        }

#ifdef SIM_PC_WIN
        RefreshDeContent(1);
#endif
    }

    GUI_LyrUnlock();
#endif
    return ret;
}

/*
**********************************************************************************************************************
*                                               GUI_LyrCKOff
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 GUI_LyrWinCKOff(H_LYR h_lyr)
{
    __s32                    ret = EPDK_FAIL;
#ifdef CONFIG_SOC_SUN3IW1
    __gui_lyrwin_t           *lyr_attr   = (__gui_lyrwin_t *)h_lyr;
    __hdle                   dispbuffer[3] = { NULL, };
#if GUI_CHECK_ARG_EN > 0

    if (!lyr_attr)
    {
        __wrn("h_lyr empty!");
        return EPDK_FAIL;
    }

#endif
    GUI_LyrLock();

    if (lyr_attr->h_hwlyr)
    {
        /*modified by Derek,2010-12-9 19:13:54*/
        dispbuffer[0] = lyr_attr->h_hwlyr;
        dispbuffer[1] = NULL;
        dispbuffer[2] = NULL;
        ret = eLIBs_fioctrl(lyr_attr->devfp, DISP_CMD_LAYER_CK_OFF, SEL_SCREEN, (void *)dispbuffer); //end modified

        if (ret != EPDK_OK)
        {
            __wrn("layer set top failed!");
        }
        else
        {
            lyr_attr->lyrpara.ck_mode = EPDK_FALSE;
        }

#ifdef SIM_PC_WIN
        RefreshDeContent(1);
#endif
    }

    GUI_LyrUnlock();
#endif
    return ret;
}
