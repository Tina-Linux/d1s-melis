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
#ifndef __LIST_BOX__C____
#define __LIST_BOX__C____


#include "config.h"
#include "ListMenu_i.h"
#include "ListBox.h"
#include "MenuList.h"
#include "MenuItem.h"
#include "sysmemory.h"
#include "ListLayer_Process.h"


/*
**********************************************************************************************************************
*                                               ListLayer_SysCopy
*
* author:
*
* date:             2014-7-21
*
* Description:      listlayer syscopy
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 ListLayer_HeadToTailCopy(LIST_LAYER_MAN_t *listlayer)
{
    __u32       i;
    __u32       width;
    __u32       height;
    __u32       fb_width;
    void       *src_addr;
    void       *dst_addr;

    if (listlayer->dir == GUI_SCNDIR_NORMAL)
    {
        sysmemcpy(listlayer->listbuffer->ebuffer, listlayer->listbuffer->buffer,
                  listlayer->listbuffer->esize);
    }
    else if (listlayer->dir == GUI_SCNDIR_ROTATE90)
    {
        src_addr = listlayer->listbuffer->buffer;
        dst_addr = (void *)((unsigned long)listlayer->listbuffer->buffer +
                            ((listlayer->listbuffer->vrect.height * listlayer->menulist->bpp + 7) >> 3));
        width    = (listlayer->viewrect.height * listlayer->menulist->bpp + 7) >> 3;
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
        sysmemcpy(listlayer->listbuffer->buffer, listlayer->listbuffer->ebuffer,
                  listlayer->listbuffer->esize);
    }
    else
    {
        dst_addr = listlayer->listbuffer->buffer;
        src_addr = (void *)((unsigned long)listlayer->listbuffer->buffer +
                            ((listlayer->listbuffer->vrect.height * listlayer->menulist->bpp + 7) >> 3));
        width    = (listlayer->viewrect.height * listlayer->menulist->bpp + 7) >> 3;
        height   = listlayer->listbuffer->rectsz.width;
        fb_width = (listlayer->lyr_size.height * listlayer->menulist->bpp + 7) >> 3;

        for (i = 0; i < height; i++)
        {
            sysmemcpy(dst_addr, src_addr, width);
            src_addr = (void *)((unsigned long)src_addr + fb_width);
            dst_addr = (void *)((unsigned long)dst_addr + fb_width);
        }
    }

    return  EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               ListLayer_TailToHeadCopy
*
* author:
*
* date:             2014-7-21
*
* Description:      listlayer tailtoheadcopy
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 ListLayer_TailToHeadCopy(LIST_LAYER_MAN_t *listlayer)
{
    __u32       i;
    __u32       width;
    __u32       height;
    __u32       fb_width;
    void       *src_addr;
    void       *dst_addr;

    if (listlayer->dir == GUI_SCNDIR_NORMAL)
    {
        sysmemcpy(listlayer->listbuffer->buffer, listlayer->listbuffer->ebuffer,
                  listlayer->listbuffer->esize);
    }
    else if (listlayer->dir == GUI_SCNDIR_ROTATE90)
    {
        dst_addr = listlayer->listbuffer->buffer;
        src_addr = (void *)((unsigned long)listlayer->listbuffer->buffer +
                            ((listlayer->listbuffer->vrect.height * listlayer->menulist->bpp + 7) >> 3));
        width    = (listlayer->viewrect.height * listlayer->menulist->bpp + 7) >> 3;
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
        sysmemcpy(listlayer->listbuffer->ebuffer, listlayer->listbuffer->buffer,
                  listlayer->listbuffer->esize);
    }
    else
    {
        src_addr = listlayer->listbuffer->buffer;
        dst_addr = (void *)((unsigned long)listlayer->listbuffer->buffer +
                            ((listlayer->listbuffer->vrect.height * listlayer->menulist->bpp + 7) >> 3));
        width    = (listlayer->viewrect.height * listlayer->menulist->bpp + 7) >> 3;
        height   = listlayer->listbuffer->rectsz.width;
        fb_width = (listlayer->lyr_size.height * listlayer->menulist->bpp + 7) >> 3;

        for (i = 0; i < height; i++)
        {
            sysmemcpy(dst_addr, src_addr, width);
            src_addr = (void *)((unsigned long)src_addr + fb_width);
            dst_addr = (void *)((unsigned long)dst_addr + fb_width);
        }
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_BoxDeleteMoveDown
*
* author:
*
* date:             2014-7-21
*
* Description:      listlayer boxdeletemovedown
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListLayer_BoxDeleteMoveDown(LIST_LAYER_MAN_t *listlayer, __u32 dist)
{
    __u8                       err;
    __u32                      ret;
    __rect_t                   sw;
    __u32                      count;
    __move_para_t              dist_dly;
    count = listlayer->menulist->tail->itemno - listlayer->menulist->HeadItem->itemno + 1;

    if (listlayer->menulist->HeadItem != listlayer->menulist->head
        && count < listlayer->scnnum)
    {
        sw.x = listlayer->viewrect.x;
        sw.y = listlayer->viewrect.y;
        /*需要上移区域*/
        listlayer->menulist->HeadItem   = listlayer->menulist->HeadItem->Prev;
        listlayer->menulist->SelItem    = listlayer->menulist->HeadItem;
        listlayer->menulist->dist       -= listlayer->menulist->CurItem->rect.height;
        dist_dly.dist                   = dist;
        sw.y = sw.y - dist_dly.dist;

        if (sw.y < 0)
        {
            sw.y = listlayer->listbuffer->vrect.height + sw.y;
        }
        else if (sw.y > listlayer->listbuffer->vrect.height)
        {
            sw.y = sw.y - listlayer->listbuffer->vrect.height;
        }

        ret = ListLayer_SwitchFB(listlayer, 0, sw.x, sw.y);

        if (ret != EPDK_OK)
        {
            __wrn("layer close failed,layer handle = %d\n", listlayer->layer);
            return __LINE__;
        }

        esKRNL_SemPend(listlayer->pos_sem, 0, &err); /*pend pos semphore*/

        if (err == OS_NO_ERR)
        {
            listlayer->viewrect.x = sw.x;
            listlayer->viewrect.y = sw.y;
        }

        esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/
        esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_LOAD_FLAG, OS_FLAG_SET, &err);
        esKRNL_TimeDly(2);
    }
    else
    {
        esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_LOAD_FLAG, OS_FLAG_SET, &err);
        esKRNL_TimeDly(2);
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_UpdateHead
*
* author:
*
* date:             2009-1-30
*
* Description:      listlayer update head  删除item后更新当前Item的上半部
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static  __u32  ListLayer_BoxDelUpdateHead(LIST_LAYER_MAN_t    *listlayer)
{
    __pos_t              cur_pos;
    MENUITEM_t          *tmpItem;
    /*从焦点位置开始绘制*/
    cur_pos.x               = listlayer->foucs_pos.x;
    cur_pos.y               = listlayer->foucs_pos.y;
    tmpItem                 = listlayer->menulist->CurItem;

    if (tmpItem)
    {
        ListLayer_ClearMenuItem(listlayer, 0, cur_pos.x, cur_pos.y);
        listlayer->foucs_pos.x  = cur_pos.x;
        listlayer->foucs_pos.y  = cur_pos.y - listlayer->menulist->head->rect.height;
        listlayer->tail_pos.x = cur_pos.x;
        listlayer->tail_pos.y = cur_pos.y;
    }
    else
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
        listlayer->menulist->load_tail  = NULL;
        listlayer->menulist->load_head  = NULL;
        listlayer->tail_pos.x           = 0;
        listlayer->tail_pos.y           = 0;
    }

    return OK;
}


