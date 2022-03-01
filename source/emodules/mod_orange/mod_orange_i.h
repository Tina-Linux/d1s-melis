/*
*********************************************************************************************************
*                                                   eMOD
*                                  the Easy Portable/Player Operation System
*                                              mod_lemon sub-system
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File   : mod_orange_i.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*////

#ifndef _MOD_ORANGE_I_H_
#define _MOD_ORANGE_I_H_

#define DEBUGLEVEL EPDK_DEBUG_LEVEL_LOG_ALL

#include "mod_orange.h"
#include "GUI/gui_e.h"
#include <mod_defs.h>
#include <kapi.h>

typedef __u32 INT32U;
typedef __u16 INT16U;
typedef __u8  INT8U;
typedef __s32 INT32S;
typedef __s16 INT16S;
typedef __s8 INT8S;

#ifndef TRUE
#define TRUE EPDK_TRUE
#endif
#ifndef FALSE
#define FALSE EPDK_FALSE
#endif


typedef struct
{
    __u8            mid;
    __u8              used;
    __u16          *fbaddr[GUI_NUM_LAYERS];  //  mcl 08.12.11
    __u32           width1;
    __u32           height1;

} __orange_t;

extern __orange_t orange;
extern __mp *drv_key;

#endif  /* _MOD_LEMON_I_H_ */


