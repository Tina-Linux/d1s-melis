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