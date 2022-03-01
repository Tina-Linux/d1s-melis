/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    ctrlwin.c
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2009-8-4
*
*Description :  widget windows function implement
*
*Others :       None at present.
*
* History :
*
* <Author>         <time>      <version>     <description>
*
* William Wang    2009-8-4       1.1.0         Create File
*
*******************************************************************************************************************
*/

#include "ctrlwin_i.h"

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

static __bool GUI_CtrlWinAssertPara(__gui_ctlwincreate_para_t *create_info)
{
    RECT            rect;
    __gui_win_t     *Parent;
    __gui_win_t     *Child;

    if ((!create_info) || (!create_info->spClassName)
        || (!create_info->hParent))
    {
        ORANGE_WRN("input create info NULL or layer handle NULL!\n");
        return ORANGE_FALSE;
    }

    GUI_WinGetWinRECT(create_info->hParent, &rect);

    if (create_info->ctl_rect.x + create_info->ctl_rect.width > rect.width)
    {
        ORANGE_WRN("input width over layer framebuffer width\n");
        return ORANGE_FALSE;
    }

    if (create_info->ctl_rect.y + create_info->ctl_rect.height > rect.height)
    {
        ORANGE_WRN("input height over layer framebuffer width\n");
        return ORANGE_FALSE;
    }

    Parent  = GUI_WinH2P(create_info->hParent);
    Child   = Parent->FirstChild;

    while (Child)
    {
        if (Child->id == create_info->id)
        {
            ORANGE_WRN("create id overlap\n");
            return ORANGE_FALSE;
        }

        Child = Child->Next;
    }

    return ORANGE_TRUE;
}

