/*
*******************************************************************************************************************
*                                                           Graphic Library
*                                                   the graphic library support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name:       ListBox_Process.c
*
*Author:          William Wang
*
*Version :        1.1.0
*
*Date:            2008-10-20
*
*Description :    ListLayer Process implement
*
*Others :         None at present.
*
* History :
*
* <Author>          <time>              <version>     <description>
*
* William Wang     2008-10-20    1.1.0          Create File
*
*******************************************************************************************************************
*/

#ifndef __LIST_LAYER_PROCESS_C____
#define __LIST_LAYER_PROCESS_C____


#include "config.h"
#include "ListMenu_i.h"
#include "ListLayer.h"
#include "ListBox.h"
#include "MenuList.h"
#include "MenuItem.h"
#include "ListLayer_Process.h"
#include "ListBox_Process.h"
#include "sysmemory.h"

/*
**********************************************************************************************************************
*                                               ListBox_GetBufferStatus
*
* author:
*
* date:             2008-10-8
*
* Description:      获取加载的状态信息
*
* parameters:       listlayer 当前使用的listlayer信息
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListBox_GetBufferStatus(LIST_LAYER_MAN_t    *listlayer)
{
    __s32                      logic_tail = 0;
    __s32                      logic_head = 0;
    __s32                      logic_pos = 0;
    __s32                      head_dist = 0;
    __s32                      tail_dist = 0;
    __u8                       err;
    __u32                      status = 0;

    /*当前listmenu的模式为固定数量的模式*/
    if (listlayer->mode == FIXED_MODE || listlayer->menulist->ItemCount == 0)
    {
        status = 0;
        return status;
    }

    /*当前listmenu的模式为可变数量的模式*/
    esKRNL_SemPend(listlayer->pos_sem, 0, &err); /*pend pos semphore*/

    if (err == OS_NO_ERR)
    {
        esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/

        if (listlayer->viewrect.y > listlayer->tail_pos.y)
        {
            /*设置逻辑尾*/
            logic_tail = listlayer->tail_pos.y + listlayer->listbuffer->vrect.height;
        }
        else
        {
            /*设置逻辑尾*/
            logic_tail = listlayer->tail_pos.y;
        }

        if (listlayer->viewrect.y < listlayer->head_pos.y)
        {
            /*设置逻辑头*/
            logic_head = listlayer->head_pos.y - listlayer->listbuffer->vrect.height;
        }
        else
        {
            /*设置逻辑头*/
            logic_head = listlayer->head_pos.y;
        }

        logic_pos = listlayer->viewrect.y;
        /*获取头位移*/
        head_dist = (logic_pos - logic_head) -
                    (logic_pos - logic_head) % (listlayer->menulist->head->rect.height);

        /*非对齐的情况*/
        if ((logic_tail - logic_pos) % (listlayer->menulist->head->rect.height))
        {
            /*获取尾位移*/
            tail_dist = (logic_tail - logic_pos) -
                        (logic_tail - logic_pos) % (listlayer->menulist->head->rect.height) + listlayer->menulist->head->rect.height;
        }
        else
        {
            /*获取尾位移*/
            tail_dist = (logic_tail - logic_pos) -
                        (logic_tail - logic_pos) % (listlayer->menulist->head->rect.height);
        }

        if (listlayer->menulist->load_tail == listlayer->menulist->head)
        {
            status |= DOWNOVER;
        }

        if (listlayer->menulist->load_head == listlayer->menulist->tail)
        {
            status |= UPOVER;
        }

        if (listlayer->scnnum != 1)
        {
            if (head_dist <= listlayer->menulist->head->rect.height)
            {
                status = status | WAITUP;

                if (status & DOWNOVER)
                {
                    status &= (~DOWNOVER);
                }

                if (status & UPOVER)
                {
                    status &= (~UPOVER);
                    status &= (~WAITUP);
                }

                esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
                esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/
                return status;
            }
        }
        else
        {
            if (head_dist < listlayer->menulist->head->rect.height)
            {
                status = status | WAITUP;

                if (status & DOWNOVER)
                {
                    status &= (~DOWNOVER);
                }

                if (status & UPOVER)
                {
                    status &= (~UPOVER);
                    status &= (~WAITUP);
                }

                esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
                esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/
                return status;
            }
        }

        if (listlayer->scnnum != 1)
        {
            if (tail_dist <= (listlayer->viewrect.height + listlayer->menulist->head->rect.height))
            {
                status = status | WAITDOWN;

                if (status & DOWNOVER)
                {
                    status &= (~DOWNOVER);
                    status &= (~WAITDOWN);
                }

                if (status & UPOVER)
                {
                    status &= (~UPOVER);
                }

                esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
                esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/
                return status;
            }
        }
        else
        {
            if (tail_dist < (listlayer->viewrect.height + listlayer->menulist->head->rect.height))
            {
                status = status | WAITDOWN;

                if (status & DOWNOVER)
                {
                    status &= (~DOWNOVER);
                    status &= (~WAITDOWN);
                }

                if (status & UPOVER)
                {
                    status &= (~UPOVER);
                }

                esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
                esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/
                return status;
            }
        }

        /*需要预取头item*/
        if (head_dist < (listlayer->fb_depth / 2 - 1) * (listlayer->viewrect.height))
        {
            if (status & UPOVER)
            {
                status &= (~UPOVER);
            }
            else
            {
                status |= ((__u32)((listlayer->fb_depth / 2 - 1) *
                                   (listlayer->viewrect.height) - head_dist) << 16);
                status |= UPWARDS;
            }
        }

        if (tail_dist < ((listlayer->viewrect.height) * (listlayer->fb_depth / 2)))
        {
            if (status & DOWNOVER)
            {
                status &= (~DOWNOVER);
            }
            else
            {
                status |= ((__u32)(((listlayer->viewrect.height) *
                                    (listlayer->fb_depth / 2)) - tail_dist) << 16);
                status |= DOWNWARDS;
            }
        }

        esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
    }

    esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/

    if (status & DOWNOVER)
    {
        status &= (~DOWNOVER);
    }

    if (status & UPOVER)
    {
        status &= (~UPOVER);
    }

    return status;
}

