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
* By      : Andy.zhang
* Version : V1.00
*********************************************************************************************************
*/

#include "common_module_i.h"

const __module_mgsec_t Common_modinfo __attribute__((section(".magic"))) =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'}, //.magic
    0x01000000,                             //.version
    EMOD_TYPE_APPS,                         //.type
    0xF0000,                                //.heapaddr
    0x400,                                  //.heapsize
    {
        //.mif
        &Common_MInit,
        &Common_MExit,
        &Common_MOpen,
        &Common_MClose,
        &Common_MRead,
        &Common_MWrite,
        &Common_MIoctrl
    }
};
