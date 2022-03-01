/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                 System Sevice
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : system_sevice_i.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-19
* Descript: system sevices internal header file.
* Update  : date                auther      ver     notes
*           2011-3-19 10:31:49  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __SYS_SEVICE_I_H__
#define __SYS_SEVICE_I_H__

#include <stdint.h>
#include <typedef.h>

#define  EPDK_KRNLMOD

//platform define for register
#define EPOS_PLATFORM
//register sevice enable or not
#define SVC_REG_EN  0


// structure for resource item
typedef struct __SYS_RESOURCE
{
    uint32_t    resource;
} __sys_resource_t;

// structure for resource manager
typedef struct __SYS_RESOURCE_MAN
{
    void        *FlagsGrp;    /* Flags group for resource management */
} __sys_resource_man_t;

#endif  /* __SYS_SEVICE_I_H__ */