/*
**********************************************************************************************************************
*                                               ListBox_LoadItemThread
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer loaditemthread
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void ListBox_LoadItemThread(void *p_arg)
{
    __u8                       err;
    __u8                       num;
    __u8                       count;
    __bool                     lock_flags = EPDK_FALSE;
    __u32                      distance;
    __u32                      status;
    __s32                      dist_tmp;
    void                      *src_addr;
    void                      *dst_addr1;
    __krnl_flags_t             flags;
    __krnl_tcb_t               cur_tcb;
    LIST_LAYER_MAN_t          *listlayer = (LIST_LAYER_MAN_t *)NULL;
    listlayer = (LIST_LAYER_MAN_t *)p_arg;
    err = esKRNL_TaskQuery(OS_PRIO_SELF, &cur_tcb);

    if (err == OS_NO_ERR)
    {
        listlayer->LoadPrio = cur_tcb.OSTCBPrio;
    }

    esKRNL_FlagPend(listlayer->load_flags, LISTLAYER_LOAD_FLAG,
                    ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);

    while (1)
    {
        /*get the listbuffer status*/
        status = ListBox_GetBufferStatus(listlayer);

        /*need not to load item*/
        if (status == 0)
        {
            if (!(listlayer->status & WAIT_LOAD))
            {
                /*suspend this thread*/
                esKRNL_FlagPend(listlayer->load_flags, LISTLAYER_LOAD_FLAG + LISTLAYER_WAIT_LOAD_FLAG,
                                ((OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME) << 16) | 0, &err);

                if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
                {
                    esKRNL_TDel(EXEC_prioself);
                    return;
                }
            }

            esKRNL_TimeDly(1);
        }
        else
        {
            /*拿到锁的标志位*/
            lock_flags = EPDK_TRUE;

            while (GUI_LockAccept() == EPDK_FAIL)
            {
                if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
                {
                    esKRNL_TDel(EXEC_prioself);
                    return;
                }

                esKRNL_TimeDly(1);
                /*post run thread*/
                flags = esKRNL_FlagQuery(listlayer->wait_flags, &err);

                if (!(flags & LISTLAYER_WAIT_FLAG))
                {
                    status = ListBox_GetBufferStatus(listlayer);

                    if (status == 0)
                    {
                        /*get framebuffer status*/
                        esKRNL_FlagPost(listlayer->wait_flags, LISTLAYER_WAIT_FLAG, OS_FLAG_SET, &err);
                        lock_flags = EPDK_FALSE;
                        break;
                    }
                }
            }

            /*get lock and get listlayer status*/
            if (lock_flags == EPDK_TRUE)
            {
                status = ListBox_GetBufferStatus(listlayer);
            }

            /*if main thread wait up*/
            if (status & WAITUP)
            {
                count = listlayer->scnnum;

                for (num = 0; num < count; num++)
                {
                    if (listlayer->menulist->load_head != listlayer->menulist->tail)
                    {
                        ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->load_head,
                                                   0, listlayer->head_pos.x, listlayer->head_pos.y);
                        dist_tmp = listlayer->head_pos.y - listlayer->menulist->load_head->rect.height;

                        if (dist_tmp < 0)
                        {
                            src_addr = (void *)((unsigned long)listlayer->listbuffer->buffer);
                            dst_addr1 = listlayer->listbuffer->ebuffer;
                            ListLayer_HeadToTailCopy(listlayer);
                            esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/
                            listlayer->head_pos.y = listlayer->listbuffer->vrect.height -
                                                    listlayer->menulist->head->rect.height;
                        }
                        else
                        {
                            esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/
                            listlayer->head_pos.y = listlayer->head_pos.y -
                                                    listlayer->menulist->load_head->rect.height;
                        }

                        /*需要加载的item为上一个item*/
                        listlayer->menulist->load_head = listlayer->menulist->load_head->Prev;

                        /*如果需要加载的起始位置小于结束位置(此时由于加载头item而改写为item加载序列)*/
                        if (listlayer->head_pos.y < listlayer->tail_pos.y)
                        {
                            /*如果加载位置在起始屏位置之外,则直接更新结束屏位置*/
                            if (listlayer->head_pos.y >= listlayer->viewrect.height)
                            {
                                listlayer->tail_pos.y = listlayer->head_pos.y;
                            }/*加载开始位置在起始屏之内，加载尾为结束屏相应位置*/
                            else if (listlayer->head_pos.y == listlayer->viewrect.height -
                                     listlayer->menulist->head->rect.height)
                            {
                                listlayer->tail_pos.y = listlayer->listbuffer->rectsz.height -
                                                        listlayer->menulist->head->rect.height;
                            }
                            else if ((listlayer->head_pos.y < listlayer->viewrect.height) &&
                                     (listlayer->tail_pos.y > listlayer->listbuffer->vrect.height))
                            {
                                /*更新相应的变化值*/
                                listlayer->tail_pos.y = listlayer->tail_pos.y -
                                                        listlayer->menulist->load_tail->rect.height;
                            }

                            /*更新加载为指针*/
                            listlayer->menulist->load_tail = listlayer->menulist->load_tail->Prev;
                        }

                        esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
                    }
                }

                status = (status & (~WAITUP)) & (~UPWARDS);
                flags = esKRNL_FlagQuery(listlayer->wait_flags, &err);

                if (!(flags & LISTLAYER_WAIT_FLAG))
                {
                    status = ListBox_GetBufferStatus(listlayer);

                    if (status == 0)
                    {
                        /*get framebuffer status*/
                        esKRNL_FlagPost(listlayer->wait_flags, LISTLAYER_WAIT_FLAG, OS_FLAG_SET, &err);
                    }
                }

                if (lock_flags == EPDK_TRUE)
                {
                    GUI_UNLOCK();
                }
            }
            else if (status & WAITDOWN)
            {
                count = listlayer->scnnum;

                for (num = 0; num < count; num++)
                {
                    if (listlayer->menulist->load_tail != listlayer->menulist->head)
                    {
                        ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->load_tail,
                                                   0, listlayer->tail_pos.x, listlayer->tail_pos.y);
                        //esKRNL_SchedLock();
                        dist_tmp = listlayer->tail_pos.y + listlayer->menulist->load_tail->rect.height;

                        if (dist_tmp >= listlayer->listbuffer->rectsz.height)
                        {
                            ListLayer_TailToHeadCopy(listlayer);
                            esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/
                            listlayer->tail_pos.y = listlayer->viewrect.height;
                        }
                        else
                        {
                            esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/
                            listlayer->tail_pos.y = listlayer->tail_pos.y + listlayer->menulist->load_tail->rect.height;
                        }

                        listlayer->menulist->load_tail = listlayer->menulist->load_tail->Next;

                        if (listlayer->tail_pos.y > listlayer->head_pos.y)
                        {
                            if (listlayer->tail_pos.y < listlayer->listbuffer->vrect.height)
                            {
                                listlayer->head_pos.y = listlayer->tail_pos.y;
                            }
                            else if (listlayer->tail_pos.y == listlayer->listbuffer->vrect.height)
                            {
                                listlayer->head_pos.y = 0;
                            }
                            else if ((listlayer->tail_pos.y > listlayer->listbuffer->vrect.height)
                                     && (listlayer->head_pos.y < listlayer->viewrect.height))
                            {
                                listlayer->head_pos.y = listlayer->head_pos.y +
                                                        listlayer->menulist->load_head->rect.height;
                            }

                            listlayer->menulist->load_head = listlayer->menulist->load_head->Next;
                        }

                        esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
                        //esKRNL_SchedUnlock();
                    }
                }

                status = (status & (~WAITDOWN)) & (~DOWNWARDS);
                flags = esKRNL_FlagQuery(listlayer->wait_flags, &err);

                if (!(flags & LISTLAYER_WAIT_FLAG))
                {
                    status = ListBox_GetBufferStatus(listlayer);

                    if (status == 0)
                    {
                        esKRNL_FlagPost(listlayer->wait_flags, LISTLAYER_WAIT_FLAG, OS_FLAG_SET, &err);
                    }
                }

                if (lock_flags == EPDK_TRUE)
                {
                    GUI_UNLOCK();
                }
            }
            else if (status & UPWARDS)
            {
                distance = status >> 16;
                count = distance / (listlayer->menulist->head->rect.height);

                for (num = 0; num < count; num++)
                {
                    if (listlayer->menulist->load_head != listlayer->menulist->tail)
                    {
                        ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->load_head,
                                                   0, listlayer->head_pos.x,
                                                   listlayer->head_pos.y);
                        //esKRNL_SchedLock();
                        dist_tmp = listlayer->head_pos.y - listlayer->menulist->load_head->rect.height;

                        if (dist_tmp < 0)
                        {
                            ListLayer_HeadToTailCopy(listlayer);
                            esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/
                            listlayer->head_pos.y = listlayer->listbuffer->vrect.height -
                                                    listlayer->menulist->head->rect.height;
                        }
                        else
                        {
                            esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/
                            listlayer->head_pos.y = listlayer->head_pos.y -
                                                    listlayer->menulist->load_head->rect.height;
                        }

                        listlayer->menulist->load_head = listlayer->menulist->load_head->Prev;

                        if (listlayer->head_pos.y < listlayer->tail_pos.y)
                        {
                            if (listlayer->head_pos.y >= listlayer->viewrect.height)
                            {
                                listlayer->tail_pos.y = listlayer->head_pos.y;
                            }
                            else if (listlayer->head_pos.y == listlayer->viewrect.height -
                                     listlayer->menulist->head->rect.height)
                            {
                                listlayer->tail_pos.y = listlayer->listbuffer->rectsz.height -
                                                        listlayer->menulist->head->rect.height;
                            }
                            else if ((listlayer->head_pos.y < listlayer->viewrect.height)
                                     && (listlayer->tail_pos.y > listlayer->listbuffer->vrect.height))
                            {
                                listlayer->tail_pos.y = listlayer->tail_pos.y -
                                                        listlayer->menulist->load_tail->rect.height;
                            }

                            listlayer->menulist->load_tail = listlayer->menulist->load_tail->Prev;
                        }

                        esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
                        //esKRNL_SchedUnlock();
                    }
                }

                status = status & (~UPWARDS);
                flags = esKRNL_FlagQuery(listlayer->wait_flags, &err);

                if (!(flags & LISTLAYER_WAIT_FLAG))
                {
                    status = ListBox_GetBufferStatus(listlayer);

                    if (status == 0)
                    {
                        esKRNL_FlagPost(listlayer->wait_flags, LISTLAYER_WAIT_FLAG, OS_FLAG_SET, &err);
                    }
                }

                if (lock_flags == EPDK_TRUE)
                {
                    GUI_UNLOCK();
                }
            }
            else if (status & DOWNWARDS)
            {
                distance = status >> 16;
                count = distance / (listlayer->menulist->head->rect.height);

                for (num = 0; num < count; num++)
                {
                    if (listlayer->menulist->load_tail != listlayer->menulist->head)
                    {
                        ListLayer_FastLoadMenuItem(listlayer, listlayer->menulist->load_tail, 0,
                                                   listlayer->tail_pos.x, listlayer->tail_pos.y);
                        //esKRNL_SchedLock();
                        dist_tmp = listlayer->tail_pos.y + listlayer->menulist->load_tail->rect.height;

                        if (dist_tmp >= listlayer->listbuffer->rectsz.height)
                        {
                            ListLayer_TailToHeadCopy(listlayer);
                            esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/
                            listlayer->tail_pos.y = listlayer->viewrect.height;
                        }
                        else
                        {
                            esKRNL_SemPend(listlayer->load_sem, 0, &err); /*pend load semphore*/
                            listlayer->tail_pos.y = listlayer->tail_pos.y + listlayer->menulist->load_tail->rect.height;
                        }

                        if (listlayer->menulist->load_tail == listlayer->menulist->head)
                        {
                            listlayer->filldown++;
                        }

                        if (listlayer->menulist->load_tail != listlayer->menulist->head)
                        {
                            listlayer->menulist->load_tail = listlayer->menulist->load_tail->Next;
                        }

                        if (listlayer->tail_pos.y > listlayer->head_pos.y)
                        {
                            if (listlayer->tail_pos.y < listlayer->listbuffer->vrect.height)
                            {
                                listlayer->head_pos.y = listlayer->tail_pos.y;
                            }
                            else if (listlayer->tail_pos.y == listlayer->listbuffer->vrect.height)
                            {
                                listlayer->head_pos.y = 0;
                            }
                            else if ((listlayer->tail_pos.y > listlayer->listbuffer->vrect.height)
                                     && (listlayer->head_pos.y < listlayer->viewrect.height))
                            {
                                listlayer->head_pos.y = listlayer->head_pos.y +
                                                        listlayer->menulist->load_head->rect.height;
                            }

                            listlayer->menulist->load_head = listlayer->menulist->load_head->Next;
                        }

                        esKRNL_SemPost(listlayer->load_sem); /*post load semphore*/
                        //esKRNL_SchedUnlock();
                    }
                }

                status = status & (~DOWNWARDS);
                flags = esKRNL_FlagQuery(listlayer->wait_flags, &err);

                if (!(flags & LISTLAYER_WAIT_FLAG))
                {
                    status = ListBox_GetBufferStatus(listlayer);

                    if (status == 0)
                    {
                        esKRNL_FlagPost(listlayer->wait_flags, LISTLAYER_WAIT_FLAG, OS_FLAG_SET, &err);
                    }
                }

                if (lock_flags == EPDK_TRUE)
                {
                    GUI_UNLOCK();
                }
            }
            else
            {
                if (lock_flags == EPDK_TRUE)
                {
                    GUI_UNLOCK();
                }
            }

            POS_PRINTF("thread phy pos = %x", listlayer->viewrect.y);
            POS_PRINTF("thread phy head = %x", listlayer->head_pos.y);
            // LISTMENU_PRINTF("thread phy tail = %x",listlayer->tail_pos.y);
        }
    }
}


