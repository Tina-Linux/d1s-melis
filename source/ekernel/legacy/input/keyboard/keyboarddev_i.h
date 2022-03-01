/*
*********************************************************************************************************
*                                                    eMOD
*                                         the melis Operation System
*                                               input sub-system
*                                          keyboard logic device handler
*
*                                    (c) Copyright 2010-2012, sunny China
*                                              All Rights Reserved
*
* File   : keyboarddev_i.h
* Version: V1.0
* By     : Sunny
* Date   : 2010-7-11
* Note   : this module is for general key device handler,
*          all key class physic devices use this module to dispatch and package message.
*********************************************************************************************************
*/
#ifndef   __KEYBOARD_DEV_I_H__
#define   __KEYBOARD_DEV_I_H__
#include    "../sys_input.h"

typedef struct __LKEY_DEV_MAN
{
    __hdle                  lock;      /* lock for protect logical keyboard system  */
    __input_ldev_t          *LKeyDev;   /* logical key device handle                 */
    __input_event_packet_t  packet;    /* logical key device event packet           */
} __lkey_dev_man_t;

#endif      /* __KEYBOARD_DEV_I_H__ */
