/*
************************************************************************************************************************
*                                                        walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_ab_i.c
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
#ifndef  __DSK_WKM_AB_I_C__
#define  __DSK_WKM_AB_I_C__


#include "foundation_i.h"


/*
****************************************************************************************************
*Name        : __dsk_wkm_set_ab_a
*Prototype   : __s32  __dsk_wkm_set_ab_a( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set the current time point as the A point in AB play mode.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_set_ab_a(void)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_SET_AB_A, 0, NULL) == EPDK_OK)
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
*Name        : __dsk_wkm_set_ab_b
*Prototype   : __s32  __dsk_wkm_set_ab_b( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set the current time point as the B point in AB play mode and start AB play mode.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_set_ab_b(void)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_SET_AB_B, 0, NULL) == EPDK_OK)
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
*Name        : __dsk_wkm_set_ab_loop_count
*Prototype   : __s32  __dsk_wkm_set_ab_loop_count( __u32 count )
*Arguments   : count     input. AB loop count.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the loop count in AB play mode.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_set_ab_loop_count(__u32 count)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_SET_AB_LOOPCNT, count, NULL) == EPDK_OK)
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
*Name        : __dsk_wkm_cancle_ab
*Prototype   : __s32  __dsk_wkm_cancle_ab( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : cancle AB play.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_cancle_ab(void)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_CLEAR_AB, 0, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}



#endif     //  ifndef __DSK_WKM_AB_I_C__

/* end of dsk_wkm_ab_i.c */
