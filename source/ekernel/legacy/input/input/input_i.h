/*
*********************************************************************************************************
*                                                    eMOD
*                                         the melis Operation System
*                                               input sub-system
*                                          input system core module
*
*                                    (c) Copyright 2010-2012, sunny China
*                                              All Rights Reserved
*
* File   : input_i.h
* Version: V1.0
* By     : Sunny
* Date   : 2010-7-11
* Note   : input system core module.
*********************************************************************************************************
*/
#ifndef     __INPUT_I_H__
#define     __INPUT_I_H__
#include    "../sys_input.h"

typedef struct __INPUT_SYS_MAN
{
    __input_ldev_t   *input_ldevs[INPUT_LOGICALDEV_MAX];/* input logic devices array    */
    __hdle           lock;                             /* lock for protect input system*/
} __input_sys_man_t;

#endif      /* __INPUT_I_H__ */

