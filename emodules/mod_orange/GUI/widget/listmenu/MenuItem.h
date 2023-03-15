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
#ifndef __MENU_ITEM__H____
#define __MENU_ITEM__H____

#include "config.h"
#include "Listmenu_e.h"

#define   LOAD_STS              1
#define   FLOAD_STS             2
#define   FFOUCS_STS            3
#define   FOUCS_STS             4
#define   UNFOUCS_STS           5
#define   FUNFOUCS_STS          6
#define   ACTIVED_STS           7
#define   INACTIVED_STS         8

#define   MENUITEM_MAXWIN       8

#define   GUI_ItemH2P(h_item)       ((MENUITEM_t *)h_item)
#define   GUI_ItemP2H(pitem)        ((H_ITEM)pitem)

typedef struct tag_ITEMCHILD
{
    H_WIN    hWnd;
    RECT     win_rect;
} ITEMCHILD_t;
//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------

typedef struct tag_MEMUITEM
{
    RECT                       rect;
    struct tag_MEMUITEM       *Prev;
    struct tag_MEMUITEM       *Next;
    __u32                      itemno;
    __u32                      foucs_w;
    __pITEM_DRAW_CB            cb;                         /* item draw回调函数                                          */
    __u32                      status;
    H_WIN                      handle;                     /*menuitem所在listmenu的句柄*/
    ITEMCHILD_t                child[MENUITEM_MAXWIN];     /*menuitem的子窗口句柄数组*/
    __u8                       nctrls;                      /*menuitem的子窗口数量*/
    void                      *attr;
} MENUITEM_t;

MENUITEM_t *MenuItem_Create(__rectsz_t *rectsz, __lstm_item_attr_t *item_attr, H_WIN handle);
__u32       MenuItem_Destroy(MENUITEM_t *menuitem);
MENUITEM_t *MenuItem_GetPrev(MENUITEM_t *menuitem);
MENUITEM_t *Menuitem_GetNext(MENUITEM_t *menuitem);

//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------

#endif //__MENU_ITEM__H____
