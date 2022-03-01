/*
************************************************************************************************************************
*                                                 desktop charset manage
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_charset.c
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
#ifndef  __DSK_CHARSET_C__
#define  __DSK_CHARSET_C__


#include "functions_i.h"


static __epdk_charset_enm_e fs_charset  = EPDK_CHARSET_ENM_UNKNOWN;
static __epdk_charset_enm_e isn_charset = EPDK_CHARSET_ENM_UNKNOWN;


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
__epdk_charset_enm_e  dsk_get_langres_charset(void)
{
    return EPDK_CHARSET_ENM_UTF8;
}


/*
****************************************************************************************************
*Name        : dsk_set_fs_charset
*Prototype   : int32_t dsk_set_fs_charset( __epdk_charset_enm_e charset )
*Arguments   : charset    input. charset.
*Return      : == EPDK_OK      succeed
*              != EPDK_OK      fail
*Description : set the charset of the file system.
*Other       :
****************************************************************************************************
*/
int32_t dsk_set_fs_charset(__epdk_charset_enm_e charset)
{
    eLIBs_setfs(0, 0, charset, NULL);

    fs_charset = charset;
    return EPDK_OK;
}


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
__epdk_charset_enm_e  dsk_get_fs_charset(void)
{
    eLIBs_GetFSCharset("d:\\", (int32_t *)(&fs_charset));
    return fs_charset;
}


/*
****************************************************************************************************
*Name        : dsk_set_isn_charset
*Prototype   : int32_t dsk_set_isn_charset( __epdk_charset_enm_e charset )
*Arguments   : charset    input. charset.
*Return      : == EPDK_OK      succeed
*              != EPDK_OK      fail
*Description : set the charset of the isn( internal status number).
*Other       :
****************************************************************************************************
*/
int32_t dsk_set_isn_charset(__epdk_charset_enm_e charset)
{
    isn_charset = charset;
    return EPDK_OK;
}


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
__epdk_charset_enm_e  dsk_get_isn_charset(void)
{
    return isn_charset;
}

#endif     //  ifndef __DSK_CHARSET_C__
