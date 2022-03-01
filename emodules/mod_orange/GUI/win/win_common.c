/*
*******************************************************************************************************************
*                                                           Mod orange
*                                                   the embedded graphic support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    win_common.c
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2009-8-6
*
*Description :  windows common function definition
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
#include "win_i.h"
#include <mod_display.h>
#include <kconfig.h>


/*static*/
/*
**********************************************************************************************************************
*                                               add_new_window
*
* author:
*
* date:             2009-7-28
*
* Description:      add new window
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static void add_new_window(__gui_zorderinfo_t *zorder, __gui_zordernode_t *pNode)
{
    pNode->pNext        = zorder->pTopMost;
    zorder->pTopMost    = pNode;
    zorder->nNumber++;
}

/*
**********************************************************************************************************************
*                                               remove_window
*
* author:
*
* date:             2009-7-28
*
* Description:      remove window
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static void remove_window(__gui_zorderinfo_t *zorder, pguiwin pWin)
{
    __gui_zordernode_t *pNode, *pTemp;
    pNode = zorder->pTopMost;

    if (pNode->hWnd == GUI_WinP2H(pWin))
    {
        zorder->pTopMost = pNode->pNext;
    }
    else
    {
        while (pNode->pNext)
        {
            if (pNode->pNext->hWnd == GUI_WinP2H(pWin))
            {
                pTemp        = pNode->pNext->pNext;
                pNode->pNext = pTemp;
                break;
            }

            pNode = pNode->pNext;
        }
    }

    zorder->nNumber--;
}

/*
**********************************************************************************************************************
*                                               LyrWin_AddToVisableList
*
* author:
*
* date:             2009-8-2
*
* Description:      添加图层到显示列表中
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32  LyrWin_AttachToVisableList(__gui_lyrwin_t *pLayer)
{
    __gui_lyrwin_t       *pCur;
    __gui_lyrwin_t       *pNext;
    __s32                prio1;
    __s32                prio2;
    GUI_WinLock();

    if (sg_VisiLayerList == NULL)
    {
        sg_VisiLayerList    = pLayer;
        pLayer->Next        = NULL;
    }
    else
    {
        prio2   = GUI_LyrGetPrio(GUI_LyrP2H(pLayer));
        pCur    = sg_VisiLayerList;
        pNext   = pCur->Next;

        while (pCur)
        {
            prio1 = GUI_LyrGetPrio(GUI_LyrP2H(pCur));

            if (prio1 < prio2)
            {
                /*最顶图层的优先级小于当前图层的优先级*/
                if (pCur == sg_VisiLayerList)
                {
                    sg_VisiLayerList       = pLayer;
                    sg_VisiLayerList->Next = pCur;
                    GUI_WinUnlock();
                    return ORANGE_OK;
                }
            }

            /*放在结束位置*/
            if (pNext == NULL)
            {
                pCur->Next      = pLayer;
                pLayer->Next    = NULL;
                GUI_WinUnlock();
                return ORANGE_OK;
            }

            /*比较next和新加图层的优先级*/
            prio1 = GUI_LyrGetPrio(GUI_LyrP2H(pNext));

            if (prio1 < prio2)
            {
                pCur->Next      = pLayer;
                pLayer->Next    = pNext;
                GUI_WinUnlock();
                return ORANGE_OK;
            }

            pCur        = pCur->Next;
            pNext       = pNext->Next;
        }
    }

    GUI_WinUnlock();
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               LyrWin_DeattachFromVisList
*
* author:
*
* date:             2009-8-2
*
* Description:      从可见图层链表中删除图层
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32  LyrWin_DeattachFromVisList(__gui_lyrwin_t *pLayer)
{
    __gui_lyrwin_t       *pCur;
    __gui_lyrwin_t       *pNext;
    GUI_WinLock();

    if (sg_VisiLayerList == NULL)
    {
        GUI_WinUnlock();
        return ORANGE_FAIL;
    }

    pCur    = sg_VisiLayerList;

    if (pCur == pLayer)
    {
        sg_VisiLayerList = pLayer->Next;
        GUI_WinUnlock();
        return ORANGE_OK;
    }

    pNext   = pCur->Next;

    while (pCur)
    {
        if (pNext == pLayer)
        {
            pCur->Next = pNext->Next;
            GUI_WinUnlock();
            return ORANGE_OK;
        }

        pCur        = pCur->Next;
        pNext       = pNext->Next;
    }

    GUI_WinUnlock();
    return ORANGE_FAIL;
}

/*
**********************************************************************************************************************
*                                               Layer_AttachToInVisList
*
* author:
*
* date:             2009-8-2
*
* Description:      向不可见图层链表中添加图层
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32 LyrWin_AttachToInVisList(__gui_lyrwin_t *pLayer)
{
    __gui_lyrwin_t       *pCur;
    GUI_WinLock();

    if (sg_InVisLayerList == NULL)
    {
        sg_InVisLayerList = pLayer;
        pLayer->Next      = NULL;
        GUI_WinUnlock();
        return ORANGE_OK;
    }

    pCur                = sg_InVisLayerList;
    sg_InVisLayerList   = pLayer;
    pLayer->Next        = pCur;
    GUI_WinUnlock();
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               Layer_DeAttachFromInVisList
*
* author:
*
* date:             2009-8-2
*
* Description:      从不可见图层链表中删除图层
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32 LyrWin_DeAttachFromInVisList(__gui_lyrwin_t *pLayer)
{
    __gui_lyrwin_t       *pCur;
    __gui_lyrwin_t       *pNext;
    GUI_WinLock();

    if (sg_InVisLayerList == NULL)
    {
        GUI_WinUnlock();
        return ORANGE_FAIL;
    }

    pCur    = sg_InVisLayerList;

    if (pCur == pLayer)
    {
        sg_InVisLayerList = pLayer->Next;
        GUI_WinUnlock();
        return ORANGE_OK;
    }

    pNext   = pCur->Next;

    while (pCur)
    {
        if (pNext == pLayer)
        {
            pCur->Next = pNext->Next;
            GUI_WinUnlock();
            return ORANGE_OK;
        }

        pCur        = pCur->Next;

        if (pNext)
        {
            pNext       = pNext->Next;
        }
    }

    GUI_WinUnlock();
    return ORANGE_FAIL;
}

/*
**********************************************************************************************************************
*                                               Layer_ListSetTop
*
* author:
*
* date:             2009-8-2
*
* Description:      图层置顶
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32 LyrWin_ListSetTop(__gui_lyrwin_t *pLayer)
{
    __gui_lyrwin_t       *pCur;
    __gui_lyrwin_t       *pNext;
    GUI_WinLock();

    if (sg_VisiLayerList == NULL)
    {
        GUI_WinUnlock();
        return ORANGE_FAIL;
    }

    pCur    = sg_VisiLayerList;
    pNext   = pCur->Next;

    while (pCur)
    {
        if (pNext == pLayer)
        {
            pCur->Next      = pNext->Next;
            pNext           = sg_VisiLayerList;
            pLayer->Next    = pNext;
            sg_VisiLayerList = pLayer;
            GUI_WinUnlock();
            return ORANGE_OK;
        }

        pCur        = pCur->Next;

        if (pCur)
        {
            pNext   = pCur->Next;
        }
        else
        {
            break;
        }
    }

    GUI_WinUnlock();
    return ORANGE_FAIL;
}

/*
**********************************************************************************************************************
*                                               Layer_ListSetBottom
*
* author:
*
* date:             2009-8-2
*
* Description:      设置图层置底
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32 LyrWin_ListSetBottom(__gui_lyrwin_t *pLayer)
{
    __gui_lyrwin_t       *pCur;
    __gui_lyrwin_t       *pNext;
    __bool               bFind = ORANGE_FALSE;
    GUI_WinLock();

    if (sg_VisiLayerList == NULL)
    {
        GUI_WinLock();
        return ORANGE_FAIL;
    }

    pCur    = sg_VisiLayerList;
    pNext   = pCur->Next;

    if (pCur == pLayer)
    {
        bFind               = ORANGE_TRUE;
        sg_VisiLayerList    = pLayer->Next;
    }

    while (pNext)
    {
        if (pNext == pLayer)
        {
            if (pNext->Next != NULL)
            {
                pCur->Next      = pNext->Next;
                bFind           = ORANGE_TRUE;
            }
            else
            {
                /*链表尾即为该图层节点*/
                GUI_WinUnlock();
                return ORANGE_OK;
            }
        }

        pCur                = pCur->Next;

        if (pCur)
        {
            pNext           = pCur->Next;
        }
        else
        {
            break;
        }
    }

    if (bFind)
    {
        if (sg_VisiLayerList == NULL)
        {
            sg_VisiLayerList = pLayer;
            pLayer->Next     = NULL;
        }
        else
        {
            pCur->Next      = pLayer;
            pLayer->Next    = NULL;
        }

        GUI_WinUnlock();
        return ORANGE_OK;
    }

    /*链表尾即为该图层节点*/
    if (pCur == pLayer)
    {
        GUI_WinUnlock();
        return ORANGE_OK;
    }

    /*找不到该节点*/
    GUI_WinUnlock();
    return ORANGE_FAIL;
}

