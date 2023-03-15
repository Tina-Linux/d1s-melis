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
#include "ebook_tips.h"
#include "ebook_uipara.h"

void *My_Malloc(void *heap, __u32 size);

typedef struct tag_tip_ctrl
{
    ebook_tip_para_t *tips_para;
    __s32 focus_txt_color;
    __s32 unfocus_txt_color;
    __s32 tips_old_focus;
    __s32 tips_new_focus;
    H_THEME tip_bmp;
    H_THEME tip_f_bmp;
    H_THEME tip_uf_bmp;
    char no[128];       //string no
    char yes[128];      //string yes
    char tips[128];     //breakpoint tips
} ebook_tips_ctrl_t;

static void ebook_tip_init(ebook_tips_ctrl_t *tips_ctrl)
{
    tips_ctrl->tip_bmp = theme_open(NULL);
    tips_ctrl->tip_uf_bmp = theme_open(NULL);
    tips_ctrl->tip_f_bmp = theme_open(NULL);
    get_menu_text(STRING_EBOOK_TIPS_NO, tips_ctrl->no, 128);    //
    get_menu_text(STRING_EBOOK_TIPS_YES, tips_ctrl->yes, 128);//
    get_menu_text(STRING_EBOOK_BKPOINT, tips_ctrl->tips, 128);//
    tips_ctrl->tips_new_focus = TIPS_YES;
    tips_ctrl->tips_old_focus = TIPS_YES;
}

static __s32 ebook_tips_paint_item(__gui_msg_t *msg)
{
    ebook_tips_ctrl_t *tips_ctrl;
    GUI_RECT gui_rect;
    ebook_uipara_t *ebook_uipara;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    tips_ctrl = (ebook_tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();
    GUI_LyrWinSel(tips_ctrl->tips_para->layer);
    GUI_SetFont(tips_ctrl->tips_para->font);
    GUI_UC_SetEncodeUTF8();

    if (tips_ctrl->tips_new_focus == TIPS_YES)
    {
        GUI_SetColor(tips_ctrl->focus_txt_color);
        gui_rect.x0 = ebook_uipara->tip_yes_bmp_pos.x ;
        gui_rect.y0 = ebook_uipara->tip_yes_bmp_pos.y;
        gui_rect.x1 = gui_rect.x0 + ebook_uipara->tip_yes_bmp_pos.w;
        gui_rect.y1 = gui_rect.y0 + ebook_uipara->tip_yes_bmp_pos.h;
        GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->tip_f_bmp), ebook_uipara->tip_yes_bmp_pos.x, ebook_uipara->tip_yes_bmp_pos.y);
        GUI_DispStringInRect(tips_ctrl->yes, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        GUI_SetColor(tips_ctrl->unfocus_txt_color);
        gui_rect.x0 = ebook_uipara->tip_no_bmp_pos.x ;
        gui_rect.y0 = ebook_uipara->tip_no_bmp_pos.y;
        gui_rect.x1 = gui_rect.x0 + ebook_uipara->tip_no_bmp_pos.w;
        gui_rect.y1 = gui_rect.y0 + ebook_uipara->tip_no_bmp_pos.h;
        GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->tip_uf_bmp), ebook_uipara->tip_no_bmp_pos.x, ebook_uipara->tip_no_bmp_pos.y);
        GUI_DispStringInRect(tips_ctrl->no, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    }
    else
    {
        GUI_SetColor(tips_ctrl->focus_txt_color);
        gui_rect.x0 = ebook_uipara->tip_no_bmp_pos.x ;
        gui_rect.y0 = ebook_uipara->tip_no_bmp_pos.y;
        gui_rect.x1 = gui_rect.x0 + ebook_uipara->tip_no_bmp_pos.w;
        gui_rect.y1 = gui_rect.y0 + ebook_uipara->tip_no_bmp_pos.h;
        GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->tip_f_bmp), ebook_uipara->tip_no_bmp_pos.x, ebook_uipara->tip_no_bmp_pos.y);
        GUI_DispStringInRect(tips_ctrl->no, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        GUI_SetColor(tips_ctrl->unfocus_txt_color);
        gui_rect.x0 = ebook_uipara->tip_yes_bmp_pos.x ;
        gui_rect.y0 = ebook_uipara->tip_yes_bmp_pos.y;
        gui_rect.x1 = gui_rect.x0 + ebook_uipara->tip_yes_bmp_pos.w;
        gui_rect.y1 = gui_rect.y0 + ebook_uipara->tip_yes_bmp_pos.h;
        GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->tip_uf_bmp), ebook_uipara->tip_yes_bmp_pos.x, ebook_uipara->tip_yes_bmp_pos.y);
        GUI_DispStringInRect(tips_ctrl->yes, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    }

    return EPDK_OK;
}

