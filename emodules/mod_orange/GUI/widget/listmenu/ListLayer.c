/*
*******************************************************************************************************************
*                                                           display driver
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：       ListLayer.c
*
*Author：          William Wang
*
*Version :         1.1.0
*
*Date:             2008-8-25
*
*Description :     listlayer interface implement function
*
*Others :          None at present.
*
* History :
*
* <Author>          <time>      <version>     <description>
*
* William Wang     2008-8-25      1.1.0        Create File
*
*******************************************************************************************************************
*/
#ifndef __LIST_LAYER__C____
#define __LIST_LAYER__C____


#include "config.h"
#include "ListMenu_i.h"
#include "ListLayer.h"
#include "ListBox.h"
#include "MenuList.h"
#include "MenuItem.h"
#include "sysmemory.h"
#include "ListLayer_Process.h"
#include <mod_display.h>
/*
**********************************************************************************************************************
*                                               ListLayer_GetBPP
*
* author:
*
* date:             2008-10-9
*
* Description:      listlayer getbpp
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  ListLayer_GetBPP(LIST_LAYER_MAN_t *plist_layer)
{
    __u8  bpp;

    switch (plist_layer->listbuffer->layer_format.fmt.rgb.pixelfmt)
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

/*
**********************************************************************************************************************
*                                               ListLayer_SwitchFB
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer switchfb
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  ListLayer_SwitchFB(LIST_LAYER_MAN_t *listlayer, __bool fbno, __u32 x, __u32 y)
{
    __s32               ret;
    RECT                src_rect;

    if ((x < listlayer->listbuffer->rectsz.width) && (y < listlayer->listbuffer->rectsz.height))
    {
        src_rect.x = x;
        src_rect.y = y;
        src_rect.width = listlayer->scnrect.width;
        src_rect.height = listlayer->scnrect.height;
        /*switch source windows */
        ret = GUI_LyrWinSetSrcWindow(listlayer->layer, &src_rect);

        if (ret != 0)
        {
            __wrn("layer get framebuffer failed layer handle = %d\n", listlayer->layer);
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
*                                               ListLayer_SwitchLyr
*
* author:
*
* date:             2009-1-17
*
* Description:      listlayer switchlyr
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  ListLayer_SwitchLyr(LIST_LAYER_MAN_t *listlayer, __u32 height, __u32 y)
{
    __s32               ret;
    RECT                src_rect;
    /*get listmenu layer sceen windows*/
    ret = GUI_LyrWinGetScnWindow(listlayer->layer, &src_rect);

    if (ret != 0)
    {
        __wrn("layer set scn windows handle = %d\n", listlayer->layer);
        return __LINE__;
    }

    /*set sceen y postition*/
    src_rect.y = y;

    if (height != 0)
    {
        src_rect.height = height;
    }

    ret = GUI_LyrWinSetScnWindow(listlayer->layer, &src_rect);

    if (ret != 0)
    {
        __wrn("layer set scn windows handle = %d\n", listlayer->layer);
        return __LINE__;
    }

    return OK;
}


/*
**********************************************************************************************************************
*                                               ListLayer_GetLayerInfo
*
* author:
*
* date:             2008-12-09
*
* Description:      listlayer getlayerinfo
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_GetLayerInfo(LIST_LAYER_MAN_t *plist_layer)
{
    __u32                    ret;
    FB                       tmpfb;
    /*get listlayer sceen size*/
    ret = GUI_LyrWinGetScnWindow(plist_layer->layer, &plist_layer->scnrect);

    if (ret != 0)
    {
        __wrn("layer set sceen window failed,layer handle = %d\n", plist_layer->layer);
        return __LINE__;
    }

    plist_layer->userscnrect = plist_layer->scnrect;
    /*获取图层的framebuffer相关信息*/
    ret = GUI_LyrWinGetFB(plist_layer->layer, &tmpfb);

    if (ret != 0)
    {
        __wrn("layer get framebuffer failed layer handle = %d\n", plist_layer->layer);
        return __LINE__;
    }

    /*设置图层的framebuffer信息至listbuffer结构*/
    plist_layer->listbuffer->buffer                = tmpfb.addr[0];
    plist_layer->listbuffer->layer_format.type     = FB_TYPE_RGB;
    plist_layer->listbuffer->layer_format          = tmpfb.fmt;
    plist_layer->listbuffer->rectsz.width          = tmpfb.size.width;
    plist_layer->listbuffer->rectsz.height         = tmpfb.size.height;
    plist_layer->listbuffer->vrect.x               = 0;
    plist_layer->listbuffer->vrect.y               = 0;
    plist_layer->dir                               = GUI_GetScnDir();
    plist_layer->lyr_size.width = tmpfb.size.width;
    plist_layer->lyr_size.height = tmpfb.size.height;
    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_AdjustFb
*
* author:
*
* date:             2009-1-17
*
* Description:      listlayer adjustfb
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_AdjustFb(LIST_LAYER_MAN_t *plist_layer, __u32 item_w, __u32 foucs_w)
{
    __u32                    bpp;
    __u32                    valid_w;
    __u32                    extend_w;

    if (item_w != 0)
    {
        if (plist_layer->viewrect.height %  item_w)
        {
            /*重新设置可视窗口宽度*/
            plist_layer->viewrect.height = (plist_layer->viewrect.height -
                                            plist_layer->viewrect.height %  item_w) + item_w;

            /*获取扩展宽度*/
            if ((foucs_w - item_w) > plist_layer->viewrect.height %  item_w)
            {
                extend_w = foucs_w - item_w;
            }
            else
            {
                extend_w = 0;
            }

            /*获取有效宽度*/
            //valid_w = plist_layer->viewrect.height - plist_layer->viewrect.height %  item_w;
            valid_w = plist_layer->viewrect.height;
            /*获取framebuffer深度*/
            plist_layer->fb_depth = (plist_layer->listbuffer->rectsz.height - extend_w) / valid_w;
            plist_layer->scnnum = (plist_layer->viewrect.height) / item_w;
        }
        else
        {
            extend_w = foucs_w - item_w;
            valid_w = plist_layer->viewrect.height;
            /*获取framebuffer深度*/
            plist_layer->fb_depth = (plist_layer->listbuffer->rectsz.height - extend_w) / valid_w;
            plist_layer->scnnum = (plist_layer->viewrect.height) / item_w;
        }
    }
    else
    {
        extend_w = 0;
        valid_w = plist_layer->viewrect.height;
    }

    bpp = ListLayer_GetBPP(plist_layer);
    /*if(plist_layer->listbuffer->rectsz.height%plist_layer->viewrect.height)
    {
        plist_layer->listbuffer->buffer = (void *)((__u32)plist_layer->listbuffer->buffer+ ((((plist_layer->listbuffer->rectsz.height%plist_layer->viewrect.height)/2)*(plist_layer->listbuffer->rectsz.width*bpp) + 7)>>3));
    }*/
    __inf("plist_layer->fb_depth = %d,plist_layer->listbuffer->rectsz.height = %d,plist_layer->viewrect.height = %d\n",
          plist_layer->fb_depth, plist_layer->listbuffer->rectsz.height, plist_layer->viewrect.height);
#if 0
    __inf("buffer = %x,ebuffer = %x,esize = %d,vsize = %d,vheight = %d,size = %d,height = %d\n",
          plist_layer->listbuffer->buffer, plist_layer->listbuffer->ebuffer, plist_layer->listbuffer->esize,
          plist_layer->listbuffer->vsize, plist_layer->listbuffer->vrect.height, plist_layer->listbuffer->rectsz.height);
    __inf("extend_w = %x,valid_w = %x,bpp = %d\n",
          extend_w, valid_w, bpp);
#endif

    if (plist_layer->fb_depth >= 4)
    {
        /*可变数量的条目处理*/
        plist_layer->mode = VAR_MODE;
        plist_layer->listbuffer->rectsz.height   = valid_w * plist_layer->fb_depth + extend_w;
        plist_layer->listbuffer->size            = (plist_layer->listbuffer->rectsz.height *
                plist_layer->listbuffer->rectsz.width *
                bpp + 7) >> 3;
        plist_layer->listbuffer->vrect.height    = plist_layer->listbuffer->rectsz.height - valid_w - extend_w;
        plist_layer->listbuffer->esize           = ((valid_w + extend_w) * plist_layer->listbuffer->rectsz.width * bpp + 7) >> 3;
        plist_layer->listbuffer->vsize           = (valid_w * (plist_layer->fb_depth - 1) *
                plist_layer->listbuffer->rectsz.width * bpp + 7) >> 3;
        plist_layer->listbuffer->ebuffer         = (void *)((unsigned long)plist_layer->listbuffer->buffer + plist_layer->listbuffer->vsize);
    }
    else
    {
        if (plist_layer->listbuffer->rectsz.height - 2 * LISTMENU_FIXEDEXT_WIDTH
            >= plist_layer->scnrect.height)
        {
            //ES_FILE *disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
            __mp* disp = esKSRV_Get_Display_Hld();
            plist_layer->scn_width = esMODS_MIoctrl(disp, DISP_GET_SCN_WIDTH, SEL_SCREEN, 0); //modified by Derek,2010.12.07.15:05
            plist_layer->scn_height = esMODS_MIoctrl(disp, DISP_GET_SCN_HEIGHT, SEL_SCREEN, 0); //modified by Derek,2010.12.07.15:05
            //eLIBs_fclose(disp);
            disp = NULL;
            /*固定数量的条目处理*/
            plist_layer->mode = FIXED_MODE;
            plist_layer->listbuffer->esize           = (LISTMENU_FIXEDEXT_WIDTH * plist_layer->listbuffer->rectsz.width * bpp + 7) >> 3;
            plist_layer->listbuffer->buffer          = (void *)((unsigned long)plist_layer->listbuffer->buffer + plist_layer->listbuffer->esize);
            __inf("esize = %d,buffer = %x,plist_layer->scnrect.height = %d\n",
                  plist_layer->listbuffer->esize, plist_layer->listbuffer->buffer, plist_layer->scnrect.height);
        }
        else
        {
            __wrn("no enough free page used!\n");
            return __LINE__;
        }
    }

    return OK;
}


/*
**********************************************************************************************************************
*                                               ListLayer_Create
*
* author:
*
* date:             2009-10-7
*
* Description:      listlayer create
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__hdle ListLayer_Create(H_LYR listlayer, __hdle listmenu, __u32 item_w, __u32 foucs_w)
{
    __u8                     err;
    __hdle                   hlistbuffer;
    __u32                    bpp;
    __u32                    valid_w;
    __u32                    extend_w;
    __u32                    ret;
    LIST_LAYER_MAN_t        *plist_layer = (LIST_LAYER_MAN_t *)NULL;
    plist_layer                         = (LIST_LAYER_MAN_t *)MEM_MALLOC(sizeof(LIST_LAYER_MAN_t));

    if (!plist_layer)
    {
        goto error;
    }

    /*create listbuffer of this listlayer object*/
    hlistbuffer                         = ListBuffer_Create();
    plist_layer->listbuffer             = (LIST_BUFFER_MAN_t *)hlistbuffer;
    plist_layer->plistmenu              = (struct tag_LISTMENU_DATA *)listmenu;
    plist_layer->layer                  =  listlayer;
    ret                                 = ListLayer_GetLayerInfo(plist_layer);

    if (ret != OK)
    {
        __msg("get layer information failed!\n");
        goto error;
    }

    bpp                                 = ListLayer_GetBPP(plist_layer);
    plist_layer->menulist               = NULL;
    plist_layer->load_flags             = (__krnl_flag_grp_t *)NULL;
    plist_layer->wait_flags             = (__krnl_flag_grp_t *)NULL;
    plist_layer->foucs_flags            = (__krnl_flag_grp_t *)NULL;
    plist_layer->pos_sem                = (__krnl_event_t *)NULL;
    plist_layer->load_sem               = (__krnl_event_t *) NULL;
    plist_layer->head_pos.x             = 0;
    plist_layer->head_pos.y             = 0;
    plist_layer->tail_pos.x             = 0;
    plist_layer->tail_pos.y             = 0;
    plist_layer->foucs_pos.x            = 0;
    plist_layer->foucs_pos.y            = 0;
    plist_layer->unfoucs_pos.x          = 0;
    plist_layer->unfoucs_pos.y          = 0;
    plist_layer->viewrect.x             = 0;
    plist_layer->viewrect.y             = 0;
    plist_layer->viewrect.width         = plist_layer->scnrect.width;
    plist_layer->viewrect.height        = plist_layer->scnrect.height;
    plist_layer->msgcount               = 0;
    plist_layer->msgflush               = 0;
    plist_layer->LoadPrio               = 0;
    plist_layer->setfoucs               = 0;
    plist_layer->fillup                 = 0;
    plist_layer->filldown               = 0;
    plist_layer->adjust_wm              = FALSE;
    plist_layer->mode                   = 0;
    plist_layer->status                 = 0;
    plist_layer->msgdowncount           = 0;
    plist_layer->msgupcount             = 0;

    if (item_w != 0)
    {
        if (plist_layer->viewrect.height %  item_w)
        {
            /*重新设置可视窗口宽度*/
            plist_layer->viewrect.height = (plist_layer->viewrect.height -
                                            plist_layer->viewrect.height %  item_w) + item_w;

            /*获取扩展宽度*/
            if ((foucs_w - item_w) > plist_layer->viewrect.height %  item_w)
            {
                extend_w = foucs_w - item_w;
            }
            else
            {
                extend_w = 0;
            }

            /*获取有效宽度*/
            //valid_w = plist_layer->viewrect.height - plist_layer->viewrect.height %  item_w;
            valid_w = plist_layer->viewrect.height;
            /*获取framebuffer深度*/
            plist_layer->fb_depth = (plist_layer->listbuffer->rectsz.height - extend_w) / valid_w;
            plist_layer->scnnum = (plist_layer->viewrect.height) / item_w;
        }
        else
        {
            extend_w = foucs_w - item_w;
            valid_w = plist_layer->viewrect.height;
            /*获取framebuffer深度*/
            plist_layer->fb_depth = (plist_layer->listbuffer->rectsz.height - extend_w) / valid_w;
            plist_layer->scnnum = (plist_layer->viewrect.height) / item_w;
        }
    }
    else
    {
        extend_w = 0;
        valid_w = plist_layer->viewrect.height;
    }

    /*if(plist_layer->listbuffer->rectsz.height%plist_layer->viewrect.height)
    {
        plist_layer->listbuffer->buffer = (void *)((__u32)plist_layer->listbuffer->buffer+ ((((plist_layer->listbuffer->rectsz.height%plist_layer->viewrect.height)/2)*(plist_layer->listbuffer->rectsz.width*bpp) + 7)>>3));
    }*/
    __inf("plist_layer->fb_depth = %d,plist_layer->listbuffer->rectsz.height = %d,plist_layer->viewrect.height = %d\n",
          plist_layer->fb_depth, plist_layer->listbuffer->rectsz.height, plist_layer->viewrect.height);

    if (plist_layer->fb_depth >= 4)
    {
        /*可变数量的条目处理*/
        plist_layer->mode = VAR_MODE;
        plist_layer->listbuffer->rectsz.height   = valid_w * plist_layer->fb_depth + extend_w;
        plist_layer->listbuffer->size            = (plist_layer->listbuffer->rectsz.height *
                plist_layer->listbuffer->rectsz.width *
                bpp + 7) >> 3;
        plist_layer->listbuffer->vrect.height    = plist_layer->listbuffer->rectsz.height - valid_w - extend_w;
        plist_layer->listbuffer->esize           = ((valid_w + extend_w) * plist_layer->listbuffer->rectsz.width * bpp + 7) >> 3;
        plist_layer->listbuffer->vsize           = (valid_w * (plist_layer->fb_depth - 1) *
                plist_layer->listbuffer->rectsz.width * bpp + 7) >> 3;
        plist_layer->listbuffer->ebuffer         = (void *)((unsigned  long)plist_layer->listbuffer->buffer + plist_layer->listbuffer->vsize);
    }
    else
    {
        if (plist_layer->listbuffer->rectsz.height - 2 * LISTMENU_FIXEDEXT_WIDTH
            >= plist_layer->scnrect.height)
        {
            //ES_FILE *disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
            __mp* disp  = esKSRV_Get_Display_Hld();
            plist_layer->scn_width = esMODS_MIoctrl(disp, DISP_GET_SCN_WIDTH, SEL_SCREEN, 0); //modified by Derek,2010.12.07.15:05
            plist_layer->scn_height = esMODS_MIoctrl(disp, DISP_GET_SCN_HEIGHT, SEL_SCREEN, 0); //modified by Derek,2010.12.07.15:05
            //eLIBs_fclose(disp);
            disp = NULL;
            /*固定数量的条目处理*/
            plist_layer->mode = FIXED_MODE;
            plist_layer->listbuffer->esize           = (LISTMENU_FIXEDEXT_WIDTH * plist_layer->listbuffer->rectsz.width * bpp + 7) >> 3;
            plist_layer->listbuffer->buffer          = (void *)((unsigned long)plist_layer->listbuffer->buffer + plist_layer->listbuffer->esize);
        }
        else
        {
            __wrn("no enough free page used!\n");
            goto error;
        }
    }

    /*create menulist of this listlayer*/
    plist_layer->menulist = MenuList_Create();
    plist_layer->menulist->bpp = bpp;
    plist_layer->load_flags    = esKRNL_FlagCreate(0x00, &err);

    if (err != OS_NO_ERR)
    {
        __inf("create load flags group! fail\n");
        goto error;
    }

    plist_layer->wait_flags = esKRNL_FlagCreate(0x00, &err);

    if (err != OS_NO_ERR)
    {
        __inf("create wait flags group! fail\n");
        goto error;
    }

    plist_layer->foucs_flags = esKRNL_FlagCreate(0x00, &err);

    if (err != OS_NO_ERR)
    {
        __inf("create foucs flags group! fail\n");
        goto error;
    }

    plist_layer->load_sem = esKRNL_SemCreate(1);

    if (plist_layer->load_sem == NULL)
    {
        __inf("create load sem! fail\n");
        goto error;
    }

    plist_layer->pos_sem = esKRNL_SemCreate(1);

    if (plist_layer->pos_sem == NULL)
    {
        __inf("create pos sem! fail\n");
        goto error;
    }

    plist_layer->layer = listlayer;
    return (__hdle)plist_layer;
error:

    //__err("layer request faile,disp = %d,rid = %d\n",gdispfile,handle);
    if (plist_layer->load_flags)
    {
        __inf("delete load flags group! fail\n");
        esKRNL_FlagDel(plist_layer->load_flags, OS_DEL_ALWAYS, &err);
    }

    if (plist_layer->wait_flags)
    {
        __inf("delete wait flags group! fail\n");
        esKRNL_FlagDel(plist_layer->wait_flags, OS_DEL_ALWAYS, &err);
    }

    if (plist_layer->foucs_flags)
    {
        __inf("delete foucs flags group! fail\n");
        esKRNL_FlagDel(plist_layer->foucs_flags, OS_DEL_ALWAYS, &err);
    }

    if (plist_layer->load_sem)
    {
        esKRNL_SemDel(plist_layer->load_sem, OS_DEL_ALWAYS, &err);
    }

    if (plist_layer->pos_sem)
    {
        esKRNL_SemDel(plist_layer->pos_sem, OS_DEL_ALWAYS, &err);
    }

    if (plist_layer->menulist)
    {
        MenuList_Destroy(plist_layer->menulist);
    }

    if (plist_layer->listbuffer)
    {
        ListBuffer_Destroy((__hdle)plist_layer->listbuffer);
    }

    if (plist_layer)
    {
        MEM_FREE(plist_layer);
    }

    return NULL;
}

