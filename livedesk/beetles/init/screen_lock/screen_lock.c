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
#include "../mod_init_i.h"
#include "scnlock_uipara.h"
#include <kconfig.h>

typedef struct
{
    __screen_lock_para_t        *uipara;
    void                        *lock_bmp;
    void                        *unlock_start_bmp;
    void                        *unlock1_bmp;
    void                        *unlock2_bmp;
    void                        *unlock_end_bmp;
    char                        text[128];
    H_LYR                       layer;
    GUI_FONT                    *SWFFont;
    __u32                       lock_touch_flag;
    __u32                       touch_status;
    __u32                       layer_status;
    __pos_t                     init_pos;
} screen_lock_t;

GUI_FONT    *ScnLockFont;

H_LYR create_screen_lock_layer(screen_lock_t *screen_lock)
{
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt       */
    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        9,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                             /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "tbar layer",
        NULL,
        GUI_LYRWIN_STA_SLEEP,
        GUI_LYRWIN_NORMAL
    };

    fb.size.width           = screen_lock->uipara->screen_lock_layer.fb_w;
    fb.size.height          = screen_lock->uipara->screen_lock_layer.fb_h;
    lstlyr.src_win.x        = screen_lock->uipara->screen_lock_layer.src_x;
    lstlyr.src_win.y        = screen_lock->uipara->screen_lock_layer.src_y;
    lstlyr.src_win.width    = screen_lock->uipara->screen_lock_layer.src_w;
    lstlyr.src_win.height   = screen_lock->uipara->screen_lock_layer.src_h;
    lstlyr.scn_win.x        = screen_lock->uipara->screen_lock_layer.scn_x;
    lstlyr.scn_win.y        = screen_lock->uipara->screen_lock_layer.scn_y;
    lstlyr.scn_win.width    = screen_lock->uipara->screen_lock_layer.scn_w;
    lstlyr.scn_win.height   = screen_lock->uipara->screen_lock_layer.scn_h;
    lstlyr.pipe             = screen_lock->uipara->screen_lock_layer.pipe;
    lstlyr.fb               = &fb;
    lyrcreate_info.lyrpara  = &lstlyr;

    __msg("width=%d,height=%d\n", fb.size.width, fb.size.height);

    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("app bar layer create error !\n");
    }

    return layer;
}

__s32 init_screen_lock_res(screen_lock_t *screen_lock)
{
    HRES                    h_them;
    __screen_lock_para_t    *uipara;

    h_them = OpenRes(THEM_BIN_PATH, 0);

    if (h_them == NULL)
    {
        return -1;
    }

    uipara = screen_lock->uipara;

    screen_lock->lock_bmp = (char *)esMEMS_Balloc(uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);

    if (screen_lock->lock_bmp == NULL)
    {
        return -1;
    }

    eLIBs_memset(screen_lock->lock_bmp, 0, uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);

    GetRes(h_them, STYLEID, uipara->screen_lock_icon, screen_lock->lock_bmp, uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);

    screen_lock->unlock_start_bmp = (char *)esMEMS_Balloc(uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);

    if (screen_lock->unlock_start_bmp == NULL)
    {
        return -1;
    }

    eLIBs_memset(screen_lock->unlock_start_bmp, 0, uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);

    GetRes(h_them, STYLEID, uipara->screen_unlock_start_icon, screen_lock->unlock_start_bmp, uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);

    screen_lock->unlock1_bmp = (char *)esMEMS_Balloc(uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);

    if (screen_lock->unlock1_bmp == NULL)
    {
        return -1;
    }

    eLIBs_memset(screen_lock->unlock1_bmp, 0, uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);

    GetRes(h_them, STYLEID, uipara->screen_unlock1_icon, screen_lock->unlock1_bmp, uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);

    screen_lock->unlock2_bmp = (char *)esMEMS_Balloc(uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);

    if (screen_lock->unlock2_bmp == NULL)
    {
        return -1;
    }

    eLIBs_memset(screen_lock->unlock2_bmp, 0, uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);

    GetRes(h_them, STYLEID, uipara->screen_unlock2_icon, screen_lock->unlock2_bmp, uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);

    screen_lock->unlock_end_bmp = (char *)esMEMS_Balloc(uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);

    if (screen_lock->unlock_end_bmp == NULL)
    {
        return -1;
    }

    eLIBs_memset(screen_lock->unlock2_bmp, 0, uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);
    GetRes(h_them,
           STYLEID,
           uipara->screen_unlock_end_icon,
           screen_lock->unlock_end_bmp,
           uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);
    dsk_langres_get_menu_text(uipara->text, screen_lock->text, 128);
    CloseRes(h_them);
    return 0;
}

