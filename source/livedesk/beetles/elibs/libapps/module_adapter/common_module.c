#include "common_module_i.h"
#include <kapi.h>
#include <string.h>

extern AppletPlugin common_applet_plugin_interface__;

__s32 Common_MInit(void)
{
    return EPDK_OK;
}

__mp *Common_MOpen(__u32 mid, __u32 mode)
{
    __common_t *common = NULL;
    common = (__common_t *)esMEMS_Malloc(0, sizeof(__common_t));

    if (common == NULL)
    {
        __err("common module malloc err");
        return NULL;
    }

    memset(common, 0, sizeof(__common_t));
    common->mid     = mid;
    common->iface   = &common_applet_plugin_interface__;
    return (__mp *)common;
}

__u32 Common_MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}

__u32 Common_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}

long Common_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    __common_t *com_ctr = (__common_t *)mp;

    switch (cmd)
    {
        case APPLET_CMD_START:
            (*com_ctr->iface->start)((Activity *)pbuffer);
            break;

        case APPLET_CMD_SUSPEND:
            (*com_ctr->iface->suspend)((Activity *)pbuffer);
            break;

        case APPLET_CMD_RESUME:
            (*com_ctr->iface->resume)((Activity *)pbuffer);
            break;

        case APPLET_CMD_BACKGROUND:
            (*com_ctr->iface->background)((Activity *)pbuffer);
            break;

        case APPLET_CMD_STOP:
            (*com_ctr->iface->stop)((Activity *)pbuffer);
            break;

        default:
            __msg("unknown command ");
            break;
    }

    return EPDK_OK;
}

__s32 Common_MClose(__mp *mp)
{
    __common_t *com_ctr = (__common_t *)mp;
    esMEMS_Mfree(0, com_ctr);
    return EPDK_OK;
}

__s32 Common_MExit(void)
{
    return EPDK_OK;
}

