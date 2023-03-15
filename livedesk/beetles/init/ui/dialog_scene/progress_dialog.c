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
#include "progress_dialog.h"
#include <kconfig.h>

#define PROGRESS_DIALOG_PIC_ID  2000
#define PROG_TIMER_ID           1

#define POINT_BMP_WIDTH         24
#define POINT_BMP_HEIGHT        24

extern GUI_FONT    *SWFFont;

typedef struct tag_pdialog_para
{
    int   text_id;
    HTHEME prog_bmp_hdl;
    char text[256];
    int  timer_count;
} pdialog_para_t;

static __s32 init_prog_dialog_res(pdialog_para_t *com)
{
    com->prog_bmp_hdl = dsk_theme_open(ID_INIT_POINT_BMP);

    if (!com->prog_bmp_hdl)
    {
        __err(" dsk_theme_open fail ");
        return EPDK_FAIL;
    }

    dsk_langres_get_menu_text(com->text_id, com->text, 256);
    return EPDK_OK;
}

static __s32 deinit_prog_dialog_res(pdialog_para_t *com)
{
    if (com && com->prog_bmp_hdl)
    {
        dsk_theme_close(com->prog_bmp_hdl);
        com->prog_bmp_hdl = (unsigned long)NULL;
    }

    return EPDK_OK;
}

static __s32 cb_progress_dialog(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_INITDIALOG:
        {
            GUI_SetTimer(msg->h_deswin, PROG_TIMER_ID, 100, NULL);
            return 0;
        }

        case GUI_MSG_PAINT:
        {
            GUI_RECT        rcrect;
            RECT            fbrect;
            pdialog_para_t  *com;
            H_LYR           layer;
            /* create font  */
            //SWFFont = GUI_SFT_CreateFont(16, BEETLES_APP_ROOT"res\\fonts\\font16.sft" );
            com   = (pdialog_para_t *)GUI_WinGetAttr(msg->h_deswin);
            layer = GUI_WinGetLyrWin(msg->h_deswin);

            if (GUI_LYRWIN_STA_SUSPEND != GUI_LyrWinGetSta(layer))
            {
                GUI_LyrWinSel(layer);
                //GUI_WinGetClientFBRect(msg->h_deswin,&fbrect);
                fbrect.x        = 100; //136;   //12;
                fbrect.y        = 68;  //78;    //14;
                fbrect.width    = 300;   //200+40;
                fbrect.height   = 100;
                rcrect.x0 = fbrect.x;
                rcrect.y0 = fbrect.y ;
                rcrect.x1 = fbrect.x + fbrect.width;
                rcrect.y1 = fbrect.y + fbrect.height - 25;
                GUI_SetBkColor(0);
                GUI_ClearRect(rcrect.x0, rcrect.y0, rcrect.x1, rcrect.y1);
                /* draw dailog content */
                GUI_SetFont(SWFFont);
                GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
                GUI_SetColor(GUI_WHITE);
                GUI_DrawLine(fbrect.x, fbrect.y + 5, fbrect.x + fbrect.width, fbrect.y + 5);
                GUI_DrawLine(fbrect.x, fbrect.y + fbrect.height - 5, fbrect.x + fbrect.width, fbrect.y + fbrect.height - 5);
                GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
                GUI_DispStringInRect(com->text, &rcrect, GUI_TA_HCENTER | GUI_TA_VCENTER);
                GUI_LyrWinFlushFB(layer);
            }
        }

        return 0;

        case GUI_MSG_TIMER:
        {
            pdialog_para_t  *com;
            GUI_RECT        rcrect;
            RECT            fbrect;
            H_LYR           layer;
            com   = (pdialog_para_t *)GUI_WinGetAttr(msg->h_deswin);
            layer = GUI_WinGetLyrWin(msg->h_deswin);

            if (GUI_LYRWIN_STA_SUSPEND != GUI_LyrWinGetSta(layer))
            {
                GUI_LyrWinSel(layer);
                //GUI_WinGetClientFBRect(msg->h_deswin,&fbrect);
                fbrect.x        = 100; //136; //12;
                fbrect.y        = 68;  //78;  //14;
                fbrect.width    = 300; //200+40;
                fbrect.height   = 100;
                rcrect.x0 = fbrect.x + fbrect.width / 2 - 35;
                rcrect.y0 = fbrect.y + fbrect.height / 2 + 10;
                rcrect.x1 = fbrect.x + fbrect.width;
                rcrect.y1 = rcrect.y0 + 25;

                if (com->timer_count == 5)
                {
                    GUI_SetBkColor(0);
                    GUI_ClearRect(rcrect.x0, rcrect.y0, rcrect.x1, rcrect.y1);
                    com->timer_count = 0;
                }

                GUI_BMP_Draw(dsk_theme_hdl2buf(com->prog_bmp_hdl), rcrect.x0 + 15 * com->timer_count, rcrect.y0);
                com->timer_count++;
            }
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            pdialog_para_t  *com;
            GUI_RECT        rcrect;
            RECT            fbrect;
            H_LYR           layer;
            GUI_KillTimer(msg->h_deswin, PROG_TIMER_ID);
            com   = (pdialog_para_t *)GUI_WinGetAttr(msg->h_deswin);
            layer = GUI_WinGetLyrWin(msg->h_deswin);

            if (GUI_LYRWIN_STA_SUSPEND != GUI_LyrWinGetSta(layer))
            {
                GUI_LyrWinSel(layer);
                GUI_WinGetClientFBRect(msg->h_deswin, &fbrect);
                rcrect.x0 = fbrect.x;
                rcrect.y0 = fbrect.y ;
                rcrect.x1 = fbrect.x + fbrect.width;
                rcrect.y1 = fbrect.y + fbrect.height;
                GUI_SetBkColor(0);
                GUI_ClearRect(rcrect.x0, rcrect.y0, rcrect.x1, rcrect.y1);
            }

            deinit_prog_dialog_res(com);
            esMEMS_Mfree(0, com);
            /* 释放字体 */
            //GUI_SetFont(&GUI_Font6x8);
            //GUI_SFT_ReleaseFont(SWFFont);
        }

        return EPDK_OK;

        case GUI_MSG_KEY:
        {
            __gui_msg_t notify_msg;
            notify_msg.id         = GUI_MSG_COMMAND;
            notify_msg.h_srcwin   = msg->h_deswin;
            notify_msg.h_deswin   = GUI_WinGetOwnerWin(msg->h_deswin);
            notify_msg.dwAddData1 = (msg->dwAddData1 << 16) + GUI_WinGetItemId(msg->h_deswin);
            notify_msg.dwAddData2 = 0 ;
            notify_msg.dwReserved = 0;
            notify_msg.p_arg      = NULL;
            GUI_SendMessage(&notify_msg);
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
            GUI_EndDialog(msg->h_deswin, 0);
            return EPDK_OK;

        default:
            break;
    }

    return GUI_DialogDefaultProc(msg);
}

