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