/*static*/
/*
**********************************************************************************************************************
*                                               GUI_AddNewCtrlInstance
*
* author:
*
* date:             2009-8-4
*
* Description:      增加一个widget实例
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static void _GUI_AddNewCtrlInstance(__gui_widget_t *pParent, __gui_widget_t *pNewCtrl)
{
    GUI_WinLock();
    //init_invrgn ((__gui_win_t *)pNewCtrl);
    pNewCtrl->pcci->nUseCount ++;
    GUI_WinUnlock();
}

/*
**********************************************************************************************************************
*                                               GUI_RemoveCtrlInstance
*
* author:
*
* date:             2009-8-4
*
* Description:      删除一个widget实例
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __bool _GUI_RemoveCtrlInstance(__gui_widget_t *pParent, __gui_widget_t *pCtrl)
{
    __gui_widget_t *pFirstCtrl;
    __bool          fFound = ORANGE_FALSE;
    GUI_WinLock();
    pFirstCtrl = (__gui_widget_t *)pParent->base_win.FirstChild;

    if (!pFirstCtrl)
    {
        GUI_WinUnlock();
        return ORANGE_FALSE;
    }
    else
    {
        if (pFirstCtrl == pCtrl)
        {
            pParent->base_win.FirstChild = (__gui_win_t *)pCtrl->base_win.Next;

            if (pCtrl->base_win.Next)
            {
                pCtrl->base_win.Next->Prev = NULL;
            }

            fFound = ORANGE_TRUE;
        }
        else
        {
            while (pFirstCtrl->base_win.Next)
            {
                if (pFirstCtrl->base_win.Next == (__gui_win_t *)pCtrl)
                {
                    pFirstCtrl->base_win.Next = pCtrl->base_win.Next;

                    if (pFirstCtrl->base_win.Next)
                    {
                        pFirstCtrl->base_win.Next->Prev = pCtrl->base_win.Next->Prev;
                    }

                    fFound = ORANGE_TRUE;
                    break;
                }

                pFirstCtrl = (__gui_widget_t *)pFirstCtrl->base_win.Next;
            }
        }
    }

    if (fFound)
    {
        pCtrl->pcci->nUseCount --;
    }

    GUI_WinUnlock();
    return fFound;
}



/*public*/
/*
**********************************************************************************************************************
*                                               GUI_CtrlWinGetAddData
*
* author:
*
* date:             2009-8-6
*
* Description:      GUI_CtrlWinGetAddData
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

unsigned long  GUI_CtrlWinGetAddData(H_WIN hWnd)
{
    __gui_win_t         *pWin       = GUI_WinH2P(hWnd);
    __gui_widget_t      *pWidget    = (__gui_widget_t *)pWin;
#if GUI_CHECK_ARG_EN > 0

    if (!hWnd)
    {
        ORANGE_WRN("input para error!\n");
        return HWND_INVALID;
    }

#endif

    if (pWin->DataType != TYPE_HWND)
    {
        ORANGE_WRN("windows handle error!\n");
        return 0;
    }

    if (pWin->WinType != TYPE_CONTROL)
    {
        ORANGE_WRN("windows not control windows!\n");
        return 0;
    }

    return (unsigned long)pWidget->dwAddData;
}

/*
**********************************************************************************************************************
*                                               GUI_CtrlWinSetAddData
*
* author:
*
* date:             2009-8-6
*
* Description:      GUI_CtrlWinSetAddData
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_CtrlWinSetAddData(H_WIN hWnd, unsigned long dwAddData)
{
    __gui_win_t         *pWin       = GUI_WinH2P(hWnd);
    __gui_widget_t      *pWidget    = (__gui_widget_t *)pWin;
#if GUI_CHECK_ARG_EN > 0

    if (!hWnd)
    {
        ORANGE_WRN("input para error!\n");
        return HWND_INVALID;
    }

#endif

    if (pWin->DataType != TYPE_HWND)
    {
        ORANGE_WRN("windows handle error!\n");
        return ORANGE_FAIL;
    }

    if (pWin->WinType != TYPE_CONTROL)
    {
        ORANGE_WRN("windows not control windows!\n");
        return ORANGE_FAIL;
    }

    pWidget->dwAddData  = (unsigned long)dwAddData;
    return ORANGE_OK;
}


/*
**********************************************************************************************************************
*                                               GUI_DefaultControlProc
*
* author:
*
* date:             2009-8-6
*
* Description:      gui defaultcontrolproc
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_CtrlWinDefaultProc(__gui_msg_t *msg)
{
    return GUI_FrmWinDefaultProc(msg);
}


/*
**********************************************************************************************************************
*                                               GUI_CtrlWinGetClassName
*
* author:
*
* date:             2009-11-29
*
* Description:      gui ctrlwingetclassname
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_CtrlWinGetClassName(H_WIN hWnd, char *name)
{
    __gui_widget_t          *pWin;
    __gui_ctlclass_info_t   *cci;
#if GUI_CHECK_ARG_EN > 0

    if (hWnd == NULL || name == NULL)
    {
        ORANGE_WRN("input parameter error!\n");
        return ORANGE_FAIL;
    }

#endif
    pWin = (__gui_widget_t *)GUI_WinH2P(hWnd);
    cci = pWin->pcci;
    orange_strncpy(name, cci->name, GUI_CLASSNAME_MAX);
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_CtrlWinCreate
*
* author:
*
* date:             2009-8-4
*
* Description:      GUI_CtrlWinCreate
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN  GUI_CtrlWinCreate(__gui_ctlwincreate_para_t *create_info)
{
    __gui_frmwin_t          *pFrameWin = NULL;
    __gui_ctlclass_info_t   *cci;
    __gui_widget_t          *pNewCtrl   = NULL;
    __gui_win_t              create_para;
    H_WIN                    hWnd;
    __gui_lyrwin_t          *pLayer;
#if GUI_CHECK_ARG_EN > 0

    if (!GUI_CtrlWinAssertPara(create_info))
    {
        ORANGE_WRN("control create para error!\n");
        return HWND_INVALID;
    }

#endif
    cci = GUI_GetControlClassInfo(create_info->spClassName);

    if (!cci)
    {
        ORANGE_WRN("can not find windows class info!\n");
        return HWND_INVALID;
    }

    if (!create_info->hParent)
    {
        ORANGE_WRN("input parent ststus failed!\n");
        return HWND_INVALID;
    }

    pFrameWin = (__gui_frmwin_t *)GUI_WinH2P(GUI_WinGetFrmWin(create_info->hParent));
    orange_memset(&create_para, 0, sizeof(__gui_win_t));

    /*控件直接采用图层加速机制实现*/
    if (cci->dwStyle & WS_LYRWIN)
    {
        if (create_info->hLayer == NULL)
        {
            ORANGE_WRN("layer handle can't be NULL!\n");
            return HWND_INVALID;
        }

        pLayer  =  GUI_LyrH2P(create_info->hLayer);

        /*此时layer已经在其上建立了窗口机制，故不能在作为一个单独的图层使用*/
        if (pLayer->use_sts != LAYER_DRAW)
        {
            ORANGE_WRN("layer status error!\n");
            return HWND_INVALID;
        }

        pLayer->use_sts  =   LAYER_WINDOWS;

        /*控件直接挂在管理窗口之下，此时需要把该窗口添加到图层窗口Z序中*/
        if (!pFrameWin)
        {
            /*分配Z序节点*/
            create_para.pZOrderNode = orange_malloc(sizeof(__gui_zordernode_t));

            if (!create_para.pZOrderNode)
            {
                ORANGE_WRN("alloc zordernode failed!\n");
                goto err;
            }
        }/*控件直接挂在framewin窗口之下，此时不需要把该窗口添加到图层窗口Z序中*/
        else
        {
            create_para.pZOrderNode = NULL;
        }
    }
    else /*控件采用窗口机制实现*/
    {
        if (!pFrameWin)
        {
            ORANGE_WRN("input parent ststus failed!\n");
            return HWND_INVALID;
        }

        pLayer  = GUI_WinH2P(create_info->hParent)->LyrWin;

        /*此时layer已经在其上建立了单窗口机制，而且该窗口(create win)不是图层单窗口上的子窗口。
          故不能在其上建立其他窗口机制*/
        if (pLayer->use_sts == LAYER_WINDOWS
            && create_info->hParent != pLayer->ManWinZOrder.pTopMost->hWnd
            && create_info->hParent != pLayer->TopMostWinZOrder.pTopMost->hWnd)
        {
            ORANGE_WRN("layer status error!\n");
            return HWND_INVALID;
        }

        /*图层上没有建立单窗口机制，直接作为窗口容器使用*/
        if (pLayer->use_sts != LAYER_WINDOWS)
        {
            pLayer->use_sts         =   LAYER_CONTAINER;
        }

        /*控件直接挂在framewin窗口之下，此时不需要把该窗口添加到图层窗口Z序中*/
        create_para.pZOrderNode = NULL;
    }

    create_para.ActiveChild         = NULL;
    create_para.Next                = NULL;
    create_para.win_rect.left       = create_info->ctl_rect.x;
    create_para.win_rect.top        = create_info->ctl_rect.y;
    create_para.win_rect.right      = create_info->ctl_rect.x + create_info->ctl_rect.width - 1;
    create_para.win_rect.bottom     = create_info->ctl_rect.y + create_info->ctl_rect.height - 1;
    create_para.client_rect.left    = create_info->ctl_rect.x;
    create_para.client_rect.top     = create_info->ctl_rect.y;
    create_para.client_rect.right   = create_info->ctl_rect.x + create_info->ctl_rect.width - 1;
    create_para.client_rect.bottom  = create_info->ctl_rect.y + create_info->ctl_rect.height - 1;
    create_para.DataType            = TYPE_HWND;
    create_para.WinType             = TYPE_CONTROL;
    create_para.dwAddData           = (unsigned long)create_info->attr;
    create_para.dwExStyle           = create_info->dwExStyle | cci->dwExStyle;
    create_para.dwStyle             = create_info->dwStyle | WS_CHILD | cci->dwStyle;
    create_para.FirstChild          = NULL;
    create_para.LyrWin              = pLayer;
    create_para.id                  = create_info->id;
    create_para.NotifProc           = NULL;
    create_para.OldUnderPointer     = NULL;
    create_para.Parent              = GUI_WinH2P(create_info->hParent);
    create_para.pLogFont            = NULL;
    create_para.Primitive           = NULL;
    create_para.bkColor             = cci->iBkColor;
    create_para.WindowProc          = cci->ControlProc;
    orange_memcpy(create_para.name, create_info->name, GUI_NAME_MAX);
    /*创建基本的窗口框架*/
    hWnd = GUI_WinCreate(&create_para, sizeof(__gui_widget_t) - sizeof(__gui_win_t));
    if (hWnd == HWND_INVALID)
    {
        ORANGE_WRN("create windows handle failed!\n");
        goto err;
    }

    pNewCtrl                    = (__gui_widget_t *)GUI_WinH2P(hWnd);
    pNewCtrl->pFrameWin         = (__gui_frmwin_t *)pFrameWin;
    pNewCtrl->pcci              = cci;
    pNewCtrl->base_win.pGCRInfo = pFrameWin->base_win.pGCRInfo;
    _GUI_AddNewCtrlInstance((__gui_widget_t *)pNewCtrl->base_win.Parent, (__gui_widget_t *)pNewCtrl);
    /*send message to win to create private data*/
    {
        __gui_msg_t   new_msg;
        new_msg.id          =  GUI_MSG_CREATE;
        new_msg.h_srcwin    = 0;
        new_msg.h_deswin    = (H_WIN)hWnd;
        new_msg.dwAddData1  = (unsigned long)pNewCtrl->base_win.Parent;
        new_msg.dwAddData2  = (unsigned long)create_info->attr;
        new_msg.p_arg       = 0;

        if (GUI_SendMessage(&new_msg))
        {
            ORANGE_WRN("create manwin private data failed!\n");
            _GUI_RemoveCtrlInstance((__gui_widget_t *)pNewCtrl->base_win.Parent, (__gui_widget_t *)pNewCtrl);
            goto err;
        }
    }
    if (pNewCtrl->base_win.Parent->dwStyle & WS_VISIBLE && pNewCtrl->base_win.dwStyle & WS_VISIBLE)
    {
        GUI_WinUpdate(GUI_WinP2H(pNewCtrl),  ORANGE_TRUE);
    }

    return (H_WIN)pNewCtrl;
