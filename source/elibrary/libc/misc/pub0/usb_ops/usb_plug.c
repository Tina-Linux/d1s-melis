/*
*************************************************************************************
*                                           Melis
*                                   USB Host Driver Operation
*
*                       (c) Copyright 2006-2010, All winners Co,Ld.
*                                  All Rights Reserved
*
* File Name     : usb_plug.c
*
* Author        : javen
*
* Description   : usb 操作库。
*
* Note          : 具体命令可参考 "D:\winners\ePDK\includes\emod\sys_hwsc.h"
*                                "D:\winners\ePDK\includes\emod\drv_host.h"
*                                "D:\winners\ePDK\includes\emod\drv_udisk.h"
*
* History       :
*      <author>         <time>          <version >          <desc>
*       javen          2010-12-13          1.0          create this file
*
*************************************************************************************
*/
#include <typedef.h>
#include <libc/elibs_stdio.h>
#include <module/mod_usbhost.h>
#include <kernel/drivers/sys_hwsc.h>
#include <kernel/kapi.h>
#include <log.h>

/*
*********************************************************************
*                     App_InsmodUsbDeivceDriver
*
* Description:
*    向 usbh_monitor 发送加载 deivce 驱动命令
*
* Arguments:
*    usbc_no   :  input. USB控制器编号
*
* Returns:
*    EPDK_OK   :  发送成功
*    EPDK_FAIL :  发送失败
*
* note:
*    命令发送完毕后，delay 2s 确保加载成功
*
*********************************************************************
*/
__s32 App_InsmodUsbDeivceDriver(__u32 usbc_no)
{
    ES_FILE *usbm_dev_hadle = NULL;
    __s32 ret = 0;
    __inf("%s %d :App_InsmodUsbDeivceDriver usbc_no = %d\n", __FUNCTION__, __LINE__, usbc_no);
    /* 打开 hwsc 设备 */
    usbm_dev_hadle = eLIBs_fopen("b:\\HWSC\\hwsc", "r");

    if (usbm_dev_hadle == NULL)
    {
        __wrn("ERR: eLIBs_fopen failed\n");
        return EPDK_FAIL;
    }

    /* 发送加载host命令 */
    ret = eLIBs_fioctrl(usbm_dev_hadle, DEV_IOC_USR_HWSC_APP_INSMOD_USBD, 0, NULL);

    if (ret == EPDK_FAIL)
    {
        __inf("ERR: execute DEV_IOC_USR_HWSC_APP_INSMOD_USBD failed\n");
        eLIBs_fclose(usbm_dev_hadle);
        usbm_dev_hadle = NULL;
        return EPDK_FAIL;
    }

    /* 关闭 hwsc 设备 */
    eLIBs_fclose(usbm_dev_hadle);
    usbm_dev_hadle = NULL;
    /*wait to install host driver less than 2 second*/
    //  esKRNL_TimeDly(200);
    return EPDK_OK;
}

/*
*********************************************************************
*                     App_RmmodUsbDeivceDriver
*
* Description:
*    向 usbh_monitor 发送卸载 device 驱动命令
*
* Arguments:
*    usbc_no   :  input. USB控制器编号
*
* Returns:
*    EPDK_OK   :  发送成功
*    EPDK_FAIL :  发送失败
*
* note:
*    命令发送完毕后，delay 1s 确保卸载成功
*
*********************************************************************
*/
__s32 App_RmmodUsbDeivceDriver(__u32 usbc_no)
{
    ES_FILE *usbm_dev_hadle = NULL;
    __s32 ret = 0;
    __inf("%s %d :App_RmmodUsbDeivceDriver usbc_no = %d\n", __FUNCTION__, __LINE__, usbc_no);
    /* 打开 hwsc 设备 */
    usbm_dev_hadle = eLIBs_fopen("b:\\HWSC\\hwsc", "r");

    if (usbm_dev_hadle == NULL)
    {
        __wrn("ERR: eLIBs_fopen failed\n");
        return EPDK_FAIL;
    }

    /* 发送加载host命令 */
    ret = eLIBs_fioctrl(usbm_dev_hadle, DEV_IOC_USR_HWSC_APP_RMMOD_USBD, 0, NULL);

    if (ret == EPDK_FAIL)
    {
        __inf("ERR: execute DEV_IOC_USR_HWSC_APP_RMMOD_USBD failed\n");
        eLIBs_fclose(usbm_dev_hadle);
        usbm_dev_hadle = NULL;
        return EPDK_FAIL;
    }

    /* 关闭 hwsc 设备 */
    eLIBs_fclose(usbm_dev_hadle);
    usbm_dev_hadle = NULL;
    /* delay 1s 确保usb_monitor有足够的时间卸载 */
    //  esKRNL_TimeDly(200);
    return EPDK_OK;
}

