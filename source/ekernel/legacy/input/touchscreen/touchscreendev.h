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
* File   : touchscreendev.h
* Version: V1.0
* By     : Sunny
* Date   : 2010-7-11
* Note   : this module is for general touchpanel device handler,
*          all touchpanel class physic devices use this module to dispatch and package message.
*********************************************************************************************************
*/

#ifndef     __TOUCHSCREEN_DEV_H__
#define     __TOUCHSCREEN_DEV_H__

extern int32_t INPUT_LTSDevInit(void);
extern void  INPUT_LTSDevExit(void);

#endif      /* __TOUCHSCREEN_DEV_H__ */