#if 0
/*
**********************************************************************************************************************
*                                               LyrWin_AddToVisableList
*
* author:
*
* date:             2009-8-2
*
* Description:      添加图层到显示列表中
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32  Sprite_AttachToVisableList(__gui_lyrwin_t *pLayer)
{
    __gui_lyrwin_t       *pCur;
    __gui_lyrwin_t       *pNext;
    __s32                prio1;
    __s32                prio2;
    GUI_WinLock();

    if (sg_VisiSpriteList == NULL)
    {
        sg_VisiSpriteList   = pLayer;
        pLayer->Next        = NULL;
    }
    else
    {
        prio2   = GUI_LyrGetPrio(GUI_LyrP2H(pLayer));
        pCur    = sg_VisiSpriteList;
        pNext   = pCur->Next;

        while (pCur)
        {
            prio1 = GUI_LyrGetPrio(GUI_LyrP2H(pCur));

            if (prio1 < prio2)
            {
                /*最顶图层的优先级小于当前图层的优先级*/
                if (pCur == sg_VisiSpriteList)
                {
                    sg_VisiSpriteList       = pLayer;
                    sg_VisiSpriteList->Next = pCur;
                    GUI_WinUnlock();
                    return ORANGE_OK;
                }
            }

            /*放在结束位置*/
            if (pNext == NULL)
            {
                pCur->Next      = pLayer;
                pLayer->Next    = NULL;
                GUI_WinUnlock();
                return ORANGE_OK;
            }

            /*比较next和新加图层的优先级*/
            prio1 = GUI_LyrGetPrio(GUI_LyrP2H(pNext));

            if (prio1 < prio2)
            {
                pCur->Next      = pLayer;
                pLayer->Next    = pNext;
                GUI_WinUnlock();
                return ORANGE_OK;
            }

            pCur        = pCur->Next;
            pNext       = pNext->Next;
        }
    }

    GUI_WinUnlock();
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               LyrWin_DeattachFromVisList
*
* author:
*
* date:             2009-8-2
*
* Description:      从可见图层链表中删除图层
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32  Sprite_DeattachFromVisList(__gui_lyrwin_t *pLayer)
{
    __gui_lyrwin_t       *pCur;
    __gui_lyrwin_t       *pNext;
    GUI_WinLock();

    if (sg_VisiSpriteList == NULL)
    {
        GUI_WinUnlock();
        return ORANGE_FAIL;
    }

    pCur    = sg_VisiSpriteList;

    if (pCur == pLayer)
    {
        sg_VisiSpriteList = pLayer->Next;
        GUI_WinUnlock();
        return ORANGE_OK;
    }

    pNext   = pCur->Next;

    while (pCur)
    {
        if (pNext == pLayer)
        {
            pCur->Next = pNext->Next;
            GUI_WinUnlock();
            return ORANGE_OK;
        }

        pCur        = pCur->Next;
        pNext       = pNext->Next;
    }

    GUI_WinUnlock();
    return ORANGE_FAIL;
}

/*
**********************************************************************************************************************
*                                               Layer_AttachToInVisList
*
* author:
*
* date:             2009-8-2
*
* Description:      向不可见图层链表中添加图层
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32 Sprite_AttachToInVisList(__gui_lyrwin_t *pLayer)
{
    __gui_lyrwin_t       *pCur;
    GUI_WinLock();

    if (sg_InVisSpriteList == NULL)
    {
        sg_InVisSpriteList = pLayer;
        pLayer->Next       = NULL;
        GUI_WinUnlock();
        return ORANGE_OK;
    }

    pCur                = sg_InVisSpriteList;
    sg_InVisSpriteList  = pLayer;
    pLayer->Next        = pCur;
    GUI_WinUnlock();
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               Layer_DeAttachFromInVisList
*
* author:
*
* date:             2009-8-2
*
* Description:      从不可见图层链表中删除图层
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32 Sprite_DeAttachFromInVisList(__gui_lyrwin_t *pLayer)
{
    __gui_lyrwin_t       *pCur;
    __gui_lyrwin_t       *pNext;
    GUI_WinLock();

    if (sg_InVisSpriteList == NULL)
    {
        GUI_WinUnlock();
        return ORANGE_FAIL;
    }

    pCur    = sg_InVisSpriteList;

    if (pCur == pLayer)
    {
        sg_InVisSpriteList = pLayer->Next;
        GUI_WinUnlock();
        return ORANGE_OK;
    }

    pNext   = pCur->Next;

    while (pCur)
    {
        if (pNext == pLayer)
        {
            pCur->Next = pNext->Next;
            GUI_WinUnlock();
            return ORANGE_OK;
        }

        pCur        = pCur->Next;

        if (pNext)
        {
            pNext       = pNext->Next;
        }
    }

    GUI_WinUnlock();
    return ORANGE_FAIL;
}

/*
**********************************************************************************************************************
*                                               Layer_ListSetTop
*
* author:
*
* date:             2009-8-2
*
* Description:      图层置顶
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32 Sprite_ListSetTop(__gui_lyrwin_t *pLayer)
{
    __gui_lyrwin_t       *pCur;
    __gui_lyrwin_t       *pNext;
    GUI_WinLock();

    if (sg_VisiSpriteList == NULL)
    {
        GUI_WinUnlock();
        return ORANGE_FAIL;
    }

    pCur    = sg_VisiSpriteList;
    pNext   = pCur->Next;

    while (pCur)
    {
        if (pNext == pLayer)
        {
            pCur->Next                  = pNext->Next;
            pNext                       = sg_VisiSpriteList;
            pLayer->Next                = pNext;
            sg_VisiSpriteList           = pLayer;
            GUI_WinUnlock();
            return ORANGE_OK;
        }

        pCur        = pCur->Next;

        if (pCur)
        {
            pNext   = pCur->Next;
        }
        else
        {
            break;
        }
    }

    GUI_WinUnlock();
    return ORANGE_FAIL;
}

/*
**********************************************************************************************************************
*                                               Layer_ListSetBottom
*
* author:
*
* date:             2009-8-2
*
* Description:      设置图层置底
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32 Sprite_ListSetBottom(__gui_lyrwin_t *pLayer)
{
    __gui_lyrwin_t       *pCur;
    __gui_lyrwin_t       *pNext;
    __bool               bFind = ORANGE_FALSE;
    GUI_WinLock();

    if (sg_VisiSpriteList == NULL)
    {
        GUI_WinLock();
        return ORANGE_FAIL;
    }

    pCur    = sg_VisiSpriteList;
    pNext   = pCur->Next;

    if (pCur == pLayer)
    {
        bFind               = ORANGE_TRUE;
        sg_VisiSpriteList   = pLayer->Next;
    }

    while (pNext)
    {
        if (pNext == pLayer)
        {
            if (pNext->Next != NULL)
            {
                pCur->Next      = pNext->Next;
                bFind           = ORANGE_TRUE;
            }
            else
            {
                /*链表尾即为该图层节点*/
                GUI_WinUnlock();
                return ORANGE_OK;
            }
        }

        pCur                = pCur->Next;

        if (pCur)
        {
            pNext           = pCur->Next;
        }
        else
        {
            break;
        }
    }

    if (bFind)
    {
        if (sg_VisiSpriteList == NULL)
        {
            sg_VisiSpriteList = pLayer;
            pLayer->Next     = NULL;
        }
        else
        {
            pCur->Next      = pLayer;
            pLayer->Next    = NULL;
        }

        GUI_WinUnlock();
        return ORANGE_OK;
    }

    /*链表尾即为该图层节点*/
    if (pCur == pLayer)
    {
        GUI_WinUnlock();
        return ORANGE_OK;
    }

    /*找不到该节点*/
    GUI_WinUnlock();
    return ORANGE_FAIL;
}
#endif
/*public*/



