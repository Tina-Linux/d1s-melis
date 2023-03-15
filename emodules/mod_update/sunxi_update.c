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
#include <mod_update.h>
#include <libc.h>
#include <kapi.h>
#include <log.h>
#include "sunxi_flash.h"
#include "sunxi_update.h"
#include "sunxi_partition.h"
#include "private_boot0.h"

#define CRCPOLY_LE 					0xedb88320

//don't change this size to high,make sure < 2M;some usb disk can't not success response
#define SIZE_1M							(1024*1024*1) 
#define SUNXI_FIXED_SIZE				(64*1024) //boot0+gpt
#define MAX_NULL_PART					(8)
#define UPDATE_CHECK_SUM

typedef struct __mod_update
{
    __u8 mid;
    __u8 used;
    __u32 mtsk;
    __u8 reserved;
} __mod_update_t;

static __Update_Info_t Update_Info = {0};
static __mod_update_t  *mod_update;
static unsigned char *Gpt_Buf = NULL;
static unsigned char *LocalGpt_Buf = NULL;
static sunxi_download_info  dl_info;

extern __s32 FW_Img_Open(const char *fw_name);
extern __u32 FW_GetItemSize(__Update_Info_t *info);
extern __s32 FW_ImgItem_Open(__Update_Info_t *info);
extern __s32 FW_GetItemData(__Update_Info_t *info, __u64 len);
extern __s32 FW_ImgItem_Close(void);
extern __s32 FW_Img_Close(void);

