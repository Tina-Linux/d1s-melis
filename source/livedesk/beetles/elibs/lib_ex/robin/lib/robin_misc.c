/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_misc.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.11.30
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
* Gary.Wang      2009.11.30       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_misc_c
#define  __robin_misc_c

#include <typedef.h>
#include <log.h>
#include <kapi.h>
#include "robin_lib.h"


__s32  get_msg_nr_of_q(g_Queue q)
{
    __krnl_q_data_t  Q_data;
    g_QQuery(q, &Q_data);
    return Q_data.OSNMsgs;
}




#endif     //  ifndef __robin_misc_c

/* end of robin_misc.c */
