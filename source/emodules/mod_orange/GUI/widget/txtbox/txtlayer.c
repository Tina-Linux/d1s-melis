/*
*******************************************************************************************************************
*                                                           txtbox  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：        txtlayer.c
*
*Author：           William Wang
*
*Version :          1.1.0
*
*Date:              2008-12-18
*
*Description :      txtbox layer function implement
*
*Others :           None at present.
*
* History :
*
* <Author>          <time>          <version>     <description>
*
* William Wang     2008-12-18         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __TXTLAYER_C__
#define __TXTLAYER_C__

#include "txtcfg.h"
#include "txtbox_i.h"
#include "txtlist.h"
#include "txtitem.h"
#include "txtlayer_process.h"
//------------------------------------------------------------------------------------------------------------
//创建TxtLayer
//------------------------------------------------------------------------------------------------------------

//==================================================================
//function name:    TxtLayer_GetBPP
//author:
//date:             2008-10-7
//description:      TxtLayer getbpp
//parameters:
//return:           if success return
//                  if fail return the number of fail
//modify history:
//==================================================================

__s32  TxtLayer_GetBPP(txtlayer_man_t *ptxtlayer)
{
    __u8  bpp;

    switch (ptxtlayer->txtbuffer->layer_format.fmt.rgb.pixelfmt)
    {
        case  PIXEL_MONO_1BPP:
            bpp = 1;
            break;

        case PIXEL_MONO_2BPP:
            bpp = 2;
            break;

        case PIXEL_MONO_4BPP:
            bpp = 4;
            break;

        case PIXEL_MONO_8BPP:
            bpp = 8;
            break;

        case PIXEL_COLOR_RGB655:
        case PIXEL_COLOR_RGB565:
        case PIXEL_COLOR_RGB556:
        case PIXEL_COLOR_ARGB1555:
        case PIXEL_COLOR_RGBA5551:
            bpp = 16;
            break;

        case PIXEL_COLOR_RGB0888:
            bpp = 32;
            break;

        case PIXEL_COLOR_ARGB8888:
            bpp = 32;
            break;

        default:
            bpp = 0;
            break;
    }

    return bpp;
}

__s32  TxtLayer_SwitchFB(txtlayer_man_t *txtlayer, __u32 x, __u32 y)
{
    __s32               ret;
    RECT                src_rect;

    if ((x < txtlayer->txtbuffer->rectsz.width) && (y < txtlayer->txtbuffer->rectsz.height))
    {
        src_rect.x = x;
        src_rect.y = y;
        txtlayer->scnrect.x = x;
        txtlayer->scnrect.y = y;
        src_rect.width = txtlayer->scnrect.width;
        src_rect.height = txtlayer->scnrect.height;
        /*switch source windows */
        ret = GUI_LyrWinSetSrcWindow(txtlayer->layer, &src_rect);

        if (ret != 0)
        {
            __wrn("layer get framebuffer failed layer handle = %d\n", txtlayer->layer);
            return __LINE__;
        }

        return OK;
    }
    else
    {
        return __LINE__;
    }
}

/*
**********************************************************************************************************************
*                                               TxtLayer_SwitchLyr
*
* author:
*
* date:             2009-2-22
*
* Description:      txtlayer switchlyr
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  TxtLayer_SwitchLyr(txtlayer_man_t *txtlayer, __u32 x, __u32 y)
{
    __s32               ret;
    RECT                src_rect;
    /*get listmenu layer sceen windows*/
    ret = GUI_LyrWinGetScnWindow(txtlayer->layer, &src_rect);

    if (ret != 0)
    {
        __wrn("layer set scn windows handle = %d\n", txtlayer->layer);
        return __LINE__;
    }

    /*set sceen y postition*/
    src_rect.y = y;
    ret = GUI_LyrWinSetScnWindow(txtlayer->layer, &src_rect);

    if (ret != 0)
    {
        __wrn("layer set scn windows handle = %d\n", txtlayer->layer);
        return __LINE__;
    }

    return OK;
}

