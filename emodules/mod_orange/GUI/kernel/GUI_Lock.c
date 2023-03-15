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
#ifndef __GUI_LOCK_C__
#define __GUI_LOCK_C__

#include "GUI_Lock.h"

/*
**********************************************************************************************************************
*                                               _GetTaskNo
*
* author:
*
* date:             2009-10-29
*
* Description:       gettaskno
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static int _GetTaskNo(__gui_nestlock_t *nestlock)
{
    int i;

    for (i = 0; i < GUI_MAXTASK; i++)
    {
        U32 TaskId = pthread_self();

        if (nestlock->TaskID[i] == TaskId)
        {
            return i;
        }

        if (nestlock->TaskID[i] == 0)
        {
            nestlock->TaskID[i] = TaskId;
            return i;
        }
    }

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
void GUI_NestUnlock(__gui_nestlock_t *nestlock)
{
#if GUI_CHECK_ARG_EN > 0

    if (!nestlock)
    {
        ORANGE_WRN("lock para error!\n");
        return ;
    }

#endif

    if (--nestlock->_EntranceCnt == 0)
    {
        pthread_mutex_unlock(nestlock->lock);
    }

    /* Test if _EntranceCnt is in reasonable range ... Not required in release builds */
    if (nestlock->_EntranceCnt < 0)
    {
        __err("GUI_LOCK.c: GUI_Unlock() _EntranceCnt underflow \n");
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
void GUI_NestLock(__gui_nestlock_t *nestlock, void *context)
{
#if GUI_CHECK_ARG_EN > 0

    if (!nestlock)
    {
        ORANGE_WRN("lock para error!\n");
        return ;
    }

#endif

    if (nestlock->_EntranceCnt == 0)
    {
        pthread_mutex_lock(nestlock->lock);
        nestlock->_TaskIDLock = pthread_self();            /* Save task ID */
    }
    else
    {
        if (nestlock->_TaskIDLock != pthread_self())
        {
            pthread_mutex_lock(nestlock->lock);
            nestlock->_TaskIDLock  = pthread_self();        /* Save task ID */
        }
    }

    nestlock->_EntranceCnt++;

    if (nestlock->_EntranceCnt == 1)
    {
        int TaskNo = _GetTaskNo(nestlock);

        if (TaskNo != nestlock->_CurrentTaskNo)
        {
            /* Save data of current task            */
            if (nestlock->_CurrentTaskNo >= 0)
            {
                /* Make sure _CurrentTaskNo is valid*/
                if (nestlock->ContextSize)
                {
                    orange_memcpy(nestlock->TaskContext[nestlock->_CurrentTaskNo], context, nestlock->ContextSize);
                    /* Load data of this task           */
                    orange_memcpy(context, nestlock->TaskContext[TaskNo], nestlock->ContextSize);
                }
            }

            nestlock->_CurrentTaskNo = TaskNo;
        }
    }

    /* Test if _EntranceCnt is in reasonable range ... Not required in release builds */
    if (nestlock->_EntranceCnt > GUI_MAX_LOCK_NESTING)
    {
        ORANGE_ERR("GUITASK.c: GUI_Lock() _EntranceCnt overflow \n");
    }
}
/*
**********************************************************************************************************************
*                                               GUI_NestLockInit
*
* author:
*
* date:             2009-10-29
*
* Description:      gui nestlockinit
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_NestLockInit(__gui_nestlock_t *nestlock)
{
    __u32   i;
#if GUI_CHECK_ARG_EN > 0

    if (!nestlock)
    {
        ORANGE_WRN("lock para error!\n");
        return ;
    }

#endif
    pthread_mutex_init(&nestlock->lock, (void *)KRNL_UNLOCKVAL);

    for (i = 0; i < GUI_MAXTASK; i++)
    {
        nestlock->TaskID[i] = 0;
    }

    nestlock->_CurrentTaskNo = -1;
    nestlock->_EntranceCnt   =  0;
    nestlock->_TaskIDLock    =  0;
    return ;
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
void GUI_NestLockExit(__gui_nestlock_t *nestlock)
{
    pthread_mutex_destroy(nestlock->lock);
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
__s32 GUI_NestLockAccept(__gui_nestlock_t *nestlock, void *context)
{
#if GUI_CHECK_ARG_EN > 0

    if (!nestlock)
    {
        ORANGE_WRN("lock para error!\n");
        return ORANGE_FAIL;
    }

#endif

    if (nestlock->_EntranceCnt == 0)
    {
        if (!pthread_mutex_accept(nestlock->lock))
        {
            return EPDK_FAIL;
        }

        nestlock->_TaskIDLock = pthread_self();            /* Save task ID */
    }
    else
    {
        if (nestlock->_TaskIDLock != pthread_self())
        {
            if (!pthread_mutex_accept(nestlock->lock))
            {
                return EPDK_FAIL;
            }

            nestlock->_TaskIDLock  = pthread_self();        /* Save task ID */
        }
    }

    nestlock->_EntranceCnt++;

    if (nestlock->_EntranceCnt == 1)
    {
        int TaskNo = _GetTaskNo(nestlock);

        if (TaskNo != nestlock->_CurrentTaskNo)
        {
            /* Save data of current task            */
            if (nestlock->_CurrentTaskNo >= 0)
            {
                /* Make sure _CurrentTaskNo is valid*/
                if (nestlock->ContextSize)
                {
                    orange_memcpy(nestlock->TaskContext[nestlock->_CurrentTaskNo], context, nestlock->ContextSize);
                    /* Load data of this task           */
                    orange_memcpy(context, nestlock->TaskContext[TaskNo], nestlock->ContextSize);
                }
            }

            nestlock->_CurrentTaskNo = TaskNo;
        }
    }

    /* Test if _EntranceCnt is in reasonable range ... Not required in release builds */
    if (nestlock->_EntranceCnt > GUI_MAX_LOCK_NESTING)
    {
        ORANGE_ERR("GUITASK.c: GUI_Lock() _EntranceCnt overflow ");
    }

    return EPDK_OK;
}
#endif
/*************************** End of file ****************************/