static u64 Sunxi_GetFirmWare_TotalSize(void) {
	unsigned char Index = 0;
	dl_one_part_info *part_info = NULL;

	for (part_info = dl_info.one_part_info, Index = 0; Index < dl_info.download_count; Index++, part_info++) {
		unsigned int Size = 0;

		eLIBs_memset(Update_Info.MainType, 0x00, MAINTYPE_LEN);
		eLIBs_memset(Update_Info.subType, 0x00, SUBTYPE_LEN);
		eLIBs_strncpy(Update_Info.MainType, "RFSFAT16", MAINTYPE_LEN);
		eLIBs_strncpy(Update_Info.subType, part_info->dl_filename, SUBTYPE_LEN);
		Size = FW_GetItemSize(&Update_Info);
#ifdef UPDATE_DEBUG
		eLIBs_printf("current MainType:%s\n", Update_Info.MainType);
		eLIBs_printf("current subType:%s\n", Update_Info.subType);
		eLIBs_printf("size:%d\n", Size);
#endif		
		if (Size == 0) {
			eLIBs_printf("current MainType:%s\n", Update_Info.MainType);
			eLIBs_printf("current subType:%s\n", Update_Info.subType);			
			eLIBs_printf("size:%d\n", Size);
		}
		Update_Info.TotalSize += Size;
	}	
	Update_Info.TotalSize += SUNXI_FIXED_SIZE;
	return Update_Info.TotalSize;
}
#if 1
static __s32 Sunxi_Update_Boot0(void) {
	__u32 check_sum_file = 0, check_sum_flash = 0;

	blk_dev_erase_t blk_dev_erase;
	blk_dev_get_data_t get_data_request;
	eLIBs_memset(Update_Info.MainType, 0x00, MAINTYPE_LEN);
	eLIBs_memset(Update_Info.subType, 0x00, SUBTYPE_LEN);
	eLIBs_strncpy(Update_Info.MainType, "12345678", MAINTYPE_LEN);
	eLIBs_strncpy(Update_Info.subType, "1234567890BNOR_0", SUBTYPE_LEN);

	//get data
	Update_Info.Current_UpdateItemSize = FW_GetItemSize(&Update_Info);
	Update_Info.Data = (__u8 *)esMEMS_Malloc(0, Update_Info.Current_UpdateItemSize);
	if (Update_Info.Data == NULL) {
		eLIBs_printf("Hal_malloc Fail Size:%d\n", Update_Info.Current_UpdateItemSize);
		return EPDK_FAIL;
	}			
	eLIBs_memset(Update_Info.Data, 0xFF, Update_Info.Current_UpdateItemSize);
	FW_ImgItem_Open(&Update_Info);
	FW_GetItemData(&Update_Info, Update_Info.Current_UpdateItemSize);
	//需要close掉后重新open，才能重置pos
	FW_ImgItem_Close(); 
#ifdef	UPDATE_CHECK_SUM
	check_sum_file = add_sum((void *)Update_Info.Data, Update_Info.Current_UpdateItemSize);
#ifdef UPDATE_DEBUG
	eLIBs_printf("Update_Info.Current_UpdateItemSize:%d!\n", Update_Info.Current_UpdateItemSize);
	eLIBs_printf("New Boot0 check_sum:0x%x!\n", check_sum_file);
#endif
	eLIBs_memset(Update_Info.Data, 0x00, Update_Info.Current_UpdateItemSize);
	get_data_request.addr = 0;
	get_data_request.buff = Update_Info.Data;
	get_data_request.len = Update_Info.Current_UpdateItemSize;
	sunxi_flash_read(&get_data_request);
	check_sum_flash = add_sum((void *)Update_Info.Data, Update_Info.Current_UpdateItemSize);
#ifdef UPDATE_DEBUG
	eLIBs_printf("Flash Boot0 check_sum:0x%x!\n", check_sum_flash);
#endif	
	if (check_sum_file == check_sum_flash) { // nothing change, just jump
		eLIBs_printf("Boot0 check_sum_file == check_sum_flash!\n");
		eLIBs_printf("Boot0 nothing change, give up update boot0!\n");				
		esMEMS_Mfree(0, Update_Info.Data);
		Update_Info.Data = NULL;
		FW_ImgItem_Close();				
		Update_Info.Already_UpdateSize += Update_Info.Current_UpdateItemSize;
		Update_Info.Burn_prog = Update_Info.Already_UpdateSize*100/Update_Info.TotalSize;	
		eLIBs_printf("%s %s %d Burn_prog:%d\n", __FILE__, __func__, __LINE__, Update_Info.Burn_prog);
		return EPDK_OK;
	}
#endif	
	//do update			
	eLIBs_memset(Update_Info.Data, 0xFF, Update_Info.Current_UpdateItemSize);
	blk_dev_erase.addr = 0;
	blk_dev_erase.len = Update_Info.Current_UpdateItemSize;
	sunxi_flash_erase(&blk_dev_erase);

	FW_ImgItem_Open(&Update_Info);
	FW_GetItemData(&Update_Info, Update_Info.Current_UpdateItemSize);	
	FW_ImgItem_Close();
	eLIBs_memset(&get_data_request, 0x00, sizeof(get_data_request));
	get_data_request.addr = 0;
	get_data_request.buff = Update_Info.Data;
	get_data_request.len = Update_Info.Current_UpdateItemSize;
	sunxi_flash_write(&get_data_request);
#ifdef	UPDATE_CHECK_SUM
	eLIBs_memset(Update_Info.Data, 0x00, Update_Info.Current_UpdateItemSize);
	get_data_request.addr = 0;
	get_data_request.buff = Update_Info.Data;
	get_data_request.len = Update_Info.Current_UpdateItemSize;
	sunxi_flash_read(&get_data_request);
	check_sum_flash = add_sum((void *)Update_Info.Data, Update_Info.Current_UpdateItemSize);
#ifdef UPDATE_DEBUG
	eLIBs_printf("Flash Boot0 check_sum:0x%x!\n", check_sum_flash);
#endif	
	if (check_sum_file != check_sum_flash) {
		eLIBs_printf("%s %d check_sum_file != check_sum_flash\n", __func__, __LINE__);
		eLIBs_printf("%s %d read image file boot0 part add_sum:0x%x\n", __func__, __LINE__, check_sum_file);
		eLIBs_printf("%s %d read flash boot0 part add_sum:0x%x\n", __func__, __LINE__, check_sum_flash);
	}
#endif
	Update_Info.Already_UpdateSize += Update_Info.Current_UpdateItemSize;
	Update_Info.Burn_prog = Update_Info.Already_UpdateSize*100/Update_Info.TotalSize;
	eLIBs_printf("%s %s %d Burn_prog:%d\n", __FILE__, __func__, __LINE__, Update_Info.Burn_prog);
	esMEMS_Mfree(0, Update_Info.Data);
	Update_Info.Data = NULL;
	return EPDK_OK;
}
#else
//在烧录过程中会重新设置boot0头部信息中的dram参数，导致固件中数据和实际flash中数据不一致。
//如果要更新boot0，需将头部信息中的dram部分解析保留，更新其它部分即可。
static __s32 Sunxi_Update_Boot0(void) {
	boot0_file_head_t *boot0_head_local = NULL, *boot0_head_local_new = NULL;
	blk_dev_erase_t blk_dev_erase;
	blk_dev_get_data_t get_data_request;
	int ret = 0;
	__u32 check_sum_file = 0, check_sum_flash = 0;

	boot0_head_local = (__u8 *)esMEMS_Malloc(0, (48*1024));
	eLIBs_memset(boot0_head_local, 0xFF, sizeof(boot0_file_head_t));
	get_data_request.addr = 0;
	get_data_request.buff = boot0_head_local;
	get_data_request.len = (48*1024);
	sunxi_flash_read(&get_data_request);

	ret = sunxi_verify_checksum(boot0_head_local, boot0_head_local->boot_head.length, boot0_head_local->boot_head.check_sum);
	if (ret != 0) {
		esMEMS_Mfree(0, boot0_head_local);
		eLIBs_printf("%s %s %d boot0 checksum error!\n", __FILE__, __func__, __LINE__);
		return EPDK_FAIL;
	}
	eLIBs_memset(Update_Info.MainType, 0x00, MAINTYPE_LEN);
	eLIBs_memset(Update_Info.subType, 0x00, SUBTYPE_LEN);
	eLIBs_strncpy(Update_Info.MainType, "12345678", MAINTYPE_LEN);
	eLIBs_strncpy(Update_Info.subType, "1234567890BNOR_0", SUBTYPE_LEN);

	//get data
	Update_Info.Current_UpdateItemSize = FW_GetItemSize(&Update_Info);
	Update_Info.Data = (__u8 *)esMEMS_Malloc(0, Update_Info.Current_UpdateItemSize);
	if (Update_Info.Data == NULL) {
		esMEMS_Mfree(0, boot0_head_local);
		eLIBs_printf("Hal_malloc Fail Size:%d\n", Update_Info.Current_UpdateItemSize);
		return EPDK_FAIL;
	}			
	eLIBs_memset(Update_Info.Data, 0xFF, Update_Info.Current_UpdateItemSize);
	FW_ImgItem_Open(&Update_Info);
	FW_GetItemData(&Update_Info, Update_Info.Current_UpdateItemSize);
	//需要close掉后重新open，才能重置pos
	FW_ImgItem_Close();

	boot0_head_local_new = (boot0_file_head_t *)Update_Info.Data;
	eLIBs_memcpy((void *)boot0_head_local_new->prvt_head.dram_para, (void *)boot0_head_local->prvt_head.dram_para, 32 * 4);
	boot0_head_local_new->boot_head.check_sum = sunxi_generate_checksum(Update_Info.Data, boot0_head_local_new->boot_head.length, 1, boot0_head_local_new->boot_head.check_sum);
	if(sunxi_verify_checksum(Update_Info.Data, boot0_head_local_new->boot_head.length, boot0_head_local_new->boot_head.check_sum)) {
		esMEMS_Mfree(0, boot0_head_local);
		esMEMS_Mfree(0, Update_Info.Data);
		eLIBs_printf("sunxi sprite: boot0 checksum is error\n");
		return -1;
	}

	//do update			
	blk_dev_erase.addr = 0;
	blk_dev_erase.len = Update_Info.Current_UpdateItemSize;
	sunxi_flash_erase(&blk_dev_erase);

	get_data_request.addr = 0;
	get_data_request.buff = Update_Info.Data;
	get_data_request.len = Update_Info.Current_UpdateItemSize;
	sunxi_flash_write(&get_data_request);

	Update_Info.Already_UpdateSize += Update_Info.Current_UpdateItemSize;
	Update_Info.Burn_prog = Update_Info.Already_UpdateSize*100/Update_Info.TotalSize;
	eLIBs_printf("%s %s %d Burn_prog:%d\n", __FILE__, __func__, __LINE__, Update_Info.Burn_prog);
	esMEMS_Mfree(0, boot0_head_local);
	esMEMS_Mfree(0, Update_Info.Data);
	Update_Info.Data = NULL;
	return EPDK_OK;
}
#endif
static __s32 Sunxi_Erase_Boot0(void) {
	blk_dev_erase_t blk_dev_erase;

	//do erase boot0			
	blk_dev_erase.addr = 0;
	blk_dev_erase.len = (48*1024);
	sunxi_flash_erase(&blk_dev_erase);
	return EPDK_OK;
}

