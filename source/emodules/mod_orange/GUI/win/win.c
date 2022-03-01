/*
*******************************************************************************************************************
*                                                           Mod orange
*                                                   the embedded graphic support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    win.c
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2009-8-4
*
*Description :  gui通用函数实现列表
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>                  <version>     <description>
*
* William Wang     2009-8-4         1.1.0          Create File
*
*******************************************************************************************************************
*/

#include "win_i.h"
#include "win_e.h"
#include "message_e.h"
#include <kconfig.h>


static H_WIN  h_backwin;

static __s32 GUI_RootWinProc(__gui_msg_t *msg);
/* static */
static __gui_manwincreate_para_t backwin_para =
{
    "back win",
    0,                   //parent  windows
    NULL,                   //hosting windows
    NULL,
    GUI_RootWinProc,        //back windows process //暂不使用背景窗口的
    0                       //private attr
};

H_WIN                           capture_wnd             = 0;
H_WIN                           touchfocus_wnd          = 0;
pguiwin                         active_mainwnd          = 0;
__gui_lyrwin_t                  *sg_InVisLayerList      = NULL;
__gui_lyrwin_t                  *sg_VisiLayerList       = NULL;

__gui_lyrwin_t                  *sg_InVisSpriteList      = NULL;
__gui_lyrwin_t                  *sg_VisiSpriteList       = NULL;

