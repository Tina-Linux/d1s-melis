/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : cmdq.h
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
#ifndef  __cmdq_h
#define  __cmdq_h


typedef struct _robin_msg_t
{
    __u32  id;
    __u32  data;
    __cedar_media_file_inf  file_info;
} robin_msg_t;


extern g_Queue  volatile  robin_cmdQ;


extern __s32  cmdQ_create(void);
extern void   cmdQ_delete(void);
extern robin_msg_t  *get_robin_msg_buf(void);





#endif     //  ifndef __cmdq_h

/* end of cmdq.h */
