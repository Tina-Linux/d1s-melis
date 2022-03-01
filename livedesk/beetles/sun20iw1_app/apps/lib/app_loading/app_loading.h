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

#ifndef __APP_LOADING_H_
#define  __APP_LOADING_H_

#include "beetles_app.h"
#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   480
#define DTV_LOADING_ENABLE
typedef struct tag_app_loading_para
{
    H_LYR       loading_layer;
    GUI_FONT    *font;
    __u16       id ;
} app_loading_para_t;

H_WIN app_loading_win_create(H_WIN hparent, __u16 id) ;
void  app_loading_win_delete(H_WIN loading_win);
void  app_loading_win_hide(H_WIN loading_win);//add by lihaoyi
void  app_loading_win_show(H_WIN loading_win);


#endif