__s32 GUI_RootWinProc(__gui_msg_t *msg)
{
    if (!msg)
    {
        ORANGE_WRN("message para can not be NULL!");
        return ORANGE_FAIL;
    }

    switch (msg->id)
    {
        case  GUI_MSG_NOTIFY_CHILD_DELETED:
        {
            if (GUI_WinP2H(active_mainwnd) == msg->h_srcwin)
            {
                if (GUI_WinGetOwnerWin(msg->h_srcwin))
                {
                    GUI_WinSetFocusChild(GUI_WinGetOwnerWin(msg->h_srcwin));
                }
                else
                {
                    /*mainman win删除之后将焦点窗口置为空，避免这段时间的消息发送到无效窗口*/
                    active_mainwnd = NULL;
                }
            }
        }
        break;

        case GUI_MSG_SET_UNFOCUS:
        case GUI_MSG_SET_FOCUS:
        {
            if (active_mainwnd)
            {
                __gui_msg_t     newmsg;
                newmsg.id           = msg->id;
                newmsg.h_srcwin     = GUI_WinP2H(msg->h_deswin);
                newmsg.h_deswin     = GUI_WinP2H(active_mainwnd);
                newmsg.dwAddData1   = 0;
                newmsg.dwAddData2   = 0;
                //GUI_SendNotifyMessage (&newmsg);
                GUI_SendMessage(&newmsg);
            }
        }
        break;

        default:
            break;
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               msgCheckInvalidRegion
*
* author:
*
* date:             2009-7-22
*
* Description:      msgcheckinvalidregion
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static H_WIN msgCheckInvalidRegion(pguiwin pWin)
{
    __gui_widget_t   *pCtrl = (__gui_widget_t *)pWin;
    H_WIN             hwnd;

    if (pCtrl == NULL)
    {
        return 0;
    }

    if (pCtrl->base_win.InvRgn.rgn.head)
    {
        return (H_WIN)pCtrl;
    }

    pCtrl = (__gui_widget_t *)pCtrl->base_win.FirstChild;

    while (pCtrl)
    {
        hwnd = msgCheckInvalidRegion((pguiwin) pCtrl);

        if (hwnd)
        {
            return hwnd;
        }

        pCtrl = (__gui_widget_t *)pCtrl->base_win.Next;
    }

    return 0;
}

/*
**********************************************************************************************************************
*                                               GUI_Setfocus_helper
*
* author:
*
* date:             2009-7-28
*
* Description:      gui setfocus helper
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static H_WIN GUI_Setfocus_helper(H_WIN hWnd)
{
    __gui_win_t   *pWin = GUI_WinH2P(hWnd);
    __gui_win_t   *old_active;
    if (hWnd == NULL)
    {
        __inf("GUI_Setfocus_helper hWnd==null");
    }
    __inf("enter GUI_Setfocus_helper");

    if (HWND_INVALID == hWnd)
    {
        return HWND_INVALID;
    }

    __inf("before GUI_WinGetParent");
    pWin = (__gui_win_t *) GUI_WinGetParent(hWnd);

    if (GUI_WinGetParent(hWnd) == h_backwin)
    {
        old_active = active_mainwnd;
    }
    else
    {
        old_active = pWin->ActiveChild;
    }

    __inf("after GUI_WinGetParent");
    __inf("before pWin->DataType");
    __inf("the TYPE_HWND:%d", TYPE_HWND);

    if (pWin == NULL)
    {
        __inf("pWin==null");
    }

    if (pWin->DataType != TYPE_HWND)
    {
        __inf("before ORANGE_WRN");
        ORANGE_WRN("windows type error!");
        __inf("after ORANGE_WRN");
        return HWND_INVALID;
    }

    __inf("after pWin->DataType");

    if (old_active != GUI_WinH2P(hWnd))
    {
        __inf("after GUI_WinH2P");

        if (old_active)
        {
            __gui_msg_t  new_msg;
            new_msg.h_srcwin    = 0;
            new_msg.h_deswin    = GUI_WinP2H(old_active);
            new_msg.dwAddData1  = (unsigned long)hWnd;
            new_msg.dwAddData2  = 0;
            new_msg.id          = GUI_MSG_SET_UNFOCUS;
            new_msg.p_arg       = (void *)NULL;
            __inf("before GUI_SendNotifyMessage1");
            GUI_SendNotifyMessage(&new_msg);
            //GUI_SendMessage (&new_msg);
        }

        __inf("after pWin->ActiveChild ");
        pWin->ActiveChild = GUI_WinH2P(hWnd);
        {
            __gui_msg_t  new_msg;
            new_msg.h_srcwin    = 0;
            new_msg.h_deswin    = hWnd;
            new_msg.dwAddData1  = (unsigned long)GUI_WinP2H(old_active);
            new_msg.dwAddData2  = 0;
            new_msg.id          = GUI_MSG_SET_FOCUS;
            new_msg.p_arg       = (void *)NULL;
            __inf("before GUI_SendNotifyMessage2");
            GUI_SendNotifyMessage(&new_msg);
            //GUI_SendMessage (&new_msg);
        }
    }
    else
    {
        __gui_msg_t  new_msg;
        new_msg.h_srcwin    = 0;
        new_msg.h_deswin    = hWnd;
        new_msg.dwAddData1  = (unsigned long)GUI_WinP2H(old_active);
        new_msg.dwAddData2  = 0;
        new_msg.id          = GUI_MSG_SET_FOCUS;
        new_msg.p_arg       = (void *)NULL;
        __inf("before GUI_SendNotifyMessage3");
        GUI_SendNotifyMessage(&new_msg);
        //GUI_SendMessage (&new_msg);
    }

    return GUI_WinP2H(pWin->ActiveChild);
}




/*
**********************************************************************************************************************
*                                               GUI_SysToGUIColor
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__u32   GUI_SysToGUIColor(__color_t *color)
{
    if (color)
    {
        return (__u32)(((color->alpha) << 24) | ((color->red) << 16) |
                       ((color->green) << 8) | color->blue);
    }

    return 0;
}
/*
**********************************************************************************************************************
*                                               GUI_WinGetClientRect
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__bool  GUI_WinGetClientRect(H_WIN hWnd, __gui_rect_t *prc)
{
    __gui_win_t  *pWin = GUI_WinH2P(hWnd);

    if (HWND_INVALID == hWnd)
    {
        return ORANGE_FALSE;
    }

    if (hWnd == h_backwin)
    {
        *prc = g_rcScr;
        return ORANGE_TRUE;
    }
    else if (pWin->DataType != TYPE_HWND)
    {
        return ORANGE_FALSE;
    }

    prc->left   = prc->top = 0;
    prc->right  = pWin->client_rect.right   - pWin->client_rect.left;
    prc->bottom = pWin->client_rect.bottom - pWin->client_rect.top;
    return ORANGE_TRUE;
}
/*
**********************************************************************************************************************
*                                               FUNCTION
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__bool GUI_WndInvalidateRect(H_WIN hWnd, const __gui_rect_t *prc, __bool bEraseBkgnd)
{
#if 0
    __gui_win_t         *pWin;
    __gui_win_t         *pCtrl = NULL;
    __gui_rect_t        rcInter;
    __gui_rect_t        rcInvalid;
    __gui_rect_t        rcClient;
    __win_invregn_t     *pInvRgn;
    pWin = (__gui_win_t *)hWnd;
    pCtrl = (__gui_win_t *)hWnd;

    if (pWin->WinType == TYPE_MANWIN)
    {
        rcClient.left = rcClient.top = 0;
        rcClient.right = pWin->cr - pWin->cl;
        rcClient.bottom = pWin->cb - pWin->ct;
        pInvRgn = &pWin->InvRgn;

        if (pInvRgn->frozen)
        {
            return FALSE;
        }

#ifndef _LITE_VERSION
        pthread_mutex_lock(&pInvRgn->lock);
#endif

        if (prc)
        {
            rcInvalid = *prc;
            NormalizeRect(&rcInvalid);

            if (IsCovered(&rcClient, &rcInvalid))
            {
                SetClipRgn(&pInvRgn->rgn, &rcClient);
            }
            else
            {
                if (IntersectRect(&rcInter, &rcInvalid, &rcClient))
                {
                    AddClipRect(&pInvRgn->rgn, &rcInter);
                }
            }
        }
        else
        {
            SetClipRgn(&pInvRgn->rgn, &rcClient);
            rcInvalid = rcClient;
        }

        pthread_mutex_unlock(&pInvRgn->lock);

        // erase background here.
        if (bEraseBkgnd)
        {
            ClientToScreen(hWnd, &rcInvalid.left, &rcInvalid.top);
            ClientToScreen(hWnd, &rcInvalid.right, &rcInvalid.bottom);
            SendAsyncMessage(hWnd, MSG_ERASEBKGND, 0, (LPARAM)&rcInvalid);
        }
    }
    else if (pWin->WinType == TYPE_CONTROL)
    {
        rcClient.left = rcClient.top = 0;
        rcClient.right = pCtrl->cr - pCtrl->cl;
        rcClient.bottom = pCtrl->cb - pCtrl->ct;

        if (bEraseBkgnd)
        {
            if (prc && IntersectRect(&rcInter, prc, &rcClient))
            {
                SendAsyncMessage((HWND)pCtrl,
                                 MSG_ERASEBKGND, 0, (LPARAM)(&rcInter));
            }
            else if (!prc)
            {
                SendAsyncMessage((HWND)pCtrl, MSG_ERASEBKGND, 0, 0);
            }
        }

        wndUpdateInvalidRegion(hWnd, prc, &rcClient);
    }
    else
    {
        return FALSE;
    }

    if (pCtrl->children)
    {
        wndCascadeInvalidateChildren(pCtrl->children, prc, bEraseBkgnd);
    }

#endif
    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               GUI_InvalidateRect
*
* author:
*
* date:             2009-8-7
*
* Description:      gui invalidaterect
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool GUI_InvalidateRect(H_WIN hWnd, const RECT *prc, __bool bEraseBkgnd)
{
    __bool          retval = ORANGE_TRUE;
    __gui_msg_t     new_msg;
    __gui_rect_t    prect;
#if GUI_CHECK_ARG_EN > 0

    if (!hWnd)
    {
        ORANGE_WRN("prc empty!");
        return ORANGE_FALSE;
    }

#endif

    if (prc)
    {
        prect.left      = prc->x;
        prect.right     = prc->x + prc->width - 1;
        prect.bottom    = prc->y + prc->height - 1;
        prect.top       = prc->y;
        retval = GUI_WndInvalidateRect(hWnd, &prect, bEraseBkgnd);
    }

    new_msg.id          = GUI_MSG_PAINT,
    new_msg.h_srcwin    = NULL;
    new_msg.h_deswin    = hWnd;
    new_msg.dwAddData1  = 0;
    new_msg.dwAddData2  = 0;
    new_msg.p_arg       = NULL;
    //GUI_PostMessage (&new_msg);
    GUI_SendAsyncMessage(&new_msg);
#ifdef SIM_PC_WIN
    RefreshDeContent(0);
#endif
    return retval;
}

/*
**********************************************************************************************************************
*                                               GUI_UpdateWindow
*
* author:
*
* date:             2009-7-28
*
* Description:      gui updatewindow
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_WinUpdate(H_WIN hWnd, __bool fErase)
{
    GUI_InvalidateRect(hWnd, NULL, fErase);
}

/*
**********************************************************************************************************************
*                                               GUI_ShowWindow
*
* author:
*
* date:             2009-8-7
*
* Description:      gui showwindow
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool GUI_WinShow(H_WIN hWnd, int iCmdShow)
{
    __gui_win_t  *pWin      = GUI_WinH2P(hWnd);
    __gui_win_t  *pChild;
#if GUI_CHECK_ARG_EN > 0

    if (!pWin)
    {
        ORANGE_WRN("win empty!");
        return ORANGE_FALSE;
    }

#endif
    pChild = pWin->FirstChild;

    if (iCmdShow == GUI_SW_SHOWNORMAL)
    {
        GUI_InvalidateRect(hWnd, NULL, ORANGE_TRUE);

        while (pChild)
        {
            GUI_WinShow(GUI_WinP2H(pChild), GUI_SW_SHOWNORMAL);
            pChild = pChild->Next;
        }
    }

    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               GUI_EnableWindow
*
* author:
*
* date:             2009-8-7
*
* Description:      gui enablewindow
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool GUI_WinEnable(H_WIN hWnd, __bool fEnable)
{
    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               GUI_IsWindowEnabled
*
* author:
*
* date:             2009-8-7
*
* Description:      gui iswindowenabled
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool GUI_WinIsEnabled(H_WIN hWnd)
{
    return ORANGE_TRUE;
}


/*
**********************************************************************************************************************
*                                               GUI_MoveWindow
*
* author:
*
* date:             2009-8-7
*
* Description:      gui movewindow
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
// NOTE: This function is control safe
__bool GUI_WinMove(H_WIN hWnd, int x, int y, int w, int h, __bool fPaint)
{
    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               GUI_WinThreadCleanup
*
* author:
*
* date:             2009-8-19
*
* Description:      gui窗口线程相关的信息删除，用来在主窗口或者framewin窗口结束后，清楚与线程相关的信息
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_WinThreadCleanup(H_WIN hManWnd)
{
    __gui_win_t  *pWin = GUI_WinH2P(hManWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!pWin)
    {
        ORANGE_WRN("win empty!");
        return ;
    }

#endif
    /*扔掉消息队列中与该窗口相关的消息*/
    GUI_ThrowAwayMessages(hManWnd);

    if (pWin->WinType == TYPE_MANWIN)
    {
        __gui_manwin_t  *pManWin = (__gui_manwin_t *)pWin;

        /*消息宿主窗口为空的情况*/
        if (pManWin->pHosting == NULL && pManWin->ManWinType == TYPE_ROOTMAIN)
        {
            /*在清除退出消息循环之后，断开窗口联系*/
            Win_DeattachFromParent(pWin);
            DestroyMsgQueue(pManWin->pMessages);
            orange_mfree(pManWin->pMessages);
        }

        if (pManWin)
        {
            if (active_mainwnd == pWin)
            {
                active_mainwnd = NULL;
            }

            orange_mfree(pManWin);
        }
    }
    else if (pWin->WinType == TYPE_FRAMEWIN)
    {
        if (pWin)
        {
            orange_mfree(pWin);
        }
    }

    return ;
}



/*
**********************************************************************************************************************
*                                               GUI_WinGetStyte
*
* author:
*
* date:             2009-8-4
*
* Description:      gui wingetstyte
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  GUI_WinGetStyle(H_WIN hWnd)
{
    __gui_win_t *p_win = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return 0;
    }

#endif
    return p_win->dwStyle;
}

/*
**********************************************************************************************************************
*                                               GUI_WinSetStyle
*
* author:
*
* date:             2009-8-4
*
* Description:      gui winsetstyle
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_WinSetStyle(H_WIN hWnd, __u32 style)
{
    __gui_win_t *p_win = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return ORANGE_FAIL;
    }

#endif
    p_win->dwStyle = style;
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetExStyle
*
* author:
*
* date:             2009-8-4
*
* Description:      gui wingetexstyle
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  GUI_WinGetExStyle(H_WIN hWnd)
{
    __gui_win_t *p_win = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return 0;
    }

#endif
    return (p_win->dwExStyle);
}


/*
**********************************************************************************************************************
*                                               GUI_WinSetExStyle
*
* author:
*
* date:             2009-8-4
*
* Description:      gui winsetexstyle
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_WinSetExStyle(H_WIN hWnd, __u32 style)
{
    __gui_win_t *p_win = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return ORANGE_FAIL;
    }

#endif
    p_win->dwExStyle = style;
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_WinIsAncestor
*
* author:
*
* date:             2009-10-21
*
* Description:      gui winisancestor
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool GUI_WinIsAncestor(H_WIN hWnd, H_WIN hChild)
{
    __gui_win_t     *pWin    = GUI_WinH2P(hChild);
    __gui_win_t     *pParent = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!pWin)
    {
        ORANGE_WRN("h_win empty!");
        return ORANGE_FALSE;
    }

#endif

    while (pWin != GUI_WinH2P(h_backwin))
    {
        if (pWin->Parent == pParent)
        {
            return ORANGE_TRUE;
        }

        pWin = pWin->Parent;
    }

    return ORANGE_FALSE;
}



/*
**********************************************************************************************************************
*                                               GUI_WinIsChild
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__bool GUI_WinIsChild(H_WIN hWnd, H_WIN h_parent)
{
    __bool         r = EPDK_FALSE;
    __gui_win_t *p_win = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return r;
    }

#endif

    if (p_win->Parent == (__gui_win_t *)h_parent)
    {
        r = EPDK_TRUE;
    }

    return r;
}
/*
**********************************************************************************************************************
*                                               GUI_WinSetCallback
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__pGUI_WIN_CB GUI_WinSetCallback(H_WIN hWnd, __pGUI_WIN_CB cb)
{
    __pGUI_WIN_CB   r     = (__pGUI_WIN_CB)0;
    __gui_win_t    *p_win = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return (__pGUI_WIN_CB)0;
    }

#endif
    GUI_WinLock();
    r = p_win->WindowProc;
    p_win->WindowProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)cb);
    GUI_WinUnlock();
    return r;
}

/*
**********************************************************************************************************************
*                                               GetWndProc
*
* author:
*
* date:             2009-7-22
*
* Description:      getwndproc
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__pGUI_WIN_CB GUI_WinGetCallback(H_WIN hWnd)
{
    pguiwin  pManWin = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!hWnd)
    {
        ORANGE_WRN("h_win empty!");
        return (__pGUI_WIN_CB)0;
    }

#endif

    if (hWnd == h_backwin)
    {
        return (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)GUI_RootWinProc);
        //return NULL;
    }

    return (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)pManWin->WindowProc);
}

/*
**********************************************************************************************************************
*                                               GUI_SetCaptureWin
*
* author:
*
* date:             2009-7-28
*
* Description:      gui setcapturewin
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN  GUI_WinSetCaptureWin(H_WIN hWnd)
{
    H_WIN    hTemp;

    if (!hWnd)
    {
        ORANGE_WRN("windows can not be NULL!");
        return HWND_INVALID;
    }

    GUI_WinLock();
    hTemp = capture_wnd;
    capture_wnd = hWnd;
    GUI_WinUnlock();
    return hTemp;
}

/*
**********************************************************************************************************************
*                                               GUI_GetCaptureWin
*
* author:
*
* date:             2009-7-28
*
* Description:      gui getcapturewin
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN  GUI_WinGetCaptureWin(void)
{
    return capture_wnd;
}

/*
**********************************************************************************************************************
*                                               GUI_ReleaseCapture
*
* author:
*
* date:             2009-7-28
*
* Description:      gui releasecapture
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void   GUI_WinReleaseCapture(void)
{
    GUI_WinLock();
    capture_wnd = HWND_INVALID;
    GUI_WinUnlock();
    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_WinSetTouchFocus
*
* author:
*
* date:             2010-2-21
*
* Description:      gui winsettouchfocus
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN  GUI_WinSetTouchFocus(H_WIN hWnd)
{
    H_WIN    hTemp;

    if (!hWnd)
    {
        ORANGE_WRN("windows can not be NULL!");
        return HWND_INVALID;
    }

    GUI_WinLock();
    hTemp = touchfocus_wnd;
    touchfocus_wnd = hWnd;
    GUI_WinUnlock();
    return hTemp;
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetTouchFocus
*
* author:
*
* date:             2010-2-21
*
* Description:      gui wingettouchfocus
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN  GUI_WinGetTouchFocus(void)
{
    return touchfocus_wnd;
}

/*
**********************************************************************************************************************
*                                               GUI_WinReleaseTouchFocus
*
* author:
*
* date:             2010-2-21
*
* Description:      gui winreleasetouchfocus
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void   GUI_WinReleaseTouchFocus(void)
{
    GUI_WinLock();
    touchfocus_wnd = HWND_INVALID;
    GUI_WinUnlock();
    return ;
}




/*
**********************************************************************************************************************
*                                               GUI_WinGetRootWin
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
H_WIN GUI_WinGetRootWin(void)
{
    return h_backwin;
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetMainManWin
*
* author:
*
* date:             2009-9-14
*
* Description:      得到窗口的入口管理窗口句柄
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN GUI_WinGetMainManWin(H_WIN hWnd)
{
    __gui_win_t  *pWin = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!hWnd)
    {
        ORANGE_WRN("h_win empty!");
        return HWND_INVALID;
    }

#endif

    if (pWin->DataType != TYPE_HWND)
    {
        ORANGE_WRN("windows type error!");
        return HWND_INVALID;
    }

    if (pWin->WinType == TYPE_MANWIN)
    {
        __gui_manwin_t  *pManWin = (__gui_manwin_t *)pWin;

        if (pManWin->ManWinType == TYPE_ROOTMAIN)
        {
            return hWnd;
        }
        else if (pManWin->ManWinType == TYPE_CHILDMAIN)
        {
            return GUI_WinP2H(pManWin->pRootMain);
        }
        else
        {
            ORANGE_WRN("manwin type error!");
            return HWND_INVALID;
        }
    }

    if (pWin->WinType == TYPE_FRAMEWIN)
    {
        __gui_frmwin_t        *pFramewin;
        pFramewin = (__gui_frmwin_t *)pWin;
        return GUI_WinP2H(pFramewin->pManWin->pRootMain);
    }

    if (pWin->WinType == TYPE_CONTROL)
    {
        __gui_widget_t     *pWidget;
        pWidget = (__gui_widget_t *)pWin;
        return GUI_WinP2H(pWidget->pFrameWin->pManWin->pRootMain);
    }

    return HWND_INVALID;
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetMainWin
*
* author:
*
* date:             2009-7-28
*
* Description:      得到窗口所在的管理窗口句柄
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN GUI_WinGetManWin(H_WIN hWnd)
{
    __gui_win_t  *pWin = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!hWnd)
    {
        ORANGE_WRN("h_win empty!");
        return HWND_INVALID;
    }

#endif

    if (pWin->DataType != TYPE_HWND)
    {
        ORANGE_WRN("windows type error!");
        return HWND_INVALID;
    }

    if (pWin->WinType == TYPE_MANWIN)
    {
        __gui_manwin_t  *pManWin = (__gui_manwin_t *)pWin;

        if (pManWin->ManWinType == TYPE_ROOTMAIN
            || pManWin->ManWinType == TYPE_CHILDMAIN)
        {
            return hWnd;
        }
        else
        {
            ORANGE_WRN("manwin type error!");
            return HWND_INVALID;
        }
    }

    if (pWin->WinType == TYPE_FRAMEWIN)
    {
        __gui_frmwin_t        *pFramewin;
        pFramewin = (__gui_frmwin_t *)pWin;
        return GUI_WinP2H(pFramewin->pManWin);
    }

    if (pWin->WinType == TYPE_CONTROL)
    {
        __gui_widget_t     *pWidget;
        pWidget = (__gui_widget_t *)pWin;
        return GUI_WinP2H(pWidget->pFrameWin->pManWin);
    }

    return HWND_INVALID;
}


/*
**********************************************************************************************************************
*                                               GUI_WinGetLyrWin
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

H_LYR GUI_WinGetLyrWin(H_WIN hWnd)
{
    __gui_win_t     *pWin;
#if GUI_CHECK_ARG_EN > 0

    if (!hWnd)
    {
        ORANGE_WRN("h_win empty!");
        return HWND_INVALID;
    }

#endif

    if (hWnd == HWND_INVALID || hWnd == h_backwin)
    {
        return NULL;
    }

    pWin = GUI_WinH2P(hWnd);

    if (pWin->DataType != TYPE_HWND)
    {
        return NULL;
    }

    if (pWin->WinType == TYPE_MANWIN)
    {
        return NULL;
    }

    return GUI_LyrP2H(pWin->LyrWin);
}
/*
**********************************************************************************************************************
*                                               GUI_WinGetFrmWin
*
* author:
*
* date:             2009-8-7
*
* Description:      gui getframewindow
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN GUI_WinGetFrmWin(H_WIN hWnd)
{
    __gui_win_t     *pWin;
#if GUI_CHECK_ARG_EN > 0

    if (!hWnd)
    {
        ORANGE_WRN("h_win empty!");
        return HWND_INVALID;
    }

#endif

    if (hWnd == HWND_INVALID || hWnd == h_backwin)
    {
        return NULL;
    }

    pWin = GUI_WinH2P(hWnd);

    if (pWin->DataType != TYPE_HWND)
    {
        return NULL;
    }

    if (pWin->WinType == TYPE_MANWIN)
    {
        return NULL;
    }

    if (pWin->WinType == TYPE_FRAMEWIN)
    {
        return GUI_WinP2H(pWin);
    }

    if (pWin->WinType == TYPE_CONTROL)
    {
        return GUI_WinP2H(((__gui_widget_t *)pWin)->pFrameWin);
    }

    return NULL;
}
/*
**********************************************************************************************************************
*                                               GUI_WinGetParent
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
H_WIN GUI_WinGetParent(H_WIN hWnd)
{
    __gui_win_t *p_win = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return 0;
    }

#endif

    if (p_win == NULL)
    {
        __inf("p_win==null");
    }

    if (p_win->Parent == NULL)
    {
        __inf("p_win->Parent==null");
    }

    return (H_WIN)(p_win->Parent);
}
/*
**********************************************************************************************************************
*                                               GUI_WinGetFirstChild
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
H_WIN GUI_WinGetFirstChild(H_WIN hWnd)
{
    __gui_win_t *p_win = GUI_WinH2P(hWnd);
    H_WIN          ret;

    if (HWND_INVALID == hWnd)
    {
        return 0;
    }

#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return 0;
    }

#endif
    GUI_WinLock();
    ret = (H_WIN)(p_win->FirstChild);
    GUI_WinUnlock();
    return ret;
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetWinRECT
*
* author:
*
* date:             2009-8-13
*
* Description:      gui wingetwinrect
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_WinGetWinRECT(H_WIN hWnd, RECT *Win_Rect)
{
    __gui_win_t   *p_win = GUI_WinH2P(hWnd);

    if (HWND_INVALID == hWnd)
    {
        return ORANGE_FAIL;
    }

#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return ORANGE_FAIL;
    }

    if (!Win_Rect)
    {
        ORANGE_WRN("windows rect null!");
        return ORANGE_FAIL;
    }

#endif
    Win_Rect->x         = p_win->win_rect.left;
    Win_Rect->width     = p_win->win_rect.right - p_win->win_rect.left + 1;
    Win_Rect->y         = p_win->win_rect.top;
    Win_Rect->height    = p_win->win_rect.bottom - p_win->win_rect.top + 1;
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetWinRECT
*
* author:
*
* date:             2009-8-13
*
* Description:      gui winsetwinrect
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_WinSetWinRECT(H_WIN hWnd, RECT *Win_Rect)
{
    __gui_win_t   *p_win = GUI_WinH2P(hWnd);

    if (HWND_INVALID == hWnd)
    {
        return ORANGE_FAIL;
    }

#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return ORANGE_FAIL;
    }

    if (!Win_Rect)
    {
        ORANGE_WRN("windows rect null!");
        return ORANGE_FAIL;
    }

#endif
    p_win->win_rect.left    = Win_Rect->x;
    p_win->win_rect.right   = Win_Rect->x + Win_Rect->width - 1;
    p_win->win_rect.top     = Win_Rect->y;
    p_win->win_rect.bottom  = Win_Rect->height + Win_Rect->y - 1;
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetClientRECT
*
* author:
*
* date:             2009-8-13
*
* Description:      获取窗口在父窗口中的
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_WinGetClientRECT(H_WIN hWnd, RECT *Client_Rect)
{
    __gui_win_t   *p_win = GUI_WinH2P(hWnd);

    if (HWND_INVALID == hWnd)
    {
        return ORANGE_FAIL;
    }

#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return ORANGE_FAIL;
    }

    if (!Client_Rect)
    {
        ORANGE_WRN("windows rect null!");
        return ORANGE_FAIL;
    }

#endif
    Client_Rect->x          = p_win->client_rect.left;
    Client_Rect->width      = p_win->client_rect.right - p_win->client_rect.left;
    Client_Rect->y          = p_win->client_rect.top;
    Client_Rect->height     = p_win->client_rect.bottom - p_win->client_rect.top;
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetClientRECT
*
* author:
*
* date:             2009-8-13
*
* Description:      GUI_WinSetClientRECT
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_WinSetClientRECT(H_WIN hWnd, RECT *Client_Rect)
{
    __gui_win_t   *p_win = GUI_WinH2P(hWnd);

    if (HWND_INVALID == hWnd)
    {
        return ORANGE_FAIL;
    }

#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return ORANGE_FAIL;
    }

    if (!Client_Rect)
    {
        ORANGE_WRN("windows rect null!");
        return ORANGE_FAIL;
    }

#endif
    p_win->client_rect.left     = Client_Rect->x;
    p_win->client_rect.right    = Client_Rect->width + Client_Rect->x;
    p_win->client_rect.top      = Client_Rect->y;
    p_win->client_rect.bottom   = Client_Rect->y + Client_Rect->height;
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_GetFocusChild
*
* author:
*
* date:             2009-7-28
*
* Description:      gui getfocuschild
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN GUI_WinGetFocusChild(H_WIN hWnd)
{
    __gui_win_t  *pWin;

    if (hWnd == HWND_INVALID)
    {
        return HWND_INVALID;
    }

    pWin = GUI_WinH2P(hWnd);

    if (pWin)
    {
        return (H_WIN)pWin->ActiveChild;
    }

    return HWND_INVALID;
}
/*
**********************************************************************************************************************
*                                               GUI_SetFocusChild
*
* author:
*
* date:             2009-7-28
*
* Description:      gui setfocuschild
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN GUI_WinSetFocusChild(H_WIN hWnd)
{
    H_WIN    hOldActive;
    __inf("enter GUI_WinSetFocusChild");

    if (hWnd == HWND_INVALID)
    {
        return HWND_INVALID;
    }

    __inf("before GUI_Setfocus_helper");

    if ((hOldActive = GUI_Setfocus_helper(hWnd)) != HWND_INVALID)
    {
        do
        {
            __inf("before GUI_WinGetParent1");

            if (GUI_WinGetParent(hWnd) == h_backwin)
            {
                active_mainwnd = GUI_WinH2P(hWnd);
                break;
            }

            __inf("before GUI_WinGetParent2");
            hWnd = GUI_WinGetParent(hWnd);
        } while ((GUI_Setfocus_helper(hWnd) != HWND_INVALID));
    }

    return hOldActive;
}

/*
**********************************************************************************************************************
*                                               GUI_WinSetActiveChild
*
* author:
*
* date:             2009-11-12
*
* Description:      设置窗口为某一窗口的焦点子窗口(设置完成之后该窗口不一定是系统的焦点子窗口)
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN GUI_WinSetActiveChild(H_WIN hWnd)
{
    return GUI_Setfocus_helper(hWnd);
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetNextBro
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
H_WIN GUI_WinGetNextBro(H_WIN hWnd)
{
    __gui_win_t *p_win = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return 0;
    }

#endif
    return (H_WIN)(p_win->Next);
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetOwnerWin
*
* author:
*
* date:             2009-8-29
*
* Description:      gui wingetownerwin
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN GUI_WinGetOwnerWin(H_WIN  hWnd)
{
    __gui_win_t *p_win = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return 0;
    }

#endif

    if (p_win->WinType == TYPE_MANWIN)
    {
        __gui_manwin_t  *pMain = (__gui_manwin_t *)p_win;
        return GUI_WinP2H(pMain->pHosting);
    }
    else if (p_win->WinType == TYPE_FRAMEWIN)
    {
        __gui_frmwin_t  *pMain = (__gui_frmwin_t *)p_win;
        return GUI_WinP2H(pMain->pHosting);
    }

    return NULL;
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetFirstHostedWin
*
* author:
*
* date:             2009-8-29
*
* Description:      gui wingetfirsthostedwin
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN  GUI_WinGetFirstHostedWin(H_WIN  hWnd)
{
    __gui_win_t *p_win = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return 0;
    }

#endif

    if (p_win->WinType == TYPE_MANWIN)
    {
        __gui_manwin_t  *pMain = (__gui_manwin_t *)p_win;
        return GUI_WinP2H(pMain->pFirstHosted);
    }
    else if (p_win->WinType == TYPE_FRAMEWIN)
    {
        __gui_frmwin_t  *pMain = (__gui_frmwin_t *)p_win;
        return GUI_WinP2H(pMain->pFirstHosted);
    }

    return NULL;
}


/*
**********************************************************************************************************************
*                                               GUI_WinGetNextHostedWin
*
* author:
*
* date:             2009-8-29
*
* Description:      gui wingetnexthostedwin
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN  GUI_WinGetNextHostedWin(H_WIN  hWnd)
{
    __gui_win_t *p_win = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return 0;
    }

#endif

    if (p_win->WinType == TYPE_MANWIN)
    {
        __gui_manwin_t  *pMain = (__gui_manwin_t *)p_win;
        return GUI_WinP2H(pMain->pNextHosted);
    }
    else if (p_win->WinType == TYPE_FRAMEWIN)
    {
        __gui_frmwin_t  *pMain = (__gui_frmwin_t *)p_win;
        return GUI_WinP2H(pMain->pNextHosted);
    }

    return NULL;
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetActiveCtrlWin
*
* author:
*
* date:             2009-7-28
*
* Description:      gui getactivecontrol
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN GUI_WinGetActiveCtrlWin(H_WIN hWnd)
{
    __gui_win_t   *pWin = GUI_WinH2P(hWnd);
    __gui_win_t   *pCtl = GUI_WinH2P(hWnd);

    if (hWnd == HWND_INVALID)
    {
        return HWND_INVALID;
    }

    if (pWin->DataType != TYPE_HWND)
    {
        return HWND_INVALID;
    }

    while ((pCtl->WinType != TYPE_CONTROL)
           || (pCtl != NULL))
    {
        pCtl = pCtl->ActiveChild;
    };

    return GUI_WinP2H(pCtl);
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetType
*
* author:
*
* date:             2009-7-28
*
* Description:      获取窗口类型
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32  GUI_WinGetType(H_WIN hWnd)
{
    __gui_win_t  *pWin;

    if (hWnd == HWND_INVALID)
    {
        return HWND_INVALID;
    }

    pWin = GUI_WinH2P(hWnd);

    if (pWin->DataType != TYPE_HWND)
    {
        return ORANGE_FAIL;
    }

    return pWin->WinType;
}

/*
**********************************************************************************************************************
*                                               GUI_WinIsVisible
*
* author:
*
* date:             2009-9-6
*
* Description:      判断一个窗口是否可见
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool  GUI_WinIsVisible(H_WIN hWnd)
{
    __gui_frmwin_t  *pFrmWin = (__gui_frmwin_t *)GUI_WinH2P(hWnd);
    __gui_widget_t  *pCtrl = (__gui_widget_t *)GUI_WinH2P(hWnd);

    if (hWnd == HWND_INVALID)
    {
        return HWND_INVALID;
    }

    if (pFrmWin->base_win.WinType == TYPE_FRAMEWIN)
    {
        return pFrmWin->base_win.dwStyle & WS_VISIBLE;
    }
    else if (pFrmWin->base_win.WinType == TYPE_CONTROL)
    {
        if (!GUI_WinIsVisible(GUI_WinGetParent(hWnd)))
        {
            return ORANGE_FALSE;
        }

        return pCtrl->base_win.dwStyle & WS_VISIBLE;
    }

    return ORANGE_FALSE;
}
/*
**********************************************************************************************************************
*                                               GUI_WinGetName
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 GUI_WinGetName(H_WIN hWnd, char *name)
{
    __gui_win_t *p_win = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return 0;
    }

#endif
    eLIBs_strncpy(name, p_win->name, GUI_NAME_MAX);
    return ORANGE_OK;
}

H_WIN GUI_WinGetChildFromName(__gui_win_t *pWin, char *name)
{
    __gui_win_t *Child;
    H_WIN        ret_win = HWND_INVALID;

    if (pWin)
    {
        if (orange_strcmp(name, pWin->name) == 0)
        {
            return GUI_WinP2H(pWin);
        }

        Child = pWin->FirstChild;

        while (Child)
        {
            ret_win = GUI_WinGetChildFromName(Child, name);

            if (ret_win != HWND_INVALID)
            {
                return ret_win;
            }

            Child = Child->Next;
        }
    }

    return HWND_INVALID;
}

H_WIN GUI_WinGetHandFromName(char *name)
{
    __gui_win_t     *pWin = GUI_WinH2P(h_backwin);
    H_WIN           hret  = HWND_INVALID;

    if (pWin == HWND_INVALID)
    {
        return HWND_INVALID;
    }

    hret = GUI_WinGetChildFromName(pWin, name);

    if (hret)
    {
        return hret;
    }
    else
    {
        return HWND_INVALID;
    }
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetPrimitiveWin
*
* author:
*
* date:             2010-1-26
*
* Description:      gui wingetprimitivewin
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN GUI_WinGetPrimitiveWin(H_WIN hWnd)
{
    __gui_win_t *p_win = GUI_WinH2P(hWnd);
    __gui_win_t *pManWin;
#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return 0;
    }

#endif
    pManWin = GUI_WinH2P(GUI_WinGetMainManWin(hWnd));

    if (!pManWin)
    {
        ORANGE_WRN("h_win empty!");
        return 0;
    }

    return GUI_WinP2H(pManWin->Primitive);
}
/*
**********************************************************************************************************************
*                                               GUI_WinGetAttr
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
void *GUI_WinGetAttr(H_WIN hWnd)
{
    void *ret;
#if GUI_CHECK_ARG_EN > 0

    if (!hWnd)
    {
        ORANGE_WRN("h_win empty!");
        return 0;
    }

#endif
    GUI_WinLock();
    ret = (void *)(GUI_WinH2P(hWnd))->dwAddData;
    GUI_WinUnlock();
    return ret;
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetAttr
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 GUI_WinSetAttr(H_WIN hWnd, void *attr)
{
#if GUI_CHECK_ARG_EN > 0

    if (!hWnd)
    {
        ORANGE_WRN("h_win empty!");
        return 0;
    }

#endif
    GUI_WinLock();
    (GUI_WinH2P(hWnd))->dwAddData = (unsigned long)attr;
    GUI_WinUnlock();
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_CtrlWinGetAddData
*
* author:
*
* date:             2009-8-6
*
* Description:      gui controlgetadddata
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

unsigned long  GUI_WinGetAddData(H_WIN hWnd)
{
    __gui_win_t         *pWin       = GUI_WinH2P(hWnd);

    if (!hWnd)
    {
        ORANGE_WRN("windows handle error!");
        return 0;
    }

    if (pWin->DataType != TYPE_HWND)
    {
        ORANGE_WRN("windows handle error!");
        return 0;
    }

    return (unsigned long)pWin->dwCtrlData;
}

/*
**********************************************************************************************************************
*                                               GUI_CtrlWinSetAddData
*
* author:
*
* date:             2009-8-6
*
* Description:      gui controlsetadddata
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_WinSetAddData(H_WIN hWnd, unsigned long dwAddData)
{
    __gui_win_t         *pWin       = GUI_WinH2P(hWnd);

    if (!hWnd)
    {
        ORANGE_WRN("windows handle error!");
        return 0;
    }

    if (pWin->DataType != TYPE_HWND)
    {
        ORANGE_WRN("windows handle error!");
        return 0;
    }

    pWin->dwCtrlData  = (unsigned long)dwAddData;
    return ORANGE_OK;
}
/*
**********************************************************************************************************************
*                                               GUI_WinGetClient
*
* author:
*
* date:             2009-8-4
*
* Description:      获取窗口的客户区矩形
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_WinGetClient(H_WIN hWnd, RECT *rect)
{
    __gui_win_t *p_win = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if (!p_win)
    {
        ORANGE_WRN("h_win empty!");
        return ORANGE_FAIL;
    }

    if (!rect)
    {
        ORANGE_WRN("rect parameter error!");
        return ORANGE_FAIL;
    }

#endif
    rect->height     = p_win->client_rect.bottom - p_win->client_rect.top;
    rect->x          = p_win->client_rect.left;
    rect->width      = p_win->client_rect.right - p_win->client_rect.left;
    rect->y          = p_win->client_rect.top;
    return ORANGE_OK;
}


/*
**********************************************************************************************************************
*                                               GUI_WinGetDlgItem
*
* author:
*
* date:             2009-8-4
*
* Description:      根据ID获取窗口句柄
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN GUI_WinGetDlgItem(H_WIN hDlg, __s32 nIDDlgItem)
{
    __gui_widget_t *pCtrl;
    __gui_frmwin_t *pFrameWin = (__gui_frmwin_t *)(GUI_WinH2P(hDlg));

    if (hDlg == NULL)
    {
        return 0;
    }

    pCtrl = (__gui_widget_t *)(pFrameWin->base_win.FirstChild);

    while (pCtrl)
    {
        if (pCtrl->base_win.id == nIDDlgItem)
        {
            return (H_WIN)pCtrl;
        }

        pCtrl = (__gui_widget_t *)pCtrl->base_win.Next;
    }

    return 0;
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetHandFromId
*
* author:
*
* date:             2010-1-26
*
* Description:      gui wingethandfromid
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
H_WIN GUI_WinGetHandFromId(H_WIN hParent, __s32 nID)
{
    __gui_win_t *pChild;
    __gui_win_t *pWin = GUI_WinH2P(hParent);

    if (hParent == NULL)
    {
        return 0;
    }

    pChild = pWin->FirstChild;

    while (pChild)
    {
        if (pChild->id == nID)
        {
            return GUI_WinP2H(pChild);
        }

        pChild = pChild->Next;
    }

    return 0;
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetItemId
*
* author:
*
* date:             2009-8-4
*
* Description:      根据窗口句柄获取ID
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__u32 GUI_WinGetItemId(H_WIN hItem)
{
    __gui_widget_t          *pCtrl;
    pCtrl = (__gui_widget_t *)hItem;
    return pCtrl->base_win.id;
}



/*
**********************************************************************************************************************
*                                               GetMsgQueue
*
* author:
*
* date:             2009-7-22
*
* Description:      getmsgqueue
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__win_msgqueue_t *GUI_WinGetMsgQueue(H_WIN hWnd)
{
    __gui_win_t *pWin;
#if GUI_CHECK_ARG_EN > 0

    if (!hWnd)
    {
        ORANGE_WRN("h_win empty!");
        return NULL;
    }

#endif
    pWin = GUI_WinH2P(hWnd);

    if (hWnd == h_backwin)
    {
        return ((__gui_manwin_t *)pWin)->pMessages;
    }
    else if (pWin->DataType != TYPE_HWND)
    {
        return NULL;
    }

    return ((__gui_manwin_t *)GUI_WinGetManWin(hWnd))->pMessages;
}



/*
**********************************************************************************************************************
*                                               GUI_GetWinThreadId
*
* author:
*
* date:             2009-8-3
*
* Description:      gui getwinthreadid
*
* parameters:
*
* return:           if hWnd is main win, return main win's theadid, if not, get the main win handle, and return it's
*                   thead id;
* modify history:
**********************************************************************************************************************
*/

__u32 GUI_WinGetWinThreadId(H_WIN  hWnd)
{
    __gui_win_t     *pWin = GUI_WinH2P(hWnd);

    if (hWnd == HWND_INVALID)
    {
        return 0;
    }

    if (pWin->DataType != TYPE_HWND)
    {
        return 0;    //steven ??
    }

    if (pWin->WinType != TYPE_MANWIN)
    {
        __gui_manwin_t *pManWin;
        pManWin = (__gui_manwin_t *)GUI_WinGetManWin(hWnd);
        return pManWin->threadid;
    }

    return ((__gui_manwin_t *)pWin)->threadid;
}

/*
**********************************************************************************************************************
*                                               GUI_Win_Init
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
void *GUI_CreateAutoPanel(int id)
{
    //return CreateAutoPanel(id);
    return NULL;
}
int GUI_DestroyAutoPanel(void *hdle)
{
    //return DestroyAutoPanel(hdle);
    return 0;
}
__s32 GUI_WinInit(void)
{
    void GUI_Win_InitLock(void);
    void GUI_LyrInitOS(void);
    //gui_win_initlock will init Win_Sem and cause deadlock between sl_area_search_task and mgs_srv_thread
    //GUI_Win_InitLock();
    /*GUI_LyrInitOS();*/
#ifdef CONFIG_GUI_WIN_MANWIN_EN
    h_backwin = GUI_ManWinCreate(&backwin_para);
    InitFreeQMSGList();
#ifdef CONFIG_GUI_WIDGET_EN
    GUI_InitControlClass();
#ifdef CONFIG_GUI_WIDGET_BUTTON_EN
    RegisterButtonControl();
#endif
#ifdef CONFIG_GUI_WIDGET_STATIC_EN
    RegisterStaticControl();
#endif
#ifdef CONFIG_GUI_WIDGET_LISTMENU_EN
    RegisterListMenuControl();
#endif
#ifdef CONFIG_GUI_WIDGET_PROGSHEET_EN
    RegisterProgSheetControl();
#endif
#ifdef CONFIG_GUI_WIDGET_SLIDER_EN
    RegisterSliderControl();
#endif
#ifdef CONFIG_GUI_WIDGET_TXTBOX_EN
    RegisterTxtboxControl();
#endif
#ifdef CONFIG_GUI_WIDGET_TXTBOX_EN
#endif
#ifdef CONFIG_GUI_WIDGET_TXTBOX_EN
#endif
#ifdef CONFIG_GUI_WIDGET_TXTBOX_EN
#endif
#ifdef CONFIG_GUI_WIDGET_TXTBOX_EN
#endif
#ifdef CONFIG_GUI_WIDGET_TXTBOX_EN
#endif
    //RegisterSpinboxControl ();
#endif
#endif //endmanwin
    gui_timer_init();
    //RegisterTxtboxControl();
    /*
     * Init heap of clipping rects.
     */
    //InitFreeClipRectList (&sg_FreeClipRectList, SIZE_CLIPRECTHEAP);
    /*
     * Init heap of invalid rects.
     */
    //InitFreeClipRectList (&sg_FreeInvRectList, SIZE_INVRECTHEAP);
    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                               GUI_Win_Exit
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 GUI_WinExit(void)
{
    DestroyFreeQMSGList();
#ifdef CONFIG_GUI_WIN_MANWIN_EN
    return GUI_ManWinDelete(h_backwin);
#endif
}


/*
**********************************************************************************************************************
*                                               GUI_SetNotifyCallback
*
* author:
*
* date:             2009-7-29
*
* Description:      gui setnotifycallback
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

NOTIFPROC GUI_WinSetNotifyCallback(H_WIN hwnd, NOTIFPROC notif_proc)
{
    NOTIFPROC   old_proc;
    __gui_win_t *control = (__gui_win_t *)hwnd;

    if (hwnd == HWND_INVALID)
    {
        return HWND_INVALID;
    }

    /*非法窗口结构*/
    if (control->DataType != TYPE_HWND)
    {
        return NULL;
    }

    /*窗口是根窗口*/
    if (control->Parent == NULL)
    {
        return NULL;
    }

    old_proc = (NOTIFPROC)esKRNL_GetCallBack((__pCBK_t)control->NotifProc);
    control->NotifProc = (NOTIFPROC)esKRNL_GetCallBack((__pCBK_t)notif_proc);
    return old_proc;
}

