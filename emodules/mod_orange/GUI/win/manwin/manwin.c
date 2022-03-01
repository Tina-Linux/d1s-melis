/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：        manwin.c
*
*Author：           William Wang
*
*Version :          1.1.0
*
*Date:              2009-8-1
*
*Description :      manwin function implement
*
*Others :           None at present.
*
* History :
*
* <Author>          <time>        <version>     <description>
*
* William Wang     2009-8-1         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __MANWIN_C__
#define __MANWIN_C__

#include "manwin_i.h"

/*
**********************************************************************************************************************
*                                               GUI_AssertPara
*
* author:
*
* date:             2009-11-4
*
* Description:      gui assertpara
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __bool GUI_ManWinAssertPara(__gui_manwincreate_para_t *create_info)
{
    if (!create_info)
    {
        ORANGE_WRN("input create info NULL or layer handle NULL!");
        return ORANGE_FALSE;
    }

    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               GUI_GetActiveMainWin
*
* author:
*
* date:             2009-8-1
*
* Description:      gui getactivemanwin
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN   GUI_GetActiveManWin(void)
{
    return GUI_WinP2H(active_mainwnd);
}

/*
**********************************************************************************************************************
*                                               GUI_SetActiveMainWin
*
* author:
*
* date:             2009-8-1
*
* Description:      gui setactivemanwin
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN   GUI_SetActiveManWin(H_WIN actwin)
{
    __gui_win_t   *pWin = (__gui_win_t *)actwin;
    H_WIN   oldactive = (H_WIN)active_mainwnd;
    __wrn("active main win from %s to %s.", active_mainwnd ? active_mainwnd->name : "no active", pWin->name);
#if GUI_CHECK_ARG_EN > 0

    if (!pWin)
    {
        ORANGE_WRN("active manwin empty!");
        active_mainwnd = (__gui_win_t *)actwin;
        return oldactive;
    }

#endif
    __wrn("before GUI_WinLock");
    GUI_WinLock();

    if (pWin->DataType != TYPE_HWND
        || (pWin->WinType != TYPE_MANWIN))
    {
        GUI_WinUnlock();
        return oldactive;
    }

    active_mainwnd = (__gui_win_t *)actwin;
    GUI_WinUnlock();
    __wrn("before GUI_WinSetFocusChild");
    GUI_WinSetFocusChild(GUI_WinP2H(active_mainwnd));
    return oldactive;
}

/*
**********************************************************************************************************************
*                                               GUI_GetFrameWinUnderPoint
*
* author:
*
* date:             2009-8-12
*
* Description:      获取图层上的Frmwinwin
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__gui_win_t *GUI_GetLayerFrameWinUnderPoint(__gui_lyrwin_t *pLayer, __s32 x, __s32 y)
{
    __gui_zordernode_t      *pNode;
    __gui_win_t             *pTemp;
    __gui_rect_t            screct;
    GUI_POINT               pPhyPt;
    GUI_POINT               pLogPt;
    pPhyPt.x    = x;
    pPhyPt.y    = y;
    GUI_ScnPhyPt2LogPt(gui_scndir, &pPhyPt, &pLogPt);
    x           = pLogPt.x;
    y           = pLogPt.y;

    if (pLayer == NULL)
    {
        ORANGE_WRN("not layer under point!");
        return NULL;
    }

    pNode = pLayer->TopMostWinZOrder.pTopMost;

    while (pNode)
    {
        pTemp = GUI_WinH2P(pNode->hWnd);
        GUI_FrameWinToScreen(pNode->hWnd, &screct);

        if (PtInRect(&screct, x, y)
            && (pTemp->dwStyle & WS_VISIBLE))
        {
            __wrn("found win name=%s, pTemp->dwStyle=0x%x", pTemp->name, pTemp->dwStyle);

            if (pTemp->dwStyle & WS_DISABLED)
            {
                return NULL;
            }

            return pTemp;
        }

        pNode = pNode->pNext;
    }

    pNode = pLayer->ManWinZOrder.pTopMost;

    while (pNode)
    {
        pTemp = GUI_WinH2P(pNode->hWnd);
        GUI_FrameWinToScreen(pNode->hWnd, &screct);

        if (PtInRect(&screct, x, y)
            && (pTemp->dwStyle & WS_VISIBLE))
        {
            __wrn("found win name=%s, pTemp->dwStyle=0x%x", pTemp->name, pTemp->dwStyle);

            if (pTemp->dwStyle & WS_DISABLED)
            {
                return NULL;
            }

            return pTemp;
        }

        pNode = pNode->pNext;
    }

    return NULL;
}

/*
**********************************************************************************************************************
*                                               GUI_GetFrameWinUnderPointEx
*
* author:
*
* date:             2010-7-27
*
* Description:      获取在某点之上的窗口句柄。如果上面的图层无窗口,则一直找到该点下的窗口为止
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__gui_win_t *GUI_GetFrameWinUnderPointEx(__s32 x, __s32 y)
{
    __gui_rect_t            lyr_rect;
    __gui_win_t             *pTemp  = NULL;
    __gui_lyrwin_t          *pCur   = NULL;
    __s32                   scn_x;
    __s32                   scn_y;
    GUI_POINT               pPhyPt;
    GUI_POINT               pLogPt;
    scn_x       = x;
    scn_y       = y;
    pPhyPt.x    = x;
    pPhyPt.y    = y;
    GUI_ScnPhyPt2LogPt(gui_scndir, &pPhyPt, &pLogPt);
    x           = pLogPt.x;
    y           = pLogPt.y;
    /*先在Sprite中获取Sprite的句柄*/
    pCur = sg_VisiSpriteList;

    while (pCur)
    {
        lyr_rect.left   = pCur->lyrpara.scn_win.x;
        lyr_rect.right  = pCur->lyrpara.scn_win.x + pCur->lyrpara.scn_win.width;
        lyr_rect.top    = pCur->lyrpara.scn_win.y;
        lyr_rect.bottom = pCur->lyrpara.scn_win.y + pCur->lyrpara.scn_win.height;

        if (PtInRect((__gui_rect_t *)(&lyr_rect), x, y))
        {
            pTemp = GUI_GetLayerFrameWinUnderPoint(pCur, scn_x, scn_y);

            if (pTemp != NULL)
            {
                return pTemp;
            }
        }

        pCur = pCur->Next;
    }

    /*再在图层链表中获取图层的句柄*/
    pCur = sg_VisiLayerList;

    while (pCur)
    {
        lyr_rect.left   = pCur->lyrpara.scn_win.x;
        lyr_rect.right  = pCur->lyrpara.scn_win.x + pCur->lyrpara.scn_win.width;
        lyr_rect.top    = pCur->lyrpara.scn_win.y;
        lyr_rect.bottom = pCur->lyrpara.scn_win.y + pCur->lyrpara.scn_win.height;

        if (PtInRect((__gui_rect_t *)(&lyr_rect), x, y))
        {
            pTemp = GUI_GetLayerFrameWinUnderPoint(pCur, scn_x, scn_y);

            if (pTemp != NULL)
            {
                return pTemp;
            }
        }

        pCur = pCur->Next;
    }

    if (pTemp == NULL)
    {
        return NULL;
    }

    return NULL;
}

