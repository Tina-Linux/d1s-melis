/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : globals.h
* By        : Andy.zhang
* Func      : applet info manager
* Version   : v1.0
* ============================================================================================================
* 2010-9-6 19:43:50  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __GLOBALS_H__
#define __GLOBALS_H__



ActivityManager    *activity_manager_get_default(void);
AppletInfoManager  *applet_info_manager_get_default(void);
H_WIN               message_loop_win_get_default(void);

void activity_manager_set_default(ActivityManager *manager);
void applet_info_manager_set_defualt(AppletInfoManager *manager);
void message_loop_win_set_default(H_WIN win);



#endif /* __GLOBALS_H__ */
