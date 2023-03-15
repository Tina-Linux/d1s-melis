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
#include "errno.h"
#include "part.h"
#include "fsys_debug.h"
#include "fsys_libs.h"
#include <log.h>

/* 自由分配的分区列表 */
__fsys_part_t   *pPartFTbl[FSYS_MAX_FPARTS] = {0};

/* 系统内部固定分配的分区列表 */
#ifdef CONFIG_SOC_SUN20IW1
__fix_part_t    pPartXTbl[FSYS_MAX_XPARTS]  = { {'A', 0}, {'B', 0}, {'C', 0}, {'D', 0}, {'E', 0}, {'W', 0}, {'X', 0}, {'Y', 0}, {'Z', 0} };
#else
__fix_part_t    pPartXTbl[FSYS_MAX_XPARTS]  = { {'A', 0}, {'B', 0}, {'C', 0}, {'D', 0}, {'W', 0}, {'X', 0}, {'Y', 0}, {'Z', 0} };
#endif

/* 方案用户自定义分区的分区列表 */
__fix_part_t    pPartUTbl[FSYS_MAX_UPARTS]  = { {'U', 0}, {'V', 0} };

static __hdle   mnt_parts_tid   = NULL;
static __hdle   pContolSem      = NULL;         /* 分区挂载线程唤醒、睡眠控制锁 */
__hdle          CurhNode        = NULL;
__hdle          pPartSem        = NULL;         /* for lock parts table */
__fsys_pd_t     *pPDRoot        = NULL;

int32_t         test_and_freeze_partfs(__hdle h_sb);
void            unfreezepart(__hdle h_sb);
static int32_t  __mount_parts(__hdle hNode);


static void wakeup_mnt_thread(void)
{
    esKRNL_SemPost(pContolSem);
}

static void mnt_parts_task(void *p_arg)
{
    while (1)
    {
        /* 挂载分区线程主动睡眠 */
        esKRNL_SemPend(pContolSem, 0, NULL);

        /* 收到分区挂载通知，线程被唤醒，开始挂载当前设备节点 */
        __mount_parts(CurhNode);
    }
}

int32_t fsys_vpart_init(void)
{
    uint32_t    err = 0;

    /************************************************************/
    /*  创建线程唤醒、睡眠控制信号量，                          */
    /*  分区挂载线程创建后处于睡眠状态，                        */
    /*  只有挂载分区时才主动唤醒                                */
    /************************************************************/
    pContolSem  = esKRNL_SemCreate(0);
    err         = !pContolSem;

    /* 分区操作互斥锁 */
    pPartSem    = esKRNL_SemCreate(1);
    err         |= !pPartSem;

    if (err)
    {
        __err("fail to create sem.");
        return EPDK_FAIL;
    }

    /* 初始化时没有挂载设备节点 */
    CurhNode = NULL;

    /************************************************************/
    /*  创建分区挂载线程                                        */
    /************************************************************/
    mnt_parts_tid = awos_task_create("mnt_task", mnt_parts_task, NULL, 0x4000, CONFIG_RT_TIMER_THREAD_PRIO - 1, 10);

    return mnt_parts_tid ? EPDK_OK : EPDK_FAIL;
}

/*
******************************************************************
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
******************************************************************
*/

int32_t fsys_vpart_exit(void)
{
    awos_task_delete(mnt_parts_tid);
    return EPDK_OK;
}