/*
**********************************************************************************************************************
*                                               GUI_GetLayerUnderPoint
*
* author:
*
* date:             2009-8-12
*
* Description:      gui getlayerunderpoint
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__gui_lyrwin_t *GUI_GetLayerUnderPoint(__s32 x, __s32 y)
{
    __gui_rect_t     lyr_rect;
    //__gui_lyrwin_t    *pLayer = NULL;
    __gui_lyrwin_t    *pCur   = NULL;
    /*先在Sprite中获取Sprite的句柄*/
    pCur = sg_VisiSpriteList;

    while (pCur)
    {
        lyr_rect.left   = pCur->lyrpara.scn_win.x;
        lyr_rect.right  = pCur->lyrpara.scn_win.x + pCur->lyrpara.scn_win.width;
        lyr_rect.top    = pCur->lyrpara.scn_win.y;
        lyr_rect.bottom = pCur->lyrpara.scn_win.y + pCur->lyrpara.scn_win.height;

        if (PtInRect((__gui_rect_t *)(&lyr_rect), x, y))
        {
            return pCur;
        }

        pCur = pCur->Next;
    }

    /*再在图层链表中获取图层的句柄*/
    pCur = sg_VisiLayerList;

    while (pCur)
    {
        lyr_rect.left   = pCur->lyrpara.scn_win.x;
        lyr_rect.right  = pCur->lyrpara.scn_win.x + pCur->lyrpara.scn_win.width;
        lyr_rect.top    = pCur->lyrpara.scn_win.y;
        lyr_rect.bottom = pCur->lyrpara.scn_win.y + pCur->lyrpara.scn_win.height;

        if (PtInRect((__gui_rect_t *)(&lyr_rect), x, y))
        {
            return pCur;
        }

        pCur = pCur->Next;
    }

    if (pCur == NULL)
    {
        return NULL;
    }

    return NULL;
}

/*
**********************************************************************************************************************
*                                               GUI_GetFrameWinUnderPoint
*
* author:
*
* date:             2009-8-12
*
* Description:      获取在某点之上的窗口句柄。如果上面的图层无窗口,则返回空
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__gui_win_t *GUI_GetFrameWinUnderPoint(__s32 x, __s32 y)
{
    __gui_lyrwin_t          *pLayer;
    __gui_zordernode_t      *pNode;
    __gui_win_t             *pTemp;
    __gui_rect_t            screct;
    GUI_POINT               pPhyPt;
    GUI_POINT               pLogPt;
    pPhyPt.x    = x;
    pPhyPt.y    = y;
    GUI_ScnPhyPt2LogPt(gui_scndir, &pPhyPt, &pLogPt);
    x           = pLogPt.x;
    y           = pLogPt.y;
    pLayer = GUI_GetLayerUnderPoint(x, y);

    if (pLayer == NULL)
    {
        ORANGE_WRN("not layer under point!");
        return NULL;
    }

    pNode = pLayer->TopMostWinZOrder.pTopMost;

    while (pNode)
    {
        pTemp = GUI_WinH2P(pNode->hWnd);
        GUI_FrameWinToScreen(pNode->hWnd, &screct);

        if (PtInRect(&screct, x, y)
            && (pTemp->dwStyle & WS_VISIBLE))
        {
            return pTemp;
        }

        pNode = pNode->pNext;
    }

    pNode = pLayer->ManWinZOrder.pTopMost;

    while (pNode)
    {
        pTemp = GUI_WinH2P(pNode->hWnd);
        GUI_FrameWinToScreen(pNode->hWnd, &screct);

        if (PtInRect(&screct, x, y)
            && (pTemp->dwStyle & WS_VISIBLE))
        {
            return pTemp;
        }

        pNode = pNode->pNext;
    }

    return NULL;
}

/*
**********************************************************************************************************************
*                                               GUI_GetControlUnderPoint
*
* author:
*
* date:             2009-8-6
*
* Description:      gui getcontrolunderpoint
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__gui_win_t *GUI_GetWinUnderPoint(__gui_win_t *pWin, __s32 x, __s32 y, __pos_t *winpos)
{
    __gui_frmwin_t      *pFrameWin;
    __gui_win_t         *pControl;
    H_WIN               hMainManWin;
    __gui_win_t         *pMainManWin;
    hMainManWin = GUI_WinGetMainManWin(GUI_WinP2H(pWin));
    pMainManWin = GUI_WinH2P(hMainManWin);

    /*如果是主窗口，需要找到对应的framewin,此时坐标为屏幕坐标*/
    if (pWin->WinType == TYPE_MANWIN)
    {
        __pos_t                 sc_pos;
        __pos_t                 win_pos;
        __bool                  isChild;
        GUI_POINT               pPhyPt;
        GUI_POINT               pLogPt;
        pFrameWin = (__gui_frmwin_t *)GUI_GetFrameWinUnderPointEx(x, y);

        if (pFrameWin == NULL)
        {
            ORANGE_WRN("not framewin under point!");
            return NULL;
        }

        /*判断当前找到的frmwin是否在消息循环窗口的子孙窗口中*/
        isChild = GUI_WinIsAncestor(GUI_WinP2H(pMainManWin->Primitive), GUI_WinP2H(pFrameWin));

        if (!isChild && (pMainManWin->Primitive != (__gui_win_t *)pFrameWin))
        {
            return NULL;
        }

        pPhyPt.x    = x;
        pPhyPt.y    = y;
        GUI_ScnPhyPt2LogPt(gui_scndir, &pPhyPt, &pLogPt);
        x           = pLogPt.x;
        y           = pLogPt.y;
        sc_pos.x    = x;
        sc_pos.y    = y;
        GUI_ScreenToWindow((H_WIN)pFrameWin, &sc_pos, &win_pos);
        winpos->x   = win_pos.x;
        winpos->y   = win_pos.y;
        return (__gui_win_t *)pFrameWin;
    }
    else/*如果是framewin或者控件窗口,需要找到对应的子控件,此时坐标为framewin内的坐标*/
    {
        __pos_t     sc_pos;
        __pos_t     win_pos;

        if (pWin->WinType == TYPE_FRAMEWIN && pMainManWin->Primitive == pWin)
        {
            /*如果是framewin且消息循环在此窗口之上,需要找到对应的子控件,此时坐标为屏幕内的坐标*/
            {
                GUI_POINT       pPhyPt;
                GUI_POINT       pLogPt;
                pPhyPt.x    = x;
                pPhyPt.y    = y;
                GUI_ScnPhyPt2LogPt(gui_scndir, &pPhyPt, &pLogPt);
                x           = pLogPt.x;
                y           = pLogPt.y;
            }
            sc_pos.x = x;
            sc_pos.y = y;
            GUI_ScreenToWindow(GUI_WinP2H(pWin), &sc_pos, &win_pos);
            x   = win_pos.x;
            y   = win_pos.y;
        }

        pControl = (__gui_win_t *)pWin->FirstChild;

        while (pControl)
        {
            if ((pControl->dwStyle & WS_VISIBLE)
                && PtInRect((__gui_rect_t *)(&pControl->win_rect), x, y))
            {
                if (pControl->dwStyle & WS_DISABLED)
                {
                    return NULL;
                }

                winpos->x = x - pControl->win_rect.left;
                winpos->y = y - pControl->win_rect.top;
                return pControl;
            }

            pControl = pControl->Next;
        }
    }

    return NULL;
}

