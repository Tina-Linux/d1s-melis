/*
*********************************************************************************************************
*                                                   ePDK
*                                  the Easy Portable/Player Development Kits
*                                              elibs sub-system
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File   : libc.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/

#ifndef _LIBC_H_
#define _LIBC_H_

#include "./libc/elibs_stdlib.h"
#include "./libc/elibs_string.h"
#include "./libc/elibs_stdio.h"
#include "./libc/elibs_misc.h"

int libc_stdio_set_console(const char *device_name, int mode);
int libc_stdio_get_console(void);

#endif /* #ifndef _LIBC_H_ */
