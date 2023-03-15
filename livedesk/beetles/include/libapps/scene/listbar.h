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
#ifndef __LISTBAR_H__
#define __LISTBAR_H__

#include <kconfig.h>

typedef     void   *H_LBAR;

typedef enum tag_LBAR_STATE
{
    LBAR_STATE_MOVING,
    LBAR_STATE_STOP
} __lbar_state_e;

typedef enum tag_LBAR_DRAM_MODE
{
    LBAR_MODE_NORMAL    = 0,                    // 普通条目绘制
    LBAR_MODE_FOCUS,                            // 焦点条目绘制
    LBAR_MODE_INVALID,                          // 无效条目绘制
    LBAR_MODE_VACANT,                           // 空白条目绘制
    LBAR_MODE_DIFFUSE,                          // 扩散条目绘制
    LBAR_MODE_MINIATURE,                        // 画缩略图
} __lbar_draw_mode_e;

typedef struct tag_LBAR_DRAW_PARA
{
    __lbar_draw_mode_e  mode;                   // 绘制的模式
    __s32               index;                  // 绘制条目的编号
    RECT                rect;                   // 绘制条目的区域
    void               *attr;                   // 应用的传入的私有数据
} __lbar_draw_para_t;

typedef struct tag_LISTBAR_SCENE
{
    __s32                   start_id;           // 页面的起始位置条目
    __s32                   focus_id;           // 页面的焦点条目
} __listbar_scene_t;

typedef struct tag_SCROLL_BAR
{
    RECT                    show_rect;          // 显示区域
    RECT                    scroll_rect;        // SCROLL区域
    __u32                   head_height;        // HEAD图标高度
    __u32                   tail_height;        // TAIL图标高度

    void                   *back_bmp;           // BACK图标
    void                   *head_bmp;           // HEAD图标
    void                   *body_bmp;           // BODY图标(高为一个像素)
    void                   *tail_bmp;           // TAIL图标
} __scroll_bar_t;

typedef struct tag_LISTBAR_CONFIG
{
    RECT                    list_rect;          // listbar所在图层的区域
    __s32                   item_width;         // listbar条目的宽
    __s32                   item_height;        // listbar条目的高

    __s32                   start_id;           // 页面的起始位置条目
    __s32                   focus_id;           // 页面的焦点条目
    __u32                   bk_color;           // 背景色
    __s32                   alpha_status;       // Alpha开关

    __u32                   item_cnt;           // item 的条目总数
    void                   *list_attr;          // listbar用户数据
} __listbar_config_t;


/*
**********************************************************************************************************************
*
*
* Description: 每个条目的绘制回调
*
* Arguments  : para: 绘制的参数
*
*
**********************************************************************************************************************
*/
typedef __s32(* __draw_item)(__lbar_draw_para_t *para);


/*
**********************************************************************************************************************
*                                               LISTBAR_Create
*
* Description: 创建一个控制操作句柄
*
* Arguments  : hwin : listbar所在的窗口句柄
*
*Return     : EPDK_OK: 返回一个操作句柄
*             EPDK_FAIL: 失败
*
**********************************************************************************************************************
*/
H_LBAR  LISTBAR_Create(H_WIN hwin);

/*
**********************************************************************************************************************
*                                               LISTBAR_Config
*
* Description: 配置listbar相关属性
*
* Arguments  : hdle: 操作句柄， draw_cb：绘制回调， config：配置listbar相关信息，具体见数据结构说明
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
**********************************************************************************************************************
*/
__s32   LISTBAR_Config(H_LBAR hdle, __draw_item draw_cb, __listbar_config_t *config);

/*
**********************************************************************************************************************
*                                               LISTBAR_ScrollConfig
*
* Description: 配置scroll相关属性
*
* Arguments  : hdle: 操作句柄， scroll_para：配置scroll相关信息，具体见数据结构说明
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
**********************************************************************************************************************
*/
__s32   LISTBAR_ScrollConfig(H_LBAR hdle, __scroll_bar_t *scroll_para);

