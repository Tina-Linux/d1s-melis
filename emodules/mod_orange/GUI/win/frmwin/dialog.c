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
#include "frmwin_i.h"

/*
**********************************************************************************************************************
*                                               GUI_DlgGetFirstButton
*
* author:
*
* date:             2009-11-29
*
* Description:      gui dlggetfirstbutton
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN  GUI_DlgGetFirstButton(H_WIN hDlg)
{
    __gui_widget_t                  *pCtrl;
    __gui_win_t                     *pWin;
#if GUI_CHECK_ARG_EN > 0

    if (hDlg == NULL)
    {
        ORANGE_WRN("input parameter error!\n");
        return NULL;
    }

#endif
    pWin = GUI_WinH2P(hDlg);
    pCtrl = (__gui_widget_t *)pWin->FirstChild;

    while (pCtrl)
    {
        if (orange_strcmp(pCtrl->pcci->name, CTRL_BUTTON) == 0)
        {
            return GUI_WinP2H(pCtrl);
        }
    }

    return NULL;
}
/*
**********************************************************************************************************************
*                                               GUI_GetNextDlgTabItem
*
* author:
*
* date:             2009-9-6
*
* Description:      gui getnextdlgtabitem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN GUI_GetNextDlgTabItem(H_WIN hDlg, H_WIN hCtl, __bool bPrevious)
{
    __gui_widget_t *pFirstCtrl;
    __gui_widget_t *pLastCtrl;
    __gui_widget_t *pCtrl;
    __gui_frmwin_t *pFrmWin = (__gui_frmwin_t *)GUI_WinH2P(hDlg);
    pFirstCtrl = pCtrl = (__gui_widget_t *)(pFrmWin->base_win.FirstChild);

    if (!pCtrl)
    {
        return hCtl;
    }

    while (pCtrl->base_win.Next)
    {
        pCtrl = (__gui_widget_t *)pCtrl->base_win.Next;
    }

    pLastCtrl = pCtrl;

    if (hCtl)
    {
        pCtrl = (__gui_widget_t *)GUI_WinH2P(hCtl);

        if (bPrevious)
        {
            pCtrl = (__gui_widget_t *)pCtrl->base_win.Prev;
        }
        else
        {
            pCtrl = (__gui_widget_t *)pCtrl->base_win.Next;
        }
    }
    else if (bPrevious)
    {
        pCtrl = pLastCtrl;
    }
    else
    {
        pCtrl = pFirstCtrl;
    }

    if (bPrevious)
    {
        while (pCtrl)
        {
            if (pCtrl->base_win.dwStyle & WS_TABSTOP
                && pCtrl->base_win.dwStyle & WS_VISIBLE
                && !(pCtrl->base_win.dwStyle & WS_DISABLED))
            {
                return GUI_WinP2H(pCtrl);
            }

            pCtrl = (__gui_widget_t *)pCtrl->base_win.Prev;
        }

        pCtrl = pLastCtrl;

        while (pCtrl)
        {
            if (pCtrl->base_win.dwStyle & WS_TABSTOP
                && pCtrl->base_win.dwStyle & WS_VISIBLE
                && !(pCtrl->base_win.dwStyle & WS_DISABLED))
            {
                return GUI_WinP2H(pCtrl);
            }

            pCtrl = (__gui_widget_t *)pCtrl->base_win.Prev;
        }
    }
    else
    {
        while (pCtrl)
        {
            if (pCtrl->base_win.dwStyle & WS_TABSTOP
                && pCtrl->base_win.dwStyle & WS_VISIBLE
                && !(pCtrl->base_win.dwStyle & WS_DISABLED))
            {
                return GUI_WinP2H(pCtrl);
            }

            pCtrl = (__gui_widget_t *)pCtrl->base_win.Next;
        }

        pCtrl = pFirstCtrl;

        while (pCtrl)
        {
            if (pCtrl->base_win.dwStyle & WS_TABSTOP
                && pCtrl->base_win.dwStyle & WS_VISIBLE
                && !(pCtrl->base_win.dwStyle & WS_DISABLED))
            {
                return GUI_WinP2H(pCtrl);
            }

            pCtrl = (__gui_widget_t *)pCtrl->base_win.Next;
        }
    }

    return hCtl;
}

/*
**********************************************************************************************************************
*                                               GUI_CreateModuleDialog
*
* author:
*
* date:             2009-9-6
*
* Description:      创建并显示一个模式对话框
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_ModuleDialogCreate(__gui_dialogcreate_para_t *create_info)
{
    __gui_framewincreate_para_t CreateInfo;
    H_WIN                       hDlg;
    __s32                       retCode = IDCANCEL;
    H_WIN                       hFocus;
    __gui_msg_t                 Msg;
    H_WIN                       hOwner;
    __s32                       isActive;

    if (create_info->pDlgTemplate->controlnr > 0 && !create_info->pDlgTemplate->controls)
    {
        __wrn("input control number error!\n");
        return ORANGE_FAIL;
    }

    hOwner = GUI_WinGetFrmWin(create_info->hOwner);
    CreateInfo.BkColor.alpha    = 0;
    CreateInfo.BkColor.red      = 0;
    CreateInfo.BkColor.green    = 0;
    CreateInfo.BkColor.blue     = 0;
    CreateInfo.dwStyle          = create_info->pDlgTemplate->dwStyle;
    CreateInfo.dwExStyle        = create_info->pDlgTemplate->dwExStyle;
    CreateInfo.spCaption        = create_info->pDlgTemplate->caption;
    CreateInfo.FrameWinProc     = create_info->DialogProc;
    CreateInfo.rect.x           = create_info->pDlgTemplate->dlg_rect.x;
    CreateInfo.rect.y           = create_info->pDlgTemplate->dlg_rect.y;
    CreateInfo.rect.width       = create_info->pDlgTemplate->dlg_rect.width;
    CreateInfo.rect.height      = create_info->pDlgTemplate->dlg_rect.height;

    if (create_info->hHosting)
    {
        CreateInfo.hHosting         = create_info->hHosting;
    }
    else
    {
        if (hOwner)
        {
            CreateInfo.hHosting     = GUI_WinGetManWin(hOwner);
        }
        else
        {
            CreateInfo.hHosting     = NULL;
        }
    }

    /*全局对话框*/
    if (create_info->hLayer)
    {
        CreateInfo.hLayer       = create_info->hLayer;
    }
    else
    {
        /*本地对话框*/
        CreateInfo.hLayer       = GUI_WinGetLyrWin(hOwner);
    }

    CreateInfo.id               = create_info->id;
    CreateInfo.hOwner           = hOwner;
    CreateInfo.attr             = (void *)create_info->dwAddData;
    CreateInfo.name             = create_info->name;
    CreateInfo.dwReserved       = (unsigned long)create_info->pDlgTemplate;
    hDlg = GUI_FrmWinCreate(&CreateInfo);

    if (hDlg == HWND_INVALID)
    {
        __wrn("create framewin failed!\n");
        return ORANGE_FAIL;
    }

    GUI_WinSetFocusChild(hDlg);
    hFocus = GUI_GetNextDlgTabItem(hDlg, GUI_WinP2H(0), ORANGE_FALSE);
    {
        __gui_msg_t     new_msg;
        new_msg.id          = GUI_MSG_INITDIALOG;
        new_msg.h_srcwin    = NULL;
        new_msg.h_deswin    = hDlg;
        new_msg.dwAddData1  = (unsigned long)hFocus;
        new_msg.dwAddData2  = 0;
        new_msg.dwReserved  = create_info->dwAddData;
        new_msg.p_arg       = NULL;

        if (GUI_SendMessage(&new_msg))
        {
            if (hFocus)
            {
                GUI_WinSetFocusChild(hFocus);
            }
        }
    }
    GUI_LyrWinSetSta(GUI_LyrP2H(GUI_WinGetLyrWin(hDlg)), GUI_LYRWIN_STA_ON);
    GUI_WinSetTouchFocus(hDlg);

    while (GUI_GetMessageEx(&Msg, hDlg))
    {
        __s32  ret = ORANGE_FAIL;
        ret = GUI_DispatchMessage(&Msg);

        if (Msg.p_arg)                           // 同步消息回应
        {
            GUI_SetSyncMsgRetVal(&Msg, ret);
            GUI_PostSyncSem(&Msg);
        }
    }

    GUI_WinReleaseTouchFocus();
    retCode = GUI_WinGetAddData(hDlg);
    isActive = (GUI_WinGetFocusChild(CreateInfo.hHosting) == hDlg);
    GUI_WinThreadCleanup(hDlg);

    if (hOwner)
    {
        if (isActive)
        {
            /*非全局对话框*/
            if (create_info->hLayer)
            {
                GUI_WinShow(hOwner, GUI_SW_SHOWNORMAL);
            }

            GUI_WinSetFocusChild(hOwner);
        }
    }

    return retCode;
}

