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
#include "hwscdev.h"
#include <sys_device.h>
#include <sys_fsys.h>
#include <string.h>
#include <log.h>

static __dev_devop_t    hwscdev_ops = {0};
static struct hwscdev   hwsc = {0};

static __hdle   hwschandle  = NULL;
static __hdle   hMonitor    = NULL;

int32_t hwscdev_init(void)
{
    //register "hwsc" device
    hwschandle = esDEV_DevReg(DEV_CLASS_HWSC, DEV_NAME_HWSC, &hwscdev_ops, 0);
    if (!hwschandle)
    {
        __err("hwscdev registered Error!");
        return EPDK_FAIL;
    }

    //plug-in mointor
    esDEV_Plugin("\\drv\\monitor.drv", 0, 0, 1);

    //open monitor
    hMonitor = esFSYS_fopen("b:\\HWSC\\monitor", "rb+");
    if (!hMonitor)
    {
        __err("hwsc try to open monitor failed!");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

int32_t hwscdev_exit(void)
{
    //close monitor
    if (hMonitor)
    {
        esFSYS_fclose((__hdle)hMonitor);
        hMonitor = 0;
    }

    //plugout monitor
    esDEV_Plugout("\\drv\\monitor.drv", 0);

    return esDEV_DevUnreg(hwschandle);
}


__hdle hwscdev_Open(void *open_arg, uint32_t mode)
{
    hwsc.inuse++;
    return (__hdle)&hwsc;
}

int32_t hwscdev_Close(__hdle hDev)
{
    if (hwsc.inuse)
    {
        hwsc.inuse--;
        return EPDK_OK;
    }
    return EPDK_FAIL;
}

uint32_t hwscdev_Read(void *pBuffer, uint32_t size, uint32_t n, __hdle hDev)
{
    return 0;
}
uint32_t hwscdev_Write(const void *pBuffer, uint32_t size, uint32_t n, __hdle hDev)
{
    return 0;
}

int32_t hwscdev_Ioctrl(__hdle hDev, uint32_t Cmd, int32_t Aux, void *pBuffer)
{
    //send command to monitor
    if (hMonitor)
    {
        return esFSYS_fioctrl(hMonitor, Cmd, Aux, pBuffer);
    }

    return EPDK_FAIL;
}

static __dev_devop_t hwscdev_ops =
{
    hwscdev_Open,
    hwscdev_Close,
    hwscdev_Read,
    hwscdev_Write,
    hwscdev_Ioctrl
};