/*
************************************************************************************************************************
*                                                       walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_feedbackq.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.06
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
* Gary.Wang      2010.10.06        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_WKM_FEEDBACKQ_H__
#define  __DSK_WKM_FEEDBACKQ_H__

#ifdef __cplusplus
extern "C" {
#endif



extern g_Queue  volatile  dsk_wkm_feedbackQ;


extern __s32 __dsk_wkm_feedbackQ_create(void);
extern void  __dsk_wkm_feedbackQ_delete(void);



#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_WKM_FEEDBACKQ_H__

/* end of dsk_wkm_feedbackq.h */
