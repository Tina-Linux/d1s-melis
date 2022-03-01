/*
************************************************************************************************************************
*                                                     display gamma
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_display_gamma.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.09.02
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
* Gary.Wang      2010.09.02        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_DISPLAY_GAMMA_H__
#define  __DSK_DISPLAY_GAMMA_H__

#ifdef __cplusplus
extern "C" {
#endif


/*
****************************************************************************************************
*Name        : dsk_set_gamma
*Prototype   : __s32 dsk_set_gamma( __u32 gamma )
*Arguments   : gamma       input. gamma value.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : adjust the gamma of the display.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_set_gamma(__u32 gamma);

/*
****************************************************************************************************
*Name        : dsk_gamma_on
*Prototype   : __s32 dsk_gamma_on(void)
*Arguments   : void
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : enable the adjustment of the gamma.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_gamma_on(void);

/*
****************************************************************************************************
*Name        : dsk_gamma_off
*Prototype   : __s32 dsk_gamma_off(void)
*Arguments   : void
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : disable the adjustment of the gamma.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_gamma_off(void);



#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_DISPLAY_GAMMA_H__

/* end of dsk_display_gamma.h */
