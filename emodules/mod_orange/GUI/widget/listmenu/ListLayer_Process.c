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
#ifndef __LIST_LAYER_PROCESS_C____
#define __LIST_LAYER_PROCESS_C____


#include "config.h"
#include "ListMenu_i.h"
#include "ListLayer.h"
#include "MenuList.h"
#include "MenuItem.h"
#include "ListLayer_Process.h"
#include "ListBox.h"
#include "ListBox_Process.h"
#include "sysmemory.h"


/*fast focus keytone process*/
void _lstm_key_song(void)
{
#if 0
    __gui_msg_t msg;
    msg.id  = GUI_MSG_KEYTONE;
    msg.h_srcwin = 0;
    msg.h_deswin = GUI_WinGetBackWin();
    msg.p_arg    = (void *)0;
    GUI_SendMessage(&msg);
#endif
}


/*
**********************************************************************************************************************
*                                               InterMove
*
* author:
*
* date:             2009-10-15
*
* Description:      intermove
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
/*internal move curve function definition*/
__u32  InterMove(__u32 mode, __s32 range, __u32 Steps, __u32 CurStep, __move_para_t *Parameter)
{
    if (range < Steps)
    {
        if (CurStep < range)
        {
            Parameter->dist = 1;
            Parameter->dly_time = 0;
            return OK;
        }
        else
        {
            Parameter->dist = 0;
            Parameter->dly_time = 0;
            return OK;
        }
    }

    if (range % Steps != 0)
    {
        if (CurStep < Steps - 1)
        {
            Parameter->dist = range / Steps;
            Parameter->dly_time = 0;
            return OK;
        }
        else
        {
            Parameter->dist = range % Steps + range / Steps;
            Parameter->dly_time = 0;
            return OK;
        }
    }
    else
    {
        if (CurStep < Steps)
        {
            Parameter->dist = range / Steps;
            Parameter->dly_time = 0;
            return OK;
        }

        return OK;
    }
}

void ListLayer_MenuItemUpdateChild(MENUITEM_t   *menuitem)
{
    if (menuitem->nctrls)
    {
        __u32   count;
        RECT    new_rect;

        for (count = 0; count < menuitem->nctrls; count++)
        {
            new_rect.x          = menuitem->rect.x + menuitem->child[count].win_rect.x;
            new_rect.y          = menuitem->rect.y + menuitem->child[count].win_rect.y;
            new_rect.width      = menuitem->child[count].win_rect.width;
            new_rect.height     = menuitem->child[count].win_rect.height;
            GUI_WinSetStyle(menuitem->child[count].hWnd, GUI_WinGetStyle(menuitem->child[count].hWnd) | WS_VISIBLE);
            GUI_WinSetWinRECT(menuitem->child[count].hWnd, &new_rect);
            GUI_WinSetClientRECT(menuitem->child[count].hWnd, &new_rect);
            GUI_WinUpdate(menuitem->child[count].hWnd, ORANGE_TRUE);
        }
    }
}
/*
**********************************************************************************************************************
*                                               ListLayer_MenuItemCopy
*
* author:
*
* date:             2009-10-15
*
* Description:      listlayer menuitemcopy
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListLayer_MenuItemCopy(LIST_LAYER_MAN_t    *listlayer, __u32 srcfbno, __u32 dstfbno, __u32 src_x, __u32 src_y, __u32 dst_x, __u32 dst_y)
{
    __u32                size;
    void                *src_addr;
    void                *dst_addr;
    __u32                bpp;
    __u32                i;
    __u32                width;
    __u32                height;
    __u32                fb_width;

    if (listlayer->dir == GUI_SCNDIR_NORMAL)
    {
        bpp         = listlayer->menulist->bpp;
        size        = (((listlayer->menulist->head->rect.width) * (listlayer->menulist->head->rect.height) * bpp + 7) / 8);
        src_addr    = (void *)((unsigned long)listlayer->listbuffer->buffer +
                               ((listlayer->listbuffer->rectsz.width * src_y * bpp + 7) / 8) +
                               ((src_x * bpp + 7) / 8));   /*get source address */
        /*get framebuffer destination address */
        dst_addr    = (void *)((unsigned long)listlayer->listbuffer->buffer +
                               ((listlayer->listbuffer->rectsz.width * dst_y * bpp + 7) / 8) +
                               ((dst_x * bpp + 7) / 8));
        sysmemcpy(dst_addr, src_addr, size);
    }
    else if (listlayer->dir == GUI_SCNDIR_ROTATE90)
    {
        dst_addr = (void *)((unsigned long)listlayer->listbuffer->buffer +
                            ((dst_y * listlayer->menulist->bpp + 7) >> 3));
        src_addr = (void *)((unsigned long)listlayer->listbuffer->buffer +
                            ((src_y * listlayer->menulist->bpp + 7) >> 3));
        width    = (listlayer->menulist->head->rect.height * listlayer->menulist->bpp + 7) >> 3;
        height   = listlayer->listbuffer->rectsz.width;
        fb_width = (listlayer->lyr_size.height * listlayer->menulist->bpp + 7) >> 3;

        for (i = 0; i < height; i++)
        {
            sysmemcpy(dst_addr, src_addr, width);
            src_addr = (void *)((unsigned long)src_addr + fb_width);
            dst_addr = (void *)((unsigned long)dst_addr + fb_width);
        }
    }
    else if (listlayer->dir == GUI_SCNDIR_ROTATE180)
    {
        bpp         = listlayer->menulist->bpp;
        size        = (((listlayer->menulist->head->rect.width) * (listlayer->menulist->head->rect.height) * bpp + 7) / 8);
        src_addr    = (void *)((unsigned long)listlayer->listbuffer->buffer +
                               ((listlayer->listbuffer->rectsz.width * (listlayer->listbuffer->rectsz.height - src_y - listlayer->menulist->head->rect.height) * bpp + 7) / 8) +
                               ((src_x * bpp + 7) / 8));   /*get source address */
        /*get framebuffer destination address */
        dst_addr    = (void *)((unsigned long)listlayer->listbuffer->buffer +
                               ((listlayer->listbuffer->rectsz.width * (listlayer->listbuffer->rectsz.height - dst_y - listlayer->menulist->head->rect.height) * bpp + 7) / 8) +
                               ((dst_x * bpp + 7) / 8));
        sysmemcpy(dst_addr, src_addr, size);
    }
    else
    {
        dst_addr = (void *)((unsigned long)listlayer->listbuffer->buffer +
                            (((listlayer->listbuffer->rectsz.height - dst_y - listlayer->menulist->head->rect.height) *
                              listlayer->menulist->bpp + 7) >> 3));
        src_addr = (void *)((unsigned long)listlayer->listbuffer->buffer +
                            (((listlayer->listbuffer->rectsz.height - src_y - listlayer->menulist->head->rect.height) *
                              listlayer->menulist->bpp + 7) >> 3));
        width    = (listlayer->menulist->head->rect.height * listlayer->menulist->bpp + 7) >> 3;
        height   = listlayer->listbuffer->rectsz.width;
        fb_width = (listlayer->lyr_size.height * listlayer->menulist->bpp + 7) >> 3;

        for (i = 0; i < height; i++)
        {
            sysmemcpy(dst_addr, src_addr, width);
            src_addr = (void *)((unsigned long)src_addr + fb_width);
            dst_addr = (void *)((unsigned long)dst_addr + fb_width);
        }
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_FoucsMenuItemCopy
*
* author:
*
* date:             2009-10-15
*
* Description:      listlayer foucsmenuitemcopy
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListLayer_FoucsMenuItemCopy(LIST_LAYER_MAN_t    *listlayer, __u32 srcfbno, __u32 dstfbno, __u32 src_x, __u32 src_y, __u32 dst_x, __u32 dst_y)
{
    __u32                size;
    void                *src_addr;
    void                *dst_addr;
    __u32                bpp;
    bpp         = listlayer->menulist->bpp;
    size        = (((listlayer->menulist->head->rect.width) *
                    (listlayer->menulist->head->foucs_w) * bpp + 7) / 8);
    src_addr    = (void *)((unsigned long)listlayer->listbuffer->buffer +
                           ((listlayer->listbuffer->rectsz.width * src_y * bpp + 7) / 8) +
                           ((src_x * bpp + 7) / 8));   /*get source address */
    /*get framebuffer destination address */
    dst_addr = (void *)((unsigned long)listlayer->listbuffer->buffer +
                        ((listlayer->listbuffer->rectsz.width * dst_y * bpp + 7) / 8) +
                        ((dst_x * bpp + 7) / 8));
    sysmemcpy(dst_addr, src_addr, size);
    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_LoadMenuItem
*
* author:
*
* date:             2009-1-17
*
* Description:      listlayer loadmenuitem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListLayer_LoadMenuItem(LIST_LAYER_MAN_t *listlayer, MENUITEM_t   *menuitem,
                              __u8  fbno, __u32 x, __u32 y)
{
    __draw_para_t               drawpara;
    __u32                       ret = 0;

    if (!menuitem)
    {
        return OK;
    }

    /*get current draw rectangle*/
    menuitem->rect.x = x;
    menuitem->rect.y = y;
    /*set current draw mode*/
    drawpara.mode   =   GUI_PAIN_MODE_DRAW;
    drawpara.item   =   GUI_ItemP2H(menuitem);
    /*get the draw item flag*/
    POS_PRINTF("Draw Posx = %d,y = %d\n", x, y);

    if (listlayer->plistmenu->lyrsts != LISTMENU_SLEEP)
    {
        /*call menitem callback function*/
        ret = esKRNL_CallBack((__pCBK_t) * menuitem->cb, &drawpara);
        ListLayer_MenuItemUpdateChild(menuitem);

        if (ret != LSTM_DRAWCB_ERROR)
        {
            menuitem->status = LOAD_STS;
        }
    }

    POS_PRINTF("Draw Posx = %d,y = %d,OK\n", x, y);
    return ret;
}


/*
**********************************************************************************************************************
*                                               ListLayer_FastLoadMenuItem
*
* author:
*
* date:             2009-1-19
*
* Description:      listlayer fastloadmenuitem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListLayer_FastLoadMenuItem(LIST_LAYER_MAN_t *listlayer, MENUITEM_t   *menuitem
                                  , __u8       fbno, __u32 x, __u32 y)
{
    __draw_para_t               drawpara;

    if (!menuitem)
    {
        return OK;
    }

    /*get current draw rectangle*/
    menuitem->rect.x = x;
    menuitem->rect.y = y;
    /*set current draw mode*/
    drawpara.mode   =   GUI_PAIN_MODE_FASTDRAW;
    drawpara.item   =   GUI_ItemP2H(menuitem);
    /*get the draw item flag*/
    POS_PRINTF("FAST Draw Posx = %d,y = %d\n", x, y);

    if (listlayer->plistmenu->lyrsts != LISTMENU_SLEEP)
    {
        /*call menitem callback function*/
        esKRNL_CallBack((__pCBK_t)*menuitem->cb, &drawpara);
        ListLayer_MenuItemUpdateChild(menuitem);
        menuitem->status = FLOAD_STS;
    }

    POS_PRINTF("FAST Draw Posx = %d,y = %d,OK\n", x, y);
    return 0;
}


/*
**********************************************************************************************************************
*                                               ListLayer_UnFoucsMenuItem
*
* author:
*
* date:             2008-12-08
*
* Description:      listlayer unfoucsmenuitem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListLayer_UnFoucsMenuItem(LIST_LAYER_MAN_t *listlayer, MENUITEM_t   *menuitem,
                                 __u8 fbno, __u32 x, __u32 y)
{
    __draw_para_t               drawpara;
    __u32                       ret = 0;

    if (!menuitem)
    {
        return OK;
    }

    /*get current draw rectangle*/
    menuitem->rect.x = x;
    menuitem->rect.y = y;
    /*set current draw mode*/
    drawpara.mode = GUI_PAIN_MODE_UNFOCUS;
    drawpara.item   =   GUI_ItemP2H(menuitem);
    /*get the draw item flag*/
    POS_PRINTF("unfoucs Posx = %d,y = %d\n", x, y);

    /*call menitem callback function*/
    if (listlayer->plistmenu->lyrsts != LISTMENU_SLEEP)
    {
        ret = esKRNL_CallBack((__pCBK_t) * menuitem->cb, &drawpara);
        ListLayer_MenuItemUpdateChild(menuitem);

        if (ret != LSTM_DRAWCB_ERROR)
        {
            menuitem->status = UNFOUCS_STS;
        }

        listlayer->setfoucs = NO_FOUCS;
    }

    POS_PRINTF("unfoucs Posx = %d,y = %d,OK\n", x, y);
    return ret;
}


