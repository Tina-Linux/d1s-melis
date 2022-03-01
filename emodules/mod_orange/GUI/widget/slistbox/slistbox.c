/*
*******************************************************************************************************************
*                                                           Mod orange
*                                                   the embedded graphic support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：     listbox.c
*
*Author：        William Wang
*
*Version :       1.1.0
*
*Date:           2008-8-21
*
*Description :   listbox main function implement
*
*Others :        None at present.
*
* History :
*
* <Author>          <time>      <version>     <description>
*
* William Wang     2008-8-21      1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __SLISTBOX__C____
#define __SLISTBOX__C____

#include "slistbox_i.h"

/*定义删除时是重画上半部，还是重画下半部,或者直接清空(表示最后一项时)*/
#define  DEL_DELMOVEDOWN   0
#define  DEL_DELMOVEUP     1
#define  DEL_DELTAIL       2

static __s32 SListboxCtrlProc(__gui_msg_t  *msg);
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

__bool RegisterListBoxControl(void)
{
    __gui_winclass_t  WndClass;
    WndClass.spClassName = CTRL_SLISTBOX;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.WinProc     = SListboxCtrlProc;
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
* Description:      unregisterlistboxcontrol
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool  UnRegisterListBoxControl(void)
{
    return GUI_DeleteControlClass(CTRL_SLISTBOX) == EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SListbox_InContent
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

static __bool SListbox_InContent(__slistbox_data_t *pdata, __u32 index)
{
    __u32   head_index;
    __u32   tail_index;
    head_index  = pdata->menulist->HeadItem->itemno;
    tail_index  = head_index + pdata->valid_num - 1;

    if (head_index <= index && index <= tail_index)
    {
        return  EPDK_TRUE;
    }

    return EPDK_FALSE;
}

static __u32  SListbox_Memcpy(__slistbox_data_t *pdata, __rect_t *src_rect, __pos_t *dst_rect)
{
    GUI_RECT rect;
    void    *src_addr;
    src_addr = (void *)((unsigned long)pdata->fb_addr + (src_rect->y * pdata->lyr_w + src_rect->x) * pdata->bpp);
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

void SListbox_CtrlMemdevStart(__slistbox_data_t *plistbox)
{
#if 0
    RECT   fbrect;
    GUI_LyrWinSel(plistbox->lyr_win);
    GUI_WinGetClientFBRect(plistbox->handle, &fbrect);

    if (plistbox->hmem == NULL)
    {
        plistbox->hmem = GUI_MEMDEV_Create(fbrect.x, fbrect.y, fbrect.width, fbrect.height);
    }

    GUI_MEMDEV_Select(plistbox->hmem);
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

void SListbox_CtrlMemdevStop(__slistbox_data_t *plistbox)
{
#if 0

    if (plistbox->hmem)
    {
        GUI_MEMDEV_CopyToLCD(plistbox->hmem);            // copy from memory device to fb
        GUI_MEMDEV_Select(NULL);                      // stop memory device
        GUI_MEMDEV_Delete(plistbox->hmem);
        plistbox->hmem = NULL;
    }

    GUI_LyrWinFlushFB(GUI_WinGetLyrWin(plistbox->handle));
#endif
}

/*
**********************************************************************************************************************
*                                               SListbox_DrawMenuItem
*
* author:
*
* date:             2010-5-20
*
* Description:      slistbox drawmenuitem
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SListbox_DrawMenuItem(__slistbox_data_t *pdata, __listbox_sitem_t   *menuitem, __bool memdev_flags)
{
    __listitemdraw_para_t       drawpara;
    __u32                       ret = 0;

    if (!menuitem)
    {
        return OK;
    }

    /*set current draw mode*/
    drawpara.mode           =   LISTITEM_DRAWMODE_UNFCS;
    drawpara.hwin           =   SLISTBOX_ItemP2H(menuitem->menuitem);
    drawpara.memdev_flags   =   EPDK_TRUE;
    drawpara.attr           =   menuitem->menuitem->attr;
    drawpara.rect           =   menuitem->rect;
    ret = esKRNL_CallBack((__pCBK_t) * menuitem->cb, &drawpara);

    if (ret != LISTITEM_DRAWCB_ERROR)
    {
        return  EPDK_OK;
    }

    return EPDK_FAIL;
}


__s32  SListbox_DrawFreeMenuItem(__slistbox_data_t *pdata, __listbox_sitem_t   *menuitem, __bool memdev_flags)
{
    __listitemdraw_para_t       drawpara;
    __u32                       ret = 0;

    if (!menuitem)
    {
        return OK;
    }

    /*set current draw mode*/
    drawpara.mode           =   LISTITEM_DRAWMODE_FREE;
    drawpara.hwin           =   pdata->handle;
    drawpara.memdev_flags   =   EPDK_TRUE;
    drawpara.attr           =   NULL;
    drawpara.rect           =   menuitem->rect;
    ret = esKRNL_CallBack((__pCBK_t) * menuitem->cb, &drawpara);

    if (ret != LISTITEM_DRAWCB_ERROR)
    {
        return  EPDK_OK;
    }

    return EPDK_FAIL;
}

/*
**********************************************************************************************************************
*                                               SListbox_FocusMenuItem
*
* author:
*
* date:             2010-5-21
*
* Description:      slistbox focusmenuitem
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SListbox_FocusMenuItem(__slistbox_data_t *pdata, __listbox_sitem_t   *menuitem, __bool memdev_flags)
{
    __listitemdraw_para_t       drawpara;
    __u32                       ret = 0;

    if (!menuitem)
    {
        return OK;
    }

    /*set current draw mode*/
    drawpara.mode           =   LISTITEM_DRAWMODE_FCS;
    drawpara.hwin           =   SLISTBOX_ItemP2H(menuitem->menuitem);
    drawpara.memdev_flags   =   EPDK_TRUE;
    drawpara.attr           =   menuitem->menuitem->attr;
    drawpara.rect           =   menuitem->rect;
    ret = esKRNL_CallBack((__pCBK_t) * menuitem->cb, &drawpara);

    if (ret != LISTITEM_DRAWCB_ERROR)
    {
        return EPDK_OK;
    }

    return EPDK_FAIL;
}


/*
**********************************************************************************************************************
*                                               SListbox_DisableMenuItem
*
* author:
*
* date:             2010-5-21
*
* Description:      slistbox disablemenuitem
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SListbox_DisableMenuItem(__slistbox_data_t *pdata, __listbox_sitem_t   *menuitem, __bool memdev_flags)
{
    __listitemdraw_para_t       drawpara;
    __u32                       ret = 0;

    if (!menuitem)
    {
        return OK;
    }

    /*set current draw mode*/
    drawpara.mode           =   LISTITEM_DRAWMODE_DISABLE;
    drawpara.hwin           =   SLISTBOX_ItemP2H(menuitem->menuitem);
    drawpara.memdev_flags   =   EPDK_TRUE;
    drawpara.attr           =   menuitem->menuitem->attr;
    drawpara.rect           =   menuitem->rect;
    ret = esKRNL_CallBack((__pCBK_t) * menuitem->cb, &drawpara);

    if (ret != LISTITEM_DRAWCB_ERROR)
    {
        return EPDK_OK;
    }

    return EPDK_FAIL;
}

/*
**********************************************************************************************************************
*                                               SListbox_Ondraw
*
* author:
*
* date:             2010-5-20
*
* Description:      slistbox ondraw
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SListbox_Ondraw(H_WIN  hlistbox)
{
    __u32                         num;
    __slistbox_data_t            *plistbox;
    __listbox_sitem_t            *psitem;
    H_LYR                         h_lyr;
    RECT                          rect;

    if (hlistbox == NULL)
    {
        return EPDK_FAIL;
    }

    h_lyr    = GUI_WinGetLyrWin(hlistbox);
    GUI_WinGetWinFBRect(hlistbox, &rect);
    plistbox = (__slistbox_data_t *)GUI_CtrlWinGetAddData(hlistbox);
    GUI_LyrWinSel(h_lyr);
    SListbox_CtrlMemdevStart(plistbox);

    if (plistbox->bk_draw)
    {
        if (plistbox->bk_bmp)
        {
            GUI_BMP_Draw(plistbox->bk_bmp, 0, 0);
        }
        else
        {
            GUI_SetBkColor(plistbox->bk_color);
            GUI_ClearRect(rect.x, rect.y, rect.x + rect.width - 1, rect.y + rect.height - 1);
        }
    }

    for (num = 0; num < plistbox->valid_num; num++)
    {
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[num];

        if (!psitem->menuitem->enable)
        {
            SListbox_DisableMenuItem(plistbox, psitem, EPDK_FALSE);
        }
        else
        {
            SListbox_DrawMenuItem(plistbox, psitem, EPDK_FALSE);
        }
    }

    if (plistbox->valid_num < plistbox->scnnum)
    {
        for (num = plistbox->valid_num; num < plistbox->scnnum; num++)
        {
            psitem = (__listbox_sitem_t *)plistbox->sitem_array[num];
            SListbox_DrawFreeMenuItem(plistbox, psitem, EPDK_FALSE);
        }
    }

    SListbox_CtrlMemdevStop(plistbox);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SListbox_UpdateMenulist
*
* author:
*
* date:             2010-5-24
*
* Description:      slistbox updatemenulist
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SListbox_UpdateMenulist(H_WIN  hlistbox, __slistbox_para_t *para)
{
    __u32                         num;
    __u32                         headitemno;
    __slistbox_data_t            *plistbox;
    __listbox_menulist_t         *menulist;
    __listbox_menuitem_t         *menuitem;
    __listbox_sitem_t            *sitem;
    RECT                          win_rect;

    if ((para->headitemno >= para->nitem) ||
        (para->curitemno >= para->nitem) || (para->headitemno > para->curitemno))
    {
        __wrn("input listbox parameter error!");
        return  0;
    }

    if (hlistbox == NULL || para == NULL)
    {
        __wrn("input para error!");
        return EPDK_FAIL;
    }

    plistbox = (__slistbox_data_t *)GUI_CtrlWinGetAddData(hlistbox);
    menulist = plistbox->menulist;
    SMenuList_DelAll(menulist);
    plistbox->cur_index = 0;

    if (para->nitem != 0)
    {
        GUI_WinGetWinRECT(hlistbox, &win_rect);

        for (num = 0; num < para->nitem; num++)
        {
            /*MenuItem Create*/
            menuitem                       = SMenuItem_Create(&para->items[num], hlistbox);

            if (!menuitem)
            {
                __wrn("create menuitem %d failed!", num);
                goto    error;
            }

            /*MenuItem Add to ListMenu*/
            SMenuList_AddTail(plistbox->menulist, menuitem);
        }

        for (num = 0; num < plistbox->scnnum; num++)
        {
            sitem = (__listbox_sitem_t *)plistbox->sitem_array[num];
            SItem_Destory(sitem);
        }

        slistbox_free(plistbox->sitem_array);
        plistbox->scnnum = win_rect.height / para->items[0].item_w;

        if (plistbox->scnnum == 0)
        {
            __wrn("input item width error!");
            goto error;
        }

        plistbox->menulist->pagenum  = plistbox->scnnum;

        if (plistbox->scnnum > (plistbox->menulist->ItemCount - para->headitemno))
        {
            if (plistbox->scnnum > plistbox->menulist->ItemCount)
            {
                plistbox->valid_num = plistbox->menulist->ItemCount;
                headitemno          = 0;
            }
            else
            {
                plistbox->valid_num = plistbox->scnnum;
                headitemno          = plistbox->menulist->ItemCount - plistbox->scnnum;
            }
        }
        else
        {
            plistbox->valid_num = plistbox->scnnum;
            headitemno          = para->headitemno;
        }

        menuitem = SMenuList_GetMenuitem(plistbox->menulist, headitemno);

        if (!menuitem)
        {
            menuitem = plistbox->menulist->head;
        }

        plistbox->menulist->HeadItem  = menuitem;
        plistbox->sitem_array = (h_listboxsitem *)slistbox_malloc(plistbox->scnnum * sizeof(h_listboxsitem));

        for (num = 0; num < plistbox->scnnum; num++)
        {
            __listbox_sitem_t           sitem_attr;
            __listbox_sitem_t           *tmp_sitem;
            sitem_attr.menuitem         = menuitem;
            sitem_attr.cb               = menuitem->cb;
            sitem_attr.rect.x           = 0;
            sitem_attr.rect.y           = num * menuitem->item_w;
            sitem_attr.rect.width       = win_rect.width;
            sitem_attr.rect.height      = menuitem->item_w;
            tmp_sitem                   = SItem_Create(&sitem_attr);
            plistbox->sitem_array[num]  = (h_listboxsitem)tmp_sitem;

            if (num < plistbox->valid_num)
            {
                if (menuitem->itemno == para->curitemno)
                {
                    plistbox->cur_index     = num;
                }
            }

            menuitem                    = menuitem->Next;
        }
    }

    return EPDK_OK;
