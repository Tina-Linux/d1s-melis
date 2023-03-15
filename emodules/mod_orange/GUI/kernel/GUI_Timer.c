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
#ifndef __GUI_TIMER_C__
#define __GUI_TIMER_C__

#include "GUI_Timer.h"
#include "message_e.h"

__u32   gui_timer_counter = 0;
static  __gui_timer_t   *gtimer[DEF_NR_TIMERS];
static  pthread_mutex_t timerLock;

#define TIMER_LOCK()   pthread_mutex_lock(timerLock)
#define TIMER_UNLOCK() pthread_mutex_unlock(timerLock)

/*
**********************************************************************************************************************
*                                               Timer_SetMsgQueueFlag
*
* author:
*
* date:             2009-11-2
*
* Description:      通知消息队列timer标识符
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static void Timer_SetMsgQFlag(__win_msgqueue_t *pMsgQueue, __u32 slot)
{
    pMsgQueue->TimerMask |= (0x01 << slot);
    GUI_NotifyMSGQ(pMsgQueue);
}

/*
**********************************************************************************************************************
*                                               Timer_RemoveMsgQueueFlag
*
* author:
*
* date:             2009-11-2
*
* Description:      timer removemsgqueueflag
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static void Timer_RemoveMsgQFlag(__win_msgqueue_t *pMsgQueue, __u32 slot)
{
    pMsgQueue->TimerMask &= ~(0x01 << slot);
}

/*
**********************************************************************************************************************
*                                               GUI_DispatchTimerMessage
*
* author:
*
* date:             2009-11-2
*
* Description:      gui dispatchtimermessage
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_DispatchTimerMessage(__u32 inter)
{
    __u32 i;
    TIMER_LOCK();

    for (i = 0; i < DEF_NR_TIMERS; i++)
    {
        if (gtimer[i] && gtimer[i]->msg_queue)
        {
            /*更新timer的count*/
            gtimer[i]->count += inter;

            /*timer过期处理*/
            if (gtimer[i]->count >= gtimer[i]->speed)
            {
                if (gtimer[i]->tick_count == 0)
                {
                    gtimer[i]->tick_count = gui_timer_counter;
                }

                Timer_SetMsgQFlag(gtimer[i]->msg_queue, i);
                gtimer[i]->expired++;
                gtimer[i]->count = gtimer[i]->count - gtimer[i]->speed;
            }
        }
    }

    TIMER_UNLOCK();
}

/*
**********************************************************************************************************************
*                                               __mg_timer_action
*
* author:
*
* date:             2009-11-2
*
* Description:      timer时间到来之后的执行函数
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static void gui_timer_action(void *data)
{
    gui_timer_counter ++;
    GUI_DispatchTimerMessage(1);
}

/*
**********************************************************************************************************************
*                                               InitTimer
*
* author:
*
* date:             2009-11-2
*
* Description:      inittimer
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool InitTimer(void)
{
    pthread_mutex_init(&timerLock, (void *)1);
    gui_timer_counter = 0;
    return ORANGE_TRUE;
}

#if(GUI_WIN_OS == GUI_OS_EPOS)

#if 0
/*创建系统timer*/
static __krnl_stmr_t *gui_timer = NULL;

/*
**********************************************************************************************************************
*                                               __mg_timer_init
*
* author:
*
* date:             2009-11-2
*
* Description:        mg timer init
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 gui_timer_init(void)
{
    if (!InitTimer())
    {
        return ORANGE_FAIL;
    }

    gui_timer = esKRNL_TmrCreate(DEF_TIMER_PEROID, OS_TMR_OPT_PERIODIC,
                                 (OS_TMR_CALLBACK)gui_timer_action, NULL);

    if (gui_timer == NULL)
    {
        ORANGE_WRN("create timer fail!\n");
        return ORANGE_OK;
    }

    if (esKRNL_TmrStart(gui_timer) ==  EPDK_FALSE)
    {
        esKRNL_TmrDel(gui_timer);
        gui_timer = NULL;
        ORANGE_WRN("timer start fail!\n");
        return ORANGE_FAIL;
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               TerminateTimer
*
* author:
*
* date:             2009-11-2
*
* Description:      terminatetimer
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void TerminateTimer(void)
{
    __u32 i;

    if (gui_timer)
    {
        esKRNL_TmrDel(gui_timer);
        gui_timer = NULL;
    }

    for (i = 0; i < DEF_NR_TIMERS; i++)
    {
        if (gtimer[i] != NULL)
        {
            orange_mfree(gtimer[i]);
        }

        gtimer[i] = NULL;
    }

    pthread_mutex_destroy(timerLock);
}
#else
/*创建系统timer*/
static __u32  timer_task    = 0;

