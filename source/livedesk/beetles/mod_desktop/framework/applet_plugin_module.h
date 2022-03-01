/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : applet_plugin_module.h
* By        : Andy.zhang
* Func      : applet plugin module interface
* Version   : v1.0
* ============================================================================================================
* 2010-9-3 9:46:25  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __APPLET_PLUGIN_MODULE_H__
#define __APPLET_PLUGIN_MODULE_H__

#include "mod_desktop_i.h"

AppletPlugin *applet_plugin_module_create(char *path);

#endif /* __APPLET_PLUGIN_MODULE_H__ */

