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
#ifndef __LIST_LAYER_PROCESS_H____
#define __LIST_LAYER_PROCESS_H____


#include "config.h"
#include "ListBuffer.h"
#include "ListMenu_i.h"
#include "MenuList.h"

typedef enum EVENT_LIST_FLAGS
{
    LISTLAYER_LOAD_FLAG            = 1 << 0,    /*listmenu加载线程的设置标志位*/
    LISTLAYER_WAIT_FLAG            = 1 << 1,    /*运行线程的等待加载标志位*/
    LISTLAYER_FOUCS_FLAG           = 1 << 2,    /*设焦线程的运行标志位*/
    LISTLAYER_FOUCS_FINISH_FLAG    = 1 << 3,    /*设焦线程设置焦点完成*/
    LISTLAYER_DRAW_FINISH_FLAG     = 1 << 4,
    LISTLAYER_DRAW_ITEM_FLAG       = 1 << 5,    /*设焦线程(同时负责停止运行时的扩散操作)*/
    LISTLAYER_WAIT_LOAD_FLAG       = 1 << 6     /*运行线程停止时发送给加载线程的标志位*/
} EVENT_LIST_FLAGS_E;

typedef struct list_event_trigger
{
    __u8         direct;
    __u8         num;
} list_event_trigger_t;

typedef enum LISTBUFFER_STATUS
{
    INMIDDLE     = 0,            //可视窗口位置正中
    UPWARDS      = 1 << 0,       //可视窗口位置偏上
    DOWNWARDS    = 1 << 1,       //可视窗口位置偏下
    WAITUP       = 1 << 2,       //可视窗口不可向上滑动
    WAITDOWN     = 1 << 3,       //可视窗口不可向下滑动
    DOWNOVER     = 1 << 4,
    UPOVER       = 1 << 5
} LISTBUFFER_STATUS_E;

void    _lstm_key_song(void);
__u32   InterMove(__u32 mode, __s32 range, __u32 Steps, __u32 CurStep, __move_para_t *Parameter);
void    ListLayer_MenuItemUpdateChild(MENUITEM_t   *menuitem);
__u32   ListLayer_MenuItemCopy(LIST_LAYER_MAN_t *listlayer, __u32 srcfbno, __u32 dstfbno, __u32 src_x, __u32 src_y, __u32 dst_x, __u32 dst_y);
__u32   ListLayer_LoadMenuItem(LIST_LAYER_MAN_t *listlayer, MENUITEM_t *item, __u8       fbno, __u32 x, __u32 y);
__u32   ListLayer_UnFoucsMenuItem(LIST_LAYER_MAN_t *listlayer, MENUITEM_t *item, __u8       fbno, __u32 x, __u32 y);
__u32   ListLayer_DrawFastFoucsMenuItem(LIST_LAYER_MAN_t    *listlayer, MENUITEM_t   *menuitem, __u8 fbno, __u32 x, __u32 y);
__u32   ListLayer_DrawFoucsMenuItem(LIST_LAYER_MAN_t    *listlayer, MENUITEM_t   *menuitem, __u8 fbno, __u32 x, __u32 y);
__u32   ListLayer_FastFoucsMenuItem(LIST_LAYER_MAN_t *listlayer, MENUITEM_t *item, __u8       fbno, __u32 x, __u32 y);
__u32   ListLayer_FastUnFoucsMenuItem(LIST_LAYER_MAN_t *listlayer, MENUITEM_t *item, __u8       fbno, __u32 x, __u32 y);
__u32   ListLayer_FastLoadMenuItem(LIST_LAYER_MAN_t *listlayer, MENUITEM_t *item, __u8       fbno, __u32 x, __u32 y);
__u32   ListLayer_DrawActivedMenuItem(LIST_LAYER_MAN_t    *listlayer, MENUITEM_t   *menuitem, __u8 fbno, __u32 x, __u32 y);
__u32   ListLayer_DrawInActivedMenuItem(LIST_LAYER_MAN_t    *listlayer, MENUITEM_t   *menuitem, __u8 fbno, __u32 x, __u32 y);
__u32   ListLayer_GetBufferStatus(LIST_LAYER_MAN_t *listlayer);
void    ListLayer_LoadItemThread(void *p_arg);
__s32   ListLayer_SetFoucs(LIST_LAYER_MAN_t *listlayer, MENUITEM_t *menuitem, __u32 x, __u32 y);
__u32   ListLayer_ClearMenuItem(LIST_LAYER_MAN_t *listlayer, __u8   fbno, __u32 x, __u32 y);
__u32   ListLayer_SetLayerFoucs(LIST_LAYER_MAN_t *listlayer, MENUITEM_t *menuitem);
__u32   ListLayer_SetLayerUnFoucs(LIST_LAYER_MAN_t *listlayer);
__u32   ListLayer_SetLayerFastFoucs(LIST_LAYER_MAN_t    *listlayer, MENUITEM_t   *menuitem);
void    ListLayer_FoucsItemThread(void *p_arg);
__u32   ListLayer_KeyUpMsg(LIST_LAYER_MAN_t *listlayer, __u32 dist, __u32  step);
__u32   ListLayer_KeyDownMsg(LIST_LAYER_MAN_t *listlayer, __u32 dist, __u32 step);
__bool  ListLayer_GetLoadStatus(LIST_LAYER_MAN_t *listlayer);
__u32   ListLayer_OnMesssage(LIST_LAYER_MAN_t    *listlayer, __listmenu_runmsg_t *pMsg);
__u32   ListLayer_MoveDownMsg(LIST_LAYER_MAN_t *listlayer, __u32 dist, __u32  step);
__u32   ListLayer_MoveUpMsg(LIST_LAYER_MAN_t *listlayer, __u32 dist, __u32  step);
__s32   ListLayer_GetMoveStatus(LIST_LAYER_MAN_t *listlayer);
#endif
