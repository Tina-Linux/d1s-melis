/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lion
* File    : lyr_setbottom.c
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-4 11:24:32
**********************************************************************************************************************
*/

#include "lyrwin_i.h"
#include <mod_display.h>

/*
**********************************************************************************************************************
*                                               GUI_LyrSetBottom
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
__s32 GUI_LyrWinSetAlpha(H_LYR h_lyr, __u8 alpha)
{
    __s32                   ret = EPDK_FAIL;
    __gui_lyrwin_t           *lyr_attr   = (__gui_lyrwin_t *)h_lyr;
    __u32 dispbuffer[3];
#if GUI_CHECK_ARG_EN > 0

    if (!lyr_attr)
    {
        __wrn("h_lyr empty!\n");
        return EPDK_FAIL;
    }

#endif
    GUI_LyrLock();

    if (lyr_attr->h_hwlyr)
    {
        __u8 alpha_tmp = alpha;
        /*modified by Derek,2010-12-9 19:02:27*/
        dispbuffer[0] = (unsigned long)(lyr_attr->h_hwlyr);
        dispbuffer[1] = alpha_tmp;
        dispbuffer[2] = 0;
        //ret = eLIBs_fioctrl(lyr_attr->devfp, DISP_CMD_LAYER_SET_ALPHA_VALUE, SEL_SCREEN, (void *)dispbuffer); //end modified

        if (ret != EPDK_OK)
        {
            __wrn("layer set top failed!\n");
        }
        else
        {
            lyr_attr->lyrpara.alpha_val = alpha;
        }

#ifdef SIM_PC_WIN
        RefreshDeContent(1);
#endif
    }

    GUI_LyrUnlock();
    return ret;
}
/*
**********************************************************************************************************************
*                                               GUI_LyrAlphaOn
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
__s32 GUI_LyrWinAlphaOn(H_LYR h_lyr)
{
    __s32                   ret = EPDK_FAIL;
    __gui_lyrwin_t           *lyr_attr   = (__gui_lyrwin_t *)h_lyr;
    __u32 dispbuffer[3];
#if GUI_CHECK_ARG_EN > 0

    if (!lyr_attr)
    {
        __wrn("h_lyr empty!\n");
        return EPDK_FAIL;
    }

#endif
    GUI_LyrLock();

    if (lyr_attr->h_hwlyr)
    {
        /*modified by Derek,2010-12-9 18:50:10*/
        dispbuffer[0] = (unsigned long)(lyr_attr->h_hwlyr);
        dispbuffer[1] = 0;
        dispbuffer[2] = 0;
        //ret = eLIBs_fioctrl(lyr_attr->devfp, DISP_CMD_LAYER_ALPHA_ON, SEL_SCREEN, (void *)dispbuffer); //end modified.

        if (ret != EPDK_OK)
        {
            __wrn("layer set top failed!\n");
        }
        else
        {
            lyr_attr->lyrpara.alpha_en = EPDK_TRUE;
        }

#ifdef SIM_PC_WIN
        RefreshDeContent(1);
#endif
    }

    GUI_LyrUnlock();
    return ret;
}

/*
**********************************************************************************************************************
*                                               GUI_LyrAlphaOff
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
__s32 GUI_LyrWinAlphaOff(H_LYR h_lyr)
{
    __s32                   ret = EPDK_FAIL;
    __gui_lyrwin_t           *lyr_attr   = (__gui_lyrwin_t *)h_lyr;
    __u32 dispbuffer[3];
#if GUI_CHECK_ARG_EN > 0

    if (!lyr_attr)
    {
        __wrn("h_lyr empty!\n");
        return EPDK_FAIL;
    }

#endif
    GUI_LyrLock();

    if (lyr_attr->h_hwlyr)
    {
        /*modified by Derek,2010-12-9 18:53:58*/
        dispbuffer[0] = (unsigned long)(lyr_attr->h_hwlyr);
        dispbuffer[1] = 0;
        dispbuffer[2] = 0;
        //ret = eLIBs_fioctrl(lyr_attr->devfp, DISP_CMD_LAYER_ALPHA_OFF, SEL_SCREEN, (void *)dispbuffer); //end modified

        if (ret != EPDK_OK)
        {
            __wrn("layer set top failed!\n");
        }
        else
        {
            lyr_attr->lyrpara.alpha_en = EPDK_FALSE;
        }

#ifdef SIM_PC_WIN
        RefreshDeContent(1);
#endif
    }

    GUI_LyrUnlock();
    return ret;
}
