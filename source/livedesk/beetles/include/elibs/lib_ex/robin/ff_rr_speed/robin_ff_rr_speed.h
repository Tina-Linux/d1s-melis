/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_ff_rr_speed.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.26
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
* Gary.Wang      2009.09.26       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_ff_rr_speed_h
#define  __robin_ff_rr_speed_h


/***************************************************************************************************
*Name        : robin_set_ff_rr_speed
*Prototype   : __s32  robin_set_ff_rr_speed( __u32 ff_rr_speed )
*Arguments   : ff_rr_speed   input. the speed of ff or rr. the speed rang is CEDAR_SLOWEST_FRSPEED~~
*                            ~~CEDAR_FASTEST_FRSPEED.
*Return      : ==0     succeed
*              !=0     fail
*Description : set the speed of ff or rr.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_ff_rr_speed(__u32 ff_rr_speed);

/***************************************************************************************************
*Name        : robin_get_ff_rr_speed
*Prototype   : __u32  robin_get_ff_rr_speed( void )
*Arguments   : void
*Return      : the speed of ff or rr.
*Description : get the speed of ff or rr.
*Other       :
***************************************************************************************************/
extern __u32  robin_get_ff_rr_speed(void);


#endif     //  ifndef __robin_ff_rr_speed_h

/* end of robin_ff_rr_speed.h */
