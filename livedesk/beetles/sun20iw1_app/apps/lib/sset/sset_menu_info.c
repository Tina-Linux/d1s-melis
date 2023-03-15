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
#include "sset_i.h"

//////////////////////////////////////////////////////////////////////////
#if 1 //(为了打印简单, 可把下面重定义的GUI宏临时拷到有需要的C文件中使用)
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

#define     ID_LISTITEM     1                   /* LISTITEM 条目上控件的ID 号       */

#define     COLOR_INFO_BACK     0xffababab          /* 焦点颜色                         */
#define     COLOR_INFO_RIM      0xffdddddd          /* 边框颜色                         */

/********** sset  info  position ***********/
typedef struct  taginfopos
{
    __s32               lcd_width;
    __s32               lcd_height;

    RECT                lyr_win;                /* LAYER 的RECT             */
    RECT                frm_win;                /* FRAME 的RECT             */

    RECT                bmp_pos;

} info_pos_t;

static  info_pos_t      info_pos;

/*
************************************************************************************************************************
*                                       calendar_main
*
*Description: 设置窗口内相关坐标
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static void  _info_pos_init(void)
{
    dsk_display_get_size(&info_pos.lcd_width, &info_pos.lcd_height);

    if (info_pos.lcd_width == 800 && info_pos.lcd_height == 480)
    {
        info_pos.lyr_win.x = 0;
        info_pos.lyr_win.y = 36;
        info_pos.lyr_win.width = 300;
        info_pos.lyr_win.height = 444;
        info_pos.frm_win.x = 0;
        info_pos.frm_win.y = 0;
        info_pos.frm_win.width = 300;
        info_pos.frm_win.height = 444;
        info_pos.bmp_pos.x = 30;
        info_pos.bmp_pos.y = 40;
        info_pos.bmp_pos.width = 240;
        info_pos.bmp_pos.height = 176;
    }
}

/*
************************************************************************************************************************
*                                       _draw_back
*
*Description: listmenu 的绘制函数
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static __s32 _draw_back(H_WIN h_win)
{
    H_WIN               h_lyr;
    RECT                item_rect;
    h_lyr = GUI_WinGetLyrWin(h_win);
    GUI_LyrWinSel(h_lyr);
    item_rect.x = info_pos.frm_win.x;
    item_rect.y = info_pos.frm_win.y;
    item_rect.width  = info_pos.frm_win.width - 2;
    item_rect.height = info_pos.frm_win.height;
    GUI_SetBkColor(COLOR_INFO_BACK);
    GUI_ClearRect(item_rect.x, item_rect.y, item_rect.x + item_rect.width, item_rect.y + item_rect.height);
    GUI_SetBkColor(COLOR_INFO_RIM);
    GUI_DrawVLine(item_rect.x + item_rect.width + 1, item_rect.y, item_rect.y + item_rect.height);
    GUI_SetBkColor(COLOR_INFO_BACK);
    GUI_DrawVLine(item_rect.x + item_rect.width + 2, item_rect.y, item_rect.y + item_rect.height);
    return 0;
}

/*
************************************************************************************************************************
*                                       _draw_back
*
*Description: listmenu 的绘制函数
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static __s32 _draw_info(H_WIN h_win, __u32 res_id)
{
    H_WIN                           h_lyr;
    //  void                            *pbmp = NULL;
    DECLARE_BMP_RES_Pt(pbmp);
    __sset_create_para_t        *psset;
    h_lyr = GUI_WinGetLyrWin(h_win);
    GUI_LyrWinSel(h_lyr);
    psset = (__sset_create_para_t *)GUI_WinGetAttr(GUI_WinGetParent(h_win));

    if (res_id)
    {
        //      pbmp = (void *)esMEMS_Balloc(info_pos.bmp_pos.width*info_pos.bmp_pos.height*4 + 0x100);
        //      GetRes(psset->icon_res,
        //             psset->icon_style,
        //             res_id,
        //             pbmp,
        //             info_pos.bmp_pos.width*info_pos.bmp_pos.height*4 + 0x100);
        Create_BMP_RES_Pt(pbmp, res_id);
        /*GUI_ARGB_Draw*/GUI_BMP_Draw(pbmp, info_pos.bmp_pos.x, info_pos.bmp_pos.y);
        //      esMEMS_Bfree(pbmp, info_pos.bmp_pos.width*info_pos.bmp_pos.height*4 + 0x100);
        Destroy_BMP_RES_Pt(pbmp);
    }

    pbmp = NULL;
    return 0;
}

/*
************************************************************************************************************************
*                                       _cb_info_win
*
*Description: info win回调
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static int _cb_info_win(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
            return EPDK_OK;

        case GUI_MSG_NOTIFY_CHILD_DELETED:
            break;

        case GUI_MSG_PAINT:
        {
            _draw_back(msg->h_deswin);
        }

        return EPDK_OK;

        case GUI_MSG_COMMAND:
            return EPDK_OK;

        case SSET_IN_INFO_UPDATE:
        {
            _draw_info(msg->h_deswin, msg->dwAddData1);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
            return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            GUI_FrmWinDelete(msg->h_deswin);
        }

        return EPDK_OK;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

/*
************************************************************************************************************************
*                                       SSET_MenuCreate
*
*Description:
*
*Arguments  : hparent 父窗口
*             pitem   本级菜单所对应的参数节点
*
*Return     :
*
************************************************************************************************************************
*/
H_WIN SSET_InfoCreate(H_WIN hparent)
{
    H_WIN                           h_win = 0 ;
    H_LYR                           h_lyr = 0 ;
    __gui_framewincreate_para_t     frmcreate_info;
    _info_pos_init();
    eLIBs_memset(&frmcreate_info, 0, sizeof(__gui_framewincreate_para_t));
    frmcreate_info.attr             = 0;
    frmcreate_info.BkColor.alpha    = 0;
    frmcreate_info.BkColor.red      = 0;
    frmcreate_info.BkColor.green    = 0;
    frmcreate_info.BkColor.blue     = 0;
    frmcreate_info.dwStyle          = WS_VISIBLE;
    frmcreate_info.dwExStyle        = 0;
    frmcreate_info.hLayer           = h_lyr;
    frmcreate_info.hOwner           = 0;
    frmcreate_info.hHosting         = hparent;
    frmcreate_info.FrameWinProc     = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_cb_info_win);
    frmcreate_info.rect.x           = info_pos.frm_win.x;
    frmcreate_info.rect.y           = info_pos.frm_win.y;
    frmcreate_info.rect.width       = info_pos.frm_win.width;
    frmcreate_info.rect.height      = info_pos.frm_win.height;
    h_win = GUI_FrmWinCreate(&frmcreate_info);

    if (!h_win)
    {
        __wrn("frame create create failed!\n");
        return NULL;
    }

    return h_win;
}

/*
************************************************************************************************************************
*                                       SSET_InfoDestroy
*
*Description: 销毁一级子菜单info
*
*Arguments  : cnt 菜单级数
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_InfoDestroy(H_WIN h_win)
{
    if (h_win)
    {
        GUI_FrmWinDelete(h_win);
        h_win = 0;
    }

    return EPDK_OK;
}
