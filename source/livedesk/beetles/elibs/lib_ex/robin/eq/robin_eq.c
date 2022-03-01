/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_eq.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.25
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
* Gary.Wang      2009.09.25       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_eq_c
#define  __robin_eq_c


#include "robin_i.h"
#include <kconfig.h>


/***************************************************************************************************
*Name        : robin_set_eq_mode
*Prototype   : __s32  robin_set_eq_mode( __cedar_audio_eq_t  eq_mode )
*Arguments   : eq_mode    input. eq mode.
*Return      : ==0       succeed
*              !=0       fail
*Description : set eq mode, such Jazz, Rock and so on.
*Other       :
***************************************************************************************************/
__s32  robin_set_eq_mode(__cedar_audio_eq_t  eq_mode)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_EQ, eq_mode, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}



/***************************************************************************************************
*Name        : robin_get_eq_mode
*Prototype   : __cedar_audio_eq_t  robin_get_eq_mode( void )
*Arguments   : void
*Return      : the current eq mode.
*Description : get the current eq mode.
*Other       :
***************************************************************************************************/
__cedar_audio_eq_t  robin_get_eq_mode(void)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return CEDAR_AUD_EQ_TYPE_;
    }

    return (__cedar_audio_eq_t)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_EQ, 0, NULL);
}



/***************************************************************************************************
*Name        : robin_set_eq_value
*Prototype   : __s32 robin_set_eq_value( const __s8 *eq_value_list )
*Arguments   : eq_value_list   input. eq value list. the count of the items in the list should be
*                              USR_EQ_BAND_CNT.
*Return      : ==0   succeed.
*              !=0   fail.
*Description : set eq value
*Other       :
***************************************************************************************************/
__s32 robin_set_eq_value(const __s8 *eq_value_list)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }


    if (eq_value_list == NULL)
    {
        return -1;
    }

    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_EQ, CEDAR_AUD_EQ_TYPE_USR_MODE, (void *)eq_value_list) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}



/***************************************************************************************************
*Name        : robin_get_eq_value
*Prototype   : __s32 robin_get_eq_value( __s8 *eq_value_list )
*Arguments   : eq_value_list   output. a buffer to store eq value. the count of the items in the list
*                              should be USR_EQ_BAND_CNT.
*Return      : ==0   succeed.
*              !=0   fail.
*Description : get eq value
*Other       :
***************************************************************************************************/
__s32 robin_get_eq_value(__s8 *eq_value_list)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    if (eq_value_list == NULL)
    {
        return -1;
    }

    /*---------------====== to be filled ======-----------------*/
    return 0;
}



/***************************************************************************************************
*Name        : robin_eq_mode2value
*Prototype   : __s32 robin_eq_mode2value( __cedar_audio_eq_t eq_mode, __s8 *eq_value_list )
*Arguments   : eq_mode         input.
*              eq_value_list   output. a buffer to store eq value. the count of the items in the list
*                              should be USR_EQ_BAND_CNT.
*Return      : ==0   succeed.
*              !=0   fail.
*Description : get the eq value list corresponding to a eq mode.
*Other       :
***************************************************************************************************/
__s32 robin_eq_mode2value(__cedar_audio_eq_t eq_mode, __s8 *eq_value_list)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    if (eq_value_list == NULL)
    {
        return -1;
    }

    /*---------------====== to be filled ======-----------------*/
    return 0;
}



#endif     //  ifndef __robin_eq_c

/* end of robin_eq.c */