error:

    if (plistbox && plistbox->menulist)
    {
        SMenuList_DelAll(plistbox->menulist);
    }

    return EPDK_FAIL;
}


/*
**********************************************************************************************************************
*                                               SListbox_UpdateControl
*
* author:
*
* date:             2010-5-22
*
* Description:      slistbox updatecontrol
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 SListbox_UpdateControl(__slistbox_data_t *plistbox)
{
    __u32                         num;
    __listbox_sitem_t            *psitem;
    H_LYR                         h_lyr;
    RECT                          rect;
    h_lyr    = GUI_WinGetLyrWin(plistbox->handle);
    GUI_WinGetWinFBRect(plistbox->handle, &rect);
    plistbox = (__slistbox_data_t *)GUI_CtrlWinGetAddData(plistbox->handle);
    GUI_LyrWinSel(h_lyr);
    SListbox_CtrlMemdevStart(plistbox);

    if (plistbox->valid_num != plistbox->scnnum)
    {
        if (plistbox->bk_draw)
        {
            if (plistbox->bk_bmp)
            {
                GUI_BMP_Draw(plistbox->bk_bmp, 0, 0);
            }
            else
            {
                GUI_SetBkColor(plistbox->bk_color);
                GUI_ClearRect(rect.x, rect.y, rect.x + rect.width - 1, rect.y + rect.height - 1);
            }
        }
    }

    for (num = 0; num < plistbox->valid_num; num++)
    {
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[num];

        if (!psitem->menuitem->enable)
        {
            SListbox_DisableMenuItem(plistbox, psitem, EPDK_FALSE);
        }
        else if (num == plistbox->cur_index)
        {
            SListbox_FocusMenuItem(plistbox, psitem, EPDK_FALSE);
        }
        else
        {
            SListbox_DrawMenuItem(plistbox, psitem, EPDK_FALSE);
        }
    }

    if (plistbox->valid_num < plistbox->scnnum)
    {
        for (num = plistbox->valid_num; num < plistbox->scnnum; num++)
        {
            psitem = (__listbox_sitem_t *)plistbox->sitem_array[num];
            SListbox_DrawFreeMenuItem(plistbox, psitem, EPDK_FALSE);
        }
    }

    SListbox_CtrlMemdevStop(plistbox);
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               SListbox_UpdateUpEx
*
* author:
*
* date:             2010-6-11
*
* Description:      slistbox updateupex
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SListbox_UpdateUpEx(__slistbox_data_t *plistbox)
{
    __u32                         num;
    __listbox_sitem_t            *psitem;
    RECT                          src_rect;
    __pos_t                       dst_pos;
    plistbox = (__slistbox_data_t *)GUI_CtrlWinGetAddData(plistbox->handle);

    for (num = plistbox->valid_num - 1; num > 0; num--)
    {
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[num - 1];
        src_rect.x      = psitem->rect.x;
        src_rect.y      = psitem->rect.y;
        src_rect.width  = psitem->rect.width;
        src_rect.height = psitem->rect.height;
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[num];
        dst_pos.x       = psitem->rect.x;
        dst_pos.y       = psitem->rect.y;
        SListbox_Memcpy(plistbox, &src_rect, &dst_pos);
    }

    psitem = (__listbox_sitem_t *)plistbox->sitem_array[plistbox->cur_index];
    SListbox_FocusMenuItem(plistbox, psitem, EPDK_TRUE);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SListbox_UpdateDownEx
*
* author:
*
* date:             2010-6-11
*
* Description:      slistbox updatedownex
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SListbox_UpdateDownEx(__slistbox_data_t *plistbox)
{
    __u32                         num;
    __listbox_sitem_t            *psitem;
    RECT                          src_rect;
    __pos_t                       dst_pos;
    plistbox = (__slistbox_data_t *)GUI_CtrlWinGetAddData(plistbox->handle);

    for (num = 0; num < plistbox->valid_num - 1; num++)
    {
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[num + 1];
        src_rect.x      = psitem->rect.x;
        src_rect.y      = psitem->rect.y;
        src_rect.width  = psitem->rect.width;
        src_rect.height = psitem->rect.height;
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[num];
        dst_pos.x       = psitem->rect.x;
        dst_pos.y       = psitem->rect.y;
        SListbox_Memcpy(plistbox, &src_rect, &dst_pos);
    }

    psitem = (__listbox_sitem_t *)plistbox->sitem_array[plistbox->cur_index];
    SListbox_FocusMenuItem(plistbox, psitem, EPDK_TRUE);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SListbox_UpdateUp
*
* author:
*
* date:             2010-5-22
*
* Description:      slistbox updateup
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 SListbox_UpdateUp(__slistbox_data_t *plistbox)
{
    __u32                           num;
    __u32                           real_pagenum;
    __u32                           index;
    __listbox_sitem_t              *psitem;
    __listbox_menuitem_t           *head_item;
    __listbox_menuitem_t           *cur_item;
    __listbox_menuitem_t           *prev_item;
    psitem      = (__listbox_sitem_t *)plistbox->sitem_array[plistbox->cur_index];
    cur_item    = psitem->menuitem;
    prev_item   = SMenuList_FindPrevEnable(plistbox->menulist, cur_item);
    head_item   = plistbox->menulist->HeadItem;
    /*设置起始item，并返回当前item和起始item的偏差值*/
    index = SMenuList_SetHeadItemFromItem(plistbox->menulist, prev_item);

    if (plistbox->menulist->HeadItem == head_item)
    {
        /*将以前的绘制状态去焦*/
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[plistbox->cur_index];

        if (psitem->menuitem->enable == EPDK_FALSE)
        {
            SListbox_DisableMenuItem(plistbox, psitem, EPDK_TRUE);
        }
        else
        {
            SListbox_DrawMenuItem(plistbox, psitem, EPDK_TRUE);
        }

        /*重新设置焦点状态*/
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[index];
        SListbox_FocusMenuItem(plistbox, psitem, EPDK_TRUE);
        plistbox->cur_index = index;
    }
    else
    {
        cur_item            = plistbox->menulist->HeadItem;

        if (plistbox->menulist->HeadItem == head_item->Prev)
        {
            psitem = (__listbox_sitem_t *)plistbox->sitem_array[plistbox->cur_index];
            SListbox_DrawMenuItem(plistbox, psitem, EPDK_TRUE);
        }

        plistbox->cur_index = index;
        real_pagenum = SMenuList_GetCurPageNum(plistbox->menulist);

        for (num = 0; num < real_pagenum; num++)
        {
            psitem           = (__listbox_sitem_t *)plistbox->sitem_array[num];
            psitem->menuitem = cur_item;
            cur_item         = cur_item->Next;
        }

        plistbox->valid_num  = real_pagenum;

        if (plistbox->menulist->HeadItem != head_item->Prev)
        {
            SListbox_UpdateControl(plistbox);
        }
        else
        {
            SListbox_UpdateUpEx(plistbox);
        }
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SListbox_UpdateDown
*
* author:
*
* date:             2010-5-22
*
* Description:      slistbox updatedown
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 SListbox_UpdateDown(__slistbox_data_t *plistbox)
{
    __u32                           num;
    __u32                           real_pagenum;
    __u32                           index;
    __listbox_sitem_t              *psitem;
    __listbox_menuitem_t           *head_item;
    __listbox_menuitem_t           *cur_item;
    __listbox_menuitem_t           *next_item;
    psitem      = (__listbox_sitem_t *)plistbox->sitem_array[plistbox->cur_index];
    cur_item    = psitem->menuitem;
    next_item   = SMenuList_FindNextEnable(plistbox->menulist, cur_item);
    head_item   = plistbox->menulist->HeadItem;
    /*设置起始item，并返回当前item和起始item的偏差值*/
    index = SMenuList_SetHeadItemFromItem(plistbox->menulist, next_item);

    if (plistbox->menulist->HeadItem == head_item)
    {
        /*将以前的绘制状态去焦*/
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[plistbox->cur_index];

        if (psitem->menuitem->enable == EPDK_FALSE)
        {
            SListbox_DisableMenuItem(plistbox, psitem, EPDK_TRUE);
        }
        else
        {
            SListbox_DrawMenuItem(plistbox, psitem, EPDK_TRUE);
        }

        /*重新设置焦点状态*/
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[index];
        SListbox_FocusMenuItem(plistbox, psitem, EPDK_TRUE);
        plistbox->cur_index = index;
    }
    else
    {
        cur_item            = plistbox->menulist->HeadItem;

        if (plistbox->menulist->HeadItem == head_item->Next)
        {
            psitem = (__listbox_sitem_t *)plistbox->sitem_array[plistbox->cur_index];
            SListbox_DrawMenuItem(plistbox, psitem, EPDK_TRUE);
        }

        plistbox->cur_index = index;
        real_pagenum = SMenuList_GetCurPageNum(plistbox->menulist);

        for (num = 0; num < real_pagenum; num++)
        {
            psitem           = (__listbox_sitem_t *)plistbox->sitem_array[num];
            psitem->menuitem = cur_item;
            cur_item         = cur_item->Next;
        }

        plistbox->valid_num  = real_pagenum;

        if (plistbox->menulist->HeadItem != head_item->Next)
        {
            SListbox_UpdateControl(plistbox);
        }
        else
        {
            SListbox_UpdateDownEx(plistbox);
        }
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SListbox_AddTail
*
* author:
*
* date:             2010-5-20
*
* Description:      slistbox addtail
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SListbox_AddTail(H_WIN  hlistbox, __slistbox_item_attr_t *item_para)
{
    __slistbox_data_t            *plistbox;
    __listbox_menuitem_t         *tmpItem;
    __listbox_sitem_t            *tail_sitem;
    __u32                         num;

    if (hlistbox == NULL || item_para == NULL)
    {
        return EPDK_FAIL;
    }

    plistbox = (__slistbox_data_t *)GUI_CtrlWinGetAddData(hlistbox);
    tmpItem = SMenuItem_Create(item_para, hlistbox);

    if (!tmpItem)
    {
        __wrn("create menuitem failed!");
        return EPDK_FAIL;
    }

    SMenuList_AddTail(plistbox->menulist, tmpItem);

    if (plistbox->valid_num < plistbox->scnnum)
    {
        tail_sitem  = (__listbox_sitem_t *)plistbox->sitem_array[plistbox->valid_num];
        tail_sitem->menuitem = tmpItem;

        if (tmpItem->enable)
        {
            SListbox_DrawMenuItem(plistbox, tail_sitem, EPDK_TRUE);
        }
        else
        {
            SListbox_DisableMenuItem(plistbox, tail_sitem, EPDK_TRUE);
        }

        plistbox->valid_num++;
    }

    if (plistbox->valid_num < plistbox->scnnum)
    {
        for (num = plistbox->valid_num; num < plistbox->scnnum; num++)
        {
            tail_sitem = (__listbox_sitem_t *)plistbox->sitem_array[num];
            SListbox_DrawFreeMenuItem(plistbox, tail_sitem, EPDK_FALSE);
        }
    }

    return EPDK_OK;
}

//==================================================================
//function name:    SListbox_DelUpdateBottom
//author:
//date:             2014-6-24
//description:      slistbox delupdatebottom
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

static  __s32 SListbox_DelUpdateBottom(__slistbox_data_t *plistbox, __u32 count)
{
    __u32                         num;
    __listbox_sitem_t            *psitem;
    RECT                          src_rect;
    __pos_t                       dst_pos;
    psitem = (__listbox_sitem_t *)plistbox->sitem_array[plistbox->cur_index];
    SListbox_FocusMenuItem(plistbox, psitem, EPDK_TRUE);

    for (num = plistbox->cur_index + 1; num < count - 1; num++)
    {
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[num + 1];
        src_rect.x      = psitem->rect.x;
        src_rect.y      = psitem->rect.y;
        src_rect.width  = psitem->rect.width;
        src_rect.height = psitem->rect.height;
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[num];
        dst_pos.x       = psitem->rect.x;
        dst_pos.y       = psitem->rect.y;
        SListbox_Memcpy(plistbox, &src_rect, &dst_pos);
    }

    if (plistbox->valid_num < count)
    {
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[count - 1];
        SListbox_DrawFreeMenuItem(plistbox, psitem, EPDK_TRUE);
    }
    else
    {
        if (plistbox->cur_index != plistbox->valid_num - 1)
        {
            psitem = (__listbox_sitem_t *)plistbox->sitem_array[plistbox->valid_num - 1];

            if (!psitem->menuitem->enable)
            {
                SListbox_DisableMenuItem(plistbox, psitem, EPDK_FALSE);
            }
            else
            {
                SListbox_DrawMenuItem(plistbox, psitem, EPDK_FALSE);
            }
        }
    }

    return  EPDK_OK;
}

//==================================================================
//function name:    SListbox_DelUpdateUp
//author:
//date:             2014-6-24
//description:      slistbox delupdateup
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

static  __s32 SListbox_DelUpdateUp(__slistbox_data_t *plistbox)
{
    __s32                         num;
    __listbox_sitem_t            *psitem;
    RECT                          src_rect;
    __pos_t                       dst_pos;
    psitem = (__listbox_sitem_t *)plistbox->sitem_array[plistbox->cur_index];
    SListbox_FocusMenuItem(plistbox, psitem, EPDK_TRUE);

    for (num = plistbox->cur_index - 1; num > 0; num--)
    {
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[num - 1];
        src_rect.x      = psitem->rect.x;
        src_rect.y      = psitem->rect.y;
        src_rect.width  = psitem->rect.width;
        src_rect.height = psitem->rect.height;
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[num];
        dst_pos.x       = psitem->rect.x;
        dst_pos.y       = psitem->rect.y;
        SListbox_Memcpy(plistbox, &src_rect, &dst_pos);
    }

    if (plistbox->cur_index != 0)
    {
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[0];

        if (!psitem->menuitem->enable)
        {
            SListbox_DisableMenuItem(plistbox, psitem, EPDK_FALSE);
        }
        else
        {
            SListbox_DrawMenuItem(plistbox, psitem, EPDK_FALSE);
        }
    }

    return  EPDK_OK;
}

static  __s32 SListbox_DelTail(__slistbox_data_t *plistbox, __u32 tailidx)
{
    __listbox_sitem_t            *psitem;
    psitem = (__listbox_sitem_t *)plistbox->sitem_array[tailidx];
    SListbox_DrawFreeMenuItem(plistbox, psitem, EPDK_TRUE);

    if (tailidx > 0)
    {
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[tailidx - 1];
        SListbox_FocusMenuItem(plistbox, psitem, EPDK_TRUE);
    }

    return  EPDK_OK;
}

//==================================================================
//function name:    SListbox_DelSetHeadItem
//author:
//date:             2014-6-24
//description:      slistbox delsetheaditem
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

static  __s32 SListbox_DelSetHeadItem(__slistbox_data_t *plistbox, __listbox_menuitem_t *tmpItem)
{
    __u32                         ret;
    __u32                         count;
    __listbox_menuitem_t         *HeadItem;
    __listbox_menuitem_t         *headprev;
    __listbox_menuitem_t         *headnext;
    __u32                         real_pagenum;
    __u32                         num;
    __listbox_sitem_t            *psitem;
    __listbox_menuitem_t         *cur_item;
    HeadItem     = plistbox->menulist->HeadItem;

    /*获取前一个头指针*/
    if (plistbox->menulist->ItemCount == 1)
    {
        headprev = NULL;
        headnext = NULL;
    }
    else if (plistbox->menulist->HeadItem == plistbox->menulist->head)
    {
        headprev = NULL;
        headnext = plistbox->menulist->HeadItem->Next;
    }
    else if (plistbox->menulist->HeadItem == plistbox->menulist->tail)
    {
        headprev = plistbox->menulist->HeadItem->Prev;
        headnext = NULL;
    }
    else
    {
        headprev = plistbox->menulist->HeadItem->Prev;
        headnext = plistbox->menulist->HeadItem->Next;
    }

    ret = SMenuList_DelItem(plistbox->menulist, tmpItem);

    if (ret != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    if (plistbox->cur_index == 0)
    {
        /*先设定headitem为下个item*/
        tmpItem = headnext;

        if (tmpItem != NULL)
        {
            count   = plistbox->menulist->ItemCount - tmpItem->itemno;

            if (count < plistbox->scnnum)
            {
                if (tmpItem == plistbox->menulist->head)
                {
                    plistbox->menulist->HeadItem = tmpItem;
                    ret = DEL_DELMOVEDOWN;
                }
                else
                {
                    plistbox->menulist->HeadItem = headprev;
                    ret = DEL_DELMOVEUP;
                }
            }
            else
            {
                plistbox->menulist->HeadItem = headnext;
                ret = DEL_DELMOVEDOWN;
            }
        }
        else
        {
            ret = DEL_DELTAIL;
        }
    }/*删除最后一个*/
    else if (plistbox->cur_index == plistbox->valid_num - 1)
    {
        /*先设定headitem为当前*/
        tmpItem = HeadItem;
        count   = plistbox->menulist->ItemCount - tmpItem->itemno;

        if (count < plistbox->scnnum)
        {
            if (tmpItem == plistbox->menulist->head)
            {
                plistbox->menulist->HeadItem = tmpItem;

                if (plistbox->cur_index > 0)
                {
                    plistbox->cur_index -= 1;
                }

                ret = DEL_DELTAIL;
            }
            else
            {
                plistbox->menulist->HeadItem = headprev;
                ret = DEL_DELMOVEUP;
            }
        }
        else
        {
            plistbox->menulist->HeadItem = tmpItem;
            ret = DEL_DELMOVEDOWN;
        }
    }
    else
    {
        /*先设定headitem为当前*/
        tmpItem = HeadItem;
        count   = plistbox->menulist->ItemCount - tmpItem->itemno;

        if (count < plistbox->scnnum)
        {
            if (tmpItem == plistbox->menulist->head)
            {
                plistbox->menulist->HeadItem = tmpItem;
                ret = DEL_DELMOVEDOWN;
            }
            else
            {
                plistbox->menulist->HeadItem = headprev;
                ret = DEL_DELMOVEUP;
            }
        }
        else
        {
            plistbox->menulist->HeadItem = tmpItem;
            ret = DEL_DELMOVEDOWN;
        }
    }

    cur_item             = plistbox->menulist->HeadItem;
    real_pagenum         = SMenuList_GetCurPageNum(plistbox->menulist);

    for (num = 0; num < real_pagenum; num++)
    {
        psitem           = (__listbox_sitem_t *)plistbox->sitem_array[num];
        psitem->menuitem = cur_item;
        cur_item         = cur_item->Next;
    }

    plistbox->valid_num  = real_pagenum;
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

__s32   SListbox_DelCurItem(H_WIN hlistbox)
{
    __u32                         ret;
    __slistbox_data_t            *plistbox;
    __listbox_menuitem_t         *tmpItem;
    __u32                         count;
    __listbox_sitem_t            *tail_sitem;

    if (hlistbox == NULL)
    {
        return EPDK_FAIL;
    }

    plistbox = (__slistbox_data_t *)GUI_CtrlWinGetAddData(hlistbox);

    if (plistbox->menulist->ItemCount)
    {
        __u32  index;
        index = plistbox->menulist->HeadItem->itemno + plistbox->cur_index;
        tmpItem = SMenuList_GetMenuitem(plistbox->menulist, index);
    }
    else
    {
        return EPDK_OK;
    }

    count    = plistbox->valid_num;
    ret = SListbox_DelSetHeadItem(plistbox, tmpItem);

    if (ret == DEL_DELMOVEDOWN)
    {
        SListbox_DelUpdateBottom(plistbox, count);
    }
    else if (ret == DEL_DELMOVEUP)
    {
        SListbox_DelUpdateUp(plistbox);
    }
    else
    {
        SListbox_DelTail(plistbox, count - 1);
    }

    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               SListbox_ItemEnable
*
* author:
*
* date:             2010-5-20
*
* Description:      slistbox itemenable
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SListbox_ItemEnable(H_WIN  hlistbox, __u32 index)
{
    __slistbox_data_t            *plistbox;
    __listbox_menuitem_t         *tmpItem;
    __bool                        is_content;
    __listbox_sitem_t            *cur_sitem;

    if (hlistbox == NULL)
    {
        return EPDK_FAIL;
    }

    plistbox    = (__slistbox_data_t *)GUI_CtrlWinGetAddData(hlistbox);
    tmpItem     = SMenuList_GetMenuitem(plistbox->menulist, index);

    if (!tmpItem)
    {
        __wrn("input index error!");
        return EPDK_FAIL;
    }

    if (tmpItem->enable)
    {
        return EPDK_OK;
    }

    tmpItem->enable = EPDK_TRUE;
    is_content  = SListbox_InContent(plistbox, index);

    if (is_content)
    {
        cur_sitem = (__listbox_sitem_t *)plistbox->sitem_array[tmpItem->itemno - plistbox->menulist->HeadItem->itemno];
        SListbox_DrawMenuItem(plistbox, cur_sitem, EPDK_TRUE);
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SListbox_ItemDisable
*
* author:
*
* date:             2010-5-20
*
* Description:      slistbox itemdisable
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SListbox_ItemDisable(H_WIN  hlistbox, __u32 index)
{
    __slistbox_data_t            *plistbox;
    __listbox_menuitem_t         *tmpItem;
    __bool                        is_content;
    __listbox_sitem_t            *cur_sitem;

    if (hlistbox == NULL)
    {
        return EPDK_FAIL;
    }

    plistbox    = (__slistbox_data_t *)GUI_CtrlWinGetAddData(hlistbox);
    tmpItem     = SMenuList_GetMenuitem(plistbox->menulist, index);

    if (!tmpItem)
    {
        __wrn("input index error!");
        return EPDK_FAIL;
    }

    if (!tmpItem->enable)
    {
        return EPDK_OK;
    }

    tmpItem->enable = EPDK_FALSE;
    is_content  = SListbox_InContent(plistbox, index);

    if (is_content)
    {
        cur_sitem = (__listbox_sitem_t *)plistbox->sitem_array[tmpItem->itemno - plistbox->menulist->HeadItem->itemno];
        SListbox_DisableMenuItem(plistbox, cur_sitem, EPDK_TRUE);
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SListbox_NextPage
*
* author:
*
* date:             2010-5-22
*
* Description:      slistbox nextpage
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 SListbox_NextPage(H_WIN  hlistbox)
{
    __u32                         num;
    __u32                         next_head;
    __slistbox_data_t            *plistbox;
    __listbox_menuitem_t         *tmpItem;
    __listbox_sitem_t            *cur_sitem;

    if (hlistbox == NULL)
    {
        return EPDK_FAIL;
    }

    plistbox = (__slistbox_data_t *)GUI_CtrlWinGetAddData(hlistbox);
    next_head = plistbox->menulist->HeadItem->itemno + plistbox->scnnum;

    if (next_head >= plistbox->menulist->ItemCount)
    {
        return EPDK_FAIL;
    }

    if (next_head + plistbox->scnnum >= plistbox->menulist->ItemCount)
    {
        plistbox->valid_num = plistbox->menulist->ItemCount - next_head;
    }
    else
    {
        plistbox->valid_num = plistbox->scnnum;
    }

    tmpItem = SMenuList_GetMenuitem(plistbox->menulist, next_head);
    plistbox->menulist->HeadItem = tmpItem;
    tmpItem = plistbox->menulist->HeadItem;
    plistbox->cur_index = 0;

    for (num = 0; num < plistbox->valid_num; num++)
    {
        cur_sitem = (__listbox_sitem_t *)plistbox->sitem_array[num];
        cur_sitem->menuitem = tmpItem;
        tmpItem = tmpItem->Next;
    }

    SListbox_UpdateControl(plistbox);
    cur_sitem = (__listbox_sitem_t *)plistbox->sitem_array[0];
    SListbox_FocusMenuItem(plistbox, cur_sitem, EPDK_TRUE);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SListbox_PrevPage
*
* author:
*
* date:             2010-5-22
*
* Description:      slistbox prevpage
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 SListbox_PrevPage(H_WIN  hlistbox)
{
    __u32                         num;
    __s32                         prev_head;
    __slistbox_data_t            *plistbox;
    __listbox_menuitem_t         *tmpItem;
    __listbox_sitem_t            *cur_sitem;

    if (hlistbox == NULL)
    {
        return EPDK_FAIL;
    }

    plistbox = (__slistbox_data_t *)GUI_CtrlWinGetAddData(hlistbox);
    prev_head = plistbox->menulist->HeadItem->itemno - plistbox->scnnum;

    if (prev_head < 0)
    {
        return EPDK_FAIL;
    }

    plistbox->valid_num = plistbox->scnnum;
    tmpItem = SMenuList_GetMenuitem(plistbox->menulist, prev_head);
    plistbox->menulist->HeadItem = tmpItem;
    tmpItem = plistbox->menulist->HeadItem;
    plistbox->cur_index = 0;

    for (num = 0; num < plistbox->valid_num; num++)
    {
        cur_sitem = (__listbox_sitem_t *)plistbox->sitem_array[num];
        cur_sitem->menuitem = tmpItem;
        tmpItem = tmpItem->Next;
    }

    SListbox_UpdateControl(plistbox);
    cur_sitem = (__listbox_sitem_t *)plistbox->sitem_array[plistbox->cur_index];
    SListbox_FocusMenuItem(plistbox, cur_sitem, EPDK_TRUE);
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               SListbox_PrivCreate
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

__slistbox_data_t *SListbox_PrivCreate(H_WIN hlistbox)
{
    __u32                                   num;
    __listbox_menuitem_t                    *menuitem;
    __listbox_menuitem_t                    *curitem;
    RECT                                    win_rect;
    __slistbox_data_t                       *plistbox;
    __slistbox_para_t                       *listbox_priv;
    __u32                                   headitemno;
    H_LYR                                   h_lyr;
    FB                                      fb;
    listbox_priv                       = (__slistbox_para_t *)GUI_WinGetAttr(hlistbox);

    if ((!listbox_priv) || (listbox_priv->headitemno >= listbox_priv->nitem) ||
        (listbox_priv->curitemno >= listbox_priv->nitem) || (listbox_priv->headitemno > listbox_priv->curitemno))
    {
        __wrn("input listbox parameter error!");
        return  NULL;
    }

    plistbox   = (__slistbox_data_t *)slistbox_malloc(sizeof(__slistbox_data_t));

    if (!plistbox)
    {
        __wrn("malloc listbox priv data failed!");
        return NULL;
    }

    slistbox_memset(plistbox, 0, sizeof(__slistbox_data_t));
    plistbox->lastmsg                   = 0xffffffff;
    plistbox->handle                  = hlistbox;
    /*create menulist of this listlayer*/
    plistbox->menulist                = SMenuList_Create();

    if (!plistbox->menulist)
    {
        __msg("create menulist failed!");
        goto  error;
    }

    plistbox->bk_color  = listbox_priv->bk_color;
    plistbox->bk_bmp    = listbox_priv->bk_bmp;
    plistbox->bk_draw   = listbox_priv->bk_draw;

    if (listbox_priv->nitem != 0)
    {
        GUI_WinGetWinFBRect(hlistbox, &win_rect);

        for (num = 0; num < listbox_priv->nitem; num++)
        {
            /*MenuItem Create*/
            menuitem                       = SMenuItem_Create(&listbox_priv->items[num], hlistbox);

            if (!menuitem)
            {
                __wrn("create menuitem %d failed!", num);
                goto    error;
            }

            /*MenuItem Add to ListMenu*/
            SMenuList_AddTail(plistbox->menulist, menuitem);
        }

        plistbox->scnnum = win_rect.height / listbox_priv->items[0].item_w;

        if (plistbox->scnnum == 0)
        {
            __wrn("input item width error!");
            goto error;
        }

        plistbox->menulist->pagenum  = plistbox->scnnum;

        if (plistbox->scnnum > (plistbox->menulist->ItemCount - listbox_priv->headitemno))
        {
            if (plistbox->scnnum > plistbox->menulist->ItemCount)
            {
                plistbox->valid_num = plistbox->menulist->ItemCount;
                headitemno          = 0;
            }
            else
            {
                plistbox->valid_num = plistbox->scnnum;
                headitemno          = plistbox->menulist->ItemCount - plistbox->scnnum;
            }
        }
        else
        {
            plistbox->valid_num = plistbox->scnnum;
            headitemno          = listbox_priv->headitemno;
        }

        curitem = SMenuList_GetMenuitem(plistbox->menulist, headitemno);

        if (!curitem)
        {
            curitem = plistbox->menulist->head;
        }

        plistbox->menulist->HeadItem  = curitem;
        plistbox->sitem_array = (h_listboxsitem *)slistbox_malloc(plistbox->scnnum * sizeof(h_listboxsitem));

        for (num = 0; num < plistbox->scnnum; num++)
        {
            __listbox_sitem_t           sitem_attr;
            __listbox_sitem_t           *tmp_sitem;
            sitem_attr.menuitem         = curitem;
            sitem_attr.cb               = curitem->cb;
            sitem_attr.rect.x           = win_rect.x;
            sitem_attr.rect.y           = win_rect.y + num * curitem->item_w;
            sitem_attr.rect.width       = win_rect.width;
            sitem_attr.rect.height      = curitem->item_w;
            tmp_sitem                   = SItem_Create(&sitem_attr);
            plistbox->sitem_array[num]  = (h_listboxsitem)tmp_sitem;

            if (num < plistbox->valid_num)
            {
                if (curitem->itemno == listbox_priv->curitemno)
                {
                    plistbox->cur_index     = num;
                }
            }

            curitem                     = curitem->Next;
        }
    }

    plistbox->lyr_win = GUI_WinGetLyrWin(hlistbox);
    GUI_LyrWinGetFB(plistbox->lyr_win, &fb);
    plistbox->lyr_w     = fb.size.width;
    plistbox->lyr_h     = fb.size.height;
    plistbox->fb_addr   = fb.addr[0];

    switch (fb.fmt.fmt.rgb.pixelfmt)
    {
        case PIXEL_MONO_8BPP:
            plistbox->bpp = 1;
            break;

        case PIXEL_COLOR_RGB655:
        case PIXEL_COLOR_RGB565:
        case PIXEL_COLOR_RGB556:
        case PIXEL_COLOR_ARGB1555:
        case PIXEL_COLOR_RGBA5551:
            plistbox->bpp = 2;
            break;

        case PIXEL_COLOR_RGB0888:
            plistbox->bpp = 4;
            break;

        case PIXEL_COLOR_ARGB8888:
            plistbox->bpp = 4;
            break;

        default:
            plistbox->bpp = 4;
            break;
    }

    /*初始化listbox状态*/
    plistbox->status                  = 0;
    return plistbox;
error:

    if (plistbox && plistbox->menulist)
    {
        SMenuList_Destroy(plistbox->menulist);
    }

    if (plistbox)
    {
        slistbox_free(plistbox);
    }

    return NULL;
}
//------------------------------------------------------------------------------------------------------------
//取得当前item

//==================================================================
//function name:    SListbox_GetCurItem
//author:
//date:             2008-10-13
//description:      listbox getcuritem
//parameters:
//return:           if success return
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
h_listboxitem SListbox_GetCurItem(H_WIN listbox)
{
    __slistbox_data_t             *plistbox;
    __listbox_menuitem_t          *pmenuitem;

    if (listbox == NULL)
    {
        return NULL;
    }

    plistbox = (__slistbox_data_t *)GUI_CtrlWinGetAddData(listbox);

    if (plistbox->menulist->ItemCount)
    {
        __u32  index;
        index = plistbox->menulist->HeadItem->itemno + plistbox->cur_index;
        pmenuitem = SMenuList_GetMenuitem(plistbox->menulist, index);
        return  SLISTBOX_ItemP2H(pmenuitem);
    }
    else
    {
        return NULL;
    }
}

/*
**********************************************************************************************************************
*                                               SListbox_GetChild
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

h_listboxitem   SListbox_GetItem(H_WIN hlistbox, __u32 no)
{
    __slistbox_data_t            *plistbox;
    __listbox_menuitem_t         *tmpItem;

    if (hlistbox == NULL)
    {
        return NULL;
    }

    plistbox = (__slistbox_data_t *)GUI_CtrlWinGetAddData(hlistbox);

    if (no > plistbox->menulist->ItemCount)
    {
        __wrn("curitem no beyond the number of item!");
        return NULL;
    }
    else if (plistbox->menulist->ItemCount == 0)
    {
        return NULL;
    }
    else
    {
        tmpItem = SMenuList_GetMenuitem(plistbox->menulist, no);
        return  SLISTBOX_ItemP2H(tmpItem);
    }
}

/*
**********************************************************************************************************************
*                                               SListbox_GetItemRect
*
* author:
*
* date:             2010-6-2
*
* Description:      slistbox getitemrect
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32   SListbox_GetItemRect(h_listboxitem hitem, RECT *rect)
{
    __listbox_menuitem_t    *pItem;
    __slistbox_data_t       *pdata;
    __listbox_sitem_t       *menuitem;
    __bool                   incontent;

    if (!hitem || !rect)
    {
        __wrn("imput parameter error!");
        return 0;
    }

    pItem   = SLISTBOX_ItemH2P(hitem);
    pdata   = (__slistbox_data_t *)GUI_WinGetAddData(pItem->handle);
    incontent = SListbox_InContent(pdata, pItem->itemno);

    if (incontent == EPDK_FALSE)
    {
        return EPDK_FAIL;
    }

    menuitem        = (__listbox_sitem_t *)pdata->sitem_array[pItem->itemno - pdata->menulist->HeadItem->itemno];
    rect->x         = menuitem->rect.x;
    rect->y         = menuitem->rect.y;
    rect->width     = menuitem->rect.width;
    rect->height    = menuitem->rect.height;
    return EPDK_OK;
}

//------------------------------------------------------------------------------------------------------------
//取得item的数目

//==================================================================
//function name:    SListbox_GetItemCount
//author:
//date:             2008-10-13
//description:      listbox getitemcount
//parameters:
//return:           if success return
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32 SListbox_GetItemCount(H_WIN lstm)
{
    __slistbox_data_t          *plistbox;

    if (lstm == NULL)
    {
        __wrn("set current no failed");
        return __LINE__;
    }

    /*获取listbox变量*/
    plistbox = (__slistbox_data_t *)GUI_CtrlWinGetAddData(lstm);

    if (plistbox == NULL)
    {
        return __LINE__;
    }

    return  SMenuList_GetItemCount(plistbox->menulist);
}

/*
**********************************************************************************************************************
*                                               SListbox_GetItemLayer
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

H_LYR  SListbox_GetItemLyrWin(H_WIN H_Item)
{
    __listbox_menuitem_t  *pItem;

    if (!H_Item)
    {
        __wrn("imput parameter error!");
        return NULL;
    }

    pItem = SLISTBOX_ItemH2P(H_Item);
    return GUI_WinGetLyrWin(pItem->handle);
}


/*
**********************************************************************************************************************
*                                               SListbox_GetHostWin
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

H_WIN  SListbox_GetHostWin(H_WIN H_Item)
{
    __listbox_menuitem_t  *pItem;

    if (!H_Item)
    {
        __wrn("imput parameter error!");
        return NULL;
    }

    pItem = SLISTBOX_ItemH2P(H_Item);
    return pItem->handle;
}

/*
**********************************************************************************************************************
*                                               SListbox_GetItemAttr
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

unsigned long  SListbox_GetItemAttr(H_WIN H_Item)
{
    __listbox_menuitem_t  *pItem;

    if (!H_Item)
    {
        __wrn("imput parameter error!");
        return 0;
    }

    pItem = SLISTBOX_ItemH2P(H_Item);
    return (unsigned long)pItem->attr;
}

/*
**********************************************************************************************************************
*                                               SListbox_GetHeadindex
*
* author:
*
* date:             2010-6-2
*
* Description:      slistbox getheadindex
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SListbox_GetHeadindex(H_WIN  hlistbox)
{
    __slistbox_data_t            *plistbox;

    if (hlistbox == NULL)
    {
        return EPDK_FAIL;
    }

    plistbox = (__slistbox_data_t *)GUI_CtrlWinGetAddData(hlistbox);
    return plistbox->menulist->HeadItem->itemno;
}

/*
**********************************************************************************************************************
*                                               SListbox_SetCurItem
*
* author:
*
* date:             2010-6-2
*
* Description:      slistbox setcuritem
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 SListbox_SetCurItem(H_WIN  hlistbox, __u32 cur_index)
{
    __u32                         num;
    __u32                         headitemno;
    __slistbox_data_t            *plistbox;
    __listbox_sitem_t            *psitem;
    __listbox_sitem_t            *cur_sitem;
    __listbox_menuitem_t         *curitem;

    if (hlistbox == NULL)
    {
        return 0;
    }

    plistbox    = (__slistbox_data_t *)GUI_CtrlWinGetAddData(hlistbox);

    if (plistbox->menulist->ItemCount <= cur_index)
    {
        __wrn("input parameter error!");
        return EPDK_FAIL;
    }

    cur_sitem   = (__listbox_sitem_t *)plistbox->sitem_array[plistbox->cur_index];

    for (num = 0; num < plistbox->valid_num; num++)
    {
        psitem = (__listbox_sitem_t *)plistbox->sitem_array[num];

        if (psitem->menuitem->itemno == cur_index)
        {
            if (plistbox->cur_index != num)
            {
                if (psitem->menuitem->enable == EPDK_FALSE)
                {
                    SListbox_DisableMenuItem(plistbox, cur_sitem, EPDK_TRUE);
                }
                else
                {
                    SListbox_DrawMenuItem(plistbox, cur_sitem, EPDK_TRUE);
                }

                SListbox_FocusMenuItem(plistbox, psitem, EPDK_TRUE);
                plistbox->cur_index = num;
            }

            return EPDK_OK;
        }
    }

    /*将以前的绘制状态去焦*/
    if (cur_sitem->menuitem->enable == EPDK_FALSE)
    {
        SListbox_DisableMenuItem(plistbox, cur_sitem, EPDK_TRUE);
    }
    else
    {
        SListbox_DrawMenuItem(plistbox, cur_sitem, EPDK_TRUE);
    }

    headitemno          = cur_index;

    if (plistbox->scnnum > (plistbox->menulist->ItemCount - headitemno))
    {
        if (plistbox->scnnum > plistbox->menulist->ItemCount)
        {
            plistbox->valid_num = plistbox->menulist->ItemCount;
            headitemno          = 0;
        }
        else
        {
            plistbox->valid_num = plistbox->scnnum;
            headitemno          = plistbox->menulist->ItemCount - plistbox->scnnum;
        }
    }
    else
    {
        plistbox->valid_num = plistbox->scnnum;
    }

    plistbox->cur_index     = cur_index - headitemno;
    curitem = SMenuList_GetMenuitem(plistbox->menulist, headitemno);

    if (!curitem)
    {
        curitem = plistbox->menulist->head;
    }

    plistbox->menulist->HeadItem  = curitem;

    for (num = 0; num < plistbox->valid_num; num++)
    {
        psitem           = (__listbox_sitem_t *)plistbox->sitem_array[num];
        psitem->menuitem = curitem;
        curitem          = curitem->Next;
    }

    SListbox_UpdateControl(plistbox);
    return EPDK_FAIL;
}

/*
**********************************************************************************************************************
*                                               SListbox_GetItemNo
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

__u32  SListbox_GetItemNo(H_WIN H_Item)
{
    __listbox_menuitem_t  *pItem;

    if (!H_Item)
    {
        __wrn("imput parameter error!");
        return 0;
    }

    pItem = SLISTBOX_ItemH2P(H_Item);
    return (__u32)pItem->itemno;
}

//==================================================================
//function name:    SListbox_Destroy
//author:
//date:             2008-10-13
//description:      listbox destroy
//parameters:
//return:           if success return
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32 SListbox_Destroy(H_WIN hlistbox)
{
    __u32                       num;
    __slistbox_data_t           *plistbox;
    __listbox_sitem_t           *sitem;

    if (hlistbox == NULL)
    {
        return EPDK_OK;
    }

    /*获取listbox变量*/
    plistbox = (__slistbox_data_t *)GUI_CtrlWinGetAddData(hlistbox);
    SMenuList_Destroy(plistbox->menulist);

    for (num = 0; num < plistbox->scnnum; num++)
    {
        sitem = (__listbox_sitem_t *)plistbox->sitem_array[num];
        SItem_Destory(sitem);
    }

    slistbox_free(plistbox->sitem_array);
    slistbox_free(plistbox);
    plistbox = NULL;
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               SListbox_Wakeup
*
* author:
*
* date:             2010-5-22
*
* Description:      slistbox wakeup
*
* parameters:
*
* return:           if success return EPDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 SListbox_Wakeup(H_WIN  hlistbox)
{
    SListbox_Ondraw(hlistbox);
    return EPDK_OK;
}


__u32  SListbox_Disable(H_WIN  hlistbox)
{
    __slistbox_data_t           *plistbox;
    __listbox_sitem_t            *psitem;

    if (hlistbox == NULL)
    {
        return EPDK_FAIL;
    }

    plistbox = (__slistbox_data_t *)GUI_CtrlWinGetAddData(hlistbox);
    psitem = (__listbox_sitem_t *)plistbox->sitem_array[plistbox->cur_index];
    SListbox_DrawMenuItem(plistbox, psitem, EPDK_TRUE);
    return EPDK_OK;
}



/*
**********************************************************************************************************************
*                                               SListbox_KeyUp
*
* author:
*
* date:             2010-5-23
*
* Description:      slistbox keyup
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  SListbox_KeyUp(__slistbox_data_t *plistbox)
{
    __gui_notify_msg_t      notify_msg;
    SListbox_UpdateUp(plistbox);
    notify_msg.hWnd     = plistbox->handle;
    notify_msg.id       = GUI_WinGetItemId(plistbox->handle);
    notify_msg.msgcode  = LISTBOXN_PUSHED;
    notify_msg.dwAddData = GUI_MSG_KEY_UP;
    notify_msg.dwReserved   = plistbox->cur_index + plistbox->menulist->HeadItem->itemno;
    plistbox->lastmsg = GUI_MSG_KEY_UP;
    GUI_NotifyParent(&notify_msg);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SListbox_KeyDown
*
* author:
*
* date:             2010-5-23
*
* Description:      slistbox keydown
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  SListbox_KeyDown(__slistbox_data_t *plistbox)
{
    __gui_notify_msg_t      notify_msg;
    SListbox_UpdateDown(plistbox);
    notify_msg.hWnd     = plistbox->handle;
    notify_msg.id       = GUI_WinGetItemId(plistbox->handle);
    notify_msg.msgcode  = LISTBOXN_PUSHED;
    notify_msg.dwAddData = GUI_MSG_KEY_DOWN;
    notify_msg.dwReserved   = plistbox->cur_index + plistbox->menulist->HeadItem->itemno;
    plistbox->lastmsg = GUI_MSG_KEY_DOWN;
    GUI_NotifyParent(&notify_msg);
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               SListbox_KeyRise
*
* author:
*
* date:             2010-5-23
*
* Description:      slistbox keyrise
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  SListbox_KeyRise(__slistbox_data_t *plistbox, __u32 keyvalue)
{
    __gui_notify_msg_t      notify_msg;
    notify_msg.hWnd     = plistbox->handle;
    notify_msg.id       = GUI_WinGetItemId(plistbox->handle);
    notify_msg.msgcode  = LISTBOXN_UNPUSHED;
    notify_msg.dwAddData = plistbox->lastmsg;
    notify_msg.dwReserved   = plistbox->cur_index + plistbox->menulist->HeadItem->itemno;
    plistbox->lastmsg = keyvalue; //modified by Derek;
    GUI_NotifyParent(&notify_msg);
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               SListbox_KeyDefault
*
* author:
*
* date:             2010-5-23
*
* Description:      slistbox keydefault
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  SListbox_KeyDefault(__slistbox_data_t *plistbox)
{
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               SListbox_SetFoucs
*
* author:
*
* date:             2010-5-23
*
* Description:      slistbox setfoucs
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 SListbox_SetFoucs(__slistbox_data_t *plistbox)
{
    __listbox_sitem_t            *psitem;

    if (!plistbox)
    {
        return __LINE__;
    }

    psitem = (__listbox_sitem_t *)plistbox->sitem_array[plistbox->cur_index];
    SListbox_FocusMenuItem(plistbox, psitem, EPDK_TRUE);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               SListbox_SetUnFoucs
*
* author:
*
* date:             2010-5-23
*
* Description:      slistbox setunfoucs
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 SListbox_SetUnFoucs(__slistbox_data_t *plistbox)
{
    __listbox_sitem_t            *psitem;

    if (!plistbox)
    {
        return __LINE__;
    }

    psitem = (__listbox_sitem_t *)plistbox->sitem_array[plistbox->cur_index];
    //    SListbox_DrawMenuItem(plistbox,psitem,EPDK_TRUE);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               ListMenuCtrlProc
*
* author:
*
* date:             2009-9-14
*
* Description:      listboxctrlproc
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32 SListboxCtrlProc(__gui_msg_t *msg)
{
    __slistbox_data_t        *pData;
    __s32                    ret;

    switch (msg->id)
    {
        case  GUI_MSG_CREATE:
            pData = (__slistbox_data_t *) SListbox_PrivCreate(msg->h_deswin);

            if (pData == NULL)
            {
                __wrn("create listbox private failed");
                return EPDK_FAIL;
            }

            GUI_CtrlWinSetAddData(msg->h_deswin, (unsigned long)pData);
            return EPDK_OK;

        case  GUI_MSG_DESTROY:
            SListbox_Destroy(msg->h_deswin);
            return EPDK_OK;

        case  GUI_MSG_SET_FOCUS:
        {
            __slistbox_data_t   *pData;
            pData = (__slistbox_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);

            if (!(pData->status & LISTBOX_FOCUS))
            {
                /*处理设置焦点消息*/
                ret = SListbox_SetFoucs(pData);

                if (ret != EPDK_OK)
                {
                    __inf("set foucs error!");
                    return  EPDK_FAIL;
                }

                pData->status |= LISTBOX_FOCUS;
            }
        }

        return EPDK_OK;

        case  GUI_MSG_SET_UNFOCUS:
        {
            __slistbox_data_t   *pData;
            pData = (__slistbox_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);

            if (pData->status & LISTBOX_FOCUS)
            {
                /*处理设置焦点消息*/
                ret = SListbox_SetUnFoucs(pData);

                if (ret != EPDK_OK)
                {
                    __inf("set unfoucs error!");
                    return  EPDK_FAIL;
                }

                pData->status &= (~LISTBOX_FOCUS);
            }
        }

        return EPDK_OK;

        case  GUI_MSG_PAINT:
        {
            /*处理绘制消息*/
            ret = SListbox_Ondraw(msg->h_deswin);

            if (ret != EPDK_OK)
            {
                __inf("on pain error!");
                return  EPDK_FAIL;
            }
        }

        return EPDK_OK;

        case GUI_MSG_KEY:
        {
            __slistbox_data_t   *pData;
            pData = (__slistbox_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);

            /*获取键盘ID值*/
            switch (msg->dwAddData1)
            {
                case GUI_MSG_KEY_UP    :
                case GUI_MSG_KEY_LONGUP:
                    if (msg->dwAddData2 == KEY_UP_ACTION)
                    {
                        if (pData->menulist->ItemCount)
                        {
                            SListbox_KeyRise(pData, msg->dwAddData1);
                        }
                    }

                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        if (pData->menulist->ItemCount)
                        {
                            SListbox_KeyUp(pData);
                        }
                    }

                    return  EPDK_OK;

                case GUI_MSG_KEY_DOWN:
                case GUI_MSG_KEY_LONGDOWN:
                    if (msg->dwAddData2 == KEY_UP_ACTION)
                    {
                        if (pData->menulist->ItemCount)
                        {
                            SListbox_KeyRise(pData, msg->dwAddData1);
                        }
                    }

                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        if (pData->menulist->ItemCount)
                        {
                            SListbox_KeyDown(pData);
                        }
                    }

                    return EPDK_OK;

                /*case GUI_MSG_KEY_RISE:
                    if(pData->menulist->ItemCount)
                    {
                        SListbox_KeyRise(pData);
                    }
                    return EPDK_OK;*/

                case GUI_MSG_KEY_ENTER:
                case GUI_MSG_KEY_LEFT:
                case GUI_MSG_KEY_RIGHT:
                default:
                {
                    if (msg->dwAddData2 == KEY_UP_ACTION)
                    {
                        if (pData->menulist->ItemCount)
                        {
                            SListbox_KeyRise(pData, msg->dwAddData1);
                        }
                    }

                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        if (pData->menulist->ItemCount)
                        {
                            __gui_notify_msg_t      notify_msg;
                            notify_msg.hWnd     = pData->handle;
                            notify_msg.id       = GUI_WinGetItemId(pData->handle);
                            notify_msg.msgcode  = LISTBOXN_PUSHED;
                            notify_msg.dwAddData = msg->dwAddData1;
                            notify_msg.dwReserved   = pData->cur_index + pData->menulist->HeadItem->itemno;
                            pData->lastmsg = msg->dwAddData1;
                            GUI_NotifyParent(&notify_msg);
                        }
                    }
                }

                return EPDK_OK;
            }
        }

        case GUI_MSG_WIN_WAKEUP:
        {
            ret = SListbox_Wakeup(msg->h_deswin);

            if (ret != EPDK_OK)
            {
                __wrn("listbox layer on failed!");
                return EPDK_FAIL;
            }
        }

        return EPDK_OK;

        /*menuitem上的子控件通知消息的处理函数*/
        case GUI_MSG_COMMAND:
        {
            __gui_msg_t     notifymsg;
            /*直接将子控件的通知码往listbox控件的父窗口传递
            传递的通知码和id保持不变，这需要应用程序保证在listbox中所有的控件的id，唯一*/
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
