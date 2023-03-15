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