static __s32 ebook_tips_paint_all(__gui_msg_t *msg)
{
    ebook_tips_ctrl_t *tips_ctrl;
    GUI_RECT gui_rect;
    ebook_uipara_t *ebook_uipara;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    tips_ctrl = (ebook_tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();
    GUI_LyrWinSel(tips_ctrl->tips_para->layer);
    GUI_SetFont(tips_ctrl->tips_para->font);
    GUI_UC_SetEncodeUTF8();
    //  GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->tip_bmp), ebook_uipara->tips_bmp_pos.x,ebook_uipara->tips_bmp_pos.y);
    GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->tip_bmp), 0, 0);

    if (tips_ctrl->tips_new_focus == TIPS_YES)
    {
        GUI_SetBkColor(0x00);
        gui_rect.x0 = ebook_uipara->tips_txt_pos.x;
        gui_rect.y0 = ebook_uipara->tips_txt_pos.y;
        gui_rect.x1 = gui_rect.x0 + ebook_uipara->tips_txt_pos.w;
        gui_rect.y1 = gui_rect.y0 + ebook_uipara->tips_txt_pos.h;
        GUI_SetColor(tips_ctrl->focus_txt_color);
        GUI_DispStringInRect(tips_ctrl->tips, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        gui_rect.x0 = ebook_uipara->tip_yes_bmp_pos.x ;
        gui_rect.y0 = ebook_uipara->tip_yes_bmp_pos.y;
        gui_rect.x1 = gui_rect.x0 + ebook_uipara->tip_yes_bmp_pos.w;
        gui_rect.y1 = gui_rect.y0 + ebook_uipara->tip_yes_bmp_pos.h;
        GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->tip_f_bmp), ebook_uipara->tip_yes_bmp_pos.x, ebook_uipara->tip_yes_bmp_pos.y);
        GUI_DispStringInRect(tips_ctrl->yes, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        GUI_SetColor(tips_ctrl->unfocus_txt_color);
        gui_rect.x0 = ebook_uipara->tip_no_bmp_pos.x ;
        gui_rect.y0 = ebook_uipara->tip_no_bmp_pos.y;
        gui_rect.x1 = gui_rect.x0 + ebook_uipara->tip_no_bmp_pos.w;
        gui_rect.y1 = gui_rect.y0 + ebook_uipara->tip_no_bmp_pos.h;
        GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->tip_uf_bmp), ebook_uipara->tip_no_bmp_pos.x, ebook_uipara->tip_no_bmp_pos.y);
        GUI_DispStringInRect(tips_ctrl->no, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    }
    else if (tips_ctrl->tips_new_focus == TIPS_NO)
    {
        GUI_SetColor(tips_ctrl->focus_txt_color);
        GUI_SetBkColor(0x00);
        gui_rect.x0 = ebook_uipara->tips_txt_pos.x;
        gui_rect.y0 = ebook_uipara->tips_txt_pos.y;
        gui_rect.x1 = gui_rect.x0 + ebook_uipara->tips_txt_pos.w;
        gui_rect.y1 = gui_rect.y0 + ebook_uipara->tips_txt_pos.h;
        GUI_SetColor(tips_ctrl->focus_txt_color);
        GUI_DispStringInRect(tips_ctrl->tips, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        gui_rect.x0 = ebook_uipara->tip_no_bmp_pos.x ;
        gui_rect.y0 = ebook_uipara->tip_no_bmp_pos.y;
        gui_rect.x1 = gui_rect.x0 + ebook_uipara->tip_no_bmp_pos.w;
        gui_rect.y1 = gui_rect.y0 + ebook_uipara->tip_no_bmp_pos.h;
        GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->tip_f_bmp), ebook_uipara->tip_no_bmp_pos.x, ebook_uipara->tip_no_bmp_pos.y);
        GUI_DispStringInRect(tips_ctrl->no, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        GUI_SetColor(tips_ctrl->unfocus_txt_color);
        gui_rect.x0 = ebook_uipara->tip_yes_bmp_pos.x ;
        gui_rect.y0 = ebook_uipara->tip_yes_bmp_pos.y;
        gui_rect.x1 = gui_rect.x0 + ebook_uipara->tip_yes_bmp_pos.w;
        gui_rect.y1 = gui_rect.y0 + ebook_uipara->tip_yes_bmp_pos.h;
        GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->tip_uf_bmp), ebook_uipara->tip_yes_bmp_pos.x, ebook_uipara->tip_yes_bmp_pos.y);
        GUI_DispStringInRect(tips_ctrl->yes, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    }

    return EPDK_OK;
}