/*
*********************************************************************
*                     App_InsmodUsbHostDriver
*
* Description:
*    向usbh_monitor发送加载host驱动命令
*
* Arguments:
*    usbc_no   :  input. USB控制器编号
*
* Returns:
*    EPDK_OK   :  发送成功
*    EPDK_FAIL :  发送失败
*
* note:
*    命令发送完毕后，delay 2s 确保加载成功
*
*********************************************************************
*/
__s32 App_InsmodUsbHostDriver(__u32 usbc_no)
{
    ES_FILE *usbm_dev_hadle = NULL;
    __s32 ret = 0;
    __inf("%s %d : App_InsmodUsbHostDriver usbc_no = %d\n", __FUNCTION__, __LINE__, usbc_no);
    /* 打开 hwsc 设备 */
    usbm_dev_hadle = eLIBs_fopen("b:\\HWSC\\hwsc", "r");

    if (usbm_dev_hadle == NULL)
    {
        __wrn("ERR: eLIBs_fopen failed\n");
        return EPDK_FAIL;
    }

    /* 发送加载host命令 */
    if (usbc_no == 0)
    {
        ret = eLIBs_fioctrl(usbm_dev_hadle, DEV_IOC_USR_HWSC_APP_INSMOD_USBH, 0, NULL);
    }
    else if (usbc_no == 1)
    {
        ret = eLIBs_fioctrl(usbm_dev_hadle, DEV_IOC_USR_HWSC_APP_INSMOD_USBH_1, 0, NULL);
    }
    else if (usbc_no == 2)
    {
        ret = eLIBs_fioctrl(usbm_dev_hadle, DEV_IOC_USR_HWSC_APP_INSMOD_USBH_2, 0, NULL);
    }
    else
    {
        __wrn("ERR: unkown usb controller number, usbc_no = %d\n", usbc_no);
    }

    if (ret == EPDK_FAIL)
    {
        __inf("ERR: execute DEV_IOC_USR_HWSC_APP_INSMOD_USBH failed\n");
        eLIBs_fclose(usbm_dev_hadle);
        usbm_dev_hadle = NULL;
        return EPDK_FAIL;
    }

    /* 关闭 hwsc 设备 */
    eLIBs_fclose(usbm_dev_hadle);
    usbm_dev_hadle = NULL;
    /*wait to install host driver less than 2 second*/
    //  esKRNL_TimeDly(200);
    return EPDK_OK;
}

/*
*********************************************************************
*                     App_RmmodUsbHostDriver
*
* Description:
*    向usbh_monitor发送卸载host驱动命令
*
* Arguments:
*    usbc_no   :  input. USB控制器编号
*
* Returns:
*    EPDK_OK   :  发送成功
*    EPDK_FAIL :  发送失败
*
* note:
*    命令发送完毕后，delay 1s 确保卸载成功
*
*********************************************************************
*/
__s32 App_RmmodUsbHostDriver(__u32 usbc_no)
{
    ES_FILE *usbm_dev_hadle = NULL;
    __s32 ret = 0;
    __inf("%s %d : App_RmmodUsbHostDriver usbc_no = %d\n", __FUNCTION__, __LINE__, usbc_no);
    /* 打开 hwsc 设备 */
    usbm_dev_hadle = eLIBs_fopen("b:\\HWSC\\hwsc", "r");

    if (usbm_dev_hadle == NULL)
    {
        __wrn("ERR: eLIBs_fopen failed\n");
        return EPDK_FAIL;
    }

    /* 发送加载host命令 */
    if (usbc_no == 0)
    {
        ret = eLIBs_fioctrl(usbm_dev_hadle, DEV_IOC_USR_HWSC_APP_RMMOD_USBH, 0, NULL);
    }
    else if (usbc_no == 1)
    {
        ret = eLIBs_fioctrl(usbm_dev_hadle, DEV_IOC_USR_HWSC_APP_RMMOD_USBH_1, 0, NULL);
    }
    else if (usbc_no == 2)
    {
        ret = eLIBs_fioctrl(usbm_dev_hadle, DEV_IOC_USR_HWSC_APP_RMMOD_USBH_2, 0, NULL);
    }
    else
    {
        __wrn("ERR: unkown usb controller number, usbc_no = %d\n", usbc_no);
    }

    if (ret == EPDK_FAIL)
    {
        __inf("ERR: execute DEV_IOC_USR_HWSC_APP_RMMOD_USBH failed\n");
        eLIBs_fclose(usbm_dev_hadle);
        usbm_dev_hadle = NULL;
        return EPDK_FAIL;
    }

    /* 关闭 hwsc 设备 */
    eLIBs_fclose(usbm_dev_hadle);
    usbm_dev_hadle = NULL;
    /* delay 1s 确保usb_monitor有足够的时间卸载 */
    //  esKRNL_TimeDly(200);
    return EPDK_OK;
}