/*
**********************************************************************************************************************
*                                               ListLayer_Destroy
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer destroy
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_Destroy(LIST_LAYER_MAN_t *plist_layer)
{
    __u8   err;
    __u32  ret;
    __krnl_flags_t              flags;

    if (plist_layer->mode == VAR_MODE)
    {
        if (plist_layer->LoadTID)
        {
            while (1)
            {
                /*delete listmenu receive thread*/
                if (esKRNL_TDelReq(plist_layer->LoadTID) == OS_TASK_NOT_EXIST)
                {
                    break;
                }

                flags = esKRNL_FlagQuery(plist_layer->load_flags, &err);

                if (!(flags & LISTLAYER_LOAD_FLAG))
                {
                    __inf("post to load!\n");
                    esKRNL_FlagPost(plist_layer->load_flags, LISTLAYER_LOAD_FLAG,
                                    OS_FLAG_SET, &err);
                }

                esKRNL_TimeDly(3);
            }
        }
    }

    if (plist_layer->FoucsTID)
    {
        while (1)
        {
            if (esKRNL_TDelReq(plist_layer->FoucsTID) == OS_TASK_NOT_EXIST)
            {
                break;
            }

            flags = esKRNL_FlagQuery(plist_layer->foucs_flags, &err);

            if (!(flags & LISTLAYER_FOUCS_FLAG))
            {
                __inf("post to foucs!\n");
                esKRNL_FlagPost(plist_layer->foucs_flags, LISTLAYER_FOUCS_FLAG,
                                OS_FLAG_SET, &err); /*start foucs Item thread to foucs item*/
            }

            esKRNL_TimeDly(3);
        }
    }

    ret = ListBuffer_Destroy((__hdle)plist_layer->listbuffer);

    if (ret != OK)
    {
        __wrn("listbuffer destory failed!\n");
        return __LINE__;
    }

    ret = MenuList_Destroy(plist_layer->menulist);

    if (ret != OK)
    {
        __wrn("menulist destory failed!\n");
        return __LINE__;
    }

    if (plist_layer->load_flags)
    {
        esKRNL_FlagDel(plist_layer->load_flags, OS_DEL_ALWAYS, &err);
    }

    if (plist_layer->wait_flags)
    {
        esKRNL_FlagDel(plist_layer->wait_flags, OS_DEL_ALWAYS, &err);
    }

    if (plist_layer->foucs_flags)
    {
        esKRNL_FlagDel(plist_layer->foucs_flags, OS_DEL_ALWAYS, &err);
    }

    if (plist_layer->load_sem)
    {
        esKRNL_SemDel(plist_layer->load_sem, OS_DEL_ALWAYS, &err);
    }

    if (plist_layer->pos_sem)
    {
        esKRNL_SemDel(plist_layer->pos_sem, OS_DEL_ALWAYS, &err);
    }

    MEM_FREE(plist_layer);
    return OK;
}


