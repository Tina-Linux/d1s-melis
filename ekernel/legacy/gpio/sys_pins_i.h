/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Pin Manage Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : sys_pin_i.h
* By      : kevin.z
* Version : v2.0
* Date    : 2010-11-27 9:26
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-27 9:26     kevin.z     2.0     build the file;
*           2011-6-18 13:24:07  sunny       2.0     add pin int support.
*********************************************************************************************************
*/
#ifndef __SYS_PINS_I_H__
#define __SYS_PINS_I_H__
#include <typedef.h>
#include <ktype.h>
#include <kconfig.h>

__bool  PIN_CheckPinHdle(__hdle hPin);

//define pin int hander type
typedef struct __PIN_INT_HDLER
{
    void*           pEntry;             //handler entry
    void            *pArg;              //argument for dma hander
    uint32_t        flag;
} __pin_int_hdler_t;

//define pin item node
typedef struct __PIN_ITEM_NODE
{
    __hdle                  hPin;       //pin handle, request from pin csp
    __pin_int_hdler_t       hHdler;     //pin int handler
    struct __PIN_ITEM_NODE  *pNext;     //pointer for the pin list
    uint32_t                grpSize;
} __pin_item_node_t;

//define the pin manager
#if (defined (CONFIG_SOC_SUN3IW2) || defined (CONFIG_SOC_SUN8IW19)) || defined (CONFIG_SOC_SUN20IW1)
typedef struct __PIN_MANAGER
{
    __pin_item_node_t       *pList;     //list of pin resource
    int32_t                 nCnt;       //pin count
    int32_t                 nIrqNo;     //irq number of PIOC module
} __pin_manager_t;
#elif defined CONFIG_SOC_SUN3IW1
typedef struct __PIN_MANAGER
{
    __pin_item_node_t       *pList;     //list of pin resource
    int32_t                 nCnt;       //pin count
    int32_t                 nIrqNoD;    //irq number of PIOC module
    int32_t                 nIrqNoE;    //irq number of PIOC module
    int32_t                 nIrqNoF;    //irq number of PIOC module
} __pin_manager_t;
#endif

#endif //#ifndef __SYS_PINS_I_H__
