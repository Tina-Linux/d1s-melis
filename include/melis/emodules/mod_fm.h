/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                               hello world sample
*
*                                (c) Copyright 2006-2007, Steven.ZGJ China
*                                            All    Rights Reserved
*
* File    : drv_fm.h
* By      : jerry
* Version : V1.00
* Date    : 2009-5
*********************************************************************************************************
*/
#ifndef  _MOD_FM_H_
#define  _MOD_FM_H_
#include <typedef.h>

//#define  FM_SEARCH_CHN_MIN_FREQ       87500    //4702SIÄ£¿é
#define  FM_SEARCH_CHN_MIN_FREQ         87000   //5807MÄ£¿é

#define  FM_SEARCH_CHN_MAX_FREQ         108000

#define  FM_SEARCH_JAP_MIN_FREQ         76000
#define  FM_SEARCH_JAP_MAX_FREQ         91000

#define  FM_SEARCH_STEP                 100

#define  FM_DRV_STATUS_RECEIVE          0x8000
#define  FM_DRV_STATUS_SEND             0x8001

typedef enum  __DRV_FM_CMD_SET
{
    DRV_FM_CMD_NONE              = 0,
    DRV_FM_CMD_INIT,
    DRV_FM_CMD_EXIT,
    DRV_FM_CMD_AREA_SET,
    DRV_FM_CMD_STEREO_SET,
    DRV_FM_CMD_MUTE,
    DRV_FM_CMD_SLEVEL_SET,
    DRV_FM_CMD_RECVE_INIT,
    DRV_FM_CMD_RECVE_EXIT,
    DRV_FM_CMD_RECVE_PLAY,
    DRV_FM_CMD_RECVE_SEARCH,
    DRV_FM_CMD_SET_SEARCH_CB,
    DRV_FM_CMD_SEND_INIT,
    DRV_FM_CMD_SEND_START,
    DRV_FM_CMD_SEND_EXIT,
    DRV_FM_CMD_SEND_PAGAIN_SET,
    DRV_FM_CMD_QUERY_STATUS,
    DRV_FM_CMD_VOL_SET,
    DRV_FM_CMD_VOL_GET,
} __drv_fm_cmd_set_t;

#define   DVR_FM_AREA_JAPAN             0xff00
#define   DRV_FM_AREA_CHN_US_EUP        0xff01
#define   DRV_FM_AREA_CHN_SCHOOL        0xff02

#define   DRV_FM_STEREO                 0xf000
#define   DRV_FM_MONO                   0xf001

#define   DRV_FM_AUTO_SEARCH            0x00f1
#define   DRV_FM_MANUAL_SEARCH          0x00f0

#define   DRV_FM_SEARCH_DOWN            0xf000
#define   DRV_FM_SEARCH_UP              0xf100

#define   DRV_FM_VOICE_ON               0xf0f0
#define   DRV_FM_VOICE_OFF              0xf0f1

#define   DRV_FM_SLEVEL_HIGH            0xfff0
#define   DRV_FM_SLEVEL_NORMAL          0xfff1
#define   DRV_FM_SLEVEL_LOW             0xfff2

typedef struct __DRV_FM_AREA_FREQ
{
    __s32 fm_area_min_freq;
    __s32 fm_area_max_freq;
} __drv_fm_area_freq_t;

#endif   /*_DRV_FM_H_*/