/*
**********************************************************************************************************************
*                                               ListLayer_FixedOnDraw
*
* author:
*
* date:             2009-1-17
*
* Description:      listlayer fixedondraw
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_FixedOnDraw(LIST_LAYER_MAN_t *listlayer)
{
    // __s32                ret;
    __u32                num;
    __u32                mid_num = 0;
    MENUITEM_t          *tmpItem;

    if (listlayer->menulist)
    {
        if (listlayer->menulist->ItemCount != 0)
        {
            __inf("ItemCount = %d\n", listlayer->menulist->ItemCount);
            listlayer->head_pos.x             = 0;
            listlayer->head_pos.y             = LISTMENU_FIXEDEXT_WIDTH;
            listlayer->tail_pos.x             = 0;
            listlayer->tail_pos.y             = LISTMENU_FIXEDEXT_WIDTH;
            listlayer->foucs_pos.x            = 0;
            listlayer->foucs_pos.y            = LISTMENU_FIXEDEXT_WIDTH;
            listlayer->unfoucs_pos.x          = 0;
            listlayer->unfoucs_pos.y          = LISTMENU_FIXEDEXT_WIDTH;
            listlayer->viewrect.x             = 0;
            listlayer->viewrect.y             = LISTMENU_FIXEDEXT_WIDTH;

            if (listlayer->menulist->setCurItem == 0)  /*have not set current item*/
            {
                listlayer->setfoucs             = 0;
                listlayer->menulist->CurItem    = listlayer->menulist->head;
            }
            else   /*初始化时已经设置焦点Item*/
            {
                tmpItem = listlayer->menulist->head;

                while (tmpItem != listlayer->menulist->CurItem)
                {
                    listlayer->foucs_pos.x = listlayer->foucs_pos.x;
                    listlayer->foucs_pos.y = listlayer->foucs_pos.y + tmpItem->rect.height;
                    tmpItem = tmpItem->Next;
                    mid_num++;
                }
            }

            num = 0;
            tmpItem = listlayer->menulist->head;

            /*从头开始绘制*/
            while (tmpItem != listlayer->menulist->tail)
            {
                if (num == mid_num)
                {
                    //LISTMENU_PRINTF("foucs x = %d,foucs y = %d\n",listlayer->foucs_pos.x,listlayer->foucs_pos.y);
                    ListLayer_FastLoadMenuItem(listlayer, tmpItem, 0,
                                               listlayer->tail_pos.x, listlayer->tail_pos.y);
                    //LISTMENU_PRINTF("listlayer linear on draw3!\n");
                    ListLayer_DrawFastFoucsMenuItem(listlayer, tmpItem,
                                                    0, listlayer->tail_pos.x, listlayer->tail_pos.y);
                    listlayer->menulist->setCurItem = 1;
                    tmpItem = tmpItem->Next;
                    num++;
                    listlayer->tail_pos.y = listlayer->menulist->CurItem->foucs_w +
                                            listlayer->tail_pos.y;
                }
                else
                {
                    //LISTMENU_PRINTF("listlayer linear on draw2!\n");
                    ListLayer_FastLoadMenuItem(listlayer, tmpItem, 0,
                                               listlayer->tail_pos.x, listlayer->tail_pos.y);
                    tmpItem = tmpItem->Next;
                    num++;
                    listlayer->tail_pos.y = listlayer->menulist->CurItem->rect.height +
                                            listlayer->tail_pos.y;
                }
            }

            if (listlayer->menulist->tail != listlayer->menulist->CurItem)
            {
                //LISTMENU_PRINTF("listlayer linear on draw3!\n");
                ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->tail,
                                           0, listlayer->tail_pos.x, listlayer->tail_pos.y);
                tmpItem = tmpItem->Next;
                listlayer->tail_pos.y = listlayer->menulist->head->rect.height +
                                        listlayer->tail_pos.y;
            }
            else
            {
                //LISTMENU_PRINTF("listlayer linear on draw3!\n");
                ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->tail,
                                           0, listlayer->tail_pos.x, listlayer->tail_pos.y);
                //LISTMENU_PRINTF("listlayer linear on draw3!\n");
                ListLayer_DrawFastFoucsMenuItem(listlayer, listlayer->menulist->tail,
                                                0, listlayer->tail_pos.x, listlayer->tail_pos.y);
                tmpItem = tmpItem->Next;
                listlayer->menulist->setCurItem = 1;
                listlayer->tail_pos.y = listlayer->menulist->CurItem->foucs_w +
                                        listlayer->tail_pos.y;
            }
        }
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_VarOnDraw
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer varondraw
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_VarOnDraw(LIST_LAYER_MAN_t    *listlayer)
{
    //__s32                ret;
    __u32                num;
    __u32                scnnum;
    __u32                count;
    // __u32                mid_num;
    __u32                i;
    MENUITEM_t          *tmpItem;
    MENUITEM_t          *ItemHead;

    if (listlayer->menulist)
    {
        if (listlayer->menulist->ItemCount != 0)
        {
            listlayer->head_pos.x             = 0;
            listlayer->head_pos.y             = 0;
            listlayer->tail_pos.x             = 0;
            listlayer->tail_pos.y             = 0;
            listlayer->foucs_pos.x            = 0;
            listlayer->foucs_pos.y            = 0;
            listlayer->unfoucs_pos.x          = 0;
            listlayer->unfoucs_pos.y          = 0;
            listlayer->setfoucs               = 0;
            listlayer->fillup                 = 0;
            listlayer->filldown               = 0;
            listlayer->viewrect.x             = 0;
            listlayer->viewrect.y             = 0;
            __inf("ItemCount = %d\n", listlayer->menulist->ItemCount);
            scnnum = listlayer->scnnum;

            if (scnnum % 2 == 0)
            {
                num = scnnum / 2 - 1;  /*Item number is even*/
                listlayer->upcount            = num;
                listlayer->downcount          = num + 1;
            }
            else
            {
                num = scnnum / 2;     /*Item number is odd*/
                listlayer->downcount          = num;
                listlayer->upcount            = num;
            }

            /*have not set current item,set head item as current item*/
            if (listlayer->menulist->setCurItem == 0)
            {
                tmpItem = listlayer->menulist->head;
                listlayer->menulist->CurItem = listlayer->menulist->head;

                for (i = 0; i < num; i++)
                {
                    /*set foucs position*/
                    listlayer->foucs_pos.x = listlayer->foucs_pos.x;
                    listlayer->foucs_pos.y = listlayer->foucs_pos.y + listlayer->menulist->CurItem->rect.height;
                }

                listlayer->head_pos.y = listlayer->listbuffer->vrect.height - listlayer->menulist->head->rect.height;
                listlayer->fillup = listlayer->upcount;
                listlayer->menulist->load_head = listlayer->menulist->tail;
                count = num;
                listlayer->menulist->setCurItem = 1;
            }
            else  /*set curitem*/
            {
                tmpItem = listlayer->menulist->CurItem;

                /*compute the foucs position and fill up number*/
                for (i = 0; i < num; i++)
                {
                    /*the item on the head*/
                    if (tmpItem == listlayer->menulist->head)
                    {
                        listlayer->fillup++;
                        listlayer->menulist->load_head = listlayer->menulist->tail;
                    }
                    else
                    {
                        tmpItem = tmpItem->Prev;
                        listlayer->menulist->load_head = tmpItem;
                    }

                    listlayer->foucs_pos.x = listlayer->foucs_pos.x;
                    listlayer->foucs_pos.y = listlayer->foucs_pos.y + listlayer->menulist->CurItem->rect.height;
                }

                count       = listlayer->fillup;
                num         = 0;
                ItemHead    = tmpItem->Prev;
                listlayer->head_pos.y = listlayer->listbuffer->vrect.height - listlayer->menulist->head->rect.height;

                if (listlayer->fillup == 0)
                {
                    /*set fill up region*/
                    while ((ItemHead != listlayer->menulist->tail) && (num < (listlayer->fb_depth / 2 - 1) * scnnum - 1))
                    {
                        ListLayer_FastLoadMenuItem(listlayer, ItemHead,
                                                   0, listlayer->head_pos.x, listlayer->head_pos.y);
                        ItemHead = ItemHead->Prev;
                        listlayer->head_pos.y = listlayer->head_pos.y - listlayer->menulist->CurItem->rect.height ;
                        num++;
                    }

                    listlayer->menulist->load_head = ItemHead;

                    if (ItemHead == listlayer->menulist->tail)
                    {
                        if ((listlayer->fb_depth / 2 - 1) * scnnum - 1 - num > listlayer->upcount)
                        {
                            num = listlayer->upcount;
                        }
                        else
                        {
                            num = (listlayer->fb_depth / 2 - 1) * scnnum - 1 - num;
                        }

                        for (i = 0; i < num; i++)
                        {
                            ListLayer_ClearMenuItem(listlayer, 0, listlayer->head_pos.x,
                                                    listlayer->head_pos.y);
                            listlayer->head_pos.y = listlayer->head_pos.y - listlayer->menulist->CurItem->rect.height;
                        }

                        listlayer->fillup = i;
                    }
                }
                else
                {
                    for (num = 0; num < listlayer->upcount - listlayer->fillup; num++)
                    {
                        ListLayer_ClearMenuItem(listlayer, 0,
                                                listlayer->head_pos.x,
                                                listlayer->head_pos.y);
                        listlayer->head_pos.y = listlayer->head_pos.y - listlayer->menulist->CurItem->rect.height ;
                    }

                    listlayer->fillup = listlayer->upcount;
                    listlayer->menulist->load_head = listlayer->menulist->tail;
                }
            }

            /*draw fill down region*/
            num = 0;

            if (listlayer->menulist->ItemCount != 1)
            {
                while ((tmpItem != listlayer->menulist->tail) && (num < (listlayer->fb_depth / 2) * scnnum))
                {
                    if (count > 0)
                    {
                        ListLayer_ClearMenuItem(listlayer, 0,
                                                listlayer->tail_pos.x, listlayer->tail_pos.y);
                        count--;
                    }
                    else
                    {
                        ListLayer_FastLoadMenuItem(listlayer, tmpItem,
                                                   0, listlayer->tail_pos.x,
                                                   listlayer->tail_pos.y);
                        tmpItem = tmpItem->Next;
                    }

                    listlayer->tail_pos.y = listlayer->menulist->CurItem->rect.height + listlayer->tail_pos.y;
                    num++;
                }

                if (num < (listlayer->fb_depth / 2) * scnnum)
                {
                    ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->tail,
                                               0, listlayer->tail_pos.x,
                                               listlayer->tail_pos.y);
                    tmpItem = tmpItem->Next;
                    listlayer->tail_pos.y = listlayer->menulist->head->rect.height + listlayer->tail_pos.y;
                    num++;
                    listlayer->menulist->load_tail = tmpItem;
                }

                listlayer->menulist->load_tail = tmpItem;
            }
            else
            {
                while (count > 0)
                {
                    ListLayer_ClearMenuItem(listlayer, 0,
                                            listlayer->tail_pos.x, listlayer->tail_pos.y);
                    count--;
                    listlayer->tail_pos.y = listlayer->menulist->head->rect.height + listlayer->tail_pos.y;
                }

                ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->tail,
                                           0, listlayer->tail_pos.x,
                                           listlayer->tail_pos.y);
                listlayer->tail_pos.y = listlayer->menulist->head->rect.height + listlayer->tail_pos.y;
                listlayer->menulist->load_tail = listlayer->menulist->head;
            }

            listlayer->filldown = 0;

            if (listlayer->menulist->load_tail == listlayer->menulist->head)
            {
                if ((listlayer->fb_depth / 2) * scnnum - num > listlayer->downcount)
                {
                    num = listlayer->downcount;
                }
                else
                {
                    num = (listlayer->fb_depth / 2) * scnnum - num;
                }

                for (count = 0; count < num; count++)
                {
                    ListLayer_ClearMenuItem(listlayer, 0, listlayer->tail_pos.x,
                                            listlayer->tail_pos.y);
                    listlayer->tail_pos.y = listlayer->menulist->head->rect.height + listlayer->tail_pos.y;
                }

                listlayer->filldown = count;
            }

            ListLayer_HeadToTailCopy(listlayer);
            listlayer->setfoucs = REAL_FOUCS;
        }
    }

    return OK;
}


