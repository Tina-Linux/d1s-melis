/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : msgbox.h
* By        : Andy.zhang
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __EBOOK_TIPS_H_
#define  __EBOOK_TIPS_H_

#include "beetles_app.h"

#define TIPS_YES 0
#define TIPS_NO  1

#define TIPS_YES_NO_BMP_WIDTH 37
#define TIPS_YES_NO_BMP_HEIGHT 18

typedef struct tag_ebook_tips_para
{
    GUI_FONT *font;
    H_LYR     layer;
} ebook_tip_para_t;

H_WIN ebook_tips_win_create(H_WIN h_parent, ebook_tip_para_t *para);

#endif
