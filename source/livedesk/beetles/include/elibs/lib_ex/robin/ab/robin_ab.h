/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_ab.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.27
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
* Gary.Wang      2009.09.27       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_ab_h
#define  __robin_ab_h

#include <kconfig.h>

/***************************************************************************************************
*Name        : robin_set_ab_a
*Prototype   : __s32  robin_set_ab_a( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set the current time point as the A point in AB play mode.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_ab_a(void);
extern __s32  robin_set_ab_a_v2(void);


/***************************************************************************************************
*Name        : robin_set_ab_b
*Prototype   : __s32  robin_set_ab_b( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set the current time point as the B point in AB play mode and start AB play mode.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_ab_b(void);
extern __s32  robin_set_ab_b_v2(void);

/***************************************************************************************************
*Name        : robin_set_ab_loop_count
*Prototype   : __s32  robin_set_ab_loop_count( __u32 count )
*Arguments   : count     input. AB loop count.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the loop count in AB play mode.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_ab_loop_count(__u32 count);
extern __s32  robin_set_ab_loop_count_v2(__u32 count);

/***************************************************************************************************
*Name        : robin_cancle_ab
*Prototype   : __s32  robin_cancle_ab( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : cancle AB play.
*Other       :
***************************************************************************************************/
extern __s32  robin_cancle_ab(void);
extern __s32  robin_cancle_ab_v2(void);
/***************************************************************************************************
*Name        : robin_enable_ab
*Prototype   : __s32  robin_enable_ab( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : enable AB play.
*Other       :
***************************************************************************************************/
extern __s32  robin_enable_ab(void);
extern __s32  robin_set_audio_ab_mode(__u32 mode);


#endif     //  ifndef __robin_ab_h