/*
**********************************************************************************************************************
*                                               ListBox_FoucsItemThread
*
* author:
*
* date:             2008-10-8
*
* Description:      listlayer foucsitemthread
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void ListBox_FoucsItemThread(void *p_arg)
{
    __u8                        err;
    __krnl_tcb_t                cur_tcb;
    LIST_LAYER_MAN_t           *listlayer = (LIST_LAYER_MAN_t *)NULL;
    OS_SEM_DATA                 sem_data;
    __krnl_flags_t              flags;
    __u8                        del_flags = 0;

    if (p_arg == NULL)
    {
        return ;
    }

    listlayer = (LIST_LAYER_MAN_t *)p_arg;
    err = esKRNL_TaskQuery(OS_PRIO_SELF, &cur_tcb);

    if (err == OS_NO_ERR)
    {
        listlayer->FoucsPrio = cur_tcb.OSTCBPrio;
    }

    while (1)
    {
        flags = esKRNL_FlagPend(listlayer->foucs_flags, LISTLAYER_FOUCS_FLAG + LISTLAYER_DRAW_ITEM_FLAG,
                        ((OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME) << 16) | 0, &err);

        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            esKRNL_TDel(EXEC_prioself);
            return ;
        }

        //flags = esKRNL_FlagQuery(listlayer->foucs_flags, &err);

        if (flags & LISTLAYER_FOUCS_FLAG)
        {
            if (del_flags == 1)
            {
                continue ;
            }

            if (listlayer->setfoucs > 0)
            {
                ListLayer_SetLayerUnFoucs(listlayer);
                listlayer->setfoucs = 0;
            }

            if (listlayer->setfoucs == 0)
            {
                ListLayer_SetLayerFastFoucs(listlayer, listlayer->menulist->CurItem);

                if (!(listlayer->plistmenu->mode & KEYTONE_OFF))
                {
                    _lstm_key_song();
                }

                listlayer->setfoucs = VAR_FOUCS;
            }

            esKRNL_FlagPost(listlayer->foucs_flags, LISTLAYER_FOUCS_FINISH_FLAG, OS_FLAG_SET, &err);
            //listlayer->foucs_flags->OSFlagFlags &= (~LISTLAYER_FOUCS_FLAG);
        }
        if (flags & LISTLAYER_DRAW_ITEM_FLAG)
        {
            /*扩散加载处理*/
            if (listlayer->mode != FIXED_MODE)
            {
                err = esKRNL_SemQuery(listlayer->plistmenu->load_sem, &sem_data);

                if (sem_data.OSCnt == 0)
                {
                    esKRNL_SemPost(listlayer->plistmenu->load_sem); /*post pos semphore*/
                }

                esKRNL_SemPend(listlayer->plistmenu->load_sem, 0, &err); /*pend pos semphore*/
                ListLayer_DrawItemProcess(listlayer);
                esKRNL_SemPost(listlayer->plistmenu->load_sem); /*pend pos semphore*/
            }

            //listlayer->foucs_flags->OSFlagFlags &= (~LISTLAYER_DRAW_ITEM_FLAG);
        }
    }
}

