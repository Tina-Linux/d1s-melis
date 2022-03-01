/*
*******************************************************************************************************************
*                                                           ePDK livetouch
*                                                               mod_orange
*
*                                            Copyright(C), 2006-2009, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    message.c
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2009-7-21
*
*Description :  gui message function implement
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>         <version>     <description>
*
* William Wang     2009-7-21         1.1.0          Create File
*
*******************************************************************************************************************
*/

#include "message_i.h"

static      mem_blocklink_t     qmsg_blk;


/*
**********************************************************************************************************************
*                                               InitFreeQMSGList
*
* author:
*
* date:             2009-11-2
*
* Description:      initfreeqmsglist
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
/* QMSG allocation */
__bool InitFreeQMSGList(void)
{
    InitMemBlockLink(&qmsg_blk, sizeof(__gui_qmsg_t), SIZE_QMSG_HEAP);
    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               DestroyFreeQMSGList
*
* author:
*
* date:             2009-11-2
*
* Description:      destroyfreeqmsglist
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool DestroyFreeQMSGList(void)
{
    DestroyMemBlockLink(&qmsg_blk);
    return ORANGE_FALSE;
}

/*
**********************************************************************************************************************
*                                               GUI_NotifyMSGQ
*
* author:
*
* date:             2009-11-2
*
* Description:      通知相应的消息队列，有消息来到
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_NotifyMSGQ(__win_msgqueue_t *qmsg)
{
    int   sem_value;
    sem_getvalue(&qmsg->wait, &sem_value);

    if (sem_value <= 0)
    {
        sem_post(&qmsg->wait);
    }
}

/*
**********************************************************************************************************************
*                                               GUI_IsLastThisMsg
*
* author:
*
* date:             2009-11-12
*
* Description:      判断是否是最后一个这样的消息
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __bool  GUI_IsLastThisMsg(__win_msgqueue_t   *pMsgQueue, __u32 id, __u32 childid)
{
    __gui_msg_t     tmp_msg;
    __u32           readpos;
    __u32           writepos;
    readpos     = pMsgQueue->readpos + 1;

    if (readpos >= pMsgQueue->len)
    {
        readpos = 0;
    }

    writepos    = pMsgQueue->writepos;

    while (readpos != writepos)
    {
        tmp_msg = pMsgQueue->pAsyncMsg[readpos];

        if ((tmp_msg.id == id) && (tmp_msg.dwAddData1 == childid))
        {
            return ORANGE_FALSE;
        }

        readpos++;

        if (readpos >= pMsgQueue->len)
        {
            readpos = 0;
        }
    }

    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               GUI_IsLastThisKeyMsg
*
* author:
*
* date:             2009-11-12
*
* Description:      判断是否是最后一个这样的消息
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __bool  GUI_IsLastThisKeyMsg(__win_msgqueue_t   *pMsgQueue, __u32 id, __u32 actionid)
{
    __gui_msg_t     tmp_msg;
    __u32           readpos;
    __u32           writepos;
    readpos     = pMsgQueue->readpos + 1;

    if (readpos >= pMsgQueue->len)
    {
        readpos = 0;
    }

    writepos    = pMsgQueue->writepos;

    while (readpos != writepos)
    {
        tmp_msg = pMsgQueue->pAsyncMsg[readpos];

        if ((tmp_msg.id == id) && (tmp_msg.dwAddData1 == actionid))
        {
            return ORANGE_FALSE;
        }

        readpos++;

        if (readpos >= pMsgQueue->len)
        {
            readpos = 0;
        }
    }

    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               GUI_ClearPostMsg
*
* author:
*
* date:             2009-11-20
*
* Description:      清楚消息队列消息
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_ClearPostMsg(__win_msgqueue_t   *pMsgQueue)
{
    pMsgQueue->readpos = pMsgQueue->writepos;
    pMsgQueue->dwState &= ~QS_POSTMSG;
    return ;
}


/*
**********************************************************************************************************************
*                                               GUI_ClearBeforeMsg
*
* author:
*
* date:             2009-11-20
*
* Description:      gui clearbeforemsg
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_ClearBeforeMsg(__win_msgqueue_t *pMsgQueue, __u32 id, __u32 childid)
{
    __u32           readpos;
    __u32           writepos;
    __gui_msg_t     tmp_msg;
    readpos     = pMsgQueue->readpos;
    writepos    = pMsgQueue->writepos - 1;

    while (writepos != readpos)
    {
        tmp_msg        = pMsgQueue->pAsyncMsg[writepos];
        tmp_msg.p_arg  = NULL;

        if (tmp_msg.id == id && tmp_msg.dwAddData1)
        {
            readpos++;
        }

        if (readpos >= pMsgQueue->len)
        {
            readpos = 0;
        }
    }
}
/*
**********************************************************************************************************************
*                                               GUI_ProcMsgRise
*
* author:
*
* date:             2009-11-12
*
* Description:      处理RISE消息，采样并取出最后一个RISE
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __bool  GUI_ProcMsgRise(__win_msgqueue_t   *pMsgQueue, __u32 id, __u32 childid, __gui_msg_t *msg, __bool first)
{
    __bool          last;
    __u32           readpos;
    __u32           writepos;
    readpos     = pMsgQueue->readpos + 1;

    if (readpos >= pMsgQueue->len)
    {
        readpos = 0;
    }

    writepos    = pMsgQueue->writepos;

    /*在找到最后一个RISE键时，发送第一个RISE消息给相应的目标窗口*/
    if (first == ORANGE_TRUE)
    {
        /*找到第一个RISE时，将消息队列里面的内容取出赋值给msg结构*/
        *msg        = pMsgQueue->pAsyncMsg[pMsgQueue->readpos];
        msg->p_arg  = NULL;
    }

    /*判断是否是该次采样的最后一个RISE键*/
    last        = GUI_IsLastThisMsg(pMsgQueue, id, childid);

    if (last == ORANGE_TRUE)
    {
        /*是最后一个RISE键，直接返回*/
        pMsgQueue->readpos++;

        if (pMsgQueue->readpos >= pMsgQueue->len)
        {
            pMsgQueue->readpos = 0;
        }

        //pthread_mutex_unlock (pMsgQueue->lock);
        return ORANGE_TRUE;
    }
    else
    {
        /*不是最后一个RISE键，则继续往下寻找最后一个RISE键*/
        pMsgQueue->readpos = readpos;

        while (readpos != writepos)
        {
            if ((pMsgQueue->pAsyncMsg[readpos].id == id) && (pMsgQueue->pAsyncMsg[readpos].dwAddData1 == childid))
            {
                return GUI_ProcMsgRise(pMsgQueue, id, childid, msg, ORANGE_FALSE);
            }

            readpos++;

            if (readpos >= pMsgQueue->len)
            {
                readpos = 0;
            }

            pMsgQueue->readpos = readpos;
        }
    }

    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               GUI_ProcMsgKeyRise
*
* author:
*
* date:             2009-11-12
*
* Description:      处理KEY_UP_ACTION消息，采样并取出最后一个KEY_UP_ACTION
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __bool  GUI_ProcMsgKeyRise(__win_msgqueue_t   *pMsgQueue, __u32 id, __u32 actionid, __gui_msg_t *msg, __bool first)
{
    __bool          last;
    __u32           readpos;
    __u32           writepos;
    readpos     = pMsgQueue->readpos + 1;

    if (readpos >= pMsgQueue->len)
    {
        readpos = 0;
    }

    writepos    = pMsgQueue->writepos;

    /*在找到最后一个RISE键时，发送第一个RISE消息给相应的目标窗口*/
    if (first == ORANGE_TRUE)
    {
        /*找到第一个RISE时，将消息队列里面的内容取出赋值给msg结构*/
        *msg        = pMsgQueue->pAsyncMsg[pMsgQueue->readpos];
        msg->p_arg  = NULL;
    }

    /*判断是否是该次采样的最后一个RISE键*/
    last        = GUI_IsLastThisKeyMsg(pMsgQueue, id, actionid);

    if (last == ORANGE_TRUE)
    {
        /*是最后一个RISE键，直接返回*/
        pMsgQueue->readpos++;

        if (pMsgQueue->readpos >= pMsgQueue->len)
        {
            pMsgQueue->readpos = 0;
        }

        //pthread_mutex_unlock (pMsgQueue->lock);
        return ORANGE_TRUE;
    }
    else
    {
        /*不是最后一个RISE键，则继续往下寻找最后一个RISE键*/
        pMsgQueue->readpos = readpos;

        while (readpos != writepos)
        {
            if ((pMsgQueue->pAsyncMsg[readpos].id == id) && (pMsgQueue->pAsyncMsg[readpos].dwAddData2 == actionid))
            {
                return GUI_ProcMsgKeyRise(pMsgQueue, id, actionid, msg, ORANGE_FALSE);
            }

            readpos++;

            if (readpos >= pMsgQueue->len)
            {
                readpos = 0;
            }

            pMsgQueue->readpos = readpos;
        }
    }

    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               GUI_GetFirstPostMsg
*
* author:
*
* date:             2009-11-21
*
* Description:      得到第一个id号为id，子id号为childid的消息，并清除之前的消息
*
* parameters:
*
* return:           if 找到 return ORANGE_TRUE
*                   if 没找到 返回 ORANGE_FALSE
* modify history:
**********************************************************************************************************************
*/

static __bool GUI_GetFirstPostMsg(__win_msgqueue_t   *pMsgQueue,
                                  __u32 id, __u32 childid, __gui_msg_t *msg)
{
    __u32           readpos;
    __u32           writepos;
    readpos     = pMsgQueue->readpos;
    writepos    = pMsgQueue->writepos;

    while (readpos != writepos)
    {
        if (pMsgQueue->pAsyncMsg[readpos].id == id
            && pMsgQueue->pAsyncMsg[readpos].dwAddData1 == childid)
        {
            pMsgQueue->readpos = readpos;
            *msg = pMsgQueue->pAsyncMsg[readpos];
            return ORANGE_TRUE;
        }

        readpos++;

        if (readpos >= pMsgQueue->len)
        {
            readpos = 0;
        }
    }

    return ORANGE_FALSE;
}

/*
**********************************************************************************************************************
*                                               GUI_GetFirstPostKeyMsg
*
* author:
*
* date:             2009-11-21
*
* Description:      得到第一个id号为id，子id号为actionid的消息，并清除之前的消息
*
* parameters:
*
* return:           if 找到 return ORANGE_TRUE
*                   if 没找到 返回 ORANGE_FALSE
* modify history:
**********************************************************************************************************************
*/

static __bool GUI_GetFirstPostKeyMsg(__win_msgqueue_t   *pMsgQueue,
                                     __u32 id, __u32 actionid, __gui_msg_t *msg)
{
    __u32           readpos;
    __u32           writepos;
    readpos     = pMsgQueue->readpos;
    writepos    = pMsgQueue->writepos;

    while (readpos != writepos)
    {
        if (pMsgQueue->pAsyncMsg[readpos].id == id
            && pMsgQueue->pAsyncMsg[readpos].dwAddData2 == actionid)
        {
            pMsgQueue->readpos = readpos;
            *msg = pMsgQueue->pAsyncMsg[readpos];
            return ORANGE_TRUE;
        }

        readpos++;

        if (readpos >= pMsgQueue->len)
        {
            readpos = 0;
        }
    }

    return ORANGE_FALSE;
}

/*
**********************************************************************************************************************
*                                               GUI_GetLastPostMsg
*
* author:
*
* date:             2009-11-20
*
* Description:      获取最后一个id号为id的消息
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __bool GUI_GetLastPostMsg(__win_msgqueue_t   *pMsgQueue, __gui_msg_t *msg, __u32 id)
{
    __u32           readpos;
    __u32           writepos;
    readpos     = pMsgQueue->readpos;
    writepos    = pMsgQueue->writepos;

    while (readpos != writepos)
    {
        if (pMsgQueue->pAsyncMsg[readpos].id == id)
        {
            *msg        = pMsgQueue->pAsyncMsg[readpos];
            msg->p_arg  = NULL;
        }

        readpos++;

        if (readpos >= pMsgQueue->len)
        {
            readpos = 0;
        }
    }

    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               GUI_GetLastPostChildMsg
*
* author:
*
* date:             2009-12-25
*
* Description:      获取最后一个消息id为id，子消息id为childid的消息
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __bool GUI_GetLastPostChildMsg(__win_msgqueue_t   *pMsgQueue, __gui_msg_t *msg, __u32 id, __u32 childid)
{
    __u32           readpos;
    __u32           writepos;
    readpos     = pMsgQueue->readpos;
    writepos    = pMsgQueue->writepos;

    while (readpos != writepos)
    {
        if (pMsgQueue->pAsyncMsg[readpos].id == id && pMsgQueue->pAsyncMsg[readpos].dwAddData1 == childid)
        {
            *msg        = pMsgQueue->pAsyncMsg[readpos];
            msg->p_arg  = NULL;
        }

        readpos++;

        if (readpos >= pMsgQueue->len)
        {
            readpos = 0;
        }
    }

    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               GUI_GetBeforePostMsg
*
* author:
*
* date:             2009-11-21
*
* Description:      获取最后消息，并合并最后的消息，更新读指针去掉该消息之前的消息。
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __bool GUI_GetBeforePostMsg(__win_msgqueue_t   *pMsgQueue,
                                   __u32 id, __u32 childid, __u32 activeid)
{
    __u32           readpos;
    __u32           writepos;
    readpos     = pMsgQueue->readpos;
    writepos    = pMsgQueue->writepos - 1;

    if ((__s32)writepos < 0)
    {
        writepos = pMsgQueue->len - 1;
    }

    while (readpos != writepos)
    {
        if ((pMsgQueue->pAsyncMsg[writepos].id != id)
            || (pMsgQueue->pAsyncMsg[writepos].dwAddData1 != childid)
            || (pMsgQueue->pAsyncMsg[writepos].dwAddData2 != activeid))
        {
            break;
        }

        writepos--;

        if ((__s32)writepos < 0)
        {
            writepos = pMsgQueue->len - 1;
        }
    }

    if (readpos == writepos)
    {
        if ((pMsgQueue->pAsyncMsg[writepos].id != id)
            || (pMsgQueue->pAsyncMsg[writepos].dwAddData1 != childid)
            || (pMsgQueue->pAsyncMsg[writepos].dwAddData2 != activeid))
        {
            pMsgQueue->readpos = writepos + 1;

            if (pMsgQueue->readpos >= pMsgQueue->len)
            {
                pMsgQueue->readpos = 0;
            }
        }
        else
        {
            pMsgQueue->readpos = writepos;
        }
    }
    else
    {
        pMsgQueue->readpos = writepos + 1;

        if (pMsgQueue->readpos >= pMsgQueue->len)
        {
            pMsgQueue->readpos = 0;
        }
    }

    return ORANGE_TRUE;
}


/*
**********************************************************************************************************************
*                                               GUI_GetLastBeforePostMsg
*
* author:
*
* date:             2009-12-25
*
* Description:      获取最后的某一个消息，合并之前的相同消息并更新读写指针
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __bool GUI_GetLastBeforePostMsg(__win_msgqueue_t   *pMsgQueue,
                                       __u32 id, __u32 childid)
{
    __u32           readpos;
    __u32           writepos;
    __bool          bFind   = ORANGE_FALSE;
    readpos     = pMsgQueue->readpos;
    writepos    = pMsgQueue->writepos - 1;

    if ((__s32)writepos < 0)
    {
        writepos = pMsgQueue->len - 1;
    }

    while (readpos != writepos)
    {
        if ((pMsgQueue->pAsyncMsg[writepos].id != id)
            || (pMsgQueue->pAsyncMsg[writepos].dwAddData1 != childid))
        {
            if (bFind == ORANGE_TRUE)
            {
                break;
            }
        }
        else
        {
            if (bFind == ORANGE_FALSE)
            {
                bFind = ORANGE_TRUE;
            }
        }

        writepos--;

        if ((__s32)writepos < 0)
        {
            writepos = pMsgQueue->len - 1;
        }
    }

    if (readpos == writepos)
    {
        if ((pMsgQueue->pAsyncMsg[writepos].id != id)
            || (pMsgQueue->pAsyncMsg[writepos].dwAddData1 != childid))
        {
            if (bFind == ORANGE_TRUE)
            {
                pMsgQueue->readpos = writepos + 1;

                if (pMsgQueue->readpos >= pMsgQueue->len)
                {
                    pMsgQueue->readpos = 0;
                }
            }
            else
            {
                pMsgQueue->readpos = writepos;
            }
        }
        else
        {
            pMsgQueue->readpos = writepos;
        }
    }
    else
    {
        pMsgQueue->readpos = writepos + 1;

        if (pMsgQueue->readpos >= pMsgQueue->len)
        {
            pMsgQueue->readpos = 0;
        }
    }

    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               GUI_PostMsgHandle
*
* author:
*
* date:             2009-11-12
*
* Description:      gui postmsghandle
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool GUI_PostMsgHandle(__win_msgqueue_t   *pMsgQueue, __gui_msg_t *msg)
{
    __gui_msg_t     tmp_msg;
    __gui_msg_t     last_msg;
    __gui_msg_t     first_msg;
    __bool          ret;
    pthread_mutex_lock(pMsgQueue->lock);

    while (pMsgQueue->readpos != pMsgQueue->writepos)
    {
        tmp_msg = pMsgQueue->pAsyncMsg[pMsgQueue->readpos];

        if (tmp_msg.id != GUI_MSG_KEY && tmp_msg.id != GUI_MSG_TOUCH)
        {
            *msg = pMsgQueue->pAsyncMsg[pMsgQueue->readpos];
            msg->p_arg = NULL;
            pMsgQueue->readpos++;

            if (pMsgQueue->readpos >= pMsgQueue->len)
            {
                pMsgQueue->readpos = 0;
            }

            pthread_mutex_unlock(pMsgQueue->lock);
            return ORANGE_TRUE;
        }
        else if (tmp_msg.id == GUI_MSG_KEY)
        {
            /*此处需要考虑触摸屏或鼠标先被
                按下时的按键处理情况*/
            if (pMsgQueue->touchdown == ORANGE_TRUE)
            {
                /*如果触摸屏或鼠标没有释放，直接扔掉按键*/
                pMsgQueue->readpos++;

                if (pMsgQueue->readpos >= pMsgQueue->len)
                {
                    pMsgQueue->readpos = 0;
                }

                continue;
            }

            GUI_GetLastPostMsg(pMsgQueue, &last_msg, GUI_MSG_KEY);

            /*如果最后一次是抬起键，则认为按键结束，清除按键*/
            if (last_msg.dwAddData2 == KEY_UP_ACTION)
            {
                GUI_ClearPostMsg(pMsgQueue);

                /*上一次不是抬起键，则发送抬起键*/
                if ((pMsgQueue->keyactionvalue != KEY_UP_ACTION))
                {
                    if (last_msg.dwAddData1 != pMsgQueue->keyvalue)
                    {
                        last_msg.dwAddData1 = pMsgQueue->keyvalue;
                    }

                    *msg                = last_msg;
                    msg->p_arg          = NULL;
                    pMsgQueue->keydown  = ORANGE_FALSE;
                    pMsgQueue->keycount = 0;
                    pMsgQueue->keyvalue = last_msg.dwAddData1;
                    pMsgQueue->keyactionvalue = last_msg.dwAddData2;
                    pthread_mutex_unlock(pMsgQueue->lock);
                    return ORANGE_TRUE;
                }

                continue;
            }

            /*最后一次按键不是GUI_MSG_KEY_RISE的情况*/
            /*获取第一个按键消息*/
            first_msg        = pMsgQueue->pAsyncMsg[pMsgQueue->readpos];
            first_msg.p_arg  = NULL;

            if (pMsgQueue->keycount == 0)
            {
                pMsgQueue->keycount++;
                /*此时表示第一次获取按键消息，最后一次按键有效*/
                pMsgQueue->keyvalue         = last_msg.dwAddData1;
                pMsgQueue->keyactionvalue   = last_msg.dwAddData2;
                /*第一次发送按键，直接传递给应用程序*/
                *msg                = last_msg;
                msg->p_arg          = NULL;
                /*更新消息队列读写指针*/
                GUI_GetBeforePostMsg(pMsgQueue, msg->id, msg->dwAddData1, msg->dwAddData2);
                pMsgQueue->keydown  = ORANGE_TRUE;
                pthread_mutex_unlock(pMsgQueue->lock);
                return ORANGE_TRUE;
            }
            else /*表示前面已经有按键被按下*/
            {
                /*判断消息队列中是否有抬起键*/
                ret = GUI_GetFirstPostKeyMsg(pMsgQueue, GUI_MSG_KEY, KEY_UP_ACTION, msg);

                if (ret == ORANGE_TRUE)
                {
                    pMsgQueue->keydown  = ORANGE_FALSE;
                    pMsgQueue->keycount = 0;
                    ret = GUI_ProcMsgKeyRise(pMsgQueue, GUI_MSG_KEY, KEY_UP_ACTION, msg, ORANGE_TRUE);

                    if (msg->dwAddData1 != pMsgQueue->keyvalue)
                    {
                        msg->dwAddData1 = pMsgQueue->keyvalue;
                    }

                    pMsgQueue->keyvalue = msg->dwAddData1;
                    pMsgQueue->keyactionvalue   = msg->dwAddData2;
                    /*按键是抬起键，此时需要采样丢掉重复按键*/
                    pthread_mutex_unlock(pMsgQueue->lock);
                    return ret;
                }

                pMsgQueue->keycount++;

                /*无抬起键，则直接计算长按键*/
                if (pMsgQueue->keydown == ORANGE_TRUE)
                {
                    pMsgQueue->readpos++;

                    if (pMsgQueue->readpos >= pMsgQueue->len)
                    {
                        pMsgQueue->readpos = 0;
                    }

                    /*上一次是长按键，则直接发送长按键*/
                    if (pMsgQueue->keyvalue >= GUI_MSG_KEY_LONGUP && pMsgQueue->keyvalue <GUI_MSG_KEY_MAX)
                    {
                        /*没有抬起键，则认为队列里面的消息类型一致，
                        故清除消息队列避免消息队列全部装满队列的情况*/
                        GUI_ClearPostMsg(pMsgQueue);
                        msg->id         = GUI_MSG_KEY;
                        msg->h_deswin   = first_msg.h_deswin;
                        msg->h_srcwin   = first_msg.h_srcwin;
                        msg->dwAddData1 = first_msg.dwAddData1 + LONGKEY_OFFSET;
                        msg->dwAddData2 = KEY_REPEAT_ACTION;
                        msg->dwReserved = pMsgQueue->keycount;
                        msg->p_arg      = NULL;
                        pMsgQueue->keyvalue = msg->dwAddData1;
                        pMsgQueue->keyactionvalue = msg->dwAddData2;
                        pthread_mutex_unlock(pMsgQueue->lock);
                        return ORANGE_TRUE;
                    }
                    else
                    {
                        /*在消息数量为LONGKEY_COUNT确定为长按键*/
                        if (first_msg.dwAddData2 == KEY_REPEAT_ACTION)
                        {
                            msg->id         = GUI_MSG_KEY;
                            msg->h_deswin   = first_msg.h_deswin;
                            msg->h_srcwin   = first_msg.h_srcwin;
                            msg->dwAddData1 = first_msg.dwAddData1 + LONGKEY_OFFSET;
                            msg->dwAddData2 = KEY_REPEAT_ACTION;
                            msg->dwReserved = 1;
                            msg->p_arg      = NULL;
                            pMsgQueue->keyvalue = msg->dwAddData1;
                            pMsgQueue->keyactionvalue = msg->dwAddData2;
                            pthread_mutex_unlock(pMsgQueue->lock);
                            return ORANGE_TRUE;
                        }
                    }
                }
            }
        }
        else if (tmp_msg.id == GUI_MSG_TOUCH)
        {
            /*此处需要考虑按键先被按下时的
            触摸屏或鼠标先的处理情况*/
            if (pMsgQueue->keydown == ORANGE_TRUE)
            {
                /*如果按键没有释放，直接扔掉触摸屏键*/
                pMsgQueue->readpos++;

                if (pMsgQueue->readpos >= pMsgQueue->len)
                {
                    pMsgQueue->readpos = 0;
                }

                continue;
            }

            if (tmp_msg.dwAddData1 >= GUI_MSG_TOUCH_OVERDOWN)
            {
                GUI_GetLastPostMsg(pMsgQueue, &last_msg, GUI_MSG_TOUCH);

                /*如果最后一次是TOUCH OVERUP，则认为TOUCH结束，清除TOUCH消息*/
                if (last_msg.dwAddData1 == GUI_MSG_TOUCH_OVERUP)
                {
                    /*上一次不是TOUCH OVERUP，则发送第一个TOUCH UP*/
                    if (pMsgQueue->touchvalue != GUI_MSG_TOUCH_OVERUP)
                    {
                        ret = GUI_GetFirstPostMsg(pMsgQueue, GUI_MSG_TOUCH, GUI_MSG_TOUCH_OVERUP, msg);

                        if (ret == ORANGE_FALSE)
                        {
                            *msg              = last_msg;
                        }

                        msg->p_arg            = NULL;
                        pMsgQueue->touchdown  = ORANGE_FALSE;
                        pMsgQueue->touchcount = 0;
                        pMsgQueue->touchvalue = msg->dwAddData1;
                        GUI_ClearPostMsg(pMsgQueue);
                        pthread_mutex_unlock(pMsgQueue->lock);
                        return ORANGE_TRUE;
                    }

                    GUI_ClearPostMsg(pMsgQueue);
                    continue;
                }

                /*最后一次TOUCH不是GUI_MSG_TOUCH_OVERUP的情况*/
                first_msg        = pMsgQueue->pAsyncMsg[pMsgQueue->readpos];
                first_msg.p_arg  = NULL;

                if (pMsgQueue->touchcount == 0 && pMsgQueue->touchdown == ORANGE_FALSE)
                {
                    pMsgQueue->touchcount++;
                    GUI_GetLastPostChildMsg(pMsgQueue, &last_msg, GUI_MSG_TOUCH, GUI_MSG_TOUCH_OVERDOWN);
                    /*此时表示第一次获取TOUCH消息，最后一次TOUCH DOWN有效*/
                    pMsgQueue->touchvalue = last_msg.dwAddData1;
                    /*第一次发送TOUCH，直接传递给应用程序*/
                    *msg                = last_msg;
                    msg->p_arg          = NULL;
                    /*更新消息队列读写指针*/
                    GUI_GetLastBeforePostMsg(pMsgQueue, msg->id, msg->dwAddData1);
                    pMsgQueue->touchdown  = ORANGE_TRUE;
                    pthread_mutex_unlock(pMsgQueue->lock);
                    return ORANGE_TRUE;
                }
                else /*表示前面已经有touch点被按下*/
                {
                    /*判断消息队列中是否有GUI_MSG_TOUCH_UP*/
                    ret = GUI_GetFirstPostMsg(pMsgQueue, GUI_MSG_TOUCH, GUI_MSG_TOUCH_OVERUP, msg);

                    if (ret == ORANGE_TRUE)
                    {
                        __bool tmp_ret;
                        pMsgQueue->touchdown  = ORANGE_FALSE;
                        pMsgQueue->touchcount = 0;
                        pMsgQueue->touchvalue = tmp_msg.dwAddData1;
                        /*按键是抬起键，此时需要采样丢掉重复按键*/
                        tmp_ret = GUI_ProcMsgRise(pMsgQueue, GUI_MSG_TOUCH, GUI_MSG_TOUCH_OVERUP, msg, ORANGE_TRUE);
                        pthread_mutex_unlock(pMsgQueue->lock);
                        return tmp_ret;
                    }

                    if (tmp_msg.dwAddData1 == GUI_MSG_TOUCH_OVERDOWN)
                    {
                        pMsgQueue->touchcount++;

                        if (pMsgQueue->touchcount == 1)
                        {
                            /*第一次发送touch down，直接传递给应用程序*/
                            *msg = pMsgQueue->pAsyncMsg[pMsgQueue->readpos];
                            msg->p_arg = NULL;
                            pMsgQueue->readpos++;

                            if (pMsgQueue->readpos >= pMsgQueue->len)
                            {
                                pMsgQueue->readpos = 0;
                            }

                            pMsgQueue->touchdown  = ORANGE_TRUE;
                            pthread_mutex_unlock(pMsgQueue->lock);
                            return ORANGE_TRUE;
                        }
                        else
                        {
                            /*第二次或以上发送,直接判断长按键*/
                            if (pMsgQueue->touchdown == ORANGE_TRUE)
                            {
                                pMsgQueue->readpos++;

                                if (pMsgQueue->readpos >= pMsgQueue->len)
                                {
                                    pMsgQueue->readpos = 0;
                                }

                                /*上一次是长按键，则直接发送长按键*/
                                if (pMsgQueue->touchvalue == GUI_MSG_TOUCH_OVERLONGDOWN)
                                {
                                    /*没有抬起键，则认为队列里面的消息类型一致，
                                    故清除消息队列避免消息队列全部装满队列的情况*/
                                    GUI_ClearPostMsg(pMsgQueue);
                                    msg->id         = GUI_MSG_TOUCH;
                                    msg->h_deswin   = first_msg.h_deswin;
                                    msg->h_srcwin   = first_msg.h_srcwin;
                                    msg->dwAddData1 = GUI_MSG_TOUCH_OVERLONGDOWN;
                                    msg->dwAddData2 = first_msg.dwAddData2;
                                    msg->dwReserved = pMsgQueue->touchcount - pMsgQueue->longcount + 1;
                                    msg->p_arg      = NULL;
                                    pMsgQueue->touchvalue = msg->dwAddData1;
                                    pthread_mutex_unlock(pMsgQueue->lock);
                                    return ORANGE_TRUE;
                                }
                                else
                                {
                                    /*在消息数量为LONGKEY_COUNT确定为长按键*/
                                    if (pMsgQueue->touchcount % pMsgQueue->longcount == 0)
                                    {
                                        msg->id         = GUI_MSG_TOUCH;
                                        msg->h_deswin   = first_msg.h_deswin;
                                        msg->h_srcwin   = first_msg.h_srcwin;
                                        msg->dwAddData1 = GUI_MSG_TOUCH_OVERLONGDOWN;
                                        msg->dwAddData2 = first_msg.dwAddData2;
                                        msg->dwReserved = 1;
                                        msg->p_arg      = NULL;
                                        pMsgQueue->touchvalue = msg->dwAddData1;
                                        pthread_mutex_unlock(pMsgQueue->lock);
                                        return ORANGE_TRUE;
                                    }
                                    else
                                    {
                                        continue;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        pMsgQueue->touchcount = 0;

                        if (tmp_msg.dwAddData1 == GUI_MSG_TOUCH_OVERMOVE)
                        {
                            /*是MOVE消息取出最后一个move消息发给相应的窗口*/
                            GUI_GetLastPostChildMsg(pMsgQueue, &last_msg, GUI_MSG_TOUCH, GUI_MSG_TOUCH_OVERMOVE);
                            /*此时表示第一次获取TOUCH消息，最后一次TOUCH MOVE有效*/
                            pMsgQueue->touchvalue = last_msg.dwAddData1;
                            /*第一次发送TOUCH，直接传递给应用程序*/
                            *msg                = last_msg;
                            msg->p_arg          = NULL;
                            /*更新消息队列读写指针*/
                            GUI_GetLastBeforePostMsg(pMsgQueue, msg->id, msg->dwAddData1);
                            pMsgQueue->touchdown  = ORANGE_TRUE;
                            pMsgQueue->readpos++;

                            if (pMsgQueue->readpos >= pMsgQueue->len)
                            {
                                pMsgQueue->readpos = 0;
                            }

                            pthread_mutex_unlock(pMsgQueue->lock);
                            return ORANGE_TRUE;
                        }

                        if (tmp_msg.dwAddData1 == GUI_MSG_TOUCH_OVERZOOMIN)
                        {
                            /*是OVERZOOMIN消息取出最后一个OVERZOOMIN消息发给相应的窗口*/
                            GUI_GetLastPostChildMsg(pMsgQueue, &last_msg, GUI_MSG_TOUCH, GUI_MSG_TOUCH_OVERZOOMIN);
                            /*此时表示第一次获取TOUCH消息，最后一次TOUCH OVERZOOMIN有效*/
                            pMsgQueue->touchvalue = last_msg.dwAddData1;
                            /*第一次发送TOUCH，直接传递给应用程序*/
                            *msg                = last_msg;
                            msg->p_arg          = NULL;
                            /*更新消息队列读写指针*/
                            GUI_GetLastBeforePostMsg(pMsgQueue, msg->id, msg->dwAddData1);
                            pMsgQueue->touchdown  = ORANGE_TRUE;
                            pMsgQueue->readpos++;

                            if (pMsgQueue->readpos >= pMsgQueue->len)
                            {
                                pMsgQueue->readpos = 0;
                            }

                            pthread_mutex_unlock(pMsgQueue->lock);
                            return ORANGE_TRUE;
                        }

                        if (tmp_msg.dwAddData1 == GUI_MSG_TOUCH_OVERZOOMOUT)
                        {
                            /*是OVERZOOMOUT消息取出最后一个OVERZOOMOUT消息发给相应的窗口*/
                            GUI_GetLastPostChildMsg(pMsgQueue, &last_msg, GUI_MSG_TOUCH, GUI_MSG_TOUCH_OVERZOOMOUT);
                            /*此时表示第一次获取TOUCH消息，最后一次TOUCH OVERZOOMOUT有效*/
                            pMsgQueue->touchvalue = last_msg.dwAddData1;
                            /*第一次发送TOUCH，直接传递给应用程序*/
                            *msg                = last_msg;
                            msg->p_arg          = NULL;
                            /*更新消息队列读写指针*/
                            GUI_GetLastBeforePostMsg(pMsgQueue, msg->id, msg->dwAddData1);
                            pMsgQueue->touchdown  = ORANGE_TRUE;
                            pMsgQueue->readpos++;

                            if (pMsgQueue->readpos >= pMsgQueue->len)
                            {
                                pMsgQueue->readpos = 0;
                            }

                            pthread_mutex_unlock(pMsgQueue->lock);
                            return ORANGE_TRUE;
                        }
                    }
                }
            }

            GUI_GetLastPostMsg(pMsgQueue, &last_msg, GUI_MSG_TOUCH);

            /*如果最后一次是TOUCH UP，则认为TOUCH结束，清除TOUCH消息*/
            if (last_msg.dwAddData1 == GUI_MSG_TOUCH_UP)
            {
                /*上一次不是TOUCH UP，则发送第一个TOUCH UP*/
                if (pMsgQueue->touchvalue != GUI_MSG_TOUCH_UP)
                {
                    ret = GUI_GetFirstPostMsg(pMsgQueue, GUI_MSG_TOUCH, GUI_MSG_TOUCH_UP, msg);

                    if (ret == ORANGE_FALSE)
                    {
                        *msg              = last_msg;
                    }

                    msg->p_arg            = NULL;
                    pMsgQueue->touchdown  = ORANGE_FALSE;
                    pMsgQueue->touchcount = 0;
                    pMsgQueue->touchvalue = msg->dwAddData1;
                    GUI_ClearPostMsg(pMsgQueue);
                    pthread_mutex_unlock(pMsgQueue->lock);
                    return ORANGE_TRUE;
                }

                GUI_ClearPostMsg(pMsgQueue);
                continue;
            }

            /*最后一次TOUCH不是GUI_MSG_TOUCH_UP的情况*/
            first_msg        = pMsgQueue->pAsyncMsg[pMsgQueue->readpos];
            first_msg.p_arg  = NULL;

            if (pMsgQueue->touchcount == 0 && pMsgQueue->touchdown == ORANGE_FALSE)
            {
                pMsgQueue->touchcount++;
                GUI_GetLastPostChildMsg(pMsgQueue, &last_msg, GUI_MSG_TOUCH, GUI_MSG_TOUCH_DOWN);
                /*此时表示第一次获取TOUCH消息，最后一次TOUCH DOWN有效*/
                pMsgQueue->touchvalue = last_msg.dwAddData1;
                /*第一次发送TOUCH，直接传递给应用程序*/
                *msg                = last_msg;
                msg->p_arg          = NULL;
                /*更新消息队列读写指针*/
                GUI_GetLastBeforePostMsg(pMsgQueue, msg->id, msg->dwAddData1);
                pMsgQueue->touchdown  = ORANGE_TRUE;
                pthread_mutex_unlock(pMsgQueue->lock);
                return ORANGE_TRUE;
            }
            else /*表示前面已经有touch点被按下*/
            {
                /*判断消息队列中是否有GUI_MSG_TOUCH_UP*/
                ret = GUI_GetFirstPostMsg(pMsgQueue, GUI_MSG_TOUCH, GUI_MSG_TOUCH_UP, msg);

                if (ret == ORANGE_TRUE)
                {
                    __bool tmp_ret;
                    pMsgQueue->touchdown  = ORANGE_FALSE;
                    pMsgQueue->touchcount = 0;
                    pMsgQueue->touchvalue = tmp_msg.dwAddData1;
                    /*按键是抬起键，此时需要采样丢掉重复按键*/
                    tmp_ret = GUI_ProcMsgRise(pMsgQueue, GUI_MSG_TOUCH, GUI_MSG_TOUCH_UP, msg, ORANGE_TRUE);
                    pthread_mutex_unlock(pMsgQueue->lock);
                    return tmp_ret;
                }

                if (tmp_msg.dwAddData1 == GUI_MSG_TOUCH_DOWN)
                {
                    pMsgQueue->touchcount++;

                    if (pMsgQueue->touchcount == 1)
                    {
                        /*第一次发送touch down，直接传递给应用程序*/
                        *msg = pMsgQueue->pAsyncMsg[pMsgQueue->readpos];
                        msg->p_arg = NULL;
                        pMsgQueue->readpos++;

                        if (pMsgQueue->readpos >= pMsgQueue->len)
                        {
                            pMsgQueue->readpos = 0;
                        }

                        pMsgQueue->touchdown  = ORANGE_TRUE;
                        pthread_mutex_unlock(pMsgQueue->lock);
                        return ORANGE_TRUE;
                    }
                    else
                    {
                        /*第二次或以上发送,直接判断长按键*/
                        if (pMsgQueue->touchdown == ORANGE_TRUE)
                        {
                            pMsgQueue->readpos++;

                            if (pMsgQueue->readpos >= pMsgQueue->len)
                            {
                                pMsgQueue->readpos = 0;
                            }

                            /*上一次是长按键，则直接发送长按键*/
                            if (pMsgQueue->touchvalue == GUI_MSG_TOUCH_LONGDOWN)
                            {
                                /*没有抬起键，则认为队列里面的消息类型一致，
                                故清除消息队列避免消息队列全部装满队列的情况*/
                                GUI_ClearPostMsg(pMsgQueue);
                                msg->id         = GUI_MSG_TOUCH;
                                msg->h_deswin   = tmp_msg.h_deswin;
                                msg->h_srcwin   = tmp_msg.h_srcwin;
                                msg->dwAddData1 = GUI_MSG_TOUCH_LONGDOWN;
                                msg->dwAddData2 = tmp_msg.dwAddData2;
                                msg->dwReserved = 1;
                                msg->p_arg      = NULL;
                                pMsgQueue->touchvalue = msg->dwAddData1;
                                pthread_mutex_unlock(pMsgQueue->lock);
                                return ORANGE_TRUE;
                            }
                            else
                            {
                                /*在消息数量为LONGKEY_COUNT确定为长按键*/
                                if (pMsgQueue->touchcount % pMsgQueue->longcount == 0)
                                {
                                    msg->id         = GUI_MSG_TOUCH;
                                    msg->h_deswin   = tmp_msg.h_deswin;
                                    msg->h_srcwin   = tmp_msg.h_srcwin;
                                    msg->dwAddData1 = GUI_MSG_TOUCH_LONGDOWN;
                                    msg->dwAddData2 = tmp_msg.dwAddData2;
                                    msg->dwReserved = 1;
                                    msg->p_arg      = NULL;
                                    pMsgQueue->touchvalue = msg->dwAddData1;
                                    pthread_mutex_unlock(pMsgQueue->lock);
                                    return ORANGE_TRUE;
                                }
                                else
                                {
                                    continue;
                                }
                            }
                        }
                    }
                }
                else
                {
                    pMsgQueue->touchcount = 0;

                    if (tmp_msg.dwAddData1 == GUI_MSG_TOUCH_MOVE)
                    {
                        /*是MOVE消息取出最后一个move消息发给相应的窗口*/
                        GUI_GetLastPostChildMsg(pMsgQueue, &last_msg, GUI_MSG_TOUCH, GUI_MSG_TOUCH_MOVE);
                        /*此时表示第一次获取TOUCH消息，最后一次TOUCH MOVE有效*/
                        pMsgQueue->touchvalue = last_msg.dwAddData1;
                        /*第一次发送TOUCH，直接传递给应用程序*/
                        *msg                = last_msg;
                        msg->p_arg          = NULL;
                        /*更新消息队列读写指针*/
                        GUI_GetLastBeforePostMsg(pMsgQueue, msg->id, msg->dwAddData1);
                        pMsgQueue->touchdown  = ORANGE_TRUE;
                        pMsgQueue->readpos++;

                        if (pMsgQueue->readpos >= pMsgQueue->len)
                        {
                            pMsgQueue->readpos = 0;
                        }

                        pthread_mutex_unlock(pMsgQueue->lock);
                        return ORANGE_TRUE;
                    }

                    if (tmp_msg.dwAddData1 == GUI_MSG_TOUCH_ZOOMIN)
                    {
                        /*是ZOOMIN消息取出最后一个ZOOMIN消息发给相应的窗口*/
                        GUI_GetLastPostChildMsg(pMsgQueue, &last_msg, GUI_MSG_TOUCH, GUI_MSG_TOUCH_ZOOMIN);
                        /*此时表示第一次获取TOUCH消息，最后一次TOUCH ZOOMIN有效*/
                        pMsgQueue->touchvalue = last_msg.dwAddData1;
                        /*第一次发送TOUCH，直接传递给应用程序*/
                        *msg                = last_msg;
                        msg->p_arg          = NULL;
                        /*更新消息队列读写指针*/
                        GUI_GetLastBeforePostMsg(pMsgQueue, msg->id, msg->dwAddData1);
                        pMsgQueue->touchdown  = ORANGE_TRUE;
                        pMsgQueue->readpos++;

                        if (pMsgQueue->readpos >= pMsgQueue->len)
                        {
                            pMsgQueue->readpos = 0;
                        }

                        pthread_mutex_unlock(pMsgQueue->lock);
                        return ORANGE_TRUE;
                    }

                    if (tmp_msg.dwAddData1 == GUI_MSG_TOUCH_ZOOMOUT)
                    {
                        /*是ZOOMOUT消息取出最后一个ZOOMOUT消息发给相应的窗口*/
                        GUI_GetLastPostChildMsg(pMsgQueue, &last_msg, GUI_MSG_TOUCH, GUI_MSG_TOUCH_ZOOMOUT);
                        /*此时表示第一次获取TOUCH消息，最后一次TOUCH ZOOMOUT有效*/
                        pMsgQueue->touchvalue = last_msg.dwAddData1;
                        /*第一次发送TOUCH，直接传递给应用程序*/
                        *msg                = last_msg;
                        msg->p_arg          = NULL;
                        /*更新消息队列读写指针*/
                        GUI_GetLastBeforePostMsg(pMsgQueue, msg->id, msg->dwAddData1);
                        pMsgQueue->touchdown  = ORANGE_TRUE;
                        pMsgQueue->readpos++;

                        if (pMsgQueue->readpos >= pMsgQueue->len)
                        {
                            pMsgQueue->readpos = 0;
                        }

                        pthread_mutex_unlock(pMsgQueue->lock);
                        return ORANGE_TRUE;
                    }
                }
            }
        }
    }

    pMsgQueue->dwState &= ~QS_POSTMSG;
    pthread_mutex_unlock(pMsgQueue->lock);
    return ORANGE_FALSE;
}

/*
**********************************************************************************************************************
*                                               QMsgMemAlloc
*
* author:
*
* date:             2009-7-21
*
* Description:      qmsgmemalloc
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
/* global */
static __gui_qmsg_t *QMsgMemAlloc(void)
{
    return (__gui_qmsg_t *) MemBlockAlloc(&qmsg_blk);
}

/*
**********************************************************************************************************************
*                                               FreeQMsgMem
*
* author:
*
* date:             2009-7-21
*
* Description:      freeqmsgmem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static void FreeQMsgMem(__gui_qmsg_t *pqmsg)
{
    MemBlockFree(&qmsg_blk, pqmsg);
}

/*
**********************************************************************************************************************
*                                               BE_THIS_THREAD
*
* author:
*
* date:             2009-11-2
*
* Description:      be this thread
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __bool be_this_thread(H_WIN hWnd)
{
    __gui_manwin_t  *pManWin;
    pManWin = (__gui_manwin_t *)GUI_WinH2P(GUI_WinGetManWin(hWnd));

    if (pManWin && pManWin->threadid == pthread_self())
    {
        return ORANGE_TRUE;
    }

    return ORANGE_FALSE;
}

/*
**********************************************************************************************************************
*                                               InitMsgQueue
*
* author:
*
* date:             2009-7-20
*
* Description:      initmsgqueue
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool  InitMsgQueue(__win_msgqueue_t *qmsg, __u32  buf_len)
{
    __u32  i;
    qmsg->dwState = QS_EMPTY;
    pthread_mutex_init(&qmsg->lock, (void *)1);

    if (qmsg->lock == NULL)
    {
        ORANGE_WRN("create message queue mutex failed!");
        return ORANGE_FAIL;
    }

    sem_init(&qmsg->wait, 0, 0);

    if (qmsg->wait == NULL)
    {
        ORANGE_WRN("create message wait semphore failed!");
        return ORANGE_FAIL;
    }

    for (i = 0; i < DEF_MAX_QUIT_WIN; i++)
    {
        qmsg->quit_win[i]   = 0;
        qmsg->loop_win[i]   = 0;
    }

    qmsg->pFirstNotifyMsg   = NULL;
    qmsg->pLastNotifyMsg    = NULL;
    qmsg->readpos           = 0;
    qmsg->writepos          = 0;
    qmsg->ireadpos          = 0;
    qmsg->iwritepos         = 0;
    qmsg->pFirstSyncMsg     = NULL;
    qmsg->pLastSyncMsg      = NULL;

    if (buf_len <= 0)
    {
        buf_len             = DEF_MAXMSG_LEN;
    }

    qmsg->pAsyncMsg         = orange_malloc(sizeof(__gui_msg_t) * buf_len);
    qmsg->len               = buf_len;
    qmsg->pPaintMsg         = orange_malloc(sizeof(__gui_msg_t) * buf_len);
    qmsg->len               = buf_len;
    qmsg->FirstTimerSlot    = 0;
    qmsg->TimerMask         = 0;
    qmsg->keydown           = 0;
    qmsg->keycount          = 0;
    qmsg->touchdown         = 0;
    qmsg->touchcount        = 0;
    qmsg->longcount         = LONGKEY_COUNT;
    return qmsg->pAsyncMsg != NULL;
}

/*
**********************************************************************************************************************
*                                               DestroyMsgQueue
*
* author:
*
* date:             2009-7-21
*
* Description:      destroymsgqueue
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void DestroyMsgQueue(__win_msgqueue_t *qmsg)
{
    __gui_qmsg_t   *head;
    __gui_qmsg_t   *next;
    head = next = qmsg->pFirstNotifyMsg;

    while (head)
    {
        next = head->next;
        FreeQMsgMem(head);
        head = next;
    }

    pthread_mutex_destroy(qmsg->lock);
    sem_destroy(&qmsg->wait);

    if (qmsg->pAsyncMsg)
    {
        orange_mfree(qmsg->pAsyncMsg);
    }

    qmsg->pAsyncMsg = NULL;

    if (qmsg->pPaintMsg)
    {
        orange_mfree(qmsg->pPaintMsg);
    }

    qmsg->pPaintMsg = NULL;
    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_HavePendingMessage
*
* author:
*
* date:             2009-7-23
*
* Description:      判断是否有消息进入消息队列
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool GUI_HavePendingMessage(H_WIN hWnd)
{
    __win_msgqueue_t   *pMsgQueue;
    pMsgQueue = GUI_WinGetMsgQueue(hWnd);

    if (!pMsgQueue)
    {
        return ORANGE_FALSE;
    }

    if (pMsgQueue->dwState & QS_NOTIFYMSG)
    {
        if (pMsgQueue->pFirstNotifyMsg)
        {
            return ORANGE_TRUE;
        }
    }

    if (pMsgQueue->dwState & QS_SYNCMSG)
    {
        if (pMsgQueue->pFirstSyncMsg)
        {
            return ORANGE_TRUE;
        }
    }

    if (pMsgQueue->dwState & QS_POSTMSG)
    {
        if (pMsgQueue->readpos != pMsgQueue->writepos)
        {
            return ORANGE_TRUE;
        }
    }

    if (pMsgQueue->dwState & QS_USERASYNC)
    {
        if (pMsgQueue->ireadpos != pMsgQueue->iwritepos)
        {
            return ORANGE_TRUE;
        }
    }

    if (pMsgQueue->dwState & (QS_QUIT | QS_TIMER | QS_PAINT))
    {
        return ORANGE_TRUE;
    }

    return ORANGE_FALSE;
}

/*
**********************************************************************************************************************
*                                               GUI_InsertLoopWin
*
* author:
*
* date:             2009-12-17
*
* Description:      gui insertloopwin
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __bool GUI_InsertLoopWin(__win_msgqueue_t   *pMsgQueue, H_WIN hWnd)
{
    __u32  i;

    for (i = 0; i < DEF_MAX_QUIT_WIN; i++)
    {
        if (pMsgQueue->loop_win[i] == 0)
        {
            pMsgQueue->loop_win[i] = hWnd;
            break;
        }
        else if (pMsgQueue->loop_win[i] == hWnd)
        {
            break;
        }
    }

    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               GUI_DeleteLoopWin
*
* author:
*
* date:             2009-12-17
*
* Description:      gui deleteloopwin
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __bool GUI_DeleteLoopWin(__win_msgqueue_t   *pMsgQueue, H_WIN hWnd)
{
    __u32  i;

    for (i = 0; i < DEF_MAX_QUIT_WIN; i++)
    {
        if (pMsgQueue->loop_win[i] == hWnd)
        {
            pMsgQueue->loop_win[i] = 0;
            break;
        }
    }

    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               GUI_QueryLoopWin
*
* author:
*
* date:             2009-12-17
*
* Description:      gui queryloopwin
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __bool GUI_QueryLoopWin(__win_msgqueue_t   *pMsgQueue, H_WIN hWnd)
{
    __u32  i;

    for (i = 0; i < DEF_MAX_QUIT_WIN; i++)
    {
        if (pMsgQueue->loop_win[i] == hWnd)
        {
            return ORANGE_TRUE;
        }
    }

    return ORANGE_FALSE;
}

/*
**********************************************************************************************************************
*                                               GUI_QueryQuitWin
*
* author:
*
* date:             2009-12-17
*
* Description:      gui queryquitwin
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __bool GUI_QueryQuitWin(__win_msgqueue_t   *pMsgQueue)
{
    __u32  i;

    for (i = 0; i < DEF_MAX_QUIT_WIN; i++)
    {
        if (pMsgQueue->quit_win[i])
        {
            return ORANGE_TRUE;
        }
    }

    return ORANGE_FALSE;
}

/*
**********************************************************************************************************************
*                                               GUI_BroadcastMessage
*
* author:
*
* date:             2009-7-23
*
* Description:      gui broadcastmessage
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_BroadcastMessage(__gui_msg_t *pmsg)
{
    __gui_msg_t         msg_para;
    __gui_msg_t         msg;
    __gui_win_t        *pMainWin;
    __gui_win_t        *pWin;
    pWin                 =   GUI_WinH2P(GUI_WinGetRootWin());
    pMainWin             =   pWin->FirstChild;

    while (pMainWin)
    {
        msg_para.id          =   pmsg->id;
        msg_para.dwAddData1  =   pmsg->dwAddData1;
        msg_para.dwAddData2  =   pmsg->dwAddData2;
        msg_para.dwReserved  =   pmsg->dwReserved;
        msg_para.p_arg       =   NULL;
        msg_para.h_deswin    =   GUI_WinP2H(pMainWin);
        msg_para.h_srcwin    =   pmsg->h_srcwin;
        GUI_SendMessage(&msg_para);
        pMainWin             =   pMainWin->Next;
    };

    return  ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_PostSyncMessage
*
* author:
*
* date:             2009-7-23
*
* Description:      gui postsyncmessage
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_PostSyncMessage(__gui_msg_t *pmsg)
{
    __win_msgqueue_t   *pMsgQueue;
    __gui_syncmsg_t     SyncMsg;
    H_WIN               hWnd = pmsg->h_deswin;
    pMsgQueue = GUI_WinGetMsgQueue(hWnd);

    if (!pMsgQueue)
    {
        return ORANGE_FAIL;
    }

    pthread_mutex_lock(pMsgQueue->lock);
    // queue the sync message.
    SyncMsg.msg.h_deswin        = hWnd;
    SyncMsg.msg.h_srcwin        = pmsg->h_srcwin;
    SyncMsg.msg.id              = pmsg->id;
    SyncMsg.msg.dwAddData1      = pmsg->dwAddData1;
    SyncMsg.msg.dwAddData2      = pmsg->dwAddData2;
    SyncMsg.msg.dwReserved      = pmsg->dwReserved;
    SyncMsg.pNext               = NULL;
    pthread_mutex_init(&SyncMsg.sem_handle, (void *)0);

    if (pMsgQueue->pFirstSyncMsg == NULL)
    {
        pMsgQueue->pFirstSyncMsg = pMsgQueue->pLastSyncMsg = &SyncMsg;
    }
    else
    {
        pMsgQueue->pLastSyncMsg->pNext = &SyncMsg;
        pMsgQueue->pLastSyncMsg = &SyncMsg;
    }

    pMsgQueue->dwState |= QS_SYNCMSG;
    pthread_mutex_unlock(pMsgQueue->lock);
    /*通知系统消息已经到达相应的消息队列*/
    GUI_NotifyMSGQ(pMsgQueue);
    // suspend until the message been handled.
    pthread_mutex_lock(SyncMsg.sem_handle);
    pthread_mutex_destroy(SyncMsg.sem_handle);
    return SyncMsg.retval;
}

/*
**********************************************************************************************************************
*                                               GUI_SendMessgae
*
* author:
*
* date:             2009-7-21
*
* Description:      gui sendmessgae
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_SendMessage(__gui_msg_t *msg)
{
    __pGUI_WIN_CB           WndProc;
    H_WIN                   hWnd = msg->h_deswin;
    __u32                    cur_tid;

    if (hWnd == HWND_INVALID)
    {
        return ORANGE_FAIL;
    }

    if (hWnd == GUI_WinGetRootWin())
    {
        WndProc = GUI_WinGetCallback(hWnd);
        return esKRNL_CallBack((__pCBK_t) * WndProc, msg);
    }

    cur_tid     = pthread_self();

    if (GUI_WinGetWinThreadId(hWnd) != cur_tid)
    {
        __gui_msg_t newmsg;
        newmsg.dwAddData1   = msg->dwAddData1;
        newmsg.dwAddData2   = msg->dwAddData2;
        newmsg.dwReserved   = msg->dwReserved;
        newmsg.h_deswin     = msg->h_deswin;
        newmsg.h_srcwin     = msg->h_srcwin;
        newmsg.id           = msg->id;

        return GUI_PostSyncMessage(&newmsg);
    }

    WndProc = GUI_WinGetCallback(hWnd);

    return esKRNL_CallBack((__pCBK_t) * WndProc, msg);
}


/*
**********************************************************************************************************************
*                                               GUI_SendMessageTimeout
*
* author:
*
* date:             2009-12-3
*
* Description:      gui sendmessagetimeout
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_SendMessageTimeout(__gui_msg_t *msg, __u32 timeout)
{
    __pGUI_WIN_CB           WndProc;
    H_WIN                   hWnd = msg->h_deswin;
    __u32                   cur_tid;

    if (hWnd == HWND_INVALID)
    {
        return ORANGE_FAIL;
    }

    if (hWnd == GUI_WinGetRootWin())
    {
        WndProc = GUI_WinGetCallback(hWnd);
        return esKRNL_CallBack((__pCBK_t) * WndProc, msg);
    }

    cur_tid     = pthread_self();

    if (GUI_WinGetWinThreadId(hWnd) != cur_tid)
    {
        __gui_msg_t newmsg;
        newmsg.dwAddData1   = msg->dwAddData1;
        newmsg.dwAddData2   = msg->dwAddData2;
        newmsg.dwReserved   = msg->dwReserved;
        newmsg.h_deswin     = msg->h_deswin;
        newmsg.h_srcwin     = msg->h_srcwin;
        newmsg.id           = msg->id;
        return GUI_PostSyncMessage(&newmsg);
    }

    WndProc = GUI_WinGetCallback(hWnd);
    return esKRNL_CallBack((__pCBK_t) * WndProc, msg);
}

/*
**********************************************************************************************************************
*                                               GUI_PostMessage
*
* author:
*
* date:             2009-7-21
*
* Description:      gui postmessage
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32 GUI_PostInputMessage(__win_msgqueue_t *pMsgQueue, __gui_msg_t *msg)
{
    pmsg        pMsg;
    H_WIN       hWnd = msg->h_deswin;
    pthread_mutex_lock(pMsgQueue->lock);

    if ((pMsgQueue->writepos + 1) % pMsgQueue->len == pMsgQueue->readpos)
    {
        pthread_mutex_unlock(pMsgQueue->lock);
        return ORANGE_QUEUE_FULL;
    }

    // Write the data and advance write pointer */
    pMsg                = &(pMsgQueue->pAsyncMsg[pMsgQueue->writepos]);
    pMsg->h_deswin      = hWnd;
    pMsg->id            = msg->id;
    pMsg->dwAddData1    = msg->dwAddData1;
    pMsg->dwAddData2    = msg->dwAddData2;
    pMsg->dwReserved    = msg->dwReserved;
    pMsgQueue->writepos++;

    if (pMsgQueue->writepos >= pMsgQueue->len)
    {
        pMsgQueue->writepos = 0;
    }

    pMsgQueue->dwState |= QS_POSTMSG;
    pthread_mutex_unlock(pMsgQueue->lock);

    if (!be_this_thread(hWnd))
    {
        GUI_NotifyMSGQ(pMsgQueue);
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_PostUserMessage
*
* author:
*
* date:             2009-11-21
*
* Description:      gui postusermessage
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32 GUI_PostUserMessage(__win_msgqueue_t *pMsgQueue, __gui_msg_t *msg)
{
    pmsg        pMsg;
    H_WIN       hWnd = msg->h_deswin;
    pMsgQueue = GUI_WinGetMsgQueue(hWnd);

    if (!pMsgQueue)
    {
        ORANGE_WRN("get windows msgqueue failed!");
        return ORANGE_FAIL;
    }

    pthread_mutex_lock(pMsgQueue->lock);

    if ((pMsgQueue->iwritepos + 1) % pMsgQueue->len == pMsgQueue->ireadpos)
    {
        pthread_mutex_unlock(pMsgQueue->lock);
        return ORANGE_QUEUE_FULL;
    }

    // Write the data and advance write pointer */
    pMsg                = &(pMsgQueue->pPaintMsg[pMsgQueue->iwritepos]);
    pMsg->h_deswin      = hWnd;
    pMsg->id            = msg->id;
    pMsg->dwAddData1    = msg->dwAddData1;
    pMsg->dwAddData2    = msg->dwAddData2;
    pMsg->dwReserved    = msg->dwReserved;
    pMsgQueue->iwritepos++;

    if (pMsgQueue->iwritepos >= pMsgQueue->len)
    {
        pMsgQueue->iwritepos = 0;
    }

    if (msg->id == GUI_MSG_PAINT)
    {
        pMsgQueue->dwState |= QS_PAINT;
        goto goodret;
    }
    else
    {
        pMsgQueue->dwState |= QS_USERASYNC;
    }

goodret:
    pthread_mutex_unlock(pMsgQueue->lock);

    if (!be_this_thread(hWnd))
    {
        GUI_NotifyMSGQ(pMsgQueue);
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_PostMessage
*
* author:
*
* date:             2009-7-21
*
* Description:      gui postmessage
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_PostMessage(__gui_msg_t *msg)
{
    pmsgqueue   pMsgQueue;
    H_WIN       hWnd = msg->h_deswin;
    pMsgQueue = GUI_WinGetMsgQueue(hWnd);
    if (!pMsgQueue)
    {
        ORANGE_WRN("get windows msgqueue failed!");
        return ORANGE_FAIL;
    }

    if (msg->id == GUI_MSG_KEY || msg->id == GUI_MSG_TOUCH)
    {
        return  GUI_PostInputMessage(pMsgQueue, msg);
    }
    else
    {
        return  GUI_PostUserMessage(pMsgQueue, msg);
    }
}

/*
**********************************************************************************************************************
*                                               GUI_SendNotifyMessage
*
* author:
*
* date:             2009-7-21
*
* Description:      gui sendnotifymessage
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_SendNotifyMessage(__gui_msg_t *msg)
{
    __win_msgqueue_t   *pMsgQueue;
    __gui_qmsg_t       *pqmsg;
    H_WIN       hWnd = msg->h_deswin;

    if (hWnd == HWND_INVALID)
    {
        return ORANGE_FAIL;
    }

    pMsgQueue = GUI_WinGetMsgQueue(hWnd);

    if (!pMsgQueue)
    {
        ORANGE_WRN("get windows msgqueue failed!");
        return ORANGE_FAIL;
    }

    pqmsg = QMsgMemAlloc();
    pthread_mutex_lock(pMsgQueue->lock);
    // queue the sync message.
    pqmsg->msg.h_deswin     = hWnd;
    pqmsg->msg.id           = msg->id;
    pqmsg->msg.dwAddData1   = msg->dwAddData1;
    pqmsg->msg.dwAddData2   = msg->dwAddData2;
    pqmsg->msg.dwReserved   = msg->dwReserved;
    pqmsg->msg.h_srcwin     = msg->h_srcwin;
    pqmsg->next             = NULL;

    if (pMsgQueue->pFirstNotifyMsg == NULL)
    {
        pMsgQueue->pFirstNotifyMsg = pMsgQueue->pLastNotifyMsg = pqmsg;
    }
    else
    {
        pMsgQueue->pLastNotifyMsg->next = pqmsg;
        pMsgQueue->pLastNotifyMsg = pqmsg;
    }

    pMsgQueue->dwState |= QS_NOTIFYMSG;
    pthread_mutex_unlock(pMsgQueue->lock);

    if (!be_this_thread(hWnd))
    {
        GUI_NotifyMSGQ(pMsgQueue);
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_GetMessage
*
* author:
*
* date:             2009-7-21
*
* Description:      gui getmessage
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_GetMessage(__gui_msg_t *msg, H_WIN  hManWin)
{
    __win_msgqueue_t   *pMsgQueue;
    __gui_qmsg_t       *phead;
    __gui_win_t        *pWin;
    __gui_win_t        *pMainWin;
    pMsgQueue = GUI_WinGetMsgQueue(hManWin);

    if (!pMsgQueue)
    {
        ORANGE_WRN("get windows msgqueue failed!");
        return ORANGE_FAIL;
    }

    pWin        =   GUI_WinH2P(hManWin);

    if ((pWin->WinType != TYPE_FRAMEWIN)
        && (pWin->WinType != TYPE_MANWIN))
    {
        ORANGE_WRN("windows type error!");
        return ORANGE_FAIL;
    }

    pMainWin    =   GUI_WinH2P(GUI_WinGetMainManWin(hManWin));
    orange_memset(msg, 0, sizeof(__gui_msg_t));
checkagain:

    if (pMainWin->Primitive != pWin)
    {
        pMainWin->Primitive = pWin;
        GUI_InsertLoopWin(pMsgQueue, GUI_WinP2H(pWin));
    }

    if (pMsgQueue->dwState & QS_QUIT)
    {
        __u32 i;

        for (i = 0; i < DEF_MAX_QUIT_WIN; i++)
        {
            if (pMsgQueue->quit_win[i])
            {
                /*判断需要退出消息循环的窗口，是不是正在循环的窗口
                  对frmwin而言，消息队列寄生在mainwin上,可以有自己的消息循环
                  也可以没有，对消息循环frmwin而言，此时退出消息循环,否则清楚窗口信息*/
                if (hManWin == pMsgQueue->quit_win[i])
                {
                    msg->h_deswin   = hManWin;
                    msg->id         = GUI_MSG_QUIT;
                    msg->dwAddData1 = 0;
                    msg->dwAddData2 = 0;
                    msg->dwReserved = 0;
                    msg->p_arg      = NULL;

                    if (GUI_WinP2H(pMainWin->Primitive) == hManWin)
                    {
                        pMainWin->Primitive = NULL;
                    }

                    GUI_DeleteLoopWin(pMsgQueue, pMsgQueue->quit_win[i]);
                    pMsgQueue->quit_win[i] = 0;
                    return 0;
                }

                if (!GUI_QueryLoopWin(pMsgQueue, pMsgQueue->quit_win[i]))
                {
                    GUI_WinThreadCleanup(pMsgQueue->quit_win[i]);
                    pMsgQueue->quit_win[i] = 0;
                }
            }
        }

        if ((i == DEF_MAX_QUIT_WIN)
            && (!GUI_QueryQuitWin(pMsgQueue)))
        {
            pMsgQueue->dwState &= ~QS_QUIT;
        }

        return 1;
    }

    if (pMsgQueue->dwState & QS_NOTIFYMSG)
    {
        pthread_mutex_lock(pMsgQueue->lock);

        if (pMsgQueue->pFirstNotifyMsg)
        {
            phead = pMsgQueue->pFirstNotifyMsg;
            pMsgQueue->pFirstNotifyMsg = phead->next;
            *msg = phead->msg;
            msg->p_arg = NULL;
            FreeQMsgMem(phead);
            pthread_mutex_unlock(pMsgQueue->lock);
            return 1;
        }
        else
        {
            pMsgQueue->dwState &= ~QS_NOTIFYMSG;
        }

        pthread_mutex_unlock(pMsgQueue->lock);
    }

    if (pMsgQueue->dwState & QS_SYNCMSG)
    {
        pthread_mutex_lock(pMsgQueue->lock);

        if (pMsgQueue->pFirstSyncMsg)
        {
            *msg = pMsgQueue->pFirstSyncMsg->msg;
            msg->p_arg = pMsgQueue->pFirstSyncMsg;
            pMsgQueue->pFirstSyncMsg = pMsgQueue->pFirstSyncMsg->pNext;
            pthread_mutex_unlock(pMsgQueue->lock);
            return 1;
        }
        else
        {
            pMsgQueue->dwState &= ~QS_SYNCMSG;
        }

        pthread_mutex_unlock(pMsgQueue->lock);
    }

    if (pMsgQueue->dwState & QS_USERASYNC)
    {
        pthread_mutex_lock(pMsgQueue->lock);

        if (pMsgQueue->ireadpos != pMsgQueue->iwritepos)
        {
            *msg = pMsgQueue->pPaintMsg[pMsgQueue->ireadpos];
            msg->p_arg = NULL;
            pMsgQueue->ireadpos++;

            if (pMsgQueue->ireadpos >= pMsgQueue->len)
            {
                pMsgQueue->ireadpos = 0;
            }

            pthread_mutex_unlock(pMsgQueue->lock);
            return 1;
        }
        else
        {
            pMsgQueue->dwState &= ~QS_USERASYNC;
        }

        pthread_mutex_unlock(pMsgQueue->lock);
    }

    if (pMsgQueue->dwState & QS_POSTMSG)
    {
        pthread_mutex_lock(pMsgQueue->lock);

        if (pMsgQueue->readpos != pMsgQueue->writepos)
        {
            __bool isChild;
            *msg = pMsgQueue->pAsyncMsg[pMsgQueue->readpos];
            /*判断当前找到的frmwin是否在消息循环窗口的子孙窗口中*/
            isChild = GUI_WinIsAncestor(msg->h_deswin, GUI_WinP2H(pMainWin->Primitive));

            if (isChild)
            {
                /*修改外部消息目标窗口，直接传给消息循环窗口*/
                msg->h_deswin = GUI_WinP2H(pMainWin->Primitive);
            }

            msg->p_arg = NULL;
            pMsgQueue->readpos++;

            if (pMsgQueue->readpos >= pMsgQueue->len)
            {
                pMsgQueue->readpos = 0;
            }

            pthread_mutex_unlock(pMsgQueue->lock);
            return 1;
        }
        else
        {
            pMsgQueue->dwState &= ~QS_POSTMSG;
        }

        pthread_mutex_unlock(pMsgQueue->lock);
    }

    if (pMsgQueue->dwState & QS_PAINT)
    {
        if (hManWin == GUI_WinGetRootWin())
        {
            msg->h_deswin   = hManWin;
            msg->id         = GUI_MSG_PAINT;
            msg->dwAddData1 = 0;
            msg->dwAddData2 = 0;
            msg->dwReserved = 0;
            msg->p_arg      = NULL;
            pthread_mutex_lock(pMsgQueue->lock);
            pMsgQueue->dwState &= ~QS_PAINT;
            pthread_mutex_unlock(pMsgQueue->lock);
            return 1;
        }

        pthread_mutex_lock(pMsgQueue->lock);

        if (pMsgQueue->readpos != pMsgQueue->writepos)
        {
            *msg = pMsgQueue->pAsyncMsg[pMsgQueue->readpos];
            msg->p_arg = NULL;
            pMsgQueue->readpos++;

            if (pMsgQueue->readpos >= pMsgQueue->len)
            {
                pMsgQueue->readpos = 0;
            }

            pthread_mutex_unlock(pMsgQueue->lock);
            return 1;
        }
        else
        {
            pMsgQueue->dwState &= ~QS_POSTMSG;
        }

        //msg->id         = GUI_MSG_PAINT;
        //msg->dwAddData1 = 0;
        //msg->dwAddData2 = 0;
        //msg->p_arg       = NULL;
        pMsgQueue->dwState &= ~QS_PAINT;
        pthread_mutex_unlock(pMsgQueue->lock);
    }

    if (pMsgQueue->TimerMask)
    {
        __s32           slot;
        __gui_timer_t    *timer;
        pthread_mutex_lock(pMsgQueue->lock);
        /* 获取第一个过期的timer */
        slot = pMsgQueue->FirstTimerSlot;

        do
        {
            if (pMsgQueue->TimerMask & (0x01 << slot))
            {
                break;
            }

            slot ++;
            slot %= DEF_NR_TIMERS;

            if (slot == pMsgQueue->FirstTimerSlot)
            {
                slot = 0xffffffff;
                break;
            }
        } while (ORANGE_TRUE);

        pMsgQueue->FirstTimerSlot ++;
        pMsgQueue->FirstTimerSlot %= DEF_NR_TIMERS;
        timer = gui_get_timer(slot);

        if (timer)
        {
            __u32 tick_count = timer->tick_count;
            timer->tick_count = 0;
            pMsgQueue->TimerMask &= ~(0x01 << slot);

            if (timer->proc)
            {
                __u32 ret_timer_proc;
                /* unlock the message queue when calling timer proc */
                pthread_mutex_unlock(pMsgQueue->lock);
                {
                    __gui_timer_procpara_t  para;
                    para.hWnd       = timer->hWnd;
                    para.id         = timer->id;
                    para.dwAddData  = tick_count;
                    para.dwReserved = 0;
                    /* calling the timer callback procedure */
                    ret_timer_proc = esKRNL_CallBack((__pCBK_t) * timer->proc, &para);
                }
                /* lock the message queue again */
                pthread_mutex_lock(pMsgQueue->lock);

                if (!ret_timer_proc)
                {
                    /* remove the timer */
                    gui_remove_timer(timer, slot);
                }
            }
            else
            {
                msg->id         = GUI_MSG_TIMER;
                msg->h_deswin   = timer->hWnd;
                msg->h_srcwin   = 0;
                msg->dwAddData1 = timer->id;
                msg->dwAddData2 = tick_count;
                msg->dwReserved = 0;
                msg->p_arg      = NULL;
                pthread_mutex_unlock(pMsgQueue->lock);
                return ORANGE_TRUE;
            }
        }

        pthread_mutex_unlock(pMsgQueue->lock);
    }

    // no message, wait again.
    sem_wait(&pMsgQueue->wait);
    //orange_Dalay(1);
    goto checkagain;
}


/*
**********************************************************************************************************************
*                                               GUI_GetMessageEx
*
* author:
*
* date:             2009-11-12
*
* Description:      gui getmessageex
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_GetMessageEx(__gui_msg_t *msg, H_WIN  hManWin)
{
    __win_msgqueue_t   *pMsgQueue;
    __gui_qmsg_t       *phead;
    __gui_win_t        *pWin;
    __gui_win_t        *pMainWin;
    pMsgQueue = GUI_WinGetMsgQueue(hManWin);

    if (!pMsgQueue)
    {
        ORANGE_WRN("get windows msgqueue failed!");
        return ORANGE_FAIL;
    }

    pWin        =   GUI_WinH2P(hManWin);

    if ((pWin->WinType != TYPE_FRAMEWIN)
        && (pWin->WinType != TYPE_MANWIN))
    {
        ORANGE_WRN("windows type error!");
        return ORANGE_FAIL;
    }

    pMainWin    =   GUI_WinH2P(GUI_WinGetMainManWin(hManWin));
    orange_memset(msg, 0, sizeof(__gui_msg_t));
checkagain:

    if (pMainWin->Primitive != pWin)
    {
        pMainWin->Primitive = pWin;
        GUI_InsertLoopWin(pMsgQueue, GUI_WinP2H(pWin));
    }

    if (pMsgQueue->dwState & QS_QUIT)
    {
        __u32 i;

        for (i = 0; i < DEF_MAX_QUIT_WIN; i++)
        {
            if (pMsgQueue->quit_win[i])
            {
                /*判断需要退出消息循环的窗口，是不是正在循环的窗口
                  对frmwin而言，消息队列寄生在mainwin上,可以有自己的消息循环
                  也可以没有，对消息循环frmwin而言，此时退出消息循环,否则清楚窗口信息*/
                if (hManWin == pMsgQueue->quit_win[i])
                {
                    msg->h_deswin   = hManWin;
                    msg->id         = GUI_MSG_QUIT;
                    msg->dwAddData1 = 0;
                    msg->dwAddData2 = 0;
                    msg->dwReserved = 0;
                    msg->p_arg      = NULL;

                    if (GUI_WinP2H(pMainWin->Primitive) == hManWin)
                    {
                        pMainWin->Primitive = NULL;
                    }

                    GUI_DeleteLoopWin(pMsgQueue, pMsgQueue->quit_win[i]);
                    pMsgQueue->quit_win[i] = 0;
                    return 0;
                }

                if (!GUI_QueryLoopWin(pMsgQueue, pMsgQueue->quit_win[i]))
                {
                    GUI_WinThreadCleanup(pMsgQueue->quit_win[i]);
                    pMsgQueue->quit_win[i] = 0;
                }
            }
        }

        if ((i == DEF_MAX_QUIT_WIN)
            && (!GUI_QueryQuitWin(pMsgQueue)))
        {
            pMsgQueue->dwState &= ~QS_QUIT;
        }

        return 1;
    }

    if (pMsgQueue->dwState & QS_NOTIFYMSG)
    {
        pthread_mutex_lock(pMsgQueue->lock);

        if (pMsgQueue->pFirstNotifyMsg)
        {
            phead = pMsgQueue->pFirstNotifyMsg;
            pMsgQueue->pFirstNotifyMsg = phead->next;
            *msg = phead->msg;
            msg->p_arg = NULL;
            FreeQMsgMem(phead);
            pthread_mutex_unlock(pMsgQueue->lock);
            return 1;
        }
        else
        {
            pMsgQueue->dwState &= ~QS_NOTIFYMSG;
        }

        pthread_mutex_unlock(pMsgQueue->lock);
    }

    if (pMsgQueue->dwState & QS_SYNCMSG)
    {
        pthread_mutex_lock(pMsgQueue->lock);

        if (pMsgQueue->pFirstSyncMsg)
        {
            *msg = pMsgQueue->pFirstSyncMsg->msg;
            msg->p_arg = pMsgQueue->pFirstSyncMsg;
            pMsgQueue->pFirstSyncMsg = pMsgQueue->pFirstSyncMsg->pNext;
            pthread_mutex_unlock(pMsgQueue->lock);
            return 1;
        }
        else
        {
            pMsgQueue->dwState &= ~QS_SYNCMSG;
        }

        pthread_mutex_unlock(pMsgQueue->lock);
    }

    if (pMsgQueue->dwState & QS_USERASYNC)
    {
        pthread_mutex_lock(pMsgQueue->lock);

        if (pMsgQueue->ireadpos != pMsgQueue->iwritepos)
        {
            *msg = pMsgQueue->pPaintMsg[pMsgQueue->ireadpos];
            msg->p_arg = NULL;
            pMsgQueue->ireadpos++;

            if (pMsgQueue->ireadpos >= pMsgQueue->len)
            {
                pMsgQueue->ireadpos = 0;
            }

            pthread_mutex_unlock(pMsgQueue->lock);
            return 1;
        }
        else
        {
            pMsgQueue->dwState &= ~QS_USERASYNC;
        }

        pthread_mutex_unlock(pMsgQueue->lock);
    }

    if (pMsgQueue->dwState & QS_POSTMSG)
    {
        __bool  ret;
        __bool  isChild;
        ret = GUI_PostMsgHandle(pMsgQueue, msg);

        if (ret == ORANGE_TRUE)
        {
            /*判断当前找到的frmwin是否在消息循环窗口的子孙窗口中*/
            isChild = GUI_WinIsAncestor(msg->h_deswin, GUI_WinP2H(pMainWin->Primitive));

            if (isChild)
            {
                /*修改外部消息目标窗口，直接传给消息循环窗口*/
                msg->h_deswin = GUI_WinP2H(pMainWin->Primitive);
            }

            return ORANGE_TRUE;
        }
    }

    if (pMsgQueue->dwState & QS_PAINT)
    {
        if (hManWin == GUI_WinGetRootWin())
        {
            msg->h_deswin   = hManWin;
            msg->id         = GUI_MSG_PAINT;
            msg->dwAddData1 = 0;
            msg->dwAddData2 = 0;
            msg->dwReserved = 0;
            msg->p_arg      = NULL;
            pthread_mutex_lock(pMsgQueue->lock);
            pMsgQueue->dwState &= ~QS_PAINT;
            pthread_mutex_unlock(pMsgQueue->lock);
            return 1;
        }

        pthread_mutex_lock(pMsgQueue->lock);

        if (pMsgQueue->readpos != pMsgQueue->writepos)
        {
            *msg = pMsgQueue->pAsyncMsg[pMsgQueue->readpos];
            msg->p_arg = NULL;
            pMsgQueue->readpos++;

            if (pMsgQueue->readpos >= pMsgQueue->len)
            {
                pMsgQueue->readpos = 0;
            }

            pthread_mutex_unlock(pMsgQueue->lock);
            return 1;
        }
        else
        {
            pMsgQueue->dwState &= ~QS_POSTMSG;
        }

        //msg->id         = GUI_MSG_PAINT;
        //msg->dwAddData1 = 0;
        //msg->dwAddData2 = 0;
        //msg->p_arg       = NULL;
        pMsgQueue->dwState &= ~QS_PAINT;
        pthread_mutex_unlock(pMsgQueue->lock);
    }

    if (pMsgQueue->TimerMask)
    {
        __s32           slot;
        __gui_timer_t    *timer;
        pthread_mutex_lock(pMsgQueue->lock);
        /* 获取第一个过去的timer */
        slot = pMsgQueue->FirstTimerSlot;

        do
        {
            if (pMsgQueue->TimerMask & (0x01 << slot))
            {
                break;
            }

            slot ++;
            slot %= DEF_NR_TIMERS;

            if (slot == pMsgQueue->FirstTimerSlot)
            {
                slot = 0xffffffff;
                break;
            }
        } while (ORANGE_TRUE);

        pMsgQueue->FirstTimerSlot ++;
        pMsgQueue->FirstTimerSlot %= DEF_NR_TIMERS;
        timer = gui_get_timer(slot);

        if (timer)
        {
            __u32 tick_count = timer->tick_count;
            timer->tick_count = 0;
            pMsgQueue->TimerMask &= ~(0x01 << slot);

            if (timer->proc)
            {
                __u32 ret_timer_proc;
                /* unlock the message queue when calling timer proc */
                pthread_mutex_unlock(pMsgQueue->lock);
                {
                    __gui_timer_procpara_t  para;
                    para.hWnd       = timer->hWnd;
                    para.id         = timer->id;
                    para.dwAddData  = tick_count;
                    para.dwReserved = 0;
                    /* calling the timer callback procedure */
                    ret_timer_proc = esKRNL_CallBack((__pCBK_t) * timer->proc, &para);
                }
                /* lock the message queue again */
                pthread_mutex_lock(pMsgQueue->lock);

                if (!ret_timer_proc)
                {
                    /* remove the timer */
                    gui_remove_timer(timer, slot);
                }
            }
            else
            {
                msg->id         = GUI_MSG_TIMER;
                msg->h_deswin   = timer->hWnd;
                msg->h_srcwin   = 0;
                msg->dwAddData1 = timer->id;
                msg->dwAddData2 = tick_count;
                msg->dwReserved = 0;
                msg->p_arg      = NULL;
                pthread_mutex_unlock(pMsgQueue->lock);
                return ORANGE_TRUE;
            }
        }

        pthread_mutex_unlock(pMsgQueue->lock);
    }

    // no message, wait again.
    sem_wait(&pMsgQueue->wait);
    //orange_Dalay(1);
    goto checkagain;
}

