/*
************************************************************************************************************************
*                                                       walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_monitor_gate.c
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.07
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2010.10.07        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_WKM_MONITOR_GATE_C__
#define  __DSK_WKM_MONITOR_GATE_C__

#include "dsk_walkman_i.h"


/*
****************************************************************************************************
*Name        : __dsk_wkm_monitor_enable
*Prototype   : __s32  dsk_wkm_monitor_enable( void )
*Arguments   :
*Return      : ==0       succeed
*              !=0       fail
*Description : enable monitor.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_monitor_enable(void)
{
    __u8   err;
    g_pend_mutex(dsk_wkm_res_mutex, &err);
    __dsk_wkm_monitor_enable();
    g_post_mutex(dsk_wkm_res_mutex);
    return 0;
}

/*
****************************************************************************************************
*Name        : __dsk_wkm_monitor_enable
*Prototype   : __s32  dsk_wkm_monitor_disable( void )
*Arguments   :
*Return      : ==0       succeed
*              !=0       fail
*Description : disable monitor.
*Other       :

****************************************************************************************************
*/
__s32  dsk_wkm_monitor_disable(void)
{
    __u8   err;
    g_pend_mutex(dsk_wkm_res_mutex, &err);
    __dsk_wkm_monitor_disable();
    g_post_mutex(dsk_wkm_res_mutex);
    return 0;
}

#endif     //  ifndef __DSK_WKM_MONITOR_GATE_C__

/* end of dsk_wkm_monitor_gate.h */
