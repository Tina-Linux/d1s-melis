/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_spectrum.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.24
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
* Gary.Wang      2009.09.24       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_spectrum_h
#define  __robin_spectrum_h


/***************************************************************************************************
*Name        : robin_enable_spectrum
*Prototype   : void  robin_enable_spectrum( void )
*Arguments   : void
*Return      : void
*Description : enable parsing spectrum
*Other       :
***************************************************************************************************/
extern void  robin_enable_spectrum(void);

/***************************************************************************************************
*Name        : robin_disable_spectrum
*Prototype   : void  robin_disable_spectrum( void )
*Arguments   : void
*Return      : void
*Description : disable parsing spectrum
*Other       :
***************************************************************************************************/
extern void  robin_disable_spectrum(void);

/***************************************************************************************************
*Name        : robin_get_spectrum_info
*Prototype   : __s32  robin_get_spectrum_info( __u16 *value_list )
*Arguments   : value_list     output. a buffer to store instant spectrum info, whose item's count is
*                             SPECT_GROUP_WIDTH.
*Return      : ==0   succeed.
*              !=0   there is no spectrum info at this moment.
*Description : get instant spectrum info
*Other       :
***************************************************************************************************/
extern __s32 robin_get_spectrum_info(__u16 *value_list);


#endif     //  ifndef __robin_spectrum_h

/* end of robin_spectrum.h */
