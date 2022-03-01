/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_ab.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.27
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
* Gary.Wang      2009.09.27       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_ab_c
#define  __robin_ab_c


#include "robin_i.h"
#include <kconfig.h>


/***************************************************************************************************
*Name        : robin_set_ab_a
*Prototype   : __s32  robin_set_ab_a( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set the current time point as the A point in AB play mode.
*Other       :
***************************************************************************************************/
__s32  robin_set_ab_a(void)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_AB_A, 0, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

__s32  robin_set_ab_a_v2(void)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_AB_A_V2, -1, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/***************************************************************************************************
*Name        : robin_set_ab_b
*Prototype   : __s32  robin_set_ab_b( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set the current time point as the B point in AB play mode and start AB play mode.
*Other       :
***************************************************************************************************/
__s32  robin_set_ab_b(void)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_AB_B, 0, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

__s32  robin_set_ab_b_v2(void)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_AB_B_V2, -1, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}


/***************************************************************************************************
*Name        : robin_set_ab_loop_count
*Prototype   : __s32  robin_set_ab_loop_count( __u32 count )
*Arguments   : count     input. AB loop count.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the loop count in AB play mode.
*Other       :
***************************************************************************************************/
__s32  robin_set_ab_loop_count(__u32 count)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_AB_LOOPCNT, count, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

__s32  robin_set_ab_loop_count_v2(__u32 count)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    __u32 ret;

    if ((ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_AB_LOOPCNT_V2, count, NULL)) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/***************************************************************************************************
*Name        : robin_cancle_ab
*Prototype   : __s32  robin_cancle_ab( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : cancle AB play.
*Other       :
***************************************************************************************************/
__s32  robin_cancle_ab(void)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_CLEAR_AB, 0, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

__s32  robin_cancle_ab_v2(void)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_CLEAR_AB_V2, 0, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/***************************************************************************************************
*Name        : robin_cancle_ab
*Prototype   : __s32  robin_cancle_ab( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : cancle AB play.
*Other       :
***************************************************************************************************/
__s32  robin_enable_ab(void)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    __u32 ret;

    if ((ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_ENABLE_AB_V2, 0, NULL)) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}



__s32  robin_set_audio_ab_mode(__u32 mode)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_AUDIO_AB_MODE_V2, mode, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

#endif     //  ifndef __robin_ab_c
