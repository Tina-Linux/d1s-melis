/*
*********************************************************************************************************
*                                                    eMOD
*                                         the melis Operation System
*                                               HID sub-system
*                                          keyboard logic device handler
*
*                                    (c) Copyright 2010-2012, sunny China
*                                              All Rights Reserved
*
* File   : keyboarddev.h
* Version: V1.0
* By     : Sunny
* Date   : 2010-7-11
* Note   : this module is for general key device handler,
*          all key class physic devices use this module to dispatch and package message.
*********************************************************************************************************
*/

#ifndef     __KEYBOARD_DEV_H__
#define     __KEYBOARD_DEV_H__

extern int32_t  INPUT_LKeyDevInit(void);
extern void     INPUT_LKeyDevExit(void);

#endif      /* __KEYBOARD_DEV_H__ */
