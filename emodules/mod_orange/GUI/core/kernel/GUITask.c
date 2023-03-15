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
#include <stddef.h>           /* needed for definition of NULL */
#include "gui_core_kernel_private.h"

/*********************************************************************
*       Configuration defaults
**********************************************************************
*/

#ifndef GUI_MAXTASK
#define GUI_MAXTASK (32)
#endif

#ifndef GUI_MAX_LOCK_NESTING
#define GUI_MAX_LOCK_NESTING 48   /* If a lot of nested dialogs are used, this may not be sufficient. */
#endif

#if GUI_OS

/* static data */
static struct
{
    U32  TaskID;
    GUI_CONTEXT Context;
} _Save[GUI_MAXTASK];

static int _CurrentTaskNo = -1;
static int _EntranceCnt   =  0;
static U32 _TaskIDLock    =  0;

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
        U32 TaskId = GUI_X_GetTaskId();

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
void GUI_Unlock(void)
{
    if (--_EntranceCnt == 0)
    {
        GUI_X_Unlock();
    }

    /* Test if _EntranceCnt is in reasonable range ... Not required in release builds */
    if (_EntranceCnt < 0)
    {
        __err("GUITASK.c: GUI_Unlock() _EntranceCnt underflow \n");
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
void GUI_Lock(void)
{
    if (_EntranceCnt == 0)
    {
        GUI_X_Lock();
        _TaskIDLock = GUI_X_GetTaskId();            /* Save task ID */
    }
    else
    {
        if (_TaskIDLock != GUI_X_GetTaskId())
        {
            GUI_X_Lock();
            _TaskIDLock  = GUI_X_GetTaskId();        /* Save task ID */
        }
    }

    _EntranceCnt++;

    if (_EntranceCnt == 1)
    {
        int TaskNo = _GetTaskNo();

        if (TaskNo != _CurrentTaskNo)
        {
            /* Save data of current task            */
            if (_CurrentTaskNo >= 0)
            {
                /* Make sure _CurrentTaskNo is valid*/
                _Save[_CurrentTaskNo].Context = GUI_Context;
                /* Load data of this task           */
                GUI_Context = _Save[TaskNo].Context;
            }
            else
            {
                /* Make sure _CurrentTaskNo is valid*/
                _Save[TaskNo].Context = GUI_Context;
            }

            _CurrentTaskNo = TaskNo;
        }
    }

    /* Test if _EntranceCnt is in reasonable range ... Not required in release builds */
    if (_EntranceCnt > GUI_MAX_LOCK_NESTING)
    {
        __err("GUITASK.c: GUI_Lock() _EntranceCnt overflow \n");
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
__s32 GUI_LockAccept(void)
{
    if (_EntranceCnt == 0)
    {
        if (!GUI_X_LockAccept())
        {
            return EPDK_FAIL;
        }

        _TaskIDLock = GUI_X_GetTaskId();            /* Save task ID */
    }
    else
    {
        if (_TaskIDLock != GUI_X_GetTaskId())
        {
            if (!GUI_X_LockAccept())
            {
                return EPDK_FAIL;
            }

            _TaskIDLock  = GUI_X_GetTaskId();        /* Save task ID */
        }
    }

    _EntranceCnt++;

    if (_EntranceCnt == 1)
    {
        int TaskNo = _GetTaskNo();

        if (TaskNo != _CurrentTaskNo)
        {
            /* Save data of current task            */
            if (_CurrentTaskNo >= 0)
            {
                /* Make sure _CurrentTaskNo is valid*/
                _Save[_CurrentTaskNo].Context = GUI_Context;
                /* Load data of this task           */
                GUI_Context = _Save[TaskNo].Context;
            }
            else
            {
                /* Make sure _CurrentTaskNo is valid*/
                _Save[TaskNo].Context = GUI_Context;
            }

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
/*
**********************************************************************************************************************
*                                               GUITASK_Init
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
void GUITASK_Init(void)
{
    _CurrentTaskNo = -1;                            /* Invalidate                                                   */
    GUI_X_InitOS();
}

/*
**********************************************************************************************************************
*                                               GUITASK_CopyContext
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
void GUITASK_CopyContext(void)
{
    int i;

    for (i = 0; i < GUI_MAXTASK; i++)
    {
        _Save[i].Context = GUI_Context;
        _Save[i].TaskID  = 0;
    }
}


#if defined(WIN32) && defined(_DEBUG)

/*********************************************************************
*
*       GUITASK_AssertLock
*
*/
void GUITASK_AssertLock(void)
{
    if (!_EntranceCnt)
    {
        GUI_DEBUG_ERROROUT("GUITASK.c: GUITASK_AssertLock failed!");
    }
}

/*********************************************************************
*
*       GUITASK_AssertNoLock
*
*/
void GUITASK_AssertNoLock(void)
{
    GUI_Lock();

    if (_EntranceCnt > 1)
    {
        GUI_DEBUG_ERROROUT("GUITASK.c: GUITASK_AssertNoLock failed!");
    }

    GUI_Unlock();
}

#endif

#else

/*********************************************************************
*
*       Dummy Kernel routines
*
* The routines below are dummies in case configuration tells us not
* to use any kernel. In this case the routines below should
* not be required, but it can not hurt to have them. The linker
* will eliminate them anyhow.
*/
void GUI_Unlock(void);
void GUI_Unlock(void) {}
void GUI_Lock(void);
void GUI_Lock(void) {}
void GUITASK_Init(void);
void GUITASK_Init(void) {}
void GUITASK_StoreDefaultContext(void);
void GUITASK_StoreDefaultContext(void) {}

#endif

/*************************** End of file ****************************/
