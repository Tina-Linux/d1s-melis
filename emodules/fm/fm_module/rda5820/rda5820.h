/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              hello world sample
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File    : rda5820.h
* By      : xueli
* Version : V1.00
*********************************************************************************************************
*/
#ifndef  _FM_RDA5820_H_
#define  _FM_RDA5820_H_

#include  "drv_fm_i.h"

#if(FM_MODULE_TYPE == FM_MODULE_TYPE_RDA5820)

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
extern   __s32  fm_send(__s32 freq);
extern   __s32  fm_mute(__s32 voice_onoff);
extern   __s32  fm_signal_level(__s32 signal_level);

extern   __s32  fm_send_on(void);
extern   __s32  fm_send(__s32 freq);
extern   __s32  fm_send_off(void);
extern   __s32  fm_pa_gain(__u8 pagain);

extern   __s32  fm_get_status(void);

#endif


#endif /*_FM_TEA5767_H_*/
