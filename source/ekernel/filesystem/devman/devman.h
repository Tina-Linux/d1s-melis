/*
*********************************************************************************************************
*                                                   ePOS
*                               the Easy Portable/Player Operation System
*                                              eMOD sub-system
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File   : devman_private.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/
#ifndef __DEV_H__
#define __DEV_H__
#include <typedef.h>
#include <sys_device.h>

#define  NODETYPE_CLASS         0x00000000
#define  NODETYPE_DEV           0x00000001

#define  DEV_STAT_INACTIVE      0x00
#define  DEV_STAT_ACTIVE        0x01

typedef struct __DEV_NODE       __dev_node_t;
typedef struct __DEV_CLASSNODE  __dev_classnode_t;

/* 设备类节点   */
struct __DEV_CLASSNODE
{
    __dev_classnode_t       *next;
    __dev_node_t            *nodelist;

    int32_t                 nodetype;
    uint8_t                 usedcnt;
    char                    *name;
};

/* 设备节点句柄，此句柄用于保存一个设备各种信息的数据结构   */
struct  __DEV_NODE
{
    __dev_node_t            *next;
    __dev_classnode_t       *classnode;         /* 类节点                                                       */

    int32_t                 attrib;
    char                    *name;
    char                    pletter;

    uint32_t                opentimes;          /* 句柄打开的次数                                               */
    uint32_t                status;             /* 节点状态：0表示正常，0xff表示为死节点（驱动程序代码已经卸载）*/

    __dev_devop_t           DevOp;              /* 保存设备操作句柄                                             */
    void                    *pOpenArg;

    __hdle                  sem;
    __hdle                  hDev;               /* 设备句柄                                                     */

};

/*
******************************************************
* 设备节点操作句柄，此句柄用于用户打开一个设备节点时返
* 回的句柄，它不同于设备句柄，它是设备节点的实例，而设
* 备句柄是设备的实例。
******************************************************
*/
typedef struct __DEV_DEV
{
    __dev_node_t            *devnode;           /* 指向设备节点                                                 */
    __hdle                  hDev;               /* 设备句柄，直接从__dev_node_t里copy，以方便使用               */
    __dev_devop_t           DevOp;              /* 设备操作入口，直接从__dev_node_t里copy，以方便使用           */
} __dev_dev_t;

#endif //#ifndef __DEV_H__
