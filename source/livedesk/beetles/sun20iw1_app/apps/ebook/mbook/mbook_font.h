/*
**************************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              FM app sample
*
*                                   (c) Copyright 2006-2007, ANDY, China
*                                            All Rights Reserved
*
* File    : mbook_font.h
* By      : lyn
* Version : V1.00
* time    : 2009-8-14
**************************************************************************************************************
*/
#ifndef __MBOOK_FONT_H__
#define __MBOOK_FONT_H__

typedef     void   *H_FONT;

H_FONT  MBOOK_Font_Init(GUI_FONT *font, __u32 size);

__s32   MBOOK_Font_Uninit(H_FONT hdle);

__s32  *MBOOK_Font_GetTable(H_FONT hdle);

GUI_FONT   *MBOOK_Font_GetFont(H_FONT hdle);


#endif /* __MBOOK_FONT_H__ */

