/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                              mod_mmp sub-system
*
*                                    (c) Copyright 2008-2009, Kevin.Z China
*                                              All Rights Reserved
*
* File    : magic.c
* By      : Kevin.Z
* Version : V1.00
*********************************************************************************************************
*/
#include <typedef.h>
#include <kapi.h>
#include <mod_defs.h>
__s32 MInit(void);
__s32 MExit(void);
__mp *MOpen(__u32 mid, __u32 mod);
__s32 MClose(__mp *mp);
__u32 MRead(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
long MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);


const __module_mgsec_t modinfo __attribute__((section(".magic"))) =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'}, //.magic
    0x01000000,                             //.version
    EMOD_TYPE_MOD_JPEG_DEC,                                 //EMOD_TYPE_
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
