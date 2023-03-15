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