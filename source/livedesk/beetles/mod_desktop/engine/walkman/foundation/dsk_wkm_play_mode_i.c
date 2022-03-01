/*
************************************************************************************************************************
*                                                        walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_play_mode_i.c
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
#ifndef  __DSK_WKM_PLAY_MODE_I_C__
#define  __DSK_WKM_PLAY_MODE_I_C__


#include "dsk_walkman_i.h"


/*
****************************************************************************************************
*Name        : __dsk_wkm_set_play_mode
*Prototype   : __s32  __dsk_wkm_set_play_mode( orc_play_mode_e  play_mode )
*Arguments   : play_mode   input. play mode are defined in drv_orchid.h, see orc_play_mode_e.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the play mode.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_set_play_mode(orc_play_mode_e  play_mode)
{
    __s32  ret;
    ret = g_fioctrl(dsk_wkm_horc, ORC_CMD_SET_PLAY_MODE, (__s32)dsk_wkm_hnpl, (void *)play_mode);

    if (ret == ORC_OK)
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
*Name        : __dsk_wkm_get_play_mode
*Prototype   : orc_play_mode_e  __dsk_wkm_get_play_mode( void )
*Arguments   : void
*Return      : the current play mode
*Description : get the current play mode
*Other       :
****************************************************************************************************
*/
orc_play_mode_e  __dsk_wkm_get_play_mode(void)
{
    __s32  play_mode;
    g_fioctrl(dsk_wkm_horc, ORC_CMD_GET_PLAY_MODE, (__s32)dsk_wkm_hnpl, (void *)&play_mode);
    return (orc_play_mode_e)play_mode;
}



#endif     //  ifndef __DSK_WKM_PLAY_MODE_I_C__

/* end of dsk_wkm_play_mode_i.c */
