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
#include "mod_init_i.h"
#include "assistant_scene.h"

#define TIME_REFRESH_TIMER      1234
#define TIME_REFRESH_TIMEOUT    100
#define PI                      3.1416

typedef struct __scene_para
{
    H_LYR lyr;
    H_WIN frmwin;
} __scene_para_t;

typedef struct __frmwin_para
{
    HTHEME          backgrd_theme;
    void            *backgrd_bmp;
} __frmwin_para_t;


typedef struct __ui_para
{
    __u32   x;
    __u32   y;
    __u32   width;
    __u32   height;
    __u32   reserved[32];
} __ui_para_t;


GUI_FONT    *TTFFONT;


/*static __s32 __cmd_handle(__gui_msg_t* msg)
{
    switch (LOSWORD(msg->dwAddData1))
    {
        case BTN_ID_ESCAPE:
        {
            if (HISWORD(msg->dwAddData1) == BN_CLICKED)
            {

            }
            break;
        }

        default:
            break;
    }

    return 0;
}*/

H_LYR __lyr_create(void)
{
    __layerwincreate_para_t lyrcreate_info;
    FB pBarFB =
    {
        {300, 390},
        {0, 0, 0},
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},
    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,         //mode
        0,                                   //ck_mode
        0,                                   //alpha_en
        190,                                //alpha_val
        1,                                   //pipe
        10,                               //prio
        {0, 0, 300, 390},                    //scr_win
        {0, 0, 300, 390},                   //src_win
        DISP_LAYER_OUTPUT_CHN_DE_CH1,        //channel
        NULL                                 //fb
    };
    eLIBs_memset(&lyrcreate_info, 0x0, sizeof(__layerwincreate_para_t));
    pBarFB.size.width =     300;
    pBarFB.size.height =    390;//
    lstlyr.scn_win.x =      250;//
    lstlyr.scn_win.y =      90;//
    lstlyr.scn_win.width =  300;
    lstlyr.scn_win.height = 390;//
    lstlyr.src_win.x =      0;//
    lstlyr.src_win.y =      0;//
    lstlyr.src_win.width =  300;//
    lstlyr.src_win.height = 390;//
    lstlyr.fb = &pBarFB;
    lyrcreate_info.name = "layer";
    lyrcreate_info.lyrpara = &lstlyr;
    lyrcreate_info.initsta = GUI_LYRWIN_STA_SLEEP;
    return (GUI_LyrWinCreate(&lyrcreate_info));
}

void __time_update(__gui_msg_t  *msg)
{
    GUI_MEMDEV_Handle   hmem;
    __awos_time_t           cur_time;
    char            str[16];
    GUI_RECT        text_rect;
    GUI_POINT       curhourpoly[4];
    GUI_POINT       curminutepoly[4];
    GUI_POINT       cursecondpoly[4];
    GUI_POINT   hourpoly[4] =
    {
        {0, 0},  { -4, -6},    {0, -47},    {4, -6},
    };
    GUI_POINT   minutepoly[4] =
    {
        {0, 0},  { -4, -9},    {0, -65},    {4, -9},
    };
    GUI_POINT   secondpoly[4] =
    {
        {0, 0},  { -2, -11},   {0, -77},    {2, -11},
    };
    __frmwin_para_t     *frmwin_para;
    frmwin_para = (__frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);
    GUI_LyrWinSel((H_LYR)GUI_WinGetLyrWin(msg->h_deswin));
    hmem = GUI_MEMDEV_Create(0, 0, 300, 390);

    if (hmem)
    {
        GUI_MEMDEV_Select(hmem);
    }

    GUI_OpenAlphaBlend();

    if (frmwin_para->backgrd_bmp)
    {
        GUI_ARGB_Draw(frmwin_para->backgrd_bmp, 0, 0);
    }

    esTIME_GetTime(&cur_time);
    //__msg("%d:%d:%d\n", cur_time.hour, cur_time.minute, cur_time.second);
    GUI_SetColor(GUI_BLACK);
    //GUI_RotatePolygon(curhourpoly, hourpoly, 4, -((__fp32)(cur_time.hour%12)*PI/6 + (__fp32)cur_time.minute*PI/360));
    GUI_FillPolygon(curhourpoly, 4, 149, 183);
    //GUI_RotatePolygon(curminutepoly, minutepoly, 4, -(__fp32)cur_time.minute*PI/30);
    GUI_FillPolygon(curminutepoly, 4, 149, 183);
    GUI_SetColor(GUI_RED);
    //GUI_RotatePolygon(cursecondpoly, secondpoly, 4, -(__fp32)cur_time.second*PI/30);
    GUI_FillPolygon(cursecondpoly, 4, 149, 183);
    GUI_CloseAlphaBlend();
    GUI_SetBkColor(0x00000000);
    GUI_ClearRect(0, 300, 550, 390);

    if (cur_time.hour < 10)
    {
        str[0] = '0';
        eLIBs_uint2str_dec(cur_time.hour, str + 1);
    }
    else
    {
        eLIBs_uint2str_dec(cur_time.hour, str);
    }

    str[2] = ':';

    if (cur_time.minute < 10)
    {
        str[3] = '0';
        eLIBs_uint2str_dec(cur_time.minute, str + 4);
    }
    else
    {
        eLIBs_uint2str_dec(cur_time.minute, str + 3);
    }

    str[5] = ':';

    if (cur_time.second < 10)
    {
        str[6] = '0';
        eLIBs_uint2str_dec(cur_time.second, str + 7);
    }
    else
    {
        eLIBs_uint2str_dec(cur_time.second, str + 6);
    }

    text_rect.x0 = 45;
    text_rect.y0 = 310;
    text_rect.x1 = text_rect.x0 + 250;
    text_rect.y1 = text_rect.y0 + 50;
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(TTFFONT);
    GUI_DispStringInRect(str, (GUI_RECT *)&text_rect, GUI_TA_LEFT | GUI_TA_VCENTER);

    if (hmem)
    {
        GUI_MEMDEV_CopyToLCD(hmem);
        GUI_MEMDEV_Select(0);
        GUI_MEMDEV_Delete(hmem);
        hmem = 0;
    }

    return;
}

