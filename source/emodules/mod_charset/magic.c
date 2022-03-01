/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              nop driver module
*
*                                     (c) Copyright 2008-2010, Kevin China
*                                             All Rights Reserved
*
* File    : magic.c
* By      : kevin.z
* Version : V1.00
* Date    : 2010-10-25 16:26
*********************************************************************************************************
*/
#include "kapi.h"
#include <mod_defs.h>

extern __s32 _CHS_Init(void);
extern void  _CHS_Exit(void);

__s32 MInit(void)
{
    _CHS_Init();
    return EPDK_OK;
}

__s32 MExit(void)
{
    _CHS_Exit();
    return EPDK_OK;
}


__mp *MOpen(__u32 mid, __u32 mode)
{
    return (__mp *)0;
}

__s32 MClose(__mp *mp)
{
    return EPDK_OK;
}

__u32 MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}

__u32 MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}

long MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    return EPDK_OK;
}



const __module_mgsec_t modinfo __attribute__((section(".magic"))) =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'}, //.magic
    0x01000000,                             //.version
    EMOD_TYPE_MOD_CHARSET,                  //.type
    0xF0000,                                //.heapaddr
    0x400,                                  //.heapsize
    {
        //.mif
        &MInit,
        &MExit,
        &MOpen,
        &MClose,
        &MRead,
        &MWrite,
        &MIoctrl
    }
};