/*
**********************************************************************************************************************
*                                               ListLayer_FastUnFoucsMenuItem
*
* author:
*
* date:             2008-12-11
*
* Description:      listlayer fastunfoucsmenuitem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListLayer_FastUnFoucsMenuItem(LIST_LAYER_MAN_t *listlayer, MENUITEM_t   *menuitem,
                                     __u8       fbno, __u32 x, __u32 y)
{
    __draw_para_t               drawpara;

    if (listlayer->plistmenu->mode & UNFOCUS_MOD)
    {
        return OK;
    }

    if (!menuitem)
    {
        return OK;
    }

    /*get current draw rectangle*/
    menuitem->rect.x = x;
    menuitem->rect.y = y;
    /*set current draw mode*/
    drawpara.mode   =   GUI_PAIN_MODE_FASTUNFOCUS;
    drawpara.item   =   GUI_ItemP2H(menuitem);
    /*get the draw item flag*/
    POS_PRINTF("unfoucs Posx = %d,y = %d\n", x, y);

    if (listlayer->plistmenu->lyrsts != LISTMENU_SLEEP)
    {
        /*call menitem callback function*/
        esKRNL_CallBack((__pCBK_t)*menuitem->cb, &drawpara);
        ListLayer_MenuItemUpdateChild(menuitem);
        menuitem->status = FFOUCS_STS;
        listlayer->setfoucs = NO_FOUCS;
    }

    POS_PRINTF("unfoucs Posx = %d,y = %d,OK\n", x, y);
    return 0;
}

/*
**********************************************************************************************************************
*                                               ListLayer_DrawFastFoucsMenuItem
*
* author:
*
* date:             2008-12-11
*
* Description:      listlayer drawfastfoucsmenuitem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListLayer_DrawFastFoucsMenuItem(LIST_LAYER_MAN_t *listlayer, MENUITEM_t   *menuitem,
                                       __u8       fbno, __u32 x, __u32 y)
{
    __draw_para_t               drawpara;

    if (listlayer->plistmenu->mode & UNFOCUS_MOD)
    {
        return OK;
    }

    if (!menuitem)
    {
        return OK;
    }

    /*get current draw rectangle*/
    menuitem->rect.x = x;
    menuitem->rect.y = y;
    /*set current draw mode*/
    drawpara.mode   =   GUI_PAIN_MODE_FASTFOCUS;
    drawpara.item   =   GUI_ItemP2H(menuitem);
    /*get the draw item flag*/
    POS_PRINTF("fast Posx = %d,y = %d\n", x, y);

    if (listlayer->plistmenu->lyrsts != LISTMENU_SLEEP)
    {
        /*call menitem callback function*/
        esKRNL_CallBack((__pCBK_t)*menuitem->cb, &drawpara);
        ListLayer_MenuItemUpdateChild(menuitem);
        menuitem->status = FFOUCS_STS;
        listlayer->setfoucs = VAR_FOUCS;
    }

    POS_PRINTF("fast Posx = %d,y = %d,OK\n", x, y);
    return 0;
}


/*
**********************************************************************************************************************
*                                               ListLayer_ClearMenuItem
*
* author:
*
* date:             2008-12-8
*
* Description:      listlayer clearmenuitem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListLayer_ClearMenuItem(LIST_LAYER_MAN_t    *listlayer, __u8   fbno, __u32 x, __u32 y)
{
#if 0
    void                *base;
    __u32                bpp;
    __u32                size;
    bpp = listlayer->menulist->bpp;
    size = (listlayer->menulist->CurItem->rect.width *
            listlayer->menulist->CurItem->rect.height * listlayer->menulist->bpp + 7) / 8;
    base = (void *)((__u32)listlayer->listbuffer->buffer +
                    (listlayer->listbuffer->rectsz.width * y * bpp + 7) / 8 +
                    (x * bpp + 7) / 8);

    if (listlayer->plistmenu->lyrsts != LISTMENU_SLEEP)
    {
        eLIBs_memset(base, 0x00, size);
    }

#else
    GUI_LyrWinSel(listlayer->layer);
    GUI_SetBkColor(listlayer->bk_color);
    GUI_ClearRect(x, y, x + listlayer->listbuffer->rectsz.width - 1, y + listlayer->menulist->CurItem->rect.height - 1);
#endif
    return 0;
}


/*
**********************************************************************************************************************
*                                               ListLayer_FastFoucsMenuItem
*
* author:
*
* date:             2008-12-8
*
* Description:      listlayer fastfoucsmenuitem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListLayer_FastFoucsMenuItem(LIST_LAYER_MAN_t    *listlayer, MENUITEM_t   *menuitem,
                                   __u8       fbno, __u32 x, __u32 y)
{
    __draw_para_t               drawpara;

    if (listlayer->plistmenu->mode & UNFOCUS_MOD)
    {
        return OK;
    }

    if (!menuitem)
    {
        return OK;
    }

    /*get current draw rectangle*/
    menuitem->rect.x = x;
    menuitem->rect.y = y;
    /*set current draw mode*/
    drawpara.mode   =   GUI_PAIN_MODE_FASTFOCUS;
    drawpara.item   =   GUI_ItemP2H(menuitem);
    /*get the draw item flag*/
    POS_PRINTF("fast Posx = %d,y = %d\n", x, y);

    if (listlayer->plistmenu->lyrsts != LISTMENU_SLEEP)
    {
        /*call menitem callback function*/
        esKRNL_CallBack((__pCBK_t)*menuitem->cb, &drawpara);
        ListLayer_MenuItemUpdateChild(menuitem);
        menuitem->status = FFOUCS_STS;
        listlayer->setfoucs = VAR_FOUCS;
    }

    POS_PRINTF("fast Posx = %d,y = %d,OK\n", x, y);
    return 0;
}


/*
**********************************************************************************************************************
*                                               ListLayer_DrawFoucsMenuItem
*
* author:
*
* date:             2008-12-8
*
* Description:      listlayer drawfoucsmenuitem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListLayer_DrawFoucsMenuItem(LIST_LAYER_MAN_t    *listlayer, MENUITEM_t   *menuitem,
                                   __u8       fbno, __u32 x, __u32 y)
{
    __draw_para_t               drawpara;
    __u32                       ret = 0;

    if (!menuitem)
    {
        return OK;
    }

    /*get current draw rectangle*/
    menuitem->rect.x = x;
    menuitem->rect.y = y;
    ///*set current draw mode*/
    drawpara.mode   =   GUI_PAIN_MODE_FOCUS;
    drawpara.item   =   GUI_ItemP2H(menuitem);

    if (listlayer->plistmenu->lyrsts != LISTMENU_SLEEP)
    {
        /*call menitem callback function*/
        ret = esKRNL_CallBack((__pCBK_t) * menuitem->cb, &drawpara);
        ListLayer_MenuItemUpdateChild(menuitem);

        if (ret != LSTM_DRAWCB_ERROR)
        {
            menuitem->status = FOUCS_STS;
        }

        listlayer->setfoucs = REAL_FOUCS;
    }

    POS_PRINTF("Foucs Posx = %d,y = %d,OK\n", x, y);
    //esKRNL_SemPost(listlayer->draw_sem); /*pend pos semphore*/
    return ret;
}


/*
**********************************************************************************************************************
*                                               ListLayer_DrawFoucsMenuItem
*
* author:
*
* date:             2008-12-8
*
* Description:      listlayer drawfoucsmenuitem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListLayer_DrawActivedMenuItem(LIST_LAYER_MAN_t    *listlayer, MENUITEM_t   *menuitem,
                                     __u8       fbno, __u32 x, __u32 y)
{
    __draw_para_t               drawpara;
    __u32                       ret = 0;

    if (!menuitem)
    {
        return OK;
    }

    /*get current draw rectangle*/
    menuitem->rect.x = x;
    menuitem->rect.y = y;
    ///*set current draw mode*/
    drawpara.mode   =   GUI_PAIN_MODE_ACTIVED;
    drawpara.item   =   GUI_ItemP2H(menuitem);

    if (listlayer->plistmenu->lyrsts != LISTMENU_SLEEP)
    {
        /*call menitem callback function*/
        ret = esKRNL_CallBack((__pCBK_t) * menuitem->cb, &drawpara);
        ListLayer_MenuItemUpdateChild(menuitem);

        if (ret != LSTM_DRAWCB_ERROR)
        {
            menuitem->status = FOUCS_STS;
        }

        listlayer->setfoucs = REAL_FOUCS;
    }

    POS_PRINTF("Foucs Posx = %d,y = %d,OK\n", x, y);
    //esKRNL_SemPost(listlayer->draw_sem); /*pend pos semphore*/
    return ret;
}