static __s32  ebook_tips_key_proc(__gui_msg_t *msg)
{
    static __s32 last_key = -1;
    ebook_tips_ctrl_t *tips_ctrl;
    tips_ctrl = (ebook_tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        if (GUI_MSG_KEY_ENTER == last_key)
        {
            if (tips_ctrl->tips_new_focus == TIPS_YES)
            {
                Ebook_cmd2parent(msg->h_deswin, TIPS_YES, 0, 0);
            }
            else
            {
                Ebook_cmd2parent(msg->h_deswin, TIPS_NO, 0, 0);
            }
        }
    }
    else
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_LEFT:      //映射为up
            case GUI_MSG_KEY_LONGLEFT:
            {
                if (tips_ctrl->tips_new_focus == TIPS_NO)
                {
                    tips_ctrl->tips_new_focus = TIPS_YES;
                    ebook_tips_paint_item(msg);
                }
                else if (tips_ctrl->tips_new_focus == TIPS_YES)
                {
                    tips_ctrl->tips_new_focus = TIPS_NO;
                    ebook_tips_paint_item(msg);
                }
            }
            break;

            case GUI_MSG_KEY_RIGHT:     //映射为down
            case GUI_MSG_KEY_LONGRIGHT:
            {
                if (tips_ctrl->tips_new_focus == TIPS_NO)
                {
                    tips_ctrl->tips_new_focus = TIPS_YES;
                    ebook_tips_paint_item(msg);
                }
                else if (tips_ctrl->tips_new_focus == TIPS_YES)
                {
                    tips_ctrl->tips_new_focus = TIPS_NO;
                    ebook_tips_paint_item(msg);
                }
            }
            break;

            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
            {
                if (tips_ctrl->tips_new_focus == TIPS_NO)
                {
                    tips_ctrl->tips_new_focus = TIPS_YES;
                    ebook_tips_paint_item(msg);
                }
                else if (tips_ctrl->tips_new_focus == TIPS_YES)
                {
                    tips_ctrl->tips_new_focus = TIPS_NO;
                    ebook_tips_paint_item(msg);
                }
            }
            break;

            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
            {
                if (tips_ctrl->tips_new_focus == TIPS_NO)
                {
                    tips_ctrl->tips_new_focus = TIPS_YES;
                    ebook_tips_paint_item(msg);
                }
                else if (tips_ctrl->tips_new_focus == TIPS_YES)
                {
                    tips_ctrl->tips_new_focus = TIPS_NO;
                    ebook_tips_paint_item(msg);
                }
            }
            break;

            case GUI_MSG_KEY_ESCAPE:
            case GUI_MSG_KEY_MENU:
                break;

            default:
                break;
        }
    }

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        last_key = -1;
    }
    else
    {
        last_key = msg->dwAddData1;
    }

    return EPDK_OK;
}

