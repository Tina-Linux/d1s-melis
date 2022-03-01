/*
************************************************************************************************************************
*                                                        walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_ff_rr_speed.c
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
#ifndef  __DSK_WKM_FF_RR_SPEED_C__
#define  __DSK_WKM_FF_RR_SPEED_C__


#include "dsk_walkman_i.h"


/*
****************************************************************************************************
*Name        : dsk_wkm_set_ff_rr_speed
*Prototype   : __s32  dsk_wkm_set_ff_rr_speed( __u32 ff_rr_speed )
*Arguments   : ff_rr_speed   input. the speed of ff or rr. the speed rang is CEDAR_SLOWEST_FRSPEED~~
*                            ~~CEDAR_FASTEST_FRSPEED.
*Return      : ==0     succeed
*              !=0     fail
*Description : set the speed of ff or rr.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_set_ff_rr_speed(__u32 ff_rr_speed)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    if (__dsk_wkm_get_playing_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT playing.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    /* set speed */
    ret = __dsk_wkm_set_ff_rr_speed(ff_rr_speed);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_get_ff_rr_speed
*Prototype   : __s32  dsk_wkm_get_ff_rr_speed( void )
*Arguments   : void
*Return      : the speed of ff or rr. -1 means error.
*Description : get the speed of ff or rr.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_get_ff_rr_speed(void)
{
    __s32  speed;
    __u8   err;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    if (__dsk_wkm_get_playing_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT playing.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    /* get speed */
    speed = __dsk_wkm_get_ff_rr_speed();
    g_post_mutex(dsk_wkm_res_mutex);
    return speed;
}



#endif     //  ifndef __DSK_WKM_FF_RR_SPEED_C__

/* end of dsk_wkm_ff_rr_speed.c */
