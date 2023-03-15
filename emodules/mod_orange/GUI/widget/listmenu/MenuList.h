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

#ifndef __MENU_LIST__H____
#define __MENU_LIST__H____

#include "config.h"
#include "MenuItem.h"

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
typedef struct tag_MENULIST
{
    __bool             setCurItem;

    /*last current item*/
    MENUITEM_t        *UnCurItem;
    MENUITEM_t        *head;
    MENUITEM_t        *tail;
    MENUITEM_t        *load_head;
    MENUITEM_t        *load_tail;
    MENUITEM_t        *SelItem;
    MENUITEM_t        *UnSelItem;
    __u32              ItemCount;
    __u32              bpp;
    __s32              dist;        //用来判断移动的距离和selItem之间的关系
    __s32              lyrdist;
    __u32              total_dist;
    MENUITEM_t        *CurItem;

    /*only used in listbox mod*/
    MENUITEM_t        *HeadItem;
} MENULIST_t;


MENULIST_t     *MenuList_Create(void);
__u32           MenuList_InsertAfter(MENULIST_t *menulist, MENUITEM_t *after, MENUITEM_t *menuitem);
__u32           MenuList_DelItem(MENULIST_t *menulist, MENUITEM_t *menuitem);
__u32           MenuList_DelAll(MENULIST_t *menulist);
__u32           MenuList_AddTail(MENULIST_t *menulist,  MENUITEM_t *menuitem);
__u32           MenuList_AddHead(MENULIST_t *menulist, MENUITEM_t *menuitem);
MENUITEM_t     *MenuList_GetHead(MENULIST_t *menulist);
H_WIN           MenuList_GetCurItem(MENULIST_t *menulist);
__u32           MenuList_SetCurItem(MENULIST_t *menulist, MENUITEM_t *menuitem);
MENUITEM_t     *MenuList_GetTail(MENULIST_t *menulist);
__u32           MenuList_GetItemCount(MENULIST_t *menulist);
__u32           MenuList_Destroy(MENULIST_t *menulist);
#endif //__MENU_LIST__H____


