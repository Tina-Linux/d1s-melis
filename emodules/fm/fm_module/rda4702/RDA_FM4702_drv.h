/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              hello world sample
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File    : tea5767.h
* By      : xueli
* Version : V1.00
*********************************************************************************************************
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
