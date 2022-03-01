/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_play_mode.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.28
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
* Gary.Wang      2009.09.28       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_play_mode_h
#define  __robin_play_mode_h


/***************************************************************************************************
*Name        : robin_set_play_mode
*Prototype   : __s32  robin_set_play_mode( orc_play_mode_e  play_mode )
*Arguments   : play_mode   input. play mode are defined in drv_orchid.h, see orc_play_mode_e.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the play mode.
*Other       :
********************************************************************************************
*******/
extern __s32  robin_set_play_mode(rat_play_mode_e play_mode);

/***************************************************************************************************
*Name        : robin_get_play_mode
*Prototype   : orc_play_mode_e  robin_get_play_mode( void )
*Arguments   : void
*Return      : the current play mode
*Description : get the current play mode
*Other       :
***************************************************************************************************/
extern rat_play_mode_e  robin_get_play_mode(void);



#endif     //  ifndef __robin_play_mode_h

/* end of robin_play_mode.h */