/*
**********************************************************************************************************************
*                                               GUI_GetChildWinUnderPoint
*
* author:
*
* date:             2009-10-21
*
* Description:      gui getchildwinunderpoint
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN GUI_GetChildWinUnderPoint(H_WIN hWnd, __s32 x, __s32 y)
{
    __gui_frmwin_t    *pFrameWin;
    __gui_win_t       *pControl;
    __gui_win_t       *pWin     = GUI_WinH2P(hWnd);
    __bool             ischild  = ORANGE_FALSE;
    GUI_POINT          pPhyPt;
    GUI_POINT          pLogPt;
#if GUI_CHECK_ARG_EN > 0

    if (!pWin)
    {
        ORANGE_WRN("win handle empty!");
        return NULL;
    }

#endif

    /*capture 窗口直接判断*/
    if (capture_wnd)
    {
        ischild = GUI_WinIsAncestor(hWnd, capture_wnd);

        if (ischild)
        {
            return capture_wnd;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        /*如果是主窗口，需要找到对应的framewin,此时坐标为屏幕坐标*/
        if (pWin->WinType == TYPE_MANWIN)
        {
            pPhyPt.x    = x;
            pPhyPt.y    = y;
            GUI_ScnPhyPt2LogPt(gui_scndir, &pPhyPt, &pLogPt);
            x           = pLogPt.x;
            y           = pLogPt.y;
            pFrameWin = (__gui_frmwin_t *)GUI_GetFrameWinUnderPointEx(x, y);

            if (pFrameWin == NULL)
            {
                ORANGE_WRN("not framewin under point!");
                return NULL;
            }

            ischild = GUI_WinIsAncestor(hWnd, GUI_WinP2H(pFrameWin));

            if (ischild)
            {
                return GUI_WinP2H(pFrameWin);
            }
            else
            {
                return NULL;
            }
        }
        else/*如果是framewin或者控件窗口,需要找到对应的子控件,此时坐标为framewin内的坐标*/
        {
            pControl = (__gui_win_t *)pWin->FirstChild;

            while (pControl)
            {
                if ((pControl->dwStyle & WS_VISIBLE)
                    && PtInRect((__gui_rect_t *)(&pControl->win_rect), x, y))
                {
                    return GUI_WinP2H(pControl);
                }

                pControl = pControl->Next;
            }
        }

        return NULL;
    }
}