/*
*******************************************************************************
*                     App_IsUsbHostReomveComplete
*
* Description:
*    查询 usb host 驱动是否卸载完成
*
* Parameters:
*    usbc_no   :  input. USB控制器编号
*
* Return value:
*    0  : 没有完全卸载
*    !0 : 已经完全卸载了
*
* note:
*    无设备插入 : 卸载时间小于 1s
*    移动硬盘   : 卸载时间大于 10s
*    光驱       : 卸载时间大于 10s
*    U盘        : 卸载时间小于 1s
*    读卡器     : 卸载时间小于 1s
*
*******************************************************************************
*/
__u32 App_IsUsbHostReomveComplete(__u32 usbc_no)
{
    ES_FILE *pHdle = NULL;
    __s32 ret = 0;
    __u32 status = 0;
    /* 打开 hwsc 设备 */
    pHdle = eLIBs_fopen("b:\\HWSC\\hwsc", "r");

    if (pHdle == NULL)
    {
        __wrn("ERR: eLIBs_fopen failed\n");
        return 0;
    }

    /* 发送命令 */
    if (usbc_no == 0)
    {
        ret = eLIBs_fioctrl(pHdle, DEV_IOC_USR_HWSC_GET_USBH_WORK_STATUS, 0, NULL);
    }
    else if (usbc_no == 1)
    {
        ret = eLIBs_fioctrl(pHdle, DEV_IOC_USR_HWSC_GET_USBH1_WORK_STATUS, 0, NULL);
    }
    else if (usbc_no == 2)
    {
        ret = eLIBs_fioctrl(pHdle, DEV_IOC_USR_HWSC_GET_USBH2_WORK_STATUS, 0, NULL);
    }
    else
    {
        __wrn("ERR: unkown usb controller number, usbc_no = %d\n", usbc_no);
    }

    /* 关闭 hwsc 设备 */
    eLIBs_fclose(pHdle);
    pHdle = NULL;

    /* 根据返回状态，作出相应的判断 */
    if (ret)
    {
        status = 1;
    }
    else
    {
        status = 0;
    }

    return status;
}

/*
*******************************************************************************
*                     App_WaitForUsbHostReomveComplete
*
* Description:
*    等待 USB HOST 驱动卸载完毕。因为在有移动硬盘插入的情况下，host驱动卸载
* 需要花 10s 的时间，因此卸载的时间大于 10s 。
*
* Parameters:
*    usbc_no   :  input. USB控制器编号
*
* Return value:
*    EPDK_OK    :  成功
*    EPDK_FAIL  :  失败
*
* note:
*    如果有在关机的情况下卸载 host 驱动，可以在 delay 期间去关闭 LCD，避免
* 由于 host 驱动卸载过久，造成关机时间太长。
*
*******************************************************************************
*/
__s32 App_WaitForUsbHostReomveComplete(__u32 usbc_no)
{
    __u32 time = 200;

    /* wait for usb host driver remove complete */
    while (time)
    {
        if (App_IsUsbHostReomveComplete(usbc_no))
        {
            __msg("remove usb%d host complete\n", usbc_no);
            break;
        }

        esKRNL_TimeDly(100);
        time--;
    }

    /* timeout */
    if (time == 0)
    {
        __wrn("remove usb%d host timeout\n", usbc_no);
        return EPDK_FAIL;
    }

    return EPDK_OK;
}