/*
**********************************************************************************************************************
*                                               ListBox_VarKeyUpMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer varkeyupmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListBox_VarKeyUpMsg(LIST_LAYER_MAN_t *listlayer, __u32 dist, __u32  step)
{
    __u8                       err;
    __u8                       fbno;
    __u8                       i;
    __u32                      ret;
    __u32                      status;
    __rect_t                   sw;
    __move_para_t              dist_dly;
    __move_para_t              dist_slave;   /*slave layer dist*/
    __krnl_flags_t             flags;
    fbno = 0;
    status = ListBox_GetBufferStatus(listlayer);

    if (status & WAITUP)
    {
        __inf("wait to load! status = %d", status);
        listlayer->status |= WAIT_LOAD;
        esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_WAIT_LOAD_FLAG, OS_FLAG_SET, &err);
        listlayer->wait_flags->OSFlagFlags &= (~LISTLAYER_WAIT_FLAG);
        esKRNL_FlagPend(listlayer->wait_flags, LISTLAYER_WAIT_FLAG,
                        ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);
        listlayer->status &= (~WAIT_LOAD);
    }

    if (listlayer->menulist->CurItem != listlayer->menulist->head)
    {
        /*set listlayer unfoucs pos*/
        listlayer->unfoucs_pos.x = listlayer->foucs_pos.x;
        listlayer->unfoucs_pos.y = listlayer->foucs_pos.y + listlayer->menulist->CurItem->foucs_w -
                                   listlayer->menulist->CurItem->rect.height;
        listlayer->menulist->UnCurItem = listlayer->menulist->CurItem;

        //condition = listlayer->msgupcount;
        if ((listlayer->viewrect.y < listlayer->menulist->CurItem->rect.height)
            && (listlayer->unfoucs_pos.y < listlayer->menulist->CurItem->rect.height))
        {
            /*current foucs pos locate blow the start sceen */
            listlayer->foucs_pos.y = listlayer->foucs_pos.y -
                                     listlayer->menulist->CurItem->rect.height +
                                     listlayer->listbuffer->vrect.height;
            listlayer->foucs_pos.x = listlayer->foucs_pos.x;
        }
        else
        {
            listlayer->foucs_pos.y = listlayer->foucs_pos.y - listlayer->menulist->CurItem->rect.height;
            listlayer->foucs_pos.x = listlayer->foucs_pos.x;
        }

        LISTMENU_PRINTF("listlayer->foucs.y = %d", listlayer->foucs_pos.y);
        sw.x = listlayer->viewrect.x;
        sw.y = listlayer->viewrect.y;
        listlayer->menulist->CurItem = listlayer->menulist->CurItem->Prev;
        esKRNL_FlagPost(listlayer->foucs_flags, LISTLAYER_FOUCS_FLAG, OS_FLAG_SET, &err);
        listlayer->cursteps = listlayer->nextsteps;

        for (i = 0; i < listlayer->cursteps; i++)
        {
            /*需要上移区域*/
            if (listlayer->viewrect.y > listlayer->foucs_pos.y)
            {
                if (i == 0)
                {
                    listlayer->menulist->HeadItem = listlayer->menulist->HeadItem->Prev;
                    listlayer->menulist->SelItem = listlayer->menulist->HeadItem;
                    listlayer->menulist->dist -= listlayer->menulist->CurItem->rect.height;
                }

                InterMove(0, listlayer->menulist->CurItem->rect.height, listlayer->cursteps, i, &dist_dly);
                InterMove(0, 0, listlayer->cursteps, i, &dist_slave);
            }
            else if ((listlayer->viewrect.y < listlayer->menulist->CurItem->rect.height)
                     && (listlayer->unfoucs_pos.y < listlayer->menulist->CurItem->rect.height))
            {
                if (i == 0)
                {
                    listlayer->menulist->HeadItem = listlayer->menulist->HeadItem->Prev;
                    listlayer->menulist->SelItem = listlayer->menulist->HeadItem;
                    listlayer->menulist->dist -= listlayer->menulist->CurItem->rect.height;
                }

                InterMove(0, listlayer->menulist->CurItem->rect.height, listlayer->cursteps, i, &dist_dly);
                InterMove(0, 0, listlayer->cursteps, i, &dist_slave);
            }
            else
            {
                InterMove(0, 0, listlayer->cursteps, i, &dist_dly);
                InterMove(0, listlayer->menulist->CurItem->rect.height, listlayer->cursteps, i, &dist_slave);
            }

            sw.y = sw.y - dist_dly.dist;

            if (sw.y < 0)
            {
                sw.y = listlayer->listbuffer->vrect.height + sw.y;
            }
            else if (sw.y > listlayer->listbuffer->vrect.height)
            {
                sw.y = sw.y - listlayer->listbuffer->vrect.height;
            }

            ret = ListLayer_SwitchFB(listlayer, fbno, sw.x, sw.y);

            if (ret != EPDK_OK)
            {
                __wrn("layer close failed,layer handle = %d", listlayer->layer);
                return __LINE__;
            }

            esKRNL_TimeDly(dist_dly.dly_time);
        }

        esKRNL_SemPend(listlayer->pos_sem, 0, &err); /*pend pos semphore*/

        if (err == OS_NO_ERR)
        {
            listlayer->viewrect.x = sw.x;
            listlayer->viewrect.y = sw.y;
        }

        esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/
        esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_LOAD_FLAG, OS_FLAG_SET, &err);
        flags = esKRNL_FlagQuery(listlayer->foucs_flags, &err);

        if (!(flags & LISTLAYER_FOUCS_FINISH_FLAG))
        {
            __inf("wait to foucs!");
            esKRNL_FlagPend(listlayer->foucs_flags, LISTLAYER_FOUCS_FINISH_FLAG,
                            ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);
        }

        listlayer->foucs_flags->OSFlagFlags &= (~LISTLAYER_FOUCS_FINISH_FLAG);

        if (listlayer->LoadPrio)
        {
            esKRNL_TaskSuspend(listlayer->LoadPrio);
            esKRNL_TimeDly(2);
            esKRNL_TaskResume(listlayer->LoadPrio);
        }
        else
        {
            esKRNL_TimeDly(2);
        }

        /*判断是否有停止消息过来*/
        if (listlayer->msgcount != 0 &&
            listlayer->plistmenu->lastmsg == GUI_MSG_KEY_LONGUP)
        {
            /*无停止消息过来，继续运行*/
            GUI_LstmPostRunMsg(listlayer->plistmenu, LSTMM_RUNUP);
        }
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListBox_VarKeyDownMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer varkeydownmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListBox_VarKeyDownMsg(LIST_LAYER_MAN_t    *listlayer, __u32 dist, __u32 step)
{
    __u8                       err;
    __u8                       fbno;
    __u8                       i;
    __u32                      ret;
    __u32                      status;
    __rect_t                   sw;
    __move_para_t              dist_dly;
    __move_para_t              dist_slave;   /*slave layer dist*/
    __krnl_flags_t             flags;
    fbno = 0;
    //__inf("listlayer->foucs.y = %d",listlayer->foucs_pos.y);
    status = ListBox_GetBufferStatus(listlayer);

    if (status & WAITDOWN)
    {
        __inf("wait to load! status = %d", status);
        listlayer->status |= WAIT_LOAD;
        esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_WAIT_LOAD_FLAG, OS_FLAG_SET, &err);
        listlayer->wait_flags->OSFlagFlags &= (~LISTLAYER_WAIT_FLAG);
        esKRNL_FlagPend(listlayer->wait_flags, LISTLAYER_WAIT_FLAG,
                        ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);
        listlayer->status &= (~WAIT_LOAD);
    }

    if (listlayer->menulist->CurItem != listlayer->menulist->tail)
    {
        /*save prev foucs position*/
        listlayer->unfoucs_pos.x = listlayer->foucs_pos.x;
        listlayer->unfoucs_pos.y = listlayer->foucs_pos.y;
        listlayer->menulist->UnCurItem = listlayer->menulist->CurItem;

        if ((listlayer->viewrect.y > listlayer->listbuffer->vrect.height - listlayer->menulist->CurItem->rect.height)
            && (listlayer->unfoucs_pos.y >= listlayer->listbuffer->rectsz.height - listlayer->menulist->CurItem->rect.height))
        {
            listlayer->foucs_pos.y = listlayer->foucs_pos.y +
                                     listlayer->menulist->CurItem->rect.height -
                                     listlayer->listbuffer->vrect.height;
            listlayer->foucs_pos.x = listlayer->foucs_pos.x;
        }
        else
        {
            listlayer->foucs_pos.y = listlayer->foucs_pos.y + listlayer->menulist->CurItem->rect.height;
            listlayer->foucs_pos.x = listlayer->foucs_pos.x;
        }

        __inf("listlayer->foucs_pos.y = %d", listlayer->foucs_pos.y);
        sw.x                            = listlayer->viewrect.x;
        sw.y                            = listlayer->viewrect.y;
        listlayer->menulist->CurItem    = listlayer->menulist->CurItem->Next;
        esKRNL_FlagPost(listlayer->foucs_flags, LISTLAYER_FOUCS_FLAG, OS_FLAG_SET, &err); /*start foucs Item thread to foucs item*/
        listlayer->cursteps = listlayer->nextsteps;

        for (i = 0; i < listlayer->cursteps; i++)
        {
            if (listlayer->viewrect.y + listlayer->viewrect.height <
                listlayer->foucs_pos.y + listlayer->menulist->CurItem->rect.height)
            {
                if (i == 0)
                {
                    listlayer->menulist->HeadItem = listlayer->menulist->HeadItem->Next;
                    listlayer->menulist->SelItem = listlayer->menulist->HeadItem;
                    listlayer->menulist->dist += listlayer->menulist->CurItem->rect.height;
                }

                InterMove(0, listlayer->menulist->CurItem->rect.height, listlayer->cursteps, i, &dist_dly);
                InterMove(0, 0, listlayer->cursteps, i, &dist_slave);
            }
            else if ((listlayer->viewrect.y > listlayer->listbuffer->vrect.height - listlayer->menulist->CurItem->rect.height)
                     && (listlayer->unfoucs_pos.y >= listlayer->listbuffer->rectsz.height - listlayer->menulist->CurItem->rect.height))
            {
                if (i == 0)
                {
                    listlayer->menulist->HeadItem = listlayer->menulist->HeadItem->Next;
                    listlayer->menulist->SelItem = listlayer->menulist->HeadItem;
                    listlayer->menulist->dist += listlayer->menulist->CurItem->rect.height;
                }

                InterMove(0, listlayer->menulist->CurItem->rect.height, listlayer->cursteps, i, &dist_dly);
                InterMove(0, 0, listlayer->cursteps, i, &dist_slave);
            }
            else
            {
                InterMove(0, 0, listlayer->cursteps, i, &dist_dly);
                InterMove(0, listlayer->menulist->CurItem->rect.height, listlayer->cursteps, i, &dist_slave);
            }

            sw.y = sw.y + dist_dly.dist;

            if (sw.y > listlayer->listbuffer->vrect.height)
            {
                sw.y = sw.y - listlayer->listbuffer->vrect.height;
            }
            else if (sw.y < 0)
            {
                sw.y = sw.y + listlayer->listbuffer->vrect.height;
            }

            ret = ListLayer_SwitchFB(listlayer, fbno, sw.x, sw.y);

            if (ret != EPDK_OK)
            {
                __wrn("layer close failed,layer handle = %d", listlayer->layer);
                return __LINE__;
            }

            esKRNL_TimeDly(dist_dly.dly_time);
        }

        esKRNL_SemPend(listlayer->pos_sem, 0, &err); /*pend pos semphore*/

        if (err == OS_NO_ERR)
        {
            listlayer->viewrect.x = sw.x;
            listlayer->viewrect.y = sw.y;
        }

        esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/
        esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_LOAD_FLAG, OS_FLAG_SET, &err);
        flags = esKRNL_FlagQuery(listlayer->foucs_flags, &err);

        if (!(flags & LISTLAYER_FOUCS_FINISH_FLAG))
        {
            __inf("wait to foucs!flags = %d", flags);
            esKRNL_FlagPend(listlayer->foucs_flags, LISTLAYER_FOUCS_FINISH_FLAG,
                            ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);
        }

        listlayer->foucs_flags->OSFlagFlags &= (~LISTLAYER_FOUCS_FINISH_FLAG);

        if (listlayer->LoadPrio)
        {
            esKRNL_TaskSuspend(listlayer->LoadPrio);
            esKRNL_TimeDly(2);
            esKRNL_TaskResume(listlayer->LoadPrio);
        }
        else
        {
            esKRNL_TimeDly(2);
        }

        /*判断是否有停止消息过来*/
        if (listlayer->msgcount != 0 &&
            listlayer->plistmenu->lastmsg == GUI_MSG_KEY_LONGDOWN)
        {
            /*无停止消息过来，继续运行*/
            GUI_LstmPostRunMsg(listlayer->plistmenu, LSTMM_RUNDOWN);
        }
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListBox_FixedKeyUpMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer fixedkeyupmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListBox_FixedKeyUpMsg(LIST_LAYER_MAN_t *listlayer, __u32 dist, __u32  step)
{
    __u8                       err;
    __u8                       i;
    __u32                      ret;
    __rect_t                   sw = {0, 0, 0, 0};
    __move_para_t              dist_dly;
    __krnl_flags_t             flags;

    if (listlayer->menulist->CurItem != listlayer->menulist->head)
    {
        listlayer->unfoucs_pos.x = listlayer->foucs_pos.x;
        listlayer->unfoucs_pos.y = listlayer->foucs_pos.y + listlayer->menulist->CurItem->foucs_w -
                                   listlayer->menulist->CurItem->rect.height;
        listlayer->menulist->UnCurItem = listlayer->menulist->CurItem;
        listlayer->foucs_pos.y = listlayer->foucs_pos.y - listlayer->menulist->CurItem->rect.height;
        listlayer->foucs_pos.x = listlayer->foucs_pos.x;
        sw.x                            = listlayer->viewrect.x;
        sw.y                            = listlayer->viewrect.y;
        listlayer->menulist->CurItem    = listlayer->menulist->CurItem->Prev;
        listlayer->menulist->SelItem    = listlayer->menulist->SelItem->Prev;
        esKRNL_FlagPost(listlayer->foucs_flags, LISTLAYER_FOUCS_FLAG, OS_FLAG_SET, &err);
        listlayer->cursteps = listlayer->nextsteps;

        for (i = 0; i < listlayer->cursteps; i++)
        {
            if (listlayer->foucs_pos.y < listlayer->viewrect.y)
            {
                if (i == 0)
                {
                    listlayer->menulist->HeadItem = listlayer->menulist->HeadItem->Prev;
                    listlayer->menulist->dist -= listlayer->menulist->CurItem->rect.height;
                }

                InterMove(0, listlayer->menulist->CurItem->rect.height, listlayer->cursteps, i, &dist_dly);
            }
            else
            {
                InterMove(0, 0, listlayer->cursteps, i, &dist_dly);
            }

            sw.y = sw.y - dist_dly.dist;
            ret = ListLayer_SwitchFB(listlayer, 0, sw.x, sw.y);

            if (ret != EPDK_OK)
            {
                __wrn("layer close failed,layer handle = %d", listlayer->layer);
                return __LINE__;
            }

            esKRNL_TimeDly(dist_dly.dly_time);
        }

        listlayer->viewrect.x = sw.x;
        listlayer->viewrect.y = sw.y;
        flags = esKRNL_FlagQuery(listlayer->foucs_flags, &err);

        if (!(flags & LISTLAYER_FOUCS_FINISH_FLAG))
        {
            __inf("wait to foucs!flags = %d", flags);
            esKRNL_FlagPend(listlayer->foucs_flags, LISTLAYER_FOUCS_FINISH_FLAG,
                            ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);
        }

        listlayer->foucs_flags->OSFlagFlags &= (~LISTLAYER_FOUCS_FINISH_FLAG);
        esKRNL_TimeDly(2);

        /*判断是否有停止消息过来*/
        if (listlayer->msgcount != 0 &&
            listlayer->plistmenu->lastmsg == GUI_MSG_KEY_LONGUP)
        {
            /*无停止消息过来，继续运行*/
            GUI_LstmPostRunMsg(listlayer->plistmenu, LSTMM_RUNUP);
        }
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListBox_FixedKeyDownMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer fixedkeydownmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListBox_FixedKeyDownMsg(LIST_LAYER_MAN_t    *listlayer, __u32 dist, __u32 step)
{
    __u8                       err;
    __u8                       i;
    __u32                      ret;
    __rect_t                   sw;
    __move_para_t              dist_dly;
    __krnl_flags_t             flags;

    //fbno = 0;

    if (listlayer->menulist->CurItem != listlayer->menulist->tail)
    {
        /*save prev foucs position*/
        listlayer->unfoucs_pos.x = listlayer->foucs_pos.x;
        listlayer->unfoucs_pos.y = listlayer->foucs_pos.y;
        listlayer->menulist->UnCurItem = listlayer->menulist->CurItem;
        listlayer->foucs_pos.y = listlayer->foucs_pos.y + listlayer->menulist->CurItem->rect.height;
        listlayer->foucs_pos.x = listlayer->foucs_pos.x;
        sw.x                            = listlayer->viewrect.x;
        sw.y                            = listlayer->viewrect.y;
        listlayer->menulist->CurItem    = listlayer->menulist->CurItem->Next;
        listlayer->menulist->SelItem    = listlayer->menulist->SelItem->Next;
        esKRNL_FlagPost(listlayer->foucs_flags, LISTLAYER_FOUCS_FLAG, OS_FLAG_SET, &err); /*start foucs Item thread to foucs item*/
        listlayer->cursteps = listlayer->nextsteps;

        for (i = 0; i < listlayer->cursteps; i++)
        {
            if (listlayer->foucs_pos.y - listlayer->viewrect.y >= listlayer->scnrect.height)
            {
                if (i == 0)
                {
                    listlayer->menulist->HeadItem = listlayer->menulist->HeadItem->Next;
                    listlayer->menulist->dist += listlayer->menulist->CurItem->rect.height;
                }

                InterMove(0, listlayer->menulist->CurItem->rect.height, listlayer->cursteps, i, &dist_dly);
            }
            else
            {
                InterMove(0, 0, listlayer->cursteps, i, &dist_dly);
            }

            sw.y = sw.y + dist_dly.dist;
            ret = ListLayer_SwitchFB(listlayer, 0, sw.x, sw.y);

            if (ret != EPDK_OK)
            {
                __wrn("layer close failed,layer handle = %d", listlayer->layer);
                return __LINE__;
            }

            esKRNL_TimeDly(dist_dly.dly_time);
        }

        listlayer->viewrect.x = sw.x;
        listlayer->viewrect.y = sw.y;
        flags = esKRNL_FlagQuery(listlayer->foucs_flags, &err);

        if (!(flags & LISTLAYER_FOUCS_FINISH_FLAG))
        {
            __inf("wait to foucs!flags = %d", flags);
            esKRNL_FlagPend(listlayer->foucs_flags, LISTLAYER_FOUCS_FINISH_FLAG,
                            ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);
        }

        listlayer->foucs_flags->OSFlagFlags &= (~LISTLAYER_FOUCS_FINISH_FLAG);
        esKRNL_TimeDly(2);

        /*判断是否有停止消息过来*/
        if (listlayer->msgcount != 0 &&
            listlayer->plistmenu->lastmsg == GUI_MSG_KEY_LONGDOWN)
        {
            /*无停止消息过来，继续运行*/
            GUI_LstmPostRunMsg(listlayer->plistmenu, LSTMM_RUNDOWN);
        }
    }

    return OK;
}



/*
**********************************************************************************************************************
*                                               ListBox_VarKeyUpMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer varkeyupmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListBox_FlatVarMoveDownMsg(LIST_LAYER_MAN_t *listlayer, __u32 dist)
{
    __u8                        err;
    __u8                        fbno;
    __u32                       ret;
    __u32                       status;
    __rect_t                    sw;
    __u32                       height;
    __u32                       itemno;
    __u32                       uplyr_dist      = 0;
    __u32                       downlyr_dist    = 0;
    fbno = 0;

    if (dist == 0)
    {
        return ORANGE_OK;
    }

    status = ListBox_GetBufferStatus(listlayer);

    if (status & WAITUP)
    {
        __msg("wait to load! status = %d", status);
        listlayer->status |= WAIT_LOAD;
        esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_WAIT_LOAD_FLAG, OS_FLAG_SET, &err);
        listlayer->wait_flags->OSFlagFlags &= (~LISTLAYER_WAIT_FLAG);
        esKRNL_FlagPend(listlayer->wait_flags, LISTLAYER_WAIT_FLAG,
                        ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);
        listlayer->status &= (~WAIT_LOAD);
    }

    /*get listmenu layer sceen windows*/
    ret = GUI_LyrWinGetScnWindow(listlayer->layer, &listlayer->userscnrect);

    if (ret != 0)
    {
        return __LINE__;
    }

    if (listlayer->menulist->SelItem == NULL)
    {
        return __LINE__;
    }/*开始位置，此时不能往下移动*/
    else if (listlayer->menulist->dist == 0)
    {
        downlyr_dist  = dist;
        dist      = 0;
    }
    else
    {
        if ((listlayer->userscnrect.y == listlayer->scnrect.y)
            && (listlayer->userscnrect.height < listlayer->scnrect.height))
        {
            uplyr_dist = listlayer->scnrect.height - listlayer->userscnrect.height;

            if (uplyr_dist > dist)
            {
                uplyr_dist = dist;
                dist     = 0;
            }
            else
            {
                dist     = dist - uplyr_dist;
            }

            if (listlayer->menulist->dist > dist)
            {
                listlayer->menulist->dist -= dist;
                itemno  =    listlayer->menulist->dist / listlayer->menulist->SelItem->rect.height;
            }
            else
            {
                itemno  = 0;
                dist    = listlayer->menulist->dist;
                listlayer->menulist->dist = 0;
            }

            while (listlayer->menulist->SelItem->itemno != itemno)
            {
                listlayer->menulist->SelItem = listlayer->menulist->SelItem->Prev;
            }

            listlayer->menulist->HeadItem = listlayer->menulist->SelItem;
        }
        else
        {
            if (listlayer->menulist->dist > dist)
            {
                listlayer->menulist->dist -= dist;
                itemno  =    listlayer->menulist->dist / listlayer->menulist->SelItem->rect.height;
            }
            else
            {
                itemno  = 0;
                downlyr_dist = dist - listlayer->menulist->dist;
                dist    = listlayer->menulist->dist;
                listlayer->menulist->dist = 0;
            }

            while (listlayer->menulist->SelItem->itemno != itemno)
            {
                listlayer->menulist->SelItem = listlayer->menulist->SelItem->Prev;
            }

            listlayer->menulist->HeadItem = listlayer->menulist->SelItem;
        }
    }

    if (downlyr_dist)
    {
        listlayer->menulist->lyrdist += downlyr_dist;
        sw.y    = listlayer->userscnrect.y;
        sw.y    = sw.y + downlyr_dist;
        height  = listlayer->scnrect.y + listlayer->scnrect.height - sw.y;
        ret = ListLayer_SwitchLyr(listlayer, height, sw.y);

        if (ret != EPDK_OK)
        {
            return __LINE__;
        }

        listlayer->userscnrect.y = sw.y;
    }

    if (uplyr_dist)
    {
        listlayer->menulist->lyrdist += uplyr_dist;
        sw.y    = listlayer->userscnrect.y;
        sw.y    = sw.y;
        height  = listlayer->scnrect.height + listlayer->menulist->lyrdist;
        GUI_LyrWinCacheOn();
        ret = ListLayer_SwitchLyr(listlayer, height, sw.y);

        if (ret != EPDK_OK)
        {
            return __LINE__;
        }

        sw.y = listlayer->viewrect.y + (- listlayer->menulist->lyrdist);
        ret = ListLayer_SwitchFB(listlayer, 0, listlayer->viewrect.x, sw.y);

        if (ret != EPDK_OK)
        {
            return __LINE__;
        }

        GUI_LyrWinCacheOff();
        listlayer->userscnrect.y = sw.y;
    }

    if (dist)
    {
        sw.x = listlayer->viewrect.x;
        sw.y = listlayer->viewrect.y;
        sw.y = sw.y - dist;

        if (sw.y < 0)
        {
            sw.y = listlayer->listbuffer->vrect.height + sw.y;
        }
        else if (sw.y > listlayer->listbuffer->vrect.height)
        {
            sw.y = sw.y - listlayer->listbuffer->vrect.height;
        }

        esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_LOAD_FLAG, OS_FLAG_SET, &err);
        ret = ListLayer_SwitchFB(listlayer, fbno, sw.x, sw.y);

        if (ret != EPDK_OK)
        {
            __wrn("layer close failed,layer handle = %d", listlayer->layer);
            return __LINE__;
        }

        esKRNL_SemPend(listlayer->pos_sem, 0, &err); /*pend pos semphore*/

        if (err == OS_NO_ERR)
        {
            listlayer->viewrect.x = sw.x;
            listlayer->viewrect.y = sw.y;
        }

        esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/
        esKRNL_TimeDly(2);
    }

    return OK;
}