/*
******************************************************************
*
*             fsys_regist_part
*
*  Description:
*   注册块设备到文件系统中
*
*  Parameters:
*   pFullName   - Fully qualified name.
*   pFilename   - Address of a pointer, which is modified to point
*                 to the file name part of pFullName.
*
*  Return value:
*   <0          - Unable to find the device.
*   >=0         - Index of the device in the device information
*                 table.
******************************************************************
*/
int32_t esFSYS_pdreg(__hdle hPD)
{
    int32_t         res;
    __fsys_pd_t     *p, *pd = (__fsys_pd_t *)hPD;

    esKRNL_SemPend(pPartSem, 0, NULL);

    /* 判断文件系统是否已经被注册 */
    for (p = pPDRoot; p; p = p->next)
    {
        if (strcmp(p->name, pd->name) == 0)
        {
            fs_log_info("pd already registered!");
            res = EPDK_FAIL;
            goto out;
        }
    }

    /* 将驱动置为0个user                                        */
    pd->nUsr    = 0;
    /* 将驱动置为有效驱动                                       */
    pd->status  = PD_STAT_ACTIVE;
    /* 将驱动挂接到pPDRoot上                                    */
    pd->next    = pPDRoot;
    pPDRoot     = pd;
    res         = EPDK_OK;

out:
    esKRNL_SemPost(pPartSem);

    return res;
}
/*
******************************************************************
*
*             fsys_regist_part
*
*  Description:
*   拔除块设备
*
*  Parameters:
*   pFullName   - Fully qualified name.
*   pFilename   - Address of a pointer, which is modified to point
*                 to the file name part of pFullName.
*
*  Return value:
*   <0          - Unable to find the device.
*   >=0         - Index of the device in the device information
*                 table.
******************************************************************
*/
int32_t esFSYS_pdunreg(__hdle hPD)
{
    uint32_t        res;
    __fsys_pd_t     *pd = (__fsys_pd_t *)hPD;
    __fsys_pd_t     *p, **pp = &pPDRoot;

    esKRNL_SemPend(pPartSem, 0, NULL);

    if (pd->nUsr)
    {
        fs_log_info("part driver is used by someone!");
        res = EPDK_FAIL;
        goto out;
    }

    for (p = *pp; p && (pd != p); pp = &(p->next), p = p->next);

    if (!p)
    {
        fs_log_info("BUG when unres pd: try to destroy a fs not exsit in pd list!");
        res = EPDK_FAIL;
        goto out;
    }
    pd->status = PD_STAT_INACTIVE;
    *pp = pd->next;
    res = EPDK_OK;

out:
    esKRNL_SemPost(pPartSem);

    return res;
}

/*
******************************************************************
*
*             esFSYS_mntparts
*
*  Description:
*   注册块设备到文件系统中
*
*  Parameters:
*   hNode       - 设备节点句柄.
*
*  Return value:
*   EPDK_OK     - registered ok
*   EPDK_FAIL   - fail.
******************************************************************
*/
__attribute__((weak)) int32_t blockdev_unregister_callback(int32_t u_arg, int32_t s_arg) {
	//do nothing
	return 0;
}
__attribute__((weak)) int32_t blockdev_register_callback(int32_t u_arg, int32_t s_arg) {
	//do nothing
	return 0;
}



