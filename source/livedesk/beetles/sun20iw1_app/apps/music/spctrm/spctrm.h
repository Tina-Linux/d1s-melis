/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                               (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : spectrum.h
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#ifndef _SPCTRM_H_
#define _SPCTRM_H_

/* define message para   */
typedef enum        /* user define msg id */
{
    SPCTRM_ID_UPDATE = GUI_MSG_,
    SPCTRM_ID_SET_NUM
} __spctrm_cmdid_t;

H_WIN SPCTRM_SceneCreate(H_WIN h_parent, H_WIN lyrwin);
void  SPCTRM_Update(H_WIN hwin);
void  SPCTRM_Clear(__u32 value);                    //add by libaiao,2011.5.10

#endif
