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
#ifndef  _RDA_FM4702_DRV_H_
#define  _RDA_FM4702_DRV_H_

#include  "drv_fm_i.h"
#if(FM_MODULE_TYPE == FM_MODULE_TYPE_RDA4702)
/***************************************************************************
 * Type Definitions
 ***************************************************************************/
typedef enum TAG_WRITE_REG_ADDR
{
    WRITING,
    W02h,
    W03h,
    W04h,
    W05h,
    W06h,
    W07h,
    W08h,
    W09h
} WRITE_REG_ADDR;

typedef enum TAG_READ_REG_ADDR
{
    READING,
    R0Ah,
    R0Bh,
    R0Ch,
    R0Dh,
    R0Eh,
    R0Fh,
    R00h,
    R01h,
    R02h,
    R03h,
    R04h,
    R05h,
    R06h,
    R07h,
    R08h,
    R09h
} READ_REG_ADDR;
typedef enum TAG_SEEKTH
{
    SEEKTH25, SEEKTH26, SEEKTH27, SEEKTH28,
    SEEKTH29, SEEKTH30, SEEKTH31, SEEKTH32
} SEEKTH;


//typedef enum SEEK_DIRECTION_TAG
//{
//SEEKUP =0,
//SEEKDOWN=1
//}SEEK_DIRECTION;

#define SEEKUP    0
#define SEEKDOWN  1

typedef enum VOL_DIRECTION_TAG
{
    TURNUP,
    TURNDOWN
} VOL_DIRECTION;

typedef enum SEEK_STATUS_TAG
{
    SEEKSUCCESS,
    SEEKFAILURE
} SEEK_STATUS;


#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


#define  FM_MAX_FREQ               108000
#define  FM_MIN_FREQ                87500

typedef struct __FM_SETTING_INFO
{
    __s32   min_freq;
    __s32   max_freq;
    __s8    area_select;
    __s8    stereo_select;
    __s8    signal_level;
    __s8    reserved;

} __fm_setting_info_t;


#define SEEKUP    0
#define SEEKDOWN  1
extern   __s32  fm_init(void);
extern   __s32  fm_exit(void);
extern   __s32  fm_play(__s32 freq);
extern   __s32  fm_auto_search(__s32 freq, __u32 search_dir);
extern   __s32  fm_manual_search(__s32 freq, __u32 search_dir);
extern   __s32  fm_area_choose(__s32 area, void *pbuffer);
extern   __s32  fm_stereo_choose(__s32 audio_method);
extern   __s32  fm_send(__s32 freq);
extern   __s32  fm_mute(__s32 voice_onoff);
extern   __s32  fm_signal_level(__s32 signal_level);

extern   __s32  fm_send_on(void);
extern   __s32  fm_send(__s32 freq);
extern   __s32  fm_send_off(void);
extern   __s32  fm_pa_gain(__u8 pagain);
extern   __s32  fm_get_status(void);
extern   __s32  fm_open_rds(void);
extern   __s32  fm_close_rds(void);
extern   __s32 fm_rds_rdsr(void);
extern   __s32 fm_rds_rdse(void);
extern   __s32 fm_rds_block(__s32 block);

#endif

#endif /*_RDA_FM4702_DRV_H_*/
