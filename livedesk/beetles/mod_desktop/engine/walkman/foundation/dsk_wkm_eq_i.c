/*
************************************************************************************************************************
*                                                        walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_eq_i.c
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
#ifndef  __DSK_WKM_EQ_I_C__
#define  __DSK_WKM_EQ_I_C__


#include "foundation_i.h"


/*
****************************************************************************************************
*Name        : __dsk_wkm_set_eq_mode
*Prototype   : __s32  __dsk_wkm_set_eq_mode( __cedar_audio_eq_t  eq_mode )
*Arguments   : eq_mode    input. eq mode.
*Return      : ==0       succeed
*              !=0       fail
*Description : set eq mode, such Jazz, Rock and so on.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_set_eq_mode(__cedar_audio_eq_t  eq_mode)
{
    if (esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_SET_EQ, eq_mode, NULL) == EPDK_OK)
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
*Name        : __dsk_wkm_get_eq_mode
*Prototype   : __cedar_audio_eq_t  __dsk_wkm_get_eq_mode( void )
*Arguments   : void
*Return      : the current eq mode.
*Description : get the current eq mode.
*Other       :
****************************************************************************************************
*/
__cedar_audio_eq_t  __dsk_wkm_get_eq_mode(void)
{
    return (__cedar_audio_eq_t)esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_EQ, 0, NULL);
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_set_eq_value
*Prototype   : __s32 __dsk_wkm_set_eq_value( const __s8 *eq_value_list )
*Arguments   : eq_value_list   input. eq value list. the count of the items in the list should be
*                              USR_EQ_BAND_CNT.
*Return      : ==0   succeed.
*              !=0   fail.
*Description : set eq value
*Other       :
****************************************************************************************************
*/
__s32 __dsk_wkm_set_eq_value(const __s8 *eq_value_list)
{
    if (eq_value_list == NULL)
    {
        return -1;
    }

    if (esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_SET_EQ, CEDAR_AUD_EQ_TYPE_USR_MODE, (void *)eq_value_list) == EPDK_OK)
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
*Name        : __dsk_wkm_get_eq_value
*Prototype   : __s32 __dsk_wkm_get_eq_value( __s8 *eq_value_list )
*Arguments   : eq_value_list   output. a buffer to store eq value. the count of the items in the list
*                              should be USR_EQ_BAND_CNT.
*Return      : ==0   succeed.
*              !=0   fail.
*Description : get eq value
*Other       :
****************************************************************************************************
*/
__s32 __dsk_wkm_get_eq_value(__s8 *eq_value_list)
{
    if (eq_value_list == NULL)
    {
        return -1;
    }

    /*---------------====== to be filled ======-----------------*/
    return 0;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_eq_mode2value
*Prototype   : __s32 __dsk_wkm_eq_mode2value( __cedar_audio_eq_t eq_mode, __s8 *eq_value_list )
*Arguments   : eq_mode         input.
*              eq_value_list   output. a buffer to store eq value. the count of the items in the list
*                              should be USR_EQ_BAND_CNT.
*Return      : ==0   succeed.
*              !=0   fail.
*Description : get the eq value list corresponding to a eq mode.
*Other       :
****************************************************************************************************
*/
__s32 __dsk_wkm_eq_mode2value(__cedar_audio_eq_t eq_mode, __s8 *eq_value_list)
{
    if (eq_value_list == NULL)
    {
        return -1;
    }

    /*---------------====== to be filled ======-----------------*/
    return 0;
}



#endif     //  ifndef __DSK_WKM_EQ_I_C__

/* end of dsk_wkm_eq_i.c */
