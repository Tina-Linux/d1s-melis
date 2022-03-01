#ifndef _MOD_MIXTURE_I_H_
#define _MOD_MIXTURE_I_H_

#include <libc.h>
#include <typedef.h>
#include <mod_defs.h>

typedef struct
{
    __u8            mid;
    __u8            used;
} __mixture_mp_t;

__s32 Mod_Mixture_MInit(void);
__s32 Mod_Mixture_MExit(void);
__mp *Mod_Mixture_MOpen(__u32 mid, __u32 mode);
__s32 Mod_Mixture_MClose(__mp *mp);
__u32 Mod_Mixture_MRead(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 Mod_Mixture_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
long Mod_Mixture_MIoctrl(__mp *mp, __u32 cmd,  __s32 aux,void *pbuffer);

#endif

