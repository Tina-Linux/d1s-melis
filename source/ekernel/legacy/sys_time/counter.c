/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                              Time manage Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : counter.c
* By      : kevin.z
* Version : v2.0
* Date    : 2010-11-25 13:38
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-25 13:38    kevin.z     2.0     build the file.
*********************************************************************************************************
*/
#include "sys_time_i.h"
#include <string.h>
#include <sys_time.h>
#include <sys_clock.h>
//#include <csp.h>
#include <port.h>
#include <log.h>
#include <sunxi_drv_avs.h>

//clock handle for counter
static __hdle           h_clk;
static __time_cntr_t    TimeCntr;
static int32_t  CNTR_ClockChangeCb(uint32_t cmd, int32_t aux);


/*
*********************************************************************************************************
*                                   COUNTER INITIALISE
*
* Description: counter initialise;
*
* Arguments  : none
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t CNTR_Init(void)
{
    //initialise counter manager structure
    TimeCntr.pList = 0;
    TimeCntr.nCnt  = 0;

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   COUNTER EXIT
*
* Description: counter exit;
*
* Arguments  : none;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t CNTR_Exit(void)
{
    if (TimeCntr.nCnt)
    {
        __wrn("some counter has not been released!");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                               CALL-BACK FUNCTION FOR NOTIFY CLOCK CHANGE
*
* Description: clock change call back function
*
* Arguments  : cmd      command;
*              aux      prameter;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
static int32_t CNTR_ClockChangeCb(uint32_t cmd, int32_t aux)
{
    __time_cntr_node_t  *tmpCntr = TimeCntr.pList;

    //notify every counter user to process clock change
    while (tmpCntr)
    {
        tmpCntr->pCb(cmd, aux);
        tmpCntr = tmpCntr->pNext;
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   REQUEST COUNTER
*
* Description: request counter;
*
* Arguments  : cb   call-back function for process clock change;
*              pUsr name of coutner user;
*
* Returns    : result, counter handle;
*
* Note       :
*********************************************************************************************************
*/
__hdle esTIME_RequestCntr(__pCB_ClkCtl_t cb, char *pUsr)
{
    int32_t             cpu_sr;
    __time_cntr_node_t  *tmpCntr;
    int                 i = 0;
    char                devName[MAX_NAME_LEN];
    rt_device_t         devFd = RT_NULL;
    sunxi_driver_avs_t  *ppusr = NULL;

    tmpCntr = (__time_cntr_node_t *)k_malloc(sizeof(__time_cntr_node_t));
    if (!tmpCntr)
    {
        __wrn("request memory for counter failed!");
        return 0;
    }
    memset(tmpCntr, 0, sizeof(__time_cntr_node_t));

    //insert the counter to couter list
    cpu_sr = awos_arch_lock_irq();
    for (i = 0; i < AVS_NUM; i++)
    {
        sprintf(devName, "avs%d", i);
        devFd = rt_device_find(devName);
        if (devFd != RT_NULL)
        {
            ppusr = (sunxi_driver_avs_t *)devFd->user_data;
            if (ppusr->used == 0)
            {
                ppusr->used = 1;
                break;
            }
        }
    }

    if (i == AVS_NUM)
    {
        __wrn("request counter failed!");

        k_free(tmpCntr);
        return 0;
    }

    rt_device_open(devFd, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_STREAM);

    tmpCntr->hCntr  = devFd;
    tmpCntr->pCb    = cb;
    tmpCntr->nStat  = TMR_CNT_STAT_STOP;
    if (pUsr)
    {
        strncpy(tmpCntr->pUsr, pUsr, TIMER_USER_NAME_SIZE - 1);
    }

    tmpCntr->pNext = TimeCntr.pList;
    TimeCntr.pList = tmpCntr;
    TimeCntr.nCnt++;
    awos_arch_unlock_irq(cpu_sr);

    return (__hdle)tmpCntr;
}


