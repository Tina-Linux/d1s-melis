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
#ifndef  _FM_TEA5767_H_
#define  _FM_TEA5767_H_

#include  "drv_fm_i.h"

#if(FM_MODULE_TYPE == FM_MODULE_TYPE_TEA5767)

#define  FM_MAX_FREQ               108000
#define  FM_MIN_FREQ                87000

typedef struct __FM_SETTING_INFO
{
    __s32   min_freq;
    __s32   max_freq;
    __s8    area_select;
    __s8    stereo_select;
    __s8    signal_level;
    __s8    reserved;

} __fm_setting_info_t;

extern   __s32  fm_init(void);
extern   __s32  fm_exit(void);
extern   __s32  fm_play(__s32 freq);
extern   __s32  fm_auto_search(__s32 freq, __u32 search_dir);
extern   __s32  fm_manual_search(__s32 freq, __u32 search_dir);
extern   __s32  fm_area_choose(__s32 area, void *pbuffer);
extern   __s32  fm_stereo_choose(__s32 audio_method);
extern   __s32  fm_mute(__s32 voice_onoff);
extern   __s32  fm_signal_level(__s32 signal_level);

extern   __s32  fm_send_on(void);
extern   __s32  fm_send(__s32 freq);
extern   __s32  fm_send_off(void);
extern   __s32  fm_pa_gain(__u8 pagain);

extern   __s32  fm_get_status(void);

#endif


#endif /*_FM_TEA5767_H_*/
