/*
************************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                 (c) Copyright 2007-2010, Jackie, China
*                                       All Rights Reserved
*
* File          : dialog_scene.h
* By            : Jackie.chen
* Func      : desktop scene
* Version   : v1.0
* ======================================================================================================================
* 2010-10-18  Jackie.chen  create this file, implements the fundemental interface;
************************************************************************************************************************
*/

#ifndef __DIALOG_SCENE_H__
#define __DIALOG_SCENE_H__

#define LOW_POWER_DIALOG_TIME_ID        3000
#define POWER_OFF_DIALOG_TIME_ID        3001
#define ORCHID_UPDATE_DIALOG_TIME_ID    3002

//typedef enum
//{
//  POWEROFF_DIALOG,
//  LOWPOWER_DIALOG,
//  USBD_DIALOG,
//  ORCHID_DIALOG,
//  FW_UPDATA_DIALOG,
//  NONE_DIALOG
//}main_dialog_t;
//
//typedef struct
//{
//  /* Í¼²ã¾ä±ú */
//  H_LYR dialog_layer;         // µ¯¿òÍ¼²ã
//
//  /* ´°¿Ú¾ä±ú */
//  H_WIN parent;
//  H_WIN dialog_win;           // ¶Ô»°¿ò´°¿Ú
//
//  /* ÏµÍ³×´Ì¬ dailog Ïà¹Ø */
//  main_dialog_t cur_dialog;
//
//  __bool usb_connect;
//  __bool orchid_update;
//  __bool low_power;
//  __bool power_off;
//  __bool fw_update;
//
//  __bool msg_lock;
//}dialog_scene_t;

__s32 dialog_init(H_WIN hwnd);

__s32 dialog_deinit(H_WIN hwnd);

void scene_on_dialog(__gui_msg_t *msg);

#endif