/*
**********************************************************************************************************************
*                                               GUI_QueryMessage
*
* author:
*
* date:             2009-10-22
*
* Description:      gui querymessage
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool GUI_QueryMessage(H_WIN  hManWin)
{
    __win_msgqueue_t   *pMsgQueue;
    pMsgQueue = GUI_WinGetMsgQueue(hManWin);

    if (!pMsgQueue)
    {
        __wrn("get windows msgqueue failed!");
        return ORANGE_FALSE;
    }

    pthread_mutex_lock(pMsgQueue->lock);

    if (pMsgQueue->dwState && pMsgQueue->readpos != pMsgQueue->writepos)
    {
        pthread_mutex_unlock(pMsgQueue->lock);
        return ORANGE_TRUE;
    }

    pthread_mutex_unlock(pMsgQueue->lock);
    return ORANGE_FALSE;
}


/*
**********************************************************************************************************************
*                                               GUI_QueueMessage
*
* author:
*
* date:             2009-10-30
*
* Description:      gui queuemessage
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
/* post a message to a message queue */
__bool GUI_QueueMessage(__win_msgqueue_t *msg_que, __gui_msg_t *msg)
{
    pthread_mutex_lock(msg_que->lock);

    if ((msg_que->writepos + 1) % msg_que->len == msg_que->readpos)
    {
        pthread_mutex_unlock(msg_que->lock);
        return ORANGE_FALSE;
    }

    /* Write the data and advance write pointer */
    msg_que->pAsyncMsg [msg_que->writepos] = *msg;
    msg_que->writepos++;

    if (msg_que->writepos >= msg_que->len)
    {
        msg_que->writepos = 0;
    }

    msg_que->dwState |= QS_POSTMSG;
    pthread_mutex_unlock(msg_que->lock);

    if (!be_this_thread(msg->h_deswin))
    {
        GUI_NotifyMSGQ(msg_que);
    }

    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               GUI_SetLongCount
*
* author:
*
* date:             2009-11-18
*
* Description:      gui setlongcount
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_MsgSetRepeatTimes(H_WIN  hManWin, __u32 count)
{
    __win_msgqueue_t   *pMsgQueue;
    pMsgQueue = GUI_WinGetMsgQueue(hManWin);

    if (!pMsgQueue)
    {
        ORANGE_WRN("get windows msgqueue failed!");
        return ORANGE_FAIL;
    }

    if (count > 100 || count < 1)
    {
        pMsgQueue->longcount = LONGKEY_COUNT;
    }
    else
    {
        pMsgQueue->longcount = count;
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_PeekMessage
*
* author:
*
* date:             2009-10-21
*
* Description:      gui peekmessage
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_PeekMessage(__gui_msg_t *msg, H_WIN  hManWin, __u32 len)
{
    __win_msgqueue_t   *pMsgQueue;
    //__gui_qmsg_t*       phead;
    __u32               count = 0;
    pMsgQueue = GUI_WinGetMsgQueue(hManWin);

    if (!pMsgQueue)
    {
        ORANGE_WRN("get windows msgqueue failed!");
        return ORANGE_FAIL;
    }

    if (!msg)
    {
        ORANGE_WRN("input msg error!");
        return ORANGE_FAIL;
    }

    orange_memset(msg, 0, sizeof(__gui_msg_t) * len);

    if (len > pMsgQueue->len)
    {
        len = pMsgQueue->len;
    }

    pthread_mutex_lock(pMsgQueue->lock);

    for (count = 0; count < len; count ++)
    {
        if (pMsgQueue->readpos != pMsgQueue->writepos)
        {
            *msg = pMsgQueue->pAsyncMsg[pMsgQueue->readpos];
            msg->p_arg = NULL;
            msg++;
            pMsgQueue->readpos++;

            if (pMsgQueue->readpos >= pMsgQueue->len)
            {
                pMsgQueue->readpos = 0;
            }
        }
        else
        {
            pMsgQueue->dwState &= ~QS_POSTMSG;
            break;
        }
    }

    pthread_mutex_unlock(pMsgQueue->lock);
    return count;
}

/*
**********************************************************************************************************************
*                                               GUI_DispatchMessage
*
* author:
*
* date:             2009-7-21
*
* Description:      gui dispatchmessage
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_DispatchMessage(__gui_msg_t *msg)
{
    __pGUI_WIN_CB     WndProc;
    //psyncmsg          pSyncMsg;

    if (msg->h_deswin == HWND_INVALID)
    {
        /*if (msg->p_arg)
        {
            pSyncMsg = (psyncmsg)msg->p_arg;
            pSyncMsg->retval = HWND_INVALID;
            pthread_mutex_unlock(pSyncMsg->sem_handle);
        }*/
        return HWND_INVALID;
    }

    if (msg->id == GUI_MSG_TOUCH)
    {
        /*横竖屏坐标方向转换*/
        if ((msg->dwAddData1 == GUI_MSG_TOUCH_UP)
            || (msg->dwAddData1 == GUI_MSG_TOUCH_OVERUP))
        {
            __u8        LogDir;
            __s16       PhyDir;
            __s16       speed;
            PhyDir = HISWORD(msg->dwReserved);
            speed  = LOSWORD(msg->dwReserved);
            LogDir = GUI_TouchPhyDirToLogDir(gui_scndir, PhyDir);
            msg->dwReserved = MAKELONG(speed, LogDir);
        }
    }

    WndProc = GUI_WinGetCallback(msg->h_deswin);
    /* if ( msg->p_arg ) // this is a sync message.
     {
         pSyncMsg = (psyncmsg)msg->p_arg;
         pSyncMsg->retval = (*WndProc)(msg);

         pthread_mutex_unlock(pSyncMsg->sem_handle);

         return pSyncMsg->retval;
     }*/
    return esKRNL_CallBack((__pCBK_t) * WndProc, msg);
}

/*
**********************************************************************************************************************
*                                               GUI_PostQuitMessage
*
* author:
*
* date:             2009-7-21
*
* Description:      gui postquitmessage
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_PostQuitMessage(H_WIN  hWnd)
{
    __u32       i;
    pmsgqueue   pMsgQueue;
    pMsgQueue = GUI_WinGetMsgQueue(hWnd);

    if (!pMsgQueue)
    {
        ORANGE_WRN("get message queue failed!");
        return ORANGE_FAIL;
    }

    for (i = 0; i < DEF_MAX_QUIT_WIN; i++)
    {
        if (pMsgQueue->quit_win[i] == 0)
        {
            pMsgQueue->quit_win[i] = hWnd;
            break;
        }
        else if (pMsgQueue->quit_win[i] == hWnd)
        {
            break;
        }
    }

    pMsgQueue->dwState |= QS_QUIT;

    if (!be_this_thread(hWnd))
    {
        GUI_NotifyMSGQ(pMsgQueue);
    }

    return ORANGE_OK;
}


/*
**********************************************************************************************************************
*                                               GUI_SendAsyncMessage
*
* author:
*
* date:             2009-7-22
*
* Description:      发送消息(不管发送接收是否在同一个线程，都直接执行callback函数
                    用来提供特别的系统服务)
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_SendAsyncMessage(__gui_msg_t *msg)
{
    __pGUI_WIN_CB   WndProc;

    if (msg->h_deswin == HWND_INVALID)
    {
        ORANGE_WRN("error windows handle!");
        return ORANGE_FAIL;
    }

    WndProc = GUI_WinGetCallback(msg->h_deswin);
    return esKRNL_CallBack((__pCBK_t) * WndProc, msg);
}

/*
**********************************************************************************************************************
*                                               GUI_ThrowAwayMessages
*
* author:
*
* date:             2009-7-23
*
* Description:      gui throwawaymessages
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_ThrowAwayMessages(H_WIN hWnd)
{
    pmsg            pMsg;
    pmsgqueue       pMsgQueue;
    pqmsg           pQMsg;
    psyncmsg        pSyncMsg;
    __s32           nCount = 0;
    __s32           readpos;
    pMsgQueue = GUI_WinGetMsgQueue(hWnd);

    if (!pMsgQueue)
    {
        ORANGE_WRN("get msg queue failed!");
        return ORANGE_FAIL;
    }

    pthread_mutex_lock(pMsgQueue->lock);

    if (pMsgQueue->pFirstNotifyMsg)
    {
        pQMsg = pMsgQueue->pFirstNotifyMsg;

        while (pQMsg)
        {
            pMsg = &pQMsg->msg;

            if (pMsg->h_deswin == HWND_INVALID)
            {
            }
            else if (pMsg->h_deswin == hWnd
                     || GUI_WinGetManWin(pMsg->h_deswin) == hWnd
                     || GUI_WinGetFrmWin(pMsg->h_deswin) == hWnd)
            {
                pMsg->h_deswin = HWND_INVALID;
                nCount ++;
            }

            pQMsg = pQMsg->next;
        }
    }

    /* FIXME: notify the waiting window */
    if (pMsgQueue->pFirstSyncMsg)
    {
        pSyncMsg = pMsgQueue->pFirstSyncMsg;

        while (pSyncMsg)
        {
            pMsg = &pSyncMsg->msg;

            if (pMsg->h_deswin == HWND_INVALID)
            {
            }
            else if (pMsg->h_deswin == hWnd
                     || GUI_WinGetManWin(pMsg->h_deswin) == hWnd
                     || GUI_WinGetFrmWin(pMsg->h_deswin) == hWnd)
            {
                pSyncMsg->retval = ORANGE_FAIL;

                if (pSyncMsg->sem_handle)
                {
                    pthread_mutex_unlock(pSyncMsg->sem_handle);
                }

                pMsg->h_deswin = HWND_INVALID;
                nCount ++;
            }

            pSyncMsg = pSyncMsg->pNext;
        }
    }

    readpos = pMsgQueue->readpos;

    while (readpos != pMsgQueue->writepos)
    {
        pMsg = pMsgQueue->pAsyncMsg + readpos;

        if (pMsg->h_deswin == HWND_INVALID)
        {
        }
        else if (pMsg->h_deswin == hWnd
                 || GUI_WinGetManWin(pMsg->h_deswin) == hWnd
                 || GUI_WinGetFrmWin(pMsg->h_deswin) == hWnd)
        {
            pMsg->h_deswin = HWND_INVALID;
            nCount ++;
        }

        readpos++;

        if (readpos >= pMsgQueue->len)
        {
            readpos = 0;
        }
    }

    pthread_mutex_unlock(pMsgQueue->lock);
    return nCount;
}

