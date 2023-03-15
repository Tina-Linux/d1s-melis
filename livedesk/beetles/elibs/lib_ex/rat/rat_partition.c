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
#include <log.h>
#include <libc.h>
#include "rat_partition.h"
#include <kapi.h>
#include <kconfig.h>
/*
**********************************************************************************************************************
*                                               get_part_by_name
*
* author:          terry.zeng
*
* date:             2010-09-13
*
* Description:       获取分区
*
* parameters:       target：输入， 目标分区 res_part：输出，返回的分区名
*
* return:           0   get part success
-1  part not exist
-2  part not formatted
* modify history:
**********************************************************************************************************************
*/
static int32_t get_part_by_name(const char *equipment, char *des, int32_t search_encrypt_card)
{
    int32_t     ret = -1;
    int32_t     i = 0;
    int32_t     status = 0;
    char        equipment_name[MAX_FS_NAME_LEN];
    char        part_name[RAT_MAX_PARTITION][4];
    char        *partition[RAT_MAX_PARTITION] = {"e:", "f:", "g:", "h:", "i:", "j:", "k:", "l:", "m:", "n:", "o:"};

    ret = -2;
    __inf("============get_part_by_name");
    eLIBs_memset(part_name, 0, RAT_MAX_PARTITION * 4);

    for (i = 0; i < RAT_MAX_PARTITION; i++)
    {
        eLIBs_memset(equipment_name, 0, MAX_FS_NAME_LEN);
        eLIBs_GetPTName(partition[i], equipment_name);
        __wrn("device_name=%s,device=%s,strlen=%d", equipment_name, equipment, eLIBs_strlen(equipment));

        if (eLIBs_strncmp(equipment_name, equipment, eLIBs_strlen(equipment)) == 0)
        {
            if (0 == search_encrypt_card && eLIBs_strncmp(equipment_name, "SDMMC-DISK:5", eLIBs_strlen("SDMMC-DISK:5")) == 0)
            {
                //如果不包含加密卡，并且当前是加密卡，则不记录
            }
            else
            {
                __inf("partition[%d] = %s", i, partition[i]);
                status  = eLIBs_IsPartFormated(partition[i]);

                if (status == ELIBS_PART_FMT_FORMATED)
                {
                    __inf("found part:%s", partition[i]);
                    eLIBs_strcpy(part_name[i], partition[i]);
                    ret = 0;
                }
                else
                {
                }
            }
        }
    }

    eLIBs_memcpy(des, part_name, RAT_MAX_PARTITION * 4);
    return ret;
}

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
* return:           partition count
                    -1  part not exist
* modify history:
**********************************************************************************************************************
*/
int32_t rat_get_partition_name(char *disk_type, char disk_name[RAT_MAX_PARTITION][4], int32_t search_encrypt_card)
{
    char                *equipment[4]   = {"UDISK", "SDMMC-DISK", "SCSI_DISK", "USB_CDROM"};
    char                *usb_cdfs       = "USB_CDROM";
    int32_t             equipment_index = 0;
    char                name[RAT_MAX_PARTITION][4];
    int32_t             i;
    int32_t             j;

    __wrn("rat_get_partition_name, disk_type=%s", disk_type);

    eLIBs_memset(disk_name, 0, RAT_MAX_PARTITION * 4);
    eLIBs_memset(name,      0, sizeof(name));

    if (eLIBs_strcmp(disk_type, RAT_LOCAL_DISK) == 0)
    {
        __wrn("==============RAT_LOCAL_DISK");
        equipment_index = 0;
    }
    else if (eLIBs_strcmp(disk_type, RAT_SD_CARD) == 0)
    {
        __wrn("==============RAT_SD_CARD");
        equipment_index = 1;
    }
    else if (eLIBs_strcmp(disk_type, RAT_USB_DISK) == 0)
    {
        __wrn("==============RAT_USB_DISK");
        equipment_index = 2;
    }
    else if (eLIBs_strcmp(disk_type, RAT_CD_ROM) == 0)
    {
        __wrn("==============RAT_CD_ROM");
        equipment_index = 3;
    }
    else
    {
        __inf("*******************no match disk type");
        return EPDK_FAIL;
    }

    j = 0;

    if (get_part_by_name(equipment[equipment_index], (char *)name, search_encrypt_card) == 0)
    {
        for (i = 0; i < RAT_MAX_PARTITION; i++)
        {
            if (name[i][0])
            {
                __inf("name[i]=%s", name[i]);
                eLIBs_strcpy(disk_name[j], name[i]);
                j++;
            }
            else
            {
                __inf("********* name[i][0] null ");
            }
        }
    }
    else
    {
        __inf("************* get part by name fail ");
    }

    //如果是u盘，检测cdfs
    if (eLIBs_strcmp(disk_type, RAT_USB_DISK) == 0)
    {
        if (get_part_by_name(usb_cdfs, (char *)name, 0) == 0)
        {
            for (i = 0; i < RAT_MAX_PARTITION; i++)
            {
                if (name[i][0])
                {
                    __inf("cdfs part found, name[i]=%s", name[i]);
                    eLIBs_strcpy(disk_name[j], name[i]);
                    j++;
                }
                else
                {
                    __inf("********* name[i][0] null ");
                }
            }
        }
        else
        {
            __inf("********** get_part_by_name ");
        }
    }
    else
    {
        __inf("*************** not match ");
    }

    return EPDK_OK;
}

//判断是否存在加密分区，是则将盘符存入eq_name,并返回真
//eq_name 盘符存放buffer  ;  encryption 1＝加密区，0＝非加密区
uint32_t rat_is_encryption_partition_Insert(char *eq_name, int32_t encryption)
{
    int32_t     i = 0;
    char        equipment_nm[MAX_FS_NAME_LEN];
    char        *equipment = &equipment_nm[0];
    char        equipment_name[MAX_FS_NAME_LEN];
    char        *partition[RAT_MAX_PARTITION] = {"e:", "f:", "g:", "h:", "i:", "j:", "k:", "l:", "m:", "n:", "o:"};

    if (encryption == 1)
    {
        eLIBs_strcpy(equipment, "SDMMC-DISK:50");
    }
    else
    {
        eLIBs_strcpy(equipment, "SDMMC-DISK:00");
    }

    for (i = 0; i < RAT_MAX_PARTITION; i++)
    {
        eLIBs_memset(equipment_name, 0, MAX_FS_NAME_LEN);
        eLIBs_GetPTName(partition[i], equipment_name);

        if (eLIBs_strncmp(equipment_name, equipment, eLIBs_strlen(equipment)) == 0)
        {
            if (eq_name)
            {
                eLIBs_strcpy((void *) eq_name, partition[i]);
                eLIBs_strcat(eq_name, "\\");
            }

            return 1;
        }
    }

    return 0;
}