static void _os_timer_loop(void *p_arg)
{
    while (1)
    {
        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            break;
        }

        orange_Dalay(1);
        gui_timer_action(NULL);
    }
}

/*
**********************************************************************************************************************
*                                               __mg_timer_init
*
* author:
*
* date:             2009-11-2
*
* Description:        mg timer init
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 gui_timer_init(void)
{
    __u32 i;

    if (!InitTimer())
    {
        return ORANGE_FAIL;
    }

#ifdef SIM_PC_WIN
    __SAVE_MOD_EVN__
#endif
    timer_task = esKRNL_TCreate(_os_timer_loop, NULL, 0x1000, KRNL_priolevel1);
    esKRNL_TaskNameSet(timer_task, "OrangTmr");
#ifdef SIM_PC_WIN
    __RESTORE_MOD_EVN__
#endif

    if (timer_task == 0)
    {
        ORANGE_WRN("create timer fail!\n");
        return ORANGE_OK;
    }

    for (i = 0; i < DEF_NR_TIMERS; i++)
    {
        if (gtimer[i] != NULL)
        {
            orange_mfree(gtimer[i]);
        }

        gtimer[i] = NULL;
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               TerminateTimer
*
* author:
*
* date:             2009-11-2
*
* Description:      terminatetimer
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void TerminateTimer(void)
{
    __u32 i;

    if (timer_task)
    {
        while (1)
        {
            if (esKRNL_TDelReq(timer_task) == OS_TASK_NOT_EXIST)
            {
                break;
            }

            esKRNL_TimeDly(3);
        }

        timer_task = 0;
    }

    pthread_mutex_destroy(timerLock);
}
#endif
#else

/*
**********************************************************************************************************************
*                                               TerminateTimer
*
* author:
*
* date:             2009-11-2
*
* Description:      terminatetimer
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void TerminateTimer(void)
{
    return ;
}

#endif

/*
**********************************************************************************************************************
*                                               gui_get_timer
*
* author:
*
* date:             2009-11-2
*
* Description:      gui get timer
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__gui_timer_t *gui_get_timer(__s32 slot)
{
    __gui_timer_t *timer;
    TIMER_LOCK();

    if (slot < 0 || slot >= DEF_NR_TIMERS)
    {
        TIMER_UNLOCK();
        return NULL;
    }

    timer = gtimer[slot];
    TIMER_UNLOCK();
    return timer;
}

/*
**********************************************************************************************************************
*                                               gui_get_timer_slot
*
* author:
*
* date:             2009-11-2
*
* Description:      gui get timer slot
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 gui_get_timer_slot(H_WIN hWnd, __u16 id)
{
    __s32 i;
    __s32 slot = -1;
    TIMER_LOCK();

    for (i = 0; i < DEF_NR_TIMERS; i++)
    {
        if (gtimer[i] != NULL)
        {
            if (gtimer[i]->hWnd == hWnd && gtimer[i]->id == id)
            {
                slot = i;
                break;
            }
        }
    }

    TIMER_UNLOCK();
    return slot;
}

/*
**********************************************************************************************************************
*                                               __mg_remove_timer
*
* author:
*
* date:             2009-11-2
*
* Description:      gui remove timer
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void gui_remove_timer(__gui_timer_t *timer, __s32 slot)
{
    if (slot < 0 || slot >= DEF_NR_TIMERS)
    {
        return;
    }

    TIMER_LOCK();

    if (timer && timer->msg_queue && gtimer[slot] == timer)
    {
        /* The following code is already called in message.c...
         * timer->msg_queue->TimerMask &= ~(0x01 << slot);
         */
        orange_mfree(gtimer[slot]);
        gtimer [slot] = NULL;
    }

    TIMER_UNLOCK();
    return;
}

