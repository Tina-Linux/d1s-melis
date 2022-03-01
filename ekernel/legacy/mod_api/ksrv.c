/*
 * ===========================================================================================
 *
 *       Filename:  ksrv.c
 *
 *    Description:  kernel service definition.
 *
 *        Version:  Melis3.0
 *         Create:  2018-04-24 11:10:23
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-05-08 20:34:27
 *
 * ===========================================================================================
 */
#include "ksrv.h"
#include <kapi.h>
#include <rthw.h>
#include <log.h>
#include <kconfig.h>
#include <string.h>
#include <stdio.h>
#include <boot.h>
#include <arch.h>

extern boot_head_t  kernel_param;
static rt_mailbox_t app_msgq_l;
static rt_mailbox_t app_msgq_h;
static rt_mailbox_t ksrv_msgq;
__ksrv_add_para     g_ksrv_add_para = { 0 };
static void *display_hld;
static unsigned long mixture_mp;
static int mixture_mid;

/*
**********************************************************************************************************************
*                                               esKSRV_ClearWatchDog
*
* Description: clear watchdog;
*
* Arguments  : none;
*
* Returns    : EPDK_OK/EPDK_FAIL;
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esKSRV_ClearWatchDog(void)
{
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               esKSRV_DisableWatchDog
*
* Description: disable watchdog;
*
* Arguments  : none
*
* Returns    : EPDK_OK/EPDK_FAIL;
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esKSRV_DisableWatchDog(void)
{
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               esKSRV_DisableWatchDogSrv
*
* Description: disable watchdog server, server will not clear watchdog automatic;
*
* Arguments  : none;
*
* Returns    : EPDK_OK/EPDK_FAIL;
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esKSRV_DisableWatchDogSrv(void)
{
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               esKSRV_EnableWatchDog
*
* Description: enable watchdog;
*
* Arguments  : time duration for watchdog to reset system;
*
* Returns    : EPDK_OK/EPDK_FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esKSRV_EnableWatchDog(void)
{
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               esKSRV_EnableWatchDogSrv
*
* Description: enable watchdog server;
*
* Arguments  : none;
*
* Returns    : EPDK_OK/EPDK_FAIL;
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esKSRV_EnableWatchDogSrv(void)
{
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               esKSRV_GetAddPara
*
* Description: get aw kernel data;
*
* Arguments  : none;
*
* Returns    : EPDK_OK/EPDK_FAIL;
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esKSRV_GetAddPara(__ksrv_add_para *ksrv_add_para)
{
    *ksrv_add_para = g_ksrv_add_para;
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               esKSRV_GetDramCfgPara
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
int32_t esKSRV_GetDramCfgPara(__dram_para_t *drampara)
{
    if (!drampara)
    {
        __err("pointer is invalid!");
        return EPDK_FAIL;
    }

    //rt_memcpy(drampara, kernel_param.boot_data.dram_para, sizeof(__dram_para_t));
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               esKSRV_GetHighMsg
*
* Description: get high priority message from app message queue
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
unsigned long esKSRV_GetHighMsg(void)
{
    rt_err_t        err;
    unsigned long   ret;

    if (app_msgq_h == RT_NULL)
    {
        return 0;
    }
    err = rt_mb_recv(app_msgq_h, (unsigned long *)&ret, 0);
    if (err != RT_EOK)
    {
        ret = 0;
    }

    return ret;

}

/*
**********************************************************************************************************************
*                                               esKSRV_GetLowMsg
*
* Description: get low priority message from app message queue
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
unsigned long esKSRV_GetLowMsg(void)
{
    rt_err_t        err;
    unsigned long   ret;

    if (app_msgq_l == RT_NULL)
    {
        return 0;
    }
    err = rt_mb_recv(app_msgq_l, (rt_ubase_t *)&ret, 0);
    if (err != RT_EOK)
    {
        ret = 0;
    }

    return ret;
}

/*
**********************************************************************************************************************
*                                               esKSRV_GetMsg
*
* Description: get priority message from app message queue, high queue prio to low queue
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
unsigned long esKSRV_GetMsg(void)
{
    rt_err_t        err;
    unsigned long   ret;

    if (app_msgq_h == RT_NULL)
    {
        return 0;
    }
    if (app_msgq_l == RT_NULL)
    {
        return 0;
    }
    err = rt_mb_recv(app_msgq_h, (rt_ubase_t *)&ret, 0);
    if (err != RT_EOK)
    {
        err = rt_mb_recv(app_msgq_l, (rt_ubase_t *)&ret, 0);
        if (err != RT_EOK)
        {
            ret = 0;
        }
    }

    return ret;
}

/*
**********************************************************************************************************************
*                                               esKSRV_GetMsg
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
uint8_t esKSRV_GetSocID(void)
{
    uint8_t chip_id = 0;

#ifdef CONFIG_SOC_SUN3IW2
    chip_id = READ_REG_BYTE(EPDK_SUN3IW2_CHIP_REG);
#elif defined CONFIG_SOC_SUN3IW1
    chip_id = READ_REG_BYTE(EPDK_SUN3IW1_CHIP_REG);
#endif

    return chip_id;
}

/*
**********************************************************************************************************************
*                                               esKSRV_GetP1CfgPara
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
int32_t esKSRV_GetTargetPara(void *targetpara)
{
    //*targetpara = kernel_param.target;
    rt_kprintf("esKSRV_GetTargetPara change boot0, need to change the method for judging debug_log\n");
    arch_break();
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               esKSRV_GetMsg
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
uint32_t esKSRV_GetVersion(__epdk_ver_type_t type)
{
    uint32_t ver = 0;

    switch (type)
    {
        case EPDK_VER:
            ver = 0x2000000;
            break;
        case EPDK_VER_OS:
            ver = 0;
            break;
        default:
            break;
    }
    return ver;
}

/*
**********************************************************************************************************************
*                                               esKSRV_GetMsg
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
void * esKSRV_Get_Display_Hld(void)
{
	//printf("get display_hld is %x\n",display_hld);
	if(display_hld)
	{
		return display_hld;
	}
	else
	{
		printf("get display_hld is NULL\n");
		return NULL;
	}
}


void esKSRV_Get_Mixture_Hld(int *mid, unsigned long *mp)
{
    __inf("get mod_mixture mp is %x\n",mixture_mp);
    if(mixture_mid > 0)
    {
            *mid = mixture_mid;
    }
    if(mid > 0)
    {
       *mp = mixture_mp;
    }
}

void esKSRV_Save_Display_Hld(void* hld)
{
	if(hld)
	{
		display_hld = hld;
	}
	else
	{
		printf("set display_hld is NULL\n");
	}
}

void esKSRV_Save_Mixture_Hld(int mid, void *mp)
{
	__inf("set mixture  is %x  %x \n",mid,mp);
    if(mid > 0)
    {
       mixture_mid = mid;
    }
    if(mid > 0)
    {
      mixture_mp = mp;
    }
}
/*
*********************************************************************************************************
*                                       COPY MEMORY BY newlib when fpga
*
* Description: copy memory by newlib.
*
* Arguments  : pdest    destination buffer address;
*              psrc     source buffer address;
*              size     size of the data block need be copied;
*
* Returns    : none
*
* Notes      : the buffer should be physically sequential.
*********************************************************************************************************
*/
int32_t esKSRV_memcpy(void *pdest, const void *psrc, size_t size)
{
    memcpy(pdest, psrc, size);
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       FILE MEMORY BY newlib when fpga
*
* Description: fill data buffer with specified data octet.
*
* Arguments  : pmem        Pointer to memory buffer to fill with specified data octet.
*              value       Data fill octet value.
*              size        Number of data buffer octets to fill.
*
* Returns    : none
*
* Notes      : the buffer should be physically sequential.
*********************************************************************************************************
*/
int32_t esKSRV_memset(void *pmem, __u8 value, size_t size)
{
    memset(pmem, value, size);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               esKSRV_PowerOff
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
void esKSRV_PowerOff(void)
{
    //must do something to help restore system
    svc_exit();
}

/*
**********************************************************************************************************************
*                                               esKSRV_Random
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
static uint32_t seedtbl[] = {
    16381163 , 902867507, 703006358, 825601098, 702943101, 600362569, 824180876, 928050656,
    500242512, 600364801, 928023969, 680398149, 672661988, 825601527, 825601527, 825613410,
    823090756, 526599359, 8342805  , 631233585, 302865607, 926952182, 232272027, 824145274,
    926933440, 919200125, 631206441, 631245671, 726297021, 726289272,  85829891, 631270516,
    726269366, 727066702, 726269355, 750013275, 726212604, 672729451,  85809036, 825660596,
    417735750, 18473775 , 631290206
};

uint32_t esKSRV_Random(uint32_t max)
{
    uint32_t        ret;
    static uint8_t  index;
    static uint32_t ramdomcnt;
    static uint32_t ramdomseed;

    if (index > sizeof(seedtbl) / sizeof(seedtbl[0]))
    {
        index = 0;
    }

    ramdomcnt   = ramdomcnt * 29 + 314;
    ramdomseed  += ramdomcnt;

    ret = ramdomcnt + rt_tick_get();
    ret = ret + seedtbl[index++];
    ret = ret - (ret / max) * max;

    return ret;
}

/*
**********************************************************************************************************************
*                                               esKSRV_Reset
*
* Description: reset the system by software method, like open wtd but not feed
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
void esKSRV_Reset(void)
{
#ifdef CONFIG_DRIVER_WDT
extern void reset_cpu(void);
	reset_cpu();
#endif	
    while (1);
}

/*
**********************************************************************************************************************
*                                               esKSRV_Save_Display_Hld
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


/*
**********************************************************************************************************************
*                                               esKSRV_SendMsg
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
int32_t esKSRV_SendMsg(uint32_t msgid, uint32_t prio)
{
    rt_uint32_t ret;

    if (app_msgq_h == RT_NULL)
    {
        return -1;
    }
    if (prio)
    {
        while (rt_mb_send(app_msgq_h, msgid) == -RT_EFULL)
        {
            __err("high message pool full!");
            rt_mb_recv(app_msgq_h, (void *)&ret, 0);
        }
    }
    else
    {
        while (rt_mb_send(app_msgq_l, msgid) == -RT_EFULL)
        {
            __err("low message pool full!");
            rt_mb_recv(app_msgq_l, (void *)&ret, 0);
        }
    }
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               esKSRV_SendMsgEx
*
* Description: the message send for kernel to ksrv_msgq, like devices are monitored/detected
*
* Arguments  : pointer of message which as structure __epos_kmsg_t
*
* Returns    : 
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esKSRV_SendMsgEx(void *msg)
{
    __epos_kmsg_t *p_msg = (__epos_kmsg_t *)msg;

    if (p_msg == NULL || ksrv_msgq == NULL)
    {
        __err("wrong parameter.");
        return EPDK_FAIL;
    }

    if (rt_interrupt_get_nest())
    {
        return EPDK_FAIL;
    }

    switch (p_msg->target)
    {
        case KMSG_TGT_INT_TAIL:
            if (p_msg->prio)
            {
                __err("high prio not support!");
            }
            if (ksrv_msgq)
            {
                rt_mb_send(ksrv_msgq, (rt_ubase_t)msg);
            }
            return EPDK_OK;

        case KMSG_TGT_CALLBACK:
            if (p_msg->prio)
            {
                if (p_msg->l.cb)
                {
                    (p_msg->l.cb)(p_msg->h.cb_u_arg, 0);
                }
                rt_free(msg);
                return EPDK_OK;
            }
            if (ksrv_msgq)
            {
                rt_mb_send(ksrv_msgq, (rt_ubase_t)msg);
            }
            return EPDK_OK;
        case KMSG_TGT_BROADCST:
            if (ksrv_msgq)
            {
                rt_mb_send(ksrv_msgq, (rt_ubase_t)msg);
            }
            return EPDK_OK;
        default:
            if (ksrv_msgq)
            {
                rt_mb_send(ksrv_msgq, (rt_ubase_t)msg);
            }
            return EPDK_OK;
    }
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       system memory info
*
* Description: printf the system memory info, total.
*
* Arguments  : void
*
* Returns    : OK
*
* Notes      : the buffer should be physically sequential.
*********************************************************************************************************
*/
int32_t esKSRV_SysInfo(void)
{
    rt_uint32_t total, used, max_used;

    esCLK_SysInfo("all");
    rt_memory_info(&total, &used, &max_used);

    printf("%14s %12s 0x%08x"
            "%12s 0x%08x"
            "%12s 0x%08x",
            "Mem info:", "Total:", total, "Used:", used, "Summit:", max_used);

    return 0;
}


/*
**********************************************************************************************************************
*                                               kservice_maintask
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
static void kservice_maintask(void *p_arg)
{
    __epos_kmsg_t   *msg;
    rt_base_t       cpu_sr;
    rt_err_t        err;

    while (1)
    {
        msg = RT_NULL;
        err = rt_mb_recv(ksrv_msgq, (rt_ubase_t *)&msg, RT_WAITING_FOREVER);
        if (err != RT_EOK)
        {
            __err("get message fail.");
            continue;
        }

        switch (msg->target)
        {
            case KMSG_TGT_INT_TAIL:                 /* 临时的特殊处理，未来想好的更好的办法再更新                   */
            {
                /* 由于msg是不需要释放的，为了避免后续的消息用相同的msg单元， 加上临界区，并copy以隔离              */
                /* 对于中断尾，由于中断可能来多次，消息时容许用相同的消息单元的，因为不需要释放，静态分配的         */
                /* 此处用tmpmsg将消息备份起来再使用，主要是怕前后的消息的消息内容有可能不一样（通常一样）           */
                __epos_kmsg_t tmpmsg;

                memset(&tmpmsg, 0x00, sizeof(tmpmsg));
                cpu_sr = rt_hw_interrupt_disable();
                tmpmsg = *msg;
                rt_hw_interrupt_enable(cpu_sr);

                (tmpmsg.l.cb)(tmpmsg.h.cb_u_arg, 0);
                continue;
            }

            case KMSG_TGT_CALLBACK:
                (msg->l.cb)(msg->h.cb_u_arg, 0);
                break;

            case KMSG_TGT_BROADCST:
            {
                switch (msg->message)
                {
                    case KMSG_VAL_PHONE_CMD:
                    {
                        __err("KMSG_VAL_PHONE_CMD, msg_data=%d", msg->l.para);
                        esKSRV_SendMsg(msg->l.para, KMSG_PRIO_HIGH);
                        break;
                    }

                    case KMSG_VAL_DEV_PLUGIN:
                    {
                        switch (msg->l.para)
                        {
                            case KMSG_VAL_DEV_USBD:
                                //__log("USB device plug in message...");
                                esKSRV_SendMsg(KMSG_USR_SYSTEM_USBD_PLUGIN, KMSG_PRIO_HIGH);
                                break;
                            case KMSG_VAL_DEV_USBH:
                                //__log("USB host plug in message...");
                                esKSRV_SendMsg(KMSG_USR_SYSTEM_USBH_PLUGIN, KMSG_PRIO_HIGH);
                                break;
                            case KMSG_VAL_DEV_SDMMC:
                                //__log("SDMMC card plug in message...");
                                esKSRV_SendMsg(KMSG_USR_SYSTEM_SDMMC_PLUGIN, KMSG_PRIO_HIGH);
                                break;
                            default:
                                break;
                        }
                    }
                    break;

                    case KMSG_VAL_DEV_PLUGOUT:
                    {
                        switch (msg->l.para)
                        {
                            case KMSG_VAL_DEV_USBD:
                                //__log("USB device plug out message...");
                                esKSRV_SendMsg(KMSG_USR_SYSTEM_USBD_PLUGOUT, KMSG_PRIO_HIGH);
                                break;
                            case KMSG_VAL_DEV_USBH:
                                //__log("USB host plug out message...");
                                esKSRV_SendMsg(KMSG_USR_SYSTEM_USBH_PLUGOUT, KMSG_PRIO_HIGH);
                                break;
                            case KMSG_VAL_DEV_SDMMC:
                                //__log("SDMMC card plug out message...");
                                esKSRV_SendMsg(KMSG_USR_SYSTEM_SDMMC_PLUGOUT, KMSG_PRIO_HIGH);
                                break;
                            default:
                                break;
                        }
                    }
                    break;

                    case KMSG_VAL_FS_PLUGIN:
                    {
                        int message;

                        message = KMSG_USR_SYSTEM_FS_PLUGIN | ((msg->l.para << 16) & KMSG_USR_SYSTEM_FS_PARA_MASK);
                        __log("partition [%c] plug in..", msg->l.para);
                        esKSRV_SendMsg(message, KMSG_PRIO_HIGH);
                    }
                    break;

                    case KMSG_VAL_FS_PLUGOUT:
                    {
                        int message;

                        message = KMSG_USR_SYSTEM_FS_PLUGOUT | ((msg->l.para << 16) & KMSG_USR_SYSTEM_FS_PARA_MASK);
                        __log("partition [%c] plug out..", msg->l.para);
                        esKSRV_SendMsg(message, KMSG_PRIO_HIGH);
                    }
                    break;

                    case KMSG_VAL_USBD_CONNECT:
                    {
                        switch (msg->l.para)
                        {
                            case KMSG_USR_SYSTEM_USBD_NOT_CONNECT:
                                //__log("USB device plug in message...");
                                esKSRV_SendMsg(KMSG_USR_SYSTEM_USBD_NOT_CONNECT, KMSG_PRIO_HIGH);
                                //__log("free __epos_kmsg_t");
                                break;

                            default:
                                //__log("free __epos_kmsg_t.");
                                break;
                        }
                    }
                    break;

                    default:
                        break;
                }
                break;
            }

            default:
                __err("msg->target = %d.", msg->target);
                break;
        }
        rt_free((void *)msg);
    }
}