static int32_t __mount_parts(__hdle hNode)
{
    int32_t         i, j, res;
    int32_t         nPart;
    __hdle          hDev = NULL;
    __fsys_pd_t     *pPD = NULL;
    uint8_t         *classname  = NULL, *devname = NULL, dletter = 0;
    void            *openargs   = NULL;
    int32_t         devattrib   = 0;
    uint32_t        last_lun    = 0;

    last_lun    = 0;
    esKRNL_SemPend(pPartSem, 0, NULL);
    if (NULL == hNode)
    {
        res = EPDK_FAIL;
        goto out;
    }

    /************************************************************/
    /* 识别设备上的分区个数                                     */
    /************************************************************/
    hDev    = esDEV_Open(hNode, 0);
    if (!hDev)
    {
        res = EPDK_FAIL;
        goto out;
    }

    esDEV_Ioctl(hDev, DEV_IOC_SYS_GET_CLSNAME,  0,  &classname) ;
    esDEV_Ioctl(hDev, DEV_IOC_SYS_GET_DEVNAME,  0,  &devname)   ;
    esDEV_Ioctl(hDev, DEV_IOC_SYS_GET_ATTRIB,   0,  &devattrib) ;
    esDEV_Ioctl(hDev, DEV_IOC_SYS_GET_LETTER,   0,  &dletter)   ;

    __log("classname=%s",   classname);
    __log("devname=%s",     devname);

    if (0 == strncmp(DEV_CLASS_DISK,    classname, strlen(DEV_CLASS_DISK)) &&
        ( 0 == strncmp(DEV_NAME_SCSI_DISK_00,   devname, strlen(DEV_NAME_SCSI_DISK_00)) ||
          0 == strncmp(DEV_NAME_USB_CDROM_00,   devname, strlen(DEV_NAME_USB_CDROM_00)) ||
          0 == strncmp(DEV_NAME_SDCARD,         devname, strlen(DEV_NAME_SDCARD))
        ))
    {
        esDEV_Ioctl(hDev, DEV_IOC_SYS_GET_OPENARGS, 0, &openargs);
        if (openargs)
        {
            last_lun = (uint32_t)(long)(openargs);
        }
        else
        {
            last_lun = 1;
        }
    }
    else//如果是其它类型设备，则默认是最后一个盘
    {
        last_lun = 1;
    }

    __inf("last_lun=%d",    last_lun);

    if (1 == last_lun)
    {
        __log("find last lun.");
    }

    for (pPD = pPDRoot, nPart = 0; pPD && !nPart; pPD = pPD->next)
    {
        if (pPD->Ops.identify && pPD->status == PD_STAT_ACTIVE)
        {
            /* 由于要调用identify函数,需要将pd的使用者加一  */
            pPD->nUsr++;

            nPart   = pPD->Ops.identify(hDev);
            /* 由于调用identify函数结束,需要将pd的使用者减一 */
            pPD->nUsr--;
        }

        if (nPart)
        {
            break;
        }
    }

    esDEV_Close(hDev);

    if (!pPD)
    {
        __err("cant find pd.");
        res = EPDK_FAIL;
        goto out;
    }

    __log("nPart = %d.", nPart);

    /************************************************************/
    /* 装载分区                                                 */
    /************************************************************/
    for (i = 0; i < nPart; i++)
    {
        __fsys_part_t   *pPart;
        int32_t         k;
        uint8_t         buf[12] = {0};

        /********************************************************/
        /* 生成part数据结构                              */
        /********************************************************/
        /* 获取分区数据结构内存 */
        pPart = calloc(1, sizeof(__fsys_part_t));
        if (!pPart)
        {
            __log("malloc fail!");
            fs_err  = -ENOMEM;
            res     = EPDK_FAIL;
            goto out;
        }

        pPart->last_lun     = last_lun;

        /* 打开设备，获得句柄 */
        hDev    = esDEV_Open(hNode, 0);
        if (!hDev)
        {
            /* 释放分配的内存空间 */
            free(pPart);

            __log("device cannot be opened!");
            fs_err  = -ENODEV;
            res     = EPDK_FAIL;
            goto out;
        }

        /* 设置分区名 */
        strncpy(pPart->dname, devname, MAX_PART_NAME_LEN);

        k   = strlen(pPart->dname);
        melis_itoa(i, buf);

        strncpy(&pPart->dname[k], buf, MAX_PART_NAME_LEN - k - 1);

        /* 记录设备句柄，节点和分区号 */
        pPart->hNode        = hNode;
        pPart->hDev         = hDev;
        pPart->Unit         = i;
        pPart->error        = 0;
        pPart->attr         = ((devattrib & DEV_NODE_ATTR_RD) ? FSYS_PARTATTR_DEVR : 0) | ((devattrib & DEV_NODE_ATTR_WR) ? FSYS_PARTATTR_DEVW : 0);
        pPart->updateflag   = EPDK_FALSE;

        /********************************************************/
        /* 挂接分区驱动                                         */
        /********************************************************/
        /* 由于要调用mount函数,需要将pd使用者加1 */
        pPD->nUsr++;

        /* mount分区 */
        if (pPD->Ops.mount(pPart) == EPDK_FAIL)
        {
            /* 由于mount失败,需要将pd使用者减1 */
            if (pPD->nUsr)
            {
                pPD->nUsr--;
            }

            /* 释放分配的内存空间 */
            free(pPart);
            esDEV_Close(hDev);
            __log("part mount fail!");
            res     = EPDK_FAIL;
            goto out;
        }
        pPart->pPD  = pPD;

        /********************************************************/
        /* 将分区挂接到分区表中                                 */
        /********************************************************/
        pPart->letter   = 0xff;

        if (dletter == PART_LETTER_FREE) /* 自由分配的分区 */
        {
            for (j = 0; j < FSYS_MAX_FPARTS; j++)
            {
                if (!pPartFTbl[j])
                {
                    pPart->letter   = PART_LETTER_FREESTART + j;
                    pPartFTbl[j]    = pPart;
                    __log("%s: \"%s\\%s\" is linked to symbel \"%c\".", pPart->pPD->name, classname, pPart->dname, pPart->letter);
                    break;
                }
            }

            if (j == FSYS_MAX_FPARTS)
            {
                __log("too many parts!");
            }
        }
        else if (dletter == PART_LETTER_USER) /* 方案用户自定义的分区 */
        {
            for (j = FSYS_MAX_UPARTS - 1; j >= 0; j--)
            {
                if (!pPartUTbl[j].part)
                {
                    pPart->letter       = pPartUTbl[j].letter;
                    pPartUTbl[j].part   = pPart;
                    __log("%s: \"%s\\%s\" is linked to symbel \"%c\".", pPart->pPD->name, classname, pPart->dname, pPart->letter);
                    break;
                }
            }
            if (j < 0)
            {
                __log("too many user define parts!");
            }
        }
        else /* 系统内部固定分配的分区 */
        {
            for (j = 0; j < FSYS_MAX_XPARTS; j++)
            {
                if (pPartXTbl[j].letter == dletter + i)
                {
                    if (!pPartXTbl[j].part)
                    {
                        pPart->letter       = dletter + i;
                        pPartXTbl[j].part   = pPart;
                        __log("%s: \"%s\\%s\" is linked to symbel \"%c\".", pPart->pPD->name, classname, pPart->dname, pPart->letter);
                    }
                    else
                    {
                        __log("part \"%c\" is already exist!", dletter + i);
                    }

                    break;
                }
            }
            if (j == FSYS_MAX_XPARTS)
            {
                __err("wanna invalid fixed part letter!");
            }
        }

        /* 分配分区盘符失败 */
        if (pPart->letter == 0xff)
        {
            pPD->Ops.unmount(pPart, 1);
            free(pPart);
            /*esDEV_Close(hDev);    //device have been closed when part unmount*/
            pPD->nUsr--;
            fs_err  = -ENOSPC;
            res     = EPDK_FAIL;
            goto out;
        }

        /********************************************************/
        /* 挂接分区文件系统                                     */
        /********************************************************/
        pPart->status   = FSYS_PARTSTATUS_UNUSED;
        if (devattrib & DEV_NODE_ATTR_FS)
        {
            if (esFSYS_mntfs(pPart) == EPDK_OK)
            {
		__epos_kmsg_t *msg = NULL;
                pPart->status   = FSYS_PARTSTATUS_FSUSED;
                pPart->attr     |= FSYS_PARTATTR_FS;

		msg = (__epos_kmsg_t *)malloc(sizeof(__epos_kmsg_t));
		if (msg) {
			memset(msg, 0x00, sizeof(__epos_kmsg_t));
			msg->target		= KMSG_TGT_CALLBACK;
			msg->message		= 0;
			msg->prio		= KMSG_PRIO_HIGH;
			msg->l.cb		= blockdev_register_callback;
			msg->h.cb_u_arg		= pPart->letter;
			esKRNL_SemPost(pPartSem);
			esKSRV_SendMsgEx((void *)msg);
			esKRNL_SemPend(pPartSem, 0, NULL);
		}
            }
            else
            {
                __err("mount %c failure.", pPart->letter);
                pPart->attr     &= ~FSYS_PARTATTR_FS;
            }
        }
    }

    res     = EPDK_OK;

out:
    /* 挂载分区结束 */
    CurhNode    = NULL;
    esKRNL_SemPost(pPartSem);
    return res;
}