/*
**********************************************************************************************************************
*                                               gui_move_timer_last
*
* author:
*
* date:             2009-11-2
*
* Description:      gui move timer last
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void gui_move_timer_last(__gui_timer_t *timer, __s32 slot)
{
    if (slot < 0 || slot >= DEF_NR_TIMERS)
    {
        return;
    }

    TIMER_LOCK();

    if (timer && timer->msg_queue)
    {
        if (slot != (DEF_NR_TIMERS - 1))
        {
            __gui_timer_t *t;

            if (timer->msg_queue->TimerMask & (1 << (DEF_NR_TIMERS - 1)))
            {
                timer->msg_queue->TimerMask |= (1 << slot);
                timer->msg_queue->TimerMask &= ~(1 << (DEF_NR_TIMERS - 1));
            }

            t = gtimer [DEF_NR_TIMERS - 1];
            gtimer [DEF_NR_TIMERS - 1] = gtimer [slot];
            gtimer [slot] = t;
        }
    }

    TIMER_UNLOCK();
    return;
}

/*
**********************************************************************************************************************
*                                               GUI_SetTimer
*
* author:
*
* date:             2009-11-2
*
* Description:      gui settimer
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool GUI_SetTimer(H_WIN hWnd, __u16 id, __u32 speed,
                    __pGUI_TIMERPROC timer_proc)
{
    __u32               i;
    __win_msgqueue_t    *pMsgQueue;
    __s32               slot = -1;
    pMsgQueue = GUI_WinGetMsgQueue(hWnd);

    if (!pMsgQueue)
    {
        ORANGE_WRN("input windows handle error!\n");
        return ORANGE_FALSE;
    }

    TIMER_LOCK();

    /* Is there an empty timer slot? */
    for (i = 0; i < DEF_NR_TIMERS; i++)
    {
        if (gtimer[i] == NULL)
        {
            if (slot < 0)
            {
                slot = i;
            }
        }
        else if (gtimer[i]->hWnd == hWnd && gtimer[i]->id == id)
        {
            goto error;
        }
    }

    if (slot < 0 || slot == DEF_NR_TIMERS)
    {
        goto error ;
    }

    gtimer[slot] = orange_malloc(sizeof(__gui_timer_t));
    gtimer[slot]->speed       = speed;
    gtimer[slot]->hWnd        = hWnd;
    gtimer[slot]->id          = id;
    gtimer[slot]->count       = 0;
    gtimer[slot]->expired     = 0;

    if (timer_proc)
    {
        gtimer[slot]->proc    = (__pGUI_TIMERPROC)esKRNL_GetCallBack((__pCBK_t)timer_proc);
    }
    else
    {
        gtimer[slot]->proc    = NULL;
    }

    gtimer[slot]->tick_count  = 0;
    gtimer[slot]->msg_queue   = pMsgQueue;
    TIMER_UNLOCK();
    return ORANGE_TRUE;
error:
    TIMER_UNLOCK();
    return ORANGE_FALSE;
}


/*
**********************************************************************************************************************
*                                               GUI_KillTimer
*
* author:
*
* date:             2009-11-2
*
* Description:      gui killtimer
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
/* If id <= 0, clear all timers of the window */
__s32 GUI_KillTimer(H_WIN hWnd, __u16 id)
{
    __u32               i;
    __win_msgqueue_t    *pMsgQueue;
    //__s32               slot = -1;
    __bool              killed = 0;
    pMsgQueue = GUI_WinGetMsgQueue(hWnd);

    if (!pMsgQueue)
    {
        ORANGE_WRN("input windows handle error!\n");
        return ORANGE_FALSE;
    }

    TIMER_LOCK();

    for (i = 0; i < DEF_NR_TIMERS; i++)
    {
        if ((gtimer [i] && gtimer [i]->hWnd == hWnd) &&
            (id <= 0 || gtimer [i]->id == id))
        {
            Timer_RemoveMsgQFlag(pMsgQueue, i);
            orange_mfree(gtimer[i]);
            gtimer [i] = NULL;
            killed ++;

            if (id > 0)
            {
                break;
            }
        }
    }

    TIMER_UNLOCK();
    return killed;
}