/*
************************************************************************************************************************
*                                       LISTBAR_ShowPage
*
*Description: 显示当前页面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_ShowPage(H_LBAR hdle);

/*
************************************************************************************************************************
*                                       LISTBAR_NextPage
*
*Description: 向下翻一页
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_NextPage(H_LBAR hdle);

/*
************************************************************************************************************************
*                                       LISTBAR_PrevPage
*
*Description: 向上翻一页
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_PrevPage(H_LBAR hdle);

/*
************************************************************************************************************************
*                                       LISTBAR_NextItem
*
*Description: 下移个条目
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_NextItem(H_LBAR hdle);

/*
************************************************************************************************************************
*                                       LISTBAR_PrevItem
*
*Description: 上移个条目
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_PrevItem(H_LBAR hdle);
__s32   LISTBAR_UpdateCurItem(H_LBAR hdle);
__s32   LISTBAR_UpdateFocusItem(H_LBAR hdle);

/*
************************************************************************************************************************
*                                       LISTBAR_NextRow
*
*Description: 向下移动(一行)
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_NextRow(H_LBAR hdle);

/*
************************************************************************************************************************
*                                       LISTBAR_PrevRow
*
*Description: 向上移动(一行)
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_PrevRow(H_LBAR hdle);

/*
************************************************************************************************************************
*                                       LISTBAR_TouchDown
*
*Description: 触摸按下
*
*Arguments  : hdle: 操作句柄， x: X点坐标， y：Y点坐标
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_TouchDown(H_LBAR hdle, __s32 x, __s32 y, __s32 direct, __s32 speed);

/*
************************************************************************************************************************
*                                       LISTBAR_TouchMove
*
*Description: 触摸移动
*
*Arguments  : hdle: 操作句柄， x: X点坐标， y：Y点坐标
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_TouchMove(H_LBAR hdle, __s32 x, __s32 y, __s32 direct, __s32 speed);

/*
************************************************************************************************************************
*                                       LISTBAR_TouchUp
*
*Description: 触摸抬起
*
*Arguments  : hdle: 操作句柄， x: X点坐标， y：Y点坐标
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_TouchUp(H_LBAR hdle, __s32 x, __s32 y, __s32 direct, __s32 speed);

/*
************************************************************************************************************************
*                                       LISTBAR_GetTotalItem
*
*Description: 获取当前焦点条目
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 返回总编号（失败返回-1）
*
*
************************************************************************************************************************
*/
__s32   LISTBAR_GetTotalItem(H_LBAR hdle);

/*
************************************************************************************************************************
*                                       LISTBAR_GetListAttr
*
*Description: 获取listbar私有数据
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 返回list私有数据（失败返回NULL）
*
*
************************************************************************************************************************
*/
void   *LISTBAR_GetListAttr(H_LBAR hdle);

/*
************************************************************************************************************************
*                                       LISTBAR_GetFocusItem
*
*Description: 获取当前焦点条目
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 返回焦点条目编号（失败返回-1）
*
*
************************************************************************************************************************
*/
__s32   LISTBAR_GetFocusItem(H_LBAR hdle);

