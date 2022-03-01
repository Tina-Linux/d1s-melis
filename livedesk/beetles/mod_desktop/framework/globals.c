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

#include "mod_desktop_i.h"


struct _g_globals
{
    ActivityManager *acm;
    AppletInfoManager *aim;
    H_WIN mainwin;
};

static struct _g_globals global;


void activity_manager_set_default(ActivityManager *manager)
{
    global.acm = manager;
}

void applet_info_manager_set_defualt(AppletInfoManager *manager)
{
    global.aim = manager;
}

void message_loop_win_set_default(H_WIN win)
{
    global.mainwin = win;
}


ActivityManager *activity_manager_get_default(void)
{
    return global.acm;
}

AppletInfoManager *applet_info_manager_get_default(void)
{
    return global.aim;
}

H_WIN message_loop_win_get_default(void)
{
    return global.mainwin;
}

