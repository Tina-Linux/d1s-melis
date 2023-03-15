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