/*
    回调
*/
static __s32 _ebook_tips_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            ebook_tips_ctrl_t *tips_ctrl;
            ebook_tip_para_t *tips_para;
            tips_ctrl = (ebook_tips_ctrl_t *)My_Malloc(0, sizeof(ebook_tips_ctrl_t));

            if (!tips_ctrl)
            {
                __err("memory alloc fail........\n");
                return EPDK_FAIL;
            }

            eLIBs_memset(tips_ctrl, 0, sizeof(ebook_tips_ctrl_t));
            tips_para = (ebook_tip_para_t *)GUI_WinGetAttr(msg->h_deswin);
            tips_ctrl->focus_txt_color = APP_COLOR_YELLOW;
            tips_ctrl->unfocus_txt_color = APP_COLOR_WHITE;
            tips_ctrl->tips_para = tips_para;
            ebook_tip_init(tips_ctrl);
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)tips_ctrl);
            GUI_LyrWinCacheOn();
            GUI_LyrWinSetSta(tips_ctrl->tips_para->layer, GUI_LYRWIN_STA_ON);
            GUI_LyrWinSetTop(tips_ctrl->tips_para->layer);
            GUI_LyrWinCacheOff();
        }
        break;

        case GUI_MSG_CLOSE:
        {
            ebook_tips_ctrl_t *tips_ctrl;
            tips_ctrl = (ebook_tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (tips_ctrl->tips_new_focus == TIPS_YES)
            {
                Ebook_cmd2parent(msg->h_deswin, TIPS_YES, 0, 0);
            }
            else
            {
                Ebook_cmd2parent(msg->h_deswin, TIPS_NO, 0, 0);
            }
        }
        break;

        case GUI_MSG_DESTROY:
        {
            ebook_tips_ctrl_t *tips_ctrl;
            tips_ctrl = (ebook_tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!tips_ctrl)
            {
                __err("tips_ctrl is null ......\n");
                return EPDK_FAIL;
            }

            if (tips_ctrl->tip_bmp)
            {
                theme_close(tips_ctrl->tip_bmp);
            }

            if (tips_ctrl->tip_uf_bmp)
            {
                theme_close(tips_ctrl->tip_uf_bmp);
            }

            if (tips_ctrl->tip_f_bmp)
            {
                theme_close(tips_ctrl->tip_f_bmp);
            }

            My_Mfree(0, tips_ctrl->tips_para);
            My_Mfree(0, tips_ctrl);
        }
        break;

        case GUI_MSG_PAINT:
            ebook_tips_paint_all(msg);
            break;

        case GUI_MSG_KEY:
            ebook_tips_key_proc(msg);
            break;

        case GUI_MSG_TOUCH:
            break;

        case GUI_MSG_TIMER:
            break;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

/*
    建立窗口
*/
H_WIN ebook_tips_win_create(H_WIN h_parent, ebook_tip_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    ebook_tip_para_t *tips_para;
    FB fb;
    GUI_LyrWinGetFB(para->layer, &fb);
    tips_para = (ebook_tip_para_t *)My_Malloc(0, sizeof(ebook_tip_para_t));

    if (!tips_para)
    {
        __msg("memory alloc fail.........\n");
        return (H_WIN)EPDK_FAIL;
    }

    tips_para->font = para->font;
    tips_para->layer = para->layer;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =    "ebook_tips_win";
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.hHosting = h_parent;
    framewin_para.id         = EBOOK_TIPS_ID;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_ebook_tips_proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width =  fb.size.width;
    framewin_para.rect.height =  fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr = (void *)tips_para;
    framewin_para.hLayer = para->layer;
    return (GUI_FrmWinCreate(&framewin_para));
}

void  ebook_tips_win_delete(H_WIN tips_win)
{
    GUI_FrmWinDelete(tips_win);
}
