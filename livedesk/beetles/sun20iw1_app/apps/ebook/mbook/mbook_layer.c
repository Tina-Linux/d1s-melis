/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              record app sample
*
*                                   (c) Copyright 2006-2009, China
*                                            All Rights Reserved
*
* File    : mbook_layer.c
* By      : lyn
* Version : V1.00
*********************************************************************************************************
*/
#include <log.h>
#include "mbook_private.h"


/*
**********************************************************************************************************************
*                                               MBOOK_Layer_CreatLayer
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
H_WIN   MBOOK_Layer_CreatLayer(__u32 w, __u32 h, __s32 pipe)
{
    H_WIN   hlyr;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},          /* fmt       */
    };
    __disp_layer_para_t para =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                   /* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        0xff,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t create_info =
    {
        "layer",
        NULL,
        GUI_LYRWIN_STA_SLEEP,
        0
    };
    fb.size.width  = w;
    fb.size.height = h;
    para.pipe           = pipe;
    para.scn_win.x      = 0 ;
    para.scn_win.y      = 0 ;
    para.scn_win.width  = w ;
    para.scn_win.height = h ;
    para.src_win.x      = 0;
    para.src_win.y      = 0;
    para.src_win.width  = w;
    para.src_win.height = h;
    para.fb = &fb;
    create_info.lyrpara = &para;
    hlyr = GUI_LyrWinCreate(&create_info);
    GUI_LyrWinSetTop(hlyr);
    return hlyr;
}

/*
**********************************************************************************************************************
*                                               MBOOK_Layer_DeleteLayer
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   MBOOK_Layer_DeleteLayer(H_WIN h_lyr)
{
    if (h_lyr == NULL)
    {
        __wrn("%s h_lyr == NULL\r\n", __FUNCTION__);
        return EPDK_FAIL;
    }

    GUI_LyrWinDelete(h_lyr);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               MBOOK_Layer_OpenShow
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   MBOOK_Layer_OpenShow(H_WIN h_lyr)
{
    if (h_lyr == NULL)
    {
        return EPDK_FAIL;
    }

    GUI_LyrWinSetSta(h_lyr, GUI_LYRWIN_STA_ON);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               MBOOK_Layer_CloseShow
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   MBOOK_Layer_CloseShow(H_WIN h_lyr)
{
    if (h_lyr == NULL)
    {
        return EPDK_FAIL;
    }

    GUI_LyrWinSetSta(h_lyr, GUI_LYRWIN_STA_SLEEP);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               MBOOK_Layer_IsOnShow
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   MBOOK_Layer_IsOnShow(H_WIN h_lyr)
{
    if (h_lyr == NULL)
    {
        return EPDK_FALSE;
    }

    if (GUI_LyrWinGetSta(h_lyr) == GUI_LYRWIN_STA_ON)
    {
        return EPDK_TRUE;
    }

    return EPDK_FALSE;
}

/*
**********************************************************************************************************************
*                                               MBOOK_Layer_GetSrcY
*
* Description: 移动图层的显示区域
*
* Arguments  : h_lyr: 操作的图层
*
*
* Returns    : 返回当前图层的起始区域。

* Notes      :
*
**********************************************************************************************************************
*/
__s32   MBOOK_Layer_GetSrcY(H_WIN h_lyr)
{
    RECT        srcrect;
    GUI_LyrWinGetSrcWindow(h_lyr, &srcrect);
    return srcrect.y;
}

/*
**********************************************************************************************************************
*                                               MBOOK_Layer_GetScnY
*
* Description: 移动图层的显示区域
*
* Arguments  : h_lyr: 操作的图层
*
*
* Returns    : 返回当前图层的起始区域。

* Notes      :
*
**********************************************************************************************************************
*/
__s32   MBOOK_Layer_GetScnY(H_WIN h_lyr)
{
    RECT        scnrect;
    GUI_LyrWinGetScnWindow(h_lyr, &scnrect);
    return scnrect.y;
}

/*
**********************************************************************************************************************
*                                               MBOOK_Layer_GetSrcH
*
* Description: 移动图层的显示区域
*
* Arguments  : h_lyr: 操作的图层
*
*
* Returns    : 返回当前图层的起始区域。

* Notes      :
*
**********************************************************************************************************************
*/
__s32   MBOOK_Layer_GetSrcH(H_WIN h_lyr)
{
    RECT        srcrect;
    GUI_LyrWinGetSrcWindow(h_lyr, &srcrect);
    return srcrect.height;
}

