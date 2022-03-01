/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               nop driver Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : api_entry.c
* By      : kevin.z
* Version : v1.0
* Date    : 2010-11-9 11:26
* Descript:
* Update  : date                auther      ver     notes
*********************************************************************************************************
*/
#include <kconfig.h>
#include "orange_mod_api.h"

void GUI_StubNop(void)
{
    __err("The service not support! please select correct config in config.h!\n");
}

#include "orange_api_entry.cc"

