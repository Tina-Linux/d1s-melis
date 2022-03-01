/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_channel.h
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
#ifndef  __robin_channel_h
#define  __robin_channel_h
#include <emodules/mod_audio.h>

/***************************************************************************************************
*Name        : robin_set_channel
*Prototype   : __s32  robin_set_channel( __audio_dev_chan_mode_t  channel )
*Arguments   : channel   input. channel mode are defined in drv_audio.h, see __audio_dev_chan_mode_t.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the audio channel.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_channel(__audio_dev_chan_mode_t      channel);

/***************************************************************************************************
*Name        : robin_get_channel
*Prototype   : __audio_dev_chan_mode_t  robin_get_channel( void )
*Arguments   : void
*Return      : the current channel mode
*Description : get the current channel mode
*Other       :
***************************************************************************************************/
extern __audio_dev_chan_mode_t  robin_get_channel(void);


#endif     //  ifndef __robin_channel_h

/* end of robin_channel.h */
