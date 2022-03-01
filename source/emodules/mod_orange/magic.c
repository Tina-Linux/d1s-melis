/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              module magic segment
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File    : magic.h
* By      : Steven.ZGJ
* Version : V1.00
*********************************************************************************************************
*/
#include "mod_orange_i.h"
#include <mod_defs.h>

__s32 ORANGE_MInit(void);
__s32 ORANGE_MExit(void);
__mp *ORANGE_MOpen(__u32 mid, __u32 mode);
__s32 ORANGE_MClose(__mp *mp);
__u32 ORANGE_MRead(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 ORANGE_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
long ORANGE_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);
const __module_mgsec_t modinfo __attribute__((section(".magic"))) =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'}, //.magic
    0x01000000,                             //.version
    EMOD_TYPE_MOD_ORANGE,                   //.type
    0x4F0000,                               //.heapaddr
    0x400,                                  //.heapsize
    {
        //.mif
        &ORANGE_MInit,
        &ORANGE_MExit,
        &ORANGE_MOpen,
        &ORANGE_MClose,
        &ORANGE_MRead,
        &ORANGE_MWrite,
        &ORANGE_MIoctrl
    }
};