static __s32 __FrmwinProc(__gui_msg_t  *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            __frmwin_para_t     *frmwin_para;
            GUI_TTF_ATTR    attr;
            frmwin_para = (__frmwin_para_t *)esMEMS_Malloc(0, sizeof(__frmwin_para_t));

            if (!frmwin_para)
            {
                __msg("frmwin_para malloc fail!\n");
                return EPDK_FAIL;
            }

            eLIBs_memset(frmwin_para, 0, sizeof(__frmwin_para_t));
            GUI_WinSetAttr(msg->h_deswin, (void *)frmwin_para);
            frmwin_para->backgrd_theme  = dsk_theme_open(0);
            frmwin_para->backgrd_bmp    = dsk_theme_hdl2buf(frmwin_para->backgrd_theme);
            /* create ttf font  */
            eLIBs_strcpy(attr.filepath, BEETLES_APP_ROOT"res\\fonts\\font.ttf");
            attr.height = 50;
            TTFFONT = GUI_TTF_CreateFont(&attr);
            GUI_SetTimer(msg->h_deswin, TIME_REFRESH_TIMER, TIME_REFRESH_TIMEOUT, NULL);
            return EPDK_OK;
        }

        case GUI_MSG_DESTROY:
        {
            __frmwin_para_t     *frmwin_para;
            frmwin_para = (__frmwin_para_t *)GUI_WinGetAttr(msg->h_deswin);
            /* 释放字体 */
            GUI_SetFont(GUI_GetDefaultFont());
            GUI_TTF_Done(TTFFONT);
            GUI_KillTimer(msg->h_deswin, TIME_REFRESH_TIMER);
            dsk_theme_close(frmwin_para->backgrd_theme);
            esMEMS_Mfree(0, frmwin_para);
            return EPDK_OK;
        }

        case GUI_MSG_TIMER:
        {
            __time_update(msg);
            break;
        }

        case GUI_MSG_COMMAND:
        {
            //消息发往管理窗口处理
            __gui_msg_t      msgex;
            msgex.id        = GUI_MSG_COMMAND;
            msgex.h_srcwin  = msg->h_deswin;
            msgex.h_deswin  = GUI_WinGetParent(msg->h_deswin);
            msgex.dwAddData1 = msg->dwAddData1;
            msgex.dwAddData2 = msg->dwAddData2;
            GUI_SendNotifyMessage(&msgex);
            return EPDK_OK;
        }

        default:
        {
            break;
        }
    }

    return GUI_ManWinDefaultProc(msg);
}

H_WIN __frmwin_create(H_WIN parent, H_LYR layer)
{
    __gui_framewincreate_para_t framewin_para;
    FB                          fb;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    GUI_LyrWinGetFB(layer, &fb);
    framewin_para.name =    "toolbar frmwin",
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.hHosting = parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__FrmwinProc);
    framewin_para.rect.x = 0;// 0;
    framewin_para.rect.y = 0;//0;
    framewin_para.rect.width = fb.size.width; //
    framewin_para.rect.height = fb.size.height;//
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr =  0;
    framewin_para.hLayer = layer;
    return (GUI_FrmWinCreate(&framewin_para));
}

static __s32 __MainProc(__gui_msg_t  *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            __scene_para_t *scene_para;
            scene_para = (__scene_para_t *)esMEMS_Malloc(0, sizeof(__scene_para_t));
            eLIBs_memset(scene_para, 0, sizeof(__scene_para_t));
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)scene_para);
            scene_para->lyr     = __lyr_create();
            scene_para->frmwin  = __frmwin_create(msg->h_deswin, scene_para->lyr);
            GUI_LyrWinSetSta(scene_para->lyr, GUI_LYRWIN_STA_ON);
            GUI_LyrWinSetTop(scene_para->lyr);
            return EPDK_OK;
        }

        case GUI_MSG_DESTROY:
        {
            __scene_para_t *scene_para = (__scene_para_t *)GUI_WinGetAddData(msg->h_deswin);
            GUI_LyrWinDelete(scene_para->lyr);
            esMEMS_Mfree(0, scene_para);
            return EPDK_OK;
        }

        case GUI_MSG_COMMAND:
        {
            //__cmd_handle(msg);
            return EPDK_OK;
        }

        default:
        {
            break;
        }
    }

    return GUI_ManWinDefaultProc(msg);
}

H_WIN assistant_scene_create(H_WIN parent)
{
    H_WIN                       hManWin;
    __gui_manwincreate_para_t   create_info;
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    create_info.attr            = 0;
    create_info.hParent         = parent;
    create_info.hHosting        = NULL;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__MainProc);
    create_info.name            = "assistant";
    hManWin                     = GUI_ManWinCreate(&create_info);

    if (hManWin == NULL)
    {
        __wrn("assistant: create man win failed!\n");
        return NULL;
    }

    __msg("assistant ManWin = %x\n", hManWin);
    return hManWin;
}

__s32 assistant_scene_delete(H_WIN hwnd)
{
    return GUI_ManWinDelete(hwnd);
}
