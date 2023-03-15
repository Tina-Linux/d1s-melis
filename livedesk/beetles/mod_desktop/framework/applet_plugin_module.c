/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
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
