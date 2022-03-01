/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : headbar_com.h
* By        : Andy.zhang
* Func      : headbar manage
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __HEADBAR_COM_H__
#define __HEADBAR_COM_H__

#include <kconfig.h>

#define ID_HEADBAR_COM              0x01
#define ID_VOLUME_COM               0x02
#define ID_BRIGHT_COM               0x04
#define MAX_SIGNAL_LEVEL            6

/* Í¨ÖªÂë */
#define HBAR_ADJUST_VOLUME          1
#define HBAR_ADJUST_BRIGHT          2
#define HBAR_RETURN_HOME            3
#define HBAR_MUISC_STATE            4


typedef enum _headbar_cmd_id
{
    HEADBAR_CMD_SET_VOLUME          = GUI_MSG_USER_DEF,
    HEADBAR_CMD_SET_BRIGHT,
    HEADBAR_CMD_SET_FORMAT,
    HEADBAR_CMD_GET_SCN_POS,
    HEADBAR_CMD_GET_SIZE
} headbar_cmd_id;


typedef struct
{
    char        *name;
    __u16       id;

    H_WIN       parent;
    H_WIN       layer;
    RECT        rect;
    GUI_FONT    *font;           // ×ÖÌå¾ä±ú
} __headbar_create_para;

H_WIN headbar_frm_create(H_WIN parent, H_LYR layer);
__s32 headbar_frm_delete(H_WIN frmwin);

__s32 headbar_enable_paint_time(__bool enable);
__s32 headbar_enable_paint_signal(__bool enable);

#endif /* __HEADBAR_COM_H__ */
