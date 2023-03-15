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
#include "video_dev.h"
const __module_mgsec_t modinfo __attribute__((section(".magic"))) =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'}, //.magic
    0x01000000,                             //.version
    EMOD_TYPE_MOD_VIDEO_PLAY,               //.type
    0xF0000,                                //.heapaddr
    0x400,                                  //.heapsize
    {
        //.mif
        &Video_Play_MInit,
        &Video_Play_MExit,
        &Video_Play_MOpen,
        &Video_Play_MClose,
        &Video_Play_MRead,
        &Video_Play_MWrite,
        &Video_Play_MIoctrl
    }
};

