/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              anole app sample
*
*                               (c) Copyright 2006-2007, John, China
*                                           All Rights Reserved
*
* File    : anole_i.h
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#ifndef _ANOLE_I_H_
#define _ANOLE_I_H_

#include <typedef.h>
#include "lib_ex.h"

#define ANOLE_MAX_FULL_FILE_PATH_LEN            ( 4*1024 )
#define ANOLE_MAX_FILE_NAME_LEN             256

#define ANOLE_REG_BASE      "anole"

#define ANOLE_NODE_SETTING      "setting"
#define ANOLE_KEY_PALYORDRE     "play_order"
#define ANOLE_KEY_TIMESLOT      "time_slot"
#define ANOLE_KEY_SWITCHMODE    "switch_mode"

#define SWITCH_RANDOM  0xff

typedef struct tag_ANOLE_BASE
{
    __anole_playorder_t         PlayOrder;
    __anole_switch_mode_t       SwitchMode;     /*switch mode*/
    __u32                       interval;
    __anole_sta_t               status;
    __s32                       scale_level;        /* -4 ~ 4 = (1/16x ~ 16x  ) */
    __u32                       maintask;
    __s32                       *switchmodearray;
    __s32                       arraynum;
} __anole_core_t;



#endif