//==================================================================
//function name:    TxtLayer_GetLayerInfo
//author:
//date:             2008-12-9
//description:      TxtLayer getlayerinfo
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32 TxtLayer_GetLayerInfo(txtlayer_man_t *ptxtlayer)
{
    __u32                       ret;
    FB                          tmpfb;
    FB                          foucs_FB;
    __layerwincreate_para_t     newlyr_info;
    /*get TxtLayer sceen size*/
    ret = GUI_LyrWinGetScnWindow(ptxtlayer->layer, &ptxtlayer->scnrect);

    if (ret != 0)
    {
        __wrn("layer set sceen window failed,layer handle = %d\n", ptxtlayer->layer);
        return __LINE__;
    }

    /*获取图层的framebuffer相关信息*/
    ret = GUI_LyrWinGetFB(ptxtlayer->layer, &tmpfb);

    if (ret != 0)
    {
        __wrn("layer get framebuffer failed layer handle = %d\n", ptxtlayer->layer);
        return __LINE__;
    }

    /*设置图层的framebuffer信息至txtbuffer结构*/
    ptxtlayer->txtbuffer->buffer                = tmpfb.addr[0];
    ptxtlayer->txtbuffer->layer_format.type     = FB_TYPE_RGB;
    ptxtlayer->txtbuffer->layer_format          = tmpfb.fmt;
    ptxtlayer->txtbuffer->rectsz.width          = tmpfb.size.width;
    ptxtlayer->txtbuffer->rectsz.height         = tmpfb.size.height;
    ptxtlayer->txtbuffer->vrect.x               = 0;
    ptxtlayer->txtbuffer->vrect.y               = 0;
    foucs_FB = tmpfb;
    foucs_FB.size.height                        = ptxtlayer->ptxtbox->item_w * 2;
    foucs_FB.addr[0] = 0;
    ptxtlayer->sec_layer.alpha_en               = 0;
    ptxtlayer->sec_layer.alpha_val              = 0;
    ptxtlayer->sec_layer.channel                = DISP_LAYER_OUTPUT_CHN_DE_CH1;
    ptxtlayer->sec_layer.ck_mode                = 0;
    ptxtlayer->sec_layer.fb                     = &foucs_FB;
    ptxtlayer->sec_layer.mode                   = MOD_DISP_LAYER_WORK_MODE_NORMAL;
    ptxtlayer->sec_layer.pipe                   = 0;
    ptxtlayer->sec_layer.prio                   = 0;
    ptxtlayer->sec_layer.scn_win.x              = 0;
    ptxtlayer->sec_layer.scn_win.y              = 0;
    ptxtlayer->sec_layer.scn_win.width          = 0;
    ptxtlayer->sec_layer.scn_win.height         = 0;
    ptxtlayer->sec_layer.scn_win.x              = 0;
    ptxtlayer->sec_layer.scn_win.y              = 0;
    ptxtlayer->sec_layer.scn_win.width          = 0;
    ptxtlayer->sec_layer.scn_win.height         = 0;
    newlyr_info.initsta                         = GUI_LYRWIN_STA_SLEEP;
    newlyr_info.lyrpara                         = &ptxtlayer->sec_layer;
    newlyr_info.name                            = "txtbox sec lyr";
    ptxtlayer->focus_lyr                        = GUI_LyrWinCreate(&newlyr_info);
    /*获取图层的framebuffer相关信息*/
    ret = GUI_LyrWinGetFB(ptxtlayer->focus_lyr, &foucs_FB);

    if (ret != 0)
    {
        __wrn("layer get framebuffer failed layer handle = %d\n", ptxtlayer->layer);
        return __LINE__;
    }

    __HERE__
    ptxtlayer->foucs_FB = foucs_FB.addr[0];
    return OK;
}

__u32  TxtLayer_ClearTxtItem(txtlayer_man_t *ptxtlayer, __u32 x, __u32 y)
{
#if 0
    void                *base;
    __u32                bpp;
    __u32                size;
    bpp  = TxtLayer_GetBPP(ptxtlayer);
    size = (ptxtlayer->txtlist->head->rect.width *
            ptxtlayer->txtlist->head->rect.height * ptxtlayer->txtlist->bpp + 7) / 8;
    base = (void *)((__u32)ptxtlayer->txtbuffer->buffer +
                    (ptxtlayer->txtbuffer->rectsz.width * y * bpp + 7) / 8 +
                    (x * bpp + 7) / 8);
    eLIBs_memset(base, 0x00, size);
#else
    GUI_LyrWinSel(ptxtlayer->layer);
    GUI_SetBkColor(ptxtlayer->bk_color);
    GUI_ClearRect(x, y, x + ptxtlayer->txtbuffer->rectsz.width - 1, y + ptxtlayer->txtlist->head->rect.height - 1);
#endif
    return 0;
}
//------------------------------------------------------------------------------------------------------------
//创建TxtLayer

