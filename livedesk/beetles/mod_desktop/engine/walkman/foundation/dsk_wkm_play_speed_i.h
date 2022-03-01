/*
************************************************************************************************************************
*                                                       walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_play_speed_i.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.07
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
* Gary.Wang      2010.10.07        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_WKM_PLAY_SPEED_I_H__
#define  __DSK_WKM_PLAY_SPEED_I_H__

#ifdef __cplusplus
extern "C" {
#endif


/*
****************************************************************************************************
*Name        : __dsk_wkm_set_play_speed
*Prototype   : __s32  __dsk_wkm_set_play_speed( __s32 play_speed )
*Arguments   : play_speed    input. speed value, that should be in range AUDIO_VPS_SLOWEST~~~~~~
*                             AUDIO_VPS_FASTEST.
*Return      : ==0       succeed
*              !=0       fail
*Description : set speed.
*Other       :
****************************************************************************************************
*/
extern __s32  __dsk_wkm_set_play_speed(__s32 play_speed);

/*
****************************************************************************************************
*Name        : __dsk_wkm_get_play_speed
*Prototype   : __s32  __dsk_wkm_get_play_speed( void )
*Arguments   : void
*Return      : the current speed value.
*Description : get the current speed value.
*Other       :
****************************************************************************************************
*/
extern __s32  __dsk_wkm_get_play_speed(void);



#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_WKM_PLAY_SPEED_I_H__

/* end of dsk_wkm_play_speed_i.h */
