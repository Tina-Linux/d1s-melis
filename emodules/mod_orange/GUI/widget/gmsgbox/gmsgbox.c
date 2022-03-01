/*
*******************************************************************************************************************
*                                                           Module Orange
*                                                   the embedded Graphical module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º    gmsgbox.c
*
*Author£º       William Wang
*
*Version :      1.1.0
*
*Date:          2009-10-19
*
*Description :  global msgbox implement
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>          <version>     <description>
*
* William Wang     2009-10-19         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __GMSGBOX_C__
#define __GMSGBOX_C__

#include "gmsgbox_i.h"

/*
**********************************************************************************************************************
*                                               MsgBoxProc
*
* author:
*
* date:             2009-10-19
*
* Description:      msgboxproc
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32 MsgBoxProc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_INITDIALOG:
        {
            msgbox_para_t  *user_data;
            H_WIN   hFocus = GUI_DlgGetFirstButton(msg->h_deswin);

            /*ÉèÖÃ½¹µãbutton*/
            if (hFocus)
            {
                GUI_WinSetActiveChild(hFocus);
            }

            GUI_WinSetAddData(msg->h_deswin, (__u32)msg->dwAddData2);
            user_data   = (msgbox_para_t *)GUI_WinGetAttr(msg->h_deswin);

            if (user_data->timeout)
            {
                GUI_SetTimer(msg->h_deswin, 0, user_data->timeout * 100, NULL);
            }

            return 0;
        }

        case GUI_MSG_DESTROY:
        {
            msgbox_para_t  *user_data;
            user_data   = (msgbox_para_t *)GUI_WinGetAttr(msg->h_deswin);

            if (user_data->timeout)
            {
                GUI_KillTimer(msg->h_deswin, 0);
            }

            return 0;
        }

        case GUI_MSG_TIMER:
        {
            if (msg->dwAddData1 == 0)
            {
                GUI_EndDialog(msg->h_deswin, IDM_TIMEROUT);
                return  0;
            }
        }
        break;

        case GUI_MSG_COMMAND:
        {
            switch (LOWORD(msg->dwAddData1))
            {
                case IDOK:
                case IDCANCEL:
                case IDABORT:
                case IDRETRY:
                case IDIGNORE:
                case IDYES:
                case IDNO:
                {
                    if (HIWORD(msg->dwAddData1) == BN_CLICKED)
                    {
                        if (msg->dwAddData2 == GUI_MSG_KEY_ENTER
                            || msg->dwAddData2 == GUI_MSG_KEY_LONGENTER
                            || msg->dwAddData2 == GUI_MSG_TOUCH_DOWN
                            || msg->dwAddData2 == GUI_MSG_TOUCH_LONGDOWN)
                        {
                            if (GUI_GetDlgItem(msg->h_deswin, LOWORD(msg->dwAddData1)))
                            {
                                GUI_EndDialog(msg->h_deswin, LOWORD(msg->dwAddData1));
                            }

                            return 0;
                        }
                    }

                    break;
                }

                default:
                    break;
            }

            break;
        }

        case GUI_MSG_PAINT:
        {
            __gui_rect_t    rcrect;
            RECT            fbrect;
            msgbox_para_t  *user_data;
            GUI_MEMDEV_Handle   hmem = 0;
            user_data   = (msgbox_para_t *)GUI_WinGetAttr(msg->h_deswin);
            GUI_LyrWinSel(GUI_LyrP2H(GUI_WinGetLyrWin(msg->h_deswin)));
            GUI_LyrWinSetSta(GUI_LyrP2H(GUI_WinGetLyrWin(msg->h_deswin)), GUI_LYRWIN_STA_OFF);
            GUI_WinGetClientFBRect(msg->h_deswin, &fbrect);
            //hmem = GUI_MEMDEV_Create(fbrect.x,fbrect.y,fbrect.width,fbrect.height);
            SysRectToGuiRect(&fbrect, &rcrect);
            /*if(hmem)
            {
                GUI_MEMDEV_Select(hmem);
            }// start memory device*/
            GUI_SetBkColor(0);
            GUI_ClearRect(rcrect.left, rcrect.top,
                          rcrect.right, rcrect.bottom);

            if (user_data->bk_bmp)
            {
                GUI_ARGB_Draw(user_data->bk_bmp, rcrect.left, rcrect.top);
            }

            if (user_data->alpha_en)
            {
                GUI_OpenAlphaBlend();
            }

            GUI_CharSetToEncode(user_data->draw_code);
            GUI_SetFont(user_data->draw_font);
            GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
            GUI_SetColor(user_data->txt_color);
            GUI_DispStringAt(user_data->text,
                             rcrect.left + user_data->text_pos.x,
                             rcrect.top + user_data->text_pos.y);

            /* if(hmem)
             {
                 GUI_MEMDEV_CopyToLCD( hmem );          // copy from memory device to fb
                 GUI_MEMDEV_Select( NULL );                    // stop memory device
                 GUI_MEMDEV_Delete(hmem);
                 hmem = NULL;
             }*/

            if (user_data->alpha_en)
            {
                GUI_CloseAlphaBlend();
            }

            GUI_LyrWinFlushFB(GUI_WinGetLyrWin(msg->h_deswin));
        }

        return 0;

        case GUI_MSG_CLOSE:
            if (GUI_GetDlgItem(msg->h_deswin, IDCANCEL))
            {
                GUI_EndDialog(msg->h_deswin, IDCANCEL);
            }
            else if (GUI_GetDlgItem(msg->h_deswin, IDIGNORE))
            {
                GUI_EndDialog(msg->h_deswin, IDIGNORE);
            }
            else if (GUI_GetDlgItem(msg->h_deswin, IDNO))
            {
                GUI_EndDialog(msg->h_deswin, IDNO);
            }
            else if (GUI_GetDlgItem(msg->h_deswin, IDOK))
            {
                GUI_EndDialog(msg->h_deswin, IDOK);
            }

            return 0;

        default:
            break;
    }

    return GUI_DialogDefaultProc(msg);
}

