/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
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