/*
**********************************************************************************************************************
*                                               GUI_DefPostMsgHandler
*
* author:
*
* date:             2009-8-1
*
* Description:      gui defpostmsghandler
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static int GUI_DefOtherMsgHandler(__gui_msg_t *msg)
{
    __gui_win_t     *pWin = GUI_WinH2P(msg->h_deswin);

    switch (msg->id)
    {
        case GUI_MSG_SET_FOCUS:
        case GUI_MSG_SET_UNFOCUS:
            if (pWin->ActiveChild)
            {
                __gui_msg_t     newmsg;
                newmsg.id           = msg->id;
                newmsg.h_srcwin     = (H_WIN)pWin;
                newmsg.h_deswin     = (H_WIN)pWin->ActiveChild;
                newmsg.dwAddData1   = 0;
                newmsg.dwAddData2   = 0;
                //GUI_SendNotifyMessage (&newmsg);
                GUI_SendMessage(&newmsg);
            }

            break;

        case GUI_MSG_ENABLE:
            if ((!(pWin->dwStyle & WS_DISABLED) && !msg->dwAddData1)
                || ((pWin->dwStyle & WS_DISABLED) && msg->dwAddData1))
            {
                if (msg->dwAddData1)
                {
                    pWin->dwStyle &= ~WS_DISABLED;
                }
                else
                {
                    pWin->dwStyle |=  WS_DISABLED;
                }
            }

            break;

        /*wakeup a windows，当图层由suspend状态转换成其他状态时有效*/
        case GUI_MSG_WIN_WAKEUP:
        {
            __gui_win_t         *pControl;

            /*如果是主窗口,不需要处理wakeup消息*/
            if (pWin->WinType == TYPE_MANWIN)
            {
                break;
            }
            else/*如果是framewin或者控件窗口,需要处理绘制消息*/
            {
                {
                    __gui_msg_t     newmsg;
                    newmsg.p_arg = NULL;
                    newmsg.id           = GUI_MSG_PAINT;
                    newmsg.h_srcwin     = GUI_WinP2H(pWin);
                    newmsg.h_deswin     = GUI_WinP2H(pWin);
                    newmsg.dwAddData1   = 0;
                    newmsg.dwAddData2   = 0;
                    //GUI_SendNotifyMessage (&newmsg);
                    GUI_SendMessage(&newmsg);
                }
                pControl = (__gui_win_t *)pWin->FirstChild;

                while (pControl)
                {
                    if (pControl->dwStyle & WS_VISIBLE)
                    {
                        __gui_msg_t     newmsg;
                        newmsg.id           = GUI_MSG_WIN_WAKEUP;
                        newmsg.h_srcwin     = GUI_WinP2H(pWin);
                        newmsg.h_deswin     = GUI_WinP2H(pControl);
                        newmsg.dwAddData1   = 0;
                        newmsg.dwAddData2   = 0;
                        //GUI_SendNotifyMessage (&newmsg);
                        GUI_SendMessage(&newmsg);
                    }

                    pControl = pControl->Next;
                }
            }
        }
        break;

        case GUI_MSG_NCPAINT:
            break;

        case GUI_MSG_ERASEBKGND:
            break;

        case GUI_MSG_WIN_SLEEP:
        {
            __gui_win_t         *pControl;

            /*如果是主窗口,不需要处理wakeup消息*/
            if (pWin->WinType == TYPE_MANWIN)
            {
                break;
            }
            else/*如果是framewin或者控件窗口,需要处理绘制消息*/
            {
                pControl = (__gui_win_t *)pWin->FirstChild;

                while (pControl)
                {
                    if (pControl->dwStyle & WS_VISIBLE)
                    {
                        __gui_msg_t     newmsg;
                        newmsg.id           = GUI_MSG_WIN_SLEEP;
                        newmsg.h_srcwin     = GUI_WinP2H(pWin);
                        newmsg.h_deswin     = GUI_WinP2H(pControl);
                        newmsg.dwAddData1   = 0;
                        newmsg.dwAddData2   = 0;
                        //GUI_SendNotifyMessage (&newmsg);
                        GUI_SendMessage(&newmsg);
                    }

                    pControl = pControl->Next;
                }
            }
        }
        break;

        /*子控件通知消息的处理函数*/
        case GUI_MSG_COMMAND:
        {
            __gui_msg_t     notifymsg;
            /*直接将子控件的通知码往控件的父窗口传递*/
            notifymsg.h_srcwin  = msg->h_deswin;
            notifymsg.h_deswin  = GUI_WinGetParent(msg->h_deswin);
            notifymsg.id        = GUI_MSG_COMMAND;
            notifymsg.dwAddData1 = msg->dwAddData1;
            notifymsg.dwAddData2 = msg->dwAddData2;
            notifymsg.dwReserved = msg->dwReserved;
            notifymsg.p_arg     = NULL;
            //GUI_SendNotifyMessage (&notifymsg);
            GUI_SendMessage(&notifymsg);
        }
        break;

        case GUI_MSG_TOUCHACTIVE:
            if (pWin->WinType == TYPE_CONTROL
                && !(pWin->dwStyle & WS_DISABLED))
            {
                __gui_widget_t *pCtrl = (__gui_widget_t *)pWin;
                __gui_widget_t *old_active = (__gui_widget_t *)pCtrl->base_win.Parent->ActiveChild;

                if (old_active != (__gui_widget_t *)pWin)
                {
                    if (old_active)
                    {
                        __gui_msg_t  new_msg;
                        new_msg.h_srcwin    = 0;
                        new_msg.h_deswin    = GUI_WinP2H(old_active);
                        new_msg.dwAddData1  = (unsigned long)msg->h_deswin;
                        new_msg.dwAddData2  = 0;
                        new_msg.id          = GUI_MSG_SET_UNFOCUS;
                        new_msg.p_arg       = (void *)NULL;
                        //GUI_SendNotifyMessage (&new_msg);
                        GUI_SendMessage(&new_msg);
                    }

                    pCtrl->base_win.Parent->ActiveChild = pWin;
                    {
                        __gui_msg_t  new_msg;
                        new_msg.h_srcwin    = 0;
                        new_msg.h_deswin    = GUI_WinP2H(msg->h_deswin);
                        new_msg.dwAddData1  = (unsigned long)GUI_WinP2H(old_active);
                        new_msg.dwAddData2  = 0;
                        new_msg.id          = GUI_MSG_SET_FOCUS;
                        new_msg.p_arg       = (void *)NULL;
                        //GUI_SendNotifyMessage (&new_msg);
                        GUI_SendMessage(&new_msg);
                    }
                }
            }

            break;

        case GUI_MSG_TOUCHINACTIVE:
            if (pWin->WinType == TYPE_CONTROL
                && !(pWin->dwStyle & WS_DISABLED))
            {
                __gui_msg_t     new_msg;
                new_msg.h_srcwin    = 0;
                new_msg.h_deswin    = GUI_WinP2H(msg->h_deswin);
                new_msg.dwAddData1  = (unsigned long)(msg->h_deswin);
                new_msg.dwAddData2  = 0;
                new_msg.id          = GUI_MSG_SET_UNFOCUS;
                new_msg.p_arg       = (void *)NULL;
                //GUI_SendNotifyMessage (&new_msg);
                GUI_SendMessage(&new_msg);
            }

            break;

        case GUI_MSG_NOTIFY_CHILD_DELETED:
        {
            if (GUI_WinP2H(pWin->ActiveChild) == msg->h_srcwin)
            {
                __gui_win_t     *pChild = GUI_WinH2P(msg->h_srcwin);

                if (pChild->Next)
                {
                    pWin->ActiveChild       = pChild->Next;
                }
                else if (pChild->Prev)
                {
                    pWin->ActiveChild       = pChild->Prev;
                }
                else
                {
                    pWin->ActiveChild = NULL;
                }
            }
        }
        break;

        default:
            break;
    }

    return 0;
}

