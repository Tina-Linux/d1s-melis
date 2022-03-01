/*
**********************************************************************************************************************
*                                                   ePOS
*                                  the Easy Portable/Player Operation System
*
*                                (c) Copyright 2007-2008, Steven.ZGJ.China
*                                           All Rights Reserved
*
* File    : devman.c
* By      : steven.ZGJ
* Version : V1.00
**********************************************************************************************************************
*/

#include "dmsdev.h"
//#include <sys_fsys.h>
#include <log.h>
#include <kapi.h>

static __dev_devop_t        dmsdev_ops;
static __hdle               dmshandle = NULL;
static struct dmsdev        dms = {0};
extern __dev_classnode_t    devclass_nop;

int32_t dmsdev_init(void)
{
    dmshandle = esDEV_DevReg("DMS", "dms01", &dmsdev_ops, 0);
    if (dmshandle)
    {
        return EPDK_OK;
    }
    __err("dmsdev registered Error!");
    return EPDK_FAIL;
}

int32_t dmsdev_exit(void)
{
    return esDEV_DevUnreg(dmshandle);
}

static __hdle dmsdev_Open(void *open_arg, uint32_t mode)
{
    dms.inuse = 1;
    return (__hdle)&dms;
}

static int32_t dmsdev_Close(__hdle hDev)
{
    if (dms.inuse)
    {
        dms.inuse = 0;
        return EPDK_OK;
    }

    return EPDK_FAIL;
}

static uint32_t dmsdev_Read(void *pBuffer, uint32_t size, uint32_t n, __hdle hDev)
{
    return 0;
}

static uint32_t dmsdev_Write(const void *pBuffer, uint32_t size, uint32_t n, __hdle hDev)
{
    return 0;
}

static int32_t dmsdev_Ioctrl(__hdle hDev, uint32_t Cmd, int32_t Aux, void *pBuffer)
{
    switch (Cmd)
    {
        case DEV_IOC_USR_GET_DEVROOT:
        {
            *((__dev_classnode_t **)pBuffer) = &devclass_nop;
            return EPDK_OK;
        }

        case DEV_IOC_USR_GET_DEVITEM:
        {
            struct DmsDirInfo_dev   *itemInfo   = (struct DmsDirInfo_dev *)Aux;
            struct DmsNodeInfo_dev  *dinfo      = (struct DmsNodeInfo_dev *)pBuffer;
            __dev_classnode_t       *pClassnode = (__dev_classnode_t *)itemInfo->dir;
            int32_t                 i, res, idx = itemInfo->itemPos;

            /* 查找类顶点节点下的类节点，即一级目录 */
            if (itemInfo->dir == (__hdle)&devclass_nop)
            {
                for (i = 0, pClassnode = devclass_nop.next; pClassnode && i < idx; pClassnode = pClassnode->next, i ++);

                if (!pClassnode)
                {
                    res = EPDK_FAIL;
                }
                else
                {
                    /* 返回指向的设备节点链 */
                    dinfo->name     = pClassnode->name;
                    dinfo->hnode    = (__hdle)(pClassnode);
                    dinfo->type     = DEVFS_CLASS_TYPE;
                    res             = EPDK_OK;
                }
            }
            /* 查找设备节点，由设备类节点指向的节点链查找 */
            else
            {
                __dev_node_t *pNode = NULL;

                pNode = pClassnode->nodelist;
                for (i = 0; pNode && i < idx; pNode = pNode->next, i ++);
                if (!pNode)
                {
                    res = EPDK_FAIL;
                }
                else
                {
                    /* 返回找到的设备节点指针 */
                    dinfo->name     = pNode->name;
                    dinfo->hnode    = (__hdle)pNode;
                    dinfo->type     = DEVFS_NODE_TYPE;
                    res             = EPDK_OK;
                }
            }

            return res;
        }
        default:
            return EPDK_FAIL;
    }
}

static __dev_devop_t dmsdev_ops =
{
    dmsdev_Open,
    dmsdev_Close,
    dmsdev_Read,
    dmsdev_Write,
    dmsdev_Ioctrl
};
