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
#include <log.h>
#include "apps.h"
#include "tbar_com.h"
#include "GlideCurve.h"
#include "../sset.h"

#define     MOVIE_SSET_ID                       0x80
#define     ID_FM_SCENE_SSET               10

//////////////////////////////////////////////////////////////////////////
#if 0 //(为了打印简单, 可把下面重定义的GUI宏临时拷到有需要的C文件中使用)
//re define GUI_BMP_Draw
#ifdef GUI_BMP_Draw
#define MY_GUI_BMP_DRAW                      (ORANGEMOD_FUNC_ENTRY->ptr_GUI_BMP_Draw            )
#undef GUI_BMP_Draw
#define GUI_BMP_Draw(_p, _x, _y)             do \
    {\
        {__msg("    GUI_BMP_Draw(%d, %d)\n", _x, _y);} \
        MY_GUI_BMP_DRAW(_p, _x, _y); \
    }while(0)
#endif

//re define GUI_DispStringAt
#ifdef GUI_DispStringAt
#define MY_GUI_DispStringAt                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringAt        )
#undef GUI_DispStringAt
#define GUI_DispStringAt(_p, _x, _y)         do\
    { \
        {__msg("    GUI_DispStringAt(%d, %d)->( %s )\n", _x, _y, _p);} \
        MY_GUI_DispStringAt(_p, _x, _y); \
    }while(0)
#endif

//re define GUI_DispStringInRect
#ifdef GUI_DispStringInRect
#define MY_GUI_DispStringInRect              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringInRect    )
#undef GUI_DispStringInRect
#define GUI_DispStringInRect(_p, _prt, _fmt)  do \
    { \
        {__msg("    GUI_DispStringInRect(%d, %d, %d, %d)->( %s )\n", (_prt)->x0, (_prt)->y0, (_prt)->x1, (_prt)->y1, _p);} \
        MY_GUI_DispStringInRect(_p, _prt, _fmt); \
    } while (0)
#endif

#endif
//////////////////////////////////////////////////////////////////////////

/**********************************************************************************************************************/
#define     TB_BACKCOLOR        GUI_BLACK// 0x4c6a3906
#define     TB_TV_BACKCOLOR     0x00000000
#define     STEP_ARRAY          128
#define     SCR_ITEMS           6

static __u32    last_key    = 0xffffffff;
/**********************************************************************************************************************/
typedef enum tag_glide_state
{
    GLIDE_IDLE,
    GLIDE_START,
    GLIDE_MOVE,
    GLIDE_STOP
} glide_state_t;

typedef struct tag_tbar_ctrl
{
    tbar_para_t *data;

    __s32       max_item;       // 单屏最多显示 item 数目
    __s32       scn_in;         // 当前屏幕最左边 item index;
    __s32       scn_out;        // 当前屏幕最右边 item index;
    __s32       old_focus;      // 旧的焦点index
    __s32       new_focus;      // 新的焦点index

    __bool      init_flag;
    //  __s32       min_x, max_x;   // 图层x坐标, 最大最小值
    __pos_t     win_pos;
    __bool      touch_down;

    /* 弹性曲线 */
    int         step[STEP_ARRAY];// touch 滑动步长数组
    int         len;             // 数组内有效数据长度
    int         index;           // 图层移动计数

    __u32       tid;             // 图层移动线程
    __krnl_event_t *sem;         // 同步信号量
    __pos_t     win_start;       // 起始点对应的窗口坐标
    H_LYR       lyr;
    __gcuv_para_t gcurv;         // gcurv 参数

    glide_state_t gstate;        // tbar 移动状态
    __u16       h_ssetid;               //所属窗口判断标志
} tbar_ctrl_t;

/**********************************************************************************************************************/

static void  set_language_gui_charset(void)
{
    GUI_UC_SetEncodeUTF8();
}

__epdk_charset_enm_e get_language_charset(void)
{
    return dsk_get_langres_charset();
}

