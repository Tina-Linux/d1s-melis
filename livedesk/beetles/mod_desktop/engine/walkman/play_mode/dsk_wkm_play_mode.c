/*
************************************************************************************************************************
*                                                        walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_play_mode.c
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
#ifndef  __DSK_WKM_PLAY_MODE_C__
#define  __DSK_WKM_PLAY_MODE_C__


#include "dsk_walkman_i.h"


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
__s32  dsk_wkm_set_play_mode(orc_play_mode_e  play_mode)
{
    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    if (__dsk_wkm_npl_is_dirty())
    {
        __dsk_wkm_set_npl_dirty_flag(EPDK_TRUE);
        __dsk_wkm_npl_clear_dirty_flag();
    }

    return __dsk_wkm_set_play_mode(play_mode);
}



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
orc_play_mode_e  dsk_wkm_get_play_mode(void)
{
    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return ORC_PLAY_MODE_UNKNOWN;
    }

    if (__dsk_wkm_npl_is_dirty())
    {
        __dsk_wkm_npl_clear_dirty_flag();
    }

    return __dsk_wkm_get_play_mode();
}



#endif     //  ifndef __DSK_WKM_PLAY_MODE_C__

/* end of dsk_wkm_play_mode.c */
