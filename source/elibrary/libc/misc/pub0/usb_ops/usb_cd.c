/*
********************************************************************************************************************
*                                           Melis
*                                   USB Host Driver Operation
*
*                       (c) Copyright 2006-2010, All winners Co,Ld.
*                                   All Rights Reserved
*
* File Name     : usb_cd.c
*
* Author        : javen
*
* Description   : USB光驱的相关操作
*
* History       :
*      <author>         <time>          <version >          <desc>
*       javen          2010-12-3          1.0          create this file
*
* Note          :
*  1、USB光驱的相关命令，请参考"..\ePDK\includes\emod\drv_host.h"
*  2、USB光驱有光碟并且已经注册块设备时，可通过"b:\\DISK\\USB_CDROM_xx"找到USB光驱设备
*  3、USB光驱没有光碟时，可通过"b:\\USERDEF\\USB_CDROM_xx"找到USB光驱设备
*  4、"USB_CDROM_xx"中"xx"表示编号，如有两个USB光驱存在，那么其编号位"USB_CDROM_x00"、"USB_CDROM_01"
*
********************************************************************************************************************
*/
#include <typedef.h>
#include <libc/elibs_stdio.h>
#include <module/mod_usbhost.h>
#include <log.h>
/*
*******************************************************************************
*                     USB_GetCDROMInfo
*
* Description:
*    获得光驱的信息
*
* Parameters:
*    Path  :  input. 设备名路径. 如"b:\\DISK\\USB_CDROM_xx"。
*    Info  :  input. 光驱的信息
*
* Return value:
*    返回成功或者失败。
*
* note:
*    无
*
*******************************************************************************
*/
__s32 USB_GetCDROMInfo(char *Path, usbh_CDROM_Info_t *Info)
{
    ES_FILE *pfile = NULL;
    __s32 ret = 0;
    pfile = eLIBs_fopen(Path, "r+");

    if (pfile == NULL)
    {
        __wrn("ERR: fopen %s failed\n", Path);
        return EPDK_FAIL;
    }

    ret = eLIBs_fioctrl(pfile,
                        USB_STORAGE_USER_CTRL_CMD_GET_CDROM_INFO,
                        0,
                        Info);

    if (ret != EPDK_OK)
    {
        __wrn("ERR: execute USB_STORAGE_USER_CTRL_CMD_GET_CDROM_INFO failed\n");
        eLIBs_fclose(pfile);
        return EPDK_FAIL;
    }

    eLIBs_fclose(pfile);
    return EPDK_OK;
}

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
__u32 USB_GetDriveStatus(char *Path)
{
    ES_FILE *pfile = NULL;
    __s32 ret = 0;
    __u32 Status = 0;
    pfile = eLIBs_fopen(Path, "r+");

    if (pfile == NULL)
    {
        __wrn("ERR: fopen %s failed\n", Path);
        return USBH_CDROM_DRIVE_STATUS_NO_INFORMATION;
    }

    ret = eLIBs_fioctrl(pfile,
                        USB_STORAGE_USER_CTRL_CMD_CDROM_GET_DRIVE_STATUS,
                        0,
                        &Status);

    if (ret != EPDK_OK)
    {
        __wrn("ERR: execute USB_STORAGE_USER_CTRL_CMD_CDROM_GET_DRIVE_STATUS failed\n");
        eLIBs_fclose(pfile);
        return USBH_CDROM_DRIVE_STATUS_NO_INFORMATION;
    }

    eLIBs_fclose(pfile);
    return Status;
}

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
__s32 USB_CDEject(char *Path)
{
    ES_FILE *pfile = NULL;
    __s32 ret = 0;
    pfile = eLIBs_fopen(Path, "r+");

    if (pfile == NULL)
    {
        __wrn("ERR: fopen %s failed\n", Path);
        return EPDK_FAIL;
    }

    ret = eLIBs_fioctrl(pfile,
                        USB_STORAGE_USER_CTRL_CMD_CDROM_EJECT,
                        0,
                        NULL);

    if (ret != EPDK_OK)
    {
        __wrn("ERR: execute USB_STORAGE_USER_CTRL_CMD_CDROM_EJECT failed\n");
        eLIBs_fclose(pfile);
        return EPDK_FAIL;
    }

    eLIBs_fclose(pfile);
    return EPDK_OK;
}

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
__s32 USB_CDLoad(char *Path)
{
    ES_FILE *pfile = NULL;
    __s32 ret = 0;
    pfile = eLIBs_fopen(Path, "r+");

    if (pfile == NULL)
    {
        __wrn("ERR: fopen %s failed\n", Path);
        return EPDK_FAIL;
    }

    ret = eLIBs_fioctrl(pfile,
                        USB_STORAGE_USER_CTRL_CMD_CDROM_LOAD,
                        0,
                        NULL);

    if (ret != EPDK_OK)
    {
        __wrn("ERR: execute USB_STORAGE_USER_CTRL_CMD_CDROM_LOAD failed\n");
        eLIBs_fclose(pfile);
        return EPDK_FAIL;
    }

    eLIBs_fclose(pfile);
    return EPDK_OK;
}

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
__s32 USB_PreventMediumRemoval(char *Path)
{
    ES_FILE *pfile = NULL;
    __s32 ret = 0;
    pfile = eLIBs_fopen(Path, "r+");

    if (pfile == NULL)
    {
        __wrn("ERR: fopen %s failed\n", Path);
        return EPDK_FAIL;
    }

    ret = eLIBs_fioctrl(pfile,
                        USB_STORAGE_USER_CTRL_CMD_CDROM_PREVENT_MEDIUM_REMOVAL,
                        0,
                        NULL);

    if (ret != EPDK_OK)
    {
        __wrn("ERR: execute USB_STORAGE_USER_CTRL_CMD_CDROM_PREVENT_MEDIUM_REMOVAL failed\n");
        eLIBs_fclose(pfile);
        return EPDK_FAIL;
    }

    eLIBs_fclose(pfile);
    return EPDK_OK;
}

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
__s32 USB_AllowMediumRemoval(char *Path)
{
    ES_FILE *pfile = NULL;
    __s32 ret = 0;
    pfile = eLIBs_fopen(Path, "r+");

    if (pfile == NULL)
    {
        __wrn("ERR: fopen %s failed\n", Path);
        return EPDK_FAIL;
    }

    ret = eLIBs_fioctrl(pfile,
                        USB_STORAGE_USER_CTRL_CMD_CDROM_ALLOW_MEDIUM_REMOVAL,
                        0,
                        NULL);

    if (ret != EPDK_OK)
    {
        __wrn("ERR: execute USB_STORAGE_USER_CTRL_CMD_CDROM_ALLOW_MEDIUM_REMOVAL failed\n");
        eLIBs_fclose(pfile);
        return EPDK_FAIL;
    }

    eLIBs_fclose(pfile);
    return EPDK_OK;
}