/*global*/
/*
**********************************************************************************************************************
*                                               GUI_LyrGetPrio
*
* author:
*
* date:             2009-8-2
*
* Description:      获取硬件图层的优先级 (内部使用)
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_LyrGetPrio(H_LYR  hLyr)
{
    __gui_lyrwin_t       *player = GUI_LyrH2P(hLyr);
    __s32                prio = ORANGE_FAIL;
    __u32 dispbuffer[3];
#if GUI_CHECK_ARG_EN > 0

    if (!player)
    {
        ORANGE_WRN("h_lyr empty!\n");
        return ORANGE_FAIL;
    }

#endif

    if (player->devfp == 0)
    {
        //ES_FILE       *p_disp = (ES_FILE *)0;
        /*open display driver handle*/
        //p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
        __mp* p_disp = esKSRV_Get_Display_Hld();
        if (!p_disp)
        {
            ORANGE_WRN("open display device fail!\n");
            return ORANGE_FAIL;
        }

        player->devfp = p_disp;
    }

    if (player->type == GUI_LYRWIN_NORMAL)
    {
        /*modified by Derek,2010-12-9 19:30:08*/
        dispbuffer[0] = (unsigned long)(player->h_hwlyr);
        dispbuffer[1] = 0;
        dispbuffer[2] = 0;
       // prio = eLIBs_fioctrl(player->devfp, DISP_CMD_LAYER_GET_PRIO, SEL_SCREEN, (void *)dispbuffer); //end modified
    }
    else
    {
        __wrn("don't support sprite!\n");
    }

    return prio;
}

/*
**********************************************************************************************************************
*                                               Layer_AttachToVisableList
*
* author:
*
* date:             2014-7-20
*
* Description:      layer attachtovisablelist
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  Layer_AttachToVisableList(H_LYR  hLyr)
{
    __gui_lyrwin_t       *player = GUI_LyrH2P(hLyr);
#if GUI_CHECK_ARG_EN > 0

    if (!player)
    {
        ORANGE_WRN("h_lyr empty!\n");
        return ORANGE_FAIL;
    }

#endif

    if (player->type == GUI_LYRWIN_NORMAL)
    {
        return LyrWin_AttachToVisableList(player);
    }
    else
    {
        ORANGE_WRN("don't support sprite!\n");
        return ORANGE_FAIL;
    }
}

/*
**********************************************************************************************************************
*                                               Layer_DeattachFromVisList
*
* author:
*
* date:             2014-7-20
*
* Description:      layer deattachfromvislist
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  Layer_DeattachFromVisList(H_LYR hLyr)
{
    __gui_lyrwin_t       *player = GUI_LyrH2P(hLyr);
#if GUI_CHECK_ARG_EN > 0

    if (!player)
    {
        ORANGE_WRN("h_lyr empty!\n");
        return ORANGE_FAIL;
    }

#endif

    if (player->type == GUI_LYRWIN_NORMAL)
    {
        return LyrWin_DeattachFromVisList(player);
    }
    else
    {
        ORANGE_WRN("don't support sprite!\n");
        return ORANGE_FAIL;
    }
}

/*
**********************************************************************************************************************
*                                               Layer_AttachToInVisList
*
* author:
*
* date:             2014-7-20
*
* Description:      layer attachtoinvislist
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 Layer_AttachToInVisList(H_LYR  hLyr)
{
    __gui_lyrwin_t       *player = GUI_LyrH2P(hLyr);
#if GUI_CHECK_ARG_EN > 0

    if (!player)
    {
        ORANGE_WRN("h_lyr empty!\n");
        return ORANGE_FAIL;
    }

#endif

    if (player->type == GUI_LYRWIN_NORMAL)
    {
        return LyrWin_AttachToInVisList(player);
    }
    else
    {
        ORANGE_WRN("don't support sprite!\n");
        return ORANGE_FAIL;
    }
}

/*
**********************************************************************************************************************
*                                               Layer_DeattachFromInVisList
*
* author:
*
* date:             2014-7-20
*
* Description:      layer deattachfrominvislist
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  Layer_DeattachFromInVisList(H_LYR hLyr)
{
    __gui_lyrwin_t       *player = GUI_LyrH2P(hLyr);
#if GUI_CHECK_ARG_EN > 0

    if (!player)
    {
        ORANGE_WRN("h_lyr empty!\n");
        return ORANGE_FAIL;
    }

#endif

    if (player->type == GUI_LYRWIN_NORMAL)
    {
        return LyrWin_DeAttachFromInVisList(player);
    }
    else
    {
        ORANGE_WRN("don't support sprite!\n");
        return ORANGE_FAIL;
    }
}

/*
**********************************************************************************************************************
*                                               Layer_ListSetTop
*
* author:
*
* date:             2014-7-20
*
* Description:      layer listsettop
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 Layer_ListSetTop(H_LYR hLyr)
{
    __gui_lyrwin_t       *player = GUI_LyrH2P(hLyr);
#if GUI_CHECK_ARG_EN > 0

    if (!player)
    {
        ORANGE_WRN("h_lyr empty!\n");
        return ORANGE_FAIL;
    }

#endif

    if (player->type == GUI_LYRWIN_NORMAL)
    {
        return LyrWin_ListSetTop(player);
    }
    else
    {
        ORANGE_WRN("don't support sprite!\n");
        return ORANGE_FAIL;
    }
}


/*
**********************************************************************************************************************
*                                               Layer_ListSetBottom
*
* author:
*
* date:             2014-7-20
*
* Description:      layer listsetbottom
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 Layer_ListSetBottom(H_LYR hLyr)
{
    __gui_lyrwin_t       *player = GUI_LyrH2P(hLyr);
#if GUI_CHECK_ARG_EN > 0

    if (!player)
    {
        ORANGE_WRN("h_lyr empty!\n");
        return ORANGE_FAIL;
    }

#endif

    if (player->type == GUI_LYRWIN_NORMAL)
    {
        return LyrWin_ListSetBottom(player);
    }
    else
    {
        ORANGE_WRN("don't support sprite!\n");
        return ORANGE_FAIL;
    }
}

/*
**********************************************************************************************************************
*                                               GUI_WinAddNewHostedWin
*
* author:
*
* date:             2009-8-29
*
* Description:      增加新的Hosted主窗口
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_WinAddNewHostedWin(__gui_win_t *pHosting, __gui_win_t *pHosted)
{
    if (pHosting->WinType == TYPE_MANWIN)
    {
        __gui_manwin_t *head;
        __gui_manwin_t *prev;
        __gui_manwin_t *pCur = (__gui_manwin_t *)pHosting;
        __gui_manwin_t *pNew = (__gui_manwin_t *)pHosted;
        head = (__gui_manwin_t *)pCur->pFirstHosted;

        if (head)
        {
            while (head)
            {
                prev = head;
                head = (__gui_manwin_t *)head->pNextHosted;
            }

            prev->pNextHosted = (__gui_win_t *)pNew;
        }
        else
        {
            pCur->pFirstHosted = (__gui_win_t *)pNew;
        }

        pNew->pNextHosted = NULL;
    }
    else if (pHosting->WinType == TYPE_FRAMEWIN)
    {
        __gui_frmwin_t *head, *prev;
        __gui_frmwin_t *pCur = (__gui_frmwin_t *)pHosting;
        __gui_frmwin_t *pNew = (__gui_frmwin_t *)pHosted;
        head = (__gui_frmwin_t *)pCur->pFirstHosted;

        if (head)
        {
            while (head)
            {
                prev = head;
                head = (__gui_frmwin_t *)head->pNextHosted;
            }

            prev->pNextHosted = (__gui_win_t *)pNew;
        }
        else
        {
            pCur->pFirstHosted = (__gui_win_t *)pNew;
        }

        pNew->pNextHosted = NULL;
    }

    return;
}

/*
**********************************************************************************************************************
*                                               GUI_WinRemoveHostedWin
*
* author:
*
* date:             2009-8-31
*
* Description:      删除hosted窗口
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_WinRemoveHostedWin(__gui_win_t *pHosting, __gui_win_t *pHosted)
{
    if (pHosting->WinType == TYPE_MANWIN)
    {
        __gui_manwin_t *head, *prev;
        __gui_manwin_t *pCur = (__gui_manwin_t *)pHosting;
        __gui_manwin_t *pNew = (__gui_manwin_t *)pHosted;
        head = (__gui_manwin_t *)pCur->pFirstHosted;

        if (head == pNew)
        {
            pCur->pFirstHosted = head->pNextHosted;
            return;
        }

        while (head)
        {
            prev = head;
            head = (__gui_manwin_t *)head->pNextHosted;

            if (head == pNew)
            {
                prev->pNextHosted = head->pNextHosted;
                return;
            }
        }

        return;
    }
    else if (pHosting->WinType == TYPE_FRAMEWIN)
    {
        __gui_frmwin_t *head, *prev;
        __gui_frmwin_t *pCur = (__gui_frmwin_t *)pHosting;
        __gui_frmwin_t *pNew = (__gui_frmwin_t *)pHosted;
        head = (__gui_frmwin_t *)pCur->pFirstHosted;

        if (head == pNew)
        {
            pCur->pFirstHosted = head->pNextHosted;
            return;
        }

        while (head)
        {
            prev = head;
            head = (__gui_frmwin_t *)head->pNextHosted;

            if (head == pNew)
            {
                prev->pNextHosted = head->pNextHosted;
                return;
            }
        }
    }

    return;
}

/*
**********************************************************************************************************************
*                                               Win_AttachToParent
*
* author:
*
* date:             2009-7-28
*
* Description:      win attachtoparent
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void Win_AttachToParent(__gui_win_t *pWin)
{
    /* link win to its parent win                   */
    if (pWin->Parent)
    {
        __gui_win_t  *p_parent = pWin->Parent;

        if (!p_parent->FirstChild)
        {
            p_parent->FirstChild        = pWin;
            //p_parent->ActiveChild       = pWin;
            pWin->Next                  = NULL;
            pWin->Prev                  = NULL;
        }
        else
        {
            __gui_win_t *r;
            r = p_parent->FirstChild;

            while (r->Next)
            {
                r = r->Next;
            }

            r->Next     = pWin;
            pWin->Prev  = r;
        }

        pWin->Next = NULL;
    }
}

