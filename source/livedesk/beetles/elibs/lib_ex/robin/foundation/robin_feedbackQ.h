/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_feedbackq.h
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
#ifndef  __robin_feedbackq_h
#define  __robin_feedbackq_h




extern g_Queue  volatile  robin_feedbackQ;


extern __s32 robin_feedbackQ_create(void);
extern void  robin_feedbackQ_delete(void);



#endif     //  ifndef __robin_feedbackq_h

/* end of robin_feedbackq.h */