/*
**********************************************************************************************************************
*                                               ksrv_init
*
* Description: int a kernel service, to deal with message transition
*
* Arguments  : none
*
* Returns    : EPDK_OK/EPDK_FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t ksrv_init(void)
{
    rt_thread_t ksrv_t;

    ksrv_msgq   = rt_mb_create("ksrv_mq",     16,    0);
    app_msgq_l  = rt_mb_create("appmsgql_mq", 32,    0);
    app_msgq_h  = rt_mb_create("appmsgqh_mq", 128,   0);

    if (!app_msgq_l || !app_msgq_h || !ksrv_msgq)
    {
        __err("create msgq failed!");
        return -1;
    }

    ksrv_t = rt_thread_create("kmsg2appq", kservice_maintask, RT_NULL, 0x1000, RT_TIMER_THREAD_PRIO - 2, 10);
    if (ksrv_t == RT_NULL)
    {
        __err("create kernel service task failure.");
        return -1;
    }
    rt_thread_startup(ksrv_t);

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               ksrv_exit
*
* Description: exist the kernel service
*
* Arguments  : none
*
* Returns    : EPDK_OK/EPDK_FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t ksrv_exit(void)
{
    rt_mb_delete(ksrv_msgq);
    rt_mb_delete(app_msgq_l);
    rt_mb_delete(app_msgq_h);
    ksrv_msgq   = RT_NULL;
    app_msgq_l  = RT_NULL;
    app_msgq_h  = RT_NULL;

    return EPDK_OK;
}
