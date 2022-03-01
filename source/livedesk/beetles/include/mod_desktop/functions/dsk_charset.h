/*
************************************************************************************************************************
*                                                 desktop charset manage
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_charset.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.08.25
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
* Gary.Wang      2010.08.25        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_CHARSET_H__
#define  __DSK_CHARSET_H__



/*
****************************************************************************************************
*Name        : dsk_get_langres_charset
*Prototype   : __epdk_charset_enm_e  dsk_get_langres_charset( void )
*Arguments   : void
*Return      : the charset of the text of the language resource.
*Description : get the charset of the text of the language resource.
*Other       :
****************************************************************************************************
*/
extern __epdk_charset_enm_e  dsk_get_langres_charset(void);

/*
****************************************************************************************************
*Name        : dsk_set_fs_charset
*Prototype   : __s32 dsk_set_fs_charset( __epdk_charset_enm_e charset )
*Arguments   : charset    input. charset.
*Return      : == EPDK_OK      succeed
*              != EPDK_OK      fail
*Description : set the charset of the file system.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_set_fs_charset(__epdk_charset_enm_e charset);

/*
****************************************************************************************************
*Name        : dsk_get_fs_charset
*Prototype   : __epdk_charset_enm_e  dsk_get_fs_charset( void )
*Arguments   : void
*Return      : the charset of the file system
*Description : get the charset of the file system.
*Other       :
****************************************************************************************************
*/
extern __epdk_charset_enm_e  dsk_get_fs_charset(void);

/*
****************************************************************************************************
*Name        : dsk_set_isn_charset
*Prototype   : __s32 dsk_set_isn_charset( __epdk_charset_enm_e charset )
*Arguments   : charset    input. charset.
*Return      : == EPDK_OK      succeed
*              != EPDK_OK      fail
*Description : set the charset of the isn( internal status number).
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_set_isn_charset(__epdk_charset_enm_e charset);

/*
****************************************************************************************************
*Name        : dsk_get_isn_charset
*Prototype   : __epdk_charset_enm_e  dsk_get_isn_charset( void )
*Arguments   : void
*Return      : the charset of the isn( internal status number)
*Description : get the charset of the isn( internal status number).
*Other       :
****************************************************************************************************
*/
extern __epdk_charset_enm_e  dsk_get_isn_charset(void);



#endif     //  ifndef __DSK_CHARSET_H__

/* end of dsk_charset.h */

