/*
************************************************************************************************************************
*                                                       walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_ab.h
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
#ifndef  __DSK_WKM_AB_H__
#define  __DSK_WKM_AB_H__

#ifdef __cplusplus
extern "C" {
#endif



/*
****************************************************************************************************
*Name        : dsk_wkm_set_ab_a
*Prototype   : __s32  dsk_wkm_set_ab_a( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set the current time point as the A point in AB play mode.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_set_ab_a(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_set_ab_b
*Prototype   : __s32  dsk_wkm_set_ab_b( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set the current time point as the B point in AB play mode and start AB play mode.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_set_ab_b(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_set_ab_loop_count
*Prototype   : __s32  dsk_wkm_set_ab_loop_count( __u32 count )
*Arguments   : count     input. AB loop count.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the loop count in AB play mode.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_set_ab_loop_count(__u32 count);

/*
****************************************************************************************************
*Name        : dsk_wkm_cancle_ab
*Prototype   : __s32  dsk_wkm_cancle_ab( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : cancle AB play.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_cancle_ab(void);



#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_WKM_AB_H__

/* end of dsk_wkm_ab.h */