/*
*********************************************************************************************************
*                                   RELEASE COUNTER
*
* Description: release counter;
*
* Arguments  : hCntr    counter handle;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esTIME_ReleaseCntr(__hdle hCntr)
{
    uint32_t            cpu_sr;
    __time_cntr_node_t  *tmpNode, *tmpNode0;
    sunxi_driver_avs_t  *ppusr = NULL;

    //delete the node from the list
    cpu_sr = awos_arch_lock_irq();
    tmpNode = tmpNode0 = TimeCntr.pList;
    while (tmpNode)
    {
        if (tmpNode == (__time_cntr_node_t *)hCntr)
        {
            if (tmpNode == TimeCntr.pList)
            {
                TimeCntr.pList = tmpNode->pNext;
            }
            else
            {
                tmpNode0->pNext = tmpNode->pNext;
            }
            TimeCntr.nCnt--;
            break;
        }
        tmpNode0 = tmpNode;
        tmpNode  = tmpNode->pNext;
    }
    awos_arch_unlock_irq(cpu_sr);

    if (tmpNode)
    {
        rt_device_close(tmpNode->hCntr);

        //release counter resource
        ppusr = (sunxi_driver_avs_t *)tmpNode->hCntr->user_data;
        ppusr->used = 0;
        //release memory resource
        k_free((void *)tmpNode);

        return EPDK_OK;
    }

    return EPDK_FAIL;
}


/*
*********************************************************************************************************
*                                   START COUNTER
*
* Description: start counter;
*
* Arguments  : hCntr    counter handle;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esTIME_StartCntr(__hdle hCntr)
{
    int32_t             result;
    __time_cntr_node_t  *tmpCntr = (__time_cntr_node_t *)hCntr;

    if (!hCntr)
    {
        __wrn("counter handle is invalid!");
        return EPDK_FAIL;
    }

    //start counter
    result = rt_device_control(tmpCntr->hCntr, AVS_ENABLE, NULL);
    if (result == EPDK_OK)
    {
        tmpCntr->nStat = TMR_CNT_STAT_RUN;
        return EPDK_OK;
    }

    return EPDK_FAIL;
}


/*
*********************************************************************************************************
*                                   STOP COUNTER
*
* Description: stop counter;
*
* Arguments  : hCntr    counter handle;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esTIME_StopCntr(__hdle hCntr)
{
    int32_t             result;
    __time_cntr_node_t  *tmpCntr = (__time_cntr_node_t *)hCntr;

    if (!hCntr)
    {
        __wrn("counter handle is invalid!");
        return EPDK_FAIL;
    }

    result = rt_device_control(tmpCntr->hCntr, AVS_DISABLE, NULL);
    if (result == EPDK_OK)
    {
        tmpCntr->nStat = TMR_CNT_STAT_STOP;
        return EPDK_OK;
    }

    return EPDK_FAIL;
}


/*
*********************************************************************************************************
*                                   PAUSE COUNTER
*
* Description: pause counter;
*
* Arguments  : hCntr    counter handle;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esTIME_PauseCntr(__hdle hCntr)
{
    int32_t             result;
    __time_cntr_node_t  *tmpCntr = (__time_cntr_node_t *)hCntr;

    if (!hCntr)
    {
        __wrn("counter handle is invalid!");
        return EPDK_FAIL;
    }

    result = rt_device_control(tmpCntr->hCntr, AVS_PAUSE, NULL);
    if (result == EPDK_OK)
    {
        tmpCntr->nStat = TMR_CNT_STAT_PAUSE;
        return EPDK_OK;
    }

    return EPDK_FAIL;
}


/*
*********************************************************************************************************
*                                   CONTINUE COUNTER
*
* Description: countinue counter;
*
* Arguments  : hCntr    counter handle;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esTIME_ContiCntr(__hdle hCntr)
{
    int32_t             result;
    __time_cntr_node_t  *tmpCntr = (__time_cntr_node_t *)hCntr;

    if (!hCntr)
    {
        __wrn("counter handle is invalid!");
        return EPDK_FAIL;
    }

    result = rt_device_control(tmpCntr->hCntr, AVS_CONTINUE, NULL);
    if (result == EPDK_OK)
    {
        tmpCntr->nStat = TMR_CNT_STAT_RUN;
        return EPDK_OK;
    }

    return EPDK_FAIL;
}


/*
*********************************************************************************************************
*                                   SET COUNTER VALUE
*
* Description: set counter value;
*
* Arguments  : hCntr    counter handle;
*              value    value for set counter;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esTIME_SetCntrValue(__hdle hCntr, uint32_t value)
{
    __time_cntr_node_t  *tmpCntr = (__time_cntr_node_t *)hCntr;

    if (!hCntr)
    {
        __wrn("counter handle is invalid!");
        return EPDK_FAIL;
    }

    return rt_device_control(tmpCntr->hCntr, AVS_SET_COUNTER, &value);
}


/*
*********************************************************************************************************
*                                   QUERRY COUNTER
*
* Description: querry counter value;
*
* Arguments  : hCntr    counter handle;
*
* Returns    : result, value of the counter;
*
* Note       :
*********************************************************************************************************
*/
uint32_t esTIME_QuerryCntr(__hdle hCntr)
{
    __time_cntr_node_t  *tmpCntr = (__time_cntr_node_t *)hCntr;
    u32                 counter = 0;

    if (!hCntr)
    {
        __wrn("counter handle is invalid!");
        return 0;
    }

    rt_device_control(tmpCntr->hCntr, AVS_GET_COUNTER, &counter);
    return counter;
}


/*
*********************************************************************************************************
*                                   SET COUNTER PRESCALE
*
* Description: set counter prescale;
*
* Arguments  : hCntr    counter handle;
*              prescl   precacle for set the counter;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esTIME_SetCntrPrescale(__hdle hCntr, int32_t prescl)
{
    __time_cntr_node_t  *tmpCntr = (__time_cntr_node_t *)hCntr;

    if (!hCntr)
    {
        __wrn("counter handle is invalid!");
        return EPDK_FAIL;
    }

    return rt_device_control(tmpCntr->hCntr, AVS_SET_DIV, &prescl);
}


/*
*********************************************************************************************************
*                                   QUERRY COUNTER STATUS
*
* Description: querry the status of counter;
*
* Arguments  : hCntr    counter handle;
*
* Returns    : result, status of the counter;
*
* Note       :
*********************************************************************************************************
*/
int32_t esTIME_QuerryCntrStat(__hdle hCntr)
{
    __time_cntr_node_t  *tmpCntr = (__time_cntr_node_t *)hCntr;

    if (!hCntr)
    {
        __wrn("counter handle is invalid!");
        return TMR_CNT_STAT_INVALID;
    }

    return tmpCntr->nStat;
}


