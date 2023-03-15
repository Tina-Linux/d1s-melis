/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "devman.h"
#include <kapi.h>
#include <string.h>
#include <port.h>
#include <log.h>

__hdle pDevSem      = NULL;           /* for lock parts table */

__dev_classnode_t   devclass_nop = {.name = DEV_CLASS_NOP, .nodetype = NODETYPE_CLASS,};

/*
**********************************************************************************************************************
*                                     SetDevAttr
*
* Description: set dev node attribute.
*
* Arguments  : class name
*              dev name
*              devnode
*
* Returns    : always EPDK_OK
**********************************************************************************************************************
*/
int32_t SetDevAttr(char *classname, char *devname, __dev_node_t *devnode)
{
    devnode->attrib     = 0;

    if (!strcmp(classname, DEV_CLASS_DMS))
    {
        devnode->attrib         |= DEV_NODE_ATTR_CTL | DEV_NODE_ATTR_PART | DEV_NODE_ATTR_FS | DEV_NODE_ATTR_SYNMNT;
        devnode->pletter        = PART_LETTER_DMS;
    }
    else if (!strcmp(classname, DEV_CLASS_DISK))
    {
        devnode->attrib         |= DEV_NODE_ATTR_RD | DEV_NODE_ATTR_WR | DEV_NODE_ATTR_BLK | DEV_NODE_ATTR_PART | DEV_NODE_ATTR_FS;
        devnode->pletter        = PART_LETTER_FREE;

        if (!strcmp(devname, DEV_NAME_RAMDISK))
        {
            devnode->pletter    = PART_LETTER_RAMDISK;
            devnode->attrib     |= DEV_NODE_ATTR_SYNMNT;
        }
        else if (!strcmp(devname, DEV_NAME_ROOTFS))
        {
            devnode->pletter    = PART_LETTER_ROOTFS;
            devnode->attrib     |= DEV_NODE_ATTR_SYNMNT;
        }
#ifdef CONFIG_SOC_SUN20IW1
        else if (!strcmp(devname, DEV_NAME_UDISK))
        {
            devnode->pletter    = PART_LETTER_UDISK;
            devnode->attrib     |= DEV_NODE_ATTR_SYNMNT;
        }
#endif
        else if (!strcmp(devname, DEV_NAME_SYSDATAFS))
        {
            devnode->pletter    = PART_LETTER_SYSDATA;
            devnode->attrib     |= DEV_NODE_ATTR_SYNMNT;
        }
        else if (!strcmp(devname, DEV_NAME_SYSBOOTFS))
        {
            devnode->pletter    = PART_LETTER_SYSBOOT;
            devnode->attrib     |= DEV_NODE_ATTR_SYNMNT;
        }
        else if (!strcmp(devname, DEV_NAME_BOOTFS))
        {
            devnode->pletter    = PART_LETTER_BOOTFS;
            devnode->attrib     |= DEV_NODE_ATTR_SYNMNT;
        }
        else if (!strncmp(devname, DEV_NAME_USERDISK, strlen(DEV_NAME_USERDISK)))
        {
            /* 方案用户自定义分区，分区名格式为"USERDISKxx",
             * 其中xx表示00~99，用于区分各方案用户的分区名.
             * 方案用户自定义分区在W~U范围内根据注册顺序倒序(W~U)分配.
             */
            devnode->pletter    = PART_LETTER_USER;
            devnode->attrib     |= DEV_NODE_ATTR_SYNMNT;
        }
        else if (!strcmp(devname, DEV_NAME_SDCARD0) || !strcmp(devname, DEV_NAME_SDCARD1))
        {
            devnode->attrib     |= DEV_NODE_ATTR_MOVABLE | DEV_NODE_ATTR_USBSLOT | DEV_NODE_ATTR_SYNMNT;
        }
        else if (!strcmp(devname, DEV_NAME_SCSI_DISK_00))
        {
            devnode->attrib     |= DEV_NODE_ATTR_MOVABLE | DEV_NODE_ATTR_USBSLOT;
        }
    }
    else
    {
        devnode->attrib         |= DEV_NODE_ATTR_RD | DEV_NODE_ATTR_WR | DEV_NODE_ATTR_CTL | DEV_NODE_ATTR_SYNMNT;
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                     esDEV_DevReg
*
* Description: module init function, this function will be called by system module management: MInstall,
*               user will never call it.
* Arguments  : void
*
* Returns    : if success return EPDK_OK
*              esle return EPDK_FAIL
**********************************************************************************************************************
*/
void* esDEV_DevReg(const char *classname, const char *name, const __dev_devop_t *pDevOp, void *pOpenArg)
{
    uint8_t             newclassflag = 0;
    __hdle              res;
    __dev_node_t        *devnode;
    __dev_node_t        *predevnode;
    uint8_t             err;

    __dev_classnode_t   *classnode      = &devclass_nop;
    __dev_classnode_t   *preclassnode   = &devclass_nop;

    esKRNL_SemPend(pDevSem, 0, &err);
    if (err)
    {
        __err("pend semphore faiure.");
        return NULL;
    }

    /* step 1: 寻找class类型名，如果没有找到，则新建一个类型名 */
    while (classnode)
    {
        if (strcmp(classname, classnode->name) == 0)
        {
            break;
        }
        preclassnode    = classnode;
        classnode       = classnode->next;
    }
    if (classnode == 0)
    {
        /*申请空间                              */
        classnode       = k_malloc(sizeof(__dev_classnode_t) + strlen(classname) + 1/*'/0'*/);
        if (classnode == 0)
        {
            __wrn("k_malloc fail!");
            res     = NULL;
            goto out;
        }
        /* 将classnode挂接到list中              */
        preclassnode->next      = classnode;
        classnode->next         = 0;

        /* copy name into classnode             */
        classnode->nodetype     = NODETYPE_CLASS;
        classnode->name         = (char *)((unsigned long)classnode + sizeof(__dev_classnode_t));
        strcpy(classnode->name, classname);

        /* nodelist is empty                    */
        classnode->nodelist     = 0;

        /* new class is created                 */
        newclassflag            = 1;
    }

    /* step 2: 设备管理list里是否有相同的设备名，如果有，则返回错误     */
    devnode         = classnode->nodelist;
    predevnode      = 0;

    while (devnode)
    {
        if (strcmp(devnode->name, name) == 0)
        {
            break;
        }
        predevnode  = devnode;
        devnode     = devnode->next;
    }

    if (devnode != 0)
    {
        __wrn("dev is already registered, or name conflict!");
        res = NULL;
        goto out;
    }

    /* step 3: 根据sizeofdrv创建设备节点    */
    devnode     = k_malloc(sizeof(__dev_node_t) + strlen(name) + 1/*'/0'*/);
    if (devnode == 0)
    {
        __wrn("k_malloc fail!");
        if (newclassflag)       /* 如果class为新建，释放，并脱离class list */
        {
            k_free(classnode);
            preclassnode->next = 0;
        }
        res = NULL;
        goto out;
    }

    //记录设备名
    devnode->name       = (char *)((unsigned long)devnode + sizeof(__dev_node_t));
    strcpy(devnode->name, name);
    //下一个为空
    devnode->next       = NULL;
    //记录class
    devnode->classnode  = classnode;
    //打开次数为0
    devnode->opentimes  = 0;
    //活节点
    devnode->status     = DEV_STAT_ACTIVE;
    //保存device操作
    devnode->DevOp      = *pDevOp;
    /* save open arg    */
    devnode->pOpenArg   = pOpenArg;

    devnode->hDev       = NULL;

    devnode->sem        = esKRNL_SemCreate(1);
    if (devnode->sem == NULL)
    {
        __err("create sem. failure");
        res = NULL;
        goto out;
    }

    /* 挂接到list中(link to rear of the list    */
    if (predevnode == 0)
    {
        classnode->nodelist = devnode;
    }
    else
    {
        predevnode->next    = devnode;
    }

    __inf("device \"%s\\%s\" is setup.", classname, name);

    /* step 4: 设置设备节点的属性 */
    SetDevAttr((char *)classname, (char *)name, devnode);

    /* step 5: 尝试注册分区和文件系统   */
    if (devnode->attrib & DEV_NODE_ATTR_PART)
    {
        esFSYS_mntparts((__hdle)devnode);
    }

    res = (__hdle)devnode;

out:
    esKRNL_SemPost(pDevSem);
    return res;
}

/*
**********************************************************************************************************************
*                                     esDEV_DevUnreg
*
* Description: module init function, this function will be called by system module management: MInstall,
*               user will never call it.
* Arguments  : void
*
* Returns    : if success return EPDK_OK
*              esle return EPDK_FAIL
**********************************************************************************************************************
*/

int32_t  esDEV_DevUnreg(__hdle hNode)
{
    int32_t         res;
    __dev_node_t    *pNode = (__dev_node_t *)hNode;
    uint8_t         err;

    esKRNL_SemPend(pDevSem, 0, &err);
    if (err)
    {
        __err("create semphore faile");
        return EPDK_FAIL;
    }

    /* 通知文件系统 */
    esFSYS_umntparts(hNode, 1);

    if (pNode->opentimes == 0)                      /* 如果node没有被任何用户使用，删除node，释放空间 */
    {
        __dev_node_t *p, **pp = &(pNode->classnode->nodelist);

        for (p = *pp; p && (pNode != p); pp = &(p->next), p = p->next);

        if (!p)
        {
            __wrn("BUG when unres dev: try to destroy a devnode not exsit in node list!");
            res = EPDK_FAIL;
            goto out;
        }
        *pp = pNode->next;

        esKRNL_SemDel(pNode->sem, 0, &err);
        k_free((void *)pNode); /* 释放node空间  */
        res = EPDK_OK;
        goto out;
    }
    else                                            /* 否则，需要将node的状态设置为死node   */
    {
        pNode->DevOp.Close(pNode->hDev);            /* 关闭设备句柄                         */
        pNode->status = DEV_STAT_INACTIVE;          /* 置设备节点为死节点                   */
    }

    res = EPDK_OK;

out:
    esKRNL_SemPost(pDevSem);
    return res;
}

/*
**********************************************************************************************************************
*                                     esDEV_Open
*
* Description: module init function, this function will be called by system module management: MInstall,
*               user will never call it.
* Arguments  : void
*
* Returns    : if success return EPDK_OK
*              esle return EPDK_FAIL
**********************************************************************************************************************
*/
int32_t esDEV_Lock(__hdle hNode)
{
    esKRNL_SemPend(((__dev_node_t *)hNode)->sem, 0, NULL);
    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                     esDEV_Close
*
* Description: 关闭设备
*
* Arguments  : void
*
* Returns    : if success return EPDK_OK
*              esle return EPDK_FAIL
**********************************************************************************************************************
*/
int32_t  esDEV_Unlock(__hdle hNode)
{
    esKRNL_SemPost(((__dev_node_t *)hNode)->sem);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                     esDEV_Open
*
* Description: module init function, this function will be called by system module management: MInstall,
*               user will never call it.
* Arguments  : void
*
* Returns    : if success return EPDK_OK
*              esle return EPDK_FAIL
**********************************************************************************************************************
*/
__hdle esDEV_Open(__hdle hNode, uint32_t Mode)
{
    __dev_dev_t     *pDev;
    __hdle          hDev;
    __dev_node_t    *pNode = (__dev_node_t *)hNode;

    if (pNode->status == DEV_STAT_INACTIVE)
    {
        __wrn("Device has been killed when unreg!");
        return NULL;
    }

    if (pNode->opentimes == 0)
    {
        hDev    = pNode->DevOp.Open(pNode->pOpenArg, Mode);/* 打开设备 */
        if (hDev == NULL)
        {
            __wrn("dev cannot be open!");
            return NULL;
        }
        pNode->hDev = hDev;                     /* 保存设备句柄 */
    }                                           /* 分配设备管理句柄空间 */

    pDev = (__dev_dev_t *)k_malloc(sizeof(__dev_dev_t));
    if (pDev == NULL)
    {
        __err("alloc structure failure");
        return NULL;
    }

    pDev->devnode   = pNode;                    /* 记录node指针 */
    pDev->DevOp     = pNode->DevOp;             /* 设备操作入口，直接从__dev_node_t里copy，以方便使用 */
    pDev->hDev      = pNode->hDev;              /* 设备句柄，直接从__dev_node_t里copy，以方便使用 */

    pNode->opentimes ++;                        /* 设备节点打开次数减1 */

    return pDev;                                /* 返回设备管理句柄 */
}
/*
**********************************************************************************************************************
*                                     esDEV_Close
*
* Description: 关闭设备
*
* Arguments  : void
*
* Returns    : if success return EPDK_OK
*              esle return EPDK_FAIL
**********************************************************************************************************************
*/
int32_t  esDEV_Close(__hdle hDev)
{
    __dev_dev_t     *pDev  = (__dev_dev_t *)hDev;
    __dev_node_t    *pNode = pDev->devnode;
    uint8_t         err;

    if (pNode->opentimes)
    {
        pNode->opentimes--;    /* 设备节点打开次数减1 */
    }

    if (pNode->opentimes == 0)  /* 如果设备节点已经没有人再使用 */
    {
        /* 如果设备节点没有用户使用，同时其又是死节点，做反注册的操作 */
        if (pNode->status == DEV_STAT_INACTIVE)
        {
            __dev_node_t *p, **pp   = &(pNode->classnode->nodelist);

            for (p = *pp; p && (pNode != p); pp = &(p->next), p = p->next);

            if (!p)
            {
                __wrn("BUG when close dev: try to destroy a devnode not exsit in node list!");
                return EPDK_FAIL;
            }

            *pp     = pNode->next;

            /* 释放设备节点占用的空间 */
            esKRNL_SemDel(pNode->sem, 0, &err);
            k_free((void *)pNode);
        }
        else /* 否则，已经没有用户打开此设备节点，同时此设备节点不是死节点 */
        {
            if (pDev->DevOp.Close(pNode->hDev) == EPDK_FAIL) /* 关闭设备句柄 */
            {
                __err("close device[%s] fail!", pNode->name);
            }
        }
    }

    k_free((void *)pDev);              /* 释放设备句柄占用的空间 */

    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                     esDEV_Read
*
* Description: 读
*
* Arguments  :  pdata       需要读出的数据指针
*               size        块的大小
*               n           块数
*               hDev        设备句柄
*
* Returns    : 实际读出的块数
*
**********************************************************************************************************************
*/
uint32_t esDEV_Read(void *pdata, uint32_t size, uint32_t n, __hdle hDev)
{
    __dev_node_t *pNode = ((__dev_dev_t *)hDev)->devnode;

    if (pNode->status == DEV_STAT_INACTIVE)
    {
        __wrn("Device has been killed when unreg!");
        return 0;
    }

    return (((__dev_dev_t *)hDev)->DevOp.Read)(pdata, size, n, ((__dev_dev_t *)hDev)->hDev);
}


/*
**********************************************************************************************************************
*                                     esDEV_Write
*
* Description: 写
*
* Arguments  :  pdata       需要写入的数据指针
*               size        块的大小
*               n           块数
*               hDev        设备句柄
*
* Returns    : 实际写入的块数
*
**********************************************************************************************************************
*/
uint32_t  esDEV_Write(const void *pdata, uint32_t size, uint32_t n, __hdle hDev)
{
    __dev_node_t *pNode = ((__dev_dev_t *)hDev)->devnode;


    if (pNode->status == DEV_STAT_INACTIVE)
    {
        __wrn("Device has been killed when unreg!");
        return 0;
    }

    return (((__dev_dev_t *)hDev)->DevOp.Write)(pdata, size, n, ((__dev_dev_t *)hDev)->hDev);
}


/*
**********************************************************************************************************************
*                                     esDEV_Ioctrl
*
* Description: 设备控制
*
* Arguments  : hDev         设备句柄
*              cmd          命令
*              aux          参数
*              pbuffer      数据buffer
*
* Returns    : device defined
*
**********************************************************************************************************************
*/
int32_t  esDEV_Ioctl(__hdle hDev, uint32_t cmd, long aux, void *pbuffer)
{
    __dev_node_t *pNode = ((__dev_dev_t *)hDev)->devnode;

    if (pNode->status == DEV_STAT_INACTIVE)
    {
        __wrn("Device has been killed when unreg!");
        return EPDK_FAIL;
    }

    if (IS_DEVIOCSYS(cmd))
    {
        switch (cmd)
        {
            case DEV_IOC_SYS_GET_CLSNAME:
                *((char **)pbuffer)         = pNode->classnode->name;
                return EPDK_OK;

            case DEV_IOC_SYS_GET_DEVNAME:
                *((char **)pbuffer)         = pNode->name;
                return EPDK_OK;

            case DEV_IOC_SYS_GET_ATTRIB:
                *((int32_t *)pbuffer)       = pNode->attrib;
                return EPDK_OK;

            case DEV_IOC_SYS_GET_LETTER:
                *((char *)pbuffer)          = pNode->pletter;
                return EPDK_OK;

            case DEV_IOC_SYS_GET_OPENARGS:
                *((void **)pbuffer)         = pNode->pOpenArg;
                return EPDK_OK;

            default:
                return EPDK_FAIL;
        }
    }
    else
        return (((__dev_dev_t *)hDev)->DevOp.Ioctl)(((__dev_dev_t *)hDev)->hDev, cmd, aux, pbuffer);
}

/*
**********************************************************************************************************************
*
*             FS_Init
*
*  Description:
*  API function. Start the file system.
*
*  Parameters:
*  None.
*
*  Return value:
*  ==0         - File system has been started.
*  !=0         - An error has occured.
**********************************************************************************************************************
*/
int32_t dev_init(void)
{
    pDevSem = esKRNL_SemCreate(1);
    if (pDevSem == NULL)
    {
        __err("create sem failure.");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*
*             FS_Exit
*
*  Description:
*  API function. Stop the file system.
*
*  Parameters:
*  None.
*
*  Return value:
*  ==0         - File system has been stopped.
*  !=0         - An error has occured.
**********************************************************************************************************************
*/
int32_t dev_exit(void)
{
    uint32_t    x = EPDK_OK;

   uint8_t     err = 0;

    if (pDevSem)
    {
        esKRNL_SemDel(pDevSem, 0, &err);
        pDevSem = NULL;
    }

    return  x;
}