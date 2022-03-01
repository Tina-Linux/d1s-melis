/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_cmd_mutex.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.11.22
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
* Gary.Wang      2009.11.22       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_cmd_mutex_c
#define  __robin_cmd_mutex_c


#include "foundation_i.h"



g_mutex_t  volatile  robin_cmd_mutex = NULL;


/***************************************************************************************************
*Name        : robin_cmd_mutex_create
*Prototype   : __s32 robin_cmd_mutex_create( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    g_create_mutex
*Description : create cmd mutex
*Other       :
***************************************************************************************************/
__s32 robin_cmd_mutex_create(void)
{
    if (robin_cmd_mutex == NULL)
    {
        robin_cmd_mutex = g_create_mutex();

        if (robin_cmd_mutex == NULL)
        {
            __err("Error in creating robin_cmd_mutex!\n");
            return -1;
        }
    }

    return 0;
}



/***************************************************************************************************
*Name        : robin_cmd_mutex_delete
*Prototype   : void  robin_cmd_mutex_delete( void )
*Arguments   : void
*Return      : void
*Description : delete cmd mutex
*Other       :
***************************************************************************************************/
void  robin_cmd_mutex_delete(void)
{
    __u8  err;

    if (robin_cmd_mutex != NULL)
    {
        g_delete_mutex(robin_cmd_mutex, &err);
        robin_cmd_mutex = NULL;
    }
}





#endif     //  ifndef __robin_cmd_mutex_c

/* end of robin_cmd_mutex.c */