/*
**********************************************************************************************************************
*                                               ListLayer_DrawFoucsMenuItem
*
* author:
*
* date:             2008-12-8
*
* Description:      listlayer drawfoucsmenuitem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListLayer_DrawInActivedMenuItem(LIST_LAYER_MAN_t    *listlayer, MENUITEM_t   *menuitem,
                                       __u8       fbno, __u32 x, __u32 y)
{
    __draw_para_t               drawpara;
    __u32                       ret = 0;

    if (!menuitem)
    {
        return OK;
    }

    /*get current draw rectangle*/
    menuitem->rect.x = x;
    menuitem->rect.y = y;
    ///*set current draw mode*/
    drawpara.mode   =   GUI_PAIN_MODE_INACTIVED;
    drawpara.item   =   GUI_ItemP2H(menuitem);

    if (listlayer->plistmenu->lyrsts != LISTMENU_SLEEP)
    {
        /*call menitem callback function*/
        ret = esKRNL_CallBack((__pCBK_t) * menuitem->cb, &drawpara);
        ListLayer_MenuItemUpdateChild(menuitem);

        if (ret != LSTM_DRAWCB_ERROR)
        {
            menuitem->status = INACTIVED_STS;
        }
    }

    POS_PRINTF("Foucs Posx = %d,y = %d,OK\n", x, y);
    //esKRNL_SemPost(listlayer->draw_sem); /*pend pos semphore*/
    return ret;
}
/*
**********************************************************************************************************************
*                                               ListLayer_SetLayerFastFoucs
*
* author:
*
* date:             2008-12-8
*
* Description:      listlayer setlayerfastfoucs
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_SetLayerFastFoucs(LIST_LAYER_MAN_t    *listlayer, MENUITEM_t   *menuitem)
{
    __u8                       fbno;
    //__u32                      ret;
    fbno = 0;
    LISTMENU_PRINTF("foucs no = %d\n", fbno);
    //__inf("CurItem ID = %d\n",((MENUITEM_t *)menuitem)->itemno);
    ListLayer_FastFoucsMenuItem(listlayer, menuitem, fbno,
                                listlayer->foucs_pos.x,
                                listlayer->foucs_pos.y);
    return OK;
}


/*
**********************************************************************************************************************
*                                               ListLayer_SetLayerFoucs
*
* author:
*
* date:             2008-12-8
*
* Description:      listlayer setlayerfoucs
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_SetLayerFoucs(LIST_LAYER_MAN_t    *listlayer, MENUITEM_t   *menuitem)
{
    __u8                       fbno;
    //__u32                      ret;
    fbno = 0;
    LISTMENU_PRINTF("foucs no = %d\n", fbno);
    ListLayer_DrawFoucsMenuItem(listlayer, menuitem, fbno,
                                listlayer->foucs_pos.x,
                                listlayer->foucs_pos.y);
    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_SetLayerUnFoucs
*
* author:
*
* date:             2008-12-8
*
* Description:      listlayer setlayerunfoucs
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_SetLayerUnFoucs(LIST_LAYER_MAN_t    *listlayer)
{
    __u8                       fbno;
    //__u32                      ret;
    fbno = 0;

    if (!listlayer->menulist->UnCurItem)
    {
        return OK;
    }

    LISTMENU_PRINTF("unfoucs no = %d\n", fbno);

    if (listlayer->unfoucs_pos.y >= listlayer->listbuffer->vrect.height)
    {
        /*如果设置真实的焦点*/
        if (listlayer->setfoucs == REAL_FOUCS)
        {
            ListLayer_UnFoucsMenuItem(listlayer, listlayer->menulist->UnCurItem,
                                      fbno, listlayer->unfoucs_pos.x,
                                      listlayer->unfoucs_pos.y);
        }
        else /*设置滚动中的焦点*/
        {
            ListLayer_FastUnFoucsMenuItem(listlayer, listlayer->menulist->UnCurItem,
                                          fbno, listlayer->unfoucs_pos.x,
                                          listlayer->unfoucs_pos.y);
        }

        if (listlayer->mode != FIXED_MODE)
        {
            ListLayer_MenuItemCopy(listlayer, fbno, fbno, listlayer->unfoucs_pos.x,
                                   listlayer->unfoucs_pos.y, listlayer->unfoucs_pos.x,
                                   listlayer->unfoucs_pos.y - listlayer->listbuffer->vrect.height);
        }
    }
    else if (listlayer->unfoucs_pos.y < listlayer->viewrect.height)
    {
        /*如果设置真实的焦点*/
        if (listlayer->setfoucs == REAL_FOUCS)
        {
            ListLayer_UnFoucsMenuItem(listlayer, listlayer->menulist->UnCurItem,
                                      fbno, listlayer->unfoucs_pos.x,
                                      listlayer->unfoucs_pos.y);
        }
        else /*设置滚动中的焦点*/
        {
            ListLayer_FastUnFoucsMenuItem(listlayer, listlayer->menulist->UnCurItem,
                                          fbno, listlayer->unfoucs_pos.x,
                                          listlayer->unfoucs_pos.y);
        }

        if (listlayer->mode != FIXED_MODE)
        {
            ListLayer_MenuItemCopy(listlayer, fbno, fbno, listlayer->unfoucs_pos.x,
                                   listlayer->unfoucs_pos.y, listlayer->unfoucs_pos.x,
                                   listlayer->unfoucs_pos.y + listlayer->listbuffer->vrect.height);
        }
    }
    else
    {
        /*如果设置真实的焦点*/
        if (listlayer->setfoucs == REAL_FOUCS)
        {
            ListLayer_UnFoucsMenuItem(listlayer, listlayer->menulist->UnCurItem,
                                      fbno, listlayer->unfoucs_pos.x,
                                      listlayer->unfoucs_pos.y);
        }
        else /*设置滚动中的焦点*/
        {
            ListLayer_FastUnFoucsMenuItem(listlayer, listlayer->menulist->UnCurItem,
                                          fbno, listlayer->unfoucs_pos.x,
                                          listlayer->unfoucs_pos.y);
        }
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_DrawItemProcess
*
* author:
*
* date:             2008-12-8
*
* Description:      listlayer drawitemprocess
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_DrawItemProcess(LIST_LAYER_MAN_t    *listlayer)
{
    __u8                        fbno;
    __u32                       ret;
    __pos_t                     curh_pos;    /*当前头加载位置*/
    __pos_t                     curt_pos;    /*当前尾加载位置*/
    MENUITEM_t                 *curhitem;   /*当前头加载item*/
    MENUITEM_t                 *curtitem;   /*当前尾加载item*/
    MENUITEM_t                 *curitem;
    fbno        = 0;

    if (!listlayer->menulist->CurItem)
    {
        return OK;
    }

    curitem     = listlayer->menulist->CurItem;
    curhitem    = listlayer->menulist->CurItem->Prev;
    curh_pos.x  = listlayer->foucs_pos.x;
    curh_pos.y  = listlayer->foucs_pos.y - listlayer->menulist->CurItem->rect.height;
    curtitem    = listlayer->menulist->CurItem->Next;
    curt_pos.x  = listlayer->foucs_pos.x;
    curt_pos.y  = listlayer->foucs_pos.y + listlayer->menulist->CurItem->rect.height;
    listlayer->plistmenu->status |= LISTMENU_ONLOAD;
    esKRNL_TimeDly(3);

    if (!(listlayer->plistmenu->status & LISTMENU_ONLOAD))
    {
        listlayer->plistmenu->status &= (~LISTMENU_ONLOAD);
        return OK;
    }

startload:

    do
    {
        esKRNL_TimeDly(3);

        if (!(listlayer->plistmenu->status & LISTMENU_ONLOAD))
        {
            break;
        }

        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            break;
        }

        if (curitem->status != FOUCS_STS)
        {
            if (listlayer->focus_type == KEY_FOCUS)
            {
                ret = ListLayer_DrawFoucsMenuItem(listlayer, curitem, fbno,
                                                  listlayer->foucs_pos.x,
                                                  listlayer->foucs_pos.y);
            }
            else
            {
                ret = ListLayer_DrawActivedMenuItem(listlayer, curitem, fbno,
                                                    listlayer->foucs_pos.x,
                                                    listlayer->foucs_pos.y);
            }

            if (ret == LSTM_DRAWCB_ERROR)
            {
                goto startload;
            }

            listlayer->setfoucs = REAL_FOUCS;
        }

        if (curhitem != listlayer->menulist->load_head)
        {
            if (curh_pos.y < 0)
            {
                curh_pos.y = listlayer->listbuffer->vrect.height + curh_pos.y;
            }

            /*在起始屏位置加载*/
            if (curh_pos.y < listlayer->viewrect.height)
            {
                if (curhitem->status != UNFOUCS_STS && curhitem->status != LOAD_STS)
                {
                    ret = ListLayer_LoadMenuItem(listlayer, curhitem, fbno,
                                                 curh_pos.x, curh_pos.y);

                    if (ret == LSTM_DRAWCB_ERROR)
                    {
                        goto startload;
                    }

                    ListLayer_MenuItemCopy(listlayer, fbno, fbno,
                                           curh_pos.x, curh_pos.y, curh_pos.x,
                                           curh_pos.y + listlayer->listbuffer->vrect.height);
                }
            }
            /*在结束屏位置加载*/
            else if (curh_pos.y >= listlayer->listbuffer->vrect.height)
            {
                if (curhitem->status != UNFOUCS_STS && curhitem->status != LOAD_STS)
                {
                    ret = ListLayer_LoadMenuItem(listlayer, curhitem, fbno,
                                                 curh_pos.x, curh_pos.y);

                    if (ret == LSTM_DRAWCB_ERROR)
                    {
                        goto startload;
                    }

                    ListLayer_MenuItemCopy(listlayer, fbno, fbno, curh_pos.x,
                                           curh_pos.y, curh_pos.x, curh_pos.y -
                                           listlayer->listbuffer->vrect.height);
                }
            }
            else
            {
                if (curhitem->status != UNFOUCS_STS && curhitem->status != LOAD_STS)
                {
                    ret = ListLayer_LoadMenuItem(listlayer, curhitem, fbno,
                                                 curh_pos.x, curh_pos.y);

                    if (ret == LSTM_DRAWCB_ERROR)
                    {
                        goto startload;
                    }
                }
            }

            curhitem = curhitem->Prev;
            curh_pos.x  = curh_pos.x;
            curh_pos.y  = curh_pos.y - curtitem->rect.height;

            if (curh_pos.y < 0)
            {
                curh_pos.y = listlayer->listbuffer->vrect.height + curh_pos.y;
            }
        }

        esKRNL_TimeDly(3);

        if (!(listlayer->plistmenu->status & LISTMENU_ONLOAD))
        {
            break;
        }

        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            break;
        }

        listlayer->plistmenu->status |= LISTMENU_ONLOAD;

        if (curtitem != listlayer->menulist->load_tail)
        {
            if (curt_pos.y >= listlayer->listbuffer->rectsz.height)
            {
                curt_pos.y = curt_pos.y - listlayer->listbuffer->vrect.height;
            }

            /*在起始屏位置加载*/
            if (curt_pos.y < listlayer->viewrect.height)
            {
                if (curtitem->status != UNFOUCS_STS && curtitem->status != LOAD_STS)
                {
                    ret = ListLayer_LoadMenuItem(listlayer, curtitem, fbno,
                                                 curt_pos.x, curt_pos.y);

                    if (ret == LSTM_DRAWCB_ERROR)
                    {
                        goto startload;
                    }

                    ListLayer_MenuItemCopy(listlayer, fbno, fbno,
                                           curt_pos.x, curt_pos.y, curt_pos.x,
                                           curt_pos.y + listlayer->listbuffer->vrect.height);
                }
            }
            /*在结束屏位置加载*/
            else if (curt_pos.y >= listlayer->listbuffer->vrect.height)
            {
                if (curtitem->status != UNFOUCS_STS && curtitem->status != LOAD_STS)
                {
                    ret = ListLayer_LoadMenuItem(listlayer, curtitem, fbno,
                                                 curt_pos.x, curt_pos.y);

                    if (ret == LSTM_DRAWCB_ERROR)
                    {
                        goto startload;
                    }

                    ListLayer_MenuItemCopy(listlayer, fbno, fbno, curt_pos.x,
                                           curt_pos.y, curt_pos.x, curt_pos.y -
                                           listlayer->listbuffer->vrect.height);
                }
            }
            else
            {
                if (curtitem->status != UNFOUCS_STS && curtitem->status != LOAD_STS)
                {
                    ret = ListLayer_LoadMenuItem(listlayer, curtitem, fbno,
                                                 curt_pos.x, curt_pos.y);

                    if (ret == LSTM_DRAWCB_ERROR)
                    {
                        goto startload;
                    }
                }
            }

            curtitem = curtitem->Next;
            curt_pos.x  = curt_pos.x;
            curt_pos.y  = curt_pos.y + curtitem->rect.height;

            if (curt_pos.y >= listlayer->listbuffer->rectsz.height)
            {
                curt_pos.y = curt_pos.y - listlayer->listbuffer->vrect.height;
            }
        }
    } while ((curhitem != listlayer->menulist->load_head)  ||

             (curtitem != listlayer->menulist->load_tail));

    listlayer->plistmenu->status &= (~LISTMENU_ONLOAD);
    return OK;
}


/*
**********************************************************************************************************************
*                                               ListLayer_GetBufferStatus
*
* author:
*
* date:             2008-10-8
*
* Description:      获取加载的状态信息
*
* parameters:       listlayer 当前使用的listlayer信息
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListLayer_GetBufferStatus(LIST_LAYER_MAN_t    *listlayer)
{
    __s32                      logic_tail = 0;
    __s32                      logic_head = 0;
    __s32                      logic_pos = 0;
    __s32                      head_dist = 0;
    __s32                      tail_dist = 0;
    __s32                      tmp_dist = 0;
    __s32                      tmp_dist1 = 0;
    __u8                       err;
    __u32                      status = 0;

    /*当前listmenu的模式为固定数量的模式*/
    if (listlayer->mode == FIXED_MODE || listlayer->menulist->ItemCount == 0)
    {
        status = 0;
        return status;
    }

    /*当前listmenu的模式为可变数量的模式*/
    esKRNL_SemPend(listlayer->pos_sem, 0, &err); /*pend pos semphore*/

    if (err == OS_NO_ERR)
    {
        esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/

        if (listlayer->viewrect.y > listlayer->tail_pos.y)
        {
            /*设置逻辑尾*/
            logic_tail = listlayer->tail_pos.y + listlayer->listbuffer->vrect.height;
        }
        else
        {
            /*设置逻辑尾*/
            logic_tail = listlayer->tail_pos.y;
        }

        if (listlayer->viewrect.y < listlayer->head_pos.y)
        {
            /*设置逻辑头*/
            logic_head = listlayer->head_pos.y - listlayer->listbuffer->vrect.height;
        }
        else
        {
            /*设置逻辑头*/
            logic_head = listlayer->head_pos.y;
        }

        logic_pos = listlayer->viewrect.y;
        /*获取头位移*/
        head_dist = (logic_pos - logic_head) -
                    (logic_pos - logic_head) % (listlayer->menulist->head->rect.height);

        /*非对齐的情况*/
        if ((logic_tail - logic_pos) % (listlayer->menulist->head->rect.height))
        {
            /*获取尾位移*/
            tail_dist = (logic_tail - logic_pos) -
                        (logic_tail - logic_pos) % (listlayer->menulist->head->rect.height) + listlayer->menulist->head->rect.height;
        }
        else
        {
            /*获取尾位移*/
            tail_dist = (logic_tail - logic_pos) -
                        (logic_tail - logic_pos) % (listlayer->menulist->head->rect.height);
        }

        if ((listlayer->menulist->load_tail == listlayer->menulist->head) &&
            (listlayer->filldown == listlayer->downcount))
        {
            status |= DOWNOVER;
        }

        if ((listlayer->menulist->load_head == listlayer->menulist->tail) &&
            (listlayer->fillup == listlayer->upcount))
        {
            status |= UPOVER;
        }

        if (listlayer->scnnum != 1)
        {
            if (head_dist <= listlayer->menulist->head->rect.height)
            {
                status = status | WAITUP;

                if (status & DOWNOVER)
                {
                    status &= (~DOWNOVER);
                }

                if (status & UPOVER)
                {
                    status = 0;
                }

                esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
                esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/
                return status;
            }
        }
        else
        {
            if (head_dist < listlayer->menulist->head->rect.height)
            {
                status = status | WAITUP;

                if (status & DOWNOVER)
                {
                    status &= (~DOWNOVER);
                }

                if (status & UPOVER)
                {
                    status = 0;
                }

                esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
                esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/
                return status;
            }
        }

        if (listlayer->scnnum != 1)
        {
            if (tail_dist <= (listlayer->viewrect.height + listlayer->menulist->head->rect.height))
            {
                status = status | WAITDOWN;

                if (status & DOWNOVER)
                {
                    status = 0;
                }

                if (status & UPOVER)
                {
                    status &= (~UPOVER);
                }

                esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
                esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/
                return status;
            }
        }
        else
        {
            if (tail_dist < (listlayer->viewrect.height + listlayer->menulist->head->rect.height))
            {
                status = status | WAITDOWN;

                if (status & DOWNOVER)
                {
                    status = 0;
                }

                if (status & UPOVER)
                {
                    status &= (~UPOVER);
                }

                esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
                esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/
                return status;
            }
        }

        /*需要预取头item*/
        if (head_dist < (listlayer->fb_depth / 2 - 1) * (listlayer->viewrect.height))
        {
            if (status & UPOVER)
            {
                status = 0;
            }
            else if (listlayer->menulist->load_head == listlayer->menulist->tail)
            {
                /**/
                if (listlayer->upcount != listlayer->fillup)
                {
                    tmp_dist = (__u32)((listlayer->upcount - listlayer->fillup) *
                                       listlayer->menulist->head->rect.height);
                    tmp_dist1 = (__u32)((listlayer->fb_depth / 2 - 1) *
                                        (listlayer->viewrect.height));

                    if (tmp_dist <= tmp_dist1)
                    {
                        status |= ((tmp_dist) << 16);
                    }
                    else
                    {
                        status |= ((tmp_dist1) << 16);
                    }

                    status |= UPWARDS;
                }
            }
            else
            {
                status |= ((__u32)((listlayer->fb_depth / 2 - 1) *
                                   (listlayer->viewrect.height) - head_dist) << 16);
                status |= UPWARDS;
            }
        }

        if (tail_dist < ((listlayer->viewrect.height) * (listlayer->fb_depth / 2)))
        {
            if (status & DOWNOVER)
            {
                status = 0;
            }
            else if (listlayer->menulist->load_tail == listlayer->menulist->head)
            {
                if (listlayer->downcount != listlayer->filldown)
                {
                    tmp_dist = (__u32)((listlayer->downcount - listlayer->filldown) *
                                       (listlayer->menulist->head->rect.height));
                    tmp_dist1 = (__u32)((listlayer->viewrect.height) *
                                        (listlayer->fb_depth / 2)) - tail_dist;

                    if (tmp_dist <= tmp_dist1)
                    {
                        status |= ((tmp_dist) << 16);
                    }
                    else
                    {
                        status |= ((tmp_dist1) << 16);
                    }

                    status |= DOWNWARDS;
                }
            }
            else
            {
                status |= ((__u32)(((listlayer->viewrect.height) *
                                    (listlayer->fb_depth / 2)) - tail_dist) << 16);
                status |= DOWNWARDS;
            }
        }

        esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
    }

    esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/

    if (status & DOWNOVER)
    {
        status &= (~DOWNOVER);
    }

    if (status & UPOVER)
    {
        status &= (~UPOVER);
    }

    return status;
}