/*
**********************************************************************************************************************
*                                               GUI_SetSyncMsgRetVal
*
* author:
*
* date:             2009-8-29
*
* Description:      gui setsyncmsgretval
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_SetSyncMsgRetVal(__gui_msg_t *msg, __s32 ret)
{
    __gui_syncmsg_t     *psyncmsg;

    if (msg->p_arg)
    {
        psyncmsg = (__gui_syncmsg_t *)msg->p_arg;
        psyncmsg->retval = ret;
    }

    return ORANGE_OK;
}


/*
**********************************************************************************************************************
*                                               GUI_PostSyncSem
*
* author:
*
* date:             2009-8-29
*
* Description:      gui postsyncsem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32    GUI_PostSyncSem(__gui_msg_t *msg)
{
    __gui_syncmsg_t     *psyncmsg;

    if (msg->p_arg)
    {
        psyncmsg = (__gui_syncmsg_t *)msg->p_arg;
        pthread_mutex_unlock(psyncmsg->sem_handle);
    }

    return ORANGE_OK;
}

#if 0

__bool GUI_PeekMessageEx(__gui_msg_t *msg, H_WIN hWnd, __s32 iMsgFilterMin, __s32 iMsgFilterMax,
                         __bool bWait, __u32 uRemoveMsg)
{
    __win_msgqueue_t   *pMsgQueue;
    __gui_qmsg_t       *phead;
    __gui_win_t        *pWin;

    if (!msg)
    {
        ORANGE_WRN("input para error!");
        return ORANGE_FALSE;
    }

    pWin        =   GUI_WinH2P(hWnd);

    if ((pWin->WinType != TYPE_FRAMEWIN)
        && (pWin->WinType != TYPE_MANWIN))
    {
        ORANGE_WRN("windows type error!");
        return ORANGE_FALSE;
    }

    pMsgQueue = GUI_WinGetMsgQueue(hWnd);

    if (!pMsgQueue)
    {
        ORANGE_WRN("get windows msgqueue failed!");
        return ORANGE_FALSE;
    }

    orange_memset(msg, 0, sizeof(__gui_msg_t));
checkagain:
    LOCK_MSGQ(pMsgQueue);

    if (pMsgQueue->dwState & QS_QUIT)
    {
        __u32 i;

        for (i = 0; i < DEF_MAX_QUIT_WIN; i++)
        {
            if (pMsgQueue->quit_win[i])
            {
                /*判断需要退出消息循环的窗口，是不是正在循环的窗口
                  对frmwin而言，消息队列寄生在mainwin上,可以有自己的消息循环
                  也可以没有，对消息循环frmwin而言，此时退出消息循环,否则清楚窗口信息*/
                if (hWnd == pMsgQueue->quit_win[i])
                {
                    msg->h_deswin   = hWnd;
                    msg->id         = GUI_MSG_QUIT;
                    msg->dwAddData1 = 0;
                    msg->dwAddData2 = 0;
                    msg->dwReserved = 0;
                    msg->p_arg      = NULL;
                    pMsgQueue->dwState &= ~QS_QUIT;
                    return 0;
                }

                GUI_WinThreadCleanup(pMsgQueue->quit_win[i]);
                pMsgQueue->quit_win[i] = 0;
            }
            else
            {
                break;
            }
        }

        if (uRemoveMsg == PM_REMOVE)
        {
            pMsgQueue->dwState &= ~QS_QUIT;
        }

        return 1;
    }

    if (pMsgQueue->dwState & QS_NOTIFYMSG)
    {
        pthread_mutex_lock(pMsgQueue->lock);

        if (pMsgQueue->pFirstNotifyMsg)
        {
            phead       = pMsgQueue->pFirstNotifyMsg;
            *msg        = phead->msg;
            msg->p_arg  = NULL;

            if (IS_MSG_WANTED(msg->id))
            {
                if (uRemoveMsg == PM_REMOVE)
                {
                    pMsgQueue->pFirstNotifyMsg = phead->next;
                    FreeQMsgMem(phead);
                }

                pthread_mutex_unlock(pMsgQueue->lock);
                return ORANGE_TRUE;
            }

            pthread_mutex_unlock(pMsgQueue->lock);
            return 1;
        }
        else
        {
            pMsgQueue->dwState &= ~QS_NOTIFYMSG;
        }

        pthread_mutex_unlock(pMsgQueue->lock);
    }

    if (pMsgQueue->dwState & QS_SYNCMSG)
    {
        pthread_mutex_lock(pMsgQueue->lock);

        if (pMsgQueue->pFirstSyncMsg)
        {
            *msg = pMsgQueue->pFirstSyncMsg->msg;
            msg->p_arg = pMsgQueue->pFirstSyncMsg;

            if (IS_MSG_WANTED(msg->id))
            {
                if (uRemoveMsg == PM_REMOVE)
                {
                    pMsgQueue->pFirstSyncMsg = pMsgQueue->pFirstSyncMsg->pNext;
                }

                pthread_mutex_unlock(pMsgQueue->lock);
                return ORANGE_TRUE;
            }

            pthread_mutex_unlock(pMsgQueue->lock);
            return 1;
        }
        else
        {
            pMsgQueue->dwState &= ~QS_SYNCMSG;
        }

        pthread_mutex_unlock(pMsgQueue->lock);
    }

    if (pMsgQueue->dwState & QS_POSTMSG)
    {
        pthread_mutex_lock(pMsgQueue->lock);

        if (pMsgQueue->readpos != pMsgQueue->writepos)
        {
            *msg = pMsgQueue->pAsyncMsg[pMsgQueue->readpos];
            msg->p_arg = NULL;

            if (IS_MSG_WANTED(msg->id))
            {
                if (uRemoveMsg == PM_REMOVE)
                {
                    pMsgQueue->readpos++;

                    if (pMsgQueue->readpos >= pMsgQueue->len)
                    {
                        pMsgQueue->readpos = 0;
                    }
                }

                pthread_mutex_unlock(pMsgQueue->lock);
                return ORANGE_TRUE;
            }

            pthread_mutex_unlock(pMsgQueue->lock);
            return ORANGE_TRUE;
        }
        else
        {
            pMsgQueue->dwState &= ~QS_POSTMSG;
        }

        pthread_mutex_unlock(pMsgQueue->lock);
    }

    /*
     * check invalidate region of the windows
     */

    if (pMsgQueue->dwState & QS_PAINT && IS_MSG_WANTED(MSG_PAINT))
    {
        PMAINWIN pHostingRoot;
        HWND hNeedPaint;
        PMAINWIN pWin;

        /* REMIND this */
        if (hWnd == HWND_DESKTOP)
        {
            pMsg->hwnd = hWnd;
            pMsg->message = MSG_PAINT;
            pMsg->wParam = 0;
            pMsg->lParam = 0;
            SET_PADD(NULL);

            if (uRemoveMsg == PM_REMOVE)
            {
                pMsgQueue->dwState &= ~QS_PAINT;
            }

            UNLOCK_MSGQ(pMsgQueue);
            return TRUE;
        }

        pMsg->message = MSG_PAINT;
        pMsg->wParam = 0;
        pMsg->lParam = 0;
        SET_PADD(NULL);
        pHostingRoot = pMsgQueue->pRootMainWin;

        if ((hNeedPaint = msgCheckHostedTree(pHostingRoot)))
        {
            pMsg->hwnd = hNeedPaint;
            pWin = (PMAINWIN) hNeedPaint;
            pMsg->lParam = (LPARAM)(&pWin->InvRgn.rgn);
            UNLOCK_MSGQ(pMsgQueue);
            return TRUE;
        }

        /* no paint message */
        pMsgQueue->dwState &= ~QS_PAINT;
    }

    if (pMsgQueue->TimerMask && IS_MSG_WANTED(MSG_TIMER))
    {
        int slot;
        TIMER *timer;

        if (hWnd == HWND_DESKTOP)
        {
            pMsg->hwnd = hWnd;
            pMsg->message = MSG_TIMER;
            pMsg->wParam = 0;
            pMsg->lParam = 0;
            SET_PADD(NULL);

            if (uRemoveMsg == PM_REMOVE)
            {
                pMsgQueue->TimerMask = 0;
            }

            UNLOCK_MSGQ(pMsgQueue);
            return TRUE;
        }

        /* get the first expired timer slot */
        slot = pMsgQueue->FirstTimerSlot;

        do
        {
            if (pMsgQueue->TimerMask & (0x01 << slot))
            {
                break;
            }

            slot ++;
            slot %= DEF_NR_TIMERS;

            if (slot == pMsgQueue->FirstTimerSlot)
            {
                slot = -1;
                break;
            }
        } while (TRUE);

        pMsgQueue->FirstTimerSlot ++;
        pMsgQueue->FirstTimerSlot %= DEF_NR_TIMERS;

        if ((timer = __mg_get_timer(slot)))
        {
            unsigned int tick_count = timer->tick_count;
            timer->tick_count = 0;
            pMsgQueue->TimerMask &= ~(0x01 << slot);

            if (timer->proc)
            {
                BOOL ret_timer_proc;
                /* unlock the message queue when calling timer proc */
                UNLOCK_MSGQ(pMsgQueue);
                /* calling the timer callback procedure */
                ret_timer_proc = timer->proc(timer->hWnd,
                                             timer->id, tick_count);
                /* lock the message queue again */
                LOCK_MSGQ(pMsgQueue);

                if (!ret_timer_proc)
                {
                    /* remove the timer */
                    __mg_remove_timer(timer, slot);
                }
            }
            else
            {
                pMsg->message = MSG_TIMER;
                pMsg->hwnd = timer->hWnd;
                pMsg->wParam = timer->id;
                pMsg->lParam = tick_count;
                SET_PADD(NULL);
                UNLOCK_MSGQ(pMsgQueue);
                return TRUE;
            }
        }
    }

    UNLOCK_MSGQ(pMsgQueue);

    if (bWait)
    {
        /* no message, wait again. */
        sem_wait(&pMsgQueue->wait);
        goto checkagain;
    }

    /* no message */
    return FALSE;
}
#endif

