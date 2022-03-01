/*
************************************************************************************************************************
*                                                 desktop audio_if manage
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_audio_if.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.23
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
* Gary.Wang      2010.10.23        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_AUDIO_IF_H__
#define  __DSK_AUDIO_IF_H__



/*
****************************************************************************************************
*Name        : dsk_set_audio_if
*Prototype   : __s32 dsk_set_audio_if( __audio_dev_interface_t dev_if_type )
*Arguments   : dev_if_type   input. audio device interface type.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : set the audio device interface, such as iis, codac, ...
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_set_audio_if(__audio_dev_interface_t dev_if_type);

/*
****************************************************************************************************
*Name        : __audio_dev_interface_t
*Prototype   : __audio_dev_interface_t  dsk_get_audio_if( void )
*Arguments   : void
*Return      : the audio device interface type.
*Description : get the current audio device interface, such as iis, codac, ...
*Other       :
****************************************************************************************************
*/
__audio_dev_interface_t  dsk_get_audio_if(void);

/*
****************************************************************************************************
*Name        : dsk_set_hdmi_audio_gate
*Prototype   : __s32 dsk_set_hdmi_audio_gate( __bool flag )
*Arguments   : flag      input. on or off.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : enable or disable hdmi to output audio.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_set_hdmi_audio_gate(__bool flag);



#endif     //  ifndef __DSK_AUDIO_IF_H__

/* end of dsk_audio_if.h */