/*
**********************************************************************************************************************
*                                               Win_DeattachFromParent
*
* author:
*
* date:             2009-7-28
*
* Description:      win deattachfromparent
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void Win_DeattachFromParent(__gui_win_t *pWin)
{
    __gui_win_t *p_parent;
    /* unlink it from parent window                 */
    p_parent = pWin->Parent;

    if (p_parent)
    {
        /* if p_win is focus child, we need update focuschild to next                                               */
        if (p_parent->ActiveChild == pWin)
        {
            p_parent->ActiveChild = pWin->Next;

            /* if p_win->next is null, we need update focuschild to first child                                     */
            if (!p_parent->ActiveChild)
            {
                p_parent->ActiveChild = p_parent->FirstChild;
            }
        }

        /* if p_win is firstchild, we need to update firstchild                                                     */
        if (p_parent->FirstChild == pWin)
        {
            p_parent->FirstChild        = pWin->Next;

            if (p_parent->FirstChild)
            {
                p_parent->FirstChild->Prev  = NULL;
            }

            /* if first child is focus child, we need to update focuschild to next win                              */
            if (p_parent->ActiveChild == pWin)
            {
                p_parent->ActiveChild = pWin->Next;
            }
        }
        else
        {
            __gui_win_t *p_win_tmp;
            p_win_tmp = p_parent->FirstChild;

            /* find p_win in the child list, and unlink it                                                              */
            while (1)
            {
                if (!p_win_tmp->Next)
                {
                    ORANGE_WRN("parent's child list cannot find p_win!\n");
                }

                /* unlink p_win from child list         */
                if (p_win_tmp->Next == pWin)
                {
                    p_win_tmp->Next     = pWin->Next;

                    if (pWin->Next)
                    {
                        pWin->Next->Prev    = p_win_tmp;
                    }

                    break;
                }

                p_win_tmp = p_win_tmp->Next;
            }
        }
    }
}

/*
**********************************************************************************************************************
*                                               Win_AttachToZorder
*
* author:
*
* date:             2009-7-28
*
* Description:      win attachtozorder
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void Win_AttachToZorder(H_WIN hWnd)
{
    __gui_win_t   *pWin = GUI_WinH2P(hWnd);

    if (pWin->WinType == TYPE_FRAMEWIN)
    {
        /*link it to zorder list*/
        if (pWin->LyrWin)
        {
            __gui_zorderinfo_t      *zorderinfo;
            __gui_lyrwin_t           *player = pWin->LyrWin;

            if (pWin->dwExStyle & WS_EX_TOPMOST)
            {
                zorderinfo = &player->TopMostWinZOrder;
                add_new_window(zorderinfo, pWin->pZOrderNode);
            }
            else
            {
                zorderinfo = &player->ManWinZOrder;
                add_new_window(zorderinfo, pWin->pZOrderNode);
            }
        }
    }
}

/*
**********************************************************************************************************************
*                                               GUI_ChangActiveMainWindow
*
* author:
*
* date:             2009-7-28
*
* Description:      gui changactivemanwindow
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__gui_win_t  *GUI_ChangActiveManWindow(__gui_win_t  *pWin)
{
    pguiwin         pOldActive;
    pOldActive = active_mainwnd;
#ifdef CONFIG_GUI_WIN_MANWIN_EN
    GUI_SetActiveManWin(GUI_WinP2H(pWin));
#endif

    if (pOldActive)
    {
        __gui_msg_t new_msg;
        new_msg.h_srcwin    =   NULL;
        new_msg.h_deswin    = (H_WIN)pOldActive;
        new_msg.id          =   GUI_MSG_SET_UNFOCUS;
        new_msg.dwAddData1  = (unsigned long)pWin;
        //GUI_SendNotifyMessage (&new_msg);
        GUI_SendMessage(&new_msg);
    }

    if (pWin)
    {
        __gui_msg_t new_msg;
        new_msg.h_srcwin    =   NULL;
        new_msg.h_deswin    = (H_WIN)pWin;
        new_msg.id          =   GUI_MSG_SET_FOCUS;
        new_msg.dwAddData1  = (unsigned long)pOldActive;
        //GUI_SendNotifyMessage (&new_msg);
        GUI_SendMessage(&new_msg);
    }

    return pOldActive;
}

/*
**********************************************************************************************************************
*                                               Win_DeattachFromZorder
*
* author:
*
* date:             2009-7-28
*
* Description:      win deattachfromzorder
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void Win_DeattachFromZorder(__gui_win_t *pWin)
{
    if (pWin->WinType == TYPE_FRAMEWIN)
    {
        /*link it to zorder list*/
        if (pWin->LyrWin)
        {
            __gui_zorderinfo_t      *zorderinfo;
            __gui_lyrwin_t           *player = pWin->LyrWin;

            if (pWin->dwExStyle & WS_EX_TOPMOST)
            {
                zorderinfo = &player->TopMostWinZOrder;
                remove_window(zorderinfo, pWin);
            }
            else
            {
                zorderinfo = &player->ManWinZOrder;
                remove_window(zorderinfo, pWin);
            }
        }
    }
}

