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