/*
**********************************************************************************************************************
*                                               MBOOK_Layer_MoveSrc
*
* Description: 移动图层的显示区域
*
* Arguments  : h_lyr: 操作的图层
*              step：要移动的步长
*
* Returns    : < 0：显示区域移到了屏幕的上方，> 0：显示区域移到了屏幕的下方，= 0：刚好覆盖屏幕。

* Notes      :
*
**********************************************************************************************************************
*/
void    MBOOK_Layer_MoveSrc(H_WIN h_lyr, __s32 step)
{
    RECT        srcrect;
    RECT        scnrect;
    GUI_LyrWinGetSrcWindow(h_lyr, &srcrect);
    GUI_LyrWinGetScnWindow(h_lyr, &scnrect);
    srcrect.y = srcrect.y + step;
    scnrect.height = scnrect.height - step;
    GUI_LyrWinSetSrcWindow(h_lyr, &srcrect);
    GUI_LyrWinSetScnWindow(h_lyr, &scnrect);
}

/*
**********************************************************************************************************************
*                                               MBOOK_Layer_SetScn
*
* Description: 移动图层的显示区域
*
* Arguments  : h_lyr: 操作的图层
*              step：要移动的步长
*
* Returns    : < 0：显示区域移到了屏幕的上方，> 0：显示区域移到了屏幕的下方，= 0：刚好覆盖屏幕。

* Notes      :
*
**********************************************************************************************************************
*/
void    MBOOK_Layer_SetScn(H_WIN h_lyr, __s32 y)
{
    RECT        srcrect;
    RECT        scnrect;
    GUI_LyrWinGetSrcWindow(h_lyr, &srcrect);
    GUI_LyrWinGetScnWindow(h_lyr, &scnrect);
    scnrect.y = srcrect.height - y;
    scnrect.height = y;
    GUI_LyrWinSetScnWindow(h_lyr, &scnrect);
}

/*
**********************************************************************************************************************
*                                               MBOOK_Layer_SetSrc
*
* Description: 移动图层的显示区域
*
* Arguments  : h_lyr: 操作的图层
*              flag：要设置的模式：0为屏幕原始区域，1为下一屏幕区域，-1为上一屏幕区域。
*
* Returns    : -1：显示区域移到了屏幕的上方，1：显示区域移到了屏幕的下方，0：刚好覆盖屏幕。

* Notes      :
*
**********************************************************************************************************************
*/
void    MBOOK_Layer_SetSrc(H_WIN h_lyr, __s32 y)
{
    RECT        srcrect;
    RECT        scnrect;
    GUI_LyrWinGetSrcWindow(h_lyr, &srcrect);
    GUI_LyrWinGetSrcWindow(h_lyr, &scnrect);
    srcrect.y = y;
    scnrect.y = 0;
    scnrect.height = scnrect.height - y;
    GUI_LyrWinSetSrcWindow(h_lyr, &srcrect);
    GUI_LyrWinSetScnWindow(h_lyr, &scnrect);
}

/*
**********************************************************************************************************************
*                                               MBOOK_Layer_SetSrc
*
* Description: 移动图层的显示区域
*
* Arguments  : h_lyr: 操作的图层
*              flag：要设置的模式：0为屏幕原始区域，1为下一屏幕区域，-1为上一屏幕区域。
*
* Returns    : -1：显示区域移到了屏幕的上方，1：显示区域移到了屏幕的下方，0：刚好覆盖屏幕。

* Notes      :
*
**********************************************************************************************************************
*/
void    MBOOK_Layer_ResetWin(H_WIN h_lyr)
{
    RECT        srcrect;
    GUI_LyrWinGetSrcWindow(h_lyr, &srcrect);
    srcrect.y = 0;
    srcrect.y = 0;
    GUI_LyrWinSetSrcWindow(h_lyr, &srcrect);
    GUI_LyrWinSetScnWindow(h_lyr, &srcrect);
}

/*
**********************************************************************************************************************
*                                               MBOOK_Layer_SetSrc
*
* Description: 移动图层的显示区域
*
* Arguments  : h_lyr: 操作的图层
*              flag：要设置的模式：0为屏幕原始区域，1为下一屏幕区域，-1为上一屏幕区域。
*
* Returns    : -1：显示区域移到了屏幕的上方，1：显示区域移到了屏幕的下方，0：刚好覆盖屏幕。

* Notes      :
*
**********************************************************************************************************************
*/
void    MBOOK_Layer_Rotate(H_WIN h_lyr, __u32 w, __u32 h, __u32 rotate)
{
    GUI_RECT rect;
    __log("Enter MBOOK_Layer_Rotate\r\n");
    return;
    GUI_LyrWinSel(h_lyr);

    if (rotate == 0)
    {
        GUI_SetLCDSize(w, h);
        GUI_SetLCDDirection(rotate);
        rect.x0 = 0;
        rect.y0 = 0;
        rect.x1 = w;
        rect.y1 = h;
        GUI_SetClipRect(&rect);
    }
    else
    {
        GUI_SetLCDSize(h, w);
        GUI_SetLCDDirection(rotate);
        rect.x0 = 0;
        rect.y0 = 0;
        rect.x1 = h;
        rect.y1 = w;
        GUI_SetClipRect(&rect);
    }
}



