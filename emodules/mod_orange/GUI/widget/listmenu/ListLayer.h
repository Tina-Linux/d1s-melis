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
#ifndef __LIST_LAYER__H____
#define __LIST_LAYER__H____

#include "config.h"
#include "ListBuffer.h"
#include "MenuList.h"

struct tag_LISTMENU_DATA;

#define   FIXED_MODE   1
#define   VAR_MODE     2

#define   KEY_FOCUS     0
#define   TOUCH_FOCUS   1

#define   NO_FOUCS     0
#define   REAL_FOUCS   1     /*实际设置焦点*/
#define   VAR_FOUCS    2     /*虚拟设置焦点(快速滑动时设置)*/

#define   WAIT_LOAD    0x01

#define   LISTLAYER_ROUNDUP   0x01
#define   LISTLAYER_MOVE      0x02
#define   LISTLAYER_ROUNDDOWN 0x03
//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
typedef struct tag_LIST_LAYER_MAN
{
    __u32                       LoadTID;        /*item加载线程的id*/
    __u32                       LoadPrio;       /*item加载线程的*/
    __u32                       FoucsTID;
    __u32                       FoucsPrio;
    __s8                        fillup;         /*only used by linear mode*/
    __u8                        filldown;       /*only used by linear mode*/
    __u8                        rollspeed;
    __u8                        mode;
    __u8                        scnnum;         /*实际数量*/
    __u8                        fb_depth;
    __u8                        upcount;        /*起始位置需要填充的item数量*/
    __u8                        downcount;      /*结束位置需要填充的item数量*/
    __bool                      adjust_wm;
    H_LYR                       layer;
    SIZE                        lyr_size;
    __u32                       msgupcount;      /*消息数量*/
    __u32                       msgdowncount;      /*消息数量*/
    __u32                       bk_color;
    __u32                       dir;
    __u32                       msgcount;
    __u8                        cursteps;      /*每运行一个item当前的步数*/
    __u8                        nextsteps;     /*每运行一个item下一个步数*/
    __u16                       scn_width;     /*当前listmenu所在屏幕的宽度*/
    __u16                       scn_height;     /*当前listmenu所在屏幕的高度*/
    __bool                      msgflush;
    __u8                        setfoucs;
    __rect_t                    scnrect;
    __rect_t                    userscnrect;
    __rect_t                    viewrect;
    __pos_t                     head_pos;       /*对fixed而言，代表起始时在屏幕中的位置
                                                  对variable而言,代表后台加载的起始位置*/
    __pos_t                     tail_pos;       /*对fixed而言，代表结束时在屏幕中的位置
                                                  对variable而言,代表后台加载的结束位置*/
    __pos_t                     headlyr_pos;       /*对fixed而言，代表起始时在屏幕中的位置
                                                  对variable而言,代表后台加载的起始位置*/
    __pos_t                     taillyr_pos;       /*对fixed而言，代表结束时在屏幕中的位置
                                                  对variable而言,代表后台加载的结束位置*/
    __pos_t                     foucs_pos;
    __pos_t                     unfoucs_pos;
    LIST_BUFFER_MAN_t          *listbuffer;
    MENULIST_t                 *menulist;
    __krnl_flag_grp_t          *load_flags;
    __krnl_flag_grp_t          *wait_flags;
    __krnl_flag_grp_t          *foucs_flags;
    __krnl_event_t             *pos_sem;
    __krnl_event_t             *load_sem;
    struct tag_LISTMENU_DATA   *plistmenu;
    __u32                       status;
    __u8                        focus_type;
} LIST_LAYER_MAN_t;

__s32          ListLayer_GetBPP(LIST_LAYER_MAN_t *plist_layer);
__s32          ListLayer_SwitchFB(LIST_LAYER_MAN_t *listlayer, __bool fbno, __u32 x, __u32 y);
__s32          ListLayer_SwitchLyr(LIST_LAYER_MAN_t *listlayer, __u32 height, __u32 y);
__hdle         ListLayer_Create(H_LYR listlayer, __hdle listmenu, __u32 item_w, __u32 foucs_w);
__u32          ListLayer_Destroy(LIST_LAYER_MAN_t *listlayer);
__u32          ListLayer_Wakeup(LIST_LAYER_MAN_t *list);
__u32          ListLayer_Sleep(LIST_LAYER_MAN_t *list);
__u32          ListLayer_OnDraw(LIST_LAYER_MAN_t *listlayer);
__u32          ListLayer_SetWinFoucs(LIST_LAYER_MAN_t *listlayer);
__u32          ListLayer_SetWinUnFoucs(LIST_LAYER_MAN_t    *listlayer);
#endif //__LIST_LAYER__H____
