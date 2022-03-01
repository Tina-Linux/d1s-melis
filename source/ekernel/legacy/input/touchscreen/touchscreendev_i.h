/*
*********************************************************************************************************
*                                                    eMOD
*                                         the melis Operation System
*                                               input sub-system
*                                          touchscreen logic device handler
*
*                                    (c) Copyright 2010-2012, sunny China
*                                              All Rights Reserved
*
* File   : touchscreendev_i.h
* Version: V1.0
* By     : Sunny
* Date   : 2010-7-11
* Note   : this module is for general touchpanel device handler,
*          all touchpanel class physic devices use this module to dispatch and package message.
*********************************************************************************************************
*/
#ifndef     __TOUCHSCREEN_DEV_I_H__
#define     __TOUCHSCREEN_DEV_I_H__
#include    "../sys_input.h"

typedef struct __LTS_DEV_MAN
{
    __hdle                  lock;      /* lock for protect logical tp system       */
    __input_ldev_t          *LTSDev;    /* logical tp device handle                 */
    __input_event_packet_t  packet;    /* logical tp device event packet           */

} __lts_dev_man_t;

#endif      /* __TOUCHSCREEN_DEV_I_H__ */
