/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : mode_desktop_i.h
* By        : Andy.zhang
* Func      : system lib
* Version   : v1.0
* ============================================================================================================
* 2009-8-25 8:53:32  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __MOD_DESKTOP_I_H__
#define __MOD_DESKTOP_I_H__

#include <mod_charset.h>
#include <char_enc.h>
#include <elibs_stdio.h>
#include <mod_willow.h>
#include <mod_audio.h>
#include <mod_cedar.h>
#include <mediafmt.h>
//#include <bsp_display.h>
#include <emodules/mod_display.h>
#include <mod_orchid.h>
#include "lib_ex.h"
#include "mod_orange.h"
#include "mod_orange/orange_api_entry.h"

#include "desktop.h"

#include "include/mini_typedef.h"

//#include "theme\output\desktop_theme.h"
//#define DSKTOP_THEME_PATH BEETLES_APP_ROOT"apps\\desktop_theme.bin"

typedef struct
{
    __u32           mid;
    __u32           init_id;
    __mp           *init_mp;
} __desktop_t;

__s32 DESKTOP_MInit(void);
__s32 DESKTOP_MExit(void);
__mp *DESKTOP_MOpen(__u32 mid, __u32 mode);
__s32 DESKTOP_MClose(__mp *mp);
__u32 DESKTOP_MRead(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 DESKTOP_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
long DESKTOP_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);

#endif  /* __MOD_DESKTOP_I_H__ */
