#ifndef __COMMON_MODULE_I_H__
#define __COMMON_MODULE_I_H__

#include "apps.h"

typedef struct
{
    __u32           mid;
    AppletPlugin    *iface;
} __common_t;

__s32 Common_MInit(void);
__s32 Common_MExit(void);
__mp *Common_MOpen(__u32 mid, __u32 mode);
__s32 Common_MClose(__mp *mp);
__u32 Common_MRead(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 Common_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
long Common_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);

#endif /* __COMMON_MODULE_I_H__ */