/*
**********************************************************************************************************************
*                                               ListLayer_LoadItemThread
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer loaditemthread
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void ListLayer_LoadItemThread(void *p_arg)
{
    __u8                       err;
    __u8                       num;
    __u8                       count;
    __bool                     lock_flags = EPDK_FALSE;
    __u32                      distance;
    __u32                      status;
    __s32                      dist_tmp;
    __krnl_flags_t             flags;
    __krnl_tcb_t               cur_tcb;
    LIST_LAYER_MAN_t          *listlayer = (LIST_LAYER_MAN_t *)NULL;
    listlayer = (LIST_LAYER_MAN_t *)p_arg;
    err = esKRNL_TaskQuery(OS_PRIO_SELF, &cur_tcb);

    if (err == OS_NO_ERR)
    {
        listlayer->LoadPrio = cur_tcb.OSTCBPrio;
    }

    esKRNL_FlagPend(listlayer->load_flags, LISTLAYER_LOAD_FLAG,
                    ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);

    while (1)
    {
        /*get the listbuffer status*/
        status = ListLayer_GetBufferStatus(listlayer);

        /*need not to load item*/
        if (status == 0)
        {
            if (!(listlayer->status & WAIT_LOAD))
            {
                /*suspend this thread*/
                esKRNL_FlagPend(listlayer->load_flags, LISTLAYER_LOAD_FLAG + LISTLAYER_WAIT_LOAD_FLAG,
                                ((OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME) << 16) | 0, &err);

                if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
                {
                    esKRNL_TDel(EXEC_prioself);
                    return;
                }
            }
        }
        else
        {
            /*拿到锁的标志位*/
            lock_flags = EPDK_TRUE;

            while (GUI_LockAccept() == EPDK_FAIL)
            {
                if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
                {
                    esKRNL_TDel(EXEC_prioself);
                    return;
                }

                esKRNL_TimeDly(1);
                /*post run thread*/
                flags = esKRNL_FlagQuery(listlayer->wait_flags, &err);

                if (!(flags & LISTLAYER_WAIT_FLAG))
                {
                    status = ListLayer_GetBufferStatus(listlayer);

                    if (status == 0)
                    {
                        /*get framebuffer status*/
                        esKRNL_FlagPost(listlayer->wait_flags, LISTLAYER_WAIT_FLAG, OS_FLAG_SET, &err);
                        lock_flags = EPDK_FALSE;
                        break;
                    }
                }
            }

            /*get lock and get listlayer status*/
            if (lock_flags == EPDK_TRUE)
            {
                status = ListLayer_GetBufferStatus(listlayer);
            }

            /*if main thread wait up*/
            if (status & WAITUP)
            {
                count = listlayer->scnnum;

                for (num = 0; num < count; num++)
                {
                    if ((listlayer->menulist->load_head != listlayer->menulist->tail) ||
                        ((listlayer->menulist->load_head == listlayer->menulist->tail) &&
                         (listlayer->fillup < listlayer->upcount)))
                    {
                        if (listlayer->menulist->load_head != listlayer->menulist->tail)
                        {
                            ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->load_head,
                                                       0, listlayer->head_pos.x, listlayer->head_pos.y);
                        }
                        else
                        {
                            ListLayer_ClearMenuItem(listlayer, 0, listlayer->head_pos.x,
                                                    listlayer->head_pos.y);
                        }

                        dist_tmp = listlayer->head_pos.y - listlayer->menulist->load_head->rect.height;

                        if (dist_tmp < 0)
                        {
                            ListLayer_HeadToTailCopy(listlayer);
                            esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/
                            listlayer->head_pos.y = listlayer->listbuffer->vrect.height -
                                                    listlayer->menulist->head->rect.height;
                        }
                        else
                        {
                            esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/
                            listlayer->head_pos.y = listlayer->head_pos.y -
                                                    listlayer->menulist->load_head->rect.height;
                        }

                        /*如果需要加载的item结束，则填充空白item*/
                        if (listlayer->menulist->load_head == listlayer->menulist->tail)
                        {
                            listlayer->fillup++;
                        }
                        else
                        {
                            /*需要加载的item为上一个item*/
                            listlayer->menulist->load_head = listlayer->menulist->load_head->Prev;
                        }

                        /*如果需要加载的起始位置小于结束位置(此时由于加载头item而改写为item加载序列)*/
                        if (listlayer->head_pos.y < listlayer->tail_pos.y)
                        {
                            /*如果加载位置在起始屏位置之外,则直接更新结束屏位置*/
                            if (listlayer->head_pos.y >= listlayer->viewrect.height)
                            {
                                listlayer->tail_pos.y = listlayer->head_pos.y;
                            }/*加载开始位置在起始屏之内，加载尾为结束屏相应位置*/
                            else if (listlayer->head_pos.y == listlayer->viewrect.height -
                                     listlayer->menulist->head->rect.height)
                            {
                                listlayer->tail_pos.y = listlayer->listbuffer->rectsz.height -
                                                        listlayer->menulist->head->rect.height;
                            }
                            else if ((listlayer->head_pos.y < listlayer->viewrect.height) &&
                                     (listlayer->tail_pos.y > listlayer->listbuffer->vrect.height))
                            {
                                /*更新相应的变化值*/
                                listlayer->tail_pos.y = listlayer->tail_pos.y -
                                                        listlayer->menulist->load_tail->rect.height;
                            }

                            if (listlayer->menulist->load_tail != listlayer->menulist->head)
                            {
                                /*更新加载为指针*/
                                listlayer->menulist->load_tail = listlayer->menulist->load_tail->Prev;
                            }
                            else
                            {
                                if (listlayer->filldown > 0)
                                {
                                    listlayer->filldown--;
                                }
                                else
                                {
                                    listlayer->menulist->load_tail = listlayer->menulist->load_tail->Prev;
                                }
                            }
                        }

                        esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
                    }
                }

                status = (status & (~WAITUP)) & (~UPWARDS);
                flags = esKRNL_FlagQuery(listlayer->wait_flags, &err);

                if (!(flags & LISTLAYER_WAIT_FLAG))
                {
                    status = ListLayer_GetBufferStatus(listlayer);

                    if (status == 0)
                    {
                        /*get framebuffer status*/
                        esKRNL_FlagPost(listlayer->wait_flags, LISTLAYER_WAIT_FLAG, OS_FLAG_SET, &err);
                    }
                }

                if (lock_flags == EPDK_TRUE)
                {
                    GUI_UNLOCK();
                }
            }
            else if (status & WAITDOWN)
            {
                count = listlayer->scnnum;

                for (num = 0; num < count; num++)
                {
                    if ((listlayer->menulist->load_tail != listlayer->menulist->head) ||
                        ((listlayer->menulist->load_tail == listlayer->menulist->head)
                         && (listlayer->filldown < listlayer->downcount)))
                    {
                        if (listlayer->menulist->load_tail != listlayer->menulist->head)
                        {
                            ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->load_tail,
                                                       0, listlayer->tail_pos.x, listlayer->tail_pos.y);
                        }
                        else
                        {
                            ListLayer_ClearMenuItem(listlayer, 0, listlayer->tail_pos.x,
                                                    listlayer->tail_pos.y);
                        }

                        //esKRNL_SchedLock();
                        dist_tmp = listlayer->tail_pos.y + listlayer->menulist->load_tail->rect.height;

                        if (dist_tmp >= listlayer->listbuffer->rectsz.height)
                        {
                            ListLayer_TailToHeadCopy(listlayer);
                            esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/
                            listlayer->tail_pos.y = listlayer->viewrect.height;
                        }
                        else
                        {
                            esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/
                            listlayer->tail_pos.y = listlayer->tail_pos.y + listlayer->menulist->load_tail->rect.height;
                        }

                        if (listlayer->menulist->load_tail == listlayer->menulist->head)
                        {
                            listlayer->filldown++;
                        }

                        if (listlayer->menulist->load_tail != listlayer->menulist->head)
                        {
                            listlayer->menulist->load_tail = listlayer->menulist->load_tail->Next;
                        }

                        if (listlayer->tail_pos.y > listlayer->head_pos.y)
                        {
                            if (listlayer->tail_pos.y < listlayer->listbuffer->vrect.height)
                            {
                                listlayer->head_pos.y = listlayer->tail_pos.y;
                            }
                            else if (listlayer->tail_pos.y == listlayer->listbuffer->vrect.height)
                            {
                                listlayer->head_pos.y = 0;
                            }
                            else if ((listlayer->tail_pos.y > listlayer->listbuffer->vrect.height)
                                     && (listlayer->head_pos.y < listlayer->viewrect.height))
                            {
                                listlayer->head_pos.y = listlayer->head_pos.y +
                                                        listlayer->menulist->load_head->rect.height;
                            }

                            if (listlayer->menulist->load_head != listlayer->menulist->tail)
                            {
                                listlayer->menulist->load_head = listlayer->menulist->load_head->Next;
                            }
                            else
                            {
                                if (listlayer->fillup >= 0)
                                {
                                    listlayer->fillup--;
                                }

                                if (listlayer->fillup < 0)
                                {
                                    listlayer->menulist->load_head = listlayer->menulist->load_head->Next;
                                    listlayer->fillup = 0;
                                }
                            }
                        }

                        esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
                        //esKRNL_SchedUnlock();
                    }
                }

                status = (status & (~WAITDOWN)) & (~DOWNWARDS);
                flags = esKRNL_FlagQuery(listlayer->wait_flags, &err);

                if (!(flags & LISTLAYER_WAIT_FLAG))
                {
                    status = ListLayer_GetBufferStatus(listlayer);

                    if (status == 0)
                    {
                        esKRNL_FlagPost(listlayer->wait_flags, LISTLAYER_WAIT_FLAG, OS_FLAG_SET, &err);
                    }
                }

                if (lock_flags == EPDK_TRUE)
                {
                    GUI_UNLOCK();
                }
            }
            else if (status & UPWARDS)
            {
                distance = status >> 16;
                count = distance / (listlayer->menulist->head->rect.height);

                for (num = 0; num < count; num++)
                {
                    if ((listlayer->menulist->load_head != listlayer->menulist->tail)
                        || ((listlayer->menulist->load_head == listlayer->menulist->tail)
                            && (listlayer->fillup < listlayer->upcount)))
                    {
                        if (listlayer->menulist->load_head != listlayer->menulist->tail)
                        {
                            ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->load_head,
                                                       0, listlayer->head_pos.x,
                                                       listlayer->head_pos.y);
                        }
                        else
                        {
                            ListLayer_ClearMenuItem(listlayer, 0, listlayer->head_pos.x,
                                                    listlayer->head_pos.y);
                        }

                        //esKRNL_SchedLock();
                        dist_tmp = listlayer->head_pos.y - listlayer->menulist->load_head->rect.height;

                        if (dist_tmp < 0)
                        {
                            ListLayer_HeadToTailCopy(listlayer);
                            esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/
                            listlayer->head_pos.y = listlayer->listbuffer->vrect.height -
                                                    listlayer->menulist->head->rect.height;
                        }
                        else
                        {
                            esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/
                            listlayer->head_pos.y = listlayer->head_pos.y -
                                                    listlayer->menulist->load_head->rect.height;
                        }

                        if (listlayer->menulist->load_head == listlayer->menulist->tail)
                        {
                            listlayer->fillup++;
                        }
                        else
                        {
                            listlayer->menulist->load_head = listlayer->menulist->load_head->Prev;
                        }

                        if (listlayer->head_pos.y < listlayer->tail_pos.y)
                        {
                            if (listlayer->head_pos.y >= listlayer->viewrect.height)
                            {
                                listlayer->tail_pos.y = listlayer->head_pos.y;
                            }
                            else if (listlayer->head_pos.y == listlayer->viewrect.height -
                                     listlayer->menulist->head->rect.height)
                            {
                                listlayer->tail_pos.y = listlayer->listbuffer->rectsz.height -
                                                        listlayer->menulist->head->rect.height;
                            }
                            else if ((listlayer->head_pos.y < listlayer->viewrect.height)
                                     && (listlayer->tail_pos.y > listlayer->listbuffer->vrect.height))
                            {
                                listlayer->tail_pos.y = listlayer->tail_pos.y -
                                                        listlayer->menulist->load_tail->rect.height;
                            }

                            if (listlayer->menulist->load_tail != listlayer->menulist->head)
                            {
                                listlayer->menulist->load_tail = listlayer->menulist->load_tail->Prev;
                            }
                            else
                            {
                                if (listlayer->filldown > 0)
                                {
                                    listlayer->filldown--;
                                }
                                else
                                {
                                    listlayer->menulist->load_tail = listlayer->menulist->load_tail->Prev;
                                }
                            }
                        }

                        esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
                        //esKRNL_SchedUnlock();
                    }
                }

                status = status & (~UPWARDS);
                flags = esKRNL_FlagQuery(listlayer->wait_flags, &err);

                if (!(flags & LISTLAYER_WAIT_FLAG))
                {
                    status = ListLayer_GetBufferStatus(listlayer);

                    if (status == 0)
                    {
                        esKRNL_FlagPost(listlayer->wait_flags, LISTLAYER_WAIT_FLAG, OS_FLAG_SET, &err);
                    }
                }

                if (lock_flags == EPDK_TRUE)
                {
                    GUI_UNLOCK();
                }
            }
            else if (status & DOWNWARDS)
            {
                distance = status >> 16;
                count = distance / (listlayer->menulist->head->rect.height);

                for (num = 0; num < count; num++)
                {
                    if ((listlayer->menulist->load_tail != listlayer->menulist->head) ||
                        ((listlayer->menulist->load_tail == listlayer->menulist->head) &&
                         (listlayer->filldown < listlayer->downcount)))
                    {
                        if (listlayer->menulist->load_tail != listlayer->menulist->head)
                        {
                            ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->load_tail, 0,
                                                       listlayer->tail_pos.x, listlayer->tail_pos.y);
                        }
                        else
                        {
                            ListLayer_ClearMenuItem(listlayer, 0, listlayer->tail_pos.x,
                                                    listlayer->tail_pos.y);
                        }

                        //esKRNL_SchedLock();
                        dist_tmp = listlayer->tail_pos.y + listlayer->menulist->load_tail->rect.height;

                        if (dist_tmp >= listlayer->listbuffer->rectsz.height)
                        {
                            ListLayer_TailToHeadCopy(listlayer);
                            esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/
                            listlayer->tail_pos.y = listlayer->viewrect.height;
                        }
                        else
                        {
                            esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/
                            listlayer->tail_pos.y = listlayer->tail_pos.y + listlayer->menulist->load_tail->rect.height;
                        }

                        if (listlayer->menulist->load_tail == listlayer->menulist->head)
                        {
                            listlayer->filldown++;
                        }

                        if (listlayer->menulist->load_tail != listlayer->menulist->head)
                        {
                            listlayer->menulist->load_tail = listlayer->menulist->load_tail->Next;
                        }

                        if (listlayer->tail_pos.y > listlayer->head_pos.y)
                        {
                            if (listlayer->tail_pos.y < listlayer->listbuffer->vrect.height)
                            {
                                listlayer->head_pos.y = listlayer->tail_pos.y;
                            }
                            else if (listlayer->tail_pos.y == listlayer->listbuffer->vrect.height)
                            {
                                listlayer->head_pos.y = 0;
                            }
                            else if ((listlayer->tail_pos.y > listlayer->listbuffer->vrect.height)
                                     && (listlayer->head_pos.y < listlayer->viewrect.height))
                            {
                                listlayer->head_pos.y = listlayer->head_pos.y +
                                                        listlayer->menulist->load_head->rect.height;
                            }

                            if (listlayer->menulist->load_head != listlayer->menulist->tail)
                            {
                                listlayer->menulist->load_head = listlayer->menulist->load_head->Next;
                            }
                            else
                            {
                                if (listlayer->fillup >= 0)
                                {
                                    listlayer->fillup--;
                                }

                                if (listlayer->fillup < 0)
                                {
                                    listlayer->menulist->load_head = listlayer->menulist->load_head->Next;
                                    listlayer->fillup = 0;
                                }
                            }
                        }

                        esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
                        //esKRNL_SchedUnlock();
                    }
                }

                status = status & (~DOWNWARDS);
                flags = esKRNL_FlagQuery(listlayer->wait_flags, &err);

                if (!(flags & LISTLAYER_WAIT_FLAG))
                {
                    status = ListLayer_GetBufferStatus(listlayer);

                    if (status == 0)
                    {
                        esKRNL_FlagPost(listlayer->wait_flags, LISTLAYER_WAIT_FLAG, OS_FLAG_SET, &err);
                    }
                }

                if (lock_flags == EPDK_TRUE)
                {
                    GUI_UNLOCK();
                }
            }
            else
            {
                if (lock_flags == EPDK_TRUE)
                {
                    GUI_UNLOCK();
                }
            }

            POS_PRINTF("thread phy pos = %x\n", listlayer->viewrect.y);
            POS_PRINTF("thread phy head = %x\n", listlayer->head_pos.y);
            // LISTMENU_PRINTF("thread phy tail = %x\n",listlayer->tail_pos.y);
        }
    }
}


