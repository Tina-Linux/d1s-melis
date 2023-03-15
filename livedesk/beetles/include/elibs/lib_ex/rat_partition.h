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
#ifndef __RAT_PARTITION_H
#define __RAT_PARTITION_H
#include <kconfig.h>

#define RAT_CD_ROM      "ExternalDisk2"
#define RAT_USB_DISK    "ExternalDisk1"
#define RAT_SD_CARD     "ExternalDisk0"
#define RAT_LOCAL_DISK  "LocalDisk"

#define RAT_MAX_PARTITION       11

/*!
* \brief
*     分区属性
*/
typedef enum
{
    RAT_READONLY        = 0x01,     //只读
    RAT_READWRITE                   //可读写
} rat_pati_attr_t;


/*
**********************************************************************************************************************
*
*
* author:
*
* date:
*
* Description:       获得盘符信息
*
* parameters:       disk_type: RAT_SD_CARD RAT_USB_DISK RAT_LOCAL_DISK
                    disk_name: 返回的盘符名
*
* return:           0   get part success
                    -1  part not exist
                    -2  part not formatted
* modify history:
**********************************************************************************************************************
*/
__s32 rat_get_partition_name(char *disk_type, char disk_name[RAT_MAX_PARTITION][4], __s32 search_encrypt_card);
__u32 rat_is_encryption_partition_Insert(char *eq_name, __s32 encryption);

#endif//__RAT_PARTITION_H