__s32   deinit_screen_lock_res(screen_lock_t *screen_lock)
{
    __screen_lock_para_t    *uipara;
    uipara = screen_lock->uipara;

    if (screen_lock->lock_bmp != NULL)
    {
        esMEMS_Bfree(screen_lock->lock_bmp, uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);
        screen_lock->lock_bmp = NULL;
    }

    if (screen_lock->unlock_start_bmp != NULL)
    {
        esMEMS_Bfree(screen_lock->unlock_start_bmp, uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);
        screen_lock->unlock_start_bmp = NULL;
    }

    if (screen_lock->unlock1_bmp != NULL)
    {
        esMEMS_Bfree(screen_lock->unlock1_bmp, uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);
        screen_lock->unlock1_bmp = NULL;
    }

    if (screen_lock->unlock2_bmp != NULL)
    {
        esMEMS_Bfree(screen_lock->unlock2_bmp, uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);
        screen_lock->unlock2_bmp = NULL;
    }

    if (screen_lock->unlock_end_bmp != NULL)
    {
        esMEMS_Bfree(screen_lock->unlock_end_bmp, uipara->lock_icon_rect.width * uipara->lock_icon_rect.height * 4 + 0x36);
        screen_lock->unlock_end_bmp = NULL;
    }

    return 0;
}

