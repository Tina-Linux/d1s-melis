/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      :app_movie.h
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05   Bayden.chen  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __APP_MOVIE_H__
#define  __APP_MOVIE_H__

#define MOVIE_LCD_WIDTH 800
#define MOVIE_LCD_HEIGHT 480

#define KEY_UP_DOWN_POPUP_MENU  0

H_WIN app_movie_create(root_para_t *para);
__s32 app_movie_notify_delete_sub_scene(H_WIN hmanwin);
__s32 app_movie_notify_delete_sub_dlg_scene(H_WIN hmanwin);
__s32 app_movie_get_root_type(H_WIN movie_man_win, __u32 *root_type);

#endif
