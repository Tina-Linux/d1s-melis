/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                File system module
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : fsys_debug.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-8
* Descript: debug sub-system external interfaces.
* Update  : date                auther      ver     notes
*           2011-3-8 14:26:42   Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#ifndef __FSYS_DEBUG_H__
#define __FSYS_DEBUG_H__

//file system module debug enable or not
#define FSYS_DEBUG_ON  (0)

typedef struct debug_para_msg
{
    int             inuse;
    struct inode    *ino;
    int             pos;
    int             len;
} debug_para_msg_t;

extern debug_para_msg_t     debug_para;

#include "fs_cachedump.h"
#include "fs_logging.h"
#include "fs_timer.h"

#endif  /* __FSYS_DEBUG_H__ */

