/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_play_speed.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.25
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
* Gary.Wang      2009.09.25       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_play_speed_h
#define  __robin_play_speed_h


/***************************************************************************************************
*Name        : robin_set_play_speed
*Prototype   : __s32  robin_set_play_speed( __s32 play_speed )
*Arguments   : play_speed    input. speed value, that should be in range AUDIO_VPS_SLOWEST~~~~~~
*                            AUDIO_VPS_FASTEST.
*Return      : ==0       succeed
*              !=0       fail
*Description : set speed.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_play_speed(__s32 play_speed);

/***************************************************************************************************
*Name        : robin_get_play_speed
*Prototype   : __s32  robin_get_play_speed( void )
*Arguments   : void
*Return      : the current speed value.
*Description : get the current speed value.
*Other       :
***************************************************************************************************/
extern __s32  robin_get_play_speed(void);


#endif     //  ifndef __robin_play_speed_h

/* end of robin_play_speed.h */
