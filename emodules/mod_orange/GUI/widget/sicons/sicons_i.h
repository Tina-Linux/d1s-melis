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
#ifndef __SICONS_I_H__
#define __SICONS_I_H__

#include "../winclass_i.h"

#define sicons_malloc(size)           esMEMS_Malloc(0,size)
#define sicons_free(ptr)              esMEMS_Mfree(0,ptr)
#define sicons_balloc(size)           esMEMS_Balloc(size)
#define sicons_bfree(ptr,size)        esMEMS_Bfree(ptr,size)
#define sicons_palloc(size)           esMEMS_Palloc(((size + 1023 )/1024), MEMS_PALLOC_MODE_BND_1K);
#define sicons_pfree(ptr,size)        esMEMS_Pfree(ptr,((size + 1023 )/1024));
#define sicons_memcpy                 eLIBs_memcpy
#define sicons_memset                 eLIBs_memset

typedef __hdle  h_iconssitem;

#define LISTITEMST_PUSHED              (1<<1)

#define LISTITEMST_FOCUS               (1<<2)

/*全局链表保留用户iconitem数据*/
typedef struct tag_ICONS_MEMUITEM
{
    struct tag_ICONS_MEMUITEM          *Prev;
    struct tag_ICONS_MEMUITEM          *Next;
    __u32                               itemno;
    IconsItem_DrawCB                    cb;                         /* item draw回调函数                                          */
    H_WIN                               handle;                     /*iconitem所在listicon的句柄*/
    void                               *attr;
    __u32                               item_w;
    __bool                              enable;
} __icons_iconitem_t;

#define   SICONS_ItemH2P(h_item)       ((__icons_iconitem_t *)h_item)
#define   SICONS_ItemP2H(pitem)        ((h_iconssitem)pitem)

/*定义需要绘制的Item区域和绘制参数*/
typedef struct tag_ICONS_SITEM
{
    RECT                                rect;
    IconsItem_DrawCB                  cb;
    __icons_iconitem_t               *iconitem;
} __icons_sitem_t;

//------------------------------------------------------------------------------------------------------------
typedef struct tag_ICONS_ICONLIST
{
    /*last current item*/
    __icons_iconitem_t               *head;
    __icons_iconitem_t               *tail;
    __u32                               ItemCount;

    /*only used in icons mod*/
    __icons_iconitem_t               *HeadItem;

    /*the number of page item num*/
    __u32                               pagenum;
} __icons_iconlist_t;

typedef struct tag_SLITBOX_DATA
{
    __u8                                scnnum;         /*实际数量*/
    __u32                               bk_color;
    __u32                               status;
    __u32                               draw_sts;
    __icons_iconlist_t               *iconlist;
    H_WIN                               handle;        /*listicon所在窗口的句柄*/
    __u32                               lastmsg;
    void                               *bk_bmp;
    __u32                               bmp_pos;
    __bool                              bk_draw;
    h_iconssitem                      *sitem_array;

    /*当前正在icons中显示有效条目数*/
    __u32                               valid_num;
    __u32                               cur_index;
    __u32                               last_index;
    GUI_MEMDEV_Handle                   hmem;

    __u32                               bpp;
    __u32                               lyr_w;
    __u32                               lyr_h;
    void                               *fb_addr;
    H_LYR                               lyr_win;
} __sicons_data_t;

typedef __sicons_data_t *piconsdata;

#define ICONS_DRAWSTATUS(pCtrl)                    (((piconsdata)GUI_CtrlWinGetAddData(pCtrl))->draw_sts)
#define ICONS_STATUS(pCtrl)                        (((piconsdata)GUI_CtrlWinGetAddData(pCtrl))->status)
//#define BUTTON_STSTRANS(pCtrl)                     (BUTTON_OLDSTATUS(pCtrl) != BUTTON_STATUS(pCtrl))
#define ICONS_DATA(pCtrl)                          (((piconsdata)GUI_CtrlWinGetAddData(pCtrl))->data)

__u32                   SIconItem_Destroy(__icons_iconitem_t *iconitem);
__s32                   SShowItem_Destory(__icons_sitem_t *sitem);
__icons_sitem_t        *SShowItem_Create(__icons_sitem_t *sitem_attr);
__icons_iconitem_t   *SIconItem_Create(__sicons_item_attr_t *item_attr, H_WIN  hWnd);
__icons_iconitem_t   *SIconItem_GetPrev(__icons_iconitem_t *iconitem);
__icons_iconitem_t   *SIconItem_GetNext(__icons_iconitem_t *iconitem);

__u32                   SIconList_InsertAfter(__icons_iconlist_t *iconlist, __icons_iconitem_t *after, __icons_iconitem_t *iconitem);
__icons_iconitem_t   *SIconList_GetIconitem(__icons_iconlist_t *iconlist, __s32 index);
__u32                   SIconList_DelItem(__icons_iconlist_t *iconlist, __icons_iconitem_t *iconitem);
__u32                   SIconList_DelAll(__icons_iconlist_t *iconlist);
__u32                   SIconList_AddTail(__icons_iconlist_t *iconlist,  __icons_iconitem_t *iconitem);
__u32                   SIconList_AddHead(__icons_iconlist_t *iconlist, __icons_iconitem_t *iconitem);
__icons_iconlist_t   *SIconList_Create(void);
__icons_iconitem_t   *SIconList_GetHead(__icons_iconlist_t *iconlist);
H_WIN                   SIconList_GetCurItem(__icons_iconlist_t *iconlist);
__u32                   SIconList_SetCurItem(__icons_iconlist_t *iconlist, __icons_iconitem_t *iconitem);
__icons_iconitem_t   *SIconList_GetTail(__icons_iconlist_t *iconlist);
__u32                   SIconList_GetItemCount(__icons_iconlist_t *iconlist);
__u32                   SIconList_Destroy(__icons_iconlist_t *iconlist);
__icons_iconitem_t   *SIconList_FindNextEnable(__icons_iconlist_t *iconlist, __icons_iconitem_t *cur_item);
__icons_iconitem_t   *SIconList_FindPrevEnable(__icons_iconlist_t *iconlist, __icons_iconitem_t *cur_item);
__s32                   SIconList_SetHeadItemFromItem(__icons_iconlist_t *iconlist, __icons_iconitem_t *cur_item);
__s32                   SIconList_GetCurPageNum(__icons_iconlist_t *iconlist);
#endif