static __s32 tbar_on_key(__gui_msg_t *msg)
{
    H_WIN           layer;
    tbar_ctrl_t     *pCtl;
    __s32           ret = EPDK_OK;
    pCtl = (tbar_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        //      switch( msg->dwAddData1 )
        switch (last_key)
        {
            case GUI_MSG_KEY_ENTER:
                //          case GUI_MSG_KEY_LONGENTER:
            {
                __gui_notify_msg_t  notify_msg;                                 // 向上发通知码
                last_key = 0xffffffff;
                notify_msg.hWnd     = msg->h_deswin;
                notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                notify_msg.msgcode  = TB_UNPUSHED;
                notify_msg.dwAddData = pCtl->new_focus; //pCtl->data->item[pCtl->new_focus].type ;    // item id
                GUI_NotifyParent(&notify_msg);
                break;
            }

            default:
                break;
        }
    }
    else
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_ENTER:
            {
                __gui_notify_msg_t  notify_msg;                                 // 向上发通知码
                last_key = GUI_MSG_KEY_ENTER;
                notify_msg.hWnd     = msg->h_deswin;
                notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                notify_msg.msgcode  = TB_PUSHED;
                notify_msg.dwAddData = pCtl->new_focus ; // item id
                GUI_NotifyParent(&notify_msg);
            }
            break;

            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT:
            {
                /*if(pCtl->new_focus >=(pCtl->data->item_nr -1))//..
                {

                    pCtl->new_focus -=(pCtl->data->item_nr -1);
                }*/
                __wrn("------ test menulist 5\n");
                __wrn("----pCtl->data->item_nr=%d\n", pCtl->data->item_nr);

                if (pCtl->new_focus == (pCtl->data->item_nr - 1))
                {
                    RECT                rect;
                    pCtl->old_focus = pCtl->data->item_nr - 1;
                    pCtl->new_focus = 0;
                    layer = GUI_WinGetLyrWin(msg->h_deswin);
                    GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
                    GUI_LyrWinCacheOn();
                    GUI_LyrWinGetSrcWindow(layer, &rect);
                    rect.x = 2 * pCtl->data->frame_size.width;
                    GUI_LyrWinSetSrcWindow(layer, &rect);
                    GUI_LyrWinCacheOff();
                    {
                        __gui_notify_msg_t  notify_msg;
                        notify_msg.hWnd     = msg->h_deswin;
                        notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                        notify_msg.msgcode  = TB_SWITCH;
                        notify_msg.dwAddData = pCtl->new_focus;                     // item id
                        GUI_NotifyParent(&notify_msg);
                    }
                }
                else if (pCtl->new_focus < (pCtl->data->item_nr - 1))
                {
                    RECT                rect;
                    int                 offset;
                    layer = GUI_WinGetLyrWin(msg->h_deswin);
                    pCtl->old_focus = pCtl->new_focus;
                    pCtl->new_focus++;
                    __wrn("pCtl->old_focus=%d   pCtl->new_focus=%d\n", pCtl->old_focus, pCtl->new_focus);
                    GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
                    GUI_LyrWinGetSrcWindow(layer, &rect);
                    __wrn("----11rect.x=%d\n", rect.x);
                    offset = (pCtl->new_focus + 3) * pCtl->data->frame_size.width ;
                    __msg("offset = %d, x = %d, width = %d \n", offset, rect.x, pCtl->data->scn_width);

                    if (offset > (rect.x + pCtl->data->scn_width))//
                    {
                        GUI_LyrWinCacheOn();
                        rect.x += pCtl->data->frame_size.width;
                        __wrn("----2222rect.x=%d\n", rect.x);
                        GUI_LyrWinSetSrcWindow(layer, &rect);
                        GUI_LyrWinCacheOff();
                    }

                    {
                        __gui_notify_msg_t  notify_msg;
                        notify_msg.hWnd     = msg->h_deswin;
                        notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                        notify_msg.msgcode  = TB_SWITCH;
                        notify_msg.dwAddData = pCtl->new_focus;                     // item id
                        GUI_NotifyParent(&notify_msg);
                    }
                }
            }
            break;

            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_LONGLEFT:
            {
                /*if(pCtl->new_focus == 0)
                {

                    pCtl->new_focus += (pCtl->data->item_nr -1);
                }*/
                __wrn("------ test menulist pCtl->new_focus=%d\n", pCtl->new_focus);

                if (pCtl->new_focus == 0 && pCtl->data->item_nr > 6)
                {
                    RECT                rect;
                    pCtl->old_focus = 0;
                    pCtl->new_focus = pCtl->data->item_nr - 1;
                    layer = GUI_WinGetLyrWin(msg->h_deswin);
                    GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
                    GUI_LyrWinCacheOn();
                    GUI_LyrWinGetSrcWindow(layer, &rect);
                    rect.x = (pCtl->data->item_nr - SCR_ITEMS + 2) * pCtl->data->frame_size.width;
                    GUI_LyrWinSetSrcWindow(layer, &rect);
                    GUI_LyrWinCacheOff();
                    {
                        __gui_notify_msg_t  notify_msg;
                        notify_msg.hWnd     = msg->h_deswin;
                        notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                        notify_msg.msgcode  = TB_SWITCH;
                        notify_msg.dwAddData = pCtl->new_focus;                     // item id
                        GUI_NotifyParent(&notify_msg);
                    }
                }
                else if (pCtl->new_focus >= 0)
                {
                    RECT                rect;
                    int                 offset;

                    if (pCtl->new_focus == 0 && pCtl->data->item_nr <= 6)
                    {
                        pCtl->old_focus = 0;
                        pCtl->new_focus = pCtl->data->item_nr - 1;
                    }
                    else
                    {
                        pCtl->old_focus = pCtl->new_focus;
                        pCtl->new_focus--;
                    }

                    layer = GUI_WinGetLyrWin(msg->h_deswin);
                    __wrn("pCtl->old_focus=%d   pCtl->new_focus=%d\n", pCtl->old_focus, pCtl->new_focus);
                    GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
                    GUI_LyrWinGetSrcWindow(layer, &rect);
                    offset = (pCtl->new_focus + 2) * (pCtl->data->frame_size.width);
                    __wrn("...rect.x=%d\n", rect.x);
                    __msg("offset = %d, x = %d, width = %d \n", offset, rect.x, pCtl->data->scn_width);

                    if (offset < rect.x)
                    {
                        GUI_LyrWinCacheOn();
                        rect.x -= pCtl->data->frame_size.width;
                        GUI_LyrWinSetSrcWindow(layer, &rect);
                        GUI_LyrWinCacheOff();
                    }

                    {
                        __gui_notify_msg_t  notify_msg;
                        notify_msg.hWnd     = msg->h_deswin;
                        notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                        notify_msg.msgcode  = TB_SWITCH;
                        notify_msg.dwAddData = pCtl->new_focus;                     // item id
                        GUI_NotifyParent(&notify_msg);
                    }
                }
            }

            default:
                break;
        }
    }

    return ret;
}


