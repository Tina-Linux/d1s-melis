/*
*********************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Development Kit
*                                       ebook file scan libary
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                              All Rights Reserved
*
* File    : ScanStackPri.h
* By      : Sunny
* Version : V1.00
* Date    : 2010-4-22 11:14:07
*********************************************************************************************************
*/

#ifndef __SCAN_STACK_PRI_H__
#define __SCAN_STACK_PRI_H__

#include "ScanStack.h"

/* the length of scan stack grown one time */
#define STACK_GROWN_LEN     4096

/*
 * to store entry of scan stack.
 */
typedef struct ScanStackEntry
{
    __u32   RecLen;     /* total stack entry size                          */
    __u32   Prev;       /* prev stack entry offset within stack data areas */
    __u8    Path[1];    /* full path name, the length depend on Reclen, NOTE : Path must been located at the last   */
} ScanStackEntry_t;

/*
 * scan stack stucture.
 */
typedef struct ScanStack
{
    __u32   Alloc;      /* size of allocated stack data area*/
    __u32   Size;       /* size of useful stack data area   */
    __u32   Top;        /* offet of top scan stack          */
    __u8    *Data;      /* data area to store stack entry, data area size will be gown if necessary */
} ScanStack_t;


#endif      /* __SCAN_STACK_PRI_H__ */