/*
**********************************************************************************************************************
*                                               GUI_DefPaintMsgHandler
*
* author:
*
* date:             2009-8-1
*
* Description:      gui defpaintmsghandler
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static int GUI_DefPaintMsgHandler(__gui_msg_t *msg)
{
    return 0;
}

/*
**********************************************************************************************************************
*                                               GUI_DefTouchMsgHandler
*
* author:
*
* date:             2009-8-6
*
* Description:      gui deftouchmsghandler
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static int GUI_DefTouchMsgHandler(__gui_msg_t *msg)
{
    __gui_win_t         *pUnderPointer;
    __gui_win_t         *pWin;
    __s32               x;
    __s32               y;
    __pos_t             scpos;
    __pos_t             winpos;
    __u32               dwAddData;
    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);
    pWin = GUI_WinH2P(msg->h_deswin);

    if (capture_wnd)
    {
        GUI_POINT       pPhyPt;
        GUI_POINT       pLogPt;
        pPhyPt.x    = x;
        pPhyPt.y    = y;
        GUI_ScnPhyPt2LogPt(gui_scndir, &pPhyPt, &pLogPt);
        x           = pLogPt.x;
        y           = pLogPt.y;
        scpos.x         =  x;
        scpos.y         =  y;
        GUI_ScreenToWindow(capture_wnd, &scpos, &winpos);
        dwAddData = MAKELONG(winpos.x, winpos.y);
        pUnderPointer = NULL;
    }
    else
    {
        pUnderPointer = GUI_GetWinUnderPoint(pWin, x, y, &winpos);
        dwAddData = MAKELONG(winpos.x, winpos.y);
    }

    {
        char name[256] = {0};
        GUI_WinGetName(msg->h_deswin, name);
        __wrn(" msg.childid =%x, msg.destwin =%x name=%s", msg->dwAddData1, msg->h_deswin, name);
    }

    switch (msg->dwAddData1)
    {
        case GUI_MSG_TOUCH_OVERDOWN:
        case GUI_MSG_TOUCH_OVERMOVE:
        case GUI_MSG_TOUCH_OVERUP:
        case GUI_MSG_TOUCH_OVERZOOMIN:
        case GUI_MSG_TOUCH_OVERZOOMOUT:
        case GUI_MSG_TOUCH_OVERLONGDOWN:
            if (pWin->ActiveChild)
            {
                __gui_msg_t   new_msg;
                new_msg.id              = msg->id;
                new_msg.dwAddData1      = msg->dwAddData1;
                new_msg.h_deswin        = GUI_WinP2H(pWin->ActiveChild);
                new_msg.h_srcwin        = msg->h_deswin;
                new_msg.dwAddData2      = msg->dwAddData2;
                new_msg.dwReserved      = msg->dwReserved;
                new_msg.p_arg           = NULL;
                GUI_SendMessage(&new_msg);
            }

            break;

        case GUI_MSG_TOUCH_MOVE:
            if (capture_wnd)
            {
                __gui_msg_t     new_msg;
                new_msg.id          = GUI_MSG_TOUCH;
                new_msg.h_deswin    = (H_WIN)capture_wnd;
                new_msg.h_srcwin    = (H_WIN)msg->h_deswin;
                new_msg.dwAddData1  = GUI_MSG_TOUCH_MOVE;
                new_msg.dwAddData2  = (__u32)dwAddData;
                new_msg.dwReserved  = msg->dwReserved;
                new_msg.p_arg       = NULL;
                GUI_SendMessage(&new_msg);
            }
            else
            {
                if (pWin->OldUnderPointer != pUnderPointer)
                {
                    if (pUnderPointer)
                    {
                        __gui_msg_t     new_msg;
                        new_msg.id          = GUI_MSG_TOUCH;
                        new_msg.h_deswin    = GUI_WinP2H(pUnderPointer);
                        new_msg.h_srcwin    = (H_WIN)msg->h_deswin;
                        new_msg.dwAddData1  = GUI_MSG_TOUCH_MOVE;
                        new_msg.dwAddData2  = (__u32)dwAddData;
                        new_msg.dwReserved  = msg->dwReserved;
                        new_msg.p_arg       = NULL;
                        GUI_SendMessage(&new_msg);
                    }

                    pWin->OldUnderPointer = pUnderPointer;
                }

                if (pUnderPointer == NULL)
                {
                    pWin->OldUnderPointer = 0;
                    break;
                }
            }

            break;

        case GUI_MSG_TOUCH_ZOOMIN:
            if (capture_wnd)
            {
                __gui_msg_t     new_msg;
                new_msg.id          = GUI_MSG_TOUCH;
                new_msg.h_deswin    = (H_WIN)capture_wnd;
                new_msg.h_srcwin    = (H_WIN)msg->h_deswin;
                new_msg.dwAddData1  = GUI_MSG_TOUCH_ZOOMIN;
                new_msg.dwAddData2  = (__u32)dwAddData;
                new_msg.dwReserved  = msg->dwReserved;
                new_msg.p_arg       = NULL;
                __msg(" new_msg.childid =%x, new_msg.destwin =%x ", new_msg.dwAddData1, new_msg.h_deswin);
                GUI_SendMessage(&new_msg);
            }
            else
            {
                __msg(" oldpointer =%x, pointer =%x ", pWin->OldUnderPointer, pUnderPointer);

                if (pWin->OldUnderPointer != pUnderPointer)
                {
                    if (pUnderPointer)
                    {
                        __gui_msg_t     new_msg;
                        new_msg.id          = GUI_MSG_TOUCH;
                        new_msg.h_deswin    = GUI_WinP2H(pUnderPointer);
                        new_msg.h_srcwin    = (H_WIN)msg->h_deswin;
                        new_msg.dwAddData1  = GUI_MSG_TOUCH_ZOOMIN;
                        new_msg.dwAddData2  = (__u32)dwAddData;
                        new_msg.dwReserved  = msg->dwReserved;
                        new_msg.p_arg       = NULL;
                        __msg(" new_msg.childid =%x, new_msg.destwin =%x ", new_msg.dwAddData1, new_msg.h_deswin);
                        GUI_SendMessage(&new_msg);
                    }

                    pWin->OldUnderPointer = pUnderPointer;
                }

                if (pUnderPointer == NULL)
                {
                    pWin->OldUnderPointer = 0;
                    break;
                }
            }

            break;

        case GUI_MSG_TOUCH_ZOOMOUT:
            if (capture_wnd)
            {
                __gui_msg_t     new_msg;
                new_msg.id          = GUI_MSG_TOUCH;
                new_msg.h_deswin    = (H_WIN)capture_wnd;
                new_msg.h_srcwin    = (H_WIN)msg->h_deswin;
                new_msg.dwAddData1  = GUI_MSG_TOUCH_ZOOMOUT;
                new_msg.dwAddData2  = (__u32)dwAddData;
                new_msg.dwReserved  = msg->dwReserved;
                new_msg.p_arg       = NULL;
                __msg(" new_msg.childid =%x, new_msg.destwin =%x ", new_msg.dwAddData1, new_msg.h_deswin);
                GUI_SendMessage(&new_msg);
            }
            else
            {
                __msg(" oldpointer =%x, pointer =%x ", pWin->OldUnderPointer, pUnderPointer);

                if (pWin->OldUnderPointer != pUnderPointer)
                {
                    if (pUnderPointer)
                    {
                        __gui_msg_t     new_msg;
                        new_msg.id          = GUI_MSG_TOUCH;
                        new_msg.h_deswin    = GUI_WinP2H(pUnderPointer);
                        new_msg.h_srcwin    = (H_WIN)msg->h_deswin;
                        new_msg.dwAddData1  = GUI_MSG_TOUCH_ZOOMOUT;
                        new_msg.dwAddData2  = (__u32)dwAddData;
                        new_msg.dwReserved  = msg->dwReserved;
                        new_msg.p_arg       = NULL;
                        __msg(" new_msg.childid =%x, new_msg.destwin =%x ", new_msg.dwAddData1, new_msg.h_deswin);
                        GUI_SendMessage(&new_msg);
                    }

                    pWin->OldUnderPointer = pUnderPointer;
                }

                if (pUnderPointer == NULL)
                {
                    pWin->OldUnderPointer = 0;
                    break;
                }
            }

            break;

        case GUI_MSG_TOUCH_DOWN:
            if (capture_wnd)
            {
                __gui_msg_t  new_msg;
                new_msg.id          = GUI_MSG_TOUCHACTIVE;
                new_msg.h_deswin    = capture_wnd;
                new_msg.h_srcwin    = 0;
                new_msg.dwAddData1  = 0;
                new_msg.dwAddData2  = 0;
                new_msg.p_arg       = 0;
                GUI_SendMessage(&new_msg);
                new_msg.id          = GUI_MSG_TOUCH;
                new_msg.h_deswin    = capture_wnd;
                new_msg.h_srcwin    = (H_WIN)msg->h_deswin;
                new_msg.dwAddData1  = GUI_MSG_TOUCH_DOWN;
                new_msg.dwAddData2  = (__u32)dwAddData;
                new_msg.dwReserved  = msg->dwReserved;
                new_msg.p_arg       = NULL;
                GUI_SendMessage(&new_msg);
            }
            else if (pUnderPointer)
            {
                __gui_msg_t  new_msg;

                if (pUnderPointer->dwStyle & WS_DISABLED)
                {
                    break;
                }

                new_msg.id          = GUI_MSG_TOUCHACTIVE;
                new_msg.h_deswin    = GUI_WinP2H(pUnderPointer);
                new_msg.h_srcwin    = 0;
                new_msg.dwAddData1  = 0;
                new_msg.dwAddData2  = 0;
                new_msg.p_arg       = 0;
                GUI_SendMessage(&new_msg);
                new_msg.id          = GUI_MSG_TOUCH;
                new_msg.h_deswin    = GUI_WinP2H(pUnderPointer);
                new_msg.h_srcwin    = (H_WIN)msg->h_deswin;
                new_msg.dwAddData1  = GUI_MSG_TOUCH_DOWN;
                new_msg.dwAddData2  = (__u32)dwAddData;
                new_msg.dwReserved  = msg->dwReserved;
                new_msg.p_arg       = NULL;
                GUI_SendMessage(&new_msg);
            }

            break;

        case GUI_MSG_TOUCH_LONGDOWN:
            if (capture_wnd)
            {
                __gui_msg_t     new_msg;
                new_msg.id          = GUI_MSG_TOUCH;
                new_msg.h_deswin    = capture_wnd;
                new_msg.h_srcwin    = (H_WIN)msg->h_deswin;
                new_msg.dwAddData1  = GUI_MSG_TOUCH_LONGDOWN;
                new_msg.dwAddData2  = (__u32)dwAddData;
                new_msg.dwReserved  = msg->dwReserved;
                new_msg.p_arg       = NULL;
                GUI_SendMessage(&new_msg);
            }
            else if (pUnderPointer)
            {
                __gui_msg_t     new_msg;

                if (pUnderPointer->dwStyle & WS_DISABLED)
                {
                    break;
                }

                new_msg.id          = GUI_MSG_TOUCH;
                new_msg.h_deswin    = GUI_WinP2H(pUnderPointer);
                new_msg.h_srcwin    = (H_WIN)msg->h_deswin;
                new_msg.dwAddData1  = GUI_MSG_TOUCH_LONGDOWN;
                new_msg.dwAddData2  = (__u32)dwAddData;
                new_msg.dwReserved  = msg->dwReserved;
                new_msg.p_arg       = NULL;
                GUI_SendMessage(&new_msg);
            }

            break;

        case GUI_MSG_TOUCH_UP:
            if (capture_wnd)
            {
                __gui_msg_t     new_msg;
                /*先发消息去焦点状态*/
                new_msg.id          = GUI_MSG_TOUCHINACTIVE;
                new_msg.h_deswin    = GUI_WinP2H(capture_wnd);
                new_msg.h_srcwin    = 0;
                new_msg.dwAddData1  = 0;
                new_msg.dwAddData2  = 0;
                new_msg.p_arg       = 0;
                GUI_SendMessage(&new_msg);
                new_msg.id          = GUI_MSG_TOUCH;
                new_msg.h_deswin    = (H_WIN)capture_wnd;
                new_msg.h_srcwin    = (H_WIN)msg->h_deswin;
                new_msg.dwAddData1  = GUI_MSG_TOUCH_UP;
                new_msg.dwAddData2  = (__u32)dwAddData;
                new_msg.dwReserved  = msg->dwReserved;
                new_msg.p_arg       = NULL;
                GUI_SendMessage(&new_msg);
                //GUI_WinReleaseCapture ();
            }
            else if (pUnderPointer)
            {
                __gui_msg_t     new_msg;

                if (pUnderPointer->dwStyle & WS_DISABLED)
                {
                    break;
                }

                /*先发消息去焦点状态*/
                new_msg.id          = GUI_MSG_TOUCHINACTIVE;
                new_msg.h_deswin    = GUI_WinP2H(pUnderPointer);
                new_msg.h_srcwin    = 0;
                new_msg.dwAddData1  = 0;
                new_msg.dwAddData2  = 0;
                new_msg.p_arg       = 0;
                GUI_SendMessage(&new_msg);
                new_msg.id          = GUI_MSG_TOUCH;
                new_msg.h_deswin    = GUI_WinP2H(pUnderPointer);
                new_msg.h_srcwin    = (H_WIN)msg->h_deswin;
                new_msg.dwAddData1  = GUI_MSG_TOUCH_UP;
                new_msg.dwAddData2  = (__u32)dwAddData;
                new_msg.dwReserved  = msg->dwReserved;
                new_msg.p_arg       = NULL;
                GUI_SendMessage(&new_msg);
            }

            break;
    }

    return 0;
}

