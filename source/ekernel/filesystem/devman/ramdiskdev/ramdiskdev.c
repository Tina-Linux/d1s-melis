/*
*********************************************************************************************************
*                                                   ePOS
*                               the Easy Portable/Player Operation System
*                                          module sub-system
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File   : ramdev.c
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/
#include "ramdiskdev.h"
#include <string.h>
#include <log.h>
#include <kapi.h>

extern uint8_t      *melis_ramdisk_base;
extern uint32_t     melis_ramdisk_size;

/***************************************************************************************************
*
*             #define constants
*/
/*
  Define size of your RAM disk here.
  You specify the number of sectors (512 bytes) here.
*/
//#define RAMDISK_BLK_NUMBER      (64*1024)     /* 32M RAM */
#define RAMDISK_BLK_NUMBER      (melis_ramdisk_size >> 9)
/*
  Do not change following define !
  It may become configurable in future versions of the file system.
*/
#define RAMDISK_BLK_SIZE        0x200     /* do not change for FAT */

/***************************************************************************************************
*
*             Local Variables
*/

//static char _array[(long)FSYS_RR_BLOCKNUM * FSYS_RR_BLOCKSIZE];

#define RAMDISK_ARRAY   (melis_ramdisk_base)

static __dev_devop_t    ramdiskdev_op;
static __hdle           ramdiskhandle;
static struct
{
    uint32_t used;
} ramdiskdev;
/***************************************************************************************************
*
*             Local functions
*/

/*******************************************************************************
*
*             _FS_RAM_DevRead
*
  Description:
  FS driver function. Read a sector from the RAM disk.

  Parameters:
  Unit        - Unit number.
  Sector      - Sector to be read from the device.
  pBuffer     - Pointer to buffer for storing the data.

  Return value:
  ==0         - Sector has been read and copied to pBuffer.
  <0          - An error has occured.
*/
int32_t ramdiskdev_init(void)
{
    ramdiskdev.used = 0;
    ramdiskhandle = esDEV_DevReg("DISK", "RAMDISK", &ramdiskdev_op, 0);
    if (ramdiskhandle)
    {
        return EPDK_OK;
    }

    __err("Ramdisk registered Error!");
    return EPDK_FAIL;
}
/*******************************************************************************
*
*             _FS_RAM_DevRead
*
  Description:
  FS driver function. Read a sector from the RAM disk.

  Parameters:
  Unit        - Unit number.
  Sector      - Sector to be read from the device.
  pBuffer     - Pointer to buffer for storing the data.

  Return value:
  ==0         - Sector has been read and copied to pBuffer.
  <0          - An error has occured.
*/

int32_t ramdiskdev_exit(void)
{
    return esDEV_DevUnreg(ramdiskhandle);
}

/*******************************************************************************
*
*             ramdev_open
*
  Description:
  FS driver function. Read a sector from the RAM disk.

  Parameters:
  Unit        - Unit number.
  Sector      - Sector to be read from the device.
  pBuffer     - Pointer to buffer for storing the data.

  Return value:
  ==0         - Sector has been read and copied to pBuffer.
  <0          - An error has occured.
*/

static __hdle ramdiskdev_open(void *open_arg, uint32_t mode)
{
    if (ramdiskdev.used == 0)
    {
        ramdiskdev.used = 1;
        return (__hdle)&ramdiskdev;
    }
    return 0;

}
/*******************************************************************************
*
*             _FS_RAM_DevRead
*
  Description:
  FS driver function. Read a sector from the RAM disk.

  Parameters:
  Unit        - Unit number.
  Sector      - Sector to be read from the device.
  pBuffer     - Pointer to buffer for storing the data.

  Return value:
  ==0         - Sector has been read and copied to pBuffer.
  <0          - An error has occured.
*/

static int32_t ramdiskdev_close(__hdle hDev)
{
    if (ramdiskdev.used == 0)
    {
        return EPDK_FAIL;
    }

    ramdiskdev.used = 0;

    return EPDK_OK;
}


