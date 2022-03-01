/*
*************************************************************************************
*                                           Melis
*                                   USB Host Driver Operation
*
*                       (c) Copyright 2006-2010, All winners Co,Ld.
*                                  All Rights Reserved
*
* File Name     : usb_ops.h
*
* Author        : javen
*
* Description   :
*
* History       :
*      <author>         <time>          <version >          <desc>
*       javen          2010-12-13          1.0          create this file
*
*************************************************************************************
*/
#ifndef  __USB_OPS_H__
#define  __USB_OPS_H__

#include "module/mod_usbhost.h"


//---------------------------------------------------------------
//  加载 / 卸载  host / device 驱动
//---------------------------------------------------------------

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
__s32 App_InsmodUsbDeivceDriver(__u32 usbc_no);

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
__s32 App_RmmodUsbDeivceDriver(__u32 usbc_no);

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
__s32 App_InsmodUsbHostDriver(__u32 usbc_no);

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
__s32 App_RmmodUsbHostDriver(__u32 usbc_no);

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
__u32 App_IsUsbHostReomveComplete(__u32 usbc_no);

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
__s32 App_WaitForUsbHostReomveComplete(__u32 usbc_no);

//---------------------------------------------------------------
//  USB 光驱操作
//---------------------------------------------------------------

/*
*******************************************************************************
*                     USB_GetCDROMInfo
*
* Description:
*    获得光驱的信息
*
* Parameters:
*    Path  :  input. 设备名路径. 如"b:\\DISK\\USB_CDROM_xx"。
*    Info  :  output. 光驱的信息
*
* Return value:
*    返回成功或者失败。
*
* note:
*    无
*
*******************************************************************************
*/
__s32 USB_GetCDROMInfo(char *Path, usbh_CDROM_Info_t *Info);

/*
*******************************************************************************
*                     USB_GetDriveStatus
*
* Description:
*    获得光驱的状态
*
* Parameters:
*    Path  :  input. 设备名路径. 如"b:\\DISK\\USB_CDROM_xx"。
*
* Return value:
*    返回光驱的状态
*
* note:
*    无
*
*******************************************************************************
*/
__u32 USB_GetDriveStatus(char *Path);

/*
*******************************************************************************
*                     USB_CDROM_Eject
*
* Description:
*    弹出光碟
*
* Parameters:
*    Path  :  input. 设备名路径. 如"b:\\DISK\\USB_CDROM_xx"。
*
* Return value:
*    返回成功或者失败。
*
* note:
*    无
*
*******************************************************************************
*/
__s32 USB_CDEject(char *Path);

/*
*******************************************************************************
*                     USB_CDROM_Load
*
* Description:
*    装载光碟
*
* Parameters:
*    Path  :  input. 设备名路径. 如"b:\\DISK\\USB_CDROM_xx"。
*
* Return value:
*    返回成功或者失败。
*
* note:
*    无
*
*******************************************************************************
*/
__s32 USB_CDLoad(char *Path);

/*
*******************************************************************************
*                     USB_PreventMediumRemoval
*
* Description:
*    禁止介质移动，即禁止弹出光碟。
*
* Parameters:
*    Path  :  input. 设备名路径. 如"b:\\DISK\\USB_CDROM_xx"。
*
* Return value:
*    返回成功或者失败。
*
* note:
*    大多用在播放视频、音乐等场合，防止用户在此时通过光驱开关把光碟弹出。
*
*******************************************************************************
*/
__s32 USB_PreventMediumRemoval(char *Path);

/*
*******************************************************************************
*                     USB_AllowMediumRemoval
*
* Description:
*    允许介质移动
*
* Parameters:
*    Path  :  input. 设备名路径. 如"b:\\DISK\\USB_CDROM_xx"。
*
* Return value:
*    返回成功或者失败。
*
* note:
*    无
*
*******************************************************************************
*/
__s32 USB_AllowMediumRemoval(char *Path);

#endif   //__USB_OPS_H__
