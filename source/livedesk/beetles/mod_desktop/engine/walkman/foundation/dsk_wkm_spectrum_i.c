/*
************************************************************************************************************************
*                                                        walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_spectrum_i.c
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
#ifndef  __DSK_WKM_SPECTRUM_I_C__
#define  __DSK_WKM_SPECTRUM_I_C__


#include "foundation_i.h"


/*
****************************************************************************************************
*Name        : __dsk_wkm_enable_spectrum
*Prototype   : __s32  __dsk_wkm_enable_spectrum( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : enable parsing spectrum
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_enable_spectrum(void)
{
    if (esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_SET_SPECTRUM, 1, NULL) == EPDK_OK)
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
*Name        : __dsk_wkm_disable_spectrum
*Prototype   : __s32  __dsk_wkm_disable_spectrum( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : disable parsing spectrum
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_disable_spectrum(void)
{
    if (esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_SET_SPECTRUM, 0, NULL) == EPDK_OK)
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
*Name        : __dsk_wkm_get_spectrum_info
*Prototype   : __s32  __dsk_wkm_get_spectrum_info( __u16 *value_list )
*Arguments   : value_list     output. a buffer to store instant spectrum info, whose item's count is
*                             SPECT_GROUP_WIDTH.
*Return      : ==0   succeed.
*              !=0   there is no spectrum info at this moment.
*Description : get instant spectrum info
*Other       :
****************************************************************************************************
*/
__s32 __dsk_wkm_get_spectrum_info(__u16 *value_list)
{
    __s32  ret;

    if (value_list == NULL)
    {
        return -1;
    }

    ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_SPECTRUM, 0, value_list);

    if (ret == EPDK_OK)
    {
        return 0;
    }
    else                                // no instant spectrum info
    {
        return -1;
    }
}



#endif     //  ifndef __DSK_WKM_SPECTRUM_I_C__

/* end of dsk_wkm_spectrum_i.c */
