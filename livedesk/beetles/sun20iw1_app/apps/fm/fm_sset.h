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
#ifndef __FM_SSET_H__42A0F190_9B2C_4545_80D8_CCAF6BDFB9F1__
#define __FM_SSET_H__42A0F190_9B2C_4545_80D8_CCAF6BDFB9F1__

#include <emodules/mod_fm.h>
#include "beetles_app.h"

//////////////////////////////////////////////////////////////////////////
#define MAX_CHANNEL_FREQ                     FM_SEARCH_CHN_MAX_FREQ
#define MIN_CHANNEL_FREQ                     FM_SEARCH_CHN_MIN_FREQ
#define PER_CHANNEL_FREQ                     1000
#define PER_STEP_CHANNEL_FREQ                100
#define IsInvalidFreq(_v)                    ((_v) < MIN_CHANNEL_FREQ || MAX_CHANNEL_FREQ < (_v))

#define STR_MIN_CHANNEL_FREQ                 "87.0MHz"
#define STR_MAX_CHANNEL_FREQ                 "108.0MHz"

#define STR_CHANNEL_FREQ_FMT                 "%d.%dMHz"
#define MAKE_STR_CHANNEL_FREQ(_p, _f)         \
    app_sprintf(_p, STR_CHANNEL_FREQ_FMT, (_f) / PER_CHANNEL_FREQ, \
                ((_f) / PER_STEP_CHANNEL_FREQ) % (PER_CHANNEL_FREQ / PER_STEP_CHANNEL_FREQ))
//////////////////////////////////////////////////////////////////////////
#define fm_reg_set_channel_(_p, _i, _f)      ((_p)->FM_channel[_i] = _f)
#define fm_reg_get_channel_(_p, _i)          ((_p)->FM_channel[_i])

#define fm_reg_set_sel_channel_id(_p, _i)    ((_p)->channel_id = _i)
#define fm_reg_get_sel_channel_id(_p)        ((_p)->channel_id)

#define fm_reg_set_channel_count(_p, _c)     ((_p)->reserverd[0] = _c)
#define fm_reg_get_channel_count(_p)         ((_p)->reserverd[0])

#define fm_reg_set_audio(_p, _a)             ((_p)->reserverd[1] = _a)
#define fm_reg_get_audio(_p)                 ((_p)->reserverd[1])

#define fm_reg_set_curfreq(_p, _f)           ((_p)->reserverd[2] = _f)
#define fm_reg_get_curfreq(_p)               ((_p)->reserverd[2])
//////////////////////////////////////////////////////////////////////////
#define ID_FM_SCENE_SSET               10

#define ID_MAX_CHANNEL_COUNT 51//，因为增加了退出一项所以   
//extern __sset_item_para_t fm_channel_list[ID_MAX_CHANNEL_COUNT];
//////////////////////////////////////////////////////////////////////////
// main menu id
enum
{
    ID_FM_MMENU_EXIT = 0,
    ID_FM_MMENU_CHANNEL,
    ID_FM_MMENU_MSEARCH,
    ID_FM_MMENU_ASEARCH,
    ID_FM_MMENU_AUDIO,
    ID_FM_MMENU_RECORD,
};

/*notify message of sset*/
typedef enum
{
    FM_SSET_NC_EXIT = 1,
    FM_SSET_NC_CHANNEL,
    FM_SSET_NC_MSEARCH,
    FM_SSET_NC_ASEARCH,
    FM_SSET_NC_AUDIO,
    FM_SSET_NC_RECORD,
    FM_SSET_NC_SEL_CHANNEL,
    //  FM_SSET_NC_SELCHANNEL_END = (FM_SSET_NC_SEL_CHANNEL + ID_MAX_CHANNEL_COUNT),
    FM_SSET_NC_SAVEFREQ,

    FM_SSET_NC_SEL_AUDIO,
    //..FM_SSET_NC_SAVEFREQ,
} __sset_nc_t;
#define FM_SSET_NC_ID_UNKNOW    0xFF

//#define IDV_FM_SSET_NC_SEL_AUDIO

//////////////////////////////////////////////////////////////////////////
__sset_create_para_t *FM_SSET_Build_CreatePara(H_WIN h_win);
#define FM_SSET_Destroy_CreatePara(_p) BFreeType(_p, __sset_create_para_t)

#endif //__FM_SSET_H__42A0F190_9B2C_4545_80D8_CCAF6BDFB9F1__
//End of this file