/*
**********************************************************************************************************************
*                                               GUI_ResetTimer
*
* author:
*
* date:             2009-11-2
*
* Description:      gui resettimer
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool  GUI_ResetTimer(H_WIN hWnd, __u16 id, __u32 speed,
                       __pGUI_TIMERPROC timer_proc)
{
    __u32               i;
    __win_msgqueue_t    *pMsgQueue;
    //__s32               slot = -1;
    pMsgQueue = GUI_WinGetMsgQueue(hWnd);

    if (!pMsgQueue)
    {
        ORANGE_WRN("input windows handle error!\n");
        return ORANGE_FALSE;
    }

    TIMER_LOCK();

    for (i = 0; i < DEF_NR_TIMERS; i++)
    {
        if (gtimer[i] && gtimer[i]->hWnd == hWnd && gtimer[i]->id == id)
        {
            /* Should clear old timer flags */
            Timer_RemoveMsgQFlag(pMsgQueue, i);
            gtimer[i]->speed      = speed;
            gtimer[i]->count      = 0;
            gtimer[i]->expired    = 0;

            if (timer_proc)
            {
                gtimer[i]->proc    = (__pGUI_TIMERPROC)esKRNL_GetCallBack((__pCBK_t)timer_proc);
            }
            else
            {
                gtimer[i]->proc    = NULL;
            }

            gtimer[i]->tick_count = 0;
            TIMER_UNLOCK();
            return ORANGE_TRUE;
        }
    }

    TIMER_UNLOCK();
    return ORANGE_FALSE;
}

/*
**********************************************************************************************************************
*                                               GUI_IsTimerInstalled
*
* author:
*
* date:             2009-11-2
*
* Description:      gui istimerinstalled
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool GUI_IsTimerInstalled(H_WIN hWnd, __u16 id)
{
    __u32 i;
    TIMER_LOCK();

    for (i = 0; i < DEF_NR_TIMERS; i++)
    {
        if (gtimer[i] != NULL)
        {
            if (gtimer[i]->hWnd == hWnd && gtimer[i]->id == id)
            {
                TIMER_UNLOCK();
                return ORANGE_TRUE;
            }
        }
    }

    TIMER_UNLOCK();
    return ORANGE_FALSE;
}


/*
**********************************************************************************************************************
*                                               GUI_HaveFreeTimer
*
* author:
*
* date:             2009-11-2
*
* Description:      gui havefreetimer
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
/* no lock is ok */
__bool GUI_HaveFreeTimer(void)
{
    __u32 i;
    TIMER_LOCK();

    for (i = 0; i < DEF_NR_TIMERS; i++)
    {
        if (gtimer[i] == NULL)
        {
            TIMER_UNLOCK();
            return ORANGE_TRUE;
        }
    }

    TIMER_UNLOCK();
    return ORANGE_FALSE;
}

/*
**********************************************************************************************************************
*                                               GUI_GetTickCount
*
* author:
*
* date:             2009-11-2
*
* Description:      gui gettickcount
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 GUI_GetTickCount(void)
{
    return gui_timer_counter;
}

/*
**********************************************************************************************************************
*                                               GUI_GetTimerExpired
*
* author:
*
* date:             2009-12-9
*
* Description:      gui gettimerexpired
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 GUI_GetTimerExpired(H_WIN hWnd, __u16 id)
{
    __u32 i;
    TIMER_LOCK();

    for (i = 0; i < DEF_NR_TIMERS; i++)
    {
        if (gtimer[i] != NULL)
        {
            if (gtimer[i]->hWnd == hWnd && gtimer[i]->id == id)
            {
                TIMER_UNLOCK();
                return gtimer[i]->expired;
            }
        }
    }

    TIMER_UNLOCK();
    return 0;
}
#endif