/*
**********************************************************************************************************************
*                                               GUI_DefKeyMsgHandler
*
* author:
*
* date:             2009-8-6
*
* Description:      gui defkeymsghandler
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32 GUI_DefKeyMsgHandler(__gui_msg_t  *msg)
{
    __gui_win_t     *pWin = (__gui_win_t *)msg->h_deswin;

    if (pWin->ActiveChild)
    {
        __gui_msg_t   new_msg;
        new_msg.id              = msg->id;
        new_msg.dwAddData1      = msg->dwAddData1;
        new_msg.h_deswin        = GUI_WinP2H(pWin->ActiveChild);
        new_msg.h_srcwin        = msg->h_deswin;
        new_msg.dwAddData2      = msg->dwAddData2;
        new_msg.p_arg           = NULL;
        GUI_SendMessage(&new_msg);
    }

    return 0;
}

/*
**********************************************************************************************************************
*                                               GUI_DefCreateMsgHandler
*
* author:
*
* date:             2009-8-6
*
* Description:      gui defcreatemsghandler
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static int GUI_DefCreateMsgHandler(__gui_msg_t *msg)
{
    return 0;
}

/*
**********************************************************************************************************************
*                                               GUI_MainWinDefaultProc
*
* Description: Default callback for windows Any window should call this routine in the "default" part of the
*               its callback function for messages it does not handle itself.
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__s32 GUI_ManWinDefaultProc(__gui_msg_t *msg)
{
    if (!msg)
    {
        ORANGE_WRN("message para can not be NULL!");
        return ORANGE_FAIL;
    }

    if (msg->id == GUI_MSG_TOUCH)
    {
        return GUI_DefTouchMsgHandler(msg);
    }

    if (msg->id == GUI_MSG_KEY)
    {
        return GUI_DefKeyMsgHandler(msg);
    }

    if (msg->id == GUI_MSG_CREATE)
    {
        return GUI_DefCreateMsgHandler(msg);
    }

    if (msg->id == GUI_MSG_PAINT)
    {
        return GUI_DefPaintMsgHandler(msg);
    }

    return GUI_DefOtherMsgHandler(msg);
}


/*
**********************************************************************************************************************
*                                               GUI_GetTouchFocusWin
*
* author:
*
* date:             2009-8-6
*
* Description:      gui gettouchfocuswin
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN  GUI_GetTouchFocusWin(__s32 x, __s32 y)
{
    __gui_frmwin_t    *pFrameWin;
    H_WIN              mainwin;
    __gui_win_t       *pWin;
    __bool             isChild;
    mainwin = NULL;

    if (capture_wnd)
    {
        mainwin = GUI_WinGetMainManWin(GUI_WinP2H(capture_wnd));
    }
    else
    {
        pFrameWin = (__gui_frmwin_t *)GUI_GetFrameWinUnderPointEx(x, y);

        if (pFrameWin == NULL)
        {
            ORANGE_WRN("no framewin under point!");
            return NULL;
        }

        if (touchfocus_wnd)
        {
            /*判断当前找到的frmwin是否在touchfocus_wnd的子孙窗口中*/
            isChild = GUI_WinIsAncestor(GUI_WinP2H(touchfocus_wnd), GUI_WinP2H(pFrameWin));

            if (!isChild && (GUI_WinP2H(touchfocus_wnd) != GUI_WinP2H(pFrameWin)))
            {
                return NULL;
            }
        }

        mainwin = GUI_WinGetMainManWin(GUI_WinP2H(pFrameWin));
        pWin    = GUI_WinH2P(mainwin);
        /*判断当前找到的frmwin是否在消息循环窗口的子孙窗口中*/
        isChild = GUI_WinIsAncestor(GUI_WinP2H(pWin->Primitive), GUI_WinP2H(pFrameWin));

        if (!isChild && (pWin->Primitive != (__gui_win_t *)pFrameWin))
        {
            return NULL;
        }
    }

    return mainwin;
}