static __s32 point_to_item_id(tbar_ctrl_t *pCtl, __s32 x, __s32 y)
{
    __s32 index;
    tbar_para_t *data;
    data    = pCtl->data;
    index   = x / (data->frame_size.width);

    if (index < 0)
    {
        index = 0;
    }

    if (index >= pCtl->data->item_nr + 1)
    {
        index = pCtl->data->item_nr + 1;
    }

    return index;
}


static void tbar_glide_thread(void *arg)
{
    tbar_ctrl_t *glide = (tbar_ctrl_t *)arg;
    __u8         err;
    int          step;
    RECT         rect;
    __bool       flag = EPDK_FALSE;

    while (1)
    {
        if (esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
        {
            esKRNL_TDel(OS_PRIO_SELF);
            return;
        }

        esKRNL_SemPend(glide->sem, 0, &err);

        if (glide->index < glide->len)
        {
            step = glide->step[glide->index];
            glide->index++;
            flag = EPDK_TRUE;
        }

        esKRNL_SemPost(glide->sem);

        if (flag == EPDK_TRUE)
        {
            GUI_LyrWinGetSrcWindow(glide->lyr, &rect);
            rect.x = 2 * (glide->data->frame_size.width) - step;
            GUI_LyrWinSetSrcWindow(glide->lyr, &rect);
            flag = EPDK_FALSE;
            continue;
        }
        else
        {
            esKRNL_TimeDly(3);    /* delay 30ms */
        }
    }
}

/* touch msg proc */
static __s32 tbar_on_touch(__gui_msg_t *msg)
{
    tbar_ctrl_t *pCtl;
    __s32        ret;
    __s32        item_id;
    __pos_t      win_pos;
    __pos_t      scn_pos;
    RECT         src_rect;
    ret  = EPDK_OK;
    pCtl = (tbar_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    win_pos.x = LOSWORD(msg->dwAddData2);
    win_pos.y = HISWORD(msg->dwAddData2);
    GUI_WindowToScreen(msg->h_deswin, &win_pos, &scn_pos);
    item_id = point_to_item_id(pCtl, win_pos.x, win_pos.y);

    if (pCtl->gstate == GLIDE_IDLE)
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_TOUCH_DOWN:
            {
                if (GUI_WinGetCaptureWin() != msg->h_deswin)        // 设置capture
                {
                    GUI_WinSetCaptureWin(msg->h_deswin);
                }

                if (pCtl->index != pCtl->len)
                {
                    __u8    err;
                    esKRNL_SemPend(pCtl->sem, 0, &err);
                    pCtl->index = pCtl->len;
                    esKRNL_SemPost(pCtl->sem);
                    pCtl->gstate = GLIDE_STOP;
                }
                else
                {
                    pCtl->win_start.x = win_pos.x;
                    pCtl->win_start.y = win_pos.y;
                    GUI_LyrWinGetSrcWindow(pCtl->lyr, &src_rect);
                    pCtl->gcurv.fb_start = 2 * (pCtl->data->frame_size.width) - src_rect.x;
                    glidecuv_init(&(pCtl->gcurv));
                    glidecuv_start(scn_pos.x);
                    //__msg("pCtl->gcurv.fb_start = %d \n", pCtl->gcurv.fb_start);
                    //__msg(" glidecuv_start : scn_pos.x = %d \n", scn_pos.x);
                    /*  绘制焦点 */
                    pCtl->old_focus = pCtl->new_focus;
                    pCtl->new_focus = item_id - 2;
                    GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
                    pCtl->gstate = GLIDE_START;
                    {
                        __gui_notify_msg_t  notify_msg;                 // 向上发通知码
                        notify_msg.hWnd      = msg->h_deswin;
                        notify_msg.id        = GUI_WinGetItemId(msg->h_deswin);
                        notify_msg.msgcode   = TB_PUSHED;
                        notify_msg.dwAddData = pCtl->data->item[item_id - 2].type ;
                        GUI_NotifyParent(&notify_msg);
                    }
                }
            }
            break;

            default:
                break;
        }
    }
    else if (pCtl->gstate == GLIDE_START)
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_TOUCH_UP:
            {
                __pos_t scn_start;
                __u8    err;

                if (GUI_WinGetCaptureWin() == msg->h_deswin)             // 设置capture
                {
                    GUI_WinReleaseCapture();
                }

                GUI_WindowToScreen(msg->h_deswin, &(pCtl->win_start), &scn_start);
                esKRNL_SemPend(pCtl->sem, 0, &err);
                pCtl->len = glidecuv_up(scn_pos.x, scn_start.x, LOSWORD(msg->dwReserved), HISWORD(msg->dwReserved), pCtl->step, STEP_ARRAY);
                pCtl->index = 0;
                esKRNL_SemPost(pCtl->sem);
                pCtl->gstate = GLIDE_IDLE;
                {
                    __gui_notify_msg_t  notify_msg;
                    notify_msg.hWnd      = msg->h_deswin;
                    notify_msg.id        = GUI_WinGetItemId(msg->h_deswin);
                    notify_msg.msgcode   = TB_UNPUSHED;
                    notify_msg.dwAddData = item_id - 2;
                    GUI_NotifyParent(&notify_msg);
                }
            }
            break;

            case GUI_MSG_TOUCH_MOVE:
            {
                __pos_t scn_start;
                __u8    err;
                GUI_WindowToScreen(msg->h_deswin, &(pCtl->win_start), &scn_start);
                esKRNL_SemPend(pCtl->sem, 0, &err);
                pCtl->len = glidecuv_move(scn_pos.x, scn_start.x, pCtl->step, STEP_ARRAY);
                pCtl->index = 0;
                esKRNL_SemPost(pCtl->sem);
                pCtl->gstate = GLIDE_MOVE;
            }
            break;

            default:
                break;
        }
    }
    else if (pCtl->gstate == GLIDE_MOVE)
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_TOUCH_MOVE:
            {
                __pos_t scn_start;
                __u8    err;
                GUI_WindowToScreen(msg->h_deswin, &(pCtl->win_start), &scn_start);
                esKRNL_SemPend(pCtl->sem, 0, &err);
                pCtl->len = glidecuv_move(scn_pos.x, scn_start.x, pCtl->step, STEP_ARRAY);
                pCtl->index = 0;
                esKRNL_SemPost(pCtl->sem);
            }
            break;

            case GUI_MSG_TOUCH_UP:
            {
                __pos_t scn_start;
                __u8    err;

                if (GUI_WinGetCaptureWin() == msg->h_deswin)             // 设置capture
                {
                    GUI_WinReleaseCapture();
                }

                GUI_WindowToScreen(msg->h_deswin, &(pCtl->win_start), &scn_start);
                pCtl->gstate = GLIDE_IDLE;
                esKRNL_SemPend(pCtl->sem, 0, &err);
                pCtl->len = glidecuv_up(scn_pos.x, scn_start.x, LOSWORD(msg->dwReserved), HISWORD(msg->dwReserved), pCtl->step, STEP_ARRAY);
                pCtl->index = 0;
                esKRNL_SemPost(pCtl->sem);
            }
            break;

            default:
                break;
        }
    }
    else if (pCtl->gstate == GLIDE_STOP)
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_TOUCH_MOVE:
            {
                __pos_t scn_start;
                __u8    err;
                pCtl->win_start.x = win_pos.x;
                pCtl->win_start.y = win_pos.y;
                GUI_LyrWinGetSrcWindow(pCtl->lyr, &src_rect);
                pCtl->gcurv.fb_start = 2 * (pCtl->data->frame_size.width) - src_rect.x;
                glidecuv_init(&(pCtl->gcurv));
                glidecuv_start(scn_pos.x);
                GUI_WindowToScreen(msg->h_deswin, &(pCtl->win_start), &scn_start);
                esKRNL_SemPend(pCtl->sem, 0, &err);
                pCtl->len = glidecuv_move(scn_pos.x, scn_start.x, pCtl->step, STEP_ARRAY);
                pCtl->index = 0;
                esKRNL_SemPost(pCtl->sem);
                pCtl->gstate = GLIDE_MOVE;
            }
            break;

            case GUI_MSG_TOUCH_UP:
            {
                __pos_t scn_start;
                __u8    err;

                if (GUI_WinGetCaptureWin() == msg->h_deswin)             // 设置capture
                {
                    GUI_WinReleaseCapture();
                }

                pCtl->win_start.x = win_pos.x;
                pCtl->win_start.y = win_pos.y;
                GUI_LyrWinGetSrcWindow(pCtl->lyr, &src_rect);
                pCtl->gcurv.fb_start = 2 * (pCtl->data->frame_size.width) - src_rect.x;
                glidecuv_init(&(pCtl->gcurv));
                glidecuv_start(scn_pos.x);
                GUI_WindowToScreen(msg->h_deswin, &(pCtl->win_start), &scn_start);
                pCtl->gstate = GLIDE_IDLE;
                esKRNL_SemPend(pCtl->sem, 0, &err);
                pCtl->len = glidecuv_up(scn_pos.x, scn_start.x, LOSWORD(msg->dwReserved), HISWORD(msg->dwReserved), pCtl->step, STEP_ARRAY);
                pCtl->index = 0;
                esKRNL_SemPost(pCtl->sem);
            }
            break;

            default:
                break;
        }
    }

    return ret;
}