/*
**********************************************************************************************************************
*                                               ListLayer_Sleep
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer sleep
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_Sleep(LIST_LAYER_MAN_t    *listlayer)
{
    __s32                ret;
    ret = GUI_LyrWinSetSta(listlayer->layer, GUI_LYRWIN_STA_SUSPEND);

    if (ret != OK)
    {
        __wrn("layer sleep failed!\n");
        return __LINE__;
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_Wakeup
*
* author:
*
* date:             2008-12-9
*
* Description:      listlayer wakeup
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_Wakeup(LIST_LAYER_MAN_t    *listlayer)
{
    __u32                ret;
    ret = ListLayer_GetLayerInfo(listlayer);

    if (ret != OK)
    {
        __wrn("layer get layer information failed!\n");
        return __LINE__;
    }

    ret = ListLayer_AdjustFb(listlayer, listlayer->menulist->head->rect.height,
                             listlayer->menulist->head->foucs_w);

    if (ret != OK)
    {
        __wrn("layer adjust framebuffer failed!\n");
        return __LINE__;
    }

    ret = ListLayer_OnDraw(listlayer);

    if (ret != OK)
    {
        __wrn("layer draw failed!\n");
        return __LINE__;
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_OnDraw
*
* author:
*
* date:             2008-10-9
*
* Description:      listlayer ondraw
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_OnDraw(LIST_LAYER_MAN_t    *listlayer)
{
    __s32                ret;

    if (listlayer->menulist)
    {
        if (listlayer->menulist->ItemCount != 0)
        {
            if (listlayer->menulist->CurItem == NULL)
            {
                listlayer->menulist->setCurItem = 0;
            }

            if (listlayer->mode == VAR_MODE)
            {
                if (listlayer->plistmenu->mode & LISTBOX_MOD)
                {
                    /*item动态变化*/
                    ListLayer_VarBoxOnDraw(listlayer);
                }
                else
                {
                    /*item动态变化*/
                    ListLayer_VarOnDraw(listlayer);
                }
            }
            else
            {
                if (listlayer->plistmenu->mode & LISTBOX_MOD)
                {
                    /*item动态变化*/
                    ListLayer_FixedBoxOnDraw(listlayer);
                }
                else
                {
                    /*item动态变化*/
                    ListLayer_FixedOnDraw(listlayer);
                }
            }

            {
                RECT  rect;
                rect.x      = listlayer->viewrect.x;
                rect.y      = listlayer->viewrect.y;
                rect.width  = listlayer->viewrect.width;
                rect.height = listlayer->viewrect.height;
                ret = GUI_LyrWinSetSrcWindow(listlayer->layer, (__rect_t *)&rect);

                if (ret != EPDK_OK)
                {
                    __wrn("layer set src windows failed,layer handle = %d\n", listlayer->layer);
                    return __LINE__;
                }
            }
        }

        if (listlayer->plistmenu->mode & LISTBOX_MOD)
        {
            listlayer->menulist->SelItem    = listlayer->menulist->HeadItem;
            listlayer->menulist->dist       = listlayer->menulist->SelItem->itemno * listlayer->menulist->SelItem->rect.height;

            if (listlayer->menulist->ItemCount > listlayer->scnnum)
            {
                listlayer->menulist->total_dist = (listlayer->menulist->ItemCount  - listlayer->scnnum) * listlayer->menulist->SelItem->rect.height;
            }
            else
            {
                listlayer->menulist->total_dist = 0;
            }
        }
        else
        {
            listlayer->menulist->SelItem        = listlayer->menulist->CurItem;
            listlayer->menulist->dist           = listlayer->menulist->SelItem->itemno * listlayer->menulist->SelItem->rect.height;
            listlayer->menulist->total_dist     = (listlayer->menulist->ItemCount  - 1) * listlayer->menulist->SelItem->rect.height;
        }
    }

    //listlayer->plistmenu->lyrsts = LISTMENU_SLEEP;
    SYSTEM_PRINTF("listlayer ondraw!\n");
    // listlayer->viewrect.height = listlayer->viewrect.height - listlayer->viewrect.height;
    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_SetWinFoucs
