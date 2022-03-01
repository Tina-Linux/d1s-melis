
/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : app_movie_i.h
* By        : Bayden.chen
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05 Bayden.chen  create this file
**************************************************************************************************************
*/

#ifndef __APP_MOVIE_I_H__
#define  __APP_MOVIE_I_H__

#include "beetles_app.h"
#include "common/common.h"
#include "spsc/spsc_man.h"
#include "play_sta/play_sta.h"
#include "sub_show/sub_show.h"
#include "prog/prog.h"
#include "sset/movie_sset.h"
#include "ff_rr_show/ff_rr_show.h"
#include "app_movie.h"

//定义movie的子场景的ID
#define     MOVIE_SPSC_ID                       0x1
#define     MOVIE_PLAYSTA_ID                    0x2
#define     MOVIE_SUB_SHOW_ID                   0x4
#define     MOVIE_PROG_ID                       0x8
#define     MOVIE_BREAKPLAY_DIALOG_ID           0x10
#define     MOVIE_PLAYERROR_DIALOG_ID           0x20
#define     MOVIE_PLAYEND_DIALOG_ID             0x40
#define     MOVIE_SSET_ID                       0x80
#define     MOVIE_FF_RR_SHOW_ID                 0x100
#define     MOVIE_SUB_SCENE_NULL                0x0
#define     MOVIE_SPSC_INFOR_ID                 0x110

#define     MOVIE_CHANNEL_ID                    0x120
#define     MOVIE_TRACK_ID                      0x121
#define     MOVIE_PLAY_MODE_ID                  0x122
#define     MOVIE_ZOOM_ID                       0x123
#define     MOVIE_SUBTITLE_ID                   0x124
#define     MOVIE_AB_ID                         0x125

typedef enum FOCUSE_STA
{
    FOCUSE_NORMAL = 0,
    FOCUSE_PRESS,

    FOCUSE_STATE_MAX
} FOCUSE_STA;



//所有子场景id
#define     MOVIE_SUB_SCENE_TYPE_ALL   (MOVIE_SPSC_ID|MOVIE_PLAYSTA_ID|MOVIE_SUB_SHOW_ID\
                                        |MOVIE_PROG_ID|MOVIE_BREAKPLAY_DIALOG_ID|MOVIE_PLAYERROR_DIALOG_ID|MOVIE_PLAYEND_DIALOG_ID\
                                        |MOVIE_SSET_ID|MOVIE_FF_RR_SHOW_ID)

//除了spsc场景和sub_show场景以外的场景id
#define     MOVIE_SUB_SCENE_TYPE_OTHER (MOVIE_SUB_SCENE_TYPE_ALL&(~MOVIE_SPSC_ID)\
                                        &(~MOVIE_SUB_SHOW_ID))

#endif

