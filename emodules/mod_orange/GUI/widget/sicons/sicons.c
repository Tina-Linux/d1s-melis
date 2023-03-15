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
#ifndef __SICONS__C____
#define __SICONS__C____

#include "sicons_i.h"

/*定义删除时是重画上半部，还是重画下半部,或者直接清空(表示最后一项时)*/
#define  DEL_DELMOVEDOWN   0
#define  DEL_DELMOVEUP     1
#define  DEL_DELTAIL       2

static __s32 SIconsCtrlProc(__gui_msg_t  *msg);
/*
**********************************************************************************************************************
*                                               RegisterButtonControl
*
* author:
*
* date:             2009-8-4
*
* Description:      registerbuttoncontrol
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool RegisterIconsControl(void)
{
    __gui_winclass_t  WndClass;
    WndClass.spClassName = CTRL_SICONS;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.WinProc     = SIconsCtrlProc;
    return GUI_AddNewControlClass(&WndClass) == EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               UnRegisterListBoxControl
*
* author:
*
* date:             2010-5-24
*
* Description:      unregistericonscontrol
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool  UnRegisterIconsControl(void)
{
    return GUI_DeleteControlClass(CTRL_SICONS) == EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SIcons_InContent
*
* author:
*
* date:             2010-5-22
*
* Description:      判断某一个index是否在显示列表中
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __bool SIcons_InContent(__sicons_data_t *pdata, __u32 index)
{
    __u32   head_index;
    __u32   tail_index;
    head_index  = pdata->iconlist->HeadItem->itemno;
    tail_index  = head_index + pdata->valid_num - 1;

    if (head_index <= index && index <= tail_index)
    {
        return  EPDK_TRUE;
    }

    return EPDK_FALSE;
}

static __u32  SIcons_Memcpy(__sicons_data_t *pdata, __rect_t *src_rect, __pos_t *dst_rect)
{
    GUI_RECT rect;
    void    *src_addr;
    src_addr = (void *)((__u32)pdata->fb_addr + (src_rect->y * pdata->lyr_w + src_rect->x) * pdata->bpp);
    rect.x0 = dst_rect->x;
    rect.y0 = dst_rect->y;
    rect.x1 = dst_rect->x + src_rect->width - 1;
    rect.y1 = dst_rect->y + src_rect->height - 1;
    GUI_LyrWinSel(pdata->lyr_win);
    GUI_SetRectMem(&rect, src_addr, pdata->lyr_w, pdata->lyr_h, 0, 0);
    GUI_LyrWinFlushFB(pdata->lyr_win);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SFileList_FrmMemdevStart
*
* author:
*
* date:             2010-4-24
*
* Description:      sfilelist frmmemdevstart
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void SIcons_CtrlMemdevStart(__sicons_data_t *picons)
{
#if 0
    RECT   fbrect;
    GUI_LyrWinSel(picons->lyr_win);
    GUI_WinGetClientFBRect(picons->handle, &fbrect);

    if (picons->hmem == NULL)
    {
        picons->hmem = GUI_MEMDEV_Create(fbrect.x, fbrect.y, fbrect.width, fbrect.height);
    }

    GUI_MEMDEV_Select(picons->hmem);
#endif
}

/*
**********************************************************************************************************************
*                                               SFileList_FrmMemdevStop
*
* author:
*
* date:             2010-4-24
*
* Description:      sfilelist frmmemdevstop
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void SIcons_CtrlMemdevStop(__sicons_data_t *picons)
{
#if 0

    if (picons->hmem)
    {
        GUI_MEMDEV_CopyToLCD(picons->hmem);            // copy from memory device to fb
        GUI_MEMDEV_Select(NULL);                      // stop memory device
        GUI_MEMDEV_Delete(picons->hmem);
        picons->hmem = NULL;
    }

    GUI_LyrWinFlushFB(GUI_WinGetLyrWin(picons->handle));
#endif
}

/*
**********************************************************************************************************************
*                                               SIcons_DrawIconItem
*
* author:
*
* date:             2010-5-20
*
* Description:      sicons drawiconitem
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SIcons_DrawIconItem(__sicons_data_t *pdata, __icons_sitem_t   *iconitem, __bool memdev_flags)
{
    __iconsitemdraw_para_t       drawpara;
    __u32                       ret = 0;

    if (!iconitem)
    {
        return OK;
    }

    /*set current draw mode*/
    drawpara.mode           =   LISTITEM_DRAWMODE_UNFCS;
    drawpara.hwin           =   SICONS_ItemP2H(iconitem->iconitem);
    drawpara.memdev_flags   =   EPDK_TRUE;
    drawpara.attr           =   iconitem->iconitem->attr;
    drawpara.rect           =   iconitem->rect;
    ret = esKRNL_CallBack((__pCBK_t) * iconitem->cb, &drawpara);

    if (ret != LISTITEM_DRAWCB_ERROR)
    {
        return  EPDK_OK;
    }

    return EPDK_FAIL;
}


__s32  SIcons_DrawFreeIconItem(__sicons_data_t *pdata, __icons_sitem_t   *iconitem, __bool memdev_flags)
{
    __iconsitemdraw_para_t       drawpara;
    __u32                       ret = 0;

    if (!iconitem)
    {
        return OK;
    }

    /*set current draw mode*/
    drawpara.mode           =   LISTITEM_DRAWMODE_FREE;
    drawpara.hwin           =   pdata->handle;
    drawpara.memdev_flags   =   EPDK_TRUE;
    drawpara.attr           =   NULL;
    drawpara.rect           =   iconitem->rect;
    ret = esKRNL_CallBack((__pCBK_t) * iconitem->cb, &drawpara);

    if (ret != LISTITEM_DRAWCB_ERROR)
    {
        return  EPDK_OK;
    }

    return EPDK_FAIL;
}

/*
**********************************************************************************************************************
*                                               SIcons_FocusIconItem
*
* author:
*
* date:             2010-5-21
*
* Description:      sicons focusiconitem
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SIcons_FocusIconItem(__sicons_data_t *pdata, __icons_sitem_t   *iconitem, __bool memdev_flags)
{
    __iconsitemdraw_para_t       drawpara;
    __u32                       ret = 0;

    if (!iconitem)
    {
        return OK;
    }

    /*set current draw mode*/
    drawpara.mode           =   LISTITEM_DRAWMODE_FCS;
    drawpara.hwin           =   SICONS_ItemP2H(iconitem->iconitem);
    drawpara.memdev_flags   =   EPDK_TRUE;
    drawpara.attr           =   iconitem->iconitem->attr;
    drawpara.rect           =   iconitem->rect;
    ret = esKRNL_CallBack((__pCBK_t) * iconitem->cb, &drawpara);

    if (ret != LISTITEM_DRAWCB_ERROR)
    {
        return EPDK_OK;
    }

    return EPDK_FAIL;
}


/*
**********************************************************************************************************************
*                                               SIcons_DisableIconItem
*
* author:
*
* date:             2010-5-21
*
* Description:      sicons disableiconitem
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SIcons_DisableIconItem(__sicons_data_t *pdata, __icons_sitem_t   *iconitem, __bool memdev_flags)
{
    __iconsitemdraw_para_t       drawpara;
    __u32                       ret = 0;

    if (!iconitem)
    {
        return OK;
    }

    /*set current draw mode*/
    drawpara.mode           =   LISTITEM_DRAWMODE_DISABLE;
    drawpara.hwin           =   SICONS_ItemP2H(iconitem->iconitem);
    drawpara.memdev_flags   =   EPDK_TRUE;
    drawpara.attr           =   iconitem->iconitem->attr;
    drawpara.rect           =   iconitem->rect;
    ret = esKRNL_CallBack((__pCBK_t) * iconitem->cb, &drawpara);

    if (ret != LISTITEM_DRAWCB_ERROR)
    {
        return EPDK_OK;
    }

    return EPDK_FAIL;
}

