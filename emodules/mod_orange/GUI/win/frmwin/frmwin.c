/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    framewin.c
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2009-8-6
*
*Description :  framewin窗口定义与实现
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>        <version>     <description>
*
* William Wang     2009-8-6         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __FRMWIN_C__
#define __FRMWIN_C__

#include "frmwin_i.h"
#include <kconfig.h>

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

static __bool GUI_FrmWinAssertPara(__gui_framewincreate_para_t *create_info)
{
    FB              fb;

    if ((!create_info) || (!create_info->hLayer) || (!create_info->hHosting))
    {
        ORANGE_WRN("input create info NULL or layer handle NULL!\n");
        return ORANGE_FALSE;
    }

    GUI_LyrWinGetFB(create_info->hLayer, &fb);

    if (create_info->rect.x + create_info->rect.width > fb.size.width)
    {
        ORANGE_WRN("input width over layer framebuffer width\n");
        return ORANGE_FALSE;
    }

    if (create_info->rect.y + create_info->rect.height > fb.size.height)
    {
        ORANGE_WRN("input height over layer framebuffer width\n");
        return ORANGE_FALSE;
    }

    return ORANGE_TRUE;
}

/*static*/
/*
**********************************************************************************************************************
*                                               _GUI_GetFirstActivableWindow
*
* author:
*
* date:             2009-7-28
*
* Description:      获取下一个被激活的窗口
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __gui_win_t  *_GUI_GetFirstActivableWindow(__gui_win_t *lyrwin)
{
    __gui_zordernode_t      *pNode;
    __gui_win_t             *pTemp;
    __gui_lyrwin_t          *pLayer = lyrwin->LyrWin;
    /*sereach in layer win main win z order*/
    pNode = pLayer->ManWinZOrder.pTopMost;

    while (pNode)
    {
        pTemp = (__gui_win_t *)(pNode->hWnd);

        if ((pTemp->WinType != TYPE_CONTROL))
        {
            return pTemp;
        }

        pNode = pNode->pNext;
    }

    /*sereach in layer win topmost win z order*/
    pNode = pLayer->TopMostWinZOrder.pTopMost;

    while (pNode)
    {
        pTemp = (__gui_win_t *)(pNode->hWnd);

        if ((pTemp->WinType != TYPE_CONTROL))
        {
            return pTemp;
        }

        pNode = pNode->pNext;
    }

    return NULL;
}