//==================================================================
//function name:    GUI_PointGetCtrlWin
//author:
//date:             2009-10-26
//description:      gui pointgetctrlwin
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

static __bool GUI_PointInParent(H_WIN hWnd, __s32 x, __s32 y, H_WIN hChild)
{
    __gui_win_t     *pWin  = GUI_WinH2P(hWnd);
    __gui_win_t     *pControl;
    __bool          IsAncestor  = ORANGE_FALSE;
    pControl = (__gui_win_t *)pWin->FirstChild;

    while (pControl)
    {
        if ((pControl->dwStyle & WS_VISIBLE)
            && PtInRect((__gui_rect_t *)(&pControl->win_rect), x, y))
        {
            if (hChild == GUI_WinP2H(pControl))
            {
                return ORANGE_TRUE;
            }
            else
            {
                IsAncestor = GUI_WinIsAncestor(GUI_WinP2H(pControl), hChild);

                if (!IsAncestor)
                {
                    return ORANGE_FALSE;
                }

                x = x - pControl->win_rect.left;
                y = y - pControl->win_rect.top;
                return GUI_PointInParent(GUI_WinP2H(pControl), x, y, hChild);
            }
        }

        pControl = pControl->Next;
    }

    return ORANGE_FALSE;
}


//==================================================================
//function name:    GUI_PointInWin
//author:
//date:             2009-10-26
//description:      gui pointinwin
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__bool  GUI_SceenPointInWin(H_WIN hWnd, __s32 x, __s32 y)
{
    __gui_win_t         *pWin   =   GUI_WinH2P(hWnd);
    __gui_frmwin_t      *pFrameWin;
    __bool              IsAncestor  = ORANGE_FALSE;

    if (!pWin)
    {
        ORANGE_WRN("win para can not be NULL!");
        return ORANGE_FALSE;
    }

    if (pWin->DataType != TYPE_HWND)
    {
        ORANGE_WRN("win data para error!");
        return ORANGE_FALSE;
    }

    pFrameWin = (__gui_frmwin_t *)GUI_GetFrameWinUnderPointEx(x, y);

    if (pFrameWin == NULL)
    {
        ORANGE_WRN("not framewin under point!");
        return 0;
    }

    if (pWin->WinType == TYPE_MANWIN)
    {
        IsAncestor = GUI_WinIsAncestor(hWnd, GUI_WinP2H(pFrameWin));

        if (IsAncestor)
        {
            return ORANGE_TRUE;
        }

        return ORANGE_FALSE;
    }
    else if (pWin->WinType == TYPE_FRAMEWIN)
    {
        if (hWnd == GUI_WinP2H(pFrameWin))
        {
            return ORANGE_TRUE;
        }

        return ORANGE_FALSE;
    }
    else if (pWin->WinType == TYPE_CONTROL)
    {
        __pos_t             sc_pos;
        __pos_t             win_pos;
        IsAncestor = GUI_WinIsAncestor(GUI_WinP2H(pFrameWin), hWnd);

        if (!IsAncestor)
        {
            return ORANGE_FALSE;
        }

        sc_pos.x = x;
        sc_pos.y = y;
        {
            GUI_POINT       pPhyPt;
            GUI_POINT       pLogPt;
            pPhyPt.x    = x;
            pPhyPt.y    = y;
            GUI_ScnPhyPt2LogPt(gui_scndir, &pPhyPt, &pLogPt);
            x           = pLogPt.x;
            y           = pLogPt.y;
        }
        GUI_ScreenToWindow(GUI_WinP2H(pFrameWin), &sc_pos, &win_pos);
        return GUI_PointInParent(GUI_WinP2H(pFrameWin), win_pos.x, win_pos.y, hWnd);
    }

    return ORANGE_FALSE;
}

