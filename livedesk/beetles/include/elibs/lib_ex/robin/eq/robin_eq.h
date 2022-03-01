/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_eq.h
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
#ifndef  __robin_eq_h
#define  __robin_eq_h

#include <emodules/mod_cedar.h>

/***************************************************************************************************
*Name        : robin_set_eq_mode
*Prototype   : __s32  robin_set_eq_mode( __cedar_audio_eq_t  eq_enm )
*Arguments   : eq_enm    input. eq mode.
*Return      : ==0       succeed
*              !=0       fail
*Description : set eq mode, such Jazz, Rock and so on.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_eq_mode(__cedar_audio_eq_t  eq_enm);

/***************************************************************************************************
*Name        : robin_get_eq_mode
*Prototype   : __cedar_audio_eq_t  robin_get_eq_mode( void )
*Arguments   : void
*Return      : the current eq mode.
*Description : get the current eq mode.
*Other       :
***************************************************************************************************/
extern __cedar_audio_eq_t  robin_get_eq_mode(void);

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
extern __s32 robin_set_eq_value(const __s8 *eq_value_list);

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
extern __s32 robin_get_eq_value(__s8 *eq_value_list);

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
extern __s32 robin_eq_mode2value(__cedar_audio_eq_t eq_mode, __s8 *eq_value_list);



#endif     //  ifndef __robin_eq_h

/* end of robin_eq.h */
