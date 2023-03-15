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
#include "hotplug_private.h"
#include <emodules/mod_defs.h>
#include <kapi.h>
#include <sys_device.h>
#include <string.h>
#include <port.h>
#include <kmsg.h>
#include <log.h>

static __dev_drv_t DRVRoot =
{
    0, "drv", 0, 0
};

/*
**********************************************************************************************************************
*
*Function Name : esDEV_Plugin
*
*Description : "/usbhost core/usb disk0"
*
*Input :
*
*Output :
*
*Others : None at present.
*
**********************************************************************************************************************
*/
__s32 esDEV_Insmod(char *modfile, __u32 devno, void *p_arg)
{
    __u8                mid;
    char                drvfile[255] = {0};
    __dev_drv_t         *father;
    __dev_drv_t         *brother;
    __dev_drv_t         *me;
    __mp                *hDrv;
    char                *fathername;
    char                *myname;

    __log("modfile = %s.", modfile);

    strncpy(drvfile, modfile, strlen(modfile) + 1);
    if (*modfile == '\\')                           /* 去掉开始的'\' */
    {
        modfile++;
    }

    father = &DRVRoot;                              /* 定位父节点    */
    while (1)
    {
        fathername = modfile;
        modfile = (char *)strchr(fathername, '\\');
        if (modfile == NULL)                           /* 找到了最后的代表设备名的消息  */
        {
            myname = fathername++;
            break;
        }
        modfile++;

        if (father != &DRVRoot)
        {
            father = father->child;
        }

        while (1)
        {
            if (father == NULL)
            {
                __err("driver cannot be install, because its father driver not installed!");
                return EPDK_FAIL;
            }
            if (strncmp(father->name, fathername, ((__u64)modfile - (__u64)fathername) - 1) == 0)
            {
                break;                              /* 如果找到fatherlist */
            }
            father = father->brother;
        }
    }

    me = NULL;
    brother = father->child;                        /* 判断驱动是否被安装 */
    while (brother)
    {
        if (strcmp(brother->name, myname) == 0)
        {
            me = brother;
            break;
        }
        brother = brother->brother;
    }
    if (!me)
    {
#if defined CONFIG_UDISK_UPDATE_MODS
        __s32 i;
        char tempdrvfile[255] = {0};
        for (i = 252; i >= 0; i--)
        {
            tempdrvfile[i + 2] = drvfile[i];
        }
        tempdrvfile[0] = 'e';
        tempdrvfile[1] = ':';
        mid = esMODS_MInstall(tempdrvfile, 0);

        if (mid == 0)
#endif
            mid = esMODS_MInstall(drvfile, 0);

        if (mid == 0)
        {
            __s32 i;
            for (i = 252; i >= 0; i--)
            {
                drvfile[i + 2] = drvfile[i];
            }
            drvfile[0] = 'd';
            drvfile[1] = ':';
            mid = esMODS_MInstall(drvfile, 0);
            if (mid == 0)
            {
                __err("driver install fail!");
                return EPDK_FAIL;
            }
        }
        me = (__dev_drv_t *)k_malloc(sizeof(__dev_drv_t) + strlen(myname) + 1); /*===创建节点并挂接==*/
        if (me == NULL)
        {
            __err("driver install fail!");
            return EPDK_FAIL;
        }

        me->mid = mid;
        me->name = (char *)((__u64)me + sizeof(__dev_drv_t));
        strncpy(me->name, myname, strlen(myname) + 1);

        me->child       = NULL;                        /* 将me挂接到father之下 */
        me->brother     = father->child;
        father->child   = me;
    }
    else
    {
        __log("already plugin.");
    }

    hDrv = esMODS_MOpen(me->mid, 0);
    if (!hDrv)
    {
        __err("driver cannot be openned!");
        return EPDK_FAIL;
    }

    __log("pre- plugin module");
    esMODS_MIoctrl(hDrv, DRV_CMD_PLUGIN, devno, p_arg);
    __log("post plugin module");
    esMODS_MClose(hDrv);

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               _Plugout
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 esDEV_Unimod(char *modfile, __u32 devno)
{
    __dev_drv_t     *father;
    __dev_drv_t     *brother;
    __dev_drv_t     *brobro;
    __dev_drv_t     *me;
    char            *fathername;
    char            *myname;
    char             drvfile[255] = {0};

    __log("modfile = %s.", modfile);

    /*=====获取驱动接点=============================================*/
    if (*modfile == '\\')                           /* 去掉开始的'/'                                                */
    {
        modfile++;
    }

    //if(esSVS_GetRegKey(driver, drvfile) == 0)                                    /* 判断驱动文件是否已经注册在系统中 */
    //{
    //    __wrn("Driver file not installed!");
    //    return EPDK_FAIL;
    //}
    strncpy(drvfile, modfile, strlen(modfile) + 1);
    /*=====获取驱动挂接点===========================================*/
    if (*modfile == '\\')                            /* 去掉开始的'\'                                                */
    {
        modfile++;
    }

    father = &DRVRoot;                              /* 定位父节点                                                   */
    while (1)
    {
        fathername = modfile;
        modfile = (char *)strchr(fathername, '\\');
        if (modfile == 0)                           /* 找到了最后的代表设备名的消息                                 */
        {
            myname = fathername++;
            break;
        }
        modfile++;

        if (father != &DRVRoot)
        {
            father = father->child;
        }

        while (1)
        {
            if (father == 0)
            {
                __err("driver cannot be uninstall, because its father driver not installed!");
                return EPDK_FAIL;
            }
            if (strncmp(father->name, fathername, ((__u64)modfile - (__u64)fathername) - 1) == 0)
            {
                break;                              /* 如果找到fatherlist                                           */
            }
            father = father->brother;
        }
    }

    me = NULL;
    brother = father->child;                        /* 判断驱动是否被安装                                           */
    brobro = (__dev_drv_t *)&brother;               /* save pre brother of me                                       */
    while (brother)
    {
        if (strcmp(brother->name, myname) == 0)
        {
            me = brother;
            break;
            //__wrn("driver already installed!");
            //return EPDK_FAIL;
        }
        brobro = brother;
        brother = brother->brother;
    }

    /* send plug out command to driver              */
    if (me)
    {
        __mp *hDrv;
        hDrv = esMODS_MOpen(me->mid, 0);
        esMODS_MIoctrl(hDrv, DRV_CMD_PLUGOUT, devno, 0);
        if (esMODS_MIoctrl(hDrv, DRV_CMD_GET_STATUS, 0, 0) == DRV_STA_FREE)
        {
            /*driver is k_free, so we uninstall the driver to k_free memory                                             */
            esMODS_MClose(hDrv);
            esMODS_MUninstall(me->mid);
            /* unlink me from driver tree               */
            if (father->child == me)
            {
                father->child = me->brother;
            }
            else
            {
                brobro->brother = me->brother;
            }

            k_free(me);
        }
        else
        {
            esMODS_MClose(hDrv);
        }
    }
    else
    {
        __err("driver not installed!");
    }
    return EPDK_FAIL;
}

/*
**********************************************************************************************************************
*                                               _Hotplug
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static void _Hotplug(int64_t u_arg, int64_t s_arg)
{
    __msg_hotplug_arg_t *msg_arg = (__msg_hotplug_arg_t *)u_arg;

    if (msg_arg == NULL)
    {
        __err("error parameter.");
        return;
    }

    switch (msg_arg->id)
    {
        case DEV_HOTPLUG_MSG_PLUGIN:
            esDEV_Insmod(msg_arg->plgmsg, msg_arg->devno, msg_arg->p_arg);
            break;
        case DEV_HOTPLUG_MSG_PLUGOUT:
            esDEV_Unimod(msg_arg->plgmsg, msg_arg->devno);
            break;
        default:
            __err("not support id %d.", msg_arg->id);
            break;
    }

    k_free((void *)msg_arg);

    return;
}


/* global */
/*
**********************************************************************************************************************
*
*Function Name : esDEV_Plugin
*
*Description :
*
*Input :
*
*Output :
*
*Others : None at present.
*
**********************************************************************************************************************
*/
int32_t esDEV_Plugin(char *plgmsg, uint32_t devno, void *p_arg, uint8_t prio)
{
    __epos_kmsg_t         *msg;
    __msg_hotplug_arg_t *msg_arg;

    __log("plgmsg = %s.", plgmsg);
    /* msg must alloc memory by balloc function     */
    msg = (__epos_kmsg_t *)k_malloc(sizeof(__epos_kmsg_t));
    if (!msg)
    {
        __err("balloc fail!");
        return EPDK_FAIL;
    }

    msg_arg = (__msg_hotplug_arg_t *)k_malloc(sizeof(__msg_hotplug_arg_t) + sizeof(__dev_dskinfo_t) + sizeof(__dev_blkinfo_t) * 2);
    if (!msg_arg)
    {
        __err("balloc fail!");
        k_free((void *)msg);
        return EPDK_FAIL;
    }

    memset(msg, 0x00, sizeof(__epos_kmsg_t));
    memset(msg_arg, 0x00, sizeof(__msg_hotplug_arg_t) + sizeof(__dev_dskinfo_t) + sizeof(__dev_blkinfo_t) * 2);

    msg->target       = KMSG_TGT_CALLBACK;
    msg->message      = 0;
    msg->prio         = prio;
    msg->l.cb         = _Hotplug;
    msg->h.cb_u_arg   = (__u64)msg_arg;

    msg_arg->id       = DEV_HOTPLUG_MSG_PLUGIN;
    strcpy(msg_arg->plgmsg, plgmsg);
    msg_arg->devno    = devno;
    msg_arg->p_arg    = p_arg;

    esKSRV_SendMsgEx((void *)msg);

    return EPDK_OK;
}
/*
**********************************************************************************************************************
*
*Function Name : esDEV_Plugout
*
*Description :
*
*Input :
*
*Output :
*
*Others : None at present.
*
**********************************************************************************************************************
*/
int32_t esDEV_Plugout(char *plgmsg, uint32_t devno)
{
    __epos_kmsg_t         *msg;
    __msg_hotplug_arg_t *msg_arg;

    __log("plgmsg = %s.", plgmsg);
    /* msg must alloc memory by balloc function     */
    msg = (__epos_kmsg_t *)k_malloc(sizeof(__epos_kmsg_t));
    if (!msg)
    {
        __err("balloc fail!");
        return EPDK_FAIL;
    }

    msg_arg = (__msg_hotplug_arg_t *)k_malloc(sizeof(__msg_hotplug_arg_t));
    if (!msg_arg)
    {
        __err("balloc fail!");
        k_free((void *)msg);
        return EPDK_FAIL;
    }

    memset(msg, 0x00, sizeof(__epos_kmsg_t));
    memset(msg_arg, 0x00, sizeof(__msg_hotplug_arg_t));

    msg->target       = KMSG_TGT_CALLBACK;
    msg->message      = 0;
    msg->prio         = KMSG_PRIO_LOW;
    msg->l.cb         = _Hotplug;
    msg->h.cb_u_arg   = (__u64)msg_arg;
    msg_arg->id       = DEV_HOTPLUG_MSG_PLUGOUT;
    strcpy(msg_arg->plgmsg, plgmsg);
    msg_arg->devno    = devno;

    esKSRV_SendMsgEx((void *)msg);

    return EPDK_OK;
}