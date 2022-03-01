/*
************************************************************************************************************************
*                                                   language resource
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_langres.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.29
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
* Gary.Wang      2010.10.29        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_LANGRES_H__
#define  __DSK_LANGRES_H__


/*
****************************************************************************************************
*Name        : dsk_langres_init
*Prototype   : __s32 dsk_langres_init( const char *file )
*Arguments   : file    input. the language resouce file.
*Return      : EPDK_OK      succeed
*              EPDK_FAIL    fail
*Description : init language resource module.
*Other       :
****************************************************************************************************
*/
__s32 dsk_langres_init(const char *file);

/*
****************************************************************************************************
*Name        : dsk_langres_exit
*Prototype   : void dsk_langres_exit( void )
*Arguments   : void
*Return      : void
*Description : exit language resource module.
*Other       :
****************************************************************************************************
*/
void  dsk_langres_exit(void);

/*
****************************************************************************************************
*Name        : dsk_langres_set_type
*Prototype   : __s32 dsk_langres_set_type( __u32 language_id )
*Arguments   : language_id    input. language type id.
*Return      : EPDK_OK      succeed
*              EPDK_FAIL    fail
*Description : set language type.
*Other       :
****************************************************************************************************
*/
__s32 dsk_langres_set_type(__u32 language_id);

/*
****************************************************************************************************
*Name        : dsk_langres_get_type
*Prototype   : __s32 dsk_langres_get_type( void )
*Arguments   : void
*Return      : the current language type id.
*Description : get the current language type.
*Other       :
****************************************************************************************************
*/
__s32 dsk_langres_get_type(void);

/*
****************************************************************************************************
*Name        : dsk_langres_get_menu_len
*Prototype   : __s32 dsk_langres_get_menu_len( __u32 text_id )
*Arguments   : text_id   input. the ID of a menu text
*Return      : the length of the menu text string. return value <= 0, means error.
*Description : get the length of the menu text string.
*Other       :
****************************************************************************************************
*/
__s32 dsk_langres_get_menu_len(__u32 text_id);

/*
****************************************************************************************************
*Name        : dsk_langres_get_menu_text
*Prototype   : __s32  dsk_langres_get_menu_text( __u16 text_id, void *buf, __u32 size )
*Arguments   : text_id        input. the ID of a menu text
*              buf            output. the buffer to store the menu text
*              size           input. the size of the buffer
*Return      : EPDK_OK      succeed
*              EPDK_FAIL    fail
*Description : get the content of a menu text according to its ID. the content is a string with '\0'
*              ending.
*Other       :
****************************************************************************************************
*/
__s32 dsk_langres_get_menu_text(__u32 text_id, void *buf, __u32 size);



#endif     //  ifndef __DSK_LANGRES_H__

/* end of dsk_lang.h */