/*
**********************************************************************************************************************
*                                               ListLayer_FoucsItemThread
*
* author:
*
* date:             2008-10-8
*
* Description:      listlayer foucsitemthread
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void ListLayer_FoucsItemThread(void *p_arg)
{
    __u8                        err;
    __krnl_tcb_t                cur_tcb;
    LIST_LAYER_MAN_t           *listlayer = (LIST_LAYER_MAN_t *)NULL;
    OS_SEM_DATA                 sem_data;
    __krnl_flags_t              flags;
    __u8                        del_flags = 0;

    if (p_arg == NULL)
    {
        return ;
    }

    listlayer = (LIST_LAYER_MAN_t *)p_arg;
    err = esKRNL_TaskQuery(OS_PRIO_SELF, &cur_tcb);

    if (err == OS_NO_ERR)
    {
        listlayer->FoucsPrio = cur_tcb.OSTCBPrio;
    }

    while (1)
    {
        flags = esKRNL_FlagPend(listlayer->foucs_flags, LISTLAYER_FOUCS_FLAG + LISTLAYER_DRAW_ITEM_FLAG,
                        ((OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME) << 16) | 0, &err);

        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            esKRNL_TDel(EXEC_prioself);
            return ;
        }

        //flags = esKRNL_FlagQuery(listlayer->foucs_flags, &err);

        if (flags & LISTLAYER_FOUCS_FLAG)
        {
            /*设焦处理*/
            while (GUI_LockAccept() == EPDK_FAIL)
            {
                esKRNL_TimeDly(1);

                if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
                {
                    esKRNL_FlagPost(listlayer->foucs_flags, LISTLAYER_FOUCS_FINISH_FLAG, OS_FLAG_SET, &err);
                    //listlayer->foucs_flags->OSFlagFlags &= (~LISTLAYER_FOUCS_FLAG);
                    del_flags = 1;
                    break ;
                }
            }

            if (del_flags == 1)
            {
                continue ;
            }

            ListLayer_SetLayerUnFoucs(listlayer);
            ListLayer_SetLayerFastFoucs(listlayer, listlayer->menulist->CurItem);

            if (!(listlayer->plistmenu->mode & KEYTONE_OFF))
            {
                _lstm_key_song();
            }

            listlayer->setfoucs = VAR_FOUCS;
            GUI_UNLOCK();
            esKRNL_FlagPost(listlayer->foucs_flags, LISTLAYER_FOUCS_FINISH_FLAG, OS_FLAG_SET, &err);
            //listlayer->foucs_flags->OSFlagFlags &= (~LISTLAYER_FOUCS_FLAG);
        }
        if (flags & LISTLAYER_DRAW_ITEM_FLAG)
        {
            /*扩散加载处理*/
            if (listlayer->mode != FIXED_MODE)
            {
                err = esKRNL_SemQuery(listlayer->plistmenu->load_sem, &sem_data);

                if (sem_data.OSCnt == 0)
                {
                    esKRNL_SemPost(listlayer->plistmenu->load_sem); /*post pos semphore*/
                }

                esKRNL_SemPend(listlayer->plistmenu->load_sem, 0, &err); /*pend pos semphore*/
                ListLayer_DrawItemProcess(listlayer);
                esKRNL_SemPost(listlayer->plistmenu->load_sem); /*pend pos semphore*/
            }

            //listlayer->foucs_flags->OSFlagFlags &= (~LISTLAYER_DRAW_ITEM_FLAG);
        }
    }
}