static __s32 tbar_paint_bg(H_LYR layer, void *bg_bmp, __s32 src_pos)
{
    __s32 bmp_w;
    FB fb;
    GUI_LyrWinGetFB(layer, &fb);
    //__here__

    if (GUI_LYRWIN_STA_SUSPEND == GUI_LyrWinGetSta(layer))
    {
        return EPDK_OK;
    }

    bmp_w = GUI_BMP_GetXSize(bg_bmp);

    if (bmp_w > fb.size.width || bmp_w <= 0)
    {
        return EPDK_OK;
    }

    {
        __s32  m = 2 * src_pos; //..
        __s32 i, n;
        n = fb.size.width / bmp_w;

        if (n <= 4)
        {
            GUI_BMP_Draw(bg_bmp, m, 0);//..
        }
        else
        {
            for (i = 0 ; i < n ; i++)
            {
                GUI_BMP_Draw(bg_bmp, i * src_pos, 0);
            }

            GUI_BMP_Draw(bg_bmp, fb.size.width - src_pos, 0);
        }
    }

    return EPDK_OK;
}

static __s32 tbar_on_paint(__gui_msg_t *msg)
{
    H_LYR           layer;
    FB              fb;
    __gui_rect_t    rcrect;
    RECT            fbrect;
    tbar_ctrl_t     *pCtl = NULL;
    tbar_para_t     *data = NULL;
    __s32           ret = EPDK_OK;
    __s32           i;
    __s32           width;
    __s32           height;
    pCtl = (tbar_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    data = pCtl->data;
    ////__here__
    layer = (H_LYR)GUI_WinGetLyrWin(msg->h_deswin);
    GUI_LyrWinGetFB(layer, &fb);

    //__here__
    if (GUI_LYRWIN_STA_SUSPEND == GUI_LyrWinGetSta(layer))
    {
        return EPDK_OK;
    }

    ////__here__
    fbrect.x = 0;
    fbrect.y = 0;
    fbrect.width  = fb.size.width;
    fbrect.height = fb.size.height;
    ////__here__
    GUI_LyrWinSel(layer);
    GUI_LyrWinSetTop(layer);//add by lihaoyi

    ////__here__
    if (pCtl->init_flag == EPDK_TRUE)       // 初始化时，绘制所有icon
    {
        /* 清背景 */
        ////__here__
        //      GUI_SetBkColor(TB_BACKCOLOR);
        //      GUI_ClearRect(0,0,fbrect.width,1);
        //      GUI_SetBkColor(0xBF4B4B4B);
        //      GUI_ClearRect(0,1,fbrect.width,2);
        dsk_display_get_size(&width, &height);

        if (width == 800 && height == 480)
        {
            ////__here__
            GUI_SetBkColor(TB_BACKCOLOR);
        }
        else
        {
            GUI_SetBkColor(TB_TV_BACKCOLOR);
        }

        GUI_ClearRect(fbrect.x, fbrect.y, fbrect.x + fbrect.width - 1, fbrect.y + fbrect.height - 1);
        //__here__

        if (data->bg_bmp)
        {
            tbar_paint_bg(layer, data->bg_bmp, data->frame_size.width);//..
        }

        //菜单背景
        for (i = 0; i < data->item_nr; i++)
        {
            rcrect.left     = fbrect.x + data->src_start + (i + 2) * (data->frame_size.width);
            rcrect.top      = fbrect.y;
            rcrect.right    = rcrect.left + data->frame_size.width - 1;
            rcrect.bottom   = rcrect.top  + data->frame_size.height - 1;

            if (pCtl->new_focus == i)
            {
                //__here__
                if (data->item[i].focus_bmp)
                {
                    //__here__
                    /*GUI_ARGB_Draw*/
                    GUI_BMP_Draw(data->item[i].focus_bmp, rcrect.left + data->bmp_pos.x, rcrect.top + data->bmp_pos.y);
                    //__here__
                }

                if (data->draw_font)
                {
                    GUI_RECT txtrect;
                    __wrn("rcrect.top=%d\n", rcrect.top);
                    __wrn("rcrect.bottom=%d\n", rcrect.bottom);
                    txtrect.x0 = rcrect.left;
                    txtrect.y0 = rcrect.top + data->text_pos.y;
                    txtrect.x1 = rcrect.right;
                    txtrect.y1 = rcrect.bottom;

                    if (pCtl->data->h_ssetid == ID_FM_SCENE_SSET)
                    {
                        txtrect.x0 = rcrect.left + data->bmp_pos.x;
                    }

                    GUI_OpenAlphaBlend();
                    GUI_CharSetToEncode(dsk_get_langres_charset());
                    GUI_SetFont(data->draw_font);
                    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
                    GUI_SetColor(data->focus_color);
                    __wrn("data->focus_color=0x%x\n", data->focus_color);
                    __wrn("data->item[i].text=%s\n", data->item[i].text);
                    GUI_DispStringInRect(data->item[i].text, &txtrect, GUI_TA_HCENTER | GUI_TA_VCENTER);
                    GUI_LyrWinFlushFB(layer);
                    GUI_CloseAlphaBlend();
                    //__here__
                }
            }
            else
            {
                if (data->item[i].unfocus_bmp)
                {
                    /*GUI_ARGB_Draw*/
                    GUI_BMP_Draw(data->item[i].unfocus_bmp, rcrect.left + data->bmp_pos.x, rcrect.top + data->bmp_pos.y);
                }

                if (data->draw_font)
                {
                    GUI_RECT txtrect;
                    txtrect.x0 = rcrect.left;
                    txtrect.y0 = rcrect.top + data->text_pos.y;
                    txtrect.x1 = rcrect.right;
                    txtrect.y1 = rcrect.bottom;

                    if (pCtl->data->h_ssetid == ID_FM_SCENE_SSET)
                    {
                        txtrect.x0 =  rcrect.left + data->bmp_pos.x;
                    }

                    GUI_OpenAlphaBlend();
                    GUI_CharSetToEncode(dsk_get_langres_charset());
                    GUI_SetFont(data->draw_font);
                    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
                    GUI_SetColor(data->unfocus_color);
                    __wrn("data->unfocus_color=0x%x\n", data->unfocus_color);
                    __wrn("data->item[i].text=%s\n", data->item[i].text);
                    GUI_DispStringInRect(data->item[i].text, &txtrect, GUI_TA_HCENTER | GUI_TA_VCENTER);
                    GUI_LyrWinFlushFB(layer);
                    GUI_CloseAlphaBlend();
                }
            }
        }

        pCtl->init_flag = EPDK_FALSE;
    }
    else
    {
        // 绘制 old_focus icon
        rcrect.left     = fbrect.x + data->src_start + (pCtl->old_focus + 2) * (data->frame_size.width);
        rcrect.top      = fbrect.y;
        rcrect.right    = rcrect.left + data->frame_size.width - 1;
        rcrect.bottom   = rcrect.top  + data->frame_size.height - 1;

        if (data->item[pCtl->old_focus].unfocus_bmp)
        {
            /*GUI_ARGB_Draw*/
            GUI_BMP_Draw(data->item[pCtl->old_focus].unfocus_bmp, rcrect.left + data->bmp_pos.x, rcrect.top + data->bmp_pos.y);
        }

        if (data->draw_font)
        {
            GUI_RECT txtrect;
            txtrect.x0 = rcrect.left;
            txtrect.y0 = rcrect.top + data->text_pos.y;
            txtrect.x1 = rcrect.right;
            txtrect.y1 = rcrect.bottom;

            if (pCtl->data->h_ssetid == ID_FM_SCENE_SSET)
            {
                txtrect.x0 = rcrect.left + data->bmp_pos.x;
            }

            set_language_gui_charset();
            GUI_SetFont(data->draw_font);
            GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
            GUI_SetColor(data->unfocus_color);
            GUI_DispStringInRect(data->item[pCtl->old_focus].text, &txtrect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }

        // 绘制 new icon
        rcrect.left     = fbrect.x + data->src_start + (pCtl->new_focus + 2) * (data->frame_size.width);
        rcrect.top      = fbrect.y;
        rcrect.right    = rcrect.left + data->frame_size.width - 1;
        rcrect.bottom   = rcrect.top  + data->frame_size.height - 1;

        if (pCtl->data->h_ssetid == MOVIE_SSET_ID)
        {
            GUI_BMP_Draw(data->bg_bmp, rcrect.left, rcrect.top);//..
        }

        if (data->item[pCtl->new_focus].focus_bmp)
        {
            /*GUI_ARGB_Draw*/GUI_BMP_Draw(data->item[pCtl->new_focus].focus_bmp, rcrect.left + data->bmp_pos.x, rcrect.top + data->bmp_pos.y);
        }

        if (data->draw_font)
        {
            GUI_RECT txtrect;
            txtrect.x0 = rcrect.left;
            txtrect.y0 = rcrect.top + data->text_pos.y;
            txtrect.x1 = rcrect.right;
            txtrect.y1 = rcrect.bottom;

            if (pCtl->data->h_ssetid == ID_FM_SCENE_SSET)
            {
                txtrect.x0 = rcrect.left + data->bmp_pos.x;
            }

            set_language_gui_charset();
            GUI_SetFont(data->draw_font);
            GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
            GUI_SetColor(data->focus_color);
            GUI_DispStringInRect(data->item[pCtl->new_focus].text, &txtrect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
    }

    return ret;
}


/*
 * tool bar 窗口响应函数
 */
static __s32 cb_tbar_frm(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
            return 0;

        case GUI_MSG_DESTROY:
        {
            tbar_ctrl_t *p_ctr;
            __u8        err;
            p_ctr = (tbar_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            /* 删除线程 */
            while (esKRNL_TDelReq(p_ctr->tid) != OS_TASK_NOT_EXIST)
            {
                esKRNL_TimeDly(2);
            }

            /* 删除信号量*/
            esKRNL_SemDel(p_ctr->sem, 0, &err);
            esMEMS_Mfree(0, p_ctr);
        }

        return 0;

        case GUI_MSG_KEY:
        {
            __wrn("------ test menulist 4\n");
            tbar_on_key(msg);
        }

        return 0;

        case GUI_MSG_TOUCH:
        {
            tbar_on_touch(msg);
        }

        return 0;

        case TB_SET_UPDATE_LANGUAGE:
        case GUI_MSG_WIN_WAKEUP:
        {
            tbar_ctrl_t *p_ctr;
            p_ctr = (tbar_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
            p_ctr->init_flag = EPDK_TRUE;
            GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
            return 0;
        }

        case GUI_MSG_PAINT:
            //__here__;
            tbar_on_paint(msg);
            //__here__;
            return 0;

        case SSET_STRING_UPDATE://..
        {
            //
            tbar_ctrl_t     *pCtl = NULL;
            tbar_para_t     *data = NULL;
            char            title[GUI_TITLE_MAX] = {0};
            __gui_msg_t     dmsg;
            __u32   index = msg->dwAddData2;
            __u32   seek  = msg->dwReserved;
            __msg("seek = %d\n", seek);
            __msg("index = %d\n", index);
            __msg("+++++++SSET_STRING_UPDATE+++++++\n");
            pCtl = (tbar_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
            data = pCtl->data;
            dsk_langres_get_menu_text(msg->dwAddData1, title, sizeof(title));
            __msg("\n\n title = %s\n\n", title);

            if (index == 2)
            {
                //__here__;
                eLIBs_memcpy(data->item[pCtl->new_focus + seek].text, title, sizeof(title));
            }
            else if (index == 4)
            {
                //__here__;
                eLIBs_memcpy(data->item[pCtl->new_focus - seek].text, title, sizeof(title));
            }
            else
            {
                //__here__;
                eLIBs_memcpy(data->item[pCtl->new_focus].text, title, sizeof(title));
            }

            //__here__;
            tbar_on_paint(msg);
            return 0;
        }

        case SSET_ITEM_UPDATE://..
        {
#if 1
            tbar_ctrl_t    *pCtl = NULL;
            tbar_para_t     *data = NULL;
            char            title[GUI_TITLE_MAX] = {0};
            __u32 icon_id = msg->dwAddData1;
            __u32 index   = msg->dwAddData2;
            __u32 fcs_sta = msg->dwReserved;
            __msg("icon_id =  %d\n", icon_id);
            __msg("index =  %d\n", index);
            __msg("fcs_sta =  %d\n", fcs_sta);
            __msg("+++++++SSET_ITEM_UPDATE+++++++\n");
            SSET_item_update(icon_id, index, fcs_sta);
            tbar_on_paint(msg);
#endif
            return 0;
        }

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

/* 创建激活态 tool bar */
H_WIN tbar_frm_create(H_WIN parent, H_WIN layer, tbar_para_t *tbar_para)
{
    H_WIN                       h_win;
    __gui_framewincreate_para_t para;
    tbar_ctrl_t                 *p_ctr;
    FB                          fb;
    //__here__
    eLIBs_memset(&para, 0, sizeof(__gui_framewincreate_para_t));
    para.dwStyle          = WS_VISIBLE;
    para.dwExStyle        = 0;
    para.name             = NULL;
    para.hOwner           = 0;
    para.hHosting         = parent;
    para.FrameWinProc     = cb_tbar_frm;
    para.id               = ID_TOOL_BAR;
    para.hLayer           = layer;
    GUI_LyrWinGetFB(layer, &fb);
    para.rect.x           = 0;
    para.rect.y           = 2;
    para.rect.width       = fb.size.width;
    para.rect.height      = fb.size.height - 2;
    para.BkColor.alpha    = 0;
    para.BkColor.red      = 0;
    para.BkColor.green    = 0;
    para.BkColor.blue     = 0;
    p_ctr = (tbar_ctrl_t *)esMEMS_Malloc(0, sizeof(tbar_ctrl_t));

    if (!p_ctr)
    {
        __wrn(" tbar_ctrl_t maollc error \n");
        return NULL;
    }

    eLIBs_memset(p_ctr, 0, sizeof(tbar_ctrl_t));
    //__here__
    p_ctr->data         = tbar_para;
    p_ctr->init_flag    = EPDK_TRUE;
    p_ctr->touch_down   = EPDK_FALSE;
    p_ctr->max_item     = p_ctr->data->scn_width / p_ctr->data->frame_size.width;
    p_ctr->scn_in       = 0;
    p_ctr->scn_out      = p_ctr->max_item - 1;
    p_ctr->old_focus    = p_ctr->data->focus_item;
    p_ctr->new_focus    = p_ctr->old_focus;
    //  p_ctr->min_x        = para.rect.x;
    //  p_ctr->max_x        = para.rect.x + (p_ctr->data->item_nr - p_ctr->max_item + 1)*p_ctr->data->frame_size.width;
    p_ctr->sem          = esKRNL_SemCreate(1);
    p_ctr->lyr          = layer;
    p_ctr->gcurv.screen     = tbar_para->scn_width;
    //p_ctr->gcurv.fb_len   = fb.size.width - 4*(p_ctr->data->frame_size.width);
    p_ctr->gcurv.fb_len     = (tbar_para->item_nr) * (tbar_para->frame_size.width);
    p_ctr->gcurv.fb_start   = 0;
    p_ctr->gcurv.frame      = p_ctr->data->frame_size.width;
    p_ctr->gstate       = GLIDE_IDLE;
    p_ctr->tid          = esKRNL_TCreate(tbar_glide_thread, (void *)p_ctr, 0x1000, KRNL_priolevel3);
    //__here__
    para.attr           = (void *)p_ctr;
    h_win = GUI_FrmWinCreate(&para);

    if (!h_win)
    {
        __err("tbar fwin create error ! \n");
    }

    //__here__
    return h_win;
}

/* 删除激活态 tool bar */
__s32 tbar_frm_delete(H_WIN frmwin)
{
    GUI_FrmWinDelete(frmwin);
    return EPDK_TRUE;
}