/*
**********************************************************************************************************************
*                                               ListBox_VarKeyDownMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer varkeydownmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListBox_FlatVarMoveUpMsg(LIST_LAYER_MAN_t    *listlayer, __u32 dist)
{
    __u8                        err;
    __u32                       ret;
    __u32                       status;
    __rect_t                    sw;
    __u32                       tmp;
    __u32                       itemno;
    __u32                       uplyr_dist = 0;
    __u32                       downlyr_dist = 0;
    __u32                       height;

    if (dist == 0)
    {
        return ORANGE_OK;
    }

    //__inf("listlayer->foucs.y = %d",listlayer->foucs_pos.y);
    status = ListBox_GetBufferStatus(listlayer);

    if (status & WAITDOWN)
    {
        __msg("wait to load! status = %d", status);
        listlayer->status |= WAIT_LOAD;
        esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_WAIT_LOAD_FLAG, OS_FLAG_SET, &err);
        listlayer->wait_flags->OSFlagFlags &= (~LISTLAYER_WAIT_FLAG);
        esKRNL_FlagPend(listlayer->wait_flags, LISTLAYER_WAIT_FLAG,
                        ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);
        listlayer->status &= (~WAIT_LOAD);
    }

    /*get listmenu layer sceen windows*/
    ret = GUI_LyrWinGetScnWindow(listlayer->layer, &listlayer->userscnrect);

    if (ret != 0)
    {
        return __LINE__;
    }

    if (listlayer->menulist->SelItem == NULL)
    {
        return __LINE__;
    }
    else if (listlayer->menulist->dist == listlayer->menulist->total_dist)
    {
        uplyr_dist  = dist;
        dist      = 0;
    }
    else
    {
        if ((listlayer->userscnrect.y > listlayer->scnrect.y)
            && (listlayer->userscnrect.height < listlayer->scnrect.height))
        {
            downlyr_dist = listlayer->scnrect.height - listlayer->userscnrect.height;

            if (downlyr_dist > dist)
            {
                downlyr_dist = dist;
                dist     = 0;
            }
            else
            {
                dist     = dist - downlyr_dist;
            }

            tmp = listlayer->menulist->dist;
            listlayer->menulist->dist += dist;

            if (listlayer->menulist->dist >= listlayer->menulist->total_dist)
            {
                listlayer->menulist->dist = listlayer->menulist->total_dist;

                if (listlayer->menulist->total_dist == 0)
                {
                    dist    =  0;
                    itemno  =  0;
                }
                else
                {
                    dist    =  listlayer->menulist->total_dist - tmp;
                    itemno  =  listlayer->menulist->ItemCount - listlayer->scnnum;
                }
            }
            else
            {
                itemno  =    listlayer->menulist->dist / listlayer->menulist->SelItem->rect.height;
            }

            while (listlayer->menulist->SelItem->itemno != itemno)
            {
                listlayer->menulist->SelItem = listlayer->menulist->SelItem->Next;
            }

            listlayer->menulist->HeadItem = listlayer->menulist->SelItem;
        }
        else
        {
            tmp = listlayer->menulist->dist;
            listlayer->menulist->dist += dist;

            if (listlayer->menulist->dist >= listlayer->menulist->total_dist)
            {
                listlayer->menulist->dist = listlayer->menulist->total_dist;

                if (listlayer->menulist->total_dist == 0)
                {
                    dist    =  0;
                    itemno  =  0;
                }
                else
                {
                    uplyr_dist = dist - (listlayer->menulist->total_dist - tmp);
                    dist    =  listlayer->menulist->total_dist - tmp;
                    itemno  =  listlayer->menulist->ItemCount - listlayer->scnnum;
                }
            }
            else
            {
                itemno  =    listlayer->menulist->dist / listlayer->menulist->SelItem->rect.height;
            }

            while (listlayer->menulist->SelItem->itemno != itemno)
            {
                listlayer->menulist->SelItem = listlayer->menulist->SelItem->Next;
            }

            listlayer->menulist->HeadItem = listlayer->menulist->SelItem;
        }
    }

    if (uplyr_dist)
    {
        listlayer->menulist->lyrdist -= uplyr_dist;
        sw.y    = listlayer->userscnrect.y;
        sw.y    = sw.y;
        height  = listlayer->scnrect.height + listlayer->menulist->lyrdist;
        GUI_LyrWinCacheOn();
        ret = ListLayer_SwitchLyr(listlayer, height, sw.y);

        if (ret != EPDK_OK)
        {
            return __LINE__;
        }

        sw.y = listlayer->viewrect.y + (- listlayer->menulist->lyrdist);
        ret = ListLayer_SwitchFB(listlayer, 0, listlayer->viewrect.x, sw.y);

        if (ret != EPDK_OK)
        {
            return __LINE__;
        }

        GUI_LyrWinCacheOff();
        listlayer->userscnrect.y = sw.y;
    }

    if (downlyr_dist)
    {
        listlayer->menulist->lyrdist -= downlyr_dist;
        sw.y    = listlayer->userscnrect.y;
        sw.y    = sw.y - downlyr_dist;
        height  = listlayer->scnrect.y + listlayer->scnrect.height - sw.y;
        ret = ListLayer_SwitchLyr(listlayer, height, sw.y);

        if (ret != EPDK_OK)
        {
            return __LINE__;
        }

        listlayer->userscnrect.y = sw.y;
    }

    if (dist)
    {
        sw.x = listlayer->viewrect.x;
        sw.y = listlayer->viewrect.y;
        sw.y = sw.y + dist;

        if (sw.y > listlayer->listbuffer->vrect.height)
        {
            sw.y = sw.y - listlayer->listbuffer->vrect.height;
        }
        else if (sw.y < 0)
        {
            sw.y = sw.y + listlayer->listbuffer->vrect.height;
        }

        esKRNL_FlagPost(listlayer->load_flags, LISTLAYER_LOAD_FLAG, OS_FLAG_SET, &err);
        ret = ListLayer_SwitchFB(listlayer, 0, sw.x, sw.y);

        if (ret != EPDK_OK)
        {
            __wrn("layer close failed,layer handle = %d", listlayer->layer);
            return __LINE__;
        }

        esKRNL_SemPend(listlayer->pos_sem, 0, &err); /*pend pos semphore*/

        if (err == OS_NO_ERR)
        {
            listlayer->viewrect.x = sw.x;
            listlayer->viewrect.y = sw.y;
        }

        esKRNL_SemPost(listlayer->pos_sem); /*post pos semphore*/
        esKRNL_TimeDly(2);
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListBox_FixedKeyUpMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer fixedkeyupmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListBox_FlatFixedMoveUpMsg(LIST_LAYER_MAN_t *listlayer, __u32 dist)
{
    __u32                       ret;
    __rect_t                    sw = {0, 0, 0, 0};
    __u32                       itemno;
    __u32                       tmp;
    __u32                       uplyr_dist = 0;
    __u32                       downlyr_dist = 0;
    __u32                       height;

    if (dist == 0)
    {
        return ORANGE_OK;
    }

    /*get listmenu layer sceen windows*/
    ret = GUI_LyrWinGetScnWindow(listlayer->layer, &listlayer->userscnrect);

    if (ret != 0)
    {
        return __LINE__;
    }

    if (listlayer->menulist->SelItem == NULL)
    {
        return __LINE__;
    }
    else if (listlayer->menulist->dist == listlayer->menulist->total_dist)
    {
        uplyr_dist  = dist;
        dist      = 0;
    }
    else
    {
        if ((listlayer->userscnrect.y > listlayer->scnrect.y)
            && (listlayer->userscnrect.height < listlayer->scnrect.height))
        {
            downlyr_dist = listlayer->scnrect.height - listlayer->userscnrect.height;

            if (downlyr_dist > dist)
            {
                downlyr_dist = dist;
                dist     = 0;
            }
            else
            {
                dist     = dist - downlyr_dist;
            }

            tmp = listlayer->menulist->dist;
            listlayer->menulist->dist += dist;

            if (listlayer->menulist->dist >= listlayer->menulist->total_dist)
            {
                listlayer->menulist->dist = listlayer->menulist->total_dist;

                if (listlayer->menulist->total_dist == 0)
                {
                    dist    =  0;
                    itemno  =  0;
                }
                else
                {
                    dist    =  listlayer->menulist->total_dist - tmp;
                    itemno  =  listlayer->menulist->ItemCount - listlayer->scnnum;
                }
            }
            else
            {
                itemno  =    listlayer->menulist->dist / listlayer->menulist->SelItem->rect.height;
            }

            while (listlayer->menulist->SelItem->itemno != itemno)
            {
                listlayer->menulist->SelItem = listlayer->menulist->SelItem->Next;
            }

            listlayer->menulist->HeadItem = listlayer->menulist->SelItem;
        }
        else
        {
            tmp = listlayer->menulist->dist;
            listlayer->menulist->dist += dist;

            if (listlayer->menulist->dist >= listlayer->menulist->total_dist)
            {
                listlayer->menulist->dist = listlayer->menulist->total_dist;

                if (listlayer->menulist->total_dist == 0)
                {
                    dist    =  0;
                    itemno  =  0;
                }
                else
                {
                    uplyr_dist = dist - (listlayer->menulist->total_dist - tmp);
                    dist    =  listlayer->menulist->total_dist - tmp;
                    itemno  =  listlayer->menulist->ItemCount - listlayer->scnnum;
                }
            }
            else
            {
                itemno  =    listlayer->menulist->dist / listlayer->menulist->SelItem->rect.height;
            }

            while (listlayer->menulist->SelItem->itemno != itemno)
            {
                listlayer->menulist->SelItem = listlayer->menulist->SelItem->Next;
            }

            listlayer->menulist->HeadItem = listlayer->menulist->SelItem;
        }
    }

    if (uplyr_dist)
    {
        listlayer->menulist->lyrdist -= uplyr_dist;
        sw.y    = listlayer->userscnrect.y;
        sw.y    = sw.y;
        height  = listlayer->scnrect.height + listlayer->menulist->lyrdist;
        GUI_LyrWinCacheOn();
        ret = ListLayer_SwitchLyr(listlayer, height, sw.y);

        if (ret != EPDK_OK)
        {
            return __LINE__;
        }

        sw.y = listlayer->viewrect.y + (- listlayer->menulist->lyrdist);
        ret = ListLayer_SwitchFB(listlayer, 0, listlayer->viewrect.x, sw.y);

        if (ret != EPDK_OK)
        {
            return __LINE__;
        }

        GUI_LyrWinCacheOff();
        listlayer->userscnrect.y = sw.y;
    }

    if (downlyr_dist)
    {
        listlayer->menulist->lyrdist -= downlyr_dist;
        sw.y    = listlayer->userscnrect.y;
        sw.y    = sw.y - downlyr_dist;
        height  = listlayer->scnrect.y + listlayer->scnrect.height - sw.y;
        ret = ListLayer_SwitchLyr(listlayer, height, sw.y);

        if (ret != EPDK_OK)
        {
            return __LINE__;
        }

        listlayer->userscnrect.y = sw.y;
    }

    if (dist)
    {
        sw.x = listlayer->viewrect.x;
        sw.y = listlayer->viewrect.y;
        sw.y = sw.y + dist;
        ret = ListLayer_SwitchFB(listlayer, 0, sw.x, sw.y);

        if (ret != EPDK_OK)
        {
            __wrn("layer close failed,layer handle = %d", listlayer->layer);
            return __LINE__;
        }

        listlayer->viewrect.x = sw.x;
        listlayer->viewrect.y = sw.y;
        esKRNL_TimeDly(2);
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListBox_FixedKeyDownMsg
*
* author:
*
* date:             2008-10-7
*
* Description:      listlayer fixedkeydownmsg
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 ListBox_FlatFixedMoveDownMsg(LIST_LAYER_MAN_t    *listlayer, __u32 dist)
{
    __u32                      ret;
    __rect_t                   sw;
    __u32                      itemno;
    __u32                       uplyr_dist = 0;
    __u32                       downlyr_dist = 0;
    __u32                       height;

    if (dist == 0)
    {
        return ORANGE_OK;
    }

    /*get listmenu layer sceen windows*/
    ret = GUI_LyrWinGetScnWindow(listlayer->layer, &listlayer->userscnrect);

    if (ret != 0)
    {
        return __LINE__;
    }

    if (listlayer->menulist->SelItem == NULL)
    {
        return __LINE__;
    }/*开始位置，此时不能往下移动*/
    else if (listlayer->menulist->dist == 0)
    {
        downlyr_dist  = dist;
        dist      = 0;
    }
    else
    {
        if ((listlayer->userscnrect.y == listlayer->scnrect.y)
            && (listlayer->userscnrect.height < listlayer->scnrect.height))
        {
            uplyr_dist = listlayer->scnrect.height - listlayer->userscnrect.height;

            if (uplyr_dist > dist)
            {
                uplyr_dist = dist;
                dist     = 0;
            }
            else
            {
                dist     = dist - uplyr_dist;
            }

            if (listlayer->menulist->dist > dist)
            {
                listlayer->menulist->dist -= dist;
                itemno  =    listlayer->menulist->dist / listlayer->menulist->SelItem->rect.height;
            }
            else
            {
                itemno  = 0;
                dist    = listlayer->menulist->dist;
                listlayer->menulist->dist = 0;
            }

            while (listlayer->menulist->SelItem->itemno != itemno)
            {
                listlayer->menulist->SelItem = listlayer->menulist->SelItem->Prev;
            }

            listlayer->menulist->HeadItem = listlayer->menulist->SelItem;
        }
        else
        {
            if (listlayer->menulist->dist > dist)
            {
                listlayer->menulist->dist -= dist;
                itemno  =    listlayer->menulist->dist / listlayer->menulist->SelItem->rect.height;
            }
            else
            {
                itemno  = 0;
                downlyr_dist = dist - listlayer->menulist->dist;
                dist    = listlayer->menulist->dist;
                listlayer->menulist->dist = 0;
            }

            while (listlayer->menulist->SelItem->itemno != itemno)
            {
                listlayer->menulist->SelItem = listlayer->menulist->SelItem->Prev;
            }

            listlayer->menulist->HeadItem = listlayer->menulist->SelItem;
        }
    }

    if (downlyr_dist)
    {
        listlayer->menulist->lyrdist += downlyr_dist;
        sw.y    = listlayer->userscnrect.y;
        sw.y    = sw.y + downlyr_dist;
        height  = listlayer->scnrect.y + listlayer->scnrect.height - sw.y;
        ret = ListLayer_SwitchLyr(listlayer, height, sw.y);

        if (ret != EPDK_OK)
        {
            return __LINE__;
        }

        listlayer->userscnrect.y = sw.y;
    }

    if (uplyr_dist)
    {
        listlayer->menulist->lyrdist += uplyr_dist;
        sw.y    = listlayer->userscnrect.y;
        sw.y    = sw.y;
        height  = listlayer->scnrect.height + listlayer->menulist->lyrdist;
        GUI_LyrWinCacheOn();
        ret = ListLayer_SwitchLyr(listlayer, height, sw.y);

        if (ret != EPDK_OK)
        {
            return __LINE__;
        }

        sw.y = listlayer->viewrect.y + (- listlayer->menulist->lyrdist);
        ret = ListLayer_SwitchFB(listlayer, 0, listlayer->viewrect.x, sw.y);

        if (ret != EPDK_OK)
        {
            return __LINE__;
        }

        GUI_LyrWinCacheOff();
        listlayer->userscnrect.y = sw.y;
    }

    if (dist)
    {
        sw.x = listlayer->viewrect.x;
        sw.y = listlayer->viewrect.y;
        sw.y = sw.y - dist;
        ret = ListLayer_SwitchFB(listlayer, 0, sw.x, sw.y);

        if (ret != EPDK_OK)
        {
            __wrn("layer close failed,layer handle = %d", listlayer->layer);
            return __LINE__;
        }

        listlayer->viewrect.x = sw.x;
        listlayer->viewrect.y = sw.y;
        esKRNL_TimeDly(2);
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               ListBox_FlatMoveToDist
*
* author:
*
* date:             2010-1-18
*
* Description:      listbox flatmovetodist
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  ListBox_FlatMoveToDist(LIST_LAYER_MAN_t    *listlayer, __s32 dist)
{
    __s32   move_dist = 0;
    __s32   lstm_dist = 0;
    __u32   other_dist;
    other_dist = LISTMENU_OTHER_NUM * listlayer->menulist->head->rect.height;

    if (listlayer->menulist->ItemCount > 0)
    {
        if (listlayer->menulist->dist == 0)
        {
            lstm_dist = other_dist - listlayer->menulist->lyrdist;
        }
        else if (listlayer->menulist->dist < listlayer->menulist->total_dist)
        {
            lstm_dist = other_dist + listlayer->menulist->dist;
        }
        else
        {
            lstm_dist = other_dist + listlayer->menulist->total_dist + (-listlayer->menulist->lyrdist);
        }

        if (dist < 0)
        {
            dist = 0;
        }
        else if (dist > listlayer->menulist->total_dist + 2 * other_dist)
        {
            dist = listlayer->menulist->total_dist + 2 * other_dist;
        }

        move_dist = dist - lstm_dist;

        /*负值*/
        if (move_dist < 0)
        {
            move_dist = -move_dist;
            ListLayer_MoveDownMsg(listlayer, move_dist, 0);
        }
        else
        {
            ListLayer_MoveUpMsg(listlayer, move_dist, 0);
        }
    }

    return ORANGE_OK;
}


/*
**********************************************************************************************************************
*                                               ListBox_FlatMoveGetDist
*
* author:
*
* date:             2010-1-18
*
* Description:      listbox flatmovegetdist
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  ListBox_FlatMoveGetDist(LIST_LAYER_MAN_t    *listlayer)
{
    __s32   lstm_dist = 0;
    __u32   other_dist;
    other_dist = LISTMENU_OTHER_NUM * listlayer->menulist->head->rect.height;

    if (listlayer->menulist->ItemCount > 0)
    {
        if (listlayer->menulist->dist == 0)
        {
            lstm_dist = other_dist - listlayer->menulist->lyrdist;
        }
        else if (listlayer->menulist->dist < listlayer->menulist->total_dist)
        {
            lstm_dist = other_dist + listlayer->menulist->dist;
        }
        else
        {
            lstm_dist = other_dist + listlayer->menulist->total_dist + (-listlayer->menulist->lyrdist);
        }
    }

    return lstm_dist;
}

/*
**********************************************************************************************************************
*                                               ListBox_GetMoveStatus
*
* author:
*
* date:             2010-2-25
*
* Description:      listbox getmovestatus
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  ListBox_GetMoveStatus(LIST_LAYER_MAN_t *listlayer)
{
    __u32                      ret;
    /*get listmenu layer sceen windows*/
    ret = GUI_LyrWinGetScnWindow(listlayer->layer, &listlayer->userscnrect);

    if (ret != 0)
    {
        return __LINE__;
    }

    if ((listlayer->userscnrect.y == listlayer->scnrect.y)
        && (listlayer->userscnrect.height < listlayer->scnrect.height))
    {
        return   LISTLAYER_ROUNDUP;
    }
    else if ((listlayer->userscnrect.y > listlayer->scnrect.y)
             && (listlayer->userscnrect.height < listlayer->scnrect.height))
    {
        return   LISTLAYER_ROUNDDOWN;
    }
    else
    {
        return   LISTLAYER_MOVE;
    }
}
//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------


#endif //__LIST_LAYER__C____

