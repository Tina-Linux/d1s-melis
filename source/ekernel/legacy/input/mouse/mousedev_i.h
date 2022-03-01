/*
*********************************************************************************************************
*                                                    eMOD
*                                         the melis Operation System
*                                               input sub-system
*                                          mouse logic device handler
*
*                                    (c) Copyright 2010-2012, sunny China
*                                              All Rights Reserved
*
* File   : mousedev_i.h
* Version: V1.0
* By     : Sunny
* Date   : 2010-7-11
* Note   : this module is for general mouse device handler,
*          all mouse class physic devices use this module to dispatch and package message.
*********************************************************************************************************
*/
#ifndef     __MOUSE_DEV_I_H__
#define     __MOUSE_DEV_I_H__
#include    "../sys_input.h"

typedef struct __LNOUSE_DEV_MAN
{
    __hdle                  lock;        /* lock for protect logical mouse system */
    __input_ldev_t          *LMouseDev; /* logical mouse device handle           */
    __input_event_packet_t  packet;    /* logical mouse device event packet     */
} __lmouse_dev_man_t;

#endif      /* __MOUSE_DEV_I_H__ */
