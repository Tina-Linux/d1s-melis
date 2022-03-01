/*
************************************************************************************************************************
*                                                        usb host
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_usbh.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.18
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2010.10.18        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_USBH_H__
#define  __DSK_USBH_H__

#ifdef __cplusplus
extern "C" {
#endif


/*
****************************************************************************************************
*Name        : dsk_send_insmod_usbh_cmd
*Prototype   : __s32 dsk_send_insmod_usbh_cmd(void)
*Arguments   : void
*Return      : EPDK_OK     发送成功
*              EPDK_FAIL   发送失败
*Description : 向usbh_monitor发送加载host驱动命令
*Other       : 命令发送完毕后，delay 2s 确保加载成功
****************************************************************************************************
*/
extern __s32 dsk_send_insmod_usbh_cmd(void);

/*
****************************************************************************************************
*Name        : dsk_send_rmmod_usbh_cmd
*Prototype   : __s32 dsk_send_rmmod_usbh_cmd(void)
*Arguments   : void
*Return      : EPDK_OK     发送成功
*              EPDK_FAIL   发送失败
*Description : 向usbh_monitor发送卸载host驱动命令
*Other       : 命令发送完毕后，delay 1s 确保卸载成功
****************************************************************************************************
*/
extern __s32 dsk_send_rmmod_usbh_cmd(void);

/*
****************************************************************************************************
*Name        : dsk_usbh_is_working
*Prototype   : __bool dsk_usbh_is_working(void)
*Arguments   : void
*Return      : EPDK_TRUE     usb host is working
*              EPDK_FAIL     usb host is NOT working
*Description : check whether the usb host is working or not.
*Other       :
****************************************************************************************************
*/
__bool dsk_usbh_is_working(void);



#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_USBH_H__

/* end of dsk_usbh.h */
