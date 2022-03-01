/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : GUITask.c
*
* By      : Steven
* Version : v1.0
* Date    : 2008-11-25 19:07:41
* Note    : Saves/Restores task context with supported OSs. It also uses a resource semaphore.
**********************************************************************************************************************
*/

#include "win_i.h"

/*********************************************************************
*       Configuration defaults
**********************************************************************
*/

#ifndef GUI_MAXTASK
#define GUI_MAXTASK (16)
#endif

#ifndef GUI_MAX_LOCK_NESTING
#define GUI_MAX_LOCK_NESTING 48   /* If a lot of nested dialogs are used, this may not be sufficient. */
#endif

static  __krnl_event_t  *WinSem;
/* static data */
static struct
{
    U32  TaskID;
} _Save[GUI_MAXTASK];

static int _CurrentTaskNo = -1;
static int _EntranceCnt   =  0;
static U32 _TaskIDLock    =  0;

void  GUI_Win_InitLock(void)
{
    WinSem   = esKRNL_SemCreate(1);
}


/*
**********************************************************************************************************************
*                                               GUI_X_Lock
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

void  GUI_Win_Lock(void)
{
    INT8U  err;
    esKRNL_SemPend(WinSem, 0, &err);
}

/*
**********************************************************************************************************************
*                                               GUI_X_Unlock
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
void  GUI_Win_Unlock(void)
{
    esKRNL_SemPost(WinSem);
}
/*
**********************************************************************************************************************
*                                               GUI_X_LockAccept
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
__u16  GUI_Win_LockAccept(void)
{
    return esKRNL_SemAccept(WinSem);
}

/*
**********************************************************************************************************************
*                                               GUI_Win_GetTaskId
*
* author:
*
* date:             2009-7-24
*
* Description:      gui win gettaskid
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  GUI_Win_GetTaskId(void)
{
    __u32 tid;
    tid = esKRNL_GetTIDCur();
    return tid;
}

/* static */
/*
**********************************************************************************************************************
*                                               _GetTaskNo
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      : 先进先占有，剩下的就为0
*
**********************************************************************************************************************
*/
static int _GetTaskNo(void)
{
    int i;

    for (i = 0; i < GUI_MAXTASK; i++)
    {
        __u32 TaskId = GUI_Win_GetTaskId();

        if (_Save[i].TaskID == TaskId)
        {
            return i;
        }

        if (_Save[i].TaskID == 0)
        {
            _Save[i].TaskID = TaskId;
            return i;
        }
    }

    GUI_DEBUG_ERROROUT("No Context available for task ... (increase GUI_MAXTASK)");
    return 0;
}

/* global */
/*
**********************************************************************************************************************
*                                               GUI_Unlock
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
void GUI_WinUnlock(void)
{
    if (--_EntranceCnt == 0)
    {
        GUI_Win_Unlock();
    }

    /* Test if _EntranceCnt is in reasonable range ... Not required in release builds */
    if (_EntranceCnt < 0)
    {
        __err("GUITASK.c: GUI_WinUnlock() _EntranceCnt underflow \n");
    }
}

/*
**********************************************************************************************************************
*                                               GUI_Lock
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
void GUI_WinLock(void)
{
    if (_EntranceCnt == 0)
    {
        GUI_Win_Lock();
        _TaskIDLock = GUI_Win_GetTaskId();            /* Save task ID */
    }
    else
    {
        if (_TaskIDLock != GUI_Win_GetTaskId())
        {
            GUI_Win_Lock();
            _TaskIDLock  = GUI_Win_GetTaskId();        /* Save task ID */
        }
    }

    _EntranceCnt++;

    if (_EntranceCnt == 1)
    {
        __s32 TaskNo = _GetTaskNo();

        if (TaskNo != _CurrentTaskNo)
        {
            _CurrentTaskNo = TaskNo;
        }
    }

    /* Test if _EntranceCnt is in reasonable range ... Not required in release builds */
    if (_EntranceCnt > GUI_MAX_LOCK_NESTING)
    {
        __err("GUITASK.c: GUI_WinLock() _EntranceCnt overflow \n");
    }
}
/*
**********************************************************************************************************************
*                                               GUI_LockAccept
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
__s32 GUI_WinLockAccept(void)
{
    if (_EntranceCnt == 0)
    {
        if (!GUI_Win_LockAccept())
        {
            return EPDK_FAIL;
        }

        _TaskIDLock = GUI_Win_GetTaskId();            /* Save task ID */
    }
    else
    {
        if (_TaskIDLock != GUI_Win_GetTaskId())
        {
            if (!GUI_Win_LockAccept())
            {
                return EPDK_FAIL;
            }

            _TaskIDLock  = GUI_Win_GetTaskId();        /* Save task ID */
        }
    }

    _EntranceCnt++;

    if (_EntranceCnt == 1)
    {
        int TaskNo = _GetTaskNo();

        if (TaskNo != _CurrentTaskNo)
        {
            _CurrentTaskNo = TaskNo;
        }
    }

    /* Test if _EntranceCnt is in reasonable range ... Not required in release builds */
    if (_EntranceCnt > GUI_MAX_LOCK_NESTING)
    {
        __err("GUITASK.c: GUI_Lock() _EntranceCnt overflow ");
    }

    return EPDK_OK;
}