/*
**********************************************************************************************************************
*                                               GUI_FrameWinToScreen
*
* author:
*
* date:             2009-8-4
*
* Description:      framewin窗口区域转化成屏幕区域
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_FrameWinToScreen(H_WIN hWnd, __gui_rect_t *screct)
{
    __gui_frmwin_t      *pFramewin = (__gui_frmwin_t *)GUI_WinH2P(hWnd);
    __gui_lyrwin_t      *pLayer;
#if GUI_CHECK_ARG_EN > 0

    if ((!hWnd) || (!screct))
    {
        ORANGE_WRN("input parameter empty!\n");
        return ;
    }

#endif
    pLayer = pFramewin->base_win.LyrWin;

    if (pLayer->status == GUI_LYRWIN_STA_ON)
    {
        screct->left   = (__s32)(pFramewin->base_win.win_rect.left - pLayer->lyrpara.src_win.x
                                 + pLayer->lyrpara.scn_win.x);

        /*屏幕左坐标超出图层显示范围*/
        if (screct->left >= (__s32)(pLayer->lyrpara.scn_win.x + pLayer->lyrpara.scn_win.width))
        {
            screct->left    = 0;
            screct->right   = 0;
            screct->bottom  = 0;
            screct->top     = 0;
            return ;
        }

        screct->top    = (__s32)(pFramewin->base_win.win_rect.top - pLayer->lyrpara.src_win.y
                                 + pLayer->lyrpara.scn_win.y);

        /*屏幕上坐标超出图层之外*/
        if (screct->top >= (__s32)(pLayer->lyrpara.scn_win.y + pLayer->lyrpara.scn_win.height))
        {
            screct->left    = 0;
            screct->right   = 0;
            screct->bottom  = 0;
            screct->top     = 0;
            return ;
        }

        screct->right  = (__s32)(screct->left  + pFramewin->base_win.win_rect.right -
                                 pFramewin->base_win.win_rect.left);

        /*屏幕右坐标超出图层之外*/
        if (screct->right >= (__s32)(pLayer->lyrpara.scn_win.x + pLayer->lyrpara.scn_win.width))
        {
            screct->right = (__s32)(pLayer->lyrpara.scn_win.x + pLayer->lyrpara.scn_win.width);
        }

        screct->bottom = (__s32)(screct->top + pFramewin->base_win.win_rect.bottom - pFramewin->base_win.win_rect.top);

        /*屏幕下坐标超出图层之外*/
        if (screct->bottom >= (__s32)(pLayer->lyrpara.scn_win.y + pLayer->lyrpara.scn_win.height))
        {
            screct->bottom = (__s32)(pLayer->lyrpara.scn_win.y + pLayer->lyrpara.scn_win.height);
        }
    }
    else
    {
        screct->left        = 0;
        screct->right       = 0;
        screct->top         = 0;
        screct->bottom      = 0;
    }

    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_FrameClentToScreen
