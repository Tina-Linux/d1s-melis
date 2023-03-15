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