/*
**********************************************************************************************************************
*                                               GUI_EndDialog
*
* author:
*
* date:             2009-9-6
*
* Description:      gui enddialog
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool GUI_EndDialog(H_WIN hDlg, __s32 endCode)
{
    __u32 dwAddData;
    dwAddData = GUI_WinGetAddData(hDlg);
    GUI_WinSetAddData(hDlg, (__u32)endCode);
    GUI_WinDelete(hDlg);
    GUI_PostQuitMessage(hDlg);
    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               GUI_GetDlgItem
*
* author:
*
* date:             2009-9-6
*
* Description:      gui getdlgitem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN GUI_GetDlgItem(H_WIN hDlg, __s32 nIDDlgItem)
{
    __gui_widget_t      *pCtrl;
    __gui_frmwin_t      *pManWin = (__gui_frmwin_t *)GUI_WinH2P(hDlg);
    pCtrl = (__gui_widget_t *)(pManWin->base_win.FirstChild);

    while (pCtrl)
    {
        if (pCtrl->base_win.id == nIDDlgItem)
        {
            return GUI_WinP2H(pCtrl);
        }

        pCtrl = (__gui_widget_t *)pCtrl->base_win.Next;
    }

    return 0;
}

/*
**********************************************************************************************************************
*                                               GUI_DialogDefaultProc
*
* author:
*
* date:             2009-9-6
*
* Description:      gui dialogdefaultproc
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_DialogDefaultProc(__gui_msg_t    *msg)
{
    H_WIN hCurFocus;

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            __s32                       i;
            __gui_ctrldata_t            *pCtrlData;
            H_WIN                       hCtrl;
            __gui_dlgtemplate_t         *pDlgTmpl;
            /*
            PDLGTEMPLATE pDlgTmpl
                        = (PDLGTEMPLATE)(((PMAINWINCREATE)lParam)->dwAddData);
            */
            pDlgTmpl  = (__gui_dlgtemplate_t *)(((__gui_framewincreate_para_t *)msg->dwAddData2)->dwReserved);

            for (i = 0; i < pDlgTmpl->controlnr; i++)
            {
                pCtrlData = pDlgTmpl->controls + i;

                if (pCtrlData->class_name)
                {
                    __gui_ctlwincreate_para_t create_info;
                    create_info.spClassName     = pCtrlData->class_name;
                    create_info.spCaption       = pCtrlData->caption;
                    create_info.dwStyle         = pCtrlData->dwStyle | WS_CHILD;
                    create_info.dwExStyle       = pCtrlData->dwExStyle;
                    create_info.id              = pCtrlData->id;
                    create_info.name            = pCtrlData->name;
                    create_info.hParent         = msg->h_deswin;
                    create_info.ctl_rect.x      = pCtrlData->ctl_rect.x;
                    create_info.ctl_rect.y      = pCtrlData->ctl_rect.y;
                    create_info.ctl_rect.width  = pCtrlData->ctl_rect.width;
                    create_info.ctl_rect.height = pCtrlData->ctl_rect.height;
                    create_info.attr            = (void *)pCtrlData->dwAddData;
                    hCtrl = GUI_CtrlWinCreate(&create_info);
                }
                else
                {
                    break;
                }

                if (hCtrl == HWND_INVALID)
                {
                    __wrn("create control win failed!\n");
                    GUI_FrmWinDestroyAllCtrlWins(msg->h_deswin);
                    return ORANGE_FAIL;
                }
            }

            /* set back dwAddData to be the value from dialog template */
            //SetWindowAdditionalData (hWnd, pDlgTmpl->dwAddData);
            return 0;
        }

        case GUI_MSG_COMMAND:
            if (msg->dwAddData1 == IDCANCEL)
            {
                H_WIN hCancel;
                hCancel = GUI_GetDlgItem(msg->h_deswin, IDCANCEL);

                if (hCancel && (!(GUI_WinGetStyle(hCancel) & WS_DISABLED))
                    && (!(GUI_WinGetStyle(hCancel) & WS_DISABLED)))
                {
                    GUI_EndDialog(msg->h_deswin, IDCANCEL);
                }
            }

            break;

        case GUI_MSG_CLOSE:
        {
            H_WIN hCancel;
            hCancel = GUI_GetDlgItem(msg->h_deswin, IDCANCEL);

            if (hCancel && (!(GUI_WinGetStyle(hCancel) & WS_DISABLED))
                && (!(GUI_WinGetStyle(hCancel) & WS_DISABLED)))
            {
                GUI_EndDialog(msg->h_deswin, IDCANCEL);
            }

            return 0;
        }

        case GUI_MSG_KEY:
            switch (msg->dwAddData1)
            {
                case GUI_MSG_KEY_DOWN:
                case GUI_MSG_KEY_LONGDOWN:
                case GUI_MSG_KEY_RIGHT:
                case GUI_MSG_KEY_LONGRIGHT:
                {
                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        H_WIN   hNewFocus;
                        hCurFocus = GUI_WinGetFocusChild(msg->h_deswin);
                        hNewFocus = GUI_GetNextDlgTabItem(msg->h_deswin, hCurFocus, ORANGE_FALSE);

                        if (hNewFocus != hCurFocus)
                        {
                            GUI_WinSetFocusChild(hNewFocus);
                        }
                    }

                    return 0;
                }

                case GUI_MSG_KEY_LEFT:
                case GUI_MSG_KEY_LONGLEFT:
                case GUI_MSG_KEY_UP:
                case GUI_MSG_KEY_LONGUP:
                {
                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        H_WIN   hNewFocus;
                        hCurFocus = GUI_WinGetFocusChild(msg->h_deswin);
                        hNewFocus = GUI_GetNextDlgTabItem(msg->h_deswin, hCurFocus, ORANGE_TRUE);

                        if (hNewFocus != hCurFocus)
                        {
                            GUI_WinSetFocusChild(hNewFocus);
                        }
                    }

                    return 0;
                }

                case GUI_MSG_KEY_ENTER:
                case GUI_MSG_KEY_LONGENTER:
                {
                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        H_WIN hDef;
                        hDef = GUI_WinGetFocusChild(msg->h_deswin);

                        if (hDef)
                        {
                            __gui_msg_t     new_msg;
                            new_msg.id          = GUI_MSG_COMMAND;
                            new_msg.h_deswin    = msg->h_deswin;
                            new_msg.h_srcwin    = hDef;
                            new_msg.dwAddData1  = (__u32)MAKELONG(GUI_WinGetItemId(hDef), 0);
                            new_msg.dwAddData2  = GUI_MSG_KEY_ENTER;
                            GUI_SendMessage(&new_msg);
                            return 0;
                        }
                    }

                    break;
                }
            }

            break;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

