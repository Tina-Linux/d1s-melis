/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2011, CHIPHD, China
*                                            All Rights Reserved
*
* File      : photo_main.h
* By        : CQQ
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011/06/15 17:26  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#ifndef __PHOTO_MAIN_H__1BCC82B8_2AEA_420A_BB7D_BCB181413437__
#define __PHOTO_MAIN_H__1BCC82B8_2AEA_420A_BB7D_BCB181413437__
//////////////////////////////////////////////////////////////////////////
#include "App_photo_i.h" // "photo_uipara.h"
#include "photo_fileinfo.h"

#define PHOTO_MAIN_FRW_NAME               "PHOTO_MAIN_FRW"
#define PHOTO_MAIN_FRW_ID                 1000



enum
{
    PHOTO_SHOW_NULL = 0,
    PHOTO_SHOW_AUTO_PLAY,
    //  PHOTO_SHOW_MANUAL_PLAY,
    PHOTO_SHOW_PLAY_BACKWARD,
    PHOTO_SHOW_PLAY_FORWARD,
    PHOTO_SHOW_PLAY_NEXT,
    PHOTO_SHOW_PLAY_PREV,
    PHOTO_SHOW_PAUSE,
    PHOTO_SHOW_FILEINFO,
    PHOTO_SHOW_MENU,
    PHOTO_SHOW_SCALE,
};



H_WIN photo_main_frw_create(H_WIN h_parent, H_LYR layer, void *para);

#endif //__PHOTO_MAIN_H__1BCC82B8_2AEA_420A_BB7D_BCB181413437__
//End of this file

