/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              Apps Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lion
* File    : msgbox_private.h
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-4 11:28:56
**********************************************************************************************************************
*/
#ifndef _MSGBOX_PRIVATE_H_
#define _MSGBOX_PRIVATE_H_

#include "gui_i.h"

typedef enum GUI_MBOX_FLAG
{
    NO_BUTTON = 0,
    YES_BUTTON,
    YES_NO_BUTTON,
    YES_NO_CANCEL_BUTTON
} __gui_mbox_flag_t;

typedef struct tag__GUI_MBOX
{
    char                name[GUI_NAME_MAX];
    __pGUI_WIN_CB       mbox_cb;
    __u32               nbutton;
    __pGUI_WIN_CB       yes_usercb;
    __pGUI_WIN_CB       no_usercb;
    __pGUI_WIN_CB       cancel_usercb;
    H_WIN               yesbutton;
    H_WIN               nobutton;
    H_WIN               cancelbutton;
    H_WIN               box_win;
    H_LYR               layer;
    __u8                flags;
    void               *usr_attr;
} __gui_mbox_t;

#endif /* #ifndef _MSGBOX_PRIVATE_H_ */
