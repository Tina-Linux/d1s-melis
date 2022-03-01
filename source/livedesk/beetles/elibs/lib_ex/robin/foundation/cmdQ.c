/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : cmdq.c
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
#ifndef  __cmdq_c
#define  __cmdq_c


#include "foundation_i.h"
#include <kapi.h>


#define ROBIN_CMDQ_LEN                  1
#define ROBIN_MSG_BUF_LEN               16        // must be 2^n


g_Queue  volatile  robin_cmdQ = NULL;


static robin_msg_t              robin_msg_array[ROBIN_MSG_BUF_LEN];
static __u32          volatile  robin_msg_counter = 0;


/***************************************************************************************************
*Name        : cmdQ_create
*Prototype   : __s32 cmdQ_create( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : create command queue
*Other       :
***************************************************************************************************/
__s32 cmdQ_create(void)
{
    if (robin_cmdQ == NULL)
    {
        robin_cmdQ = g_QCreate(ROBIN_CMDQ_LEN);

        if (robin_cmdQ == NULL)
        {
            __err("Error in creating cmdQ!\n");
            return -1;
        }
    }

    return 0;
}



/***************************************************************************************************
*Name        : cmdQ_delete
*Prototype   : void  cmdQ_delete( void )
*Arguments   : void
*Return      : void
*Description : delete command queue
*Other       :
***************************************************************************************************/
void  cmdQ_delete(void)
{
    __u8  err;

    if (robin_cmdQ != NULL)
    {
        g_QDel(robin_cmdQ, OS_DEL_ALWAYS, &err);
        robin_cmdQ = NULL;
    }
}



robin_msg_t  *get_robin_msg_buf(void)
{
    return  &(robin_msg_array[robin_msg_counter++ & (ROBIN_MSG_BUF_LEN - 1)]);
}





#endif     //  ifndef __cmdq_c

/* end of cmdq.c */
