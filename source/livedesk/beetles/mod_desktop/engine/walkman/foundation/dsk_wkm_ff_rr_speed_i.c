/*
************************************************************************************************************************
*                                                        walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_ff_rr_speed_i.c
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
#ifndef  __DSK_WKM_FF_RR_SPEED_I_C__
#define  __DSK_WKM_FF_RR_SPEED_I_C__


#include "dsk_walkman_i.h"


/*
****************************************************************************************************
*Name        : __dsk_wkm_set_ff_rr_speed
*Prototype   : __s32  __dsk_wkm_set_ff_rr_speed( __u32 ff_rr_speed )
*Arguments   : ff_rr_speed   input. the speed of ff or rr. the speed rang is CEDAR_SLOWEST_FRSPEED~~
*                            ~~CEDAR_FASTEST_FRSPEED.
*Return      : ==0     succeed
*              !=0     fail
*Description : set the speed of ff or rr.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_set_ff_rr_speed(__u32 ff_rr_speed)
{
    __s32  ret;
    /* set speed */
    ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_SET_FRSPEED, ff_rr_speed, NULL);

    if (ret == EPDK_OK)
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
*Name        : __dsk_wkm_get_ff_rr_speed
*Prototype   : __s32  __dsk_wkm_get_ff_rr_speed( void )
*Arguments   : void
*Return      : the speed of ff or rr. -1 means error.
*Description : get the speed of ff or rr.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_get_ff_rr_speed(void)
{
    /* get speed */
    return (__s32)esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_FRSPEED, 0, NULL);
}



#endif     //  ifndef __DSK_WKM_FF_RR_SPEED_I_C__

/* end of dsk_wkm_ff_rr_speed_i.c */
