/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                Module Loader
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : module.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-30
* Descript: module loader internal header file.
* Update  : date                auther      ver     notes
*           2011-3-30 16:14:15  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#ifndef __MODS_I_H__
#define __MODS_I_H__
#include <typedef.h>
#include <mod_defs.h>
#include <romldr.h>
#include "loader.h"
#include "exec.h"

typedef struct __MODS_MCB 
{
    __krnl_xcb_t    xcb;
    uint8_t         type;
    __mif_t         mif; 
    uint32_t        vmbitmap; 
} __module_mcb_t;

#endif //__MODS_I_H__