*
* author:
*
* date:             2008-12-9
*
* Description:      listlayer setwinfoucs
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_SetWinFoucs(LIST_LAYER_MAN_t    *listlayer)
{
    if (listlayer->menulist)
    {
        if (listlayer->menulist->ItemCount != 0)
        {
            //LISTMENU_PRINTF("foucs x = %d,foucs y = %d\n",listlayer->foucs_pos.x,listlayer->foucs_pos.y);
            ListLayer_FastFoucsMenuItem(listlayer, listlayer->menulist->CurItem,
                                        0, listlayer->foucs_pos.x, listlayer->foucs_pos.y);
            //LISTMENU_PRINTF("foucs x = %d,foucs y = %d\n",listlayer->foucs_pos.x,listlayer->foucs_pos.y);
            ListLayer_DrawFoucsMenuItem(listlayer, listlayer->menulist->CurItem,
                                        0, listlayer->foucs_pos.x, listlayer->foucs_pos.y);
            listlayer->setfoucs = REAL_FOUCS;
        }
        else  /*clear listmenu framebuffer*/
        {
#if 0
            void                *base;
            __u32                bpp;
            __u32                size;
            bpp = listlayer->menulist->bpp;
            size = (listlayer->listbuffer->rectsz.width *
                    listlayer->listbuffer->rectsz.height * bpp + 7) / 8;
            base = (void *)((__u32)listlayer->listbuffer->buffer);

            if (listlayer->plistmenu->lyrsts != LISTMENU_SLEEP)
            {
                /*清除framebuffer*/
                eLIBs_memset(base, 0x00, size);
            }

#else
            GUI_LyrWinSel(listlayer->layer);
            GUI_SetBkColor(listlayer->bk_color);
            GUI_ClearRect(0, 0, listlayer->listbuffer->rectsz.width - 1, listlayer->listbuffer->rectsz.height - 1);
#endif
        }
    }

    return OK;
}


