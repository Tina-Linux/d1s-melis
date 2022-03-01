/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : part.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-15
* Descript: partition management of file system.
* Update  : date                auther      ver     notes
*           2011-3-15 14:58:06  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __PART_H__
#define __PART_H__
#include <sys_fsys.h>
#include <sys_device.h>
#include "sys_fsys_i.h"


typedef struct __FSYS_PART              __fsys_part_t;      /* 分区         */
typedef struct fix_part_s               __fix_part_t;
typedef struct __FSYS_PD                __fsys_pd_t;        /* 分区驱动句柄 */
typedef struct __FSYS_PDOPS             __fsys_pdops_t;     /* 分区操作     */

#define     FSYS_PARTSTATUS_UNUSED      (1<<0)      /* 文件系统使用    */
#define     FSYS_PARTSTATUS_FSUSED      (1<<1)      /* 文件系统使用    */
#define     FSYS_PARTSTATUS_RAWUSED     (1<<2)      /* 直接分区使用    */
#define     FSYS_PARTSTATUS_DEAD        (1<<3)      /* 此分区已经被卸载*/
#define     FSYS_PARTSTATUS_INVALID     (FSYS_PARTSTATUS_UNUSED | FSYS_PARTSTATUS_DEAD)

#define     FSYS_PARTATTR_OPENRWM       0x03
#define     FSYS_PARTATTR_OPENR         (1<<0)      /* 分区以可读打开 */
#define     FSYS_PARTATTR_OPENW         (1<<1)      /* 分区以可读打开 */
#define     FSYS_PARTATTR_DEVRWM        0x0c
#define     FSYS_PARTATTR_DEVR          (1<<2)      /* 分区自带的可读属性 */
#define     FSYS_PARTATTR_DEVW          (1<<3)      /* 分区自带的可写属性 */
#define     FSYS_PARTATTR_FS            (1<<4)      /* 分区带有可识别的文件系统 */

#define     PD_STAT_INACTIVE            0x00
#define     PD_STAT_ACTIVE              0x01

/*
**********************************************************************************************************************
* struct: part
*/
struct __FSYS_PDOPS
{
    uint32_t    (*read)(void *pData, uint32_t Sector, uint32_t N, __fsys_part_t *pPart);
    uint32_t    (*write)(const void *pData, uint32_t Sector, uint32_t N, __fsys_part_t *pPart);
    int32_t     (*ioctl)(__fsys_part_t *pPart, uint32_t Cmd, long Aux, void *pBuffer);
    int32_t     (*mount)(__fsys_part_t *pPart);
    int32_t     (*unmount)(__fsys_part_t *pPart, int32_t force);
    uint32_t    (*identify)(__hdle hDev);
    int32_t     (*fdisk)(__hdle hDev);
};

struct __FSYS_PD
{
    char                    *name;
    uint32_t                nUsr;
    uint32_t                status;         /* 节点状态：0表示正常，0xff表示为死节点（驱动程序代码已经卸载）*/
    __fsys_pd_t             *next;
    __fsys_pdops_t          Ops;            /* 分区操作函数集 */
    //uint32_t                last_lun;   //是否u盘或卡的最后一个盘符
};

struct __FSYS_PART
{
    char                    dname[MAX_PART_NAME_LEN];   /* 分区名 */
    char                    letter;                     /* 盘符 */
    uint32_t                status;                     /* 见常量定义FSYS_PARTSTATUS_XXX */
    int32_t                 error;                      /* 分区操作出错的纪录 */
    int32_t                 attr;                       /* access mode */
    __fsys_pd_t             *pPD;                       /* 分区驱动 */
    __hdle                  hPDPrivate;                 /* 分区驱动私有数据块句柄 */
    __hdle                  hFSPrivate;                 /* 文件系统私有数据块句柄 */
    __hdle                  hNode;                      /* 设备节点句柄 */
    __hdle                  hDev;                       /* 设备驱动的操作句柄 */
    uint32_t                Unit;                       /* 设备内分区号 */
    __bool                  updateflag;                 /* 分区更新标记 */
    uint32_t                last_lun;                   //是否u盘或卡的最后一个盘符
};

struct fix_part_s
{
    char                    letter;
    struct __FSYS_PART      *part;
};

extern __hdle   pPartSem;

extern int32_t  deactivate_part(__fsys_part_t *part, int32_t force);

#endif  /* __PART_H__ */
