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

#include "mod_desktop_i.h"

const __module_mgsec_t modinfo_desktop __attribute__((section(".magic"))) =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'}, //.magic
    0x01000000,                             //.version
    EMOD_TYPE_MOD_DESKTOP,                  //.type
    0xF0000,                                //.heapaddr
    0x400,                                  //.heapsize
    {
        //.mif
        &DESKTOP_MInit,
        &DESKTOP_MExit,
        &DESKTOP_MOpen,
        &DESKTOP_MClose,
        &DESKTOP_MRead,
        &DESKTOP_MWrite,
        &DESKTOP_MIoctrl
    }
};