*
* author:
*
* date:             2009-8-4
*
* Description:      framewin有效客户区转换成屏幕区域
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_FrameClentToScreen(H_WIN hWnd, __gui_rect_t *screct)
{
    __gui_frmwin_t      *pFramewin = (__gui_frmwin_t *)GUI_WinH2P(hWnd);
    __gui_lyrwin_t      *pLayer;
#if GUI_CHECK_ARG_EN > 0

    if ((!hWnd) || (!screct))
    {
        ORANGE_WRN("input parameter empty!\n");
        return ;
    }

#endif
    pLayer = (__gui_lyrwin_t *)pFramewin->base_win.LyrWin;

    if (pLayer->status == GUI_LYRWIN_STA_ON)
    {
        screct->left   = (__s32)(pFramewin->base_win.client_rect.left - pLayer->lyrpara.src_win.x
                                 + pLayer->lyrpara.scn_win.x);

        /*屏幕左坐标超出图层显示范围*/
        if (screct->left >= (__s32)(pLayer->lyrpara.scn_win.x + pLayer->lyrpara.scn_win.width))
        {
            screct->left    = 0;
            screct->right   = 0;
            screct->bottom  = 0;
            screct->top     = 0;
            return ;
        }

        screct->top    = (__s32)(pFramewin->base_win.client_rect.top - pLayer->lyrpara.src_win.y
                                 + pLayer->lyrpara.scn_win.y);

        /*屏幕上坐标超出图层之外*/
        if (screct->top >= (__s32)(pLayer->lyrpara.scn_win.y + pLayer->lyrpara.scn_win.height))
        {
            screct->left    = 0;
            screct->right   = 0;
            screct->bottom  = 0;
            screct->top     = 0;
            return ;
        }

        screct->right  = (__s32)(screct->left  + pFramewin->base_win.client_rect.right -
                                 pFramewin->base_win.client_rect.left);

        /*屏幕右坐标超出图层之外*/
        if (screct->right >= (__s32)(pLayer->lyrpara.scn_win.x + pLayer->lyrpara.scn_win.width))
        {
            screct->right = (__s32)(pLayer->lyrpara.scn_win.x + pLayer->lyrpara.scn_win.width);
        }

        screct->bottom = (__s32)(screct->top + pFramewin->base_win.client_rect.bottom - pFramewin->base_win.client_rect.top);

        /*屏幕下坐标超出图层之外*/
        if (screct->bottom >= (__s32)(pLayer->lyrpara.scn_win.y + pLayer->lyrpara.scn_win.height))
        {
            screct->bottom = (__s32)(pLayer->lyrpara.scn_win.y + pLayer->lyrpara.scn_win.height);
        }
    }
    else
    {
        screct->left        = 0;
        screct->right       = 0;
        screct->top         = 0;
        screct->bottom      = 0;
    }

    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_ScreenToFrameWin
*
* author:
*
* date:             2009-8-4
*
* Description:      屏幕区域转换成framewin区域
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_ScreenToFrameWin(H_WIN hWnd, __gui_rect_t *screct, __gui_rect_t *win_rect)
{
    __gui_frmwin_t      *pFramewin = (__gui_frmwin_t *)GUI_WinH2P(hWnd);
    __gui_lyrwin_t      *pLayer;
#if GUI_CHECK_ARG_EN > 0

    if ((!hWnd) || (!screct) || (!win_rect))
    {
        ORANGE_WRN("input parameter empty!\n");
        return ;
    }

#endif
    pLayer = pFramewin->base_win.LyrWin;

    if (pLayer->status == GUI_LYRWIN_STA_ON)
    {
        win_rect->left   = (__s32)(screct->left + pLayer->lyrpara.src_win.x
                                   - pLayer->lyrpara.scn_win.x);
        win_rect->top    = (__s32)(screct->top + pLayer->lyrpara.src_win.y
                                   - pLayer->lyrpara.scn_win.y);
        win_rect->right  = (__s32)(win_rect->left  + screct->right - screct->left);

        /*屏幕右坐标超出图层之外*/
        if (win_rect->right >= (__s32)(pLayer->lyrpara.scn_win.x + pLayer->lyrpara.scn_win.width))
        {
            win_rect->right = (__s32)(pLayer->lyrpara.scn_win.x + pLayer->lyrpara.scn_win.width);
        }

        win_rect->bottom = (__s32)(win_rect->top + screct->bottom - screct->top);

        /*屏幕下坐标超出图层之外*/
        if (win_rect->bottom >= (__s32)(pLayer->lyrpara.scn_win.y + pLayer->lyrpara.scn_win.height))
        {
            win_rect->bottom = (__s32)(pLayer->lyrpara.scn_win.y + pLayer->lyrpara.scn_win.height);
        }
    }
    else
    {
        win_rect->left        = 0;
        win_rect->right       = 0;
        win_rect->top         = 0;
        win_rect->bottom      = 0;
    }

    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_WidgetToScreen
*
* author:
*
* date:             2009-8-4
*
* Description:      widget类的窗口区域转换成屏幕区域
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_WidgetToScreen(H_WIN hWnd, __gui_rect_t *screct)
{
    __gui_win_t         *Parent;
    __gui_widget_t      *pWidget = (__gui_widget_t *)GUI_WinH2P(hWnd);
    __gui_lyrwin_t      *pLayer;
#if GUI_CHECK_ARG_EN > 0

    if ((!hWnd) || (!screct))
    {
        ORANGE_WRN("input parameter empty!\n");
        return ;
    }

#endif
    pLayer = pWidget->base_win.LyrWin;

    if (pLayer->status == GUI_LYRWIN_STA_ON)
    {
        screct->left   = (__s32)(pWidget->base_win.win_rect.left - pLayer->lyrpara.src_win.x
                                 + pLayer->lyrpara.scn_win.x);
        Parent = pWidget->base_win.Parent;

        while (Parent->WinType != TYPE_MANWIN)
        {
            screct->left += (__s32)(Parent->client_rect.left);
        }

        /*屏幕左坐标超出图层显示范围*/
        if (screct->left >= (__s32)(pLayer->lyrpara.scn_win.x + pLayer->lyrpara.scn_win.width))
        {
            screct->left    = 0;
            screct->right   = 0;
            screct->bottom  = 0;
            screct->top     = 0;
            return ;
        }

        screct->top    = (__s32)(pWidget->base_win.win_rect.top - pLayer->lyrpara.src_win.y
                                 + pLayer->lyrpara.scn_win.y);

        while (Parent->WinType != TYPE_MANWIN)
        {
            screct->top += (__s32)(Parent->client_rect.top);
        }

        /*屏幕上坐标超出图层之外*/
        if (screct->top >= (__s32)(pLayer->lyrpara.scn_win.y + pLayer->lyrpara.scn_win.height))
        {
            screct->left    = 0;
            screct->right   = 0;
            screct->bottom  = 0;
            screct->top     = 0;
            return ;
        }

        screct->right  = (__s32)(screct->left  + pWidget->base_win.win_rect.right -
                                 pWidget->base_win.win_rect.left);

        /*屏幕右坐标超出图层之外*/
        if (screct->right >= (__s32)(pLayer->lyrpara.scn_win.x + pLayer->lyrpara.scn_win.width))
        {
            screct->right = (__s32)(pLayer->lyrpara.scn_win.x + pLayer->lyrpara.scn_win.width);
        }

        screct->bottom = (__s32)(screct->top + pWidget->base_win.win_rect.bottom -
                                 pWidget->base_win.win_rect.top);

        /*屏幕下坐标超出图层之外*/
        if (screct->bottom >= (__s32)(pLayer->lyrpara.scn_win.y + pLayer->lyrpara.scn_win.height))
        {
            screct->bottom = (__s32)(pLayer->lyrpara.scn_win.y + pLayer->lyrpara.scn_win.height);
        }
    }
    else
    {
        screct->left        = 0;
        screct->right       = 0;
        screct->top         = 0;
        screct->bottom      = 0;
    }

    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_ClentToScreen
*
* author:
*
* date:             2009-8-4
*
* Description:      窗口的客户区域转换成屏幕的矩形区域
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_ClientToScreen(H_WIN hWnd, __pos_t *client_pos, __pos_t *sc_pos)
{
    __gui_win_t  *pWin      = GUI_WinH2P(hWnd);
    __gui_lyrwin_t *pLayer   = pWin->LyrWin;
    __gui_win_t  *Parent    = pWin->Parent;
    __pos_t       tmp_pos;
#if GUI_CHECK_ARG_EN > 0

    if ((!hWnd) || (!client_pos) || (!sc_pos))
    {
        ORANGE_WRN("input parameter empty!\n");
        return ;
    }

#endif
    tmp_pos.x   = (__s32)(client_pos->x + pWin->client_rect.left);
    tmp_pos.y   = (__s32)(client_pos->y + pWin->client_rect.top);

    /*转换窗口客户区坐标至FB坐标*/
    while (Parent->WinType != TYPE_MANWIN)
    {
        tmp_pos.y += (__s32)(Parent->client_rect.top);
        tmp_pos.x += (__s32)(Parent->client_rect.left);
        Parent = Parent->Parent;
    }

    /*转换FB坐标至屏幕坐标*/
    sc_pos->x = (__s32)(tmp_pos.x - pLayer->lyrpara.src_win.x + pLayer->lyrpara.scn_win.x);

    if (sc_pos->x >= (__s32)(pLayer->lyrpara.scn_win.x + pLayer->lyrpara.scn_win.width))
    {
        sc_pos->x = GUI_OVERWIDTH;
        sc_pos->y = GUI_OVERHEIGHT;
        return ;
    }

    sc_pos->y = (__s32)(tmp_pos.y - pLayer->lyrpara.src_win.y + pLayer->lyrpara.scn_win.y);

    if (sc_pos->y >= (__s32)(pLayer->lyrpara.scn_win.y + pLayer->lyrpara.scn_win.height))
    {
        sc_pos->x = GUI_OVERWIDTH;
        sc_pos->y = GUI_OVERHEIGHT;
        return ;
    }

    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_ClentToScreen
*
* author:
*
* date:             2009-8-4
*
* Description:      屏幕的坐标点转换成窗口的客户区坐标点
*                   前提条件是该坐标点位于窗口之内
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
void GUI_ScreenToClient(H_WIN hWnd, __pos_t *sc_pos, __pos_t *client_pos)
{
    __gui_win_t  *pWin      = GUI_WinH2P(hWnd);
    __gui_lyrwin_t *pLayer   = pWin->LyrWin;
    __gui_win_t  *Parent    = pWin->Parent;
    __pos_t       tmp_pos;
#if GUI_CHECK_ARG_EN > 0

    if ((!hWnd) || (!client_pos) || (!sc_pos))
    {
        ORANGE_WRN("input parameter empty!\n");
        return ;
    }

#endif
    /*转换屏幕坐标至FB坐标*/
    tmp_pos.x = (__s32)(sc_pos->x + pLayer->lyrpara.src_win.x - pLayer->lyrpara.scn_win.x);
    tmp_pos.y = (__s32)(sc_pos->y + pLayer->lyrpara.src_win.y - pLayer->lyrpara.scn_win.y);
    client_pos->x   = (__s32)(tmp_pos.x - pWin->client_rect.left);
    client_pos->y   = (__s32)(tmp_pos.x - pWin->client_rect.top);

    /*转换FB坐标至窗口客户区坐标*/
    while (Parent->WinType != TYPE_MANWIN)
    {
        client_pos->x -= (__s32)(Parent->client_rect.left);
        client_pos->y -= (__s32)(Parent->client_rect.top);
        Parent = Parent->Parent;
    }

    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_ClentToScreen
*
* author:
*
* date:             2009-8-4
*
* Description:      窗口的坐标转换成屏幕的矩形区域
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_WindowToScreen(H_WIN hWnd, __pos_t *win_pos, __pos_t *sc_pos)
{
    __gui_win_t  *pWin      = GUI_WinH2P(hWnd);
    __gui_lyrwin_t *pLayer   = pWin->LyrWin;
    __gui_win_t  *Parent    = pWin->Parent;
    __pos_t       tmp_pos;
#if GUI_CHECK_ARG_EN > 0

    if ((!hWnd) || (!win_pos) || (!sc_pos))
    {
        ORANGE_WRN("input parameter empty!\n");
        return ;
    }

#endif
    tmp_pos.x   = (__s32)(win_pos->x + pWin->win_rect.left);
    tmp_pos.y   = (__s32)(win_pos->y + pWin->win_rect.top);

    /*转换窗口客户区坐标至FB坐标*/
    while (Parent->WinType != TYPE_MANWIN)
    {
        tmp_pos.y += (__s32)(Parent->win_rect.top);
        tmp_pos.x += (__s32)(Parent->win_rect.left);
        Parent = Parent->Parent;
    }

    /*转换FB坐标至屏幕坐标*/
    sc_pos->x = (__s32)(tmp_pos.x - pLayer->lyrpara.src_win.x + pLayer->lyrpara.scn_win.x);
    sc_pos->y = (__s32)(tmp_pos.y - pLayer->lyrpara.src_win.y + pLayer->lyrpara.scn_win.y);
    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_ClentToScreen
*
* author:
*
* date:             2009-8-4
*
* Description:      屏幕的坐标点转换成窗口的坐标点
*                   前提条件是该坐标点位于窗口之内
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_ScreenToWindow(H_WIN hWnd, __pos_t *sc_pos, __pos_t *win_pos)
{
    __gui_win_t  *pWin      = GUI_WinH2P(hWnd);
    __gui_lyrwin_t *pLayer   = pWin->LyrWin;
    __gui_win_t  *Parent    = pWin->Parent;
    __pos_t       tmp_pos;
#if GUI_CHECK_ARG_EN > 0

    if ((!hWnd) || (!win_pos) || (!sc_pos))
    {
        ORANGE_WRN("input parameter empty!\n");
        return ;
    }

#endif
    /*转换屏幕坐标至FB坐标*/
    tmp_pos.x = (__s32)(sc_pos->x + pLayer->lyrpara.src_win.x - pLayer->lyrpara.scn_win.x);
    tmp_pos.y = (__s32)(sc_pos->y + pLayer->lyrpara.src_win.y - pLayer->lyrpara.scn_win.y);
    win_pos->x  = (__s32)(tmp_pos.x - pWin->win_rect.left);
    win_pos->y  = (__s32)(tmp_pos.y - pWin->win_rect.top);

    /*转换FB坐标至窗口坐标*/
    while (Parent->WinType != TYPE_MANWIN)
    {
        win_pos->x -= (__s32)(Parent->win_rect.left);
        win_pos->y -= (__s32)(Parent->win_rect.top);
        Parent = Parent->Parent;
    }

    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_ClentToScreen
*
* author:
*
* date:             2009-8-4
*
* Description:      窗口的坐标转换成屏幕的矩形区域
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_FBToScreen(H_WIN hWnd, __pos_t *fb_pos, __pos_t *sc_pos)
{
    __gui_win_t  *pWin      = GUI_WinH2P(hWnd);
    //__gui_lyrwin_t*pLayer = pWin->LyrWin;
    __gui_win_t  *Parent    = pWin->Parent;
#if GUI_CHECK_ARG_EN > 0

    if ((!hWnd) || (!fb_pos) || (!sc_pos))
    {
        ORANGE_WRN("input parameter empty!\n");
        return ;
    }

#endif
    sc_pos->x   = (__s32)(fb_pos->x + pWin->win_rect.left);
    sc_pos->y   = (__s32)(fb_pos->y + pWin->win_rect.top);

    /*转换窗口客户区坐标至FB坐标*/
    while (Parent->WinType != TYPE_MANWIN)
    {
        sc_pos->y += (__s32)(Parent->win_rect.top);
        sc_pos->x += (__s32)(Parent->win_rect.left);
        Parent = Parent->Parent;
    }

    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_ClentToScreen
*
* author:
*
* date:             2009-8-4
*
* Description:      屏幕的坐标点转换成窗口的坐标点
*                   前提条件是该坐标点位于窗口之内
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_ScreenToFB(H_WIN hWnd, __pos_t *sc_pos, __pos_t *fb_pos)
{
    __gui_win_t  *pWin      = GUI_WinH2P(hWnd);
    __gui_lyrwin_t *pLayer   = pWin->LyrWin;
    //  __gui_win_t  *Parent    = pWin->Parent;
#if GUI_CHECK_ARG_EN > 0

    if ((!hWnd) || (!fb_pos) || (!sc_pos))
    {
        ORANGE_WRN("input parameter empty!\n");
        return ;
    }

#endif
    /*转换屏幕坐标至FB坐标*/
    fb_pos->x = (__s32)(sc_pos->x + pLayer->lyrpara.src_win.x - pLayer->lyrpara.scn_win.x);
    fb_pos->y = (__s32)(sc_pos->y + pLayer->lyrpara.src_win.y - pLayer->lyrpara.scn_win.y);
    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetClientFBRect
*
* author:
*
* date:             2009-8-14
*
* Description:      获取GUI客户区在FB中的矩形区域
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_WinGetClientFBRect(H_WIN hWnd, RECT *fb_rect)
{
    __gui_win_t  *pWin      = GUI_WinH2P(hWnd);
    __gui_win_t  *Parent    = pWin->Parent;
#if GUI_CHECK_ARG_EN > 0

    if ((!hWnd) || (!fb_rect))
    {
        ORANGE_WRN("input parameter empty!\n");
        return ;
    }

#endif
    fb_rect->x      = (__s32)(pWin->client_rect.left);
    fb_rect->y      = (__s32)(pWin->client_rect.top);
    fb_rect->width  = (__u32)(pWin->client_rect.right - pWin->client_rect.left + 1);
    fb_rect->height = (__u32)(pWin->client_rect.bottom - pWin->client_rect.top + 1);

    while (Parent->WinType != TYPE_MANWIN)
    {
        fb_rect->x = (__s32)(fb_rect->x + Parent->client_rect.left);
        fb_rect->y = (__s32)(fb_rect->y + Parent->client_rect.top);
        Parent = Parent->Parent;
    }

    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_WinFBToClient
*
* author:
*
* date:             2009-9-16
*
* Description:      framebuffer区域转换成窗口的客户区域
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_WinFBToClient(H_WIN hWnd, const __gui_rect_t *rcFB, __gui_rect_t *rcClient)
{
    __gui_win_t *pParent;
    __gui_win_t *pWin = GUI_WinH2P(hWnd);
#if GUI_CHECK_ARG_EN > 0

    if ((!hWnd) || (!rcFB) || (!rcClient))
    {
        ORANGE_WRN("input parameter empty!\n");
        return ;
    }

#endif

    if (pWin->DataType != TYPE_HWND)
    {
        ORANGE_WRN("input win data error!\n");
        rcClient->top       = 0;
        rcClient->left      = 0;
        rcClient->right     = 0;
        rcClient->bottom    = 0;
        return ;
    }

    if (pWin->WinType == TYPE_MANWIN)
    {
        ORANGE_WRN("management win have no client!\n");
        rcClient->top       = 0;
        rcClient->left      = 0;
        rcClient->right     = 0;
        rcClient->bottom    = 0;
        return ;
    }

    rcClient->top       = (__s32)(rcFB->top     - pWin->client_rect.top);
    rcClient->left      = (__s32)(rcFB->left    - pWin->client_rect.left);
    rcClient->right     = (__s32)(rcFB->right   - pWin->client_rect.left);
    rcClient->bottom    = (__s32)(rcFB->bottom  - pWin->client_rect.top);
    pParent = pWin;

    while (pParent->WinType != TYPE_MANWIN)
    {
        rcClient->top       -= (__s32)(pParent->client_rect.top);
        rcClient->left      -= (__s32)(pParent->client_rect.left);
        rcClient->right     -= (__s32)(pParent->client_rect.left);
        rcClient->bottom    -= (__s32)(pParent->client_rect.top);
        pParent = pParent->Parent;
    }
}

/*
**********************************************************************************************************************
*                                               GUI_WinGetWinFBRect
*
* author:
*
* date:             2009-8-14
*
* Description:      获取GUI窗口在FB中的矩形区域
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_WinGetWinFBRect(H_WIN hWnd, RECT *fb_rect)
{
    __gui_win_t  *pWin      = GUI_WinH2P(hWnd);
    __gui_win_t  *Parent    = pWin->Parent;
#if GUI_CHECK_ARG_EN > 0

    if ((!hWnd) || (!fb_rect))
    {
        ORANGE_WRN("input parameter empty!\n");
        return ;
    }

#endif
    fb_rect->x      = (__s32)(pWin->win_rect.left);
    fb_rect->y      = (__s32)(pWin->win_rect.top);
    fb_rect->width  = (__u32)(pWin->win_rect.right - pWin->win_rect.left + 1);
    fb_rect->height = (__u32)(pWin->win_rect.bottom - pWin->win_rect.top + 1);

    while (Parent->WinType != TYPE_MANWIN)
    {
        fb_rect->x = (__s32)(fb_rect->x + Parent->win_rect.left);
        fb_rect->y = (__s32)(fb_rect->y + Parent->win_rect.top);
        Parent = Parent->Parent;
    }

    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_WinCreateAsChild
*
* author:
*
* date:             2009-8-1
*
* Description:      创建创建通用窗口
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN GUI_WinCreate(__gui_win_t *create_info, __u32 extNums)
{
    __gui_win_t     *pWin;
    pWin  = (__gui_win_t *)orange_malloc(sizeof(__gui_win_t) + extNums);

    if (!pWin)
    {
        ORANGE_WRN("malloc win handle failed!\n");
        return HWND_INVALID;
    }

    orange_memset(pWin, 0, sizeof(__gui_win_t) + extNums);
    /*初始化Win信息结构*/
    pWin->ActiveChild       = create_info->ActiveChild;
    pWin->bkColor           = create_info->bkColor;
    pWin->Next              = create_info->Next;
    pWin->Prev              = create_info->Prev;
    pWin->win_rect          = create_info->win_rect;
    pWin->client_rect       = create_info->client_rect;
    pWin->DataType          = create_info->DataType;
    pWin->dwAddData         = create_info->dwAddData;
    pWin->dwExStyle         = create_info->dwExStyle;
    pWin->dwStyle           = create_info->dwStyle;
    pWin->FirstChild        = create_info->FirstChild;
    pWin->GCRInfo           = create_info->GCRInfo;
    pWin->hscroll           = create_info->hscroll;
    pWin->id                = create_info->id;
    pWin->InvRgn            = create_info->InvRgn;
    pWin->LyrWin            = create_info->LyrWin;
    pWin->NotifProc         = create_info->NotifProc;
    pWin->OldUnderPointer   = create_info->OldUnderPointer;
    pWin->Parent            = create_info->Parent;
    pWin->pGCRInfo          = &pWin->GCRInfo;
    pWin->pLogFont          = create_info->pLogFont;
    pWin->Primitive         = create_info->Primitive;
    pWin->pZOrderNode       = create_info->pZOrderNode;
    pWin->vscroll           = create_info->vscroll;
    pWin->WindowProc        = create_info->WindowProc;
    pWin->WinType           = create_info->WinType;
    pWin->Next              = NULL;
    /*从父窗口链表中删除该窗口*/
    GUI_WinLock();

    if (pWin->pZOrderNode)
    {
        pWin->pZOrderNode->hWnd = GUI_WinP2H(pWin);
        pWin->pZOrderNode->pNext = NULL;
        Win_AttachToZorder(GUI_WinP2H(pWin));
    }

    // Scroll bar
    if (pWin->dwStyle && WS_VSCROLL)
    {
        pWin->vscroll.minPos = 0;
        pWin->vscroll.maxPos = 100;
        pWin->vscroll.curPos = 0;
        pWin->vscroll.pageStep = 0;
        pWin->vscroll.barStart = 0;
        pWin->vscroll.barLen = 10;
        pWin->vscroll.status = SBS_NORMAL;
    }
    else
    {
        pWin->vscroll.status = SBS_HIDE | SBS_DISABLED;
    }

    if (pWin->dwStyle && WS_HSCROLL)
    {
        pWin->hscroll.minPos = 0;
        pWin->hscroll.maxPos = 100;
        pWin->hscroll.curPos = 0;
        pWin->hscroll.pageStep = 0;
        pWin->hscroll.barStart = 0;
        pWin->hscroll.barLen = 10;
        pWin->hscroll.status = SBS_NORMAL;
    }
    else
    {
        pWin->hscroll.status = SBS_HIDE | SBS_DISABLED;
    }

    Win_AttachToParent(pWin);
    GUI_WinUnlock();
    orange_memcpy(pWin->name, create_info->name, GUI_NAME_MAX);
    return (H_WIN)pWin;
}

/*
**********************************************************************************************************************
*                                               GUI_WinDelete
*
* author:
*
* date:             2009-8-2
*
* Description:      删除通用窗口
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_WinDelete(H_WIN hWnd)
{
    __gui_win_t                 *pWin;
    __gui_win_t                 *p_firstchild;
    // __gui_win_t                 *p_parent;

    if (!hWnd)
    {
        ORANGE_WRN("Win handle can not be null!\n");
        return ORANGE_FAIL;
    }

    pWin = GUI_WinH2P(hWnd);

    if (pWin->DataType != TYPE_HWND)
    {
        ORANGE_WRN("Win handle error!\n");
        return ORANGE_FAIL;
    }

    /*notify the windows delete the owner resource*/
    {
        __gui_msg_t  new_msg;
        new_msg.h_deswin    = hWnd;
        new_msg.h_srcwin    = 0;
        new_msg.id          = GUI_MSG_DESTROY;
        new_msg.dwAddData1  = 0;
        new_msg.dwAddData2  = 0;
        new_msg.p_arg       = NULL;

        if (GUI_SendMessage(&new_msg))
        {
            return ORANGE_FAIL;
        }
    }

    if (pWin->WinType == TYPE_MANWIN || pWin->WinType == TYPE_FRAMEWIN)
    {
        __gui_win_t     *head;
        __gui_win_t     *next;
        __gui_manwin_t *pMain = (__gui_manwin_t *)pWin;
        __gui_frmwin_t  *pFrm  = (__gui_frmwin_t *)pWin;

        // 删除宿窗口.
        if (pWin->WinType == TYPE_MANWIN)
        {
            head = pMain->pFirstHosted;
        }
        else
        {
            head = pFrm->pFirstHosted;
        }

        while (head)
        {
            if (pWin->WinType == TYPE_MANWIN)
            {
                next = ((__gui_manwin_t *)head)->pNextHosted;
            }
            else
            {
                next = ((__gui_frmwin_t *)head)->pNextHosted;
            }

            if (pWin->WinType == TYPE_MANWIN)
            {
#ifdef CONFIG_GUI_WIN_MANWIN_EN

                if (GUI_ManWinDelete(GUI_WinP2H(head)))
                {
                    return ORANGE_FAIL;
                }

#endif
            }
            else
            {
#ifdef CONFIG_GUI_WIN_FRMWIN_EN

                if (GUI_FrmWinDelete(GUI_WinP2H(head)))
                {
                    return ORANGE_FAIL;
                }

#endif
            }

            head = next;
        }

        if (pWin->WinType == TYPE_MANWIN)
        {
            if (pMain->pHosting)
            {
                GUI_WinRemoveHostedWin(pMain->pHosting, pWin);
            }
        }
        else
        {
            if (pFrm->pHosting)
            {
                GUI_WinRemoveHostedWin(pFrm->pHosting, pWin);
            }
        }
    }

    /* delete child win                             */
    while (1)
    {
        p_firstchild = pWin->FirstChild;

        /* if child list is empty, no win need be delete                                                            */
        if (!p_firstchild)
        {
            break;
        }

        if (p_firstchild->WinType == TYPE_MANWIN)
        {
#ifdef CONFIG_GUI_WIN_MANWIN_EN

            if (GUI_ManWinDelete(GUI_WinP2H(p_firstchild)) != ORANGE_OK)
            {
                ORANGE_WRN("Not all child window can be deleted!\n");
                //GUI_WinUnlock();
                return EPDK_FAIL;
            }

#endif
        }
        else if (p_firstchild->WinType == TYPE_FRAMEWIN)
        {
#ifdef CONFIG_GUI_WIN_FRMWIN_EN

            if (GUI_FrmWinDelete(GUI_WinP2H(p_firstchild)) != ORANGE_OK)
            {
                ORANGE_WRN("Not all child window can be deleted!\n");
                //GUI_WinUnlock();
                return EPDK_FAIL;
            }

#endif
        }
        else if (p_firstchild->WinType == TYPE_CONTROL)
        {
#ifdef CONFIG_GUI_WIN_CTRLWIN_EN

            if (GUI_CtrlWinDelete(GUI_WinP2H(p_firstchild)) != ORANGE_OK)
            {
                ORANGE_WRN("Not all child window can be deleted!\n");
                //GUI_WinUnlock();
                return EPDK_FAIL;
            }

#endif
        }
    }

    /*如果该子窗口为焦点子窗口*/
    /* if (pWin->Parent->ActiveChild == pWin)
     {
         pWin->Parent->ActiveChild = NULL;
     } */
    if (capture_wnd == GUI_WinP2H(pWin))
    {
        capture_wnd = NULL;
    }

    /*发消息通知父窗口，重新设置焦点主窗口*/
    {
        __gui_msg_t     newmsg;
        /* notify parent the child is deleted           */
        newmsg.id          = GUI_MSG_NOTIFY_CHILD_DELETED;
        newmsg.h_srcwin    = GUI_WinP2H(pWin);
        newmsg.h_deswin    = GUI_WinP2H(pWin->Parent);
        //GUI_SendNotifyMessage(&newmsg);
        GUI_SendMessage(&newmsg);
    }
    /*从父窗口链表中删除该窗口*/
    GUI_WinLock();

    if (pWin->WinType != TYPE_MANWIN)
    {
        Win_DeattachFromParent(pWin);
        Win_DeattachFromZorder(pWin);

        if (pWin->pZOrderNode)
        {
            /*free win z order node*/
            orange_mfree(pWin->pZOrderNode);
        }
    }
    else
    {
        __gui_manwin_t  *pManWin = (__gui_manwin_t *)pWin;

        if (pManWin->ManWinType != TYPE_ROOTMAIN)
        {
            Win_DeattachFromParent(pWin);
        }
    }

    GUI_WinUnlock();

    /*删除消息队列*/
    if (pWin->WinType == TYPE_CONTROL)
    {
        /*扔掉消息队列中与该窗口相关的消息*/
        GUI_ThrowAwayMessages(hWnd);
        /* free p_win memory                            */
        orange_mfree((void *)pWin);
    }

    return ORANGE_OK;
}