/*
**********************************************************************************************************************
*                                               ListLayer_VarKeyUpMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer varkeyupmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_VarKeyUpMsg(LIST_LAYER_MAN_t *listlayer, __u32 dist, __u32  step)
{
    __u8                       err;
    __u8                       fbno;
    __u8                       i;
    __u32                      ret;
    __u32                      status;
    __rect_t                   sw;
    //__u32                      condition;
    __move_para_t              dist_dly;
    __krnl_flags_t             flags;
    fbno = 0;
    status = ListLayer_GetBufferStatus(listlayer);

    if (status & WAITUP)
    {
        __inf("wait to load! status = %d\n", status);
        listlayer->status |= WAIT_LOAD;
        esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_WAIT_LOAD_FLAG, OS_FLAG_SET, &err);
        listlayer->wait_flags->OSFlagFlags &= (~LISTLAYER_WAIT_FLAG);
        esKRNL_FlagPend(listlayer->wait_flags, LISTLAYER_WAIT_FLAG,
                        ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);
        listlayer->status &= (~WAIT_LOAD);
    }

    if (listlayer->menulist->CurItem != listlayer->menulist->head)
    {
        /*set listlayer unfoucs pos*/
        listlayer->unfoucs_pos.x = listlayer->foucs_pos.x;
        listlayer->unfoucs_pos.y = listlayer->foucs_pos.y + listlayer->menulist->CurItem->foucs_w -
                                   listlayer->menulist->CurItem->rect.height;
        listlayer->menulist->UnCurItem = listlayer->menulist->CurItem;

        //condition = listlayer->msgupcount;
        if (listlayer->viewrect.y < listlayer->menulist->CurItem->rect.height)
        {
            /*current foucs pos locate blow the start sceen */
            listlayer->foucs_pos.y = listlayer->foucs_pos.y -
                                     listlayer->menulist->CurItem->rect.height +
                                     listlayer->listbuffer->vrect.height;
            listlayer->foucs_pos.x = listlayer->foucs_pos.x;
        }
        else
        {
            listlayer->foucs_pos.y = listlayer->foucs_pos.y - listlayer->menulist->CurItem->rect.height;
            listlayer->foucs_pos.x = listlayer->foucs_pos.x;
        }

        LISTMENU_PRINTF("listlayer->foucs.y = %d\n", listlayer->foucs_pos.y);
        sw.x = listlayer->viewrect.x;
        sw.y = listlayer->viewrect.y;
        listlayer->menulist->CurItem = listlayer->menulist->CurItem->Prev;
        listlayer->menulist->SelItem = listlayer->menulist->SelItem->Prev;
        esKRNL_FlagPost(listlayer->foucs_flags, LISTLAYER_FOUCS_FLAG, OS_FLAG_SET, &err);
        listlayer->cursteps = listlayer->nextsteps;

        for (i = 0; i < listlayer->cursteps; i++)
        {
            ret = InterMove(0, listlayer->menulist->CurItem->rect.height, listlayer->cursteps, i, &dist_dly);

            if (ret != OK)
            {
                return __LINE__;
            }

            sw.y = sw.y - dist_dly.dist;

            if (sw.y < 0)
            {
                sw.y = listlayer->listbuffer->vrect.height + sw.y;
            }
            else if (sw.y > listlayer->listbuffer->vrect.height)
            {
                sw.y = sw.y - listlayer->listbuffer->vrect.height;
            }

            if (listlayer->plistmenu->slave_lyr == 0)
            {
                ret = ListLayer_SwitchFB(listlayer, fbno, sw.x, sw.y);

                if (ret != EPDK_OK)
                {
                    __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
                    return __LINE__;
                }
            }
            else
            {
                GUI_LyrWinCacheOn();
                ret = ListLayer_SwitchFB(listlayer, fbno, sw.x, sw.y);

                if (ret != EPDK_OK)
                {
                    __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
                    return __LINE__;
                }

                if (i == listlayer->cursteps - 1)
                {
                    ret = GUI_LyrWinSetScnWindow(listlayer->plistmenu->slave_lyr, &listlayer->plistmenu->slyr_rect);

                    if (ret != EPDK_OK)
                    {
                        __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
                        return __LINE__;
                    }

                    listlayer->plistmenu->slyr_crect.y = listlayer->plistmenu->slyr_rect.y;
                }
                else
                {
                    listlayer->plistmenu->slyr_crect.y = listlayer->plistmenu->slyr_rect.y + dist_dly.dist;
                    ret = GUI_LyrWinSetScnWindow(listlayer->plistmenu->slave_lyr, &listlayer->plistmenu->slyr_crect);

                    if (ret != EPDK_OK)
                    {
                        __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
                        return __LINE__;
                    }
                }

                GUI_LyrWinCacheOff();
            }

            esKRNL_TimeDly(dist_dly.dly_time);
        }

        esKRNL_SemPend(listlayer->pos_sem, 0, &err); /*pend pos semphore*/

        if (err == OS_NO_ERR)
        {
            listlayer->viewrect.x = sw.x;
            listlayer->viewrect.y = sw.y;
        }

        esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/
        esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_LOAD_FLAG, OS_FLAG_SET, &err);
        flags = esKRNL_FlagQuery(listlayer->foucs_flags, &err);

        if (!(flags & LISTLAYER_FOUCS_FINISH_FLAG))
        {
            __inf("wait to foucs!\n");
            esKRNL_FlagPend(listlayer->foucs_flags, LISTLAYER_FOUCS_FINISH_FLAG,
                            ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);
        }

        listlayer->foucs_flags->OSFlagFlags &= (~LISTLAYER_FOUCS_FINISH_FLAG);

        if (listlayer->LoadPrio)
        {
            //esKRNL_SemPend(listlayer->draw_sem,0,&err); /*pend pos semphore*/
            esKRNL_TaskSuspend(listlayer->LoadPrio);
            esKRNL_TimeDly(2);
            esKRNL_TaskResume(listlayer->LoadPrio);
            //esKRNL_SemPost(listlayer->draw_sem); /*pend pos semphore*/
        }
        else
        {
            esKRNL_TimeDly(2);
        }

        /*判断是否有停止消息过来*/
        if (listlayer->msgcount != 0 &&
            listlayer->plistmenu->lastmsg == GUI_MSG_KEY_LONGUP)
        {
            /*无停止消息过来，继续运行*/
            GUI_LstmPostRunMsg(listlayer->plistmenu, LSTMM_RUNUP);
        }
    }
    else
    {
        if (listlayer->setfoucs != REAL_FOUCS)
        {
            ListLayer_SetLayerFoucs(listlayer, listlayer->menulist->CurItem);
            listlayer->setfoucs = REAL_FOUCS;
        }

        return OK;
    }

    //  LISTMENU_PRINTF("finish no = %d\n",listlayer->cur_fbno);
    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_VarKeyDownMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer varkeydownmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_VarKeyDownMsg(LIST_LAYER_MAN_t    *listlayer, __u32 dist, __u32 step)
{
    __u8                       err;
    __u8                       fbno;
    __u8                       i;
    __u32                      ret;
    //__u32                      condition;
    __u32                      status;
    __rect_t                   sw;
    __move_para_t              dist_dly;
    __krnl_flags_t             flags;
    fbno = 0;
    //__inf("listlayer->foucs.y = %d\n",listlayer->foucs_pos.y);
    status = ListLayer_GetBufferStatus(listlayer);

    if (status & WAITDOWN)
    {
        __inf("wait to load! status = %d\n", status);
        listlayer->status |= WAIT_LOAD;
        esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_WAIT_LOAD_FLAG, OS_FLAG_SET, &err);
        listlayer->wait_flags->OSFlagFlags &= (~LISTLAYER_WAIT_FLAG);
        esKRNL_FlagPend(listlayer->wait_flags, LISTLAYER_WAIT_FLAG,
                        ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);
        listlayer->status &= (~WAIT_LOAD);
    }

    if (listlayer->menulist->CurItem != listlayer->menulist->tail)
    {
        /*save prev foucs position*/
        listlayer->unfoucs_pos.x = listlayer->foucs_pos.x;
        listlayer->unfoucs_pos.y = listlayer->foucs_pos.y;
        listlayer->menulist->UnCurItem = listlayer->menulist->CurItem;
        //condition = listlayer->msgdowncount;

        if (listlayer->viewrect.y > listlayer->listbuffer->vrect.height - listlayer->menulist->CurItem->rect.height)
        {
            listlayer->foucs_pos.y = listlayer->foucs_pos.y +
                                     listlayer->menulist->CurItem->rect.height -
                                     listlayer->listbuffer->vrect.height;
            listlayer->foucs_pos.x = listlayer->foucs_pos.x;
        }
        else
        {
            listlayer->foucs_pos.y = listlayer->foucs_pos.y + listlayer->menulist->CurItem->rect.height;
            listlayer->foucs_pos.x = listlayer->foucs_pos.x;
        }

        sw.x = listlayer->viewrect.x;
        sw.y = listlayer->viewrect.y;
        listlayer->menulist->CurItem = listlayer->menulist->CurItem->Next;
        listlayer->menulist->SelItem = listlayer->menulist->SelItem->Next;
        esKRNL_FlagPost(listlayer->foucs_flags, LISTLAYER_FOUCS_FLAG, OS_FLAG_SET, &err); /*start foucs Item thread to foucs item*/
        listlayer->cursteps = listlayer->nextsteps;

        for (i = 0; i < listlayer->cursteps; i++)
        {
            ret = InterMove(0, listlayer->menulist->CurItem->rect.height, listlayer->cursteps, i, &dist_dly);

            if (ret != OK)
            {
                return __LINE__;
            }

            sw.y = sw.y + dist_dly.dist;

            if (sw.y > listlayer->listbuffer->vrect.height)
            {
                sw.y = sw.y - listlayer->listbuffer->vrect.height;
            }
            else if (sw.y < 0)
            {
                sw.y = sw.y + listlayer->listbuffer->vrect.height;
            }

            if (listlayer->plistmenu->slave_lyr == 0)
            {
                ret = ListLayer_SwitchFB(listlayer, fbno, sw.x, sw.y);

                if (ret != EPDK_OK)
                {
                    __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
                    return __LINE__;
                }
            }
            else
            {
                GUI_LyrWinCacheOn();
                ret = ListLayer_SwitchFB(listlayer, fbno, sw.x, sw.y);

                if (ret != EPDK_OK)
                {
                    __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
                    return __LINE__;
                }

                if (i == listlayer->cursteps - 1)
                {
                    ret = GUI_LyrWinSetScnWindow(listlayer->plistmenu->slave_lyr, &listlayer->plistmenu->slyr_rect);

                    if (ret != EPDK_OK)
                    {
                        __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
                        return __LINE__;
                    }

                    listlayer->plistmenu->slyr_crect.y = listlayer->plistmenu->slyr_rect.y;
                }
                else
                {
                    listlayer->plistmenu->slyr_crect.y = listlayer->plistmenu->slyr_rect.y - dist_dly.dist;
                    ret = GUI_LyrWinSetScnWindow(listlayer->plistmenu->slave_lyr, &listlayer->plistmenu->slyr_crect);

                    if (ret != EPDK_OK)
                    {
                        __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
                        return __LINE__;
                    }
                }

                GUI_LyrWinCacheOff();
            }

            esKRNL_TimeDly(dist_dly.dly_time);
        }

        esKRNL_SemPend(listlayer->pos_sem, 0, &err); /*pend pos semphore*/

        if (err == OS_NO_ERR)
        {
            listlayer->viewrect.x = sw.x;
            listlayer->viewrect.y = sw.y;
        }

        esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/
        esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_LOAD_FLAG, OS_FLAG_SET, &err);
        flags = esKRNL_FlagQuery(listlayer->foucs_flags, &err);

        if (!(flags & LISTLAYER_FOUCS_FINISH_FLAG))
        {
            __inf("wait to foucs!flags = %d\n", flags);
            esKRNL_FlagPend(listlayer->foucs_flags, LISTLAYER_FOUCS_FINISH_FLAG,
                            ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);
        }

        listlayer->foucs_flags->OSFlagFlags &= (~LISTLAYER_FOUCS_FINISH_FLAG);

        if (listlayer->LoadPrio)
        {
            esKRNL_TaskSuspend(listlayer->LoadPrio);
            esKRNL_TimeDly(2);
            esKRNL_TaskResume(listlayer->LoadPrio);
        }
        else
        {
            esKRNL_TimeDly(2);
        }

        /*判断是否有停止消息过来*/
        if (listlayer->msgcount != 0 &&
            listlayer->plistmenu->lastmsg == GUI_MSG_KEY_LONGDOWN)
        {
            /*无停止消息过来，继续运行*/
            GUI_LstmPostRunMsg(listlayer->plistmenu, LSTMM_RUNDOWN);
        }
    }
    else
    {
        if (listlayer->setfoucs != REAL_FOUCS)
        {
            ListLayer_SetLayerFoucs(listlayer, listlayer->menulist->CurItem);
            listlayer->setfoucs = REAL_FOUCS;
        }

        return OK;
    }

    //LISTMENU_PRINTF("finish no = %d\n",listlayer->cur_fbno);
    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_FixedKeyUpMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer fixedkeyupmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_FixedKeyUpMsg(LIST_LAYER_MAN_t *listlayer, __u32 dist, __u32  step)
{
    __u8                       err;
    //__u8                       fbno;
    __u8                       i;
    __u32                      ret;
    //__u32                      status;
    __rect_t                   sw = {0, 0, 0, 0};
    //__u32                      condition;
    __move_para_t              dist_dly;
    __krnl_flags_t             flags;

    //fbno = 0;

    if (listlayer->menulist->CurItem != listlayer->menulist->head)
    {
        listlayer->unfoucs_pos.x = listlayer->foucs_pos.x;
        listlayer->unfoucs_pos.y = listlayer->foucs_pos.y + listlayer->menulist->CurItem->foucs_w -
                                   listlayer->menulist->CurItem->rect.height;
        listlayer->menulist->UnCurItem = listlayer->menulist->CurItem;
        listlayer->foucs_pos.y = listlayer->foucs_pos.y - listlayer->menulist->CurItem->rect.height;
        listlayer->foucs_pos.x = listlayer->foucs_pos.x;
        /*get listmenu layer sceen windows*/
        ret = GUI_LyrWinGetScnWindow(listlayer->layer, &listlayer->scnrect);

        if (ret != 0)
        {
            __wrn("layer set scn windows handle = %d\n", listlayer->layer);
            return __LINE__;
        }

        sw.y = listlayer->scnrect.y;
        listlayer->menulist->CurItem = listlayer->menulist->CurItem->Prev;
        listlayer->menulist->SelItem = listlayer->menulist->SelItem->Prev;
        esKRNL_FlagPost(listlayer->foucs_flags, LISTLAYER_FOUCS_FLAG, OS_FLAG_SET, &err);
        listlayer->cursteps = listlayer->nextsteps;

        for (i = 0; i < listlayer->cursteps; i++)
        {
            ret = InterMove(0, listlayer->menulist->CurItem->rect.height, listlayer->cursteps, i, &dist_dly);

            if (ret != OK)
            {
                return __LINE__;
            }

            if (listlayer->plistmenu->mode & LISTBOX_MOD)
            {
                sw.y = sw.y;
            }
            else
            {
                sw.y = sw.y + dist_dly.dist;
            }

            ret = ListLayer_SwitchLyr(listlayer, 0, sw.y);

            if (ret != EPDK_OK)
            {
                __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
                return __LINE__;
            }

            esKRNL_TimeDly(dist_dly.dly_time);
        }

        listlayer->scnrect.y = sw.y;
        flags = esKRNL_FlagQuery(listlayer->foucs_flags, &err);

        if (!(flags & LISTLAYER_FOUCS_FINISH_FLAG))
        {
            __inf("wait to foucs!flags = %d\n", flags);
            esKRNL_FlagPend(listlayer->foucs_flags, LISTLAYER_FOUCS_FINISH_FLAG,
                            ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);
        }

        listlayer->foucs_flags->OSFlagFlags &= (~LISTLAYER_FOUCS_FINISH_FLAG);
        esKRNL_TimeDly(2);

        /*判断是否有停止消息过来*/
        if (listlayer->msgcount != 0 &&
            listlayer->plistmenu->lastmsg == GUI_MSG_KEY_LONGUP)
        {
            /*无停止消息过来，继续运行*/
            GUI_LstmPostRunMsg(listlayer->plistmenu, LSTMM_RUNUP);
        }
    }
    else
    {
        if (listlayer->setfoucs != REAL_FOUCS)
        {
            ListLayer_SetLayerFoucs(listlayer, listlayer->menulist->CurItem);
            listlayer->setfoucs = REAL_FOUCS;
        }
    }

    // LISTMENU_PRINTF("finish no = %d\n",listlayer->cur_fbno);
    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_FixedKeyDownMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer fixedkeydownmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_FixedKeyDownMsg(LIST_LAYER_MAN_t    *listlayer, __u32 dist, __u32 step)
{
    __u8                       err;
    //__u8                       fbno;
    __u8                       i;
    __u32                      ret;
    //__u32                      condition;
    //__u32                      status;
    __rect_t                   sw;
    __move_para_t              dist_dly;
    __krnl_flags_t             flags;

    //fbno = 0;

    if (listlayer->menulist->CurItem != listlayer->menulist->tail)
    {
        /*save prev foucs position*/
        listlayer->unfoucs_pos.x = listlayer->foucs_pos.x;
        listlayer->unfoucs_pos.y = listlayer->foucs_pos.y;
        listlayer->menulist->UnCurItem = listlayer->menulist->CurItem;
        listlayer->foucs_pos.y = listlayer->foucs_pos.y + listlayer->menulist->CurItem->rect.height;
        listlayer->foucs_pos.x = listlayer->foucs_pos.x;
        /*get listmenu layer sceen windows*/
        ret = GUI_LyrWinGetScnWindow(listlayer->layer, &listlayer->scnrect);

        if (ret != 0)
        {
            __wrn("layer set scn windows handle = %d\n", listlayer->layer);
            return __LINE__;
        }

        sw.x = listlayer->scnrect.x;
        sw.y = listlayer->scnrect.y;
        listlayer->menulist->CurItem = listlayer->menulist->CurItem->Next;
        listlayer->menulist->SelItem = listlayer->menulist->SelItem->Next;
        esKRNL_FlagPost(listlayer->foucs_flags, LISTLAYER_FOUCS_FLAG, OS_FLAG_SET, &err); /*start foucs Item thread to foucs item*/
        listlayer->cursteps = listlayer->nextsteps;

        for (i = 0; i < listlayer->cursteps; i++)
        {
            ret = InterMove(0, listlayer->menulist->CurItem->rect.height, listlayer->cursteps, i, &dist_dly);

            if (ret != OK)
            {
                return __LINE__;
            }

            if (listlayer->plistmenu->mode & LISTBOX_MOD)
            {
                sw.y = sw.y;
            }
            else
            {
                sw.y = sw.y - dist_dly.dist;
            }

            ret = ListLayer_SwitchLyr(listlayer, 0, sw.y);

            if (ret != EPDK_OK)
            {
                __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
                return __LINE__;
            }

            esKRNL_TimeDly(dist_dly.dly_time);
        }

        listlayer->scnrect.x = sw.x;
        listlayer->scnrect.y = sw.y;
        flags = esKRNL_FlagQuery(listlayer->foucs_flags, &err);

        if (!(flags & LISTLAYER_FOUCS_FINISH_FLAG))
        {
            __inf("wait to foucs!flags = %d\n", flags);
            esKRNL_FlagPend(listlayer->foucs_flags, LISTLAYER_FOUCS_FINISH_FLAG,
                            ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);
        }

        listlayer->foucs_flags->OSFlagFlags &= (~LISTLAYER_FOUCS_FINISH_FLAG);
        esKRNL_TimeDly(2);

        /*判断是否有停止消息过来*/
        if (listlayer->msgcount != 0 &&
            listlayer->plistmenu->lastmsg == GUI_MSG_KEY_LONGDOWN)
        {
            /*无停止消息过来，继续运行*/
            GUI_LstmPostRunMsg(listlayer->plistmenu, LSTMM_RUNDOWN);
        }
    }
    else
    {
        if (listlayer->setfoucs != REAL_FOUCS)
        {
            ListLayer_SetLayerFoucs(listlayer, listlayer->menulist->CurItem);
            listlayer->setfoucs = REAL_FOUCS;
        }
    }

    return OK;
}


/*
**********************************************************************************************************************
*                                               ListLayer_VarKeyUpMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer varkeyupmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_FlatVarMoveDownMsg(LIST_LAYER_MAN_t *listlayer, __u32 dist)
{
    __u8                        err;
    __u8                        fbno;
    __u32                       ret;
    __u32                       status;
    __rect_t                    sw;
    __u32                       itemno;
    fbno = 0;
    status = ListLayer_GetBufferStatus(listlayer);

    if (status & WAITUP)
    {
        __inf("wait to load! status = %d\n", status);
        listlayer->status |= WAIT_LOAD;
        esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_WAIT_LOAD_FLAG, OS_FLAG_SET, &err);
        listlayer->wait_flags->OSFlagFlags &= (~LISTLAYER_WAIT_FLAG);
        esKRNL_FlagPend(listlayer->wait_flags, LISTLAYER_WAIT_FLAG,
                        ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);
        listlayer->status &= (~WAIT_LOAD);
    }

    if (listlayer->menulist->dist == 0)
    {
        return __LINE__;
    }
    else
    {
        if (listlayer->menulist->dist > dist)
        {
            listlayer->menulist->dist -= dist;
            itemno  =    listlayer->menulist->dist / listlayer->menulist->SelItem->rect.height;
        }
        else
        {
            itemno  = 0;
            dist    = listlayer->menulist->dist;
            listlayer->menulist->dist = 0;
        }

        while (listlayer->menulist->SelItem->itemno != itemno)
        {
            listlayer->menulist->SelItem = listlayer->menulist->SelItem->Prev;
        }
    }

    sw.x = listlayer->viewrect.x;
    sw.y = listlayer->viewrect.y;
    sw.y = sw.y - dist;

    if (sw.y < 0)
    {
        sw.y = listlayer->listbuffer->vrect.height + sw.y;
    }
    else if (sw.y > listlayer->listbuffer->vrect.height)
    {
        sw.y = sw.y - listlayer->listbuffer->vrect.height;
    }

    if (listlayer->plistmenu->slave_lyr == 0)
    {
        ret = ListLayer_SwitchFB(listlayer, fbno, sw.x, sw.y);

        if (ret != EPDK_OK)
        {
            __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
            return __LINE__;
        }
    }
    else
    {
        GUI_LyrWinCacheOn();
        ret = ListLayer_SwitchFB(listlayer, fbno, sw.x, sw.y);

        if (ret != EPDK_OK)
        {
            __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
            return __LINE__;
        }

        ret = GUI_LyrWinSetScnWindow(listlayer->plistmenu->slave_lyr, &listlayer->plistmenu->slyr_rect);

        if (ret != EPDK_OK)
        {
            __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
            return __LINE__;
        }

        listlayer->plistmenu->slyr_crect.y = listlayer->plistmenu->slyr_rect.y;
        GUI_LyrWinCacheOff();
    }

    esKRNL_SemPend(listlayer->pos_sem, 0, &err); /*pend pos semphore*/

    if (err == OS_NO_ERR)
    {
        listlayer->viewrect.x = sw.x;
        listlayer->viewrect.y = sw.y;
    }

    esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/
    esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_LOAD_FLAG, OS_FLAG_SET, &err);

    if (listlayer->LoadPrio)
    {
        //esKRNL_SemPend(listlayer->draw_sem,0,&err); /*pend pos semphore*/
        esKRNL_TaskSuspend(listlayer->LoadPrio);
        esKRNL_TimeDly(2);
        esKRNL_TaskResume(listlayer->LoadPrio);
        //esKRNL_SemPost(listlayer->draw_sem); /*pend pos semphore*/
    }
    else
    {
        esKRNL_TimeDly(2);
    }

    return OK;
}