/*
******************************************************************
*
*             esFSYS_blkdevreg
*
*  Description:
*   注册块设备到文件系统中
*
*  Parameters:
*   hNode       - 设备节点句柄.
*
*  Return value:
*   EPDK_OK     - registered ok
*   EPDK_FAIL   - fail.
******************************************************************
*/
int32_t esFSYS_mntparts(__hdle hNode)
{
    __hdle          hDev;
    int32_t         devattrib;
	
    esKRNL_SemPend(pPartSem, 0, NULL);

    /************************************************************/
    /* 打开设备，获取设备属性                                   */
    /************************************************************/
    hDev    = esDEV_Open(hNode, 0);
    if (!hDev)
    {
        __log("open device node failed");
        esKRNL_SemPost(pPartSem);
        return EPDK_FAIL;
    }

    esDEV_Ioctl(hDev, DEV_IOC_SYS_GET_ATTRIB, 0, &devattrib);
    esDEV_Close(hDev);
    esKRNL_SemPost(pPartSem);

    if (devattrib & DEV_NODE_ATTR_SYNMNT)
    {
        /************************************************************/
        /* 同步挂载设备节点，直接挂载设备分区                       */
        /************************************************************/
        __mount_parts(hNode);
    }
    else
    {
        /************************************************************/
        /* 异步挂载设备节点，唤醒分区挂载线程挂载设备节点分区       */
        /************************************************************/
        CurhNode    = hNode;
        wakeup_mnt_thread();

    }

    return EPDK_OK;
}

