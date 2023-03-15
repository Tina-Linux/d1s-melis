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
#include <string.h>

#include "sys_time_i.h"
#include <port.h>
#include <sys_time.h>
#include <sys_int.h>
#include <log.h>
#include <arch.h>
#include <sunxi_hal_timer.h>
#include <sunxi_drv_timer.h>

#include <hal_mem.h>

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

    tmpTmr = (__time_tmr_node_t *)hal_malloc(sizeof(__time_tmr_node_t));
    if (NULL == TimeTmr[i])
    {
        __err("request memory for timer failed!");
        return 0;
    }
   
    tmpTmr  = TimeTmr[i];

    memset(tmpTmr, 0, sizeof(__time_tmr_node_t));

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
        strncpy(tmpTmr->pUsr, pUsr, TIMER_USER_NAME_SIZE - 1);
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
        hal_free(TimeTmr[i]);
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
        hal_free((void *)tmpNode);

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
