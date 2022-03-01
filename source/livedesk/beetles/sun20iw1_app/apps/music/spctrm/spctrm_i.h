/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                               (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : spctrm_i.h
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#ifndef _SPCTRM_I_H_
#define _SPCTRM_I_H_

//#include "music_ui_i.h"

#include "spctrm.h"

#include "spctrm_uipara.h"

//#define NUM_SPCTRM  12
#define NUM_SPCTRM  9

#define ID_SPCTRUM  2000

typedef struct __TAG_SPCTRM_ITEM
{
    HTHEME h_theme_particle_bmp;
    HTHEME h_theme_buoy_bmp;
} __spctrm_item;

#endif