/*
**********************************************************************************************************************
*                                               GUI_ModulelessDialogCreate
*
* author:
*
* date:             2009-9-6
*
* Description:      创建并显示一个非模式对话框
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN GUI_ModulelessDialogCreate(__gui_dialogcreate_para_t *create_info)
{
    __gui_framewincreate_para_t CreateInfo;
    H_WIN                       hManWin;
    __s32                       i;
    __gui_ctrldata_t            *pCtrlData;
    H_WIN                       hCtrl;
    H_WIN                       hFocus;
    H_WIN                       hOwner;

    if (create_info->pDlgTemplate->controlnr > 0 && !create_info->pDlgTemplate->controls)
    {
        __wrn("input controls error!\n");
        return HWND_INVALID;
    }

    hOwner = GUI_WinGetFrmWin(create_info->hOwner);
    CreateInfo.dwReserved       = create_info->pDlgTemplate->dwAddData;
    CreateInfo.dwStyle          = create_info->pDlgTemplate->dwStyle & ~WS_VISIBLE;
    CreateInfo.dwExStyle        = create_info->pDlgTemplate->dwExStyle;
    CreateInfo.spCaption        = create_info->pDlgTemplate->caption;
    CreateInfo.FrameWinProc     = create_info->DialogProc ? create_info->DialogProc : GUI_DialogDefaultProc;
    CreateInfo.rect.x           = create_info->pDlgTemplate->dlg_rect.x;
    CreateInfo.rect.y           = create_info->pDlgTemplate->dlg_rect.y;
    CreateInfo.rect.width       = create_info->pDlgTemplate->dlg_rect.width;
    CreateInfo.rect.height      = create_info->pDlgTemplate->dlg_rect.height;
    CreateInfo.attr             = (void *)create_info->pDlgTemplate->dwAddData;
    CreateInfo.hHosting         = GUI_WinGetManWin(hOwner);
    CreateInfo.hOwner           = create_info->hOwner;
    CreateInfo.hLayer           = create_info->hLayer;
    CreateInfo.name             = create_info->name;
    CreateInfo.id               = create_info->id;
    hManWin = GUI_FrmWinCreate(&CreateInfo);

    if (hManWin == HWND_INVALID)
    {
        __wrn("create framewin failed!\n");
        return HWND_INVALID;
    }

    for (i = 0; i < create_info->pDlgTemplate->controlnr; i++)
    {
        pCtrlData = create_info->pDlgTemplate->controls + i;

        if (pCtrlData->class_name)
        {
            __gui_ctlwincreate_para_t create_info;
            create_info.spClassName     = pCtrlData->class_name;
            create_info.spCaption       = pCtrlData->caption;
            create_info.dwStyle         = pCtrlData->dwStyle | WS_CHILD;
            create_info.dwExStyle       = pCtrlData->dwExStyle;
            create_info.id              = pCtrlData->id;
            create_info.name            = pCtrlData->name;
            create_info.hParent         = hManWin;
            create_info.ctl_rect.x      = pCtrlData->ctl_rect.x;
            create_info.ctl_rect.y      = pCtrlData->ctl_rect.x;
            create_info.ctl_rect.width  = pCtrlData->ctl_rect.width;
            create_info.ctl_rect.height = pCtrlData->ctl_rect.height;
            create_info.attr            = (void *)pCtrlData->dwAddData;
            hCtrl = GUI_CtrlWinCreate(&create_info);
        }
        else
        {
            break;
        }

        if (hCtrl == HWND_INVALID)
        {
            GUI_FrmWinDestroyAllCtrlWins(hManWin);
            GUI_FrmWinDelete(hManWin);
            GUI_WinThreadCleanup(hManWin);
            return HWND_INVALID;
        }
    }

    hFocus = GUI_GetNextDlgTabItem(hManWin, GUI_WinP2H(0), ORANGE_FALSE);
    {
        __gui_msg_t     new_msg;
        new_msg.id          = GUI_MSG_INITDIALOG;
        new_msg.h_srcwin    = NULL;
        new_msg.h_deswin    = hManWin;
        new_msg.dwAddData1  = (unsigned long)hFocus;
        new_msg.dwAddData2  = create_info->dwAddData;
        new_msg.p_arg       = NULL;

        if (GUI_SendMessage(&new_msg))
        {
            if (hFocus)
            {
                GUI_WinSetFocusChild(hFocus);
            }
        }
    }
    GUI_WinShow(hManWin, GUI_SW_SHOWNORMAL);
    return hManWin;
}

/*
**********************************************************************************************************************
*                                               GUI_ModulelessDialogDelete
*
* author:
*
* date:             2009-9-6
*
* Description:      删除一个无模式对话框
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool GUI_ModulelessDialogDelete(H_WIN hManWin)
{
    GUI_FrmWinDestroyAllCtrlWins(hManWin);
    GUI_FrmWinDelete(hManWin);
    GUI_ThrowAwayMessages(hManWin);
    GUI_WinThreadCleanup(hManWin);
    return ORANGE_TRUE;
}