/*public*/
/*
**********************************************************************************************************************
*                                               DestroyAllControls
*
* author:
*
* date:             2009-8-5
*
* Description:      destroyallcontrols
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_FrmWinDestroyAllCtrlWins(H_WIN hDlg)
{
    __gui_widget_t      *pNext;
    __gui_win_t         *pWin   = GUI_WinH2P(hDlg);
    //__gui_frmwin_t      *pDlg   = (__gui_frmwin_t*)GUI_WinH2P(hDlg);
    __gui_win_t         *pCtrl  = (__gui_win_t *)pWin->FirstChild;

    while (pCtrl)
    {
        pNext = (__gui_widget_t *)pCtrl->Next;
#ifdef CONFIG_GUI_WIN_CTRLWIN_EN
        GUI_CtrlWinDelete((H_WIN)pCtrl);
#endif
        pCtrl = (__gui_win_t *)pNext;
    }
}
/*
**********************************************************************************************************************
*                                               GUI_FrmWinDefaultProc
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

__s32 GUI_FrmWinDefaultProc(__gui_msg_t *msg)
{
    return GUI_ManWinDefaultProc(msg);
}

/*
**********************************************************************************************************************
*                                               GUI_FrmWinCreate
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

H_WIN GUI_FrmWinCreate(__gui_framewincreate_para_t *create_info)
{
    __gui_win_t                 create_para = {0};
    H_WIN                       hWnd        = HWND_INVALID;
    __gui_frmwin_t              *framewin   = NULL;

#if GUI_CHECK_ARG_EN > 0

    if (!GUI_FrmWinAssertPara(create_info))
    {
        ORANGE_WRN("framewin create para error!\n");
        return HWND_INVALID;
    }

#endif
    orange_memset(&create_para, 0, sizeof(__gui_win_t));

    create_para.Parent              = (__gui_win_t *)create_info->hHosting;
    create_para.LyrWin              = GUI_LyrH2P(create_info->hLayer);
    create_para.DataType            = TYPE_HWND;
    create_para.WinType             = TYPE_FRAMEWIN;
    create_para.client_rect.left    = create_info->rect.x;
    create_para.client_rect.right   = create_info->rect.x + create_info->rect.width - 1;
    create_para.client_rect.top     = create_info->rect.y;
    create_para.client_rect.bottom  = create_info->rect.y + create_info->rect.height - 1;
    create_para.win_rect.left       = create_info->rect.x;
    create_para.win_rect.right      = create_info->rect.x + create_info->rect.width - 1;
    create_para.win_rect.top        = create_info->rect.y;
    create_para.win_rect.bottom     = create_info->rect.y + create_info->rect.height - 1;
    create_para.id                  = create_info->id;
    create_para.FirstChild          = NULL;
    create_para.ActiveChild         = NULL;
    create_para.OldUnderPointer     = NULL;
    create_para.Primitive           = NULL;
    create_para.NotifProc           = NULL;
    create_para.dwStyle             = create_info->dwStyle;
    create_para.dwExStyle           = create_info->dwExStyle;
    /* init win struct contents                     */
    /* name                                         */
    eLIBs_memclr(create_para.name, GUI_NAME_MAX);
    eLIBs_strncpy(create_para.name, create_info->name, GUI_NAME_MAX - 1);
    /* cb function                                  */
    if (create_info->FrameWinProc)
    {
        create_para.WindowProc      = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)create_info->FrameWinProc);
    }
    else
    {
        create_para.WindowProc      = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)GUI_FrmWinDefaultProc);
    }

    create_para.dwAddData           = (unsigned long)create_info->attr;

    /*此时layer已经在其上建立了单窗口机制，故不能在其上建立其他窗口机制*/
    if (create_para.LyrWin->use_sts == LAYER_WINDOWS)
    {
        ORANGE_WRN("input layer parameter error!\n");
        return HWND_INVALID;
    }
    else
    {
        /*此时有其他frmwin已经建立在此图层之上*/
        if (create_para.LyrWin->ManWinZOrder.pTopMost
            || create_para.LyrWin->TopMostWinZOrder.pTopMost)
        {
            create_para.LyrWin->use_sts = LAYER_CONTAINER;
        }
        else
        {
            create_para.LyrWin->use_sts = LAYER_DRAW;
        }
    }

    create_para.pZOrderNode         = orange_malloc(sizeof(__gui_zordernode_t));

    if (!create_para.pZOrderNode)
    {
        ORANGE_WRN("alloc zordernode failed!\n");
        goto err;
    }

    hWnd = GUI_WinCreate(&create_para, sizeof(__gui_frmwin_t) - sizeof(__gui_win_t));
    if (hWnd == HWND_INVALID)
    {
        ORANGE_WRN("create win failed!\n");
        goto err;
    }

    framewin                        = (__gui_frmwin_t *)GUI_WinH2P(hWnd);
    framewin->pManWin               = (__gui_manwin_t *)framewin->base_win.Parent;
    framewin->pHosting              = GUI_WinH2P(create_info->hOwner);
    framewin->pFirstHosted          = NULL;
    framewin->pNextHosted           = NULL;

    if (framewin->pHosting)
    {
        GUI_WinAddNewHostedWin(framewin->pHosting, GUI_WinH2P(hWnd));
    }

    /*draw framewin*/
    if (framewin->base_win.dwStyle & WS_VISIBLE)
    {
        GUI_InvalidateRect((H_WIN)hWnd, NULL, ORANGE_TRUE);
    }

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
            ORANGE_WRN("create manwin private data failed!\n");
            goto err;
        }
    }

    return hWnd;
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
            ORANGE_WRN("create manwin private data failed!\n");
            goto err;
        }
    }
#else
    if (create_para.pZOrderNode)
    {
        orange_mfree(create_para.pZOrderNode);
        create_para.pZOrderNode = NULL;
    }

    if (hWnd)
    {
        /*从父窗口链表中删除该窗口*/
        GUI_WinLock();
        Win_DeattachFromParent(GUI_WinH2P(hWnd));
        Win_DeattachFromZorder(GUI_WinH2P(hWnd));
        GUI_WinUnlock();
        /* free p_win memory                            */
        orange_mfree((void *)hWnd);
    }
#endif
    return HWND_INVALID;
}

/*
**********************************************************************************************************************
*                                               GUI_FrmWinDelete
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


__s32 GUI_FrmWinDelete(H_WIN hWnd)
{
    __s32               ret_quit;
    __s32               ret_delete;
    __gui_lyrwin_t     *pLayer = NULL;
    pLayer      = GUI_LyrH2P(GUI_WinGetLyrWin(hWnd));
    ret_delete  = GUI_WinDelete(hWnd);
    ret_quit    = GUI_PostQuitMessage(hWnd);

    if (ret_quit != ORANGE_OK || ret_delete != ORANGE_OK)
    {
        return ORANGE_FAIL;
    }

    if (pLayer)
    {
        /*此时没有任何frmwin在此图层之上*/
        if ((!pLayer->ManWinZOrder.pTopMost)
            && (!pLayer->TopMostWinZOrder.pTopMost))
        {
            pLayer->use_sts    = LAYER_DRAW;
        }
    }

    return ORANGE_OK;
}

#endif/*__FRMWIN_C__*/
