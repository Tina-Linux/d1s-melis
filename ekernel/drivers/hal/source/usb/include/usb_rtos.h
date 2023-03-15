/*-------------------------------------------------------------------------*/
/*
 * Only for FreeRtos special define
 *
 *-------------------------------------------------------------------------*/

#ifndef __USB_RTOS_H
#define __USB_RTOS_H

#include "errno.h"
#include "devfs.h"

#define RTOS_USB_DEBUG

/*-------------------------------------------------------------*/
/* debug redefine */

#define __err		hal_log_err
#define __wrn		hal_log_warn
#define __inf		hal_log_info
#define __msg		hal_log_debug

/*-------------------------------------------------------------*/
/* some special definition that system dosn't has */

#define MAX_ERRNO			4095
#define IS_ERR_VALUE(x)			((x) >= (unsigned long)-MAX_ERRNO)

#define PTR_ERR(x)			((long)(x))
#define ERR_PTR(x)			((void *)(x))
#define IS_ERR(x)			(IS_ERR_VALUE((unsigned long)x))

#define ECOMM				70  /* Communication error on send */
#define ESHUTDOWN			110 /* Can't send after socket shutdown */
#define EREMOTEIO			151 /* Remote I/O error */

#define EPDK_OK				(0)
#define EPDK_FAIL			(-1)

#define OS_NO_ERR			0u

#undef absolute
#define absolute(p)			((p) > 0 ? (p) : -(p))

/*-------------------------------------------------------------*/
/* system device IO control define */

#define DEV_CMD_GET_INFO		0
#define DEV_CMD_GET_INFO_AUX_SIZE	0
#define DEV_CMD_GET_STATUS		1
#define DEV_CMD_GET_OFFSET		2

#define DEV_CLASS_DISK			"DISK"
#define DEV_CLASS_USERDEF		"USERDEF"

/* user defined device ioctrol cmd: 0x00000000~0x7FFFFFFF */
/* system defined device ioctrol cmd: 0x80000000~0xFFFFFFFF */
#define DEV_IOC_USR_BASE		0x00000000
#define DEV_IOC_SYS_BASE		0x80000000
#define IS_DEVIOCSYS(cmd)		(cmd>=DEV_IOC_SYS_BASE)
/* sys */
#define DEV_IOC_SYS_GET_DEVNAME		(DEV_IOC_SYS_BASE + 0)
#define DEV_IOC_SYS_GET_CLSNAME		(DEV_IOC_SYS_BASE + 1)
#define DEV_IOC_SYS_GET_ATTRIB		(DEV_IOC_SYS_BASE + 2)
#define DEV_IOC_SYS_GET_LETTER		(DEV_IOC_SYS_BASE + 3)
#define DEV_IOC_SYS_GET_OPENARGS	(DEV_IOC_SYS_BASE + 4)

/* dms */
#define DEV_IOC_USR_GET_DEVITEM		(DEV_IOC_USR_BASE + 102)
#define DEV_IOC_USR_GET_DEVROOT		(DEV_IOC_USR_BASE + 103)
/* blk */
#define DEV_IOC_USR_GET_GEO		(DEV_IOC_USR_BASE + 104)
#define DEV_IOC_USR_FLUSH_CACHE		(DEV_IOC_USR_BASE + 105)
#define DEV_IOC_USR_PHY_READ		(DEV_IOC_USR_BASE + 106)
#define DEV_IOC_USR_PHY_WRITE		(DEV_IOC_USR_BASE + 107)
#define DEV_IOC_USR_PHY_ERASE		(DEV_IOC_USR_BASE + 108)
#define DEV_IOC_USR_LOGIC_WRITE		(DEV_IOC_USR_BASE + 109)
#define DEV_IOC_USR_LOGIC_READ		(DEV_IOC_USR_BASE + 110)
#define DEV_IOC_USR_GET_HW_INFO		(DEV_IOC_USR_BASE + 111)
#define DEV_IOC_USR_BLK_ERASE		(DEV_IOC_USR_BASE + 112)
#define DEV_IOC_USR_SECTOR_ERASE	(DEV_IOC_USR_BASE + 113)
#define DEV_IOC_USR_FLUSH_BLOCK_CACHE	(DEV_IOC_USR_BASE + 114)


/* cd-rom */
#define DEV_CDROM_LAST_WRITTEN		(DEV_IOC_USR_BASE + 120)  /* get last block written on disc */
#define DEV_CDROM_MULTISESSION		(DEV_IOC_USR_BASE + 121)  /* Obtain the start-of-last-session \
								   * address of multi session disks, \
								   * address type is logical block. */
typedef  struct __DEV_BLKINFO
{
	uint32_t hiddennum;
	uint32_t headnum;
	uint32_t secnum;
	uint32_t partsize;
	uint32_t secsize;
} __dev_blkinfo_t;

#ifndef DATA_TYPE_X___hdle
#define DATA_TYPE_X___hdle
typedef void *__hdle;
#endif

typedef struct __DEV_DEVOP {
	__hdle (*Open)(void *open_arg, uint32_t mode);
	int32_t (*Close)(__hdle hDev);
	uint32_t (*Read)(void * /*pBuffer*/,
			 uint32_t /*chrdev: 1,blkdev: sector pos*/,
			 uint32_t /*chrdev: byte cnt, blkdev: sector cnt*/,
			 __hdle /*hDev*/);
	uint32_t (*Write)(const void * /*pBuffer*/,
			  uint32_t /*chrdev: 1, blkdev: sector pos*/,
			  uint32_t /*chrdev: byte cnt, blkdev: sector cnt*/,
			  __hdle /*hDev*/);
	int32_t (*Ioctl)(__hdle hDev, uint32_t Cmd, long Aux, void *pBuffer);
} __dev_devop_t;

#endif /* __USB_RTOS_H */
