/*
************************************************************************************************************************
*                                                        walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_play_speed_i.c
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
#ifndef  __DSK_WKM_PLAY_SPEED_I_C__
#define  __DSK_WKM_PLAY_SPEED_I_C__


#include "foundation_i.h"


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
__s32  __dsk_wkm_set_play_speed(__s32 play_speed)
{
    if (esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_SET_VPS, play_speed, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}



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
__s32  __dsk_wkm_get_play_speed(void)
{
    return esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_VPS, 0, NULL);
}



#endif     //  ifndef __DSK_WKM_PLAY_SPEED_I_C__

/* end of dsk_wkm_play_speed_i.c */
