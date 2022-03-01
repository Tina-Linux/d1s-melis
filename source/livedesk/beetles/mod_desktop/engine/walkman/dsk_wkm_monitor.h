/*
************************************************************************************************************************
*                                                        dsk_walkman
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_monitor.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.11.19
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
* Gary.Wang      2009.11.19       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __dsk_wkm_monitor_h
#define  __dsk_wkm_monitor_h



extern __s32  dsk_wkm_monitor_create(__u8 prio);
extern void   dsk_wkm_monitor_delete(void);
extern __s32  __dsk_wkm_monitor_enable(void);
extern __s32  __dsk_wkm_monitor_disable(void);



#endif     //  ifndef __dsk_wkm_monitor_h

/* end of dsk_wkm_monitor.h */
