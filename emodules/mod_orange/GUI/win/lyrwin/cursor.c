/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __CURSOR_C__
#define __CURSOR_C__

#include "lyrwin_i.h"
#include <kconfig.h>
#ifdef CONFIG_SOC_SUN3IW1
#include <mod_display.h>
#endif

static __u8  cursor_status = GUI_CURSOR_NOINIT;

/*
**********************************************************************************************************************
*                                               GUI_CursorCreate
*
* author:
*
* date:             2010-2-22
*
* Description:      gui cursorcreate
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_CursorCreate(__gui_cursor_t *create_info)
{
#ifdef CONFIG_SOC_SUN3IW1
    __disp_hwc_pattern_t  hwc_pat;
    ES_FILE                *p_disp;
    __disp_pos_t               pos; //__pos_t modified by Derek,2010-12-11 10:46:54
    __u32 dispbuffer[3];
#if GUI_CHECK_ARG_EN > 0

    if (!create_info || (!create_info->palette) || (!create_info->pat_mem))
    {
        ORANGE_WRN("hwc para empty!\n");
        return ORANGE_FAIL;
    }

#endif
    /*open display driver handle*/
    p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

    if (!p_disp)
    {
        __err("open display device fail!\n");
    }

    hwc_pat.addr  = (__u32)create_info->pat_mem;
    hwc_pat.pat_mode = DISP_HWC_MOD_H32_V32_8BPP;
    //hwc_pat.x_offset = 0;
    //hwc_pat.y_offset = 0;
    pos.x            = 0;
    pos.y            = 0;
    /*request hardware layer; modified by Derek,2010-12-11 11:15:50*/
    dispbuffer[0] = (__u32)&hwc_pat;
    dispbuffer[1] = 0;
    dispbuffer[2] = 0;
    eLIBs_fioctrl(p_disp, DISP_CMD_HWC_SET_FB, SEL_SCREEN, (void *)dispbuffer); //DISP_CMD_HWC_SET_PATMEM
    dispbuffer[0] = (__u32)create_info->palette;
    dispbuffer[1] = 0;
    dispbuffer[2] = 1024;
    eLIBs_fioctrl(p_disp, DISP_CMD_HWC_SET_PALETTE_TABLE, SEL_SCREEN, (void *)dispbuffer);
    dispbuffer[0] = (__u32)&pos;
    dispbuffer[1] = 0;
    dispbuffer[2] = 0;
    eLIBs_fioctrl(p_disp, DISP_CMD_HWC_SET_POS, SEL_SCREEN, (void *)dispbuffer);
    eLIBs_fioctrl(p_disp, DISP_CMD_HWC_CLOSE, SEL_SCREEN, 0); //DISP_CMD_HWC_HIDE,modified by Derek,2010-12-11 10:33:54
    eLIBs_fclose(p_disp);
