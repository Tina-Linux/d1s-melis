/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_feedbackq.c
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
#ifndef  __robin_feedbackq_c
#define  __robin_feedbackq_c


#include "foundation_i.h"


#define ROBIN_FEEDBACKQ_LEN                  1


g_Queue  volatile  robin_feedbackQ = NULL;


/***************************************************************************************************
*Name        : robin_feedbackQ_create
*Prototype   : __s32 robin_feedbackQ_create( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : create feedback message queue
*Other       :
***************************************************************************************************/
__s32 robin_feedbackQ_create(void)
{
    if (robin_feedbackQ == NULL)
    {
        robin_feedbackQ = g_QCreate(ROBIN_FEEDBACKQ_LEN);

        if (robin_feedbackQ == NULL)
        {
            __err("Error in creating robin_feedbackQ!\n");
            return -1;
        }
    }

    return 0;
}



/***************************************************************************************************
*Name        : robin_feedbackQ_delete
*Prototype   : void  robin_feedbackQ_delete( void )
*Arguments   : void
*Return      : void
*Description : delete feedback message queue
*Other       :
***************************************************************************************************/
void  robin_feedbackQ_delete(void)
{
    __u8  err;

    if (robin_feedbackQ != NULL)
    {
        g_QDel(robin_feedbackQ, OS_DEL_ALWAYS, &err);
        robin_feedbackQ = NULL;
    }
}





#endif     //  ifndef __robin_feedbackq_c

/* end of robin_feedbackq.c */