/*
******************************************************************
*
*             esFSYS_blkdevunreg
*
*  Description:
*   拔除块设备
*
*  Parameters:
*   hNode       - 设备节点
*
*  Return value:
*   <0          - Unable to find the device.
*   >=0         - Index of the device in the device information
*                 table.
******************************************************************
*/
int32_t esFSYS_umntparts(__hdle hNode, uint32_t force)
{
    int32_t         ret, i, j, match_i;
    __fsys_part_t   *pPart;
    __fsys_part_t   *pParts[FSYS_MAX_PARTS], **ppParts[FSYS_MAX_PARTS];

    esKRNL_SemPend(pPartSem, 0, NULL);
	if (esFSYS_vfslock())
	{
		__log("esFSYS_umntparts err when enter vfs mutex\n");
		ret =  EPDK_FAIL;
		goto out1;
	}

    /************************************************************/
    /* 测试分区上的文件系统是否可以冻结并实施冻结               */
    /************************************************************/
    match_i = 0;
    for (i = 0; i < FSYS_MAX_FPARTS; i++)
    {
        pPart   = pPartFTbl[i];
        if (pPart && (pPart->hNode == hNode))
        {
            pParts[match_i] = pPart;
            ppParts[match_i] = &pPartFTbl[i];
            match_i++;
        }
    }

    for (i = 0; i < FSYS_MAX_XPARTS; i++)
    {
        pPart   = pPartXTbl[i].part;
        if (pPart && (pPart->hNode == hNode))
        {
            pParts[match_i] = pPart;
            ppParts[match_i] = (__fsys_part_t **)&pPartXTbl[i];
            match_i++;
        }
    }

    for (i = 0; i < FSYS_MAX_UPARTS; i++)
    {
        pPart   = pPartUTbl[i].part;
        if (pPart && (pPart->hNode == hNode))
        {
            pParts[match_i] = pPart;
            ppParts[match_i] = (__fsys_part_t **)&pPartUTbl[i];
            match_i++;
        }
    }
    for (i = 0; i < match_i; i ++) {
		__epos_kmsg_t *msg = NULL;

		msg = (__epos_kmsg_t *)malloc(sizeof(__epos_kmsg_t));
		if (msg)
		{
			memset(msg, 0x00, sizeof(__epos_kmsg_t));
			msg->target			= KMSG_TGT_CALLBACK;
			msg->message		= 0;
			msg->prio			= KMSG_PRIO_HIGH;
			msg->l.cb			= blockdev_unregister_callback;
			msg->h.cb_u_arg		= pParts[i]->letter;
			esKRNL_SemPost(pPartSem);
			esKSRV_SendMsgEx((void *)msg);
			esKRNL_SemPend(pPartSem, 0, NULL);
		}
	}

    for (i = 0; i < match_i; i ++)
    {
        if (test_and_freeze_partfs(pParts[i]->hFSPrivate) && !force)
        {
            /* 分区忙，无法冻结，卸载块设备分区操作中断     */
            for (j = 0; j < i; j++)
            {
                unfreezepart(pParts[j]->hFSPrivate);
            }
            fs_err = -EBUSY;
            ret = EPDK_FAIL;
            goto out;
        }
    }

    /************************************************************/
    /* 卸载设备上的所有的分区的文件系统                 */
    /************************************************************/
    for (i = 0; i < match_i; i++)
    {
        pPart   = pParts[i];
        if (pPart->status != FSYS_PARTSTATUS_FSUSED)
        {
            continue;
        }

        /* 卸载分区上的文件系统                                 */
        ret = esFSYS_umntfs(pPart, force);
        if (ret != EPDK_OK)
            __log("unexpect error when unload partfs, force==%d, part:%c", force, pPart->letter);
        else
        {
            pPart->status = FSYS_PARTSTATUS_UNUSED;
        }
    }

    /************************************************************/
    /* 释放设备上的所有的分区的分区驱动                         */
    /************************************************************/
    for (i = 0; i < match_i; i++)
    {
        uint32_t    busy = 0;

        pPart = pParts[i];

        /* 从分区驱动上unmount分区                              */
        if (pPart->pPD && pPart->pPD->Ops.unmount)
        {
            busy |= !!pPart->pPD->Ops.unmount(pPart, force);

            /* 完成分区卸载则销毁分区句柄，否则由分区占用者销毁分
               区句柄                                           */
            if (busy && force)
            {
                __log("unexpect error when unmount part%c, force==%d", pPart->letter, force);
            }
        }
        *ppParts[i] = 0;
    }

    ret = EPDK_OK;

out:
	esFSYS_vfsunlock();
out1:
    esKRNL_SemPost(pPartSem);
    return ret;
}