#endif
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_CursorDelete
*
* author:
*
* date:             2010-2-22
*
* Description:      gui cursordelete
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_CursorDelete(void)
{
#ifdef CONFIG_SOC_SUN3IW1
    ES_FILE                *p_disp;
    /*open display driver handle*/
    p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

    if (!p_disp)
    {
        __err("open display device fail!\n");
    }

    eLIBs_fioctrl(p_disp, DISP_CMD_HWC_CLOSE, SEL_SCREEN, 0); //DISP_CMD_HWC_HIDE,modified by Derek,2010-12-11 10:39:45
    eLIBs_fclose(p_disp);
#endif
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_CursorShow
*
* author:
*
* date:             2010-2-22
*
* Description:      gui cursorshow
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_CursorShow(void)
{
#ifdef CONFIG_SOC_SUN3IW1
    ES_FILE                *p_disp;
    /*open display driver handle*/
    p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

    if (!p_disp)
    {
        __err("open display device fail!\n");
    }

    eLIBs_fioctrl(p_disp, DISP_CMD_HWC_OPEN, SEL_SCREEN, 0); //DISP_CMD_HWC_SHOW,modified by Derek,2010-12-11 10:26:35
    eLIBs_fclose(p_disp);
#endif
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_CursorHide
*
* author:
*
* date:             2010-2-22
*
* Description:      gui cursorhide
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_CursorHide(void)
{
#ifdef CONFIG_SOC_SUN3IW1
    ES_FILE                *p_disp;
    /*open display driver handle*/
    p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

    if (!p_disp)
    {
        __err("open display device fail!\n");
    }

    eLIBs_fioctrl(p_disp, DISP_CMD_HWC_CLOSE, SEL_SCREEN, 0); //DISP_CMD_HWC_HIDE,modified by Derek,2010-12-11 10:40:58
    eLIBs_fclose(p_disp);
#endif
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_CursorSetPos
*
* author:
*
* date:             2010-2-22
*
* Description:      gui cursorsetpos
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_CursorSetPos(__pos_t *pos)
{
#ifdef CONFIG_SOC_SUN3IW1
    ES_FILE                *p_disp;
    __disp_pos_t           *pdisp_pos;
    __u32                 dispbuffer[3];
#if GUI_CHECK_ARG_EN > 0

    if (!pos)
    {
        ORANGE_WRN("hwc pos empty!\n");
        return ORANGE_FAIL;
    }

#endif
    /*open display driver handle*/
    p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

    if (!p_disp)
    {
        __err("open display device fail!\n");
    }

    /*modified by Derek,2010-12-11 10:56:25*/
    pdisp_pos = (__disp_pos_t *)pos;
    dispbuffer[0] = (__u32)pdisp_pos;
    dispbuffer[1] = 0;
    dispbuffer[2] = 0;
    eLIBs_fioctrl(p_disp, DISP_CMD_HWC_SET_POS, SEL_SCREEN, (void *)dispbuffer); //end modified
    eLIBs_fclose(p_disp);
#ifdef SIM_PC_WIN
    RefreshDeContent(2);
#endif
#endif
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_CursorGetPos
*
* author:
*
* date:             2010-2-22
*
* Description:      gui cursorgetpos
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_CursorGetPos(__pos_t *pos)
{
#ifdef CONFIG_SOC_SUN3IW1
    ES_FILE                *p_disp;
    __disp_pos_t           *pdisp_pos;
    __u32                 dispbuffer[3];
#if GUI_CHECK_ARG_EN > 0

    if (!pos)
    {
        ORANGE_WRN("hwc pos empty!\n");
        return ORANGE_FAIL;
    }

#endif
    /*open display driver handle*/
    p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

    if (!p_disp)
    {
        __err("open display device fail!\n");
    }

    /*modified by Derek,2010-12-11 11:05:03*/
    pdisp_pos = (__disp_pos_t *)pos;
    dispbuffer[0] = (__u32)pdisp_pos;
    dispbuffer[1] = 0;
    dispbuffer[2] = 0;
    eLIBs_fioctrl(p_disp, DISP_CMD_HWC_GET_POS, SEL_SCREEN, (void *)dispbuffer); //end modified
    eLIBs_fclose(p_disp);
#endif
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_CursorSetPara
*
* author:
*
* date:             2010-2-22
*
* Description:      gui cursorsetpara
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_CursorSetPara(__gui_cursor_t *para)
{
#ifdef CONFIG_SOC_SUN3IW1
    __disp_hwc_pattern_t  hwc_pat;
    ES_FILE                *p_disp;
    __u32 dispbuffer[3];
#if GUI_CHECK_ARG_EN > 0

    if (!para || (!para->palette) || (!para->pat_mem))
    {
        ORANGE_WRN("hwc para empty!\n");
        return ORANGE_FAIL;
    }

#endif
    /*open display driver handle*/
    p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

    if (!p_disp)
    {
        __err("open display device fail!\n");
    }

    hwc_pat.addr  = (__u32)para->pat_mem; //modified by Derek,2010-12-13 16:34:47
    hwc_pat.pat_mode = DISP_HWC_MOD_H32_V32_8BPP;
    //hwc_pat.x_offset = 0;
    //hwc_pat.y_offset = 0;
    /*request hardware layer; modified by Derek,2010-12-11 11:23:23*/
    dispbuffer[0] = (__u32)&hwc_pat;
    dispbuffer[1] = 0;
    dispbuffer[2] = 0;
    eLIBs_fioctrl(p_disp, DISP_CMD_HWC_SET_FB, SEL_SCREEN, (void *)dispbuffer); //DISP_CMD_HWC_SET_PATMEM
    dispbuffer[0] = (__u32)para->palette;
    dispbuffer[1] = 0;
    dispbuffer[2] = 1024;
    eLIBs_fioctrl(p_disp, DISP_CMD_HWC_SET_PALETTE_TABLE, SEL_SCREEN, (void *)dispbuffer); //end modified
    eLIBs_fclose(p_disp);
#endif
    return ORANGE_OK;
}
#endif
