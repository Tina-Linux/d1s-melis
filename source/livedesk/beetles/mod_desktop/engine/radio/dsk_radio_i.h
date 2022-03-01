/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              LARK sample
*
*                               (c) Copyright 2006-2007, terry, China
*                                           All Rights Reserved
*
* File    : lark_p.h
* By      : terry.zeng
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __DSK_RADIO_I_H__
#define __DSK_RADIO_I_H__

#include "engine_i.h"
#include <mod_fm.h>
#include "dfs_posix.h"
//#include ".\receive\dsk_radio_receive_i.h"
//#include ".\send\dsk_radio_send_i.h"
typedef struct
{
    int                     fm_drv;

    __s32                   drv_status;

    /*当前频率*/
    __s32                   cur_freq;
    /*当前波段*/
    __s32                   cur_band;
    /*当前波段对应的频率范围*/
    __drv_fm_area_freq_t    freq_range;
    /*当前声道设置*/
    dsk_radio_audio_mode_t  audio_mode;

    //void                  *rcv_handle;

    //void                  *send_handle;

} dsk_radio_t;

extern dsk_radio_t *h_radio;

#endif