/*
**********************************************************************************************************************
*                                               SIcons_Ondraw
*
* author:
*
* date:             2010-5-20
*
* Description:      sicons ondraw
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SIcons_Ondraw(H_WIN  hicons)
{
    __u32                         num;
    __sicons_data_t              *picons;
    __icons_sitem_t              *psitem;
    H_LYR                         h_lyr;
    RECT                          rect;

    if (hicons == NULL)
    {
        return EPDK_FAIL;
    }

    h_lyr    = GUI_WinGetLyrWin(hicons);
    GUI_WinGetWinFBRect(hicons, &rect);
    picons = (__sicons_data_t *)GUI_CtrlWinGetAddData(hicons);
    GUI_LyrWinSel(h_lyr);
    SIcons_CtrlMemdevStart(picons);

    if (picons->bk_draw)
    {
        if (picons->bk_bmp)
        {
            GUI_BMP_Draw(picons->bk_bmp, 0, 0);
        }
        else
        {
            GUI_SetBkColor(picons->bk_color);
            GUI_ClearRect(rect.x, rect.y, rect.x + rect.width - 1, rect.y + rect.height - 1);
        }
    }

    for (num = 0; num < picons->valid_num; num++)
    {
        psitem = (__icons_sitem_t *)picons->sitem_array[num];

        if (!psitem->iconitem->enable)
        {
            SIcons_DisableIconItem(picons, psitem, EPDK_FALSE);
        }
        else
        {
            SIcons_DrawIconItem(picons, psitem, EPDK_FALSE);
        }
    }

    if (picons->valid_num < picons->scnnum)
    {
        for (num = picons->valid_num; num < picons->scnnum; num++)
        {
            psitem = (__icons_sitem_t *)picons->sitem_array[num];
            SIcons_DrawFreeIconItem(picons, psitem, EPDK_FALSE);
        }
    }

    SIcons_CtrlMemdevStop(picons);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SIcons_UpdateIconlist
*
* author:
*
* date:             2010-5-24
*
* Description:      sicons updateiconlist
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SIcons_UpdateIconlist(H_WIN  hicons, __sicons_para_t *para)
{
    __u32                         num;
    __u32                         headitemno;
    __sicons_data_t            *picons;
    __icons_iconlist_t         *iconlist;
    __icons_iconitem_t         *iconitem;
    __icons_sitem_t            *sitem;
    RECT                          win_rect;

    if ((para->headitemno >= para->nitem) ||
        (para->curitemno >= para->nitem) || (para->headitemno > para->curitemno))
    {
        __wrn("input icons parameter error!\n");
        return  0;
    }

    if (hicons == NULL || para == NULL)
    {
        __wrn("input para error!\n");
        return EPDK_FAIL;
    }

    picons = (__sicons_data_t *)GUI_CtrlWinGetAddData(hicons);
    iconlist = picons->iconlist;
    SIconList_DelAll(iconlist);
    picons->cur_index = 0;

    if (para->nitem != 0)
    {
        GUI_WinGetWinRECT(hicons, &win_rect);

        for (num = 0; num < para->nitem; num++)
        {
            /*IconItem Create*/
            iconitem                       = SIconItem_Create(&para->items[num], hicons);

            if (!iconitem)
            {
                __wrn("create iconitem %d failed!\n", num);
                goto    error;
            }

            /*IconItem Add to ListIcon*/
            SIconList_AddTail(picons->iconlist, iconitem);
        }

        for (num = 0; num < picons->scnnum; num++)
        {
            sitem = (__icons_sitem_t *)picons->sitem_array[num];
            SShowItem_Destory(sitem);
        }

        sicons_free(picons->sitem_array);
        picons->scnnum = win_rect.width / para->items[0].item_w;

        if (picons->scnnum == 0)
        {
            __wrn("input item width error!\n");
            goto error;
        }

        picons->iconlist->pagenum  = picons->scnnum;

        if (picons->scnnum > (picons->iconlist->ItemCount - para->headitemno))
        {
            if (picons->scnnum > picons->iconlist->ItemCount)
            {
                picons->valid_num = picons->iconlist->ItemCount;
                headitemno          = 0;
            }
            else
            {
                picons->valid_num = picons->scnnum;
                headitemno          = picons->iconlist->ItemCount - picons->scnnum;
            }
        }
        else
        {
            picons->valid_num = picons->scnnum;
            headitemno          = para->headitemno;
        }

        iconitem = SIconList_GetIconitem(picons->iconlist, headitemno);

        if (!iconitem)
        {
            iconitem = picons->iconlist->head;
        }

        picons->iconlist->HeadItem  = iconitem;
        picons->sitem_array = (h_iconssitem *)sicons_malloc(picons->scnnum * sizeof(h_iconssitem));

        for (num = 0; num < picons->scnnum; num++)
        {
            __icons_sitem_t           sitem_attr;
            __icons_sitem_t           *tmp_sitem;
            sitem_attr.iconitem         = iconitem;
            sitem_attr.cb               = iconitem->cb;
            sitem_attr.rect.x           = num * iconitem->item_w;
            sitem_attr.rect.y           = 0;
            sitem_attr.rect.width       = iconitem->item_w;
            sitem_attr.rect.height      = win_rect.height;
            tmp_sitem                   = SShowItem_Create(&sitem_attr);
            picons->sitem_array[num]  = (h_iconssitem)tmp_sitem;

            if (num < picons->valid_num)
            {
                if (iconitem->itemno == para->curitemno)
                {
                    picons->cur_index     = num;
                }
            }

            iconitem                    = iconitem->Next;
        }
    }

    return EPDK_OK;
error:

    if (picons && picons->iconlist)
    {
        SIconList_DelAll(picons->iconlist);
    }

    return EPDK_FAIL;
}


