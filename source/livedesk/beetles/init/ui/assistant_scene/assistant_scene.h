/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File          : hellocompass_scene.h
* By            : jackie.chen
* Func      : hellocompass scene
* Version   : v1.0
* ============================================================================================================
* 2010-12-03   jackie.chen  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __ASSISTANT_SCENE_H__
#define __ASSISTANT_SCENE_H__

#define BTN_ID_ESCAPE 0

H_WIN assistant_scene_create(H_WIN parent);

__s32 assistant_scene_delete(H_WIN hwnd);

#endif

