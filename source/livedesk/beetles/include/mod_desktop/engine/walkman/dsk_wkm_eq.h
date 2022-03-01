/*
************************************************************************************************************************
*                                                       walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_eq.h
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
#ifndef  __DSK_WKM_EQ_H__
#define  __DSK_WKM_EQ_H__

#ifdef __cplusplus
extern "C" {
#endif



/*
****************************************************************************************************
*Name        : dsk_wkm_set_eq_mode
*Prototype   : __s32  dsk_wkm_set_eq_mode( __cedar_audio_eq_t  eq_mode )
*Arguments   : eq_mode    input. eq mode.
*Return      : ==0       succeed
*              !=0       fail
*Description : set eq mode, such Jazz, Rock and so on.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_set_eq_mode(__cedar_audio_eq_t  eq_mode);

/*
****************************************************************************************************
*Name        : dsk_wkm_get_eq_mode
*Prototype   : __cedar_audio_eq_t  dsk_wkm_get_eq_mode( void )
*Arguments   : void
*Return      : the current eq mode.
*Description : get the current eq mode.
*Other       :
****************************************************************************************************
*/
extern __cedar_audio_eq_t  dsk_wkm_get_eq_mode(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_set_eq_value
*Prototype   : __s32 dsk_wkm_set_eq_value( const __s8 *eq_value_list )
*Arguments   : eq_value_list   input. eq value list. the count of the items in the list should be
*                              USR_EQ_BAND_CNT.
*Return      : ==0   succeed.
*              !=0   fail.
*Description : set eq value
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_wkm_set_eq_value(const __s8 *eq_value_list);

/*
****************************************************************************************************
*Name        : dsk_wkm_get_eq_value
*Prototype   : __s32 dsk_wkm_get_eq_value( __s8 *eq_value_list )
*Arguments   : eq_value_list   output. a buffer to store eq value. the count of the items in the list
*                              should be USR_EQ_BAND_CNT.
*Return      : ==0   succeed.
*              !=0   fail.
*Description : get eq value
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_wkm_get_eq_value(__s8 *eq_value_list);

/*
****************************************************************************************************
*Name        : dsk_wkm_eq_mode2value
*Prototype   : __s32 dsk_wkm_eq_mode2value( __cedar_audio_eq_t eq_mode, __s8 *eq_value_list )
*Arguments   : eq_mode         input.
*              eq_value_list   output. a buffer to store eq value. the count of the items in the list
*                              should be USR_EQ_BAND_CNT.
*Return      : ==0   succeed.
*              !=0   fail.
*Description : get the eq value list corresponding to a eq mode.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_wkm_eq_mode2value(__cedar_audio_eq_t eq_mode, __s8 *eq_value_list);



#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_WKM_EQ_H__

/* end of dsk_wkm_eq.h */