/*
**********************************************************************************************************************
*                                               SIcons_UpdateControl
*
* author:
*
* date:             2010-5-22
*
* Description:      sicons updatecontrol
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 SIcons_UpdateControl(__sicons_data_t *picons)
{
    __u32                         num;
    __icons_sitem_t            *psitem;
    H_LYR                         h_lyr;
    RECT                          rect;
    h_lyr    = GUI_WinGetLyrWin(picons->handle);
    GUI_WinGetWinFBRect(picons->handle, &rect);
    picons = (__sicons_data_t *)GUI_CtrlWinGetAddData(picons->handle);
    GUI_LyrWinSel(h_lyr);
    SIcons_CtrlMemdevStart(picons);

    if (picons->valid_num != picons->scnnum)
    {
        if (picons->bk_draw)
        {
            if (picons->bk_bmp)
            {
                GUI_BMP_Draw(picons->bk_bmp, 0, 0);
            }
            else
            {
                GUI_SetBkColor(picons->bk_color);
                GUI_ClearRect(rect.x, rect.y, rect.x + rect.width - 1, rect.y + rect.height - 1);
            }
        }
    }

    for (num = 0; num < picons->valid_num; num++)
    {
        psitem = (__icons_sitem_t *)picons->sitem_array[num];

        if (!psitem->iconitem->enable)
        {
            SIcons_DisableIconItem(picons, psitem, EPDK_FALSE);
        }
        else if (num == picons->cur_index)
        {
            SIcons_FocusIconItem(picons, psitem, EPDK_FALSE);
        }
        else
        {
            SIcons_DrawIconItem(picons, psitem, EPDK_FALSE);
        }
    }

    if (picons->valid_num < picons->scnnum)
    {
        for (num = picons->valid_num; num < picons->scnnum; num++)
        {
            psitem = (__icons_sitem_t *)picons->sitem_array[num];
            SIcons_DrawFreeIconItem(picons, psitem, EPDK_FALSE);
        }
    }

    SIcons_CtrlMemdevStop(picons);
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               SIcons_UpdateUpEx
*
* author:
*
* date:             2010-6-11
*
* Description:      sicons updateupex
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SIcons_UpdateUpEx(__sicons_data_t *picons)
{
    __u32                         num;
    __icons_sitem_t              *psitem;
    RECT                          src_rect;
    __pos_t                       dst_pos;
    picons = (__sicons_data_t *)GUI_CtrlWinGetAddData(picons->handle);

    for (num = picons->valid_num - 1; num > 0; num--)
    {
        psitem = (__icons_sitem_t *)picons->sitem_array[num - 1];
        src_rect.x      = psitem->rect.x;
        src_rect.y      = psitem->rect.y;
        src_rect.width  = psitem->rect.width;
        src_rect.height = psitem->rect.height;
        psitem = (__icons_sitem_t *)picons->sitem_array[num];
        dst_pos.x       = psitem->rect.x;
        dst_pos.y       = psitem->rect.y;
        SIcons_Memcpy(picons, &src_rect, &dst_pos);
    }

    psitem = (__icons_sitem_t *)picons->sitem_array[picons->cur_index];
    SIcons_FocusIconItem(picons, psitem, EPDK_TRUE);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SIcons_UpdateDownEx
*
* author:
*
* date:             2010-6-11
*
* Description:      sicons updatedownex
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SIcons_UpdateDownEx(__sicons_data_t *picons)
{
    __u32                         num;
    __icons_sitem_t            *psitem;
    RECT                          src_rect;
    __pos_t                       dst_pos;
    picons = (__sicons_data_t *)GUI_CtrlWinGetAddData(picons->handle);

    for (num = 0; num < picons->valid_num - 1; num++)
    {
        psitem = (__icons_sitem_t *)picons->sitem_array[num + 1];
        src_rect.x      = psitem->rect.x;
        src_rect.y      = psitem->rect.y;
        src_rect.width  = psitem->rect.width;
        src_rect.height = psitem->rect.height;
        psitem = (__icons_sitem_t *)picons->sitem_array[num];
        dst_pos.x       = psitem->rect.x;
        dst_pos.y       = psitem->rect.y;
        SIcons_Memcpy(picons, &src_rect, &dst_pos);
    }

    psitem = (__icons_sitem_t *)picons->sitem_array[picons->cur_index];
    SIcons_FocusIconItem(picons, psitem, EPDK_TRUE);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SIcons_UpdateUp
*
* author:
*
* date:             2010-5-22
*
* Description:      sicons updateup
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 SIcons_UpdateUp(__sicons_data_t *picons)
{
    __u32                           num;
    __u32                           real_pagenum;
    __u32                           index;
    __icons_sitem_t              *psitem;
    __icons_iconitem_t           *head_item;
    __icons_iconitem_t           *cur_item;
    __icons_iconitem_t           *prev_item;
    psitem      = (__icons_sitem_t *)picons->sitem_array[picons->cur_index];
    cur_item    = psitem->iconitem;
    prev_item   = SIconList_FindPrevEnable(picons->iconlist, cur_item);
    head_item   = picons->iconlist->HeadItem;
    /*设置起始item，并返回当前item和起始item的偏差值*/
    index = SIconList_SetHeadItemFromItem(picons->iconlist, prev_item);

    if (picons->iconlist->HeadItem == head_item)
    {
        /*将以前的绘制状态去焦*/
        psitem = (__icons_sitem_t *)picons->sitem_array[picons->cur_index];

        if (psitem->iconitem->enable == EPDK_FALSE)
        {
            SIcons_DisableIconItem(picons, psitem, EPDK_TRUE);
        }
        else
        {
            SIcons_DrawIconItem(picons, psitem, EPDK_TRUE);
        }

        /*重新设置焦点状态*/
        psitem = (__icons_sitem_t *)picons->sitem_array[index];
        SIcons_FocusIconItem(picons, psitem, EPDK_TRUE);
        picons->cur_index = index;
    }
    else
    {
        cur_item            = picons->iconlist->HeadItem;

        if (picons->iconlist->HeadItem == head_item->Prev)
        {
            psitem = (__icons_sitem_t *)picons->sitem_array[picons->cur_index];
            SIcons_DrawIconItem(picons, psitem, EPDK_TRUE);
        }

        picons->cur_index = index;
        real_pagenum = SIconList_GetCurPageNum(picons->iconlist);

        for (num = 0; num < real_pagenum; num++)
        {
            psitem           = (__icons_sitem_t *)picons->sitem_array[num];
            psitem->iconitem = cur_item;
            cur_item         = cur_item->Next;
        }

        picons->valid_num  = real_pagenum;

        if (picons->iconlist->HeadItem != head_item->Prev)
        {
            SIcons_UpdateControl(picons);
        }
        else
        {
            SIcons_UpdateUpEx(picons);
        }
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SIcons_UpdateDown
*
* author:
*
* date:             2010-5-22
*
* Description:      sicons updatedown
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 SIcons_UpdateDown(__sicons_data_t *picons)
{
    __u32                           num;
    __u32                           real_pagenum;
    __u32                           index;
    __icons_sitem_t              *psitem;
    __icons_iconitem_t           *head_item;
    __icons_iconitem_t           *cur_item;
    __icons_iconitem_t           *next_item;
    psitem      = (__icons_sitem_t *)picons->sitem_array[picons->cur_index];
    cur_item    = psitem->iconitem;
    next_item   = SIconList_FindNextEnable(picons->iconlist, cur_item);
    head_item   = picons->iconlist->HeadItem;
    /*设置起始item，并返回当前item和起始item的偏差值*/
    index = SIconList_SetHeadItemFromItem(picons->iconlist, next_item);

    if (picons->iconlist->HeadItem == head_item)
    {
        /*将以前的绘制状态去焦*/
        psitem = (__icons_sitem_t *)picons->sitem_array[picons->cur_index];

        if (psitem->iconitem->enable == EPDK_FALSE)
        {
            SIcons_DisableIconItem(picons, psitem, EPDK_TRUE);
        }
        else
        {
            SIcons_DrawIconItem(picons, psitem, EPDK_TRUE);
        }

        /*重新设置焦点状态*/
        psitem = (__icons_sitem_t *)picons->sitem_array[index];
        SIcons_FocusIconItem(picons, psitem, EPDK_TRUE);
        picons->cur_index = index;
    }
    else
    {
        cur_item            = picons->iconlist->HeadItem;

        if (picons->iconlist->HeadItem == head_item->Next)
        {
            psitem = (__icons_sitem_t *)picons->sitem_array[picons->cur_index];
            SIcons_DrawIconItem(picons, psitem, EPDK_TRUE);
        }

        picons->cur_index = index;
        real_pagenum = SIconList_GetCurPageNum(picons->iconlist);

        for (num = 0; num < real_pagenum; num++)
        {
            psitem           = (__icons_sitem_t *)picons->sitem_array[num];
            psitem->iconitem = cur_item;
            cur_item         = cur_item->Next;
        }

        picons->valid_num  = real_pagenum;

        if (picons->iconlist->HeadItem != head_item->Next)
        {
            SIcons_UpdateControl(picons);
        }
        else
        {
            SIcons_UpdateDownEx(picons);
        }
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SIcons_AddTail
*
* author:
*
* date:             2010-5-20
*
* Description:      sicons addtail
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SIcons_AddTail(H_WIN  hicons, __sicons_item_attr_t *item_para)
{
    __sicons_data_t            *picons;
    __icons_iconitem_t         *tmpItem;
    __icons_sitem_t            *tail_sitem;
    __u32                         num;

    if (hicons == NULL || item_para == NULL)
    {
        return EPDK_FAIL;
    }

    picons = (__sicons_data_t *)GUI_CtrlWinGetAddData(hicons);
    tmpItem = SIconItem_Create(item_para, hicons);

    if (!tmpItem)
    {
        __wrn("create iconitem failed!\n");
        return EPDK_FAIL;
    }

    SIconList_AddTail(picons->iconlist, tmpItem);

    if (picons->valid_num < picons->scnnum)
    {
        tail_sitem  = (__icons_sitem_t *)picons->sitem_array[picons->valid_num];
        tail_sitem->iconitem = tmpItem;

        if (tmpItem->enable)
        {
            SIcons_DrawIconItem(picons, tail_sitem, EPDK_TRUE);
        }
        else
        {
            SIcons_DisableIconItem(picons, tail_sitem, EPDK_TRUE);
        }

        picons->valid_num++;
    }

    if (picons->valid_num < picons->scnnum)
    {
        for (num = picons->valid_num; num < picons->scnnum; num++)
        {
            tail_sitem = (__icons_sitem_t *)picons->sitem_array[num];
            SIcons_DrawFreeIconItem(picons, tail_sitem, EPDK_FALSE);
        }
    }

    return EPDK_OK;
}

//==================================================================
//function name:    SIcons_DelUpdateBottom
//author:
//date:             2014-6-24
//description:      sicons delupdatebottom
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

static  __s32 SIcons_DelUpdateBottom(__sicons_data_t *picons, __u32 count)
{
    __u32                         num;
    __icons_sitem_t            *psitem;
    RECT                          src_rect;
    __pos_t                       dst_pos;
    psitem = (__icons_sitem_t *)picons->sitem_array[picons->cur_index];
    SIcons_FocusIconItem(picons, psitem, EPDK_TRUE);

    for (num = picons->cur_index + 1; num < count - 1; num++)
    {
        psitem = (__icons_sitem_t *)picons->sitem_array[num + 1];
        src_rect.x      = psitem->rect.x;
        src_rect.y      = psitem->rect.y;
        src_rect.width  = psitem->rect.width;
        src_rect.height = psitem->rect.height;
        psitem = (__icons_sitem_t *)picons->sitem_array[num];
        dst_pos.x       = psitem->rect.x;
        dst_pos.y       = psitem->rect.y;
        SIcons_Memcpy(picons, &src_rect, &dst_pos);
    }

    if (picons->valid_num < count)
    {
        psitem = (__icons_sitem_t *)picons->sitem_array[count - 1];
        SIcons_DrawFreeIconItem(picons, psitem, EPDK_TRUE);
    }
    else
    {
        if (picons->cur_index != picons->valid_num - 1)
        {
            psitem = (__icons_sitem_t *)picons->sitem_array[picons->valid_num - 1];

            if (!psitem->iconitem->enable)
            {
                SIcons_DisableIconItem(picons, psitem, EPDK_FALSE);
            }
            else
            {
                SIcons_DrawIconItem(picons, psitem, EPDK_FALSE);
            }
        }
    }

    return  EPDK_OK;
}

//==================================================================
//function name:    SIcons_DelUpdateUp
//author:
//date:             2014-6-24
//description:      sicons delupdateup
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

static  __s32 SIcons_DelUpdateUp(__sicons_data_t *picons)
{
    __s32                         num;
    __icons_sitem_t            *psitem;
    RECT                          src_rect;
    __pos_t                       dst_pos;
    psitem = (__icons_sitem_t *)picons->sitem_array[picons->cur_index];
    SIcons_FocusIconItem(picons, psitem, EPDK_TRUE);

    for (num = picons->cur_index - 1; num > 0; num--)
    {
        psitem = (__icons_sitem_t *)picons->sitem_array[num - 1];
        src_rect.x      = psitem->rect.x;
        src_rect.y      = psitem->rect.y;
        src_rect.width  = psitem->rect.width;
        src_rect.height = psitem->rect.height;
        psitem = (__icons_sitem_t *)picons->sitem_array[num];
        dst_pos.x       = psitem->rect.x;
        dst_pos.y       = psitem->rect.y;
        SIcons_Memcpy(picons, &src_rect, &dst_pos);
    }

    if (picons->cur_index != 0)
    {
        psitem = (__icons_sitem_t *)picons->sitem_array[0];

        if (!psitem->iconitem->enable)
        {
            SIcons_DisableIconItem(picons, psitem, EPDK_FALSE);
        }
        else
        {
            SIcons_DrawIconItem(picons, psitem, EPDK_FALSE);
        }
    }

    return  EPDK_OK;
}

static  __s32 SIcons_DelTail(__sicons_data_t *picons, __u32 tailidx)
{
    __icons_sitem_t            *psitem;
    psitem = (__icons_sitem_t *)picons->sitem_array[tailidx];
    SIcons_DrawFreeIconItem(picons, psitem, EPDK_TRUE);

    if (tailidx > 0)
    {
        psitem = (__icons_sitem_t *)picons->sitem_array[tailidx - 1];
        SIcons_FocusIconItem(picons, psitem, EPDK_TRUE);
    }

    return  EPDK_OK;
}

//==================================================================
//function name:    SIcons_DelSetHeadItem
//author:
//date:             2014-6-24
//description:      sicons delsetheaditem
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

static  __s32 SIcons_DelSetHeadItem(__sicons_data_t *picons, __icons_iconitem_t *tmpItem)
{
    __u32                         ret;
    __u32                         count;
    __icons_iconitem_t         *HeadItem;
    __icons_iconitem_t         *headprev;
    __icons_iconitem_t         *headnext;
    __u32                         real_pagenum;
    __u32                         num;
    __icons_sitem_t            *psitem;
    __icons_iconitem_t         *cur_item;
    HeadItem     = picons->iconlist->HeadItem;

    /*获取前一个头指针*/
    if (picons->iconlist->ItemCount == 1)
    {
        headprev = NULL;
        headnext = NULL;
    }
    else if (picons->iconlist->HeadItem == picons->iconlist->head)
    {
        headprev = NULL;
        headnext = picons->iconlist->HeadItem->Next;
    }
    else if (picons->iconlist->HeadItem == picons->iconlist->tail)
    {
        headprev = picons->iconlist->HeadItem->Prev;
        headnext = NULL;
    }
    else
    {
        headprev = picons->iconlist->HeadItem->Prev;
        headnext = picons->iconlist->HeadItem->Next;
    }

    ret = SIconList_DelItem(picons->iconlist, tmpItem);

    if (ret != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    if (picons->cur_index == 0)
    {
        /*先设定headitem为下个item*/
        tmpItem = headnext;

        if (tmpItem != NULL)
        {
            count   = picons->iconlist->ItemCount - tmpItem->itemno;

            if (count < picons->scnnum)
            {
                if (tmpItem == picons->iconlist->head)
                {
                    picons->iconlist->HeadItem = tmpItem;
                    ret = DEL_DELMOVEDOWN;
                }
                else
                {
                    picons->iconlist->HeadItem = headprev;
                    ret = DEL_DELMOVEUP;
                }
            }
            else
            {
                picons->iconlist->HeadItem = headnext;
                ret = DEL_DELMOVEDOWN;
            }
        }
        else
        {
            ret = DEL_DELTAIL;
        }
    }/*删除最后一个*/
    else if (picons->cur_index == picons->valid_num - 1)
    {
        /*先设定headitem为当前*/
        tmpItem = HeadItem;
        count   = picons->iconlist->ItemCount - tmpItem->itemno;

        if (count < picons->scnnum)
        {
            if (tmpItem == picons->iconlist->head)
            {
                picons->iconlist->HeadItem = tmpItem;

                if (picons->cur_index > 0)
                {
                    picons->cur_index -= 1;
                }

                ret = DEL_DELTAIL;
            }
            else
            {
                picons->iconlist->HeadItem = headprev;
                ret = DEL_DELMOVEUP;
            }
        }
        else
        {
            picons->iconlist->HeadItem = tmpItem;
            ret = DEL_DELMOVEDOWN;
        }
    }
    else
    {
        /*先设定headitem为当前*/
        tmpItem = HeadItem;
        count   = picons->iconlist->ItemCount - tmpItem->itemno;

        if (count < picons->scnnum)
        {
            if (tmpItem == picons->iconlist->head)
            {
                picons->iconlist->HeadItem = tmpItem;
                ret = DEL_DELMOVEDOWN;
            }
            else
            {
                picons->iconlist->HeadItem = headprev;
                ret = DEL_DELMOVEUP;
            }
        }
        else
        {
            picons->iconlist->HeadItem = tmpItem;
            ret = DEL_DELMOVEDOWN;
        }
    }

    cur_item             = picons->iconlist->HeadItem;
    real_pagenum         = SIconList_GetCurPageNum(picons->iconlist);

    for (num = 0; num < real_pagenum; num++)
    {
        psitem           = (__icons_sitem_t *)picons->sitem_array[num];
        psitem->iconitem = cur_item;
        cur_item         = cur_item->Next;
    }

    picons->valid_num  = real_pagenum;
    return   ret;
}

/*
**********************************************************************************************************************
*                                               SLIstbox_DelCurItem
*
* author:
*
* date:             2009-9-21
*
* Description:      gui lstmdelcuritem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32   SIcons_DelCurItem(H_WIN hicons)
{
    __u32                         ret;
    __sicons_data_t            *picons;
    __icons_iconitem_t         *tmpItem;
    __u32                         count;
    __icons_sitem_t            *tail_sitem;

    if (hicons == NULL)
    {
        return EPDK_FAIL;
    }

    picons = (__sicons_data_t *)GUI_CtrlWinGetAddData(hicons);

    if (picons->iconlist->ItemCount)
    {
        __u32  index;
        index = picons->iconlist->HeadItem->itemno + picons->cur_index;
        tmpItem = SIconList_GetIconitem(picons->iconlist, index);
    }
    else
    {
        return EPDK_OK;
    }

    count    = picons->valid_num;
    ret = SIcons_DelSetHeadItem(picons, tmpItem);

    if (ret == DEL_DELMOVEDOWN)
    {
        SIcons_DelUpdateBottom(picons, count);
    }
    else if (ret == DEL_DELMOVEUP)
    {
        SIcons_DelUpdateUp(picons);
    }
    else
    {
        SIcons_DelTail(picons, count - 1);
    }

    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               SIcons_ItemEnable
*
* author:
*
* date:             2010-5-20
*
* Description:      sicons itemenable
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SIcons_ItemEnable(H_WIN  hicons, __u32 index)
{
    __sicons_data_t            *picons;
    __icons_iconitem_t         *tmpItem;
    __bool                        is_content;
    __icons_sitem_t            *cur_sitem;

    if (hicons == NULL)
    {
        return EPDK_FAIL;
    }

    picons    = (__sicons_data_t *)GUI_CtrlWinGetAddData(hicons);
    tmpItem     = SIconList_GetIconitem(picons->iconlist, index);

    if (!tmpItem)
    {
        __wrn("input index error!\n");
        return EPDK_FAIL;
    }

    if (tmpItem->enable)
    {
        return EPDK_OK;
    }

    tmpItem->enable = EPDK_TRUE;
    is_content  = SIcons_InContent(picons, index);

    if (is_content)
    {
        cur_sitem = (__icons_sitem_t *)picons->sitem_array[tmpItem->itemno - picons->iconlist->HeadItem->itemno];
        SIcons_DrawIconItem(picons, cur_sitem, EPDK_TRUE);
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SIcons_ItemDisable
*
* author:
*
* date:             2010-5-20
*
* Description:      sicons itemdisable
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SIcons_ItemDisable(H_WIN  hicons, __u32 index)
{
    __sicons_data_t            *picons;
    __icons_iconitem_t         *tmpItem;
    __bool                        is_content;
    __icons_sitem_t            *cur_sitem;

    if (hicons == NULL)
    {
        return EPDK_FAIL;
    }

    picons    = (__sicons_data_t *)GUI_CtrlWinGetAddData(hicons);
    tmpItem     = SIconList_GetIconitem(picons->iconlist, index);

    if (!tmpItem)
    {
        __wrn("input index error!\n");
        return EPDK_FAIL;
    }

    if (!tmpItem->enable)
    {
        return EPDK_OK;
    }

    tmpItem->enable = EPDK_FALSE;
    is_content  = SIcons_InContent(picons, index);

    if (is_content)
    {
        cur_sitem = (__icons_sitem_t *)picons->sitem_array[tmpItem->itemno - picons->iconlist->HeadItem->itemno];
        SIcons_DisableIconItem(picons, cur_sitem, EPDK_TRUE);
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SIcons_NextPage
*
* author:
*
* date:             2010-5-22
*
* Description:      sicons nextpage
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 SIcons_NextPage(H_WIN  hicons)
{
    __u32                         num;
    __u32                         next_head;
    __sicons_data_t            *picons;
    __icons_iconitem_t         *tmpItem;
    __icons_sitem_t            *cur_sitem;

    if (hicons == NULL)
    {
        return EPDK_FAIL;
    }

    picons = (__sicons_data_t *)GUI_CtrlWinGetAddData(hicons);
    next_head = picons->iconlist->HeadItem->itemno + picons->scnnum;

    if (next_head >= picons->iconlist->ItemCount)
    {
        return EPDK_FAIL;
    }

    if (next_head + picons->scnnum >= picons->iconlist->ItemCount)
    {
        picons->valid_num = picons->iconlist->ItemCount - next_head;
    }
    else
    {
        picons->valid_num = picons->scnnum;
    }

    tmpItem = SIconList_GetIconitem(picons->iconlist, next_head);
    picons->iconlist->HeadItem = tmpItem;
    tmpItem = picons->iconlist->HeadItem;
    picons->cur_index = 0;

    for (num = 0; num < picons->valid_num; num++)
    {
        cur_sitem = (__icons_sitem_t *)picons->sitem_array[num];
        cur_sitem->iconitem = tmpItem;
        tmpItem = tmpItem->Next;
    }

    SIcons_UpdateControl(picons);
    cur_sitem = (__icons_sitem_t *)picons->sitem_array[0];
    SIcons_FocusIconItem(picons, cur_sitem, EPDK_TRUE);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SIcons_PrevPage
*
* author:
*
* date:             2010-5-22
*
* Description:      sicons prevpage
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 SIcons_PrevPage(H_WIN  hicons)
{
    __u32                         num;
    __s32                         prev_head;
    __sicons_data_t            *picons;
    __icons_iconitem_t         *tmpItem;
    __icons_sitem_t            *cur_sitem;

    if (hicons == NULL)
    {
        return EPDK_FAIL;
    }

    picons = (__sicons_data_t *)GUI_CtrlWinGetAddData(hicons);
    prev_head = picons->iconlist->HeadItem->itemno - picons->scnnum;

    if (prev_head < 0)
    {
        return EPDK_FAIL;
    }

    picons->valid_num = picons->scnnum;
    tmpItem = SIconList_GetIconitem(picons->iconlist, prev_head);
    picons->iconlist->HeadItem = tmpItem;
    tmpItem = picons->iconlist->HeadItem;
    picons->cur_index = 0;

    for (num = 0; num < picons->valid_num; num++)
    {
        cur_sitem = (__icons_sitem_t *)picons->sitem_array[num];
        cur_sitem->iconitem = tmpItem;
        tmpItem = tmpItem->Next;
    }

    SIcons_UpdateControl(picons);
    cur_sitem = (__icons_sitem_t *)picons->sitem_array[picons->cur_index];
    SIcons_FocusIconItem(picons, cur_sitem, EPDK_TRUE);
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               SIcons_PrivCreate
*
* author:
*
* date:             2009-9-17
*
* Description:      gui lstmprivcreate
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__sicons_data_t *SIcons_PrivCreate(H_WIN hicons)
{
    __u32                                   num;
    __icons_iconitem_t                    *iconitem;
    __icons_iconitem_t                    *curitem;
    RECT                                    win_rect;
    __sicons_data_t                       *picons;
    __sicons_para_t                       *icons_priv;
    __u32                                   headitemno;
    H_LYR                                   h_lyr;
    FB                                      fb;
    icons_priv                       = (__sicons_para_t *)GUI_WinGetAttr(hicons);

    if ((!icons_priv) || (icons_priv->headitemno >= icons_priv->nitem) ||
        (icons_priv->curitemno >= icons_priv->nitem) || (icons_priv->headitemno > icons_priv->curitemno))
    {
        __wrn("input icons parameter error!\n");
        return  NULL;
    }

    picons   = (__sicons_data_t *)sicons_malloc(sizeof(__sicons_data_t));

    if (!picons)
    {
        __wrn("malloc icons priv data failed!\n");
        return NULL;
    }

    sicons_memset(picons, 0, sizeof(__sicons_data_t));
    picons->lastmsg                     = 0xffffffff;
    picons->handle                  = hicons;
    /*create iconlist of this listlayer*/
    picons->iconlist                = SIconList_Create();

    if (!picons->iconlist)
    {
        __msg("create iconlist failed!\n");
        goto  error;
    }

    picons->bk_color  = icons_priv->bk_color;
    picons->bk_bmp    = icons_priv->bk_bmp;
    picons->bk_draw   = icons_priv->bk_draw;

    if (icons_priv->nitem != 0)
    {
        GUI_WinGetWinFBRect(hicons, &win_rect);

        for (num = 0; num < icons_priv->nitem; num++)
        {
            /*IconItem Create*/
            iconitem                       = SIconItem_Create(&icons_priv->items[num], hicons);

            if (!iconitem)
            {
                __wrn("create iconitem %d failed!\n", num);
                goto    error;
            }

            /*IconItem Add to ListIcon*/
            SIconList_AddTail(picons->iconlist, iconitem);
        }

        picons->scnnum = win_rect.width / icons_priv->items[0].item_w;

        if (picons->scnnum == 0)
        {
            __wrn("input item width error!\n");
            goto error;
        }

        picons->iconlist->pagenum  = picons->scnnum;

        if (picons->scnnum > (picons->iconlist->ItemCount - icons_priv->headitemno))
        {
            if (picons->scnnum > picons->iconlist->ItemCount)
            {
                picons->valid_num = picons->iconlist->ItemCount;
                headitemno          = 0;
            }
            else
            {
                picons->valid_num = picons->scnnum;
                headitemno          = picons->iconlist->ItemCount - picons->scnnum;
            }
        }
        else
        {
            picons->valid_num = picons->scnnum;
            headitemno          = icons_priv->headitemno;
        }

        curitem = SIconList_GetIconitem(picons->iconlist, headitemno);

        if (!curitem)
        {
            curitem = picons->iconlist->head;
        }

        picons->iconlist->HeadItem  = curitem;
        picons->sitem_array = (h_iconssitem *)sicons_malloc(picons->scnnum * sizeof(h_iconssitem));

        for (num = 0; num < picons->scnnum; num++)
        {
            __icons_sitem_t           sitem_attr;
            __icons_sitem_t           *tmp_sitem;
            sitem_attr.iconitem         = curitem;
            sitem_attr.cb               = curitem->cb;
            sitem_attr.rect.x           = win_rect.x + num * curitem->item_w;
            sitem_attr.rect.y           = win_rect.y;
            sitem_attr.rect.width       = curitem->item_w;
            sitem_attr.rect.height      = win_rect.height;
            tmp_sitem                   = SShowItem_Create(&sitem_attr);
            picons->sitem_array[num]  = (h_iconssitem)tmp_sitem;

            if (num < picons->valid_num)
            {
                if (curitem->itemno == icons_priv->curitemno)
                {
                    picons->cur_index     = num;
                }
            }

            curitem                     = curitem->Next;
        }
    }

    picons->lyr_win = GUI_WinGetLyrWin(hicons);
    GUI_LyrWinGetFB(picons->lyr_win, &fb);
    picons->lyr_w     = fb.size.width;
    picons->lyr_h     = fb.size.height;
    picons->fb_addr   = fb.addr[0];

    switch (fb.fmt.fmt.rgb.pixelfmt)
    {
        case PIXEL_MONO_8BPP:
            picons->bpp = 1;
            break;

        case PIXEL_COLOR_RGB655:
        case PIXEL_COLOR_RGB565:
        case PIXEL_COLOR_RGB556:
        case PIXEL_COLOR_ARGB1555:
        case PIXEL_COLOR_RGBA5551:
            picons->bpp = 2;
            break;

        case PIXEL_COLOR_RGB0888:
            picons->bpp = 4;
            break;

        case PIXEL_COLOR_ARGB8888:
            picons->bpp = 4;
            break;

        default:
            picons->bpp = 4;
            break;
    }

    /*初始化icons状态*/
    picons->status                  = 0;
    return picons;
error:

    if (picons && picons->iconlist)
    {
        SIconList_Destroy(picons->iconlist);
    }

    if (picons)
    {
        sicons_free(picons);
    }

    return NULL;
}
//------------------------------------------------------------------------------------------------------------
//取得当前item

//==================================================================
//function name:    SIcons_GetCurItem
//author:
//date:             2008-10-13
//description:      icons getcuritem
//parameters:
//return:           if success return
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
h_iconsitem SIcons_GetCurItem(H_WIN icons)
{
    __sicons_data_t             *picons;
    __icons_iconitem_t          *piconitem;

    if (icons == NULL)
    {
        return NULL;
    }

    picons = (__sicons_data_t *)GUI_CtrlWinGetAddData(icons);

    if (picons->iconlist->ItemCount)
    {
        __u32  index;
        index = picons->iconlist->HeadItem->itemno + picons->cur_index;
        piconitem = SIconList_GetIconitem(picons->iconlist, index);
        return  SICONS_ItemP2H(piconitem);
    }
    else
    {
        return NULL;
    }
}

/*
**********************************************************************************************************************
*                                               SIcons_GetChild
*
* author:
*
* date:             2009-2-2
*
* Description:      gui lstm get child
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

h_iconsitem   SIcons_GetItem(H_WIN hicons, __u32 no)
{
    __sicons_data_t            *picons;
    __icons_iconitem_t         *tmpItem;

    if (hicons == NULL)
    {
        return NULL;
    }

    picons = (__sicons_data_t *)GUI_CtrlWinGetAddData(hicons);

    if (no > picons->iconlist->ItemCount)
    {
        __wrn("curitem no beyond the number of item!\n");
        return NULL;
    }
    else if (picons->iconlist->ItemCount == 0)
    {
        return NULL;
    }
    else
    {
        tmpItem = SIconList_GetIconitem(picons->iconlist, no);
        return  SICONS_ItemP2H(tmpItem);
    }
}

/*
**********************************************************************************************************************
*                                               SIcons_GetItemRect
*
* author:
*
* date:             2010-6-2
*
* Description:      sicons getitemrect
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32   SIcons_GetItemRect(h_iconsitem hitem, RECT *rect)
{
    __icons_iconitem_t    *pItem;
    __sicons_data_t       *pdata;
    __icons_sitem_t       *iconitem;
    __bool                   incontent;

    if (!hitem || !rect)
    {
        __wrn("imput parameter error!\n");
        return 0;
    }

    pItem   = SICONS_ItemH2P(hitem);
    pdata   = (__sicons_data_t *)GUI_CtrlWinGetAddData(pItem->handle);
    incontent = SIcons_InContent(pdata, pItem->itemno);

    if (incontent == EPDK_FALSE)
    {
        return EPDK_FAIL;
    }

    iconitem        = (__icons_sitem_t *)pdata->sitem_array[pItem->itemno - pdata->iconlist->HeadItem->itemno];
    rect->x         = iconitem->rect.x;
    rect->y         = iconitem->rect.y;
    rect->width     = iconitem->rect.width;
    rect->height    = iconitem->rect.height;
    return EPDK_OK;
}

//------------------------------------------------------------------------------------------------------------
//取得item的数目

//==================================================================
//function name:    SIcons_GetItemCount
//author:
//date:             2008-10-13
//description:      icons getitemcount
//parameters:
//return:           if success return
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32 SIcons_GetItemCount(H_WIN lstm)
{
    __sicons_data_t          *picons;

    if (lstm == NULL)
    {
        __wrn("set current no failed");
        return __LINE__;
    }

    /*获取icons变量*/
    picons = (__sicons_data_t *)GUI_CtrlWinGetAddData(lstm);

    if (picons == NULL)
    {
        return __LINE__;
    }

    return  SIconList_GetItemCount(picons->iconlist);
}

/*
**********************************************************************************************************************
*                                               SIcons_GetItemLayer
*
* author:
*
* date:             2009-10-13
*
* Description:      gui lstmgetitemlayer
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_LYR  SIcons_GetItemLyrWin(H_WIN H_Item)
{
    __icons_iconitem_t  *pItem;

    if (!H_Item)
    {
        __wrn("imput parameter error!\n");
        return NULL;
    }

    pItem = SICONS_ItemH2P(H_Item);
    return GUI_WinGetLyrWin(pItem->handle);
}


/*
**********************************************************************************************************************
*                                               SIcons_GetHostWin
*
* author:
*
* date:             2009-11-23
*
* Description:      gui lstmgethostwin
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN  SIcons_GetHostWin(H_WIN H_Item)
{
    __icons_iconitem_t  *pItem;

    if (!H_Item)
    {
        __wrn("imput parameter error!\n");
        return NULL;
    }

    pItem = SICONS_ItemH2P(H_Item);
    return pItem->handle;
}

/*
**********************************************************************************************************************
*                                               SIcons_GetItemAttr
*
* author:
*
* date:             2009-10-13
*
* Description:      gui lstmgetitemattr
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  SIcons_GetItemAttr(H_WIN H_Item)
{
    __icons_iconitem_t  *pItem;

    if (!H_Item)
    {
        __wrn("imput parameter error!\n");
        return 0;
    }

    pItem = SICONS_ItemH2P(H_Item);
    return (__u32)pItem->attr;
}

/*
**********************************************************************************************************************
*                                               SIcons_GetHeadindex
*
* author:
*
* date:             2010-6-2
*
* Description:      sicons getheadindex
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SIcons_GetHeadindex(H_WIN  hicons)
{
    __sicons_data_t            *picons;

    if (hicons == NULL)
    {
        return EPDK_FAIL;
    }

    picons = (__sicons_data_t *)GUI_CtrlWinGetAddData(hicons);
    return picons->iconlist->HeadItem->itemno;
}

/*
**********************************************************************************************************************
*                                               SIcons_SetCurItem
*
* author:
*
* date:             2010-6-2
*
* Description:      sicons setcuritem
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 SIcons_SetCurItem(H_WIN  hicons, __u32 cur_index)
{
    __u32                         num;
    __u32                         headitemno;
    __sicons_data_t            *picons;
    __icons_sitem_t            *psitem;
    __icons_sitem_t            *cur_sitem;
    __icons_iconitem_t         *curitem;

    if (hicons == 0)
    {
        return 0;
    }

    picons    = (__sicons_data_t *)GUI_CtrlWinGetAddData(hicons);

    if (picons->iconlist->ItemCount <= cur_index)
    {
        __wrn("input parameter error!\n");
        return EPDK_FAIL;
    }

    cur_sitem   = (__icons_sitem_t *)picons->sitem_array[picons->cur_index];

    for (num = 0; num < picons->valid_num; num++)
    {
        psitem = (__icons_sitem_t *)picons->sitem_array[num];

        if (psitem->iconitem->itemno == cur_index)
        {
            if (picons->cur_index != num)
            {
                if (psitem->iconitem->enable == EPDK_FALSE)
                {
                    SIcons_DisableIconItem(picons, cur_sitem, EPDK_TRUE);
                }
                else
                {
                    SIcons_DrawIconItem(picons, cur_sitem, EPDK_TRUE);
                }

                SIcons_FocusIconItem(picons, psitem, EPDK_TRUE);
                picons->cur_index = num;
            }

            return EPDK_OK;
        }
    }

    /*将以前的绘制状态去焦*/
    if (cur_sitem->iconitem->enable == EPDK_FALSE)
    {
        SIcons_DisableIconItem(picons, cur_sitem, EPDK_TRUE);
    }
    else
    {
        SIcons_DrawIconItem(picons, cur_sitem, EPDK_TRUE);
    }

    headitemno          = cur_index;

    if (picons->scnnum > (picons->iconlist->ItemCount - headitemno))
    {
        if (picons->scnnum > picons->iconlist->ItemCount)
        {
            picons->valid_num = picons->iconlist->ItemCount;
            headitemno          = 0;
        }
        else
        {
            picons->valid_num = picons->scnnum;
            headitemno          = picons->iconlist->ItemCount - picons->scnnum;
        }
    }
    else
    {
        picons->valid_num = picons->scnnum;
    }

    picons->cur_index     = cur_index - headitemno;
    curitem = SIconList_GetIconitem(picons->iconlist, headitemno);

    if (!curitem)
    {
        curitem = picons->iconlist->head;
    }

    picons->iconlist->HeadItem  = curitem;

    for (num = 0; num < picons->valid_num; num++)
    {
        psitem           = (__icons_sitem_t *)picons->sitem_array[num];
        psitem->iconitem = curitem;
        curitem          = curitem->Next;
    }

    SIcons_UpdateControl(picons);
    return EPDK_FAIL;
}

/*
**********************************************************************************************************************
*                                               SIcons_GetItemNo
*
* author:
*
* date:             2010-1-28
*
* Description:      gui lstmgetitemno
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  SIcons_GetItemNo(H_WIN H_Item)
{
    __icons_iconitem_t  *pItem;

    if (!H_Item)
    {
        __wrn("imput parameter error!\n");
        return 0;
    }

    pItem = SICONS_ItemH2P(H_Item);
    return (__u32)pItem->itemno;
}

//==================================================================
//function name:    SIcons_Destroy
//author:
//date:             2008-10-13
//description:      icons destroy
//parameters:
//return:           if success return
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32 SIcons_Destroy(H_WIN hicons)
{
    __u32                       num;
    __sicons_data_t           *picons;
    __icons_sitem_t           *sitem;

    if (hicons == NULL)
    {
        return EPDK_OK;
    }

    /*获取icons变量*/
    picons = (__sicons_data_t *)GUI_CtrlWinGetAddData(hicons);
    SIconList_Destroy(picons->iconlist);

    for (num = 0; num < picons->scnnum; num++)
    {
        sitem = (__icons_sitem_t *)picons->sitem_array[num];
        SShowItem_Destory(sitem);
    }

    sicons_free(picons->sitem_array);
    sicons_free(picons);
    picons = NULL;
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               SIcons_Wakeup
*
* author:
*
* date:             2010-5-22
*
* Description:      sicons wakeup
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 SIcons_Wakeup(H_WIN  hicons)
{
    SIcons_Ondraw(hicons);
    return EPDK_OK;
}


__u32  SIcons_Disable(H_WIN  hicons)
{
    __sicons_data_t         *picons;
    __icons_sitem_t            *psitem;

    if (hicons == NULL)
    {
        return EPDK_FAIL;
    }

    picons = (__sicons_data_t *)GUI_CtrlWinGetAddData(hicons);
    psitem = (__icons_sitem_t *)picons->sitem_array[picons->cur_index];
    SIcons_DrawIconItem(picons, psitem, EPDK_TRUE);
    return EPDK_OK;
}



/*
**********************************************************************************************************************
*                                               SIcons_KeyUp
*
* author:
*
* date:             2010-5-23
*
* Description:      sicons keyup
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  SIcons_KeyUp(__sicons_data_t *picons)
{
    __gui_notify_msg_t      notify_msg;
    SIcons_UpdateUp(picons);
    notify_msg.hWnd     = picons->handle;
    notify_msg.id       = GUI_WinGetItemId(picons->handle);
    notify_msg.msgcode  = ICONSN_PUSHED;
    notify_msg.dwAddData = GUI_MSG_KEY_UP;
    notify_msg.dwReserved   = picons->cur_index + picons->iconlist->HeadItem->itemno;
    picons->lastmsg = GUI_MSG_KEY_UP;
    GUI_NotifyParent(&notify_msg);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SIcons_KeyDown
*
* author:
*
* date:             2010-5-23
*
* Description:      sicons keydown
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  SIcons_KeyDown(__sicons_data_t *picons)
{
    __gui_notify_msg_t      notify_msg;
    SIcons_UpdateDown(picons);
    notify_msg.hWnd     = picons->handle;
    notify_msg.id       = GUI_WinGetItemId(picons->handle);
    notify_msg.msgcode  = ICONSN_PUSHED;
    notify_msg.dwAddData = GUI_MSG_KEY_DOWN;
    notify_msg.dwReserved   = picons->cur_index + picons->iconlist->HeadItem->itemno;
    picons->lastmsg = GUI_MSG_KEY_DOWN;
    GUI_NotifyParent(&notify_msg);
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               SIcons_KeyRise
*
* author:
*
* date:             2010-5-23
*
* Description:      sicons keyrise
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  SIcons_KeyRise(__sicons_data_t *picons, __u32 keyvalue)
{
    __gui_notify_msg_t      notify_msg;
    notify_msg.hWnd     = picons->handle;
    notify_msg.id       = GUI_WinGetItemId(picons->handle);
    notify_msg.msgcode  = ICONSN_UNPUSHED;
    notify_msg.dwAddData = picons->lastmsg;
    notify_msg.dwReserved   = picons->cur_index + picons->iconlist->HeadItem->itemno;
    picons->lastmsg = keyvalue;  //GUI_MSG_KEY_RISE,modified by Derek
    GUI_NotifyParent(&notify_msg);
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               SIcons_KeyDefault
*
* author:
*
* date:             2010-5-23
*
* Description:      sicons keydefault
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  SIcons_KeyDefault(__sicons_data_t *picons)
{
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               SIcons_SetFoucs
*
* author:
*
* date:             2010-5-23
*
* Description:      sicons setfoucs
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 SIcons_SetFoucs(__sicons_data_t *picons)
{
    __icons_sitem_t            *psitem;

    if (!picons)
    {
        return __LINE__;
    }

    psitem = (__icons_sitem_t *)picons->sitem_array[picons->cur_index];
    SIcons_FocusIconItem(picons, psitem, EPDK_TRUE);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SIcons_SetUnFoucs
*
* author:
*
* date:             2010-5-23
*
* Description:      sicons setunfoucs
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 SIcons_SetUnFoucs(__sicons_data_t *picons)
{
    __icons_sitem_t            *psitem;

    if (!picons)
    {
        return __LINE__;
    }

    psitem = (__icons_sitem_t *)picons->sitem_array[picons->cur_index];
    //    SIcons_DrawIconItem(picons,psitem,EPDK_TRUE);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               ListIconCtrlProc
*
* author:
*
* date:             2009-9-14
*
* Description:      iconsctrlproc
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32 SIconsCtrlProc(__gui_msg_t *msg)
{
    __sicons_data_t        *pData;
    __s32                    ret;

    switch (msg->id)
    {
        case  GUI_MSG_CREATE:
            pData = (__sicons_data_t *) SIcons_PrivCreate(msg->h_deswin);

            if (pData == NULL)
            {
                __wrn("create icons private failed");
                return EPDK_FAIL;
            }

            GUI_CtrlWinSetAddData(msg->h_deswin, (__u32)pData);
            return EPDK_OK;

        case  GUI_MSG_DESTROY:
            SIcons_Destroy(msg->h_deswin);
            return EPDK_OK;

        case  GUI_MSG_SET_FOCUS:
        {
            __sicons_data_t   *pData;
            pData = (__sicons_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);

            if (!(pData->status & ICONS_FOCUS))
            {
                /*处理设置焦点消息*/
                ret = SIcons_SetFoucs(pData);

                if (ret != EPDK_OK)
                {
                    __inf("set foucs error!\n");
                    return  EPDK_FAIL;
                }

                pData->status |= ICONS_FOCUS;
            }
        }

        return EPDK_OK;

        case  GUI_MSG_SET_UNFOCUS:
        {
            __sicons_data_t   *pData;
            pData = (__sicons_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);

            if (pData->status & ICONS_FOCUS)
            {
                /*处理设置焦点消息*/
                ret = SIcons_SetUnFoucs(pData);

                if (ret != EPDK_OK)
                {
                    __inf("set unfoucs error!\n");
                    return  EPDK_FAIL;
                }

                pData->status &= (~ICONS_FOCUS);
            }
        }

        return EPDK_OK;

        case  GUI_MSG_PAINT:
        {
            /*处理绘制消息*/
            ret = SIcons_Ondraw(msg->h_deswin);

            if (ret != EPDK_OK)
            {
                __inf("on pain error!\n");
                return  EPDK_FAIL;
            }
        }

        return EPDK_OK;

        case GUI_MSG_KEY:
        {
            __sicons_data_t   *pData;
            pData = (__sicons_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);

            /*获取键盘ID值*/
            switch (msg->dwAddData1)
            {
                case GUI_MSG_KEY_LEFT    :
                case GUI_MSG_KEY_LONGLEFT:
                    if (msg->dwAddData2 == KEY_UP_ACTION)
                    {
                        if (pData->iconlist->ItemCount)
                        {
                            SIcons_KeyRise(pData, msg->dwAddData1);
                        }
                    }

                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        if (pData->iconlist->ItemCount)
                        {
                            SIcons_KeyUp(pData);
                        }
                    }

                    return  EPDK_OK;

                case GUI_MSG_KEY_RIGHT:
                case GUI_MSG_KEY_LONGRIGHT:
                    if (msg->dwAddData2 == KEY_UP_ACTION)
                    {
                        if (pData->iconlist->ItemCount)
                        {
                            SIcons_KeyRise(pData, msg->dwAddData1);
                        }
                    }

                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        if (pData->iconlist->ItemCount)
                        {
                            SIcons_KeyDown(pData);
                        }
                    }

                    return EPDK_OK;

                /*case GUI_MSG_KEY_RISE:
                    if(pData->iconlist->ItemCount)
                    {
                        SIcons_KeyRise(pData);
                    }
                    return EPDK_OK;*/

                case GUI_MSG_KEY_ENTER:
                case GUI_MSG_KEY_DOWN:
                case GUI_MSG_KEY_UP:
                default:
                    if (msg->dwAddData2 == KEY_UP_ACTION)
                    {
                        if (pData->iconlist->ItemCount)
                        {
                            SIcons_KeyRise(pData, msg->dwAddData1);
                        }
                    }

                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        if (pData->iconlist->ItemCount)
                        {
                            __gui_notify_msg_t      notify_msg;
                            notify_msg.hWnd     = pData->handle;
                            notify_msg.id       = GUI_WinGetItemId(pData->handle);
                            notify_msg.msgcode  = ICONSN_PUSHED;
                            notify_msg.dwAddData = msg->dwAddData1;
                            notify_msg.dwReserved   = pData->cur_index + pData->iconlist->HeadItem->itemno;
                            pData->lastmsg = msg->dwAddData1;
                            GUI_NotifyParent(&notify_msg);
                        }
                    }

                    return EPDK_OK;
            }
        }

        case GUI_MSG_WIN_WAKEUP:
        {
            ret = SIcons_Wakeup(msg->h_deswin);

            if (ret != EPDK_OK)
            {
                __wrn("icons layer on failed!\n");
                return EPDK_FAIL;
            }
        }

        return EPDK_OK;

        /*iconitem上的子控件通知消息的处理函数*/
        case GUI_MSG_COMMAND:
        {
            __gui_msg_t     notifymsg;
            /*直接将子控件的通知码往icons控件的父窗口传递
            传递的通知码和id保持不变，这需要应用程序保证在icons中所有的控件的id，唯一*/
            notifymsg.h_srcwin  = msg->h_deswin;
            notifymsg.h_deswin  = GUI_WinGetParent(msg->h_deswin);
            notifymsg.id        = GUI_MSG_COMMAND;
            notifymsg.dwAddData1 = msg->dwAddData1;
            notifymsg.dwAddData2 = msg->dwAddData2;
            notifymsg.dwReserved = msg->dwReserved;
            notifymsg.p_arg     = NULL;
            //GUI_SendNotifyMessage (&notifymsg);
            GUI_SendMessage(&notifymsg);
        }

        return EPDK_OK;
    }

    return  EPDK_OK;
}

#endif //__LIST__MENU__H____
