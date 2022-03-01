/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, Jackie, China
*                                            All Rights Reserved
*
* File          : mini_music.c
* By            : Jackie.chen
* Func      : mini music
* Version   : v1.0
* ============================================================================================================
* 2010-11-11   Jackie.chen  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include <log.h>
#include "mod_init_i.h"
#include "mini_music_scene.h"
#include "mini_music_uipara.h"

__s32 mini_music_open(H_WIN parent, __s32 x, __s32 y)
{
    /*__mini_music_create_para_t create_para;

    __msg(" mini music open! \n");

    if(!dsk_wkm_is_open())
    {
        __s32 index;
        char audio_file[256];

        dsk_wkm_open();
        index = dsk_wkm_npl_get_cur();
        dsk_wkm_npl_index2file(index, audio_file);
        dsk_wkm_play_file(audio_file);
    }

    if(!scene_exist())
    {
        create_para.parent  = parent;
        create_para.x       = x;
        create_para.y       = y;
        scene_create(&create_para);
    }

    */
    __err("headbar");
    return 0;
}

__s32 mini_music_close(void)
{
    /*__msg(" mini music close! \n");

    scene_delete();
    //if(dsk_wkm_is_open())
    //{
    //  dsk_wkm_close();
    //}
    */
    return 0;
}

__bool mini_music_scene_exist()
{
    /*return scene_exist();
    */
    return 0;
}