/*
**********************************************************************************************************************
*                                               ListLayer_SetWinUnFoucs
*
* author:
*
* date:             2009-9-20
*
* Description:      listlayer setwinunfoucs
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_SetWinUnFoucs(LIST_LAYER_MAN_t    *listlayer)
{
    __s32                ret;

    if (listlayer->menulist)
    {
        if (listlayer->menulist->ItemCount != 0)
        {
            if (listlayer->setfoucs == REAL_FOUCS)
            {
                ret = ListLayer_UnFoucsMenuItem(listlayer, listlayer->menulist->CurItem,
                                                0, listlayer->foucs_pos.x, listlayer->foucs_pos.y);

                if (ret != OK)
                {
                    return __LINE__;
                }
            }
            else if (listlayer->setfoucs == VAR_FOUCS)
            {
                ret = ListLayer_FastUnFoucsMenuItem(listlayer, listlayer->menulist->CurItem,
                                                    0, listlayer->foucs_pos.x, listlayer->foucs_pos.y);

                if (ret != OK)
                {
                    return __LINE__;
                }
            }

            listlayer->setfoucs = NO_FOUCS;
        }
        else  /*clear listmenu framebuffer*/
        {
#if 0
            void                *base;
            __u32                bpp;
            __u32                size;
            bpp = listlayer->menulist->bpp;
            size = (listlayer->listbuffer->rectsz.width *
                    listlayer->listbuffer->rectsz.height * bpp + 7) / 8;
            base = (void *)((__u32)listlayer->listbuffer->buffer);

            if (listlayer->plistmenu->lyrsts != LISTMENU_SLEEP)
            {
                /*清除framebuffer*/
                eLIBs_memset(base, 0x00, size);
            }

#else
            GUI_LyrWinSel(listlayer->layer);
            GUI_SetBkColor(listlayer->bk_color);
            GUI_ClearRect(0, 0, listlayer->listbuffer->rectsz.width - 1, listlayer->listbuffer->rectsz.height - 1);
#endif
        }
    }

    return OK;
}
//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------
#endif //__LIST_LAYER__C____