/*
**********************************************************************************************************************
*                                               ListLayer_VarKeyDownMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer varkeydownmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_FlatVarMoveUpMsg(LIST_LAYER_MAN_t    *listlayer, __u32 dist)
{
    __u8                        err;
    __u32                       ret;
    __u32                       status;
    __rect_t                    sw;
    __u32                       tmp;
    __u32                       itemno;
    //__inf("listlayer->foucs.y = %d\n",listlayer->foucs_pos.y);
    status = ListLayer_GetBufferStatus(listlayer);

    if (status & WAITDOWN)
    {
        __inf("wait to load! status = %d\n", status);
        listlayer->status |= WAIT_LOAD;
        esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_WAIT_LOAD_FLAG, OS_FLAG_SET, &err);
        listlayer->wait_flags->OSFlagFlags &= (~LISTLAYER_WAIT_FLAG);
        esKRNL_FlagPend(listlayer->wait_flags, LISTLAYER_WAIT_FLAG,
                        ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);
        listlayer->status &= (~WAIT_LOAD);
    }

    if (listlayer->menulist->dist == listlayer->menulist->total_dist)
    {
        return __LINE__;
    }
    else
    {
        tmp = listlayer->menulist->dist;
        listlayer->menulist->dist += dist;

        if (listlayer->menulist->dist >= listlayer->menulist->total_dist)
        {
            listlayer->menulist->dist = listlayer->menulist->total_dist;
            dist    =  listlayer->menulist->total_dist - tmp;
            itemno = listlayer->menulist->ItemCount - 1;
        }
        else
        {
            itemno  =    listlayer->menulist->dist / listlayer->menulist->SelItem->rect.height;
        }

        while (listlayer->menulist->SelItem->itemno != itemno)
        {
            listlayer->menulist->SelItem = listlayer->menulist->SelItem->Next;
        }
    }

    sw.x = listlayer->viewrect.x;
    sw.y = listlayer->viewrect.y;
    sw.y = sw.y + dist;

    if (sw.y > listlayer->listbuffer->vrect.height)
    {
        sw.y = sw.y - listlayer->listbuffer->vrect.height;
    }
    else if (sw.y < 0)
    {
        sw.y = sw.y + listlayer->listbuffer->vrect.height;
    }

    if (listlayer->plistmenu->slave_lyr == 0)
    {
        ret = ListLayer_SwitchFB(listlayer, 0, sw.x, sw.y);

        if (ret != EPDK_OK)
        {
            __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
            return __LINE__;
        }
    }
    else
    {
        GUI_LyrWinCacheOn();
        ret = ListLayer_SwitchFB(listlayer, 0, sw.x, sw.y);

        if (ret != EPDK_OK)
        {
            __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
            return __LINE__;
        }

        ret = GUI_LyrWinSetScnWindow(listlayer->plistmenu->slave_lyr, &listlayer->plistmenu->slyr_rect);

        if (ret != EPDK_OK)
        {
            __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
            return __LINE__;
        }

        listlayer->plistmenu->slyr_crect.y = listlayer->plistmenu->slyr_rect.y;
        GUI_LyrWinCacheOff();
    }

    esKRNL_SemPend(listlayer->pos_sem, 0, &err); /*pend pos semphore*/

    if (err == OS_NO_ERR)
    {
        listlayer->viewrect.x = sw.x;
        listlayer->viewrect.y = sw.y;
    }

    esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/
    esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_LOAD_FLAG, OS_FLAG_SET, &err);

    if (listlayer->LoadPrio)
    {
        //esKRNL_SemPend(listlayer->draw_sem,0,&err); /*pend pos semphore*/
        esKRNL_TaskSuspend(listlayer->LoadPrio);
        esKRNL_TimeDly(2);
        esKRNL_TaskResume(listlayer->LoadPrio);
        //esKRNL_SemPost(listlayer->draw_sem); /*pend pos semphore*/
    }
    else
    {
        esKRNL_TimeDly(2);
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_FixedKeyUpMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer fixedkeyupmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_FlatFixedMoveUpMsg(LIST_LAYER_MAN_t *listlayer, __u32 dist)
{
    __u32                      ret;
    __rect_t                   sw = {0, 0, 0, 0};
    __u32                      itemno;
    __u32                      tmp;

    if (listlayer->menulist->dist == listlayer->menulist->total_dist)
    {
        return __LINE__;
    }
    else
    {
        tmp = listlayer->menulist->dist;
        listlayer->menulist->dist += dist;

        if (listlayer->menulist->dist >= listlayer->menulist->total_dist)
        {
            listlayer->menulist->dist = listlayer->menulist->total_dist;
            dist    =  listlayer->menulist->total_dist - tmp;
            itemno = listlayer->menulist->ItemCount - 1;
        }
        else
        {
            itemno  =    listlayer->menulist->dist / listlayer->menulist->SelItem->rect.height;
        }

        while (listlayer->menulist->SelItem->itemno != itemno)
        {
            listlayer->menulist->SelItem = listlayer->menulist->SelItem->Next;
        }
    }

    /*get listmenu layer sceen windows*/
    ret = GUI_LyrWinGetScnWindow(listlayer->layer, &listlayer->scnrect);

    if (ret != 0)
    {
        __wrn("layer set scn windows handle = %d\n", listlayer->layer);
        return __LINE__;
    }

    sw.y = listlayer->scnrect.y;
    sw.y = sw.y - dist;
    ret = ListLayer_SwitchLyr(listlayer, 0, sw.y);

    if (ret != EPDK_OK)
    {
        __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
        return __LINE__;
    }

    listlayer->scnrect.y = sw.y;
    esKRNL_TimeDly(2);
    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_FixedKeyDownMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer fixedkeydownmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_FlatFixedMoveDownMsg(LIST_LAYER_MAN_t    *listlayer, __u32 dist)
{
    __u32                      ret;
    __rect_t                   sw;
    __u32                      itemno;

    if (listlayer->menulist->dist == 0)
    {
        return __LINE__;
    }
    else
    {
        if (listlayer->menulist->dist > dist)
        {
            listlayer->menulist->dist -= dist;
            itemno  =    listlayer->menulist->dist / listlayer->menulist->SelItem->rect.height;
        }
        else
        {
            itemno  = 0;
            dist    = listlayer->menulist->dist;
            listlayer->menulist->dist = 0;
        }

        while (listlayer->menulist->SelItem->itemno != itemno)
        {
            listlayer->menulist->SelItem = listlayer->menulist->SelItem->Prev;
        }
    }

    /*get listmenu layer sceen windows*/
    ret = GUI_LyrWinGetScnWindow(listlayer->layer, &listlayer->scnrect);

    if (ret != 0)
    {
        __wrn("layer set scn windows handle = %d\n", listlayer->layer);
        return __LINE__;
    }

    sw.x = listlayer->scnrect.x;
    sw.y = listlayer->scnrect.y;
    sw.y = sw.y + dist;
    ret = ListLayer_SwitchLyr(listlayer, 0, sw.y);

    if (ret != EPDK_OK)
    {
        __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
        return __LINE__;
    }

    listlayer->scnrect.x = sw.x;
    listlayer->scnrect.y = sw.y;
    esKRNL_TimeDly(2);
    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_KeyUpMsg
*
* author:
*
* date:             2009-1-17
*
* Description:      listlayer keyupmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_KeyUpMsg(LIST_LAYER_MAN_t *listlayer, __u32 dist, __u32  step)
{
    if (listlayer->mode == VAR_MODE)
    {
        if (listlayer->plistmenu->mode & LISTBOX_MOD)
        {
            ListBox_VarKeyUpMsg(listlayer, dist, step);
        }
        else
        {
            /*item动态变化*/
            ListLayer_VarKeyUpMsg(listlayer, dist, step);
        }
    }
    else
    {
        if (listlayer->plistmenu->mode & LISTBOX_MOD)
        {
            ListBox_FixedKeyUpMsg(listlayer, dist, step);
        }
        else
        {
            /*item数量固定*/
            ListLayer_FixedKeyUpMsg(listlayer, dist, step);
        }
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_KeyDownMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer keydownmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_KeyDownMsg(LIST_LAYER_MAN_t *listlayer, __u32 dist, __u32  step)
{
    if (listlayer->mode == VAR_MODE)
    {
        if (listlayer->plistmenu->mode & LISTBOX_MOD)
        {
            ListBox_VarKeyDownMsg(listlayer, dist, step);
        }
        else
        {
            /*item动态变化*/
            ListLayer_VarKeyDownMsg(listlayer, dist, step);
        }
    }
    else
    {
        if (listlayer->plistmenu->mode & LISTBOX_MOD)
        {
            ListBox_FixedKeyDownMsg(listlayer, dist, step);
        }
        else
        {
            /*item数量固定*/
            ListLayer_FixedKeyDownMsg(listlayer, dist, step);
        }
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_KeyUpMsg
*
* author:
*
* date:             2009-1-17
*
* Description:      listlayer keyupmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_MoveUpMsg(LIST_LAYER_MAN_t *listlayer, __u32 dist, __u32  step)
{
    if (listlayer->mode == VAR_MODE)
    {
        if (listlayer->plistmenu->mode & LISTBOX_MOD)
        {
            ListBox_FlatVarMoveUpMsg(listlayer, dist);
        }
        else
        {
            /*item动态变化*/
            ListLayer_FlatVarMoveUpMsg(listlayer, dist);
        }
    }
    else
    {
        if (listlayer->plistmenu->mode & LISTBOX_MOD)
        {
            ListBox_FlatFixedMoveUpMsg(listlayer, dist);
        }
        else
        {
            /*item数量固定*/
            ListLayer_FlatFixedMoveUpMsg(listlayer, dist);
        }
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_KeyDownMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer keydownmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_MoveDownMsg(LIST_LAYER_MAN_t *listlayer, __u32 dist, __u32  step)
{
    if (listlayer->mode == VAR_MODE)
    {
        if (listlayer->plistmenu->mode & LISTBOX_MOD)
        {
            ListBox_FlatVarMoveDownMsg(listlayer, dist);
        }
        else
        {
            /*item动态变化*/
            ListLayer_FlatVarMoveDownMsg(listlayer, dist);
        }
    }
    else
    {
        if (listlayer->plistmenu->mode & LISTBOX_MOD)
        {
            ListBox_FlatFixedMoveDownMsg(listlayer, dist);
        }
        else
        {
            /*item数量固定*/
            ListLayer_FlatFixedMoveDownMsg(listlayer, dist);
        }
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               Listlayer_GetMoveStatus
*
* author:
*
* date:             2010-2-25
*
* Description:      listlayer getmovestatus
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  ListLayer_GetMoveStatus(LIST_LAYER_MAN_t *listlayer)
{
    if (listlayer->plistmenu->mode & LISTBOX_MOD)
    {
        return ListBox_GetMoveStatus(listlayer);
    }
    else
    {
        /*item动态变化*/
        return ListBox_GetMoveStatus(listlayer);
    }
}
/*
**********************************************************************************************************************
*                                               ListLayer_MoveMsg
*
* author:
*
* date:             2010-1-18
*
* Description:      listlayer movemsg
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListLayer_MoveMsg(LIST_LAYER_MAN_t *listlayer)
{
    __u8         err;
    int          step = 0;
    __s32        dist;
    __bool       flag = ORANGE_FALSE;
    esKRNL_SemPend(listlayer->plistmenu->move_sem, 0, &err);

    if (listlayer->plistmenu->index < listlayer->plistmenu->len)
    {
        step = listlayer->plistmenu->step[listlayer->plistmenu->index];
        listlayer->plistmenu->index++;
        flag = EPDK_TRUE;
    }

    esKRNL_SemPost(listlayer->plistmenu->move_sem);

    if (flag == EPDK_TRUE)
    {
        listlayer->menulist->CurItem     = NULL;
        dist = 2 * (listlayer->plistmenu->gcurv.frame) - step;
        esKRNL_SemPend(listlayer->plistmenu->dist_sem, 0, &err);
        ListBox_FlatMoveToDist(listlayer, dist);
        esKRNL_SemPost(listlayer->plistmenu->dist_sem);

        if (listlayer->plistmenu->index < listlayer->plistmenu->len)
        {
            /*无停止消息过来，继续运行*/
            GUI_LstmPostRunMsg(listlayer->plistmenu, LSTMM_MOVEMSG);
        }
        else
        {
            if (listlayer->mode != FIXED_MODE)
            {
                __u8  err;

                if (listlayer->menulist->ItemCount != 0)
                {
                    listlayer->focus_type = TOUCH_FOCUS;
                    GUI_LstmCurProc(listlayer->plistmenu);
                    esKRNL_FlagPost(listlayer->foucs_flags,
                                    LISTLAYER_DRAW_ITEM_FLAG, OS_FLAG_SET, &err);
                }
            }
        }
    }
    else
    {
        if (listlayer->mode != FIXED_MODE)
        {
            __u8  err;

            if (listlayer->menulist->ItemCount != 0)
            {
                listlayer->focus_type = TOUCH_FOCUS;
                GUI_LstmCurProc(listlayer->plistmenu);
                esKRNL_FlagPost(listlayer->foucs_flags,
                                LISTLAYER_DRAW_ITEM_FLAG, OS_FLAG_SET, &err);
            }
        }

        esKRNL_TimeDly(6);    /* delay 30ms */
    }

    return  ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_GetLoadStatus
*
* author:
*
* date:             2009-11-23
*
* Description:      listlayer getloadstatus
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool  ListLayer_GetLoadStatus(LIST_LAYER_MAN_t *listlayer)
{
    __u8                       err;
    __u32                      status;

    if (listlayer->mode == FIXED_MODE)
    {
        return  EPDK_TRUE;
    }

    if (listlayer->plistmenu->mode & LISTBOX_MOD)
    {
        /*等待后台线程加载完成*/
        status = ListBox_GetBufferStatus(listlayer);
    }
    else
    {
        /*等待后台线程加载完成*/
        status = ListLayer_GetBufferStatus(listlayer);
    }

    if (status == 0 && listlayer->load_flags->OSFlagWaitList)
    {
        return  EPDK_TRUE;
    }
    else
    {
        /*启动后台加载*/
        esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_LOAD_FLAG, OS_FLAG_SET, &err);
    }

    return EPDK_FALSE;
}

/*
**********************************************************************************************************************
*                                               ListLayer_OnMesssage
*
* author:
*
* date:             2009-1-17
*
* Description:      listlayer onmesssage
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_OnMesssage(LIST_LAYER_MAN_t    *listlayer, __listmenu_runmsg_t *pMsg)
{
    switch (pMsg->id)
    {
        case LSTMM_RUNLEFT  :
            break;

        case LSTMM_RUNRIGHT :
            break;

        case LSTMM_RUNUP:
            ListLayer_KeyUpMsg(listlayer,
                               (__u32)listlayer->menulist->CurItem->rect.height,
                               12);
            listlayer->menulist->setCurItem = 1;
            break;

        case LSTMM_RUNDOWN:
            ListLayer_KeyDownMsg(listlayer,
                                 (__u32)listlayer->menulist->CurItem->rect.height,
                                 12);
            listlayer->menulist->setCurItem = 1;
            break;

        case LSTMM_MOVEDOWN:
            ListLayer_MoveDownMsg(listlayer, pMsg->dwAddData, 0);
            break;

        case LSTMM_MOVEUP:
            ListLayer_MoveUpMsg(listlayer, pMsg->dwAddData, 0);
            break;

        case LSTMM_MOVEMSG:
            ListLayer_MoveMsg(listlayer);
            break;

        default:
            break;
    }

    return OK;
}
//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------


#endif //__LIST_LAYER__C____
