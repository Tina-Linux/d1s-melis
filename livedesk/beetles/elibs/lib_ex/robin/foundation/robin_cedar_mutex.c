/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_cedar_mutex.c
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
#ifndef  __robin_cedar_mutex_c
#define  __robin_cedar_mutex_c


#include "foundation_i.h"



g_mutex_t  volatile  robin_cedar_mutex = NULL;


/***************************************************************************************************
*Name        : robin_cedar_mutex_create
*Prototype   : __s32 robin_cedar_mutex_create( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : create cedar mutex
*Other       :
***************************************************************************************************/
__s32 robin_cedar_mutex_create(void)
{
    if (robin_cedar_mutex == NULL)
    {
        robin_cedar_mutex = g_create_mutex();

        if (robin_cedar_mutex == NULL)
        {
            __err("Error in creating robin_cedar_mutex!\n");
            return -1;
        }
    }

    return 0;
}



/***************************************************************************************************
*Name        : robin_cedar_mutex_delete
*Prototype   : void  robin_cedar_mutex_delete( void )
*Arguments   : void
*Return      : void
*Description : delete cedar mutex
*Other       :
***************************************************************************************************/
void  robin_cedar_mutex_delete(void)
{
    __u8  err;

    if (robin_cedar_mutex != NULL)
    {
        g_delete_mutex(robin_cedar_mutex, &err);
        robin_cedar_mutex = NULL;
    }
}





#endif     //  ifndef __robin_cedar_mutex_c

/* end of robin_cedar_mutex.c */
