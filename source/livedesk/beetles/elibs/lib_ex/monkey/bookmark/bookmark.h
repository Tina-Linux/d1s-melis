/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                                   (c) Copyright 2006-2007, ANDY, China
*                                            All Rights Reserved
*
* File    : bookmark.h
* By      : Andy.zhang
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __BOOK_MARK_H__
#define __BOOK_MARK_H__

#include "monkey_i.h"

typedef void  *HBMK;

HBMK    bmk_open(char *szAppFile, __u32 mode);

void    bmk_set_offset(HBMK hBmk, int i, int offset);
__bool  bmk_get_offset(HBMK hBmk, int i, int *p_offset);

void    bmk_set_last_offset(HBMK hBmk, int offset);
__bool  bmk_get_last_offset(HBMK hBmk, int *p_offset);

__s32   bmk_close(HBMK hBmk);

#endif  /* __BOOK_MARK_H__ */

