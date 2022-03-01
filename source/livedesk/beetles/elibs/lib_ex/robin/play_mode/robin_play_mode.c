/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_play_mode.c
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
#ifndef  __robin_play_mode_c
#define  __robin_play_mode_c


#include "robin_i.h"
#include <kconfig.h>


/***************************************************************************************************
*Name        : robin_set_play_mode
*Prototype   : __s32  robin_set_play_mode( orc_play_mode_e  play_mode )
*Arguments   : play_mode   input. play mode are defined in drv_orchid.h, see orc_play_mode_e.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the play mode.
*Other       :
***************************************************************************************************/
__s32  robin_set_play_mode(rat_play_mode_e   play_mode)
{
    if (EPDK_FALSE == robin_is_open())
    {
        __log("robin is not open ...\n");
        return EPDK_FAIL;
    }

    return rat_npl_set_play_mode(rat_hnpl, play_mode);
}



/***************************************************************************************************
*Name        : robin_get_play_mode
*Prototype   : orc_play_mode_e  robin_get_play_mode( void )
*Arguments   : void
*Return      : the current play mode
*Description : get the current play mode
*Other       :
***************************************************************************************************/
rat_play_mode_e  robin_get_play_mode(void)
{

    if (EPDK_FALSE == robin_is_open())
    {
        return 0;
    }

    return rat_npl_get_play_mode(rat_hnpl);
}


#endif     //  ifndef __robin_play_mode_c
