/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File          : applet_plugin_module.c
* By            : Andy.zhang
* Func      : applet plugin module interface
* Version   : v1.0
* ============================================================================================================
* 2010-9-3 9:46:25  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "applet_plugin_module.h"

typedef struct _PrivInfo
{
    __u32   mid;
    __mp    *mp;
} PrivInfo;

static __s32 applet_plugin_module_start(Activity *thiz)
{
    AppletPlugin *plugin    = activity_get_plugin(thiz);
    PrivInfo     *priv      = (PrivInfo *)plugin->priv;

    return esMODS_MIoctrl(priv->mp, APPLET_CMD_START, 0, (void *)thiz);
}

static __s32 applet_plugin_module_suspend(Activity *thiz)
{
    AppletPlugin *plugin    = activity_get_plugin(thiz);
    PrivInfo     *priv      = (PrivInfo *)plugin->priv;

    return esMODS_MIoctrl(priv->mp, APPLET_CMD_SUSPEND, 0, (void *)thiz);
}

static __s32 applet_plugin_module_resume(Activity *thiz)
{
    AppletPlugin *plugin = activity_get_plugin(thiz);
    PrivInfo     *priv   = (PrivInfo *)plugin->priv;
    return esMODS_MIoctrl(priv->mp, APPLET_CMD_RESUME, 0, (void *)thiz);
}

static __s32 applet_plugin_module_background(Activity *thiz)
{
    AppletPlugin *plugin    = activity_get_plugin(thiz);
    PrivInfo     *priv      = (PrivInfo *)plugin->priv;

    return esMODS_MIoctrl(priv->mp, APPLET_CMD_BACKGROUND, 0, (void *)thiz);
}

static __s32 applet_plugin_module_stop(Activity *thiz)
{
    AppletPlugin *plugin    = activity_get_plugin(thiz);
    PrivInfo     *priv      = (PrivInfo *)plugin->priv;

    esMODS_MIoctrl(priv->mp, APPLET_CMD_STOP, 0, (void *)thiz);
    esMODS_MClose(priv->mp);
    esMODS_MUninstall(priv->mid);
    esMEMS_Mfree(0, plugin);
    return EPDK_OK;
}

AppletPlugin *applet_plugin_module_create(char *module_path)
{
    AppletPlugin    *plugin = NULL;
    PrivInfo        *priv = NULL;

    plugin  = (AppletPlugin *)esMEMS_Malloc(0, sizeof(AppletPlugin) + sizeof(PrivInfo));

    if (plugin == NULL)
    {
        __err("AppletPlugin malloc error");
        return NULL;
    }

    memset(plugin, 0, sizeof(AppletPlugin) + sizeof(PrivInfo));

    priv        = (PrivInfo *)plugin->priv;
    priv->mid   = esMODS_MInstall(module_path, 0);

    if (priv->mid == 0)
    {
        __err("%s install error ", module_path);
        esMEMS_Mfree(0, plugin);
        return NULL;
    }

    priv->mp  = esMODS_MOpen(priv->mid, 0);

    if (priv->mp == NULL)
    {
        __err("%s open error ", module_path);
        esMEMS_Mfree(0, plugin);
        return NULL;
    }

    plugin->start       = applet_plugin_module_start;
    plugin->suspend     = applet_plugin_module_suspend;
    plugin->resume      = applet_plugin_module_resume;
    plugin->background  = applet_plugin_module_background;
    plugin->stop        = applet_plugin_module_stop;
    return plugin;
}