/*
**********************************************************************************************************************
*                                               ListLayer_BoxDelUpdateHeadItem
*
* author:
*
* date:             2009-11-23
*
* Description:      listlayer boxdelupdateheaditem
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static  __u32  ListLayer_BoxDelUpdateHeadItem(LIST_LAYER_MAN_t    *listlayer)
{
    __u32                count;
    __u32                i;
    __pos_t              cur_pos;
    MENUITEM_t          *tmpItem;
    /*从焦点位置开始绘制*/
    cur_pos.x           = listlayer->foucs_pos.x;
    cur_pos.y           = listlayer->foucs_pos.y;
    tmpItem             = listlayer->menulist->CurItem;
    /*绘制数量*/
    count               = (1 + listlayer->scnnum * (listlayer->fb_depth / 2 - 1)) - 1;

    for (i = 0; i < count; i++)
    {
        /*需要绘制的item不是最后一个item*/
        if (tmpItem != NULL && tmpItem != listlayer->menulist->tail)
        {
            ListLayer_FastLoadMenuItem(listlayer, tmpItem, 0, cur_pos.x, cur_pos.y);
            tmpItem     = tmpItem->Prev;
            cur_pos.x   = cur_pos.x;
            cur_pos.y   = cur_pos.y - listlayer->menulist->head->rect.height;

            if (cur_pos.y < 0)
            {
                ListLayer_HeadToTailCopy(listlayer);
                cur_pos.y = cur_pos.y + listlayer->listbuffer->vrect.height;
            }

            listlayer->menulist->load_head  = tmpItem;
            listlayer->head_pos.x           = cur_pos.x;
            listlayer->head_pos.y           = cur_pos.y;
        }
        else
        {
            /*所有的item都已经删除完成，清除framebuffer*/
            if (tmpItem == NULL)
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
                listlayer->menulist->load_tail  = NULL;
                listlayer->menulist->load_head  = NULL;
                listlayer->tail_pos.x           = 0;
                listlayer->tail_pos.y           = 0;
                break;
            }
            else if (i == 0)
            {
                ListLayer_FastLoadMenuItem(listlayer, tmpItem, 0, cur_pos.x, cur_pos.y);
                tmpItem = tmpItem->Prev;
                cur_pos.x = cur_pos.x;
                cur_pos.y = cur_pos.y - listlayer->menulist->head->rect.height;

                if (cur_pos.y < 0)
                {
                    ListLayer_HeadToTailCopy(listlayer);
                    cur_pos.y = cur_pos.y + listlayer->listbuffer->vrect.height;
                }

                listlayer->menulist->load_head = tmpItem;
                listlayer->head_pos.x = cur_pos.x;
                listlayer->head_pos.y = cur_pos.y;
            }
            else
            {
                break;
            }
        }
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_UpdateDraw
*
* author:
*
* date:             2009-1-30
*
* Description:      listlayer update draw (load item to tail)删除item后更新当前Item的下半部
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __u32 ListLayer_BoxDelUpdateTail(LIST_LAYER_MAN_t    *listlayer)
{
    __u32                tmp;
    __u32                count;
    __u32                i;
    __pos_t              cur_pos;
    MENUITEM_t          *tmpItem;
    cur_pos.x           = listlayer->foucs_pos.x;
    cur_pos.y           = listlayer->foucs_pos.y;
    tmp                 = listlayer->menulist->CurItem->itemno - listlayer->menulist->HeadItem->itemno;
    count               = listlayer->scnnum - tmp + listlayer->scnnum * (listlayer->fb_depth / 2 - 1);
    tmpItem             = listlayer->menulist->CurItem;

    for (i = 0; i < count; i++)
    {
        if (tmpItem != NULL && tmpItem != listlayer->menulist->head)
        {
            ListLayer_FastLoadMenuItem(listlayer, tmpItem, 0, cur_pos.x, cur_pos.y);
            tmpItem = tmpItem->Next;
            cur_pos.x = cur_pos.x;
            cur_pos.y = cur_pos.y + listlayer->menulist->head->rect.height;

            if (cur_pos.y >= listlayer->listbuffer->rectsz.height)
            {
                sysmemcpy(listlayer->listbuffer->buffer, listlayer->listbuffer->ebuffer,
                          listlayer->listbuffer->esize);
                cur_pos.y = cur_pos.y - listlayer->listbuffer->vrect.height;
            }

            listlayer->menulist->load_tail  = tmpItem;
            listlayer->tail_pos.x           = cur_pos.x;
            listlayer->tail_pos.y           = cur_pos.y;
        }
        else
        {
            if (tmpItem == NULL)
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
                listlayer->menulist->load_tail  = NULL;
                listlayer->menulist->load_head  = NULL;
                listlayer->tail_pos.x           = 0;
                listlayer->tail_pos.y           = 0;
                break;
            }
            else if (i == 0)
            {
                ListLayer_FastLoadMenuItem(listlayer, tmpItem, 0, cur_pos.x, cur_pos.y);
                tmpItem = tmpItem->Next;
                cur_pos.x = cur_pos.x;
                cur_pos.y = cur_pos.y + listlayer->menulist->head->rect.height;

                if (cur_pos.y >= listlayer->listbuffer->rectsz.height)
                {
                    sysmemcpy(listlayer->listbuffer->buffer, listlayer->listbuffer->ebuffer,
                              listlayer->listbuffer->esize);
                    cur_pos.y = cur_pos.y - listlayer->listbuffer->vrect.height;
                }

                listlayer->menulist->load_tail = tmpItem;
                listlayer->tail_pos.x = cur_pos.x;
                listlayer->tail_pos.y = cur_pos.y;
            }
            else
            {
                if (i >= listlayer->scnnum - tmp)
                {
                    break;
                }

                listlayer->menulist->load_tail = listlayer->menulist->head;
                ListLayer_ClearMenuItem(listlayer, 0, cur_pos.x, cur_pos.y);
                cur_pos.x = cur_pos.x;
                cur_pos.y = cur_pos.y + tmpItem->rect.height;

                if (cur_pos.y >= listlayer->listbuffer->rectsz.height)
                {
                    sysmemcpy(listlayer->listbuffer->buffer, listlayer->listbuffer->ebuffer,
                              listlayer->listbuffer->esize);
                    cur_pos.y = cur_pos.y - listlayer->listbuffer->vrect.height;
                }
            }
        }
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_DelCurItem
*
* author:
*
* date:             2009-1-30
*
* Description:      listlayer delcuritem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListLayer_BoxDelCurItem(LIST_LAYER_MAN_t    *listlayer)
{
    __u32                ret;
    MENUITEM_t          *NewCurItem;
    __bool               isHead = EPDK_FALSE;

    if (listlayer == NULL)
    {
        return __LINE__;
    }

    if (listlayer->menulist->CurItem == NULL)
    {
        return OK;
    }

    if (listlayer->mode == FIXED_MODE)
    {
        return OK;
    }

    /*update head information,需要删除的item是尾item，此时需要更新上半部*/
    if (listlayer->menulist->CurItem == listlayer->menulist->tail)
    {
        NewCurItem = listlayer->menulist->CurItem->Prev;

        if (NewCurItem == listlayer->menulist->CurItem)
        {
            NewCurItem = NULL;
        }

        if (listlayer->menulist->CurItem == listlayer->menulist->HeadItem)
        {
            listlayer->menulist->HeadItem   = NewCurItem;
            isHead                          = EPDK_TRUE;
        }

        if (listlayer->menulist->CurItem == listlayer->menulist->SelItem)
        {
            listlayer->menulist->SelItem   = NewCurItem;
        }

        if (listlayer->menulist->load_head == listlayer->menulist->tail)
        {
            listlayer->menulist->load_head  = NewCurItem;
        }

        ret = MenuList_DelItem(listlayer->menulist, listlayer->menulist->CurItem);

        if (ret != OK)
        {
            return __LINE__;
        }

        if (NewCurItem != NULL)
        {
            listlayer->menulist->CurItem = NewCurItem;

            if (isHead)
            {
                ListLayer_BoxDelUpdateHeadItem(listlayer);
            }
            else
            {
                ListLayer_BoxDelUpdateHead(listlayer);
            }

            //LISTMENU_PRINTF("foucs x = %d,foucs y = %d\n",listlayer->foucs_pos.x,listlayer->foucs_pos.y);
            ret = ListLayer_DrawFastFoucsMenuItem(listlayer, listlayer->menulist->CurItem,
                                                  0, listlayer->foucs_pos.x, listlayer->foucs_pos.y);

            if (ret != OK)
            {
                return __LINE__;
            }

            //LISTMENU_PRINTF("foucs x = %d,foucs y = %d\n",listlayer->foucs_pos.x,listlayer->foucs_pos.y);
            ret = ListLayer_LoadMenuItem(listlayer, listlayer->menulist->CurItem,
                                         0, listlayer->foucs_pos.x, listlayer->foucs_pos.y);

            if (ret != OK)
            {
                return __LINE__;
            }

            listlayer->setfoucs = REAL_FOUCS;
        }
        else
        {
            listlayer->menulist->CurItem = NewCurItem;
            ListLayer_BoxDelUpdateHead(listlayer);
        }
    }
    else  /*update tail information，需要更新不是最后一个item，此时需要更新下半部*/
    {
        /*获取新item的窗口句柄*/
        NewCurItem = listlayer->menulist->CurItem->Next;

        /*只有一个item的情况*/
        if (NewCurItem == listlayer->menulist->CurItem)
        {
            NewCurItem = NULL;
        }

        if (listlayer->menulist->CurItem == listlayer->menulist->HeadItem)
        {
            listlayer->menulist->HeadItem = NewCurItem;
        }

        if (listlayer->menulist->CurItem == listlayer->menulist->SelItem)
        {
            listlayer->menulist->SelItem   = NewCurItem;
        }

        if (listlayer->menulist->CurItem == listlayer->menulist->head)
        {
            if (listlayer->menulist->load_tail == listlayer->menulist->head)
            {
                listlayer->menulist->load_tail  = NewCurItem;
            }
        }

        /*在menulist中删除当前item*/
        ret = MenuList_DelItem(listlayer->menulist, listlayer->menulist->CurItem);

        if (ret != OK)
        {
            return __LINE__;
        }

        if (NewCurItem != NULL)
        {
            listlayer->menulist->CurItem = NewCurItem;
            /*更新下半部*/
            ListLayer_BoxDelUpdateTail(listlayer);
            //LISTMENU_PRINTF("foucs x = %d,foucs y = %d\n",listlayer->foucs_pos.x,listlayer->foucs_pos.y);
            ret = ListLayer_DrawFastFoucsMenuItem(listlayer, listlayer->menulist->CurItem,
                                                  0, listlayer->foucs_pos.x, listlayer->foucs_pos.y);

            if (ret != OK)
            {
                return __LINE__;
            }

            //LISTMENU_PRINTF("foucs x = %d,foucs y = %d\n",listlayer->foucs_pos.x,listlayer->foucs_pos.y);
            ret = ListLayer_LoadMenuItem(listlayer, listlayer->menulist->CurItem,
                                         0, listlayer->foucs_pos.x, listlayer->foucs_pos.y);

            if (ret != OK)
            {
                return __LINE__;
            }

            listlayer->setfoucs = REAL_FOUCS;
        }
        else/*只有一个item的情况*/
        {
            listlayer->menulist->CurItem = NewCurItem;
            ListLayer_BoxDelUpdateTail(listlayer);
        }
    }

    if (listlayer->menulist->ItemCount > listlayer->scnnum)
    {
        listlayer->menulist->total_dist = (listlayer->menulist->ItemCount  - listlayer->scnnum) * listlayer->menulist->SelItem->rect.height;
    }
    else
    {
        listlayer->menulist->total_dist = 0;
    }

    ListLayer_BoxDeleteMoveDown(listlayer, listlayer->menulist->head->rect.height);

    if (listlayer->menulist->dist > listlayer->menulist->total_dist)
    {
        listlayer->menulist->dist = listlayer->menulist->total_dist;
    }

    listlayer->plistmenu->gcurv.fb_len      = listlayer->menulist->ItemCount * (listlayer->menulist->head->rect.height);
    return OK;
}

/*
**********************************************************************************************************************
*                                               ListLayer_UpdateDraw
*
* author:
*
* date:             2009-1-30
*
* Description:      listlayer update draw (load item to tail)删除item后更新当前Item的下半部
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __u32 ListLayer_BoxAddUpdateTail(LIST_LAYER_MAN_t    *listlayer)
{
    __u32                tmp;
    __u32                count;
    __u32                i;
    __pos_t              cur_pos;
    MENUITEM_t          *tmpItem;

    /*只有一个Item的情况，此时添加前为空listmenu*/
    if (listlayer->menulist->ItemCount == 1)
    {
        ListLayer_OnDraw(listlayer);
        ListLayer_SetWinFoucs(listlayer);
    }
    else
    {
        listlayer->tail_pos.x   = listlayer->foucs_pos.x;
        listlayer->tail_pos.y   = listlayer->foucs_pos.y + listlayer->menulist->CurItem->rect.height;
        cur_pos.x               = listlayer->tail_pos.x;
        cur_pos.y               = listlayer->tail_pos.y;
        tmp                     = listlayer->menulist->CurItem->itemno - listlayer->menulist->HeadItem->itemno;
        count                   = listlayer->scnnum - tmp + listlayer->scnnum * (listlayer->fb_depth / 2 - 1) - 1;
        tmpItem                 = listlayer->menulist->CurItem->Next;

        for (i = 0; i < count; i++)
        {
            if (tmpItem != listlayer->menulist->head)
            {
                if (tmpItem == listlayer->menulist->tail)
                {
                    ListLayer_FastLoadMenuItem(listlayer, tmpItem, 0, cur_pos.x, cur_pos.y);
                    cur_pos.x = cur_pos.x;
                    cur_pos.y = cur_pos.y + listlayer->menulist->head->rect.height;

                    if (cur_pos.y >= listlayer->listbuffer->rectsz.height)
                    {
                        sysmemcpy(listlayer->listbuffer->buffer, listlayer->listbuffer->ebuffer,
                                  listlayer->listbuffer->esize);
                        cur_pos.y = cur_pos.y - listlayer->listbuffer->vrect.height;
                    }

                    listlayer->menulist->load_tail  = tmpItem->Next;
                    listlayer->tail_pos.x           = cur_pos.x;
                    listlayer->tail_pos.y           = cur_pos.y;
                    break;
                }
                else
                {
                    cur_pos.x = cur_pos.x;
                    cur_pos.y = cur_pos.y + listlayer->menulist->head->rect.height;

                    if (cur_pos.y >= listlayer->listbuffer->rectsz.height)
                    {
                        ListLayer_TailToHeadCopy(listlayer);
                        cur_pos.y = cur_pos.y - listlayer->listbuffer->vrect.height;
                    }

                    tmpItem                         = tmpItem->Next;
                    listlayer->menulist->load_tail  = tmpItem;
                    listlayer->tail_pos.x           = cur_pos.x;
                    listlayer->tail_pos.y           = cur_pos.y;
                }
            }
            else
            {
                break;
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

__u32 ListLayer_VarBoxOnDraw(LIST_LAYER_MAN_t    *listlayer)
{
    __u32                num;
    __u32                scnnum;
    __s32                count;
    MENUITEM_t          *tmpItem;
    MENUITEM_t          *HeadItem;
    MENUITEM_t          *ItemHead;

    if (listlayer->menulist)
    {
        if (listlayer->menulist->ItemCount != 0)
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
            //GUI_LyrWinSel(listlayer->layer);
            //GUI_SetBkColor(listlayer->bk_color);
            //GUI_ClearRect(0, 0, listlayer->listbuffer->rectsz.width - 1, listlayer->listbuffer->rectsz.height - 1);
#endif
            listlayer->head_pos.x             = 0;
            listlayer->head_pos.y             = 0;
            listlayer->tail_pos.x             = 0;
            listlayer->tail_pos.y             = 0;
            listlayer->foucs_pos.x            = 0;
            listlayer->foucs_pos.y            = 0;
            //listlayer->unfoucs_pos.x          = 0;
            //listlayer->unfoucs_pos.y          = 0;
            listlayer->setfoucs               = 0;
            listlayer->fillup                 = 0;
            listlayer->filldown               = 0;
            listlayer->viewrect.x             = 0;
            listlayer->viewrect.y             = 0;
            __inf("ItemCount = %d\n", listlayer->menulist->ItemCount);
            scnnum = listlayer->scnnum;

            if (listlayer->menulist->HeadItem == NULL)
            {
                listlayer->menulist->HeadItem = listlayer->menulist->head;
            }

            /*have not set current item,set head item as current item*/
            if (listlayer->menulist->setCurItem == 0)
            {
                tmpItem                         = listlayer->menulist->head;
                listlayer->menulist->CurItem    = listlayer->menulist->head;
                listlayer->head_pos.y           = listlayer->listbuffer->vrect.height -
                                                  listlayer->menulist->head->rect.height;
                listlayer->menulist->load_head  = listlayer->menulist->tail;
                listlayer->menulist->setCurItem = 1;
            }
            else  /*set curitem*/
            {
                count = listlayer->menulist->CurItem->itemno - listlayer->menulist->HeadItem->itemno;
                HeadItem = listlayer->menulist->CurItem;

                /*新设置的当前item会影响头item*/
                if (count < 0 || count > scnnum - 1)
                {
                    for (num = 0; num < scnnum - 1; num++)
                    {
                        if (HeadItem == listlayer->menulist->head)
                        {
                            break;
                        }

                        HeadItem = HeadItem->Prev;
                    }
                }
                else  /*headitem不变*/
                {
                    HeadItem = listlayer->menulist->HeadItem;
                }

                tmpItem                         = HeadItem;
                listlayer->menulist->HeadItem   = HeadItem;
                /*compute the foucs position and fill up number*/
                listlayer->menulist->load_head  = HeadItem->Prev;
                ItemHead                        = HeadItem->Prev;
                listlayer->head_pos.y           = listlayer->listbuffer->vrect.height
                                                  - listlayer->menulist->head->rect.height;
                num = 0;

                /*set fill up region*/
                while ((ItemHead != listlayer->menulist->tail) && (num < (listlayer->fb_depth / 2 - 1) * scnnum - 1))
                {
                    ListLayer_FastLoadMenuItem(listlayer, ItemHead,
                                               0, listlayer->head_pos.x, listlayer->head_pos.y);
                    ItemHead                    = ItemHead->Prev;
                    listlayer->head_pos.y       = listlayer->head_pos.y - listlayer->menulist->CurItem->rect.height ;
                    num++;
                }

                listlayer->menulist->load_head  = ItemHead;
            }

            /*draw fill down region*/
            num = 0;

            if (listlayer->menulist->ItemCount != 1)
            {
                while ((tmpItem != listlayer->menulist->tail) && (num < (listlayer->fb_depth / 2) * scnnum))
                {
                    ListLayer_FastLoadMenuItem(listlayer, tmpItem,
                                               0, listlayer->tail_pos.x,
                                               listlayer->tail_pos.y);

                    if (tmpItem == listlayer->menulist->CurItem)
                    {
                        listlayer->foucs_pos.x = listlayer->tail_pos.x;
                        listlayer->foucs_pos.y = listlayer->tail_pos.y;
                    }

                    tmpItem = tmpItem->Next;
                    listlayer->tail_pos.y = listlayer->menulist->CurItem->rect.height + listlayer->tail_pos.y;
                    num++;
                }

                if (num < (listlayer->fb_depth / 2) * scnnum)
                {
                    ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->tail,
                                               0, listlayer->tail_pos.x,
                                               listlayer->tail_pos.y);

                    if (listlayer->menulist->tail == listlayer->menulist->CurItem)
                    {
                        listlayer->foucs_pos.x = listlayer->tail_pos.x;
                        listlayer->foucs_pos.y = listlayer->tail_pos.y;
                    }

                    tmpItem                 = tmpItem->Next;
                    listlayer->tail_pos.y   = listlayer->menulist->head->rect.height + listlayer->tail_pos.y;
                    num++;
                }

                listlayer->menulist->load_tail = tmpItem;
            }
            else
            {
                ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->tail,
                                           0, listlayer->tail_pos.x,
                                           listlayer->tail_pos.y);

                if (listlayer->menulist->tail == listlayer->menulist->CurItem)
                {
                    listlayer->foucs_pos.x = listlayer->tail_pos.x;
                    listlayer->foucs_pos.y = listlayer->tail_pos.y;
                }

                listlayer->tail_pos.y = listlayer->menulist->head->rect.height + listlayer->tail_pos.y;
                listlayer->menulist->load_tail = listlayer->menulist->head;
            }

            ListLayer_HeadToTailCopy(listlayer);
            listlayer->setfoucs = REAL_FOUCS;
        }

        if (listlayer->plistmenu->slave_lyr)
        {
            __s32   focus_dist;
            focus_dist = listlayer->foucs_pos.y - listlayer->viewrect.y;
            GUI_LyrWinGetScnWindow(listlayer->plistmenu->slave_lyr, &listlayer->plistmenu->slyr_rect);

            if (listlayer->plistmenu->slyr_rect.y != listlayer->scnrect.y + focus_dist)
            {
                listlayer->plistmenu->slyr_rect.y = listlayer->scnrect.y + focus_dist;
                GUI_LyrWinSetScnWindow(listlayer->plistmenu->slave_lyr, &listlayer->plistmenu->slyr_rect);
            }

            GUI_LyrWinGetScnWindow(listlayer->plistmenu->slave_lyr, &listlayer->plistmenu->slyr_rect);
            listlayer->plistmenu->slyr_crect.x = listlayer->plistmenu->slyr_rect.x;
            listlayer->plistmenu->slyr_crect.y = listlayer->plistmenu->slyr_rect.y;
            listlayer->plistmenu->slyr_crect.width = listlayer->plistmenu->slyr_rect.width;
            listlayer->plistmenu->slyr_crect.height = listlayer->plistmenu->slyr_rect.height;
        }
    }

    return OK;
}



__u32 ListLayer_FixedBoxOnDraw(LIST_LAYER_MAN_t *listlayer)
{
    // __s32                ret;
    __u32                num;
    __s32                 count;
    __u32                mid_num = 0;
    MENUITEM_t          *tmpItem;
    MENUITEM_t          *HeadItem;
    __u32                scnnum;

    if (listlayer->menulist)
    {
        if (listlayer->menulist->ItemCount != 0)
        {
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
            scnnum                            = listlayer->scnnum;

            if (listlayer->menulist->HeadItem == NULL)
            {
                listlayer->menulist->HeadItem = listlayer->menulist->head;
            }

            if (listlayer->menulist->setCurItem == 0)  /*have not set current item*/
            {
                listlayer->setfoucs             = 0;
                listlayer->menulist->CurItem    = listlayer->menulist->head;
            }
            else   /*初始化时已经设置焦点Item*/
            {
                count = listlayer->menulist->CurItem->itemno - listlayer->menulist->HeadItem->itemno;
                HeadItem = listlayer->menulist->CurItem;

                /*新设置的当前item会影响头item*/
                if (count < 0 || count > scnnum - 1)
                {
                    for (num = 0; num < scnnum - 1; num++)
                    {
                        if (HeadItem == listlayer->menulist->head)
                        {
                            break;
                        }

                        HeadItem = HeadItem->Prev;
                    }
                }
                else  /*headitem不变*/
                {
                    HeadItem = listlayer->menulist->HeadItem;
                }

                tmpItem                         = HeadItem;
                listlayer->menulist->HeadItem   = HeadItem;
                tmpItem                         = listlayer->menulist->head;

                while (tmpItem != listlayer->menulist->CurItem)
                {
                    if (tmpItem->itemno < listlayer->menulist->HeadItem->itemno)
                    {
                        listlayer->viewrect.x = listlayer->viewrect.x;
                        listlayer->viewrect.y = listlayer->viewrect.y + tmpItem->rect.height;
                    }

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
                    listlayer->setfoucs = VAR_FOUCS;
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
                listlayer->setfoucs = VAR_FOUCS;
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
*                                               ListLayer_DelCurItem
*
* author:
*
* date:             2009-1-30
*
* Description:      listlayer additem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListLayer_BoxAddItemTail(LIST_LAYER_MAN_t *listlayer, __lstm_item_attr_t *NewItem)
{
    __u32                                   ret;
    MENUITEM_t                              *menuitem;
    __rectsz_t                              rect;

    if (listlayer == NULL || NewItem == NULL)
    {
        return __LINE__;
    }

    rect.width                     = listlayer->listbuffer->rectsz.width;
    rect.height                    = NewItem->item_w_unfocs;
    /*MenuItem Create*/
    menuitem                       = MenuItem_Create(&rect, NewItem, listlayer->plistmenu->handle);

    if (!menuitem)
    {
        ORANGE_WRN("create menuitem failed!\n");
        return __LINE__;
    }

    if (listlayer->menulist->load_head == listlayer->menulist->tail)
    {
        listlayer->menulist->load_head  = menuitem;
    }

    /*add new item to menulist*/
    ret = MenuList_AddTail(listlayer->menulist, menuitem);

    if (ret != OK)
    {
        return __LINE__;
    }

    ListLayer_BoxAddUpdateTail(listlayer);

    if (listlayer->menulist->ItemCount > listlayer->scnnum)
    {
        listlayer->menulist->total_dist = (listlayer->menulist->ItemCount  - listlayer->scnnum) * listlayer->menulist->SelItem->rect.height;
    }
    else
    {
        listlayer->menulist->total_dist = 0;
    }

    listlayer->plistmenu->gcurv.fb_len      = listlayer->menulist->ItemCount * (listlayer->menulist->head->rect.height);
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

__u32 ListLayer_VarNextPage(LIST_LAYER_MAN_t    *listlayer)
{
    __u32                num;
    __u32                scnnum;
    __s32                count;
    __s32                ret;
    MENUITEM_t          *tmpItem;
    MENUITEM_t          *HeadItem;
    MENUITEM_t          *ItemHead;

    if (listlayer->menulist)
    {
        if (listlayer->menulist->ItemCount != 0)
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

            if (listlayer->menulist->HeadItem == NULL)
            {
                listlayer->menulist->HeadItem = listlayer->menulist->head;
            }

            /*have not set current item,set head item as current item*/
            if (listlayer->menulist->setCurItem == 0)
            {
                tmpItem = listlayer->menulist->head;
                listlayer->menulist->CurItem = listlayer->menulist->head;
                listlayer->head_pos.y = listlayer->listbuffer->vrect.height - listlayer->menulist->head->rect.height;
                listlayer->fillup = listlayer->upcount;
                listlayer->menulist->load_head = listlayer->menulist->tail;
                listlayer->menulist->setCurItem = 1;
            }
            else  /*set curitem*/
            {
                HeadItem    = listlayer->menulist->HeadItem;
                count       = listlayer->menulist->HeadItem->itemno + scnnum;

                if (count >= listlayer->menulist->ItemCount)
                {
                    HeadItem = listlayer->menulist->head;
                }
                else
                {
                    for (num = 0; num < scnnum; num++)
                    {
                        HeadItem = HeadItem->Next;
                    }
                }

                tmpItem = HeadItem;
                listlayer->menulist->HeadItem = HeadItem;
                listlayer->menulist->CurItem  = HeadItem;
                /*compute the foucs position and fill up number*/
                ItemHead    = HeadItem->Prev;
                listlayer->head_pos.y = listlayer->listbuffer->vrect.height - listlayer->menulist->head->rect.height;
                num = 0;

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
            }

            /*draw fill down region*/
            num = 0;

            if (listlayer->menulist->ItemCount != 1)
            {
                while ((tmpItem != listlayer->menulist->tail) && (num < (listlayer->fb_depth / 2) * scnnum))
                {
                    if (tmpItem == listlayer->menulist->CurItem)
                    {
                        //LISTMENU_PRINTF("foucs x = %d,foucs y = %d\n",listlayer->foucs_pos.x,listlayer->foucs_pos.y);
                        ListLayer_FastLoadMenuItem(listlayer, tmpItem, 0,
                                                   listlayer->tail_pos.x, listlayer->tail_pos.y);
                        listlayer->foucs_pos.x = listlayer->tail_pos.x;
                        listlayer->foucs_pos.y = listlayer->tail_pos.y;
                    }
                    else
                    {
                        ListLayer_FastLoadMenuItem(listlayer, tmpItem,
                                                   0, listlayer->tail_pos.x,
                                                   listlayer->tail_pos.y);
                    }

                    tmpItem = tmpItem->Next;
                    listlayer->tail_pos.y = listlayer->menulist->CurItem->rect.height + listlayer->tail_pos.y;
                    num++;
                }

                if (num < (listlayer->fb_depth / 2) * scnnum)
                {
                    if (listlayer->menulist->tail == listlayer->menulist->CurItem)
                    {
                        //LISTMENU_PRINTF("foucs x = %d,foucs y = %d\n",listlayer->foucs_pos.x,listlayer->foucs_pos.y);
                        ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->tail, 0,
                                                   listlayer->tail_pos.x, listlayer->tail_pos.y);
                        listlayer->foucs_pos.x = listlayer->tail_pos.x;
                        listlayer->foucs_pos.y = listlayer->tail_pos.y;
                    }
                    else
                    {
                        ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->tail,
                                                   0, listlayer->tail_pos.x,
                                                   listlayer->tail_pos.y);
                    }

                    tmpItem = tmpItem->Next;
                    listlayer->tail_pos.y = listlayer->menulist->head->rect.height + listlayer->tail_pos.y;
                    num++;
                    listlayer->menulist->load_tail = tmpItem;
                }

                listlayer->menulist->load_tail = tmpItem;
            }
            else
            {
                if (listlayer->menulist->tail == listlayer->menulist->CurItem)
                {
                    //LISTMENU_PRINTF("foucs x = %d,foucs y = %d\n",listlayer->foucs_pos.x,listlayer->foucs_pos.y);
                    ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->tail, 0,
                                               listlayer->tail_pos.x, listlayer->tail_pos.y);
                    listlayer->foucs_pos.x = listlayer->tail_pos.x;
                    listlayer->foucs_pos.y = listlayer->tail_pos.y;
                }
                else
                {
                    ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->tail,
                                               0, listlayer->tail_pos.x,
                                               listlayer->tail_pos.y);
                }

                listlayer->tail_pos.y = listlayer->menulist->head->rect.height + listlayer->tail_pos.y;
                listlayer->menulist->load_tail = listlayer->menulist->head;
            }

            ListLayer_HeadToTailCopy(listlayer);
            listlayer->setfoucs = REAL_FOUCS;
        }

        if (listlayer->plistmenu->mode & LISTBOX_MOD)
        {
            if (listlayer->plistmenu->slave_lyr)
            {
                __s32   focus_dist;
                focus_dist = listlayer->foucs_pos.y - listlayer->viewrect.y;
                GUI_LyrWinGetScnWindow(listlayer->plistmenu->slave_lyr, &listlayer->plistmenu->slyr_rect);

                if (listlayer->plistmenu->slyr_rect.y != listlayer->scnrect.y + focus_dist)
                {
                    listlayer->plistmenu->slyr_rect.y = listlayer->scnrect.y + focus_dist;
                    GUI_LyrWinSetScnWindow(listlayer->plistmenu->slave_lyr, &listlayer->plistmenu->slyr_rect);
                }

                GUI_LyrWinGetScnWindow(listlayer->plistmenu->slave_lyr, &listlayer->plistmenu->slyr_rect);
                listlayer->plistmenu->slyr_crect.x = listlayer->plistmenu->slyr_rect.x;
                listlayer->plistmenu->slyr_crect.y = listlayer->plistmenu->slyr_rect.y;
                listlayer->plistmenu->slyr_crect.width = listlayer->plistmenu->slyr_rect.width;
                listlayer->plistmenu->slyr_crect.height = listlayer->plistmenu->slyr_rect.height;
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

__u32 ListLayer_VarPrevPage(LIST_LAYER_MAN_t    *listlayer)
{
    __u32                num;
    __u32                scnnum;
    __s32                count;
    __s32                ret;
    MENUITEM_t          *tmpItem;
    MENUITEM_t          *HeadItem;
    MENUITEM_t          *ItemHead;

    if (listlayer->menulist)
    {
        if (listlayer->menulist->ItemCount != 0)
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

            if (listlayer->menulist->HeadItem == NULL)
            {
                listlayer->menulist->HeadItem = listlayer->menulist->head;
            }

            /*have not set current item,set head item as current item*/
            if (listlayer->menulist->setCurItem == 0)
            {
                tmpItem = listlayer->menulist->head;
                listlayer->menulist->CurItem = listlayer->menulist->head;
                listlayer->head_pos.y = listlayer->listbuffer->vrect.height - listlayer->menulist->head->rect.height;
                listlayer->fillup = listlayer->upcount;
                listlayer->menulist->load_head = listlayer->menulist->tail;
                listlayer->menulist->setCurItem = 1;
            }
            else  /*set curitem*/
            {
                HeadItem    = listlayer->menulist->HeadItem;
                count       = (__s32)(listlayer->menulist->HeadItem->itemno - scnnum);

                if (count < 0)
                {
                    HeadItem = listlayer->menulist->head;
                }
                else
                {
                    for (num = 0; num < scnnum; num++)
                    {
                        HeadItem = HeadItem->Prev;
                    }
                }

                tmpItem = HeadItem;
                listlayer->menulist->HeadItem = HeadItem;
                listlayer->menulist->CurItem  = HeadItem;
                /*compute the foucs position and fill up number*/
                ItemHead    = HeadItem->Prev;
                listlayer->head_pos.y = listlayer->listbuffer->vrect.height - listlayer->menulist->head->rect.height;
                num = 0;

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
            }

            /*draw fill down region*/
            num = 0;

            if (listlayer->menulist->ItemCount != 1)
            {
                while ((tmpItem != listlayer->menulist->tail) && (num < (listlayer->fb_depth / 2) * scnnum))
                {
                    if (tmpItem == listlayer->menulist->CurItem)
                    {
                        //LISTMENU_PRINTF("foucs x = %d,foucs y = %d\n",listlayer->foucs_pos.x,listlayer->foucs_pos.y);
                        ListLayer_FastLoadMenuItem(listlayer, tmpItem, 0,
                                                   listlayer->tail_pos.x, listlayer->tail_pos.y);
                        listlayer->foucs_pos.x = listlayer->tail_pos.x;
                        listlayer->foucs_pos.y = listlayer->tail_pos.y;
                    }
                    else
                    {
                        ListLayer_FastLoadMenuItem(listlayer, tmpItem,
                                                   0, listlayer->tail_pos.x,
                                                   listlayer->tail_pos.y);
                    }

                    tmpItem = tmpItem->Next;
                    listlayer->tail_pos.y = listlayer->menulist->CurItem->rect.height + listlayer->tail_pos.y;
                    num++;
                }

                if (num < (listlayer->fb_depth / 2) * scnnum)
                {
                    if (listlayer->menulist->tail == listlayer->menulist->CurItem)
                    {
                        //LISTMENU_PRINTF("foucs x = %d,foucs y = %d\n",listlayer->foucs_pos.x,listlayer->foucs_pos.y);
                        ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->tail, 0,
                                                   listlayer->tail_pos.x, listlayer->tail_pos.y);
                        listlayer->foucs_pos.x = listlayer->tail_pos.x;
                        listlayer->foucs_pos.y = listlayer->tail_pos.y;
                    }
                    else
                    {
                        ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->tail,
                                                   0, listlayer->tail_pos.x,
                                                   listlayer->tail_pos.y);
                    }

                    tmpItem = tmpItem->Next;
                    listlayer->tail_pos.y = listlayer->menulist->head->rect.height + listlayer->tail_pos.y;
                    num++;
                    listlayer->menulist->load_tail = tmpItem;
                }

                listlayer->menulist->load_tail = tmpItem;
            }
            else
            {
                if (listlayer->menulist->tail == listlayer->menulist->CurItem)
                {
                    //LISTMENU_PRINTF("foucs x = %d,foucs y = %d\n",listlayer->foucs_pos.x,listlayer->foucs_pos.y);
                    ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->tail, 0,
                                               listlayer->tail_pos.x, listlayer->tail_pos.y);
                    listlayer->foucs_pos.x = listlayer->tail_pos.x;
                    listlayer->foucs_pos.y = listlayer->tail_pos.y;
                }
                else
                {
                    ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->tail,
                                               0, listlayer->tail_pos.x,
                                               listlayer->tail_pos.y);
                }

                listlayer->tail_pos.y = listlayer->menulist->head->rect.height + listlayer->tail_pos.y;
                listlayer->menulist->load_tail = listlayer->menulist->head;
            }

            ListLayer_HeadToTailCopy(listlayer);
            listlayer->setfoucs = REAL_FOUCS;
        }

        if (listlayer->plistmenu->mode & LISTBOX_MOD)
        {
            if (listlayer->plistmenu->slave_lyr)
            {
                __s32   focus_dist;
                focus_dist = listlayer->foucs_pos.y - listlayer->viewrect.y;
                GUI_LyrWinGetScnWindow(listlayer->plistmenu->slave_lyr, &listlayer->plistmenu->slyr_rect);

                if (listlayer->plistmenu->slyr_rect.y != listlayer->scnrect.y + focus_dist)
                {
                    listlayer->plistmenu->slyr_rect.y = listlayer->scnrect.y + focus_dist;
                    GUI_LyrWinSetScnWindow(listlayer->plistmenu->slave_lyr, &listlayer->plistmenu->slyr_rect);
                }

                GUI_LyrWinGetScnWindow(listlayer->plistmenu->slave_lyr, &listlayer->plistmenu->slyr_rect);
                listlayer->plistmenu->slyr_crect.x = listlayer->plistmenu->slyr_rect.x;
                listlayer->plistmenu->slyr_crect.y = listlayer->plistmenu->slyr_rect.y;
                listlayer->plistmenu->slyr_crect.width = listlayer->plistmenu->slyr_rect.width;
                listlayer->plistmenu->slyr_crect.height = listlayer->plistmenu->slyr_rect.height;
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

    return OK;
}

//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------
#endif //__LIST_LAYER__C____
