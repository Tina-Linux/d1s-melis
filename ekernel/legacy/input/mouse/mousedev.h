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
* File   : mousedev.h
* Version: V1.0
* By     : Sunny
* Date   : 2010-7-11
* Note   : this module is for general mouse device handler,
*          all mouse class physic devices use this module to dispatch and package message.
*********************************************************************************************************
*/

#ifndef     __MOUSE_DEV_H__
#define     __MOUSE_DEV_H__

extern int32_t INPUT_LMouseDevInit(void);
extern void  INPUT_LMouseDevExit(void);

#endif      /* __MOUSE_DEV_H__ */
