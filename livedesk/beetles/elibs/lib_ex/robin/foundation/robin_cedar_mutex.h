/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_cedar_mutex.h
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
#ifndef  __robin_cedar_mutex_h
#define  __robin_cedar_mutex_h



extern g_mutex_t  volatile  robin_cedar_mutex;


/***************************************************************************************************
*Name        : robin_cedar_mutex_create
*Prototype   : __s32 robin_cedar_mutex_create( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : create cedar mutex
*Other       :
***************************************************************************************************/
extern __s32 robin_cedar_mutex_create(void);

/***************************************************************************************************
*Name        : robin_cedar_mutex_delete
*Prototype   : void  robin_cedar_mutex_delete( void )
*Arguments   : void
*Return      : void
*Description : delete cedar mutex
*Other       :
***************************************************************************************************/
extern void  robin_cedar_mutex_delete(void);



#endif     //  ifndef __robin_cedar_mutex_h

/* end of robin_cedar_mutex.h */