/*
**********************************************************************************************************************
*                                               GUI_MainWinCreate
*
* author:
*
* date:             2009-7-25
*
* Description:      gui manwincreate
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN GUI_ManWinCreate(__gui_manwincreate_para_t *create_info)
{
    __gui_win_t                 create_para;
    H_WIN                       hWnd;
    __gui_manwin_t              *manwin = NULL;
    __gui_win_t                 *Parent = GUI_WinH2P(create_info->hParent);
    __gui_win_t                 *Hosting = GUI_WinH2P(create_info->hHosting);
#if GUI_CHECK_ARG_EN > 0

    if (!GUI_ManWinAssertPara(create_info))
    {
        ORANGE_WRN("manwin create para error!");
        return HWND_INVALID;
    }

#endif
    orange_memset(&create_para, 0, sizeof(__gui_win_t));

    if (create_info->hParent == NULL)
    {
        create_para.Parent          = GUI_WinH2P(GUI_WinGetRootWin());
    }
    else if (Parent->DataType != TYPE_HWND)
    {
        ORANGE_WRN("input parent data type failed!");
        return HWND_INVALID;
    }
    else if (Parent->WinType != TYPE_MANWIN)
    {
        ORANGE_WRN("input parent win type error!");
        return HWND_INVALID;
    }
    else if ((Hosting != NULL) && (Parent != NULL))
    {
        /*在这种情况下，hosting窗口必须是新建窗口的兄弟窗口*/
        if (Hosting->Parent != Parent)
        {
            ORANGE_WRN("inpur hosting win failed!");
            return HWND_INVALID;
        }
        else
        {
            create_para.Parent      = GUI_WinH2P(create_info->hParent);
        }
    }
    else
    {
        create_para.Parent           = GUI_WinH2P(create_info->hParent);
    }

    create_para.LyrWin              = NULL;
    create_para.DataType            = TYPE_HWND;
    create_para.WinType             = TYPE_MANWIN;
    create_para.FirstChild          = NULL;
    create_para.ActiveChild         = NULL;
    create_para.OldUnderPointer     = NULL;
    create_para.Primitive           = NULL;
    create_para.NotifProc           = NULL;
    create_para.pZOrderNode         = NULL;
    create_para.dwAddData           = (unsigned long)create_info->attr;
    create_para.dwStyle             = 0;
    create_para.dwExStyle           = 0;
    create_para.id                  = create_info->id;
    /* init win struct contents                     */
    /* name                                         */
    orange_memclr(create_para.name, GUI_NAME_MAX);
    orange_strncpy(create_para.name, create_info->name, GUI_NAME_MAX - 1);

    /* cb function                                  */
    if (create_info->ManWindowProc)
    {
        create_para.WindowProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)create_info->ManWindowProc);
    }
    else
    {
        create_para.WindowProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)GUI_ManWinDefaultProc);
    }

    create_para.dwAddData       = (unsigned long)create_info->attr;
    hWnd = GUI_WinCreate(&create_para, sizeof(__gui_manwin_t) - sizeof(__gui_win_t));

    if (hWnd == HWND_INVALID)
    {
        ORANGE_WRN("create win failed!");
        goto err;
    }

    manwin                      = (__gui_manwin_t *)GUI_WinH2P(hWnd);
    manwin->pHosting            = GUI_WinH2P(create_info->hHosting);

    if (manwin->pHosting == NULL && (create_para.Parent == GUI_WinH2P(GUI_WinGetRootWin())))
    {
        // Create message queue for this new main window.
        manwin->pMessages      = orange_malloc(sizeof(__win_msgqueue_t));

        if (!manwin->pMessages)
        {
            ORANGE_WRN("alloc message queue failed!");
            goto err;
        }

        // Init message queue.
        if (!InitMsgQueue(manwin->pMessages, 0))
        {
            goto err;
        }

        /*管理窗口为入口管理窗口*/
        manwin->ManWinType    = TYPE_ROOTMAIN;
        manwin->pRootMain     = GUI_WinH2P(hWnd);
    }
    else
    {
        if (manwin->pHosting != NULL)
        {
            manwin->pMessages = GUI_WinGetMsgQueue(create_info->hHosting);
            GUI_WinAddNewHostedWin(manwin->pHosting, GUI_WinH2P(hWnd));
        }
        else
        {
            manwin->pMessages = GUI_WinGetMsgQueue(create_info->hParent);
        }

        /*管理窗口为子管理窗口*/
        manwin->ManWinType    = TYPE_CHILDMAIN;
        manwin->pRootMain     = GUI_WinH2P(GUI_WinGetMainManWin(create_info->hParent));
    }

    manwin->threadid          = pthread_self();
    manwin->pFirstHosted      = NULL;
    manwin->pNextHosted       = NULL;
    /*send message to win to create private data*/
    {
        __gui_msg_t   new_msg;
        new_msg.id          =  GUI_MSG_CREATE;
        new_msg.h_srcwin    = 0;
        new_msg.h_deswin    = (H_WIN)hWnd;
        new_msg.dwAddData1  = 0;
        new_msg.dwAddData2  = (unsigned long)create_info;
        new_msg.p_arg       = 0;

        if (GUI_SendMessage(&new_msg))
        {
            ORANGE_WRN("create manwin private data failed!");
            goto err;
        }
    }
    return (H_WIN)hWnd;
err:
#if 1
    {
        __gui_msg_t   new_msg;
        new_msg.id          =  GUI_MSG_CLOSE;
        new_msg.h_srcwin    = 0;
        new_msg.h_deswin    = (H_WIN)hWnd;
        new_msg.dwAddData1  = 0;
        new_msg.dwAddData2  = (unsigned long)create_info;
        new_msg.p_arg       = 0;

        if (GUI_SendMessage(&new_msg))
        {
            ORANGE_WRN("create manwin private data failed!");
            goto err;
        }
    }
#else
    if (manwin->pMessages && manwin->ManWinType == TYPE_ROOTMAIN)
    {
        DestroyMsgQueue(manwin->pMessages);
        orange_mfree(manwin->pMessages);
    }

    if (hWnd)
    {
        /*从父窗口链表中删除该窗口*/
        GUI_WinLock();
        Win_DeattachFromParent(GUI_WinH2P(hWnd));
        GUI_WinUnlock();
        /* free p_win memory                            */
        orange_mfree((void *)hWnd);
    }
#endif
    return HWND_INVALID;
}

/*
**********************************************************************************************************************
*                                               GUI_MainWinDelete
*
* author:
*
* date:             2009-7-25
*
* Description:      gui manwindelete
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_ManWinDelete(H_WIN hWnd)
{
    __s32   ret_quit;
    __s32   ret_delete;
    ret_delete  = GUI_WinDelete(hWnd);
    ret_quit    = GUI_PostQuitMessage(hWnd);

    if (ret_quit != ORANGE_OK || ret_delete != ORANGE_OK)
    {
        return ORANGE_FAIL;
    }

    return ORANGE_OK;
}

#endif