/*
**********************************************************************************************************************
*                                               GUI_SetNotifyCallback
*
* author:
*
* date:             2009-7-29
*
* Description:      gui getnotifycallback
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

NOTIFPROC GUI_WinGetNotifyCallback(H_WIN hwnd)
{
    __gui_win_t *control = GUI_WinH2P(hwnd);

    if (hwnd == HWND_INVALID)
    {
        return HWND_INVALID;
    }

    /*非法窗口结构*/
    if (control->DataType != TYPE_HWND)
    {
        return NULL;
    }

    /*窗口是根窗口*/
    if (control->Parent == NULL)
    {
        return NULL;
    }

    if (control->NotifProc)
    {
        return (NOTIFPROC)esKRNL_GetCallBack((__pCBK_t)control->NotifProc);
    }
    else
    {
        return NULL;
    }
}


/*
**********************************************************************************************************************
*                                               GUI_NotifyParent
*
* author:
*
* date:             2009-7-31
*
* Description:      gui notifyparent
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_NotifyParent(__gui_notify_msg_t *msg)
{
    NOTIFPROC notif_proc = GUI_WinGetNotifyCallback(msg->hWnd);

    if (notif_proc)
    {
        esKRNL_CallBack((__pCBK_t)*notif_proc, msg);
    }
    else
    {
        __gui_msg_t     notifymsg;
        notifymsg.h_srcwin  = msg->hWnd;
        notifymsg.h_deswin  = GUI_WinGetParent(msg->hWnd);
        notifymsg.id        = GUI_MSG_COMMAND;
        notifymsg.dwAddData1 = (__u32)MAKELONG(msg->id, msg->msgcode);
        notifymsg.dwAddData2 = msg->dwAddData;
        notifymsg.dwReserved = msg->dwReserved;
        notifymsg.p_arg     = NULL;
        //GUI_SendNotifyMessage (&notifymsg);
        GUI_SendMessage(&notifymsg);
    }
}

/*************************************************** end of file ****************************************************/
