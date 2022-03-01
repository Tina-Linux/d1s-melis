/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                              Time manage Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : timer.c
* By      : kevin.z
* Version : v2.0
* Date    : 2010-11-25 13:45
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-25 13:45    kevin.z     2.0     modify the file.
*********************************************************************************************************
*/
#include "sys_time_i.h"
#include <port.h>
#include <sys_time.h>
#include <sys_int.h>
#include <string.h>
#include <log.h>
#include <arch.h>
#include <rtthread.h>
#include <sunxi_hal_timer.h>
#include <sunxi_drv_timer.h>

static __time_tmr_node_t*    TimeTmr[TIMER_NUM];

/*
*********************************************************************************************************
*                                   TIMER MANAGER INITIALISE
*
* Description: timer manager initialise;
*
* Arguments  : none
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t TMR_Init(void)
{
    int32_t i = 0;
    for(i = 0; i < TIMER_NUM; i ++)
    {
        TimeTmr[i] = NULL;
    }

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                   TIMER MANAGER EXIT
*
* Description: timer manager exit;
*
* Arguments  : none;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t TMR_Exit(void)
{
    int32_t i = 0;
    for(i = 0; i < TIMER_NUM; i ++)
    {
        if(TimeTmr[i])
        {

        }
    }

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                   REQUEST TIMER
*
* Description: request timer.
*
* Arguments  : precision    type of timer need be requested;
*              nPeriod      timer call-back function;
*              isOneShot    one shot or continuous;
*              pHdlr        timer handle
*              pArg         timer handle argument
*              pUsr         user name;
*
* Returns    : result, handle of the timer.
*
* Note       :
*********************************************************************************************************
*/
int32_t esTIME_RequestTimer(__csp_timer_req_type_t *tmrType, __pCBK_t pHdlr, void *pArg, char *pUsr)
{
    int32_t             cpu_sr;
    __time_tmr_node_t   *tmpTmr;
    int32_t             i = 0, intervel = 0;

    for (i = 0; i < TIMER_NUM; i++)
    {
        if (NULL == TimeTmr[i])
        {
            break;
        }
    }

    if(TIMER_NUM <= i)
    {
        return -1;
    }

    tmpTmr = (__time_tmr_node_t *)rt_malloc(sizeof(__time_tmr_node_t));
    if (NULL == TimeTmr[i])
    {
        __err("request memory for timer failed!");
        return 0;
    }
   
    tmpTmr  = TimeTmr[i];

    rt_memset(tmpTmr, 0, sizeof(__time_tmr_node_t));

    //initialise timer node
    tmpTmr->timerId     = i;
    tmpTmr->precision   = tmrType->precision;
    tmpTmr->nPeriod     = tmrType->nPeriod;
    tmpTmr->isOneShot   = tmrType->isOneShot;
    tmpTmr->nIrqNo      = drv_timer_id_to_irq(tmpTmr->timerId);
    tmpTmr->pHdlr       = pHdlr;
    tmpTmr->pArg        = pArg;
    if (pUsr)
    {
        rt_strncpy(tmpTmr->pUsr, pUsr, TIMER_USER_NAME_SIZE - 1);
    }

    /*modified the global varietiy in lock_irq conditions*/
    cpu_sr = awos_arch_lock_irq();
    TimeTmr[i]  = tmpTmr;
    awos_arch_unlock_irq(cpu_sr);

    drv_timer_set_mode(tmpTmr->timerId , tmpTmr->isOneShot);
    switch (tmrType->precision)
    {
        case TMR_PRECISION_MICRO_SECOND:
        {
            drv_timer_set_pres(tmpTmr->timerId , 3); /* 2^3 = 8 presale from 24,000,000*/
            intervel    = 3 * tmrType->nPeriod;
            drv_timer_set_interval(tmpTmr->timerId, intervel);
            break;
        }
        case TMR_PRECISION_MILLI_SECOND:
        {
            drv_timer_set_pres(tmpTmr->timerId , 6); /* 2^6 = 64 presale from 24,000,000*/
            intervel    = 375 * tmrType->nPeriod;
            drv_timer_set_interval(tmpTmr->timerId, intervel);
            break;
        }
        case TMR_PRECISION_SECOND:
        {
            drv_timer_set_pres(tmpTmr->timerId , 7); /* 2^7 = 128 presale from 24,000,000*/
            intervel    = 187500 * tmrType->nPeriod;
            drv_timer_set_interval(tmpTmr->timerId, intervel);
            break;
        }
        default:
            break;
    }

    //install timer interrupt handler
    drv_timer_irq_request(tmpTmr->nIrqNo, pHdlr, 0, pArg);

    return i;

_error:
    if(TimeTmr[i])
    {
        rt_free(TimeTmr[i]);
        TimeTmr[i]  = NULL;
    }
    return -1;
}

/*
*********************************************************************************************************
*                                   RELEASE TIMER
*
* Description: release timer
*
* Arguments  : hTmr     timer handle;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esTIME_ReleaseTimer(int32_t timer_id)
{
    uint32_t            cpu_sr;
    __time_tmr_node_t   *tmpNode, *tmpNode0;

    tmpNode = TimeTmr[timer_id];

    if (tmpNode)
    {
        //disable interrupt and uninstall isr
        drv_timer_irq_disable(tmpNode->nIrqNo);
        //uninstall timer interrupt handle
        drv_timer_irq_free(tmpNode->nIrqNo);
        //release memory resource
        rt_free((void *)tmpNode);

        /*only disable irq and free handle? do not stop the timer????*/
        return EPDK_OK;
    }

    return EPDK_FAIL;
}

/*
*********************************************************************************************************
*                                   START TIMER
*
* Description: start timer;
*
* Arguments  : hTmr     timer handle;
*              nPeriod  period of the timer trigger;
*              nMode    mode the timer trigger;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esTIME_StartTimer(int32_t timer_id)
{
    //__time_tmr_node_t   *tmpTmr;

    if (timer_id < 0 || timer_id >= TIMER_NUM)
    {
        __err("timer handle is invalid!");
        return EPDK_FAIL;
    }

    drv_timer_irq_enable(TimeTmr[timer_id]->nIrqNo);

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                   STOP TIMER
*
* Description: stop timer;
*
* Arguments  : hTmr     timer hadnle;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esTIME_StopTimer(int32_t timer_id)
{
    //__time_tmr_node_t   *tmpTmr;

    if (timer_id < 0 || timer_id >= TIMER_NUM)
    {
        __err("timer handle is invalid!");
        return EPDK_FAIL;
    }

    drv_timer_irq_disable(TimeTmr[timer_id]->nIrqNo);
    //drv_timer_irq_disable(TimeTmr[timer_id]->nIrqNo);

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                   QUERRY TIMER
*
* Description: querry timer;
*
* Arguments  : hTmr     timer hadnle;
*
* Returns    : result, timer left time;
*
* Note       :
*********************************************************************************************************
*/
uint32_t esTIME_QuerryTimer(int32_t timer_id)
{
    //__time_tmr_node_t   *tmpTmr;
    uint32_t value = 0;

    if (timer_id < 0 || timer_id >= TIMER_NUM)
    {
        __err("timer handle is invalid!");
        return EPDK_FAIL;
    }

    drv_timer_get_counter(timer_id, &value);
    return value;
}

