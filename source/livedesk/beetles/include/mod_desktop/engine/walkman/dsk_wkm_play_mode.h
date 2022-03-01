/*
************************************************************************************************************************
*                                                       walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_play_mode.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.06
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
* Gary.Wang      2010.10.06        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_WKM_PLAY_MODE_H__
#define  __DSK_WKM_PLAY_MODE_H__

#ifdef __cplusplus
extern "C" {
#endif



/*
****************************************************************************************************
*Name        : dsk_wkm_set_play_mode
*Prototype   : __s32  dsk_wkm_set_play_mode( orc_play_mode_e  play_mode )
*Arguments   : play_mode   input. play mode are defined in drv_orchid.h, see orc_play_mode_e.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the play mode.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_set_play_mode(orc_play_mode_e  play_mode);

/*
****************************************************************************************************
*Name        : dsk_wkm_get_play_mode
*Prototype   : orc_play_mode_e  dsk_wkm_get_play_mode( void )
*Arguments   : void
*Return      : the current play mode
*Description : get the current play mode
*Other       :
****************************************************************************************************
*/
extern orc_play_mode_e  dsk_wkm_get_play_mode(void);



#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_WKM_PLAY_MODE_H__

/* end of dsk_wkm_play_mode.h */
