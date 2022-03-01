/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              LARK sample
*
*                               (c) Copyright 2006-2007, terry, China
*                                           All Rights Reserved
*
* File    : dsk_radio_p.h
* By      : terry.zeng
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __DSK_RADIO_SEND_I_H__
#define __DSK_RADIO_SEND_I_H__

#include "apps.h"

__s32 dsk_radio_send_open(void);

__s32 dsk_radio_send_close(void);

__bool dsk_radio_send_is_open(void);

__s32 dsk_radio_send_set_freq_play(signed long freq);

__s32 dsk_radio_send_get_cur_freq(__u32 *freq);

__s32 dsk_radio_send_set_gain(unsigned long gain);

__s32 dsk_radio_send_get_gain(__u32 *gain);


#endif
