/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              hello world sample
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File    : magic.c
* By      : Steven.ZGJ
* Version : V1.00
*********************************************************************************************************
*/
#include "drv_fm_i.h"

const __module_mgsec_t modinfo __attribute__((section(".magic"))) =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'}, //.magic
    0x01000000,                             //.version
    EMOD_TYPE_DRV_FM,                       //.type
    0xF0000,                                //.heapaddr
    0x400,                                  //.heapsize
    {
        //.mif
        &DRV_FM_MInit,
        &DRV_FM_MExit,
        &DRV_FM_MOpen,
        &DRV_FM_MClose,
        &DRV_FM_MRead,
        &DRV_FM_MWrite,
        &DRV_FM_MIoctrl
    }
};

