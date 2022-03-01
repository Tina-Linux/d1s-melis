/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : live_init.h
* By        : Andy.zhang
* Func      : Live main res init
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __LIVE_INIT_H__
#define __LIVE_INIT_H__


//#define THEM_BIN_PATH   BEETLES_APP_ROOT"apps\\theme.bin"
//#define THEM_BIN_PATH   "E:\\apps\\theme.bin"
#define THEM_BIN_PATH           "/data/apps/theme.bin"

extern GUI_FONT     *SWFFont;
extern GUI_FONT     *ScnLockFont;
extern int32_t      LCD_WIDTH;
extern int32_t      LCD_HEIGHT;

//跟init_manwin相关的资源初始化分开处理
int32_t live_init(void);
int32_t live_exit(void);

void    init_screen_size(void);

#endif /* __LIVE_INIT_H__ */
