/*
************************************************************************************************************************
*                                                       walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_monitor_gate.h
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
#ifndef  __DSK_WKM_MONITOR_GATE_H__
#define  __DSK_WKM_MONITOR_GATE_H__

#ifdef __cplusplus
extern "C" {
#endif



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
extern __s32  dsk_wkm_monitor_enable(void);

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
extern __s32  dsk_wkm_monitor_disable(void);

#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_WKM_MONITOR_GATE_H__

/* end of dsk_wkm_monitor_gate.h */