static __s32 Sunxi_Update_Gpt(void) {
	__u32 check_sum_file = 0, check_sum_flash = 0;
	blk_dev_erase_t blk_dev_erase;
	blk_dev_get_data_t get_data_request;

	eLIBs_printf("%s %s %d\n", __FILE__, __func__, __LINE__);
	Update_Info.Current_UpdateItemSize = GPT_TABLE_SIZE;
	Update_Info.Data = (__u8 *)esMEMS_Malloc(0, Update_Info.Current_UpdateItemSize);
	if (Update_Info.Data == NULL) {
		eLIBs_printf("Hal_malloc Fail Size:%d\n", Update_Info.Current_UpdateItemSize);
		return EPDK_FAIL;
	}			
	eLIBs_memset(Update_Info.Data, 0xFF, Update_Info.Current_UpdateItemSize);
#ifdef	UPDATE_CHECK_SUM
	check_sum_file = add_sum((void *)Gpt_Buf, GPT_TABLE_SIZE);
	check_sum_flash = add_sum((void *)LocalGpt_Buf, GPT_TABLE_SIZE);
	if (check_sum_file == check_sum_flash) {// nothing change, just jump
		eLIBs_printf("Gpt check_sum_file == check_sum_flash!\n");
		eLIBs_printf("Gpt nothing change, give up update Gpt!\n");					
		Update_Info.Already_UpdateSize += Update_Info.Current_UpdateItemSize;
		Update_Info.Burn_prog = Update_Info.Already_UpdateSize*100/Update_Info.TotalSize;	
		eLIBs_printf("%s %s %d Burn_prog:%d\n", __FILE__, __func__, __LINE__, Update_Info.Burn_prog);
		esMEMS_Mfree(0, Update_Info.Data);
		Update_Info.Data = NULL;
		return EPDK_OK;
	}
#endif		

	//do update
	blk_dev_erase.addr = GPT_ADDRESS;
	blk_dev_erase.len = GPT_TABLE_SIZE;
	sunxi_flash_erase(&blk_dev_erase);

	get_data_request.addr = GPT_ADDRESS;
	get_data_request.buff = Gpt_Buf;
	get_data_request.len = GPT_TABLE_SIZE;
	sunxi_flash_write(&get_data_request);

#ifdef	UPDATE_CHECK_SUM
	eLIBs_memset(Update_Info.Data, 0xFF, GPT_TABLE_SIZE);
	get_data_request.addr = GPT_ADDRESS;
	get_data_request.buff = Update_Info.Data;
	get_data_request.len = GPT_TABLE_SIZE;
	sunxi_flash_read(&get_data_request);
	check_sum_file = add_sum((void *)Gpt_Buf, GPT_TABLE_SIZE);
	check_sum_flash = add_sum((void *)Update_Info.Data, GPT_TABLE_SIZE);
	if (check_sum_file != check_sum_flash) {
		eLIBs_printf("%s %d check_sum_file != check_sum_flash\n", __func__, __LINE__);
		eLIBs_printf("%s %d read image file gpt part add_sum:0x%x\n", __func__, __LINE__, check_sum_file);
		eLIBs_printf("%s %d read flash gpt part add_sum:0x%x\n", __func__, __LINE__, check_sum_flash);
	}
#endif
	Update_Info.Already_UpdateSize += Update_Info.Current_UpdateItemSize;
	Update_Info.Burn_prog = Update_Info.Already_UpdateSize*100/Update_Info.TotalSize;	
	eLIBs_printf("%s %s %d Burn_prog:%d\n", __FILE__, __func__, __LINE__, Update_Info.Burn_prog);
	esMEMS_Mfree(0, Update_Info.Data);
	Update_Info.Data = NULL;
	return EPDK_OK;
}
/*
	处理所有downloadfile不为空的所有分区
*/

static __s32 Sunxi_Update_NormalPart(void) {
	unsigned char Index = 0;
	dl_one_part_info *part_info = NULL;

	for (part_info = dl_info.one_part_info, Index = 0; Index < dl_info.download_count; Index++, part_info++) {
		__u64 ItemSize = 0;		
		__u64 Already_UpdateSize = 0;
		gpt_entry GptPart_entry;
		blk_dev_erase_t blk_dev_erase;
		blk_dev_get_data_t get_data_request;
#ifdef	UPDATE_CHECK_SUM
		__u32 verify_value = 0, check_sum_file = 0, check_sum_flash = 0;

		eLIBs_strncpy(Update_Info.MainType, "RFSFAT16" , MAINTYPE_LEN);
		eLIBs_strncpy(Update_Info.subType, part_info->vf_filename, SUBTYPE_LEN);
		
		Update_Info.Data = (__u8 *)&verify_value;			
		FW_ImgItem_Open(&Update_Info);
		FW_GetItemData(&Update_Info, sizeof(verify_value));
#ifdef UPDATE_DEBUG
		eLIBs_printf("%s %s %d %s verify_value:0x%x\n", __FILE__, __func__, __LINE__, part_info->vf_filename, verify_value);
#endif		
		FW_ImgItem_Close();
		Update_Info.Data = NULL;
#endif

		eLIBs_strncpy(Update_Info.subType, part_info->dl_filename, SUBTYPE_LEN);
		//申请1M内存
		Update_Info.Current_UpdateItemSize = FW_GetItemSize(&Update_Info);
		Update_Info.Data = (__u8 *)esMEMS_Malloc(0, SIZE_1M);
		if (Update_Info.Data == NULL) {
			eLIBs_printf("Hal_malloc Fail Size:%d\n", SIZE_1M);
			return EPDK_FAIL;
		}
		FW_GetItemSize(&Update_Info);
		FW_ImgItem_Open(&Update_Info);
		
		eLIBs_memset(&GptPart_entry, 0x00, sizeof(GptPart_entry));
		eLIBs_strcpy((char *)GptPart_entry.partition_name, part_info->name);
		if (Sunxi_Get_GptPart_info(Gpt_Buf, &GptPart_entry) == EPDK_FAIL) {
			eLIBs_printf("%s %s %d Sunxi_Get_GptPart_info Fail!\n", __FILE__, __func__, __LINE__);
			return EPDK_FAIL;
		}
#ifdef UPDATE_DEBUG
		eLIBs_printf("%s %d Part:%s\n", __func__, __LINE__, GptPart_entry.partition_name);
#endif		
/*
	每M擦除读写，可增加用户体验，进度更新及时
	若整体慢，可增加此处大小
*/
		while ((Update_Info.Current_UpdateItemSize - Already_UpdateSize) >= SIZE_1M) {
			eLIBs_memset(Update_Info.Data, 0xFF, SIZE_1M);
			FW_GetItemData(&Update_Info, SIZE_1M);
			
			blk_dev_erase.addr = GptPart_entry.starting_lba*512 + Already_UpdateSize;
			blk_dev_erase.len = SIZE_1M;
			sunxi_flash_logic_erase(&blk_dev_erase);

			get_data_request.addr = GptPart_entry.starting_lba*512 + Already_UpdateSize;
			get_data_request.buff = Update_Info.Data;
			get_data_request.len = SIZE_1M;
			sunxi_flash_logic_write(&get_data_request);

#ifdef	UPDATE_CHECK_SUM
			eLIBs_memset(Update_Info.Data, 0x00, SIZE_1M);
			/*addr must reset*/
			get_data_request.addr = GptPart_entry.starting_lba*512 + Already_UpdateSize;
			sunxi_flash_logic_read(&get_data_request);			
			check_sum_flash += add_sum((void *)Update_Info.Data, SIZE_1M);
#endif
			Already_UpdateSize += SIZE_1M;
			Update_Info.Already_UpdateSize += SIZE_1M;
			Update_Info.Burn_prog = Update_Info.Already_UpdateSize*100/Update_Info.TotalSize;
			eLIBs_printf("%s %s %d Burn_prog:%d\n", __FILE__, __func__, __LINE__, Update_Info.Burn_prog);
		}

		//小于2M的最后升级
		if (Update_Info.Current_UpdateItemSize - Already_UpdateSize > 0) {
			eLIBs_memset(Update_Info.Data, 0xFF, SIZE_1M);
			FW_GetItemData(&Update_Info, Update_Info.Current_UpdateItemSize - Already_UpdateSize);

			blk_dev_erase.addr = GptPart_entry.starting_lba*512 + Already_UpdateSize;
			blk_dev_erase.len = Update_Info.Current_UpdateItemSize - Already_UpdateSize;
			sunxi_flash_logic_erase(&blk_dev_erase);
			
			get_data_request.addr = GptPart_entry.starting_lba*512 + Already_UpdateSize;
			get_data_request.buff = Update_Info.Data;
			get_data_request.len = Update_Info.Current_UpdateItemSize - Already_UpdateSize;
			sunxi_flash_logic_write(&get_data_request);
			
#ifdef	UPDATE_CHECK_SUM
			eLIBs_memset(Update_Info.Data, 0x00, SIZE_1M);
			/*addr must reset*/
			get_data_request.addr = GptPart_entry.starting_lba*512 + Already_UpdateSize;
			sunxi_flash_logic_read(&get_data_request);			
			check_sum_flash += add_sum((void *)Update_Info.Data, SIZE_1M);
#endif	
			Update_Info.Already_UpdateSize += Update_Info.Current_UpdateItemSize - Already_UpdateSize;
			Already_UpdateSize = Update_Info.Current_UpdateItemSize;
		}
		//若该分区还有剩余空间，擦除
		if (GptPart_entry.starting_lba*512 + Already_UpdateSize < 
			(GptPart_entry.ending_lba - GptPart_entry.starting_lba + 1)*512)
		{
			blk_dev_erase.addr = GptPart_entry.starting_lba*512 + Already_UpdateSize;
			blk_dev_erase.len = (GptPart_entry.ending_lba - GptPart_entry.starting_lba + 1)*512 - Already_UpdateSize;
			sunxi_flash_logic_erase(&blk_dev_erase);
		}
		Update_Info.Burn_prog = Update_Info.Already_UpdateSize*100/Update_Info.TotalSize;
		eLIBs_printf("%s %s %d Burn_prog:%d\n", __FILE__, __func__, __LINE__, Update_Info.Burn_prog);
		FW_ImgItem_Close(); 	
#ifdef	UPDATE_CHECK_SUM
		if (verify_value != check_sum_flash) {
			eLIBs_printf("%s %d Part:%s\n", __func__, __LINE__, GptPart_entry.partition_name);
			eLIBs_printf("%s %d verify_value != check_sum_flash\n", __func__, __LINE__);
			eLIBs_printf("%s %d read image verify file add_sum:0x%x\n", __func__, __LINE__, verify_value);
			eLIBs_printf("%s %d read flash ROOTFS part add_sum:0x%x\n", __func__, __LINE__, check_sum_flash);
		}
#endif
		esMEMS_Mfree(0, Update_Info.Data);
		Update_Info.Data = NULL;

	}	
	return EPDK_OK;
}
/*
	处理除private分区外所有downloadfile为空的所有分区
*/
static __s32 Sunxi_Update_NullPart(void) {
	int totoal = 0, Index = 0;
	dl_one_part_info *part_info = NULL;
	gpt_entry Part_Entry[MAX_NULL_PART] = {0}, Part_Entry_tmp;

	totoal = Sunxi_Get_DownLoadFile_Is_Null_PartInfo(Gpt_Buf, &dl_info, Part_Entry);
	if (totoal == 0) {	
		eLIBs_printf("%s %s %d\n", __FILE__, __func__, __LINE__);
		return EPDK_OK;
	} else if (totoal > MAX_NULL_PART) {
		eLIBs_printf("%s %s %d Part Num Error\n", __FILE__, __func__, __LINE__);
		return EPDK_OK;
	}
	for (Index = 0; Index < totoal; Index++) {
		eLIBs_memset(&Part_Entry_tmp, 0x00, sizeof(gpt_entry));
		eLIBs_memcpy((char *)Part_Entry_tmp.partition_name, Part_Entry[Index].partition_name, PARTNAME_SZ*sizeof(efi_char16_t));
		if (Sunxi_Get_GptPart_info(LocalGpt_Buf, &Part_Entry_tmp) == EPDK_OK) {
			if (Part_Entry_tmp.starting_lba == Part_Entry[Index].starting_lba && 
				Part_Entry_tmp.ending_lba == Part_Entry[Index].ending_lba) {
				eLIBs_printf("%s %s %d Nothing Change,Not Need to Erase!\n", __FILE__, __func__, __LINE__, Part_Entry[Index].partition_name);
				continue;
			}
		}
		blk_dev_erase_t blk_dev_erase;

		eLIBs_printf("GPT:%-12s %-12llx  %-12llx Need to Erase\n", Part_Entry[Index].partition_name,
			   Part_Entry[Index].starting_lba, Part_Entry[Index].ending_lba);			
		blk_dev_erase.addr = Part_Entry[Index].starting_lba*512;
		blk_dev_erase.len = (Part_Entry[Index].ending_lba - Part_Entry[Index].starting_lba + 1)*512;
		sunxi_flash_logic_erase(&blk_dev_erase);
	}
	return EPDK_OK;	
}

static __s32 Sunxi_Update_Private(void) {
	blk_dev_erase_t blk_dev_erase;
	blk_dev_get_data_t get_data_request;
	gpt_entry Local_PrivatePart, New_PrivatePart;

	//检查本地是否有private分区，并获取分区数据
	eLIBs_memset(&Local_PrivatePart, 0x00, sizeof(Local_PrivatePart));
	eLIBs_strcpy((char *)Local_PrivatePart.partition_name, "private");
	Sunxi_Get_GptPart_info(LocalGpt_Buf, &Local_PrivatePart);
	
	//检查新固件是否有private分区，并获取分区数据
	eLIBs_memset(&New_PrivatePart, 0x00, sizeof(New_PrivatePart));
	eLIBs_strcpy((char *)New_PrivatePart.partition_name, "private");
	Sunxi_Get_GptPart_info(Gpt_Buf, &New_PrivatePart);
	//1:若本地没有private分区，新固件也没有相应分区，则直接退出
	//2:若本地有private分区，但是新固件没有相应分区，则直接退出
	//将1 2判断条件直接合并为新固件没有相应分区，则直接退出，如果原来有private分区
	// 则数据丢失
	//3:若本地没有private分区，新固件有相应分区则擦除对应分区
	//4:若两个都有private分区
		//1:如果新固件的private分区大小较原有的大，写入数据长度按原有计算
		//反之，按新固件的private分区大小写入，数据可能会丢失
	if ((New_PrivatePart.ending_lba - New_PrivatePart.starting_lba) == 0) {
		eLIBs_printf("%s %s %d Not have private part!\n", __FILE__, __func__, __LINE__);		
		return EPDK_OK;
	} else if (((Local_PrivatePart.ending_lba - Local_PrivatePart.starting_lba) == 0) && 
		((New_PrivatePart.ending_lba - New_PrivatePart.starting_lba) > 0)) {
		eLIBs_memset(&blk_dev_erase, 0x00, sizeof(blk_dev_erase));
		blk_dev_erase.addr = New_PrivatePart.starting_lba*512;
		blk_dev_erase.len = (New_PrivatePart.ending_lba - New_PrivatePart.starting_lba + 1)*512;
		sunxi_flash_logic_erase(&blk_dev_erase);		
		return EPDK_OK;
	} else if (((Local_PrivatePart.ending_lba - Local_PrivatePart.starting_lba) > 0) && 
		((New_PrivatePart.ending_lba - New_PrivatePart.starting_lba) > 0)) {
		if ((New_PrivatePart.starting_lba == Local_PrivatePart.starting_lba) && 
			(New_PrivatePart.ending_lba == Local_PrivatePart.ending_lba)) {
			return EPDK_OK;
		}
		Update_Info.Data = (__u8 *)esMEMS_Malloc(0, (Local_PrivatePart.ending_lba - Local_PrivatePart.starting_lba + 1)*512);
		if (Update_Info.Data == NULL) {
			eLIBs_printf("Hal_malloc Fail Size:%d\n", (Local_PrivatePart.ending_lba - Local_PrivatePart.starting_lba + 1)*512);
			return EPDK_FAIL;
		}
		eLIBs_memset(Update_Info.Data, 0x00, (Local_PrivatePart.ending_lba - Local_PrivatePart.starting_lba + 1)*512);

		get_data_request.addr = Local_PrivatePart.starting_lba*512;
		get_data_request.buff = Update_Info.Data;
		get_data_request.len = (Local_PrivatePart.ending_lba - Local_PrivatePart.starting_lba + 1)*512;
		sunxi_flash_logic_read(&get_data_request);

		eLIBs_memset(&blk_dev_erase, 0x00, sizeof(blk_dev_erase));
		blk_dev_erase.addr = New_PrivatePart.starting_lba*512;
		blk_dev_erase.len = (New_PrivatePart.ending_lba - New_PrivatePart.starting_lba + 1)*512;
		sunxi_flash_logic_erase(&blk_dev_erase);

		get_data_request.addr = New_PrivatePart.starting_lba*512;
		get_data_request.buff = Update_Info.Data;
		if ((Local_PrivatePart.ending_lba - Local_PrivatePart.starting_lba) > 
			(New_PrivatePart.ending_lba - New_PrivatePart.starting_lba)) {			
			eLIBs_printf("%s %s %d private part data lost\n", __FILE__, __func__, __LINE__);
			get_data_request.len = (New_PrivatePart.ending_lba - New_PrivatePart.starting_lba + 1)*512;
		} else {
			get_data_request.len = (Local_PrivatePart.ending_lba - Local_PrivatePart.starting_lba + 1)*512;
		}
		sunxi_flash_logic_write(&get_data_request);
		esMEMS_Mfree(0, Update_Info.Data);
		Update_Info.Data = NULL;	
		return EPDK_OK;
	}
	return EPDK_OK;
}
static __s32 Sunxi_Update_Finish(void) {
	FW_Img_Close();	
	Update_Info.Burn_prog = 100;
	Update_Info.Burn_status = UPDATE_STATUS_UPDATE_FINSH;
	return EPDK_OK;
}
static void burn_tsk(void *arg)
{
	eLIBs_printf("%s %d\n", __FILE__, __LINE__);
	Update_Info.Burn_status = UPDATE_STATUS_UPDATE_ING;
	Gpt_Buf = (__u8 *)esMEMS_Malloc(0, GPT_TABLE_SIZE);
	if (Gpt_Buf == NULL) {
		eLIBs_printf("%s %d esMEMS_Malloc Fail\n", __FILE__, __LINE__);
		Update_Info.Burn_status = UPDATE_STATUS_GET_GPT_FAIL;
        goto exit;
	}
	LocalGpt_Buf = (__u8 *)esMEMS_Malloc(0, GPT_TABLE_SIZE);
	if (LocalGpt_Buf == NULL) {
		eLIBs_printf("%s %d esMEMS_Malloc Fail\n", __FILE__, __LINE__);
		Update_Info.Burn_status = UPDATE_STATUS_GET_GPT_FAIL;
        goto exit;
	}
	if (Sunxi_Verify_NewGpt(Gpt_Buf) == EPDK_FAIL) {
		eLIBs_printf("%s %d Sunxi_Verify_Mbr Fail\n", __FILE__, __LINE__);
		Update_Info.Burn_status = UPDATE_STATUS_GET_GPT_FAIL;
        goto exit;
	}
	if (Sunxi_Get_LocalGpt(LocalGpt_Buf) == EPDK_FAIL) {
		Update_Info.Burn_status = UPDATE_STATUS_GET_GPT_FAIL;
        goto exit;
	}
	Sunxi_GetFirmWare_TotalSize();

	if (Sunxi_Update_Boot0() != EPDK_OK) {
		Update_Info.Burn_status = UPDATE_STATUS_BOOT0_FAIL;
        goto exit;
	}
	if (Sunxi_Update_Private() != EPDK_OK) {
		Update_Info.Burn_status = UPDATE_STATUS_PRIVATE_FAIL;
        goto exit;
	}
	if (Sunxi_Update_Gpt() != EPDK_OK) {
		Update_Info.Burn_status = UPDATE_STATUS_GPT_FAIL;
        goto exit;
	}
	if (Sunxi_Update_NullPart() != EPDK_OK) {
		Update_Info.Burn_status = UPDATE_STATUS_NORMAL_PART_FAIL;
        goto exit;
	}
	if (Sunxi_Update_NormalPart() != EPDK_OK) {
		Update_Info.Burn_status = UPDATE_STATUS_NORMAL_PART_FAIL;
        goto exit;
	}
	Sunxi_Update_Finish();
exit:
	if (Gpt_Buf) {
		esMEMS_Mfree(0, Gpt_Buf);
		Gpt_Buf = NULL;
	}

	if (LocalGpt_Buf) {
		esMEMS_Mfree(0, LocalGpt_Buf);
		LocalGpt_Buf = NULL;
	}
    while (1)
    {
        //* 0. check quit request
        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            goto _process_exit;
        }
	}
_process_exit:
	esKRNL_TDel(EXEC_prioself);
}

/*
*********************************************************************************************************
* Function   : update_fw_start
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
static __s32 update_fw_start(const char *fw_name) {
    eLIBs_printf("get clear firmware");
	eLIBs_memset(&Update_Info, 0x00, sizeof(Update_Info));
	if (FW_Img_Open(fw_name) != EPDK_OK) {
		eLIBs_printf("%s %d FW_Img_Open %s Fail\n", __FILE__, __LINE__, fw_name);
		Update_Info.Burn_status = UPDATE_STATUS_FW_OPEN_FAIL;
		return EPDK_FAIL;
	}
	if (Sunxi_Get_download_map(&dl_info) == EPDK_FAIL) {
		eLIBs_printf("%s %d FW_Img_Open %s Fail\n", __FILE__, __LINE__, fw_name);
		Update_Info.Burn_status = UPDATE_STATUS_FW_OPEN_FAIL;
		return EPDK_FAIL;
	}
    /* burn fw into spinor */
    mod_update->mtsk = esKRNL_TCreate(burn_tsk, NULL, 0x40000, (mod_update->mid << 8) | KRNL_priolevel4);
    if (!mod_update->mtsk)
    {
        __wrn("create burn thread fail");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}
static __s32 update_fw_stop(void) {
	
	while (esKRNL_TDelReq(mod_update->mtsk) != OS_TASK_NOT_EXIST) {
		esKRNL_TimeDly(5);
	}
	return EPDK_OK;
}
static __s32 get_update_prog(void)
{
    return Update_Info.Burn_prog;
}

/*
*********************************************************************************************************
* Function   : UPDATE_MInit
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
__s32 UPDATE_MInit(void)
{
    eLIBs_printf("update module init");
    return EPDK_OK;
}

/*
*********************************************************************************************************
* Function   : UPDATE_MExit
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
__s32 UPDATE_MExit(void)
{
    eLIBs_printf("update module exit");
    return EPDK_OK;
}

/*
*********************************************************************************************************
* Function   : UPDATE_MOpen
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
__mp *UPDATE_MOpen(__u32 mid, __u32 mode)
{
    eLIBs_printf("update module mid is %x", mid);
    mod_update = esMEMS_Malloc(0, sizeof(__mod_update_t));
    mod_update->mid  = mid;
    return (__mp *)mod_update;
}

/*
*********************************************************************************************************
* Function   : UPDATE_MClose
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
__s32 UPDATE_MClose(__mp *mp)
{
    esMEMS_Mfree(0, mod_update);
    return EPDK_OK;
}

/*
*********************************************************************************************************
* Function   : UPDATE_MRead
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
__u32 UPDATE_MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return n;
}

/*
*********************************************************************************************************
* Function   : UPDATE_MWrite
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
__u32 UPDATE_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return n;
}

/*
*********************************************************************************************************
* Function   : UPDATE_MIoctrl
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
long UPDATE_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    long ret = EPDK_FAIL;

    switch (cmd)
    {
        case UPDATE_CMD_START:
            eLIBs_printf("begin to update, image file name is %s\n", (char *)pbuffer);
            ret = update_fw_start((const char *)pbuffer);
            break;
		case UPDATE_CMD_FORCE_USB_UPDATE:
			Sunxi_Erase_Boot0();
		break;
        case UPDATE_CMD_STOP:
            ret = update_fw_stop();
            break;

        case UPDATE_CMD_STATUS:
        {
            return Update_Info.Burn_status;
        }

        case UPDATE_CMD_CHECK_PROG:
            ret = get_update_prog();
            break;

        default:
            __wrn("this command is not support");
            break;
    }

    return ret;
}
