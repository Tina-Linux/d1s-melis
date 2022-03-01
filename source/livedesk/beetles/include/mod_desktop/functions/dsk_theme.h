/*
************************************************************************************************************************
*                                                    theme resource
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_theme.h
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
#ifndef  __DSK_THEME_H__
#define  __DSK_THEME_H__
#include <kconfig.h>

typedef unsigned long  HTHEME;


/*
****************************************************************************************************
*Name        : dsk_theme_init
*Prototype   : __s32 dsk_theme_init( const char *file )
*Arguments   : file    input. the theme resource file.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : init theme resource module.
*Other       :
****************************************************************************************************
*/
__s32 dsk_theme_init(const char *file);

__s32 dsk_theme_init_ex(const char *file);

/*
****************************************************************************************************
*Name        : dsk_theme_exit
*Prototype   : void dsk_theme_exit( void )
*Arguments   : void
*Return      : void
*Description : exit theme resource module.
*Other       :
****************************************************************************************************
*/
void  dsk_theme_exit(void);

void  dsk_theme_exit_ex(void);

/*
****************************************************************************************************
*Name        : dsk_theme_set_style
*Prototype   : __s32 dsk_theme_set_style( __u32 style_id )
*Arguments   : style_id    input. theme style id.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : set theme style.
*Other       :
****************************************************************************************************
*/
__s32 dsk_theme_set_style(__u32 style_id);

/*
****************************************************************************************************
*Name        : dsk_theme_get_style
*Prototype   : __s32 dsk_theme_get_style( void )
*Arguments   : void
*Return      : the current theme style id.
*Description : get the current theme style.
*Other       :
****************************************************************************************************
*/
__s32 dsk_theme_get_style(void);

/*
****************************************************************************************************
*Name        : dsk_theme_open
*Prototype   : HTHEME dsk_theme_open( __u32 theme_id )
*Arguments   : theme_id   input. the ID of a theme
*Return      : the handle of a theme. NULL means error.
*Description : open a handle of a theme.
*Other       :
****************************************************************************************************
*/
HTHEME dsk_theme_open(__u32 theme_id);

HTHEME dsk_theme_open_ex(__u32 theme_id);

/*
****************************************************************************************************
*Name        : dsk_theme_close
*Prototype   : void  dsk_theme_close( HTHEME handle )
*Arguments   : a handle of a theme
*Return      : void
*Description : close a handle of a theme.
*Other       :
****************************************************************************************************
*/
void  dsk_theme_close(HTHEME handle);
void  dsk_theme_close_ex(HTHEME handle);
void  dsk_theme_really_close_ex(HTHEME handle);
void  dsk_theme_really_close_by_id_ex(__u32 theme_id);


/*
****************************************************************************************************
*Name        : dsk_theme_hdl2size
*Prototype   : __s32 dsk_theme_hdl2size( HTHEME handle )
*Arguments   : a handle of a theme
*Return      : the size of a theme. return value <= 0, means error.
*Description : get the size of a theme.
*Other       :
****************************************************************************************************
*/
__s32 dsk_theme_hdl2size(HTHEME handle);

__s32 dsk_theme_hdl2size_ex(HTHEME handle);


/*
****************************************************************************************************
*Name        : dsk_theme_hdl2buf
*Prototype   : void *dsk_theme_hdl2buf( HTHEME handle )
*Arguments   : a handle of a theme
*Return      : the pointer to the buffer of a theme. NULL means error.
*Description : get the pointer to the buffer of a theme.
*Other       :
****************************************************************************************************
*/
void *dsk_theme_hdl2buf(HTHEME handle);

void *dsk_theme_hdl2buf_ex(HTHEME handle);

#endif     //  ifndef __DSK_THEME_H__

/* end of dsk_theme.h */
