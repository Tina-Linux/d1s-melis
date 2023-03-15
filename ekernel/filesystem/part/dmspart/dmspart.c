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
#include "dmspart.h"
#include "fsys_libs.h"
#include <string.h>
#include <sys_device.h>
#include <sys_fsys.h>
#include <kapi.h>

/*
**********************************************************************************************************************
*
*             dmspart_read
*
  Description:
  FS internal function. Read sector from device.

  Parameters:
  pDriver     - Pointer to a device driver structure.
  Unit        - Unit number.
  Sector      - Sector to be read from the device.
  pBuffer     - Pointer to buffer for storing the data.

  Return value:
  ==0         - Sector has been read and copied to pBuffer.
  <0          - An error has occured.
**********************************************************************************************************************
*/

static uint32_t dmspart_read(void *pData, uint32_t Sector, uint32_t N, __fsys_part_t *pPart)
{
    return 0;
}


/*
**********************************************************************************************************************
*
*             dmspart_write
*
*  Description:
*   FS internal function. Write sector to device.
*
*  Parameters:
*   pDriver     - Pointer to a device driver structure.
*   Sector      - Sector to be written to the device.
*   pBuffer     - Pointer to data to be stored.
*
*  Return value:
*   ==0         - Sector has been written to the device.
*   <0          - An error has occured.
**********************************************************************************************************************
*/
static uint32_t dmspart_write(const void *pData, uint32_t Sector, uint32_t N, __fsys_part_t *pPart)
{
    return 0;
}

/*
**********************************************************************************************************************
*
*             dmspart_ioctl
*
  Description:
  FS internal function. Execute device command.

  Parameters:
  pPart       - Pointer to a partition driver structure.
  Cmd         - Command to be executed.
  Aux         - Parameter depending on command.
  pBuffer     - Pointer to a buffer used for the command.

  Return value:
  Command specific. In general a negative value means an error.
**********************************************************************************************************************
*/
static int32_t dmspart_ioctl(__fsys_part_t *pPart, uint32_t Cmd, long Aux, void *pBuffer)
{
    int32_t         x;
    __hdle          hDev;

    hDev = pPart->hDev;
    if (!hDev)
    {
        return EPDK_FAIL;
    }

    switch (Cmd)
    {
        case PART_IOC_USR_GETITEM:              /* 获取信息                     */
            esDEV_Lock(pPart->hNode);
            x = esDEV_Ioctl(hDev, DEV_IOC_USR_GET_DEVITEM, Aux, pBuffer);
            esDEV_Unlock(pPart->hNode);
            return x;
        case PART_IOC_USR_GETDEVCLSROOT:
            *((long *)pBuffer) = (long)pPart->hPDPrivate;
            return EPDK_OK;
        case PART_IOC_USR_FLUSHCACHE:
            return EPDK_OK;
        default:
            return EPDK_FAIL;
    }
}

/*
**********************************************************************************************************************
*
*             dmspart_mount
*
  Description:


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/
static int32_t dmspart_mount(__fsys_part_t *pPart)
{
    int32_t         x;
    __hdle          hDev;
    __hdle          hDevRoot;

    hDev    = pPart->hDev;
    if (!hDev)
    {
        return EPDK_FAIL;
    }

    esDEV_Lock(pPart->hNode);

    x = esDEV_Ioctl(hDev, DEV_IOC_USR_GET_DEVROOT, 0, &hDevRoot);

    esDEV_Unlock(pPart->hNode);

    if (x == EPDK_OK)
    {
        pPart->hPDPrivate = hDevRoot;
    }
    else
    {
        pPart->hPDPrivate = 0;
    }

    return x;
}
/*
**********************************************************************************************************************
*
*             dmspart_unmount
*
  Description:


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/
static int32_t dmspart_unmount(__fsys_part_t *pPart, int32_t force)
{
    int32_t     res;

    res = deactivate_part(pPart, force);

    if (res == EPDK_OK || force)
    {
        pPart->status = FSYS_PARTSTATUS_DEAD;
    }

    return res;
}

/*
**********************************************************************************************************************
*
*             dmspart_identify
*
  Description:识别分区，如果hDev所指向的块设备为普通磁盘分区格式，那么返回EPDK_OK


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/
static uint32_t dmspart_identify(__hdle hDev)
{
    int32_t     res;
    char        *className;

    res = esDEV_Ioctl(hDev, DEV_IOC_SYS_GET_CLSNAME, 0, &className);

    if (res == EPDK_FAIL)
    {
        return 0;
    }
    if (strcmp(className, DEV_CLASS_DMS))
    {
        return 0;
    }

    return 1;
}

__fsys_pd_t dmspart_type =
{
    .name   = "dmsPart",
    .Ops    = {
        .read       = dmspart_read,
        .write      = dmspart_write,
        .ioctl      = dmspart_ioctl,
        .mount      = dmspart_mount,
        .unmount    = dmspart_unmount,
        .identify   = dmspart_identify,
        .fdisk      = 0
    }
};

/*
**********************************************************************************************************************
*             Global functions
**********************************************************************************************************************
*
* Functions here are global, although their names indicate a local
  scope. They should not be called by user application.
*/

/*
**********************************************************************************************************************
*
*             fsys_dmspart_init
*
  Description:


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/

int32_t fsys_dmspart_init(void)
{
    return esFSYS_pdreg((__hdle)&dmspart_type);
}

/*
**********************************************************************************************************************
*
*             fsys_dmspart_exit
*
  Description:


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/
int32_t fsys_dmspart_exit(void)
{
    return esFSYS_pdunreg((__hdle)&dmspart_type);
}