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
#include "rawpart.h"
#include "fsys_debug.h"

/*
**********************************************************************************************************************
*
*             part_read
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
uint32_t rawpart_read(void *pData, uint32_t Sector, uint32_t N, __fsys_part_t *pPart)
{
    int32_t     x;
    __hdle      hDev;

    if (!pPart)
    {
        return 0;
    }

    hDev = pPart->hDev;
    if (hDev)
    {
        esDEV_Lock(pPart->hNode);
        x = esDEV_Read(pData, Sector, N, hDev);
        esDEV_Unlock(pPart->hNode);
        return  x;
    }
    return 0;
}


/*
**********************************************************************************************************************
*
*             part_write
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
uint32_t rawpart_write(const void *pData, uint32_t Sector, uint32_t N, __fsys_part_t *pPart)
{
    int32_t     x;
    __hdle      hDev;

    if (!pPart)
    {
        return 0;
    }

    hDev = pPart->hDev;
    if (hDev)
    {
        esDEV_Lock(pPart->hNode);
        x = esDEV_Write(pData, Sector, N, hDev);
        esDEV_Unlock(pPart->hNode);
        return  x;
    }
    return 0;
}

/*
**********************************************************************************************************************
*
*             part_ioctl
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

int32_t  rawpart_ioctl(__fsys_part_t *pPart, uint32_t Cmd, long Aux, void *pBuffer)
{
    int32_t             x;
    __hdle              hDev;
    __fsys_rawpart_p_t  *pPDPrivate  = (__fsys_rawpart_p_t *)(pPart->hPDPrivate);

    if (!pPart)
    {
        return 0;
    }

    hDev = pPart->hDev;
    if (!hDev)
    {
        return EPDK_FAIL;
    }

    switch (Cmd)
    {
        case PART_IOC_USR_FLUSHCACHE:
        {
            esDEV_Lock(pPart->hNode);
            esDEV_Ioctl(pPart->hDev, DEV_IOC_USR_FLUSH_CACHE, 0, NULL);
            esDEV_Unlock(pPart->hNode);
            break;
        }

        case PART_IOC_USR_GETPARTSIZE:
        {
            *((uint32_t *)pBuffer)  = pPDPrivate->partseccnt;
            break;
        }

        case PART_IOC_USR_GETSCTSIZE:
        {
            *((uint32_t *)pBuffer)  = pPDPrivate->partsecsize;
            break;
        }
        /***********************************old******************/
        case FSYS_PART_CMD_GET_STATUS:              /* 获取状态信息                 */
        {
            return 0;
        }

        case FSYS_PART_CMD_GET_INFO:                /* 获取信息                     */
        {
            esDEV_Lock(pPart->hNode);
            x = esDEV_Ioctl(hDev, DEV_CMD_GET_INFO, Aux, pBuffer);
            esDEV_Unlock(pPart->hNode);
            return x;
        }

        case FSYS_PART_CMD_INC_BUSYCNT:             /*  */
        {
            break;
        }

        case FSYS_PART_CMD_DEC_BUSYCNT:             /*  */
        {
            break;
        }

        case FSYS_PART_CMD_FLUSH_CACHE:
        {
            esDEV_Lock(pPart->hNode);
            esDEV_Ioctl(pPart->hDev, DEV_IOC_USR_FLUSH_CACHE, 0, NULL);
            esDEV_Unlock(pPart->hNode);
            break;
        }

        case FSYS_PART_CMD_GET_PARTSIZE:
        {
            esDEV_Lock(pPart->hNode);
            *((uint32_t *)pBuffer)  = pPDPrivate->partseccnt;
            esDEV_Unlock(pPart->hNode);
            break;
        }
        /***********************************old******************/
        default:
        {
            return EPDK_FAIL;
        }
    }
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*
*             npart_mount
*
  Description:


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/

int32_t  rawpart_mount(__fsys_part_t *pPart)
{
    __dev_blkinfo_t     blkinf;
    __fsys_rawpart_p_t  *pPDPrivate  = NULL;


    /* 分配私有的数据空间       */
    pPart->hPDPrivate   = (__hdle)malloc(sizeof(__fsys_rawpart_p_t));
    pPDPrivate          = (__fsys_rawpart_p_t *)(pPart->hPDPrivate);

    if (!(pPart->hPDPrivate))
    {
        fs_log_info("mount part fail!\n");
        return EPDK_FAIL;
    }

    esDEV_Lock(pPart->hNode);

    /* 计算出分区的信息         */
    esDEV_Ioctl(pPart->hDev, DEV_CMD_GET_INFO, 0, &blkinf);
    pPDPrivate->partaddr        = 0;
    pPDPrivate->partseccnt      = blkinf.partsize;
    pPDPrivate->partsecsize     = blkinf.secsize;

    esDEV_Unlock(pPart->hNode);

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*
*             npart_unmount
*
  Description:


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/

int32_t rawpart_unmount(__fsys_part_t *pPart, int force)
{
    int32_t  res;

    res = deactivate_part(pPart, force);
    if (res == EPDK_OK || force)
    {
        free((void *)pPart->hPDPrivate);
    }

    return res;
}
/*
**********************************************************************************************************************
*
*             rawpart_identify
*
  Description:识别分区，如果hDev所指向的块设备为普通磁盘分区格式，那么返回EPDK_OK


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/
uint32_t rawpart_identify(__hdle hDev)
{
    int32_t     res;
    char        *className;

    res = esDEV_Ioctl(hDev, DEV_IOC_SYS_GET_CLSNAME, 0, &className);
    if (res == EPDK_FAIL)
    {
        return 0;
    }
    if (!strcmp(className, DEV_CLASS_DISK))
    {
        return 1;
    }

    return 0;
}
/*
**********************************************************************************************************************
*
*             rawpart_fdisk
*
  Description:分区


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/
int32_t rawpart_fdisk(__hdle hDev)
{
    return EPDK_FAIL;
}

__fsys_pd_t rawpart_type =
{
    .name   = "rawpart",
    .Ops    = {
        .read       = rawpart_read,
        .write      = rawpart_write,
        .ioctl      = rawpart_ioctl,
        .mount      = rawpart_mount,
        .unmount    = rawpart_unmount,
        .identify   = rawpart_identify,
        .fdisk      = rawpart_fdisk,
    }
};


/*
**********************************************************************************************************************
*
*             fsys_npart_init
*
  Description:


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/

int32_t fsys_rawpart_init(void)
{
    return esFSYS_pdreg((__hdle)&rawpart_type);
}

/*
**********************************************************************************************************************
*
*             fsys_npart_exit
*
  Description:


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/

int32_t fsys_rawpart_exit(void)
{
    return esFSYS_pdunreg((__hdle)&rawpart_type);
}