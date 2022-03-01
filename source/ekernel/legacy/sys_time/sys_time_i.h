/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                 Time Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : sys_time_i.h
* By      : kevin.z
* Version : v2.0
* Date    : 2010-11-24 9:52
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-24 9:52     kevin.z     2.0     build the file.
*********************************************************************************************************
*/
#ifndef _SYS_TIME_I_H_
#define _SYS_TIME_I_H_
#include <typedef.h>
#include <ktype.h>
#include <rtdef.h>

#define TIMER_USER_NAME_SIZE    (16)

typedef struct __TIME_TMR_NODE
{
    uint32_t    timerId;        //timer id
    uint32_t    precision;      //timer precision
    uint32_t    nPeriod;        //period of the timer
    uint32_t    isOneShot;      //one shot or continuous
    uint32_t    nIrqNo;         //irq numer
    __pCBK_t    pHdlr;          //timer interrupt handle
    void        *pArg;          //argument for interrupt handle
    char        pUsr[TIMER_USER_NAME_SIZE];
} __time_tmr_node_t;

typedef struct __TIME_CNTR_NODE
{
    rt_device_t     hCntr;  //counter handle request from csp
    __pCB_ClkCtl_t  pCb;    //call-back function for clock-change
    __s32           nStat;  //status
    char            pUsr[TIMER_USER_NAME_SIZE];
    struct __TIME_CNTR_NODE *pNext;
} __time_cntr_node_t;

typedef struct __TIME_CNTR
{
    __time_cntr_node_t  *pList;
    __s32               nCnt;
} __time_cntr_t;


#endif /* _SYS_TIME_I_H_    */

