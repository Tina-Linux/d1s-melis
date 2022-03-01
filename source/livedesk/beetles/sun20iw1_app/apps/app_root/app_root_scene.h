/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File          : app_root_win_scene.h
* By            : Bayden
* Func      : app_root applets plugin interface define
* Version   : v1.0
* ============================================================================================================
* 2010-9-28 17:11:20  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

/**********************************************************************************************************************/
#ifndef __APP_ROOT_SCENE_H
#define __APP_ROOT_SCENE_H

H_WIN       app_root_wincreate(Activity *activity);
int32_t     __app_root_get_bg_music_index(uint32_t *index);

#endif
