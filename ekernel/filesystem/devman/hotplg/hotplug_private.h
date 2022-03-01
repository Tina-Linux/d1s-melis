/*
*********************************************************************************************************
*                                                   ePOS
*                               the Easy Portable/Player Operation System
*                                              eMOD sub-system
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File   : hotplug_private.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/
#ifndef _HOTPLUG_PRIVATE_H_
#define _HOTPLUG_PRIVATE_H_
#include <kapi.h>
//#include "sys_dev_i.h"

typedef enum enum_DEV_HOTPLUG_MSGID
{
    DEV_HOTPLUG_MSG_PLUGIN = 0,
    DEV_HOTPLUG_MSG_PLUGOUT
} __dev_hotplug_msgid_t;

typedef struct DEV_DRV
{
    __u32                   mid;
    char                   *name;
    struct DEV_DRV         *brother;
    struct DEV_DRV         *child;

} __dev_drv_t;

typedef struct tag__MSG_HOTPLUG_ARG
{
    __u8      id;
    char      plgmsg[128];
    __u32     devno;
    void     *p_arg;

} __msg_hotplug_arg_t;



#endif //#ifndef _HOTPLUG_PRIVATE_H_

