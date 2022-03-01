/*
*********************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Development Kit
*                                       ebook file scan libary
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                              All Rights Reserved
*
* File    : ScanStack.h
* By      : Sunny
* Version : V1.00
* Date    : 2010-4-22 11:08:32
*********************************************************************************************************
*/

#ifndef __SCAN_STACK_H__
#define __SCAN_STACK_H__

#include <emodules/mod_defs.h>

typedef __hdle HSTACK;

HSTACK  ScanStackAlloc(void);
int     ScanStackPush(HSTACK hStack, const char *path);
char    *ScanStackPop(HSTACK hStack);
__bool  ScanStackEmpty(HSTACK hStack);
void    ScanStackFree(HSTACK hStack);

#endif      /* __SCAN_STACK_H__ */