__s32 draw_screen_lock(screen_lock_t *screen_lock)
{
    __screen_lock_para_t    *uipara;
    GUI_RECT                    rect;
    uipara = screen_lock->uipara;

    if (GUI_LYRWIN_STA_SUSPEND == GUI_LyrWinGetSta(screen_lock->layer))
    {
        return 0;
    }

    GUI_LyrWinSel(screen_lock->layer);
    GUI_ARGB_Draw(screen_lock->lock_bmp, uipara->frm_rect.x + uipara->lock_icon_rect.x, uipara->frm_rect.y + uipara->lock_icon_rect.y);
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetFont(ScnLockFont);
    GUI_SetColor(GUI_BLACK);
    GUI_CharSetToEncode(dsk_get_langres_charset());
    rect.x0 = uipara->frm_rect.x + uipara->text_rect.x;
    rect.y0 = uipara->frm_rect.y + uipara->text_rect.y;
    rect.x1 = rect.x0 + uipara->text_rect.width;
    rect.y1 = rect.y0 + uipara->text_rect.height;
    GUI_DispStringInRect(screen_lock->text, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    GUI_LyrWinFlushFB(screen_lock->layer);
    GUI_CloseAlphaBlend();
    return 0;
}


static __s32 ScreenLockWinProc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            screen_lock_t *screen_lock = NULL;
            screen_lock = (screen_lock_t *)GUI_WinGetAttr(msg->h_deswin);
            ScnLockFont = GUI_SFT_CreateFont(BEETLES_FONT_SIZE_INT, "E:\\apps\\font16.sft");
            //ScnLockFont = GUI_SFT_CreateFont(BEETLES_FONT_SIZE_INT, "/data/apps/font16.sft");

            if (0 != init_screen_lock_res(screen_lock))
            {
                return -1;
            }

            draw_screen_lock(screen_lock);
            //          layer_init_move(screen_lock);
            GUI_LyrWinSetSta(screen_lock->layer, GUI_LYRWIN_STA_ON);
            GUI_LyrWinSetTop(screen_lock->layer);
            screen_lock->touch_status = 0xffffffff;
            break;
        }

        case GUI_MSG_WIN_WAKEUP:
        {
            screen_lock_t                   *screen_lock = NULL;
            screen_lock = (screen_lock_t *)GUI_WinGetAttr(msg->h_deswin);
            draw_screen_lock(screen_lock);
            break;
        }

        case GUI_MSG_TOUCH:
        {
            screen_lock_t                   *screen_lock = NULL;
            __screen_lock_para_t        *uipara;
            __s32                           pos_x;
            __s32                           pos_y;
            screen_lock = (screen_lock_t *)GUI_WinGetAttr(msg->h_deswin);
            uipara = screen_lock->uipara;
            pos_x = LOSWORD(msg->dwAddData2);
            pos_y = HISWORD(msg->dwAddData2);
            __msg("x=%d,y=%d\n", pos_x, pos_y);

            if (pos_x > uipara->lock_icon_rect.x
                && pos_x < uipara->lock_icon_rect.x + uipara->lock_icon_rect.width
                && pos_y > uipara->lock_icon_rect.y
                && pos_y < uipara->lock_icon_rect.y + uipara->lock_icon_rect.height)
            {
                screen_lock->lock_touch_flag = 0x01;
            }

            switch (msg->dwAddData1)
            {
                case GUI_MSG_TOUCH_DOWN:
                case GUI_MSG_TOUCH_LONGDOWN:
                {
                    __pos_t w_pos;
                    __pos_t s_pos;

                    if (screen_lock->lock_touch_flag != 0x01)
                    {
                        break;
                    }

                    if (GUI_WinGetCaptureWin() != msg->h_deswin)        // 设置capture
                    {
                        GUI_WinSetCaptureWin(msg->h_deswin);
                    }

                    w_pos.x = pos_x;
                    w_pos.y = pos_y;
                    GUI_WindowToScreen(msg->h_deswin, &w_pos, &s_pos);
                    screen_lock->init_pos.x = s_pos.x;
                    screen_lock->init_pos.y = s_pos.y;
                    GUI_LyrWinSel(screen_lock->layer);
                    GUI_ARGB_Draw(screen_lock->unlock_start_bmp, uipara->frm_rect.x + uipara->lock_icon_rect.x, uipara->frm_rect.y + uipara->lock_icon_rect.y);
                    screen_lock->touch_status = GUI_MSG_TOUCH_DOWN;
                    break;
                }

                case GUI_MSG_TOUCH_MOVE:
                {
                    RECT    scn_rect;

                    if (screen_lock->touch_status == GUI_MSG_TOUCH_DOWN)
                    {
                        __pos_t w_pos;
                        __pos_t s_pos;
                        w_pos.x = pos_x;
                        w_pos.y = pos_y;
                        GUI_WindowToScreen(msg->h_deswin, &w_pos, &s_pos);
                        GUI_LyrWinGetScnWindow(screen_lock->layer, &scn_rect);
                        __msg("s_pos.x = %d,screen_lock->init_pos.x=%d\n", s_pos.x, screen_lock->init_pos.x);
                        scn_rect.x = s_pos.x - screen_lock->init_pos.x;

                        if (scn_rect.x < 0)
                        {
                            break;
                        }

                        __msg("scn_rect.x=%d\n", scn_rect.x);

                        if (scn_rect.x > scn_rect.width * 3)
                        {
                            GUI_LyrWinSel(screen_lock->layer);
                            GUI_ARGB_Draw(screen_lock->unlock_end_bmp, uipara->frm_rect.x + uipara->lock_icon_rect.x, uipara->frm_rect.y + uipara->lock_icon_rect.y);
                            screen_lock->layer_status = 0x01;
                        }
                        else if (scn_rect.x > scn_rect.width * 2)
                        {
                            GUI_LyrWinSel(screen_lock->layer);
                            GUI_ARGB_Draw(screen_lock->unlock_start_bmp, uipara->frm_rect.x + uipara->lock_icon_rect.x, uipara->frm_rect.y + uipara->lock_icon_rect.y);
                            screen_lock->layer_status = 0x00;
                        }
                        else if (scn_rect.x > scn_rect.width)
                        {
                            GUI_LyrWinSel(screen_lock->layer);
                            GUI_ARGB_Draw(screen_lock->unlock1_bmp, uipara->frm_rect.x + uipara->lock_icon_rect.x, uipara->frm_rect.y + uipara->lock_icon_rect.y);
                            screen_lock->layer_status = 0x00;
                        }
                        else
                        {
                            GUI_LyrWinSel(screen_lock->layer);
                            GUI_ARGB_Draw(screen_lock->unlock_start_bmp, uipara->frm_rect.x + uipara->lock_icon_rect.x, uipara->frm_rect.y + uipara->lock_icon_rect.y);
                            screen_lock->layer_status = 0x00;
                        }

                        GUI_LyrWinSetScnWindow(screen_lock->layer, &scn_rect);
                    }

                    break;
                }

                case GUI_MSG_TOUCH_UP:
                {
                    RECT    scn_rect;
                    __u32   base;
                    __s32   temp;
                    GUI_LyrWinGetScnWindow(screen_lock->layer, &scn_rect);
                    base = scn_rect.width / 2;

                    if (screen_lock->touch_status != GUI_MSG_TOUCH_DOWN)
                    {
                        break;
                    }

                    if (GUI_WinGetCaptureWin() == msg->h_deswin)                // 设置capture
                    {
                        GUI_WinReleaseCapture();
                    }

                    if (screen_lock->layer_status == 0x01)
                    {
                        //                      while(1)
                        //                      {
                        //                          GUI_LyrWinGetScnWindow(screen_lock->layer,&scn_rect);
                        //                          __msg("base = %d,scn_rect.width=%d\n",base,scn_rect.width);
                        //                          __msg("scn_rect.x=%d add=%d\n",scn_rect.x,(scn_rect.x-base+scn_rect.width));
                        //                          temp = scn_rect.x-base+scn_rect.width;
                        //                          if(temp<0)
                        //                          {
                        //                              scn_rect.x = 0-scn_rect.width;
                        //                              GUI_LyrWinSetScnWindow(screen_lock->layer,&scn_rect);
                        {
                            /*通知上层删除此窗口*/
                            __gui_notify_msg_t  notify_msg;                                 // 向上发通知码
                            notify_msg.hWnd     = msg->h_deswin;
                            notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                            notify_msg.msgcode  = SCREEN_LOCK_EXIT;
                            GUI_NotifyParent(&notify_msg);
                        }
                        //                              break;
                        //                          }
                        //                          else
                        //                          {
                        //                              //__here__
                        //                              scn_rect.x -=base;
                        //                              GUI_LyrWinSetScnWindow(screen_lock->layer,&scn_rect);
                        //                          }
                        //
                        //                      }
                    }
                    else
                    {
                        draw_screen_lock(screen_lock);
                        GUI_LyrWinGetScnWindow(screen_lock->layer, &scn_rect);

                        if (scn_rect.x < 0)
                        {
                            scn_rect.x = 0;
                            GUI_LyrWinSetScnWindow(screen_lock->layer, &scn_rect);
                        }
                        else
                        {
                            while (1)
                            {
                                GUI_LyrWinGetScnWindow(screen_lock->layer, &scn_rect);
                                temp = scn_rect.x - base;

                                if (temp < 0)
                                {
                                    scn_rect.x = 0;
                                    GUI_LyrWinSetScnWindow(screen_lock->layer, &scn_rect);
                                    break;
                                }
                                else
                                {
                                    scn_rect.x -= base;
                                    GUI_LyrWinSetScnWindow(screen_lock->layer, &scn_rect);
                                }
                            }
                        }
                    }

                    screen_lock->lock_touch_flag = 0x00;
                    screen_lock->touch_status = 0xffffffff;
                    //                  GUI_FrmWinDelete(msg->h_deswin);
                    break;
                }

                default:
                    break;
            }

            break;
        }

        case GUI_MSG_DESTROY:
        {
            screen_lock_t                   *screen_lock = NULL;
            screen_lock = (screen_lock_t *)GUI_WinGetAttr(msg->h_deswin);
            //__here__
            deinit_screen_lock_res(screen_lock);
            //__here__
            //          GUI_LyrWinDelete(screen_lock->layer);
            //__here__
            esMEMS_Bfree(screen_lock, sizeof(screen_lock_t));
            GUI_SetFont(GUI_GetDefaultFont());
            GUI_SFT_ReleaseFont(ScnLockFont);
            //__here__
            break;
        }

        case GUI_MSG_CLOSE:
        {
            GUI_FrmWinDelete(msg->h_deswin);
            break;
        }

        default:
        {
            GUI_FrmWinDefaultProc(msg);
            break;
        }
    }

    return 0;
}
H_WIN create_screen_lock(H_WIN parent_win)
{
    screen_lock_t                   *screen_lock = NULL;
    __gui_framewincreate_para_t     frmcreate_info;
    H_WIN                           frame_win = 0;
    screen_lock = (screen_lock_t *)esMEMS_Balloc(sizeof(screen_lock_t));

    if (screen_lock == NULL)
    {
        return 0;
    }

    eLIBs_memset(screen_lock, 0, sizeof(screen_lock_t));
    screen_lock->uipara = &scnlock_para;
    screen_lock->layer = create_screen_lock_layer(screen_lock);
    //screen_lock->SWFFont = Font;
    eLIBs_memset(&frmcreate_info, 0, sizeof(__gui_framewincreate_para_t));
    frmcreate_info.name             = NULL;
    frmcreate_info.attr             = screen_lock;
    frmcreate_info.BkColor.alpha    = 0;
    frmcreate_info.BkColor.red      = 0;
    frmcreate_info.BkColor.green    = 0;
    frmcreate_info.BkColor.blue     = 0;
    frmcreate_info.dwStyle          = WS_VISIBLE;
    frmcreate_info.dwExStyle        = 0;
    frmcreate_info.hOwner           = NULL;
    frmcreate_info.hHosting         = parent_win;
    frmcreate_info.id               = ID_SCREEN_LOCK;
    frmcreate_info.FrameWinProc     = ScreenLockWinProc;
    frmcreate_info.hLayer           = screen_lock->layer;
    frmcreate_info.rect.x           = scnlock_para.frm_rect.x;
    frmcreate_info.rect.y           = scnlock_para.frm_rect.y;
    frmcreate_info.rect.width       = scnlock_para.frm_rect.width;
    frmcreate_info.rect.height      = scnlock_para.frm_rect.height;
    __msg("frmcreate_info.rect.x=%d,frmcreate_info.rect.y=%d\n", frmcreate_info.rect.x, frmcreate_info.rect.y);
    __msg("frmcreate_info.rect.w=%d,frmcreate_info.rect.h=%d\n", frmcreate_info.rect.width, frmcreate_info.rect.height);
    frame_win = GUI_FrmWinCreate(&frmcreate_info);

    if (!frame_win)
    {
        __inf("\n%s,%d create screen_lock frame win failed!", __FILE__, __LINE__);
        return frame_win;
    }

    return frame_win;
}

__s32   delete_screen_lock(H_WIN win)
{
    screen_lock_t                   *screen_lock = NULL;
    screen_lock = (screen_lock_t *)GUI_WinGetAttr(win);
    GUI_LyrWinDelete(screen_lock->layer);
    return 0;
}
