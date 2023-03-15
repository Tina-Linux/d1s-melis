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
#ifndef __DSK_RADIO_H__
#define __DSK_RADIO_H__


#include <emodules/mod_defs.h>
#include "radio/dsk_radio_receive.h"
#include "radio/dsk_radio_send.h"
#include <kconfig.h>


#define MAX_CHANNEL_NUM                 50

typedef struct
{
    __u32 valid_num;
    dsk_radio_chan_t freq_list[MAX_CHANNEL_NUM];
} dsk_radio_ch_list_t;
/*!
  * \brief
  *     dsk_radio波段类型枚举
*/
typedef enum
{
    DSK_RADIO_BAND_JAPAN                = 0x01,
    DSK_RADIO_BAND_SCHOOL,
    DSK_RADIO_BAND_US_EUP
} dsk_radio_band_mode_t;

/*!
  * \brief
  *     DSK_RADIO信号强度门限值枚举
*/
typedef enum
{
    DSK_RADIO_THRESHOLD_HIGH            = 0x01,
    DSK_RADIO_THRESHOLD_MID,
    DSK_RADIO_THRESHOLD_LOW
} dsk_radio_threshold_mode_t;


/*!
  * \brief
  *     dsk_radio搜索方向
*/
typedef enum
{
    DSK_RADIO_REC_SEARCH_UP             = 0x01,
    DSK_RADIO_REC_SEARCH_DOWN
} dsk_radio_rec_auto_search_mode_t;

typedef enum
{
    DSK_RADIO_SEARCH_AUTO               = 0x01,
    DSK_RADIO_SEARCH_MANUAL
} dsk_radio_search_mode_t;

/*!
  * \brief
  *     dsk_radio播放的声道模式
*/
typedef enum
{
    DSK_RADIO_AUDIO_MONO                = 0x01,
    DSK_RADIO_AUDIO_STEREO,
    DSK_RADIO_AUDIO_EXIT
} dsk_radio_audio_mode_t;

typedef enum
{
    DSK_RADIO_EVENT_SEARCH_SUCCESS      = 0,
    DSK_RADIO_EVENT_SEARCH_FAIL         = 1,
    DSK_RADIO_EVENT_SEARCH_OVER         = 2,
    DSK_RADIO_EVENT_SEARCH_FREQ         = 3,    //显示内部搜索过程中的频率
    DSK_RADIO_EVENT_UNKNOWN             = -1
} dsk_radio_event_e;

