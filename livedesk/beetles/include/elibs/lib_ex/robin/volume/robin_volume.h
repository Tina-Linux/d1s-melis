/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_volume.h
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
#ifndef  __robin_volume_h
#define  __robin_volume_h


/***************************************************************************************************
*Name        : robin_set_volume
*Prototype   : __s32  robin_set_volume( __u32 volume )
*Arguments   : volume   input. volume value ranges AUDIO_DEVICE_VOLUME_MIN~~AUDIO_DEVICE_VOLUME_MAX,
*                       that are defined in drv_audio.h.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the volume.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_volume(__u32 volume);

/***************************************************************************************************
*Name        : robin_get_volume
*Prototype   : __u32  robin_get_volume( void )
*Arguments   : void
*Return      : the current volume mode
*Description : get the current volume mode
*Other       :
***************************************************************************************************/
extern __u32  robin_get_volume(void);


#endif     //  ifndef __robin_volume_h

/* end of robin_volume.h */