/*
**********************************************************************************************************************
*                                               GUI_GMsgBox
*
* author:
*
* date:             2009-10-19
*
* Description:      gui gmsgbox
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 GUI_GMsgBox(__gui_gmsgboxcreate_para_t *create_info)
{
    __gui_dlgtemplate_t         MsgBoxData;
    __gui_ctrldata_t            CtrlData[5];
    __gui_dialogcreate_para_t   dlg_create;
    MsgBoxData.caption     = NULL;
    MsgBoxData.controlnr   = create_info->pDlgTemplate->controlnr;
    MsgBoxData.controls    = create_info->pDlgTemplate->controls;
    MsgBoxData.dlg_rect    = create_info->pDlgTemplate->dlg_rect;
    MsgBoxData.dwAddData   = (unsigned long)create_info->skin;
    MsgBoxData.dwExStyle   = create_info->pDlgTemplate->dwExStyle;
    MsgBoxData.dwStyle     = create_info->pDlgTemplate->dwStyle;

    switch (create_info->type)
    {
        case MB_OK:
            MsgBoxData.controlnr    = 1;
            CtrlData[0].class_name  = CTRL_BUTTON;
            CtrlData[0].ctl_rect    = create_info->pDlgTemplate->controls[0].ctl_rect;
            CtrlData[0].dwAddData   = create_info->pDlgTemplate->controls[0].dwAddData;
            CtrlData[0].dwExStyle   = create_info->pDlgTemplate->controls[0].dwExStyle;
            CtrlData[0].id          = IDOK;
            CtrlData[0].name        = create_info->pDlgTemplate->controls[0].name;
            CtrlData[0].dwStyle     = create_info->pDlgTemplate->controls[0].dwStyle;
            break;

        case MB_OKCANCEL:
            MsgBoxData.controlnr    = 2;
            CtrlData[0].class_name  = CTRL_BUTTON;
            CtrlData[0].ctl_rect    = create_info->pDlgTemplate->controls[0].ctl_rect;
            CtrlData[0].dwAddData   = create_info->pDlgTemplate->controls[0].dwAddData;
            CtrlData[0].dwExStyle   = create_info->pDlgTemplate->controls[0].dwExStyle;
            CtrlData[0].id          = IDOK;
            CtrlData[0].name        = create_info->pDlgTemplate->controls[0].name;
            CtrlData[0].dwStyle     = create_info->pDlgTemplate->controls[0].dwStyle;
            CtrlData[1].class_name  = CTRL_BUTTON;
            CtrlData[1].ctl_rect    = create_info->pDlgTemplate->controls[1].ctl_rect;
            CtrlData[1].dwAddData   = create_info->pDlgTemplate->controls[1].dwAddData;
            CtrlData[1].dwExStyle   = create_info->pDlgTemplate->controls[1].dwExStyle;
            CtrlData[1].id          = IDCANCEL;
            CtrlData[1].name        = create_info->pDlgTemplate->controls[1].name;
            CtrlData[1].dwStyle     = create_info->pDlgTemplate->controls[1].dwStyle;
            break;

        case MB_YESNO:
            MsgBoxData.controlnr    = 2;
            CtrlData[0].class_name  = CTRL_BUTTON;
            CtrlData[0].ctl_rect    = create_info->pDlgTemplate->controls[0].ctl_rect;
            CtrlData[0].dwAddData   = create_info->pDlgTemplate->controls[0].dwAddData;
            CtrlData[0].dwExStyle   = create_info->pDlgTemplate->controls[0].dwExStyle;
            CtrlData[0].id          = IDYES;
            CtrlData[0].name        = create_info->pDlgTemplate->controls[0].name;
            CtrlData[0].dwStyle     = create_info->pDlgTemplate->controls[0].dwStyle;
            CtrlData[1].class_name  = CTRL_BUTTON;
            CtrlData[1].ctl_rect    = create_info->pDlgTemplate->controls[1].ctl_rect;
            CtrlData[1].dwAddData   = create_info->pDlgTemplate->controls[1].dwAddData;
            CtrlData[1].dwExStyle   = create_info->pDlgTemplate->controls[1].dwExStyle;
            CtrlData[1].id          = IDNO;
            CtrlData[1].name        = create_info->pDlgTemplate->controls[1].name;
            CtrlData[1].dwStyle     = create_info->pDlgTemplate->controls[1].dwStyle;
            break;

        case MB_RETRYCANCEL:
            MsgBoxData.controlnr    = 2;
            CtrlData[0].class_name  = CTRL_BUTTON;
            CtrlData[0].ctl_rect    = create_info->pDlgTemplate->controls[0].ctl_rect;
            CtrlData[0].dwAddData   = create_info->pDlgTemplate->controls[0].dwAddData;
            CtrlData[0].dwExStyle   = create_info->pDlgTemplate->controls[0].dwExStyle;
            CtrlData[0].id          = IDRETRY;
            CtrlData[0].name        = create_info->pDlgTemplate->controls[0].name;
            CtrlData[0].dwStyle     = create_info->pDlgTemplate->controls[0].dwStyle;
            CtrlData[1].class_name  = CTRL_BUTTON;
            CtrlData[1].ctl_rect    = create_info->pDlgTemplate->controls[1].ctl_rect;
            CtrlData[1].dwAddData   = create_info->pDlgTemplate->controls[1].dwAddData;
            CtrlData[1].dwExStyle   = create_info->pDlgTemplate->controls[1].dwExStyle;
            CtrlData[1].id          = IDCANCEL;
            CtrlData[1].name        = create_info->pDlgTemplate->controls[1].name;
            CtrlData[1].dwStyle     = create_info->pDlgTemplate->controls[1].dwStyle;
            break;

        case MB_ABORTRETRYIGNORE:
            MsgBoxData.controlnr    = 3;
            CtrlData[0].class_name  = CTRL_BUTTON;
            CtrlData[0].ctl_rect    = create_info->pDlgTemplate->controls[0].ctl_rect;
            CtrlData[0].dwAddData   = create_info->pDlgTemplate->controls[0].dwAddData;
            CtrlData[0].dwExStyle   = create_info->pDlgTemplate->controls[0].dwExStyle;
            CtrlData[0].id          = IDABORT;
            CtrlData[0].name        = create_info->pDlgTemplate->controls[0].name;
            CtrlData[0].dwStyle     = create_info->pDlgTemplate->controls[0].dwStyle;
            CtrlData[1].class_name  = CTRL_BUTTON;
            CtrlData[1].ctl_rect    = create_info->pDlgTemplate->controls[1].ctl_rect;
            CtrlData[1].dwAddData   = create_info->pDlgTemplate->controls[1].dwAddData;
            CtrlData[1].dwExStyle   = create_info->pDlgTemplate->controls[1].dwExStyle;
            CtrlData[1].id          = IDRETRY;
            CtrlData[1].name        = create_info->pDlgTemplate->controls[1].name;
            CtrlData[1].dwStyle     = create_info->pDlgTemplate->controls[1].dwStyle;
            CtrlData[2].class_name  = CTRL_BUTTON;
            CtrlData[2].ctl_rect    = create_info->pDlgTemplate->controls[2].ctl_rect;
            CtrlData[2].dwAddData   = create_info->pDlgTemplate->controls[2].dwAddData;
            CtrlData[2].dwExStyle   = create_info->pDlgTemplate->controls[2].dwExStyle;
            CtrlData[2].id          = IDIGNORE;
            CtrlData[2].name        = create_info->pDlgTemplate->controls[2].name;
            CtrlData[2].dwStyle     = create_info->pDlgTemplate->controls[2].dwStyle;
            break;

        case MB_YESNOCANCEL:
            MsgBoxData.controlnr    = 3;
            CtrlData[0].class_name  = CTRL_BUTTON;
            CtrlData[0].ctl_rect    = create_info->pDlgTemplate->controls[0].ctl_rect;
            CtrlData[0].dwAddData   = create_info->pDlgTemplate->controls[0].dwAddData;
            CtrlData[0].dwExStyle   = create_info->pDlgTemplate->controls[0].dwExStyle;
            CtrlData[0].id          = IDYES;
            CtrlData[0].name        = create_info->pDlgTemplate->controls[0].name;
            CtrlData[0].dwStyle     = create_info->pDlgTemplate->controls[0].dwStyle;
            CtrlData[1].class_name  = CTRL_BUTTON;
            CtrlData[1].ctl_rect    = create_info->pDlgTemplate->controls[1].ctl_rect;
            CtrlData[1].dwAddData   = create_info->pDlgTemplate->controls[1].dwAddData;
            CtrlData[1].dwExStyle   = create_info->pDlgTemplate->controls[1].dwExStyle;
            CtrlData[1].id          = IDNO;
            CtrlData[1].name        = create_info->pDlgTemplate->controls[1].name;
            CtrlData[1].dwStyle     = create_info->pDlgTemplate->controls[1].dwStyle;
            CtrlData[2].class_name  = CTRL_BUTTON;
            CtrlData[2].ctl_rect    = create_info->pDlgTemplate->controls[2].ctl_rect;
            CtrlData[2].dwAddData   = create_info->pDlgTemplate->controls[2].dwAddData;
            CtrlData[2].dwExStyle   = create_info->pDlgTemplate->controls[2].dwExStyle;
            CtrlData[2].id          = IDCANCEL;
            CtrlData[2].name        = create_info->pDlgTemplate->controls[2].name;
            CtrlData[2].dwStyle     = create_info->pDlgTemplate->controls[2].dwStyle;
            break;
    }

    MsgBoxData.controls = CtrlData;
    dlg_create.DialogProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)MsgBoxProc);
    dlg_create.hLayer       = create_info->hLayer;
    dlg_create.dwAddData    = (unsigned long)create_info->skin;
    dlg_create.hOwner       = create_info->hOwner;
    dlg_create.hHosting     = create_info->hHosting;
    dlg_create.name         = create_info->name;
    dlg_create.pDlgTemplate = &MsgBoxData;
    return GUI_ModuleDialogCreate(&dlg_create);
}



/*
**********************************************************************************************************************
*                                               GUI_GMsgBoxSetText
*
* author:
*
* date:             2009-11-29
*
* Description:      gui gmsgboxsettext
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_GMsgBoxSetText(H_WIN hbox, char *text)
{
    msgbox_para_t  *user_data;

    if ((!hbox) || (!text))
    {
        __wrn("input parameter error!\n");
        return ORANGE_FAIL;
    }

    user_data   = (msgbox_para_t *)GUI_WinGetAttr(hbox);
    user_data->text = text;
    return ORANGE_OK;
}


/*
**********************************************************************************************************************
*                                               GUI_GMsgBoxSetTextPos
*
* author:
*
* date:             2009-11-29
*
* Description:      gui gmsgboxsettextpos
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_GMsgBoxSetTextPos(H_WIN hbox, __pos_t *pos)
{
    msgbox_para_t  *user_data;

    if ((!hbox) || (!pos))
    {
        __wrn("input parameter error!\n");
        return ORANGE_FAIL;
    }

    user_data   = (msgbox_para_t *)GUI_WinGetAttr(hbox);
    user_data->text_pos.x = pos->x;
    user_data->text_pos.y = pos->y;
    return ORANGE_OK;
}

#endif

