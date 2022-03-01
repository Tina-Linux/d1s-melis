/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : magic.c
* By        : Andy.zhang
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2009-7-9 16:29:28  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "mod_init_i.h"

const __module_mgsec_t modinfo_init __attribute__((section(".magic"))) =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'}, //.magic
    0x01000000,                             //.version
    EMOD_TYPE_MOD_INIT,                     //.type
    0xF0000,                                //.heapaddr
    0x400,                                  //.heapsize
    {
        //.mif
        &INIT_MInit,
        &INIT_MExit,
        &INIT_MOpen,
        &INIT_MClose,
        &INIT_MRead,
        &INIT_MWrite,
        &INIT_MIoctrl
    }
};

