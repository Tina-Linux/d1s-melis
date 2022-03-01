/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lion
* File    : apps.h
*
* By      : andy.zhang
* Version : v1.0
* Date    : 2008-10-4 11:28:56
**********************************************************************************************************************
*/
#ifndef __APPS_H__
#define __APPS_H__

#include <kconfig.h>

//#define Have_eJTTS_lib      1
//#define Have_TscRec_lib     1


#define BEETLES_APP_ROOT            "d:\\"//"f:\\rootfs\\"////程序和资源的所在盘符.如果非调试，则定义为"d:\\"
#define BEETLES_RTC_EXIST           0 // 1
#define BEETLES_BLACK_COLOR         0x99000000

#define BEETLES_SUPPORT_KARAOKE
#undef  BEETLES_SUPPORT_KARAOKE

#define BEETLES_GRAY_COLOR          0x99A0A0A0

#define BEETLES_FONT_SIZE_STR       "16"
#define BEETLES_FONT_SIZE_INT       16
#define KARAOKE_SW                  0
#define ALARM_SW                    1
#define GAME_SW                     0

#define BEETLES_GAME_FLAG           0
#define AW_SDK_PMU                  0

//#include "epdk.h"
#include <stdio.h>
//#include <aw_dirent.h>
#include <libc.h>
#include <emodules/mod_defs.h>
#include <drivers/sys_time.h>
#include <emodules/mod_orchid.h>
#include <emodules/mod_charset.h>
#include <emodules/mod_willow.h>
#include <emodules/mod_orange.h>
#include <emodules/mod_cedar.h>
#include <emodules/mod_audio.h>
#include <emodules/mod_orange/orange_api_entry.h>
//#include "mod_flash.h"
#include "applets/app_load_para.h"
#include "applets/tv_visible_area.h"

#include "elibs/lib_ex.h"

#include "mod_desktop/desktop.h"

#include "init/init.h"

#include "libapps/libapps_i.h"

#include "res/lang.h"
#include "res/theme.h"

#include "res/karaoke_theme.h"

#endif