//==================================================================
//function name:    TxtLayer_Create
//author:
//date:             2008-10-7
//description:      TxtLayer create
//parameters:
//return:           if success return
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
txtlayer_man_t *TxtLayer_Create(H_LYR TxtLayer, __hdle txtbox, __u32 item_w)
{
    //   __u8                        err;
    __u32                       bpp;
    __u32                       ret;
    //    __u32                       size;
    txtlayer_man_t             *ptxtlayer = (txtlayer_man_t *)NULL;
    ptxtlayer                           = (txtlayer_man_t *)TXTBOX_MALLOC(sizeof(txtlayer_man_t));

    if (!ptxtlayer)
    {
        goto error;
    }

    /*create txtbuffer of this TxtLayer object*/
    ptxtlayer->txtbuffer                = (txtbuffer_man_t *)TxtBuffer_Create();
    ptxtlayer->ptxtbox                  = (struct tag_TXTBOX_DATA *)txtbox;
    ptxtlayer->layer                    =  TxtLayer;
    ptxtlayer->focus_lyr                = 0;
    ptxtlayer->chg_fb                   = NULL;
    ptxtlayer->foucs_FB                 = NULL;
    ret                                 = TxtLayer_GetLayerInfo(ptxtlayer);

    if (ret != OK)
    {
        __msg("get layer information failed!\n");
        goto error;
    }

    bpp                                 = TxtLayer_GetBPP(ptxtlayer);
    ptxtlayer->txtlist                  = NULL;
    ptxtlayer->mode                     = 0;
    ptxtlayer->status                   = 0;
    ptxtlayer->status                   |= TXTBOX_INIT;
    ptxtlayer->draw_font                = NULL;
    ptxtlayer->backup                   = EPDK_FALSE;
    ptxtlayer->backalpha                = EPDK_FALSE;

    if (item_w != 0)
    {
        if (ptxtlayer->txtbuffer->rectsz.height < item_w)
        {
            __inf("no enough page!\n");
            goto error;
        }
    }

    /*create txtlist of this TxtLayer*/
    ptxtlayer->txtlist = TxtList_Create();
    ptxtlayer->txtlist->bpp = bpp;
    ptxtlayer->layer   = TxtLayer;
    ptxtlayer->head_fb = NULL;
    ptxtlayer->tail_fb = NULL;
    return ptxtlayer;
error:

    if (ptxtlayer->txtlist)
    {
        TxtList_Destroy(ptxtlayer->txtlist);
    }

    if (ptxtlayer->focus_lyr)
    {
        GUI_LyrWinDelete(ptxtlayer->focus_lyr);
        ptxtlayer->foucs_FB = NULL;
        ptxtlayer->focus_lyr = 0;
    }

    if (ptxtlayer)
    {
        __inf("delete ptxtlayer\n");
        TXTBOX_MEMFREE(ptxtlayer);
    }

    return NULL;
}
//------------------------------------------------------------------------------------------------------------
//销毁TxtLayer

//==================================================================
//function name:    TxtLayer_Destroy
//author:
//date:             2008-10-7
//description:      TxtLayer destroy
//parameters:
//return:           if success return
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32 TxtLayer_Destroy(txtlayer_man_t *ptxtlayer)
{
    //  __u8   err;
    __u32  ret;
    ptxtlayer->status |= TXTBOX_REQDEL;

    while (ptxtlayer->status & TXTBOX_ONDRAW)
    {
        esKRNL_TimeDly(3);
    }

    ret = TxtBuffer_Destroy(ptxtlayer->txtbuffer);

    if (ret != OK)
    {
        return __LINE__;
    }

    ret = TxtList_Destroy(ptxtlayer->txtlist);

    if (ret != OK)
    {
        return __LINE__;
    }

    if (ptxtlayer->focus_lyr)
    {
        GUI_LyrWinDelete(ptxtlayer->focus_lyr);
        ptxtlayer->foucs_FB = NULL;
        ptxtlayer->focus_lyr = 0;
    }

    ptxtlayer->status &= (~TXTBOX_REQDEL);
    TXTBOX_MEMFREE(ptxtlayer);
    return OK;
}