err:

    if (pNewCtrl)
    {
        orange_mfree(pNewCtrl);
    }

    return HWND_INVALID;
}

/*
**********************************************************************************************************************
*                                               GUI_CtrlWinDelete
*
* author:
*
* date:             2009-8-6
*
* Description:      GUI_CtrlWinDelete
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_CtrlWinDelete(H_WIN hWnd)
{
    __s32            ret = ORANGE_FAIL;
    __gui_widget_t  *pWin   = (__gui_widget_t *)GUI_WinH2P(hWnd);
    __gui_lyrwin_t  *pLayer = NULL;
#if GUI_CHECK_ARG_EN > 0

    if (!hWnd)
    {
        ORANGE_WRN("input para error!\n");
        return HWND_INVALID;
    }

#endif
    pLayer      = GUI_LyrH2P(GUI_WinGetLyrWin(hWnd));
    pWin->pcci->nUseCount --;
    ret = GUI_WinDelete(hWnd);

    if (ret == ORANGE_OK)
    {
        if (pLayer)
        {
            /*判断该窗口是否是单窗口且直接挂在图层的Z序之上*/
            if (pLayer->use_sts == LAYER_WINDOWS
                && (pLayer->ManWinZOrder.pTopMost->hWnd == hWnd ||
                    pLayer->TopMostWinZOrder.pTopMost->hWnd == hWnd))
            {
                pLayer->use_sts = LAYER_DRAW;
            }
        }
    }

    return ret;
}