H_WIN progress_dialog_create(H_WIN owner, H_WIN layer, pdialog_create_t *para)
{
    __gui_dialogcreate_para_t   dlg_create;
    __gui_dlgtemplate_t         pd_template;
    pdialog_para_t              *com;
    com = (pdialog_para_t *)esMEMS_Malloc(0, sizeof(pdialog_para_t));

    if (!com)
    {
        __err(" progress dialog malloc error ");
        return NULL;
    }

    eLIBs_memset(com, 0, sizeof(pdialog_para_t));
    eLIBs_memset(&dlg_create, 0, sizeof(__gui_dialogcreate_para_t));
    eLIBs_memset(&pd_template, 0, sizeof(__gui_dlgtemplate_t));
    com->text_id = para->text_id;
    com->timer_count = 0;
    init_prog_dialog_res(com);
    pd_template.dwStyle     = WS_VISIBLE;
    pd_template.dwExStyle   = 0;
    GUI_LyrWinGetSrcWindow(layer, &(pd_template.dlg_rect));
    pd_template.caption     = NULL;
    pd_template.controlnr   = 0;
    pd_template.controls    = NULL;
    pd_template.dwAddData   = 0;
    dlg_create.name         = NULL;
    dlg_create.pDlgTemplate = &pd_template;
    dlg_create.hOwner       = owner;
    dlg_create.hLayer       = layer;
    dlg_create.id           = para->dialog_id;
    dlg_create.DialogProc   = cb_progress_dialog;
    dlg_create.dwAddData    = (unsigned long)com;
    __msg("before GUI_ModuleDialogCreate");
    GUI_ModuleDialogCreate(&dlg_create);
    __msg("after GUI_ModuleDialogCreate");
    return NULL;
}