//==================================================================
//function name:    TxtLayer_LinearOnDraw
//author:
//date:             2008-10-20
//description:      TxtLayer linearondraw
//parameters:
//return:           if success return
//                  if fail return the number of fail
//modify history:
//==================================================================
__u32 TxtLayer_FixedOnDraw(txtlayer_man_t *TxtLayer)
{
    __s32                ret;
    __u32                i;
    __u32                num;
    __u32                mid_num = 0;
    __u32                count;
    __pos_t              cur_pos;
    txtitem_t           *tmpItem;

    if (TxtLayer->txtlist)
    {
        if (TxtLayer->txtlist->ItemCount != 0)
        {
            __inf("ItemCount = %d\n", TxtLayer->txtlist->ItemCount);
            cur_pos.x = 0;
            cur_pos.y = 0;
            num = TxtLayer->scnnum;

            if (num % 2 == 0)
            {
                mid_num = (num >> 1) - 1;    /*Item number is even*/
            }
            else
            {
                mid_num = (num >> 1);       /*Item number is odd*/
            }

            count = mid_num;
            TxtLayer->start_pos.x = 0;
            TxtLayer->start_pos.y = mid_num * TxtLayer->txtlist->head->rect.height;
            num = 0;
            tmpItem = TxtLayer->txtlist->head;

            /*从头开始绘制*/
            while (tmpItem != TxtLayer->txtlist->tail)
            {
                if (TxtLayer->status & TXTBOX_REQDEL)
                {
                    return __LINE__;
                }

                if (count > 0)
                {
                    TxtLayer_ClearTxtItem(TxtLayer, cur_pos.x, cur_pos.y);
                    count--;
                    cur_pos.y = TxtLayer->txtlist->head->rect.height +
                                cur_pos.y;
                }
                else
                {
                    tmpItem->rect.x = cur_pos.x;
                    tmpItem->rect.y = cur_pos.y;
                    //LISTMENU_PRINTF("TxtLayer linear on draw2!\n");
                    ret = TxtLayer_DrawItem(TxtLayer, tmpItem);

                    if (ret != OK)
                    {
                        return __LINE__;
                    }

                    tmpItem = tmpItem->Next;
                    cur_pos.y = TxtLayer->txtlist->head->rect.height +
                                cur_pos.y;
                }
            }

            tmpItem->rect.x = cur_pos.x;
            tmpItem->rect.y = cur_pos.y;

            if (TxtLayer->status & TXTBOX_REQDEL)
            {
                return __LINE__;
            }

            //LISTMENU_PRINTF("TxtLayer linear on draw3!\n");
            ret = TxtLayer_DrawItem(TxtLayer, TxtLayer->txtlist->tail);

            if (ret != OK)
            {
                return __LINE__;
            }

            if (TxtLayer->status & TXTBOX_REQDEL)
            {
                return __LINE__;
            }

            cur_pos.y = TxtLayer->txtlist->head->rect.height +
                        cur_pos.y;

            for (i = 0; i <= mid_num; i++)
            {
                if (TxtLayer->status & TXTBOX_REQDEL)
                {
                    return __LINE__;
                }

                ret = TxtLayer_ClearTxtItem(TxtLayer, cur_pos.x, cur_pos.y);

                if (ret != OK)
                {
                    return __LINE__;
                }

                cur_pos.y = TxtLayer->txtlist->head->rect.height +
                            cur_pos.y;
            }
        }
    }

    return OK;
}

//------------------------------------------------------------------------------------------------------------
//显示

//==================================================================
//function name:    TxtLayer_OnDraw
//author:
//date:             2008-10-7
//description:      TxtLayer ondraw
//parameters:
//return:           if success return
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32 TxtLayer_OnDraw(txtlayer_man_t    *TxtLayer)
{
    // __s32                ret;
    if (TxtLayer->txtlist)
    {
        if (TxtLayer->txtlist->ItemCount != 0)
        {
            TxtLayer->status |= TXTBOX_ONDRAW;
            /*item数量固定*/
            TxtLayer_FixedOnDraw(TxtLayer);
            TxtLayer->status &= (~TXTBOX_ONDRAW);
            TxtLayer->status |= TXTBOX_DRAWFINISH;
        }
    }

    return OK;
}

#endif