/*!
  * \brief
  *     dsk_radio向外提供的功能命令
*/
typedef enum
{

    /*DSK_RADIO 发射接收公共接口-----------------------------------------------------------*/


    /*! 静音 param1=0,param2=0*/
    DSK_RADIO_MUTE                      = 0x01,
    /*! 取消静音 param1=0,param2=0*/
    DSK_RADIO_UNMUTE,
    /*! 设置波段 param1=dsk_radio_band_mode_t,param2=0*/
    DSK_RADIO_SET_BAND,
    /*! 设置信号强度门限值 param1=dsk_radio_threshold_mode_t,param2=0*/
    DSK_RADIO_SET_HTRESHOLD,
    /*! 设置声道模式 param1=dsk_radio_audio_mode_t,param2=0*/
    DSK_RADIO_SET_AUDIO_MODE,

    /*! 获取当前波段 param1=0,param2=(int)&band*/
    DSK_RADIO_GET_BAND,
    /*! 获取当前频率，返回值以KHZ为单位 param1=0,param2=(int)&freq*/
    DSK_RADIO_GET_FREQ,
    /*! 获取声道模式  param1=0,param2=(int)&audio_mode*/
    DSK_RADIO_GET_AUDIO_MODE,
    /*! 获取当前波段最小频率 param1=0,param2=(int)&min_freq*/
    DSK_RADIO_GET_MIN_FREQ,
    /*! 获取当前波段最大频率 param1=0,param2=(int)&max_freq*/
    DSK_RADIO_GET_MAX_FREQ,


    /*DSK_RADIO 发射接收公共接口-----------------------------------------------------------*/

    /*DSK_RADIO 接收部分------------------------------------------------------------------*/

    DSK_RADIO_REC_OPEN,

    /*搜索功能部分*/
    DSK_RADIO_REC_SET_AUTO_SEARCH_MODE,
    DSK_RADIO_REC_AUTO_SEARCH,
    DSK_RADIO_REC_AUTO_SEARCH_ALL,

    /*播放功能部分*/
    DSK_RADIO_REC_SET_FREQ_PLAY,
    DSK_RADIO_REC_NEXT_FREQ_PLAY,
    DSK_RADIO_REC_PRE_FREQ_PLAY,

    /*列表管理部分*/
    DSK_RADIO_REC_SET_PLAY_LIST_TYPE,
    DSK_RADIO_REC_GET_PLAY_LIST_TYPE,

    DSK_RADIO_REC_GET_SEARCH_RESULT_VN,
    DSK_RADIO_REC_GET_FAVORITE_VN,
    DSK_RADIO_REC_GET_SEARCH_RESULT_CHAN,
    DSK_RADIO_REC_GET_FAVORITE_CHAN,

    DSK_RADIO_REC_GET_FAVORITE,
    DSK_RADIO_REC_SAVE_FAVORITE,
    DSK_RADIO_REC_GET_SEARCH_RESULT,
    DSK_RADIO_REC_SAVE_SEARCH_RESULT,

    DSK_RADIO_REC_ADD_CHAN_TO_FAVORITE,
    DSK_RADIO_REC_DEL_CHAN_FROM_FAVORITE,

    DSK_RADIO_REC_CLOSE,

    /*DSK_RADIO 接收部分------------------------------------------------------------------*/

    /*DSK_RADIO 发射部分------------------------------------------------------------------*/


    /*! 打开DSK_RADIO发射功能 param1=0,param2=0*/
    DSK_RADIO_SEND_OPEN,
    /*! 设置发射的频率进行播放 param1=freq,param2=0*/
    DSK_RADIO_SEND_SET_FREQ_PLAY,
    /*! 设置发射的增益（即发射声音大小） param1=(__u32)gain,param2=0*/
    DSK_RADIO_SEND_SET_GAIN,
    /*! 获取发射的增益（即发射声音大小） param1=0,param2=(__u32)&gain*/
    DSK_RADIO_SEND_GET_GAIN,
    /*! 关闭DSK_RADIO发射功能 param1=0,param2=0*/
    DSK_RADIO_SEND_CLOSE

    /*DSK_RADIO 发射部分------------------------------------------------------------------*/
} dsk_radio_cmd_t;

/*
**********************************************************************************************************************
*                                               dsk_radio_open
*
* author:        terry.zeng
*
* date:             2009-12-01
*
* Description:      dsk_radio 打开函数(初始化FM播放功能)
*
* parameters:
*
* return:           if success return dsk_radio handle
*                   if fail return NULL
* modify history:
**********************************************************************************************************************
*/
__s32 dsk_radio_open(void);

__bool dsk_radio_is_open(void);

__pCBK_t dsk_radio_set_cb(dsk_radio_event_e event, __pCBK_t cb, void *ctx);

__pCBK_t dsk_radio_get_cb(dsk_radio_event_e event);

__s32 dsk_radio_mute(void);

__s32 dsk_radio_unmute(void);

__s32 dsk_radio_set_band(dsk_radio_band_mode_t band_mode);

__s32 dsk_radio_set_threshold(dsk_radio_threshold_mode_t threshold);

__s32 dsk_radio_set_audio_mode(dsk_radio_audio_mode_t audio_mode);

__s32 dsk_radio_get_band(__s32 *cur_band);

__s32 dsk_radio_get_audio_mode(__s32 *audio_mode);

__s32 dsk_radio_get_min_freq(__s32 *min_freq);

__s32 dsk_radio_get_max_freq(__s32 *max_freq);

/*
**********************************************************************************************************************
*                                               dsk_radio_control
*
* author:        terry.zeng
*
* date:             2009-12-01
*
* Description:      dsk_radio功能操作接口,参数1一般为输入参数，参数2为输出参数
*
* parameters:
*
* return:           if success return 0
*                   if fail return -1
* modify history:
**********************************************************************************************************************
*/
//__s32 dsk_radio_control(__s32 cmd,__s32 param1,__s32 param2);
/*
**********************************************************************************************************************
*                                               dsk_radio_close
*
* author:        terry.zeng
*
* date:             2009-12-01
*
* Description:       关闭dsk_radio，释放相关资源
*
* parameters:
*
* return:           if success return 0
*                   if fail return -1
* modify history:
**********************************************************************************************************************
*/
__s32 dsk_radio_close(void);
__s32 dsk_radio_get_vol(void);

__s32 dsk_radio_set_vol(__u8 vol);

#endif
