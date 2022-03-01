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
#include "mod_mixture_i.h"
#include <mod_defs.h>

const __module_mgsec_t modinfo __attribute__((section(".magic"))) =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'}, //.magic
    0x01000000,                             //.version
    EMDO_TYPE_MOD_MIXTURE,               	//.type
    0xF0000,                                //.heapaddr
    0x400,                                  //.heapsize
    {
        //.mif
        &Mod_Mixture_MInit,
        &Mod_Mixture_MExit,
        &Mod_Mixture_MOpen,
        &Mod_Mixture_MClose,
        &Mod_Mixture_MRead,
        &Mod_Mixture_MWrite,
        &Mod_Mixture_MIoctrl
    }
};