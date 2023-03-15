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
#ifndef __SUNXI_PARTITION_H__
#define __SUNXI_PARTITION_H__
#include <typedef.h>
#include <mod_defs.h>
#include <libc.h>
#include <log.h>
#include <fcntl.h>
#include "sunxi_flash.h"
#include "sunxi_update_misc.h"

#define SUNXI_MBR_MAGIC			    "softw411"
#define SUNXI_MBR_MAX_PART_COUNT	(120)
#define SUNXI_MBR_SIZE			    (16 * 1024)
#define SUNXI_DL_SIZE				(16 * 1024)
#define SUNXI_MBR_RESERVED          (SUNXI_MBR_SIZE - 32 - 4 - (SUNXI_MBR_MAX_PART_COUNT * sizeof(sunxi_partition)))   //mbr保留的空间
#define SUNXI_DL_RESERVED           (SUNXI_DL_SIZE - 32 - (SUNXI_MBR_MAX_PART_COUNT * sizeof(dl_one_part_info)))

typedef struct sunxi_partition_t
{
	unsigned  int       addrhi;				//起始地址, 以扇区为单位
	unsigned  int       addrlo;				//
	unsigned  int       lenhi;				//长度
	unsigned  int       lenlo;				//
	unsigned  char      classname[16];		//次设备名
	unsigned  char      name[16];			//主设备名
	unsigned  int       user_type;          //用户类型
	unsigned  int       keydata;            //关键数据，要求量产不丢失
	unsigned  int       ro;                 //读写属性
	unsigned  int       sig_verify;			//签名验证属性
	unsigned  int       sig_erase;          //签名擦除属性
	unsigned  int       sig_value[4];
	unsigned  int       sig_pubkey;
	unsigned  int       sig_pbumode;
	unsigned  char      reserved2[36];		//保留数据，匹配分区信息128字节
}__attribute__ ((packed))sunxi_partition;

typedef struct sunxi_mbr
{
	unsigned  int       crc32;				        // crc 1k - 4
	unsigned  int       version;			        // 版本信息， 0x00000100
	unsigned  char 	    magic[8];			        //"softw311"
	unsigned  int 	    copy;				        //分数
	unsigned  int 	    index;				        //第几个MBR备份
	unsigned  int       PartCount;			        //分区个数
	unsigned  int       stamp[1];					//对齐
	sunxi_partition     array[SUNXI_MBR_MAX_PART_COUNT];	//
	unsigned  int       lockflag;
	unsigned  char      res[SUNXI_MBR_RESERVED];
}__attribute__ ((packed)) sunxi_mbr_t;

typedef struct tag_one_part_info
{
	unsigned  char      name[16];           //所烧写分区的主设备名
	unsigned  int       addrhi;             //所烧写分区的高地址，扇区单位
	unsigned  int       addrlo;             //所烧写分区的低地址，扇区单位
	unsigned  int       lenhi;				//所烧写分区的长度，高32位，扇区单位
	unsigned  int       lenlo;				//所烧写分区的长度，低32位，扇区单位
	unsigned  char      dl_filename[16];    //所烧写分区的文件名称，长度固定16字节
	unsigned  char      vf_filename[16];    //所烧写分区的校验文件名称，长度固定16字节
	unsigned  int       encrypt;            //所烧写分区的数据是否进行加密 0:加密   1：不加密
	unsigned  int       verify;             //所烧写分区的数据是否进行校验 0:不校验 1：校验
}__attribute__ ((packed)) dl_one_part_info;

typedef struct tag_download_info
{
	unsigned  int       crc32;				        		        //crc
	unsigned  int       version;                                    //版本号  0x00000101
	unsigned  char 	    magic[8];			        		        //"softw311"
	unsigned  int       download_count;             		        //需要烧写的分区个数
	unsigned  int       stamp[3];									//对齐
	dl_one_part_info	one_part_info[SUNXI_MBR_MAX_PART_COUNT];	//烧写分区的信息
	unsigned  char      res[SUNXI_DL_RESERVED];
}
__attribute__ ((packed)) sunxi_download_info;

int Sunxi_Verify_Mbr(void *buffer);
int Sunxi_Verify_NewGpt(char *Gpt_Buf);
int Sunxi_Get_LocalGpt(char *LocalGpt_Buf);
int Sunxi_Get_GptPart_info(char *Gpt_Buf, gpt_entry *pgpt_entry);
int  Sunxi_Get_download_map(sunxi_download_info  *dl_map);
int Sunxi_Get_DownLoadFile_Is_Null_PartInfo(char *Gpt_Buf, sunxi_download_info *dl_map, gpt_entry *pgpt_entry);
#endif