/*
************************************************************************************************************************
*                                       LISTBAR_GetItemRect
*
*Description: 获取指定条目的RECT(只能是屏上显示的条目号)。
*
*Arguments  : hdle: 操作句柄，item：要获取的条目号，rect：获取后的区域
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_GetItemRect(H_LBAR hdle, __s32 item, RECT *rect);

/*
************************************************************************************************************************
*                                       LISTBAR_SetFocusItem
*
*Description: 设置一个条目焦点条目
*
*Arguments  : hdle: 操作句柄， item：条目编号
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_SetFocusItem(H_LBAR hdle, __s32 item);

/*
************************************************************************************************************************
*                                       LISTBAR_SetFocusItem
*
*Description: 设置一个条目焦点条目
*
*Arguments  : hdle: 操作句柄， item：条目编号
*
*author     : libaiao, 没有显示，只设置focus item
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_SetFocusItemExt(H_LBAR hdle, __s32 item);


extern void LISTBAR_LostFocus(H_LBAR *hdle);

/*
************************************************************************************************************************
*                                       LISTBAR_SetNormalItem
*
*Description: 获得一个条目为普通条目
*
*Arguments  : hdle: 操作句柄， item：条目编号
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_SetNormalItem(H_LBAR hdle, __s32 item);

/*
************************************************************************************************************************
*                                       LISTBAR_SetInvalidItem
*
*Description: 获得一个条目为无效条目
*
*Arguments  : hdle: 操作句柄， item：条目编号
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_SetInvalidItem(H_LBAR hdle, __s32 item);

/*
************************************************************************************************************************
*                                       LISTBAR_SetTotalItem
*
*Description: 设置总的条目数
*
*Arguments  : hdle: 操作句柄， num：条目总数
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_SetTotalItem(H_LBAR hdle, __u32 num);

/*
************************************************************************************************************************
*                                       LISTBAR_GetListAttr
*
*Description: 获取listbar私有数据
*
*Arguments  : hdle: 操作句柄，attr：用户私有数据
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_SetListAttr(H_LBAR hdle, void *attr);

/*
************************************************************************************************************************
*                                       LISTBAR_ChangeMode
*
*Description: 改变当前显示方式
*
*Arguments  : hdle: 操作句柄，item_widht：条目的宽度，item_height：条目的高度
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_ChangeMode(H_LBAR hdle, __s32 item_width, __s32 item_height);

/*
************************************************************************************************************************
*                                       LISTBAR_SuspendPage
*
*Description: 挂起当前显示界面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_SuspendPage(H_LBAR hdle);

/*
************************************************************************************************************************
*                                       LISTBAR_ResumePage
*
*Description: 恢复当前显示界面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_ResumePage(H_LBAR hdle);

/*
************************************************************************************************************************
*                                       LISTBAR_UpdatePage
*
*Description: 更新当前显示界面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_UpdatePage(H_LBAR hdle);

/*
************************************************************************************************************************
*                                       LISTBAR_SetScene
*
*Description: 设置listbar内部现场
*
*Arguments  : hdle: 操作句柄， scene：现场数据
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_SetScene(H_LBAR hdle, __listbar_scene_t *scene);

/*
************************************************************************************************************************
*                                       LISTBAR_GetScene
*
*Description: 获取listbar内部现场
*
*Arguments  : hdle: 操作句柄， scene：现场数据
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_GetScene(H_LBAR hdle, __listbar_scene_t *scene);

/*
**********************************************************************************************************************
*                                               LISTBAR_Delete
*
* Description: 删除listbar操作句柄
*
* Arguments  : hdle：操作句柄
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
**********************************************************************************************************************
*/
__s32   LISTBAR_Delete(H_LBAR hdle);

/*
**********************************************************************************************************************
*                                               LISTBAR_GetState
*
* Description: 删除listbar操作句柄
*
* Arguments  : hdle：操作句柄
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
**********************************************************************************************************************
*/
__lbar_state_e   LISTBAR_GetState(H_LBAR hdle);

/*
**********************************************************************************************************************
*                                               LISTBAR_GetItemNo
*
* Description: 获取指定位置的条目号
*
* Arguments  : hdle：操作句柄
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
**********************************************************************************************************************
*/
__s32   LISTBAR_GetItemNo(H_LBAR hdle, __pos_t pos);


/*
**********************************************************************************************************************
*                                               LISTBAR_GetPageItemCount
*
* Description   : 获得每页有多少个条目
*
* Arguments  : hdle：操作句柄
*
*Return         :条目数
*             EPDK_FAIL: 失败
*
*Author     : add by Libaiao, 2011.04.21
**********************************************************************************************************************
*/
__s32 LISTBAR_GetPageItemCount(H_LBAR handle);


/*
**********************************************************************************************************************
*                                               LISTBAR_GetRowItemCount
*
* Description   : 获得每行有多少个条目
*
* Arguments  : hdle：操作句柄
*
*Return         : 条目数
*             EPDK_FAIL: 失败
*
*Author     : add by Libaiao, 2011.04.21
**********************************************************************************************************************
*/
__s32 LISTBAR_GetRowItemCount(H_LBAR handle);

__s32 LISTBAR_Suspend_dif_task(H_LBAR handle);  //挂起diffuse 线程

__s32 LISTBAR_Resume_dif_task(H_LBAR handle);   // diffuse 解挂

#endif /* __LISTBAR_H__ */
