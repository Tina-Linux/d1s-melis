/*
*********************************************************************************************************
*                                                    eMOD
*                                         the melis Operation System
*                                               input sub-system
*                                        input sub-system internal head file
*
*                                    (c) Copyright 2010-2012, sunny China
*                                              All Rights Reserved
*
* File   : sys_input_i.h
* Version: V1.0
* By     : Sunny
* Date   : 2010-6-29 15:42:59
* Note   : include all input sub-system resource,
*          all input sub-system intrenal module just need include this head file.
*********************************************************************************************************
*/

#ifndef __SYS_INPUT_I_H__
#define __SYS_INPUT_I_H__
#include    <typedef.h>
#include    <arch.h>

/* input core */
#include    "./input/input_bitops.h"
#include    "./input/input.h"

/* logical handle devices */
#include    "./keyboard/keyboarddev.h"
#include    "./mouse/mousedev.h"
#include    "./touchscreen/touchscreendev.h"
#endif  /* __SYS_INPUT_I_H__ */
