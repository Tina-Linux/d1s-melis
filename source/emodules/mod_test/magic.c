/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              module magic segment
*
*                               (c) Copyright 2010-2012, Sunny China
*                                           All Rights Reserved
*
* File    : magic.c
* By      : Sunny
* Version : V1.00
* Date    : 2011-4-30 14:31:49
*********************************************************************************************************
*/

#include "mod_test.h"

const __module_mgsec_t __attribute__((section(".magic"))) FlashModInfo =
{
    {'e', 'P', 'D', 'K', '.', 'm', 'o', 'd'}, //.magic
    0x01000000,                               //.version
    0x88/*EMOD_TYPE_MOD_SLIB*/,                //.type
    0xF0000,                                  //.heapaddr
    0x400,                                    //.heapsize
    {
        //.mif
        &ftest_MInit,
        &ftest_MExit,
        &ftest_MOpen,
        &ftest_MClose,
        &ftest_MRead,
        &ftest_MWrite,
        &ftest_MIoctrl
    }
};