/*******************************************************************************
*
*             _FS_RAM_DevRead
*
  Description:
  FS driver function. Read a sector from the RAM disk.

  Parameters:
  Unit        - Unit number.
  Sector      - Sector to be read from the device.
  pBuffer     - Pointer to buffer for storing the data.

  Return value:
  ==0         - Sector has been read and copied to pBuffer.
  <0          - An error has occured.
*/

static uint32_t ramdiskdev_read(void *pBuffer, uint32_t blk, uint32_t n, __hdle hDev)
{
    if (blk >= RAMDISK_BLK_NUMBER)
    {
        return 0;  /* Out of physical range */
    }
    if (blk + n >= RAMDISK_BLK_NUMBER)
    {
        n = RAMDISK_BLK_NUMBER - blk;
    }

    memcpy(pBuffer, (char *)(RAMDISK_ARRAY + blk * RAMDISK_BLK_SIZE), n * RAMDISK_BLK_SIZE);
    return n;
}


/*********************************************************************
*
*             _FS_RAM_DevWrite
*
  Description:
  FS driver function. Write sector to the RAM disk.

  Parameters:
  Unit        - Unit number.
  Sector      - Sector to be written to the device.
  pBuffer     - Pointer to data to be stored.

  Return value:
  ==0         - Sector has been written to the device.
  <0          - An error has occured.
*/

static uint32_t ramdiskdev_write(const void *pBuffer, uint32_t blk, uint32_t n, __hdle hDev)
{
    if (blk >= RAMDISK_BLK_NUMBER)
    {
        return 0;  /* Out of physical range */
    }
    if (blk + n >= RAMDISK_BLK_NUMBER)
    {
        n = RAMDISK_BLK_NUMBER - blk;
    }

    memcpy((char *)(RAMDISK_ARRAY + blk * RAMDISK_BLK_SIZE), pBuffer, n * RAMDISK_BLK_SIZE);
    return n;
}


/*********************************************************************
*
*             _FS_RAM_DevIoCtl
*
  Description:
  FS driver function. Execute device command.

  Parameters:
  Unit        - Unit number.
  Cmd         - Command to be executed.
  Aux         - Parameter depending on command.
  pBuffer     - Pointer to a buffer used for the command.

  Return value:
  Command specific. In general a negative value means an error.
*/

static uint32_t ramdiskdev_ioctrl(__hdle hDev, uint32_t Cmd, long Aux, void *pBuffer)
{
    __dev_ramdiskdev_t *pDev = (__dev_ramdiskdev_t *)hDev;

    //pDev = pDev; //comment it for code static check reporting assignment of variale fo itself
    //Aux  = Aux;  /* Get rid of compiler warning */

    switch (Cmd)
    {
        case DEV_IOC_USR_GET_GEO:
        {
            if (!pBuffer)
            {
                return EPDK_FAIL;
            }
            ((__dev_blkinfo_t *)pBuffer)->hiddennum = 0;
            ((__dev_blkinfo_t *)pBuffer)->headnum   = 2;
            ((__dev_blkinfo_t *)pBuffer)->secnum    = 4;
            ((__dev_blkinfo_t *)pBuffer)->partsize  = RAMDISK_BLK_NUMBER;
        }
        return EPDK_OK;

        /**************************old ºÊ»›  ********************/
        case DEV_CMD_GET_INFO:
        {
            if (!pBuffer)
            {
                return EPDK_FAIL;
            }
            ((__dev_blkinfo_t *)pBuffer)->hiddennum = 0;
            ((__dev_blkinfo_t *)pBuffer)->headnum   = 2;
            ((__dev_blkinfo_t *)pBuffer)->secnum    = 4;
            ((__dev_blkinfo_t *)pBuffer)->partsize  = RAMDISK_BLK_NUMBER;
        }
        return EPDK_OK;

        case DEV_CMD_GET_STATUS:
            return 0;
        /**************************old ºÊ»›  ********************/

        default:
            break;
    }
    return 0;
}


/*********************************************************************
*
*             Global variables
*
**********************************************************************
*/
static __dev_devop_t ramdiskdev_op =
{
    ramdiskdev_open,
    ramdiskdev_close,
    ramdiskdev_read,
    ramdiskdev_write,
    ramdiskdev_ioctrl
};
