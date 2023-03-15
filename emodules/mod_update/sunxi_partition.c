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
#include "sunxi_partition.h"
#include "sunxi_update.h"

#define CONFIG_SUNXI_MELIS
#define GPT_HEADER_SIGNATURE 0x5452415020494645ULL
#define CONFIG_SUNXI_MELIS_LOGICAL_OFFSET 96
#define EFI_PMBR_OSTYPE_EFI_GPT 0xEE
#define MSDOS_MBR_SIGNATURE 0xAA55
#define GPT_HEADER_REVISION_V1 0x00010000
#define GPT_ENTRY_SIZE			128
#define MMC_LOGICAL_OFFSET   (20 * 1024 * 1024/512)
#define SUNXI_MBR_COPY_NUM      	(4)
#define CONFIG_SUNXI_NOT_BACKUP_GPT

#define EFI_GUID(a, b, c, d0, d1, d2, d3, d4, d5, d6, d7) \
	((efi_guid_t) \
	{{ (a) & 0xff, ((a) >> 8) & 0xff, ((a) >> 16) & 0xff, \
		((a) >> 24) & 0xff, \
		(b) & 0xff, ((b) >> 8) & 0xff, \
		(c) & 0xff, ((c) >> 8) & 0xff, \
		(d0), (d1), (d2), (d3), (d4), (d5), (d6), (d7) } })

#define PARTITION_BASIC_DATA_GUID \
	EFI_GUID( 0xEBD0A0A2, 0xB9E5, 0x4433, \
		0x87, 0xC0, 0x68, 0xB6, 0xB7, 0x26, 0x99, 0xC7)

struct _partition_ {
	u8 boot_ind;		/* 0x80 - active */
	u8 head;		/* starting head */
	u8 sector;		/* starting sector */
	u8 cyl; 		/* starting cylinder */
	u8 sys_ind; 	/* What partition type */
	u8 end_head;		/* end head */
	u8 end_sector;		/* end sector */
	u8 end_cyl; 	/* end cylinder */
	__le32 start_sect;	/* starting sector counting from 0 */
	__le32 nr_sects;	/* nr of sectors in partition */
} __packed;

typedef struct _legacy_mbr {
	u8 boot_code[440];
	__le32 unique_mbr_signature;
	__le16 unknown;
	struct _partition_ partition_record[4];
	__le16 signature;
} __packed legacy_mbr;

extern __u64 FW_GetItemSize(__Update_Info_t *info);
extern __s32 FW_ImgItem_Open(__Update_Info_t *info);
extern __s32 FW_ImgItem_Close(void);
extern __s32 FW_GetItemData(__Update_Info_t *info, __u64 len);
#ifdef UPDATE_DEBUG
static void sunxi_mbr_dump(u8 *buf)
{
	sunxi_mbr_t     *mbr_info = (sunxi_mbr_t *)buf;
	sunxi_partition *part_info;
	u32 i;
	char buffer[32];

	eLIBs_printf("*************MBR DUMP***************\n");
	eLIBs_printf("total mbr part %d\n", mbr_info->PartCount);
	eLIBs_printf("\n");
	for(part_info = mbr_info->array, i = 0; i < mbr_info->PartCount; i++, part_info++)
	{
		eLIBs_memset(buffer, 0, 32);
		eLIBs_memcpy(buffer, part_info->name, 16);
		eLIBs_printf("part[%d] name      :%s\n", i, buffer);
		eLIBs_memset(buffer, 0, 32);
		eLIBs_memcpy(buffer, part_info->classname, 16);
		eLIBs_printf("part[%d] classname :%s\n", i, buffer);
		eLIBs_printf("part[%d] addrlo    :0x%x\n", i, part_info->addrlo);
		eLIBs_printf("part[%d] lenlo     :0x%x\n", i, part_info->lenlo);
		eLIBs_printf("part[%d] user_type :%d\n", i, part_info->user_type);
		eLIBs_printf("part[%d] keydata   :%d\n", i, part_info->keydata);
		eLIBs_printf("part[%d] ro        :%d\n", i, part_info->ro);
		eLIBs_printf("\n");
	}
}
static __s32 sunxi_show_gpt_partition_info(char *buf)
{
	int i, j;

	char char8_name[PARTNAME_SZ] = { 0 };
	gpt_header *gpt_head	 = (gpt_header *)(buf + GPT_HEAD_OFFSET);
	gpt_entry *entry	     = (gpt_entry *)(buf + GPT_ENTRY_OFFSET);

	eLIBs_printf("%s %d gpt_head->num_partition_entries:%d \n", __FILE__, __LINE__, gpt_head->num_partition_entries);
	for (i = 0; i < gpt_head->num_partition_entries; i++) {
		for (j = 0; j < PARTNAME_SZ; j++) {
			char8_name[j] = (char)(entry[i].partition_name[j]);
		}
		eLIBs_printf("GPT:%-12s: %-12llx  %-12llx\n", char8_name,
		       entry[i].starting_lba, entry[i].ending_lba);
	}

	return 0;
}
static __s32 sunxi_gpt_dump(char *gpt_buff)
{
	gpt_header *gpt_head = (gpt_header *)(gpt_buff + GPT_HEAD_OFFSET);
	u32 i;
	
	eLIBs_printf("*************GPT DUMP***************\n");
	if (gpt_head->signature == GPT_HEADER_SIGNATURE) {
		u32 calc_crc32_   = 0;
		u32 backup_crc32_ = 0;

		backup_crc32_	   = gpt_head->header_crc32;
		gpt_head->header_crc32 = 0;
		calc_crc32_ = calc_crc32((const unsigned char *)gpt_head, sizeof(gpt_header));
		gpt_head->header_crc32 = backup_crc32_;
		if (calc_crc32_ != backup_crc32_) {
			eLIBs_printf("%s %d the GPT table is bad\n", __FILE__, __LINE__);
			eLIBs_printf("the GPT table is bad\n");
			return EPDK_FAIL;
		}
		sunxi_show_gpt_partition_info(gpt_buff);
		return EPDK_OK;
	} else {
		eLIBs_printf("%s %d gpt_head->signature:%x not eq GPT_HEADER_SIGNATURE!\n", __FILE__, __LINE__, gpt_head->signature);
	}
	return EPDK_FAIL;
}
static void __dump_dlmap(sunxi_download_info *dl_info)
{
	dl_one_part_info		*part_info;
	u32 i;
	char buffer[32];

	eLIBs_printf("*************DOWNLOAD MAP DUMP************\n");
	eLIBs_printf("total download part %d\n", dl_info->download_count);
	eLIBs_printf("\n");
	for(part_info = dl_info->one_part_info, i=0;i<dl_info->download_count;i++, part_info++)
	{
		eLIBs_memset(buffer, 0, 32);
		eLIBs_memcpy(buffer, part_info->name, 16);
		eLIBs_printf("download part[%d] name          :%s\n", i, buffer);
		eLIBs_memset(buffer, 0, 32);
		eLIBs_memcpy(buffer, part_info->dl_filename, 16);
		eLIBs_printf("download part[%d] download file :%s\n", i, buffer);
		eLIBs_memset(buffer, 0, 32);
		eLIBs_memcpy(buffer, part_info->vf_filename, 16);
		eLIBs_printf("download part[%d] verify file   :%s\n", i, buffer);
		eLIBs_printf("download part[%d] lenlo         :0x%x\n", i, part_info->lenlo);
		eLIBs_printf("download part[%d] addrlo        :0x%x\n", i, part_info->addrlo);
		eLIBs_printf("download part[%d] encrypt       :0x%x\n", i, part_info->encrypt);
		eLIBs_printf("download part[%d] verify        :0x%x\n", i, part_info->verify);
		eLIBs_printf("\n");
	}
}
#endif
#define isupper(c) (('A' <= (c)) && ((c) <= 'Z'))
#define tolower(c) (isupper(c) ? (((c) - 'A') + 'a') : (c))
char *UpdateStrTolower(char *str)
{
    __u32 size = 0, i = 0;

    size = eLIBs_strlen(str);
    for (i = 0; i < size; i++) {
        str[i] = tolower(str[i]);
    }
    return str;
}
static int sunxi_mbr_convert_to_gpt(void *sunxi_mbr_buf, char *gpt_buf,int storage_type) {
	legacy_mbr   *remain_mbr;
	sunxi_mbr_t  *sunxi_mbr = (sunxi_mbr_t *)sunxi_mbr_buf;

	char         *pbuf = gpt_buf;
	gpt_header   *gpt_head;
	gpt_entry    *pgpt_entry = NULL;
	char*         gpt_entry_start=NULL;
	u32           data_len = 0;
	int           total_sectors;
	u32           logic_offset = 0;
	int           i,j = 0;

	unsigned char guid[16] = {0x88,0x38,0x6f,0xab,0x9a,0x56,0x26,0x49,0x96,0x68,0x80,0x94,0x1d,0xcb,0x40,0xbc};
	unsigned char part_guid[16] = {0x46,0x55,0x08,0xa0,0x66,0x41,0x4a,0x74,0xa3,0x53,0xfc,0xa9,0x27,0x2b,0x8e,0x45};

	if(eLIBs_strncmp((const char*)sunxi_mbr->magic, SUNXI_MBR_MAGIC, 8)) {
		eLIBs_printf("%s:not sunxi mbr, can't convert to GPT partition\n", __func__);
		return EPDK_FAIL;
	}

	if(calc_crc32((const unsigned char *)(sunxi_mbr_buf + 4), SUNXI_MBR_SIZE - 4) != sunxi_mbr->crc32)
	{
		eLIBs_printf("%s:sunxi mbr crc error, can't convert to GPT partition\n",__func__);
		return EPDK_FAIL;
	}
	if(storage_type == STORAGE_SDC0 || storage_type == STORAGE_SDC1
		|| storage_type == STORAGE_SDC2) {
		logic_offset = MMC_LOGICAL_OFFSET;
	} else if(storage_type == STORAGE_NOR) {
#ifdef CONFIG_SUNXI_FREERTOS
		logic_offset = CONFIG_SUNXI_FREERTOS_LOGICAL_OFFSET;
#elif defined(CONFIG_SUNXI_MELIS)
		logic_offset = 0;
#else
		logic_offset = 0;
#endif
	} else {
		logic_offset = 0;
	}
	logic_offset = 0;
#ifdef UPDATE_DEBUG
	eLIBs_printf("%s %s %d logic_offset:0x%x\n", __FILE__, __func__, __LINE__, logic_offset);
#endif
	total_sectors = sunxi_flash_size();

	/* 1. LBA0: write legacy mbr,part type must be 0xee */
	remain_mbr = (legacy_mbr *)pbuf;
	eLIBs_memset(remain_mbr, 0x0, 512);
	remain_mbr->partition_record[0].sector = 0x2;
	remain_mbr->partition_record[0].cyl = 0x0;
	remain_mbr->partition_record[0].sys_ind = EFI_PMBR_OSTYPE_EFI_GPT;
	remain_mbr->partition_record[0].end_head = 0xFF;
	remain_mbr->partition_record[0].end_sector = 0xFF;
	remain_mbr->partition_record[0].end_cyl = 0xFF;
	remain_mbr->partition_record[0].start_sect = 1UL;
	remain_mbr->partition_record[0].nr_sects = 0xffffffff;
	remain_mbr->signature = MSDOS_MBR_SIGNATURE;
	data_len += 512;

	/* 2. LBA1: fill primary gpt header */
	gpt_head = (gpt_header *)(pbuf + data_len);
	gpt_head->signature= GPT_HEADER_SIGNATURE;
	gpt_head->revision = GPT_HEADER_REVISION_V1;
	gpt_head->header_size = sizeof(gpt_header);
	gpt_head->header_crc32 = 0x00;
	gpt_head->reserved1 = 0x0;
	gpt_head->my_lba = 0x01;
#ifdef CONFIG_SUNXI_NOT_BACKUP_GPT
	gpt_head->alternate_lba = 0;
#else
	gpt_head->alternate_lba = total_sectors - 1;
#endif
	gpt_head->first_usable_lba = sunxi_mbr->array[0].addrlo + logic_offset;
#ifdef CONFIG_SUNXI_NOT_BACKUP_GPT
	/*Do not reserve backup space(1 GPT head + 32 GPT entry)*/
	gpt_head->last_usable_lba = total_sectors - 1;
#else
	/*1 GPT head + 32 GPT entry*/
	gpt_head->last_usable_lba = total_sectors - (1 + 32) - 1;
#endif
	eLIBs_memcpy(gpt_head->disk_guid.b,guid,16);
	gpt_head->partition_entry_lba = 2;
	gpt_head->num_partition_entries = sunxi_mbr->PartCount;
	gpt_head->sizeof_partition_entry = GPT_ENTRY_SIZE;
	gpt_head->partition_entry_array_crc32 = 0;
	data_len += 512;

	/* 3. LBA2~LBAn: fill gpt entry */
	gpt_entry_start = (pbuf + data_len);
	for(i=0;i<sunxi_mbr->PartCount;i++) {
		pgpt_entry = (gpt_entry *)(gpt_entry_start + (i)*GPT_ENTRY_SIZE);

		pgpt_entry->partition_type_guid = PARTITION_BASIC_DATA_GUID;

		eLIBs_memcpy(pgpt_entry->unique_partition_guid.b, part_guid, 16);
		pgpt_entry->unique_partition_guid.b[15] = part_guid[15] + i;

		pgpt_entry->starting_lba = ((u64)sunxi_mbr->array[i].addrhi<<32) + sunxi_mbr->array[i].addrlo + logic_offset;
		pgpt_entry->ending_lba = pgpt_entry->starting_lba \
			+((u64)sunxi_mbr->array[i].lenhi<<32)  \
			+ sunxi_mbr->array[i].lenlo-1;

		/* UDISK partition */
		if(i == sunxi_mbr->PartCount-1) {
			pgpt_entry->ending_lba = gpt_head->last_usable_lba;
#ifdef CONFIG_SUNXI_UBIFS
			if (nand_use_ubi()) {
				/* backup gpt not belong to any volumes */
				pgpt_entry->ending_lba = pgpt_entry->starting_lba + sunxi_mbr->array[i].lenlo - 1;
			}
#endif
		}

		if(sunxi_mbr->array[i].ro == 1) {
			pgpt_entry->attributes.fields.type_guid_specific = 0x6000;
		}
		else {
			pgpt_entry->attributes.fields.type_guid_specific = 0x8000;
		}

		if(sunxi_mbr->array[i].keydata == 0x8000){
			pgpt_entry->attributes.fields.keydata = 1;
		}

		//ASCII to unicode
		eLIBs_memset(pgpt_entry->partition_name, 0,PARTNAME_SZ*sizeof(efi_char16_t));
		for(j=0;j < eLIBs_strlen((const char *)sunxi_mbr->array[i].name);j++ ) {
			pgpt_entry->partition_name[j] = (efi_char16_t)sunxi_mbr->array[i].name[j];
		}
		if (i == sunxi_mbr->PartCount - 1 && storage_type == STORAGE_NOR)
		{
			pgpt_entry->ending_lba -= (sunxi_gey_storage_logic_offset()/512);
		}
		data_len += GPT_ENTRY_SIZE;

	}

	//entry crc
	gpt_head->partition_entry_array_crc32 = calc_crc32((unsigned char const *)gpt_entry_start,
	             (gpt_head->num_partition_entries)*(gpt_head->sizeof_partition_entry));
	//gpt crc
	gpt_head->header_crc32 = calc_crc32((const unsigned char *)gpt_head, sizeof(gpt_header));
	/* 4. LBA-1: the last sector fill backup gpt header */

	return data_len;
}
int Sunxi_Get_LocalGpt(char *LocalGpt_Buf) {
    int ret = 0;
	blk_dev_get_data_t get_data_request = {0};

	get_data_request.len = GPT_TABLE_SIZE;
	get_data_request.addr = GPT_ADDRESS;
	get_data_request.buff = LocalGpt_Buf;
	ret = sunxi_flash_read(&get_data_request);
	if (ret == EPDK_FAIL) {
        eLIBs_printf("%s %d sunxi_flash_read Fail!\n", __FILE__, __LINE__);
        return EPDK_FAIL;
	}
	{
		gpt_header *gpt_head = (gpt_header *)(LocalGpt_Buf + GPT_HEAD_OFFSET);
		u32 i;
		
		if (gpt_head->signature == GPT_HEADER_SIGNATURE) {
			u32 calc_crc32_   = 0;
			u32 backup_crc32_ = 0;

			backup_crc32_	   = gpt_head->header_crc32;
			gpt_head->header_crc32 = 0;
			calc_crc32_ = calc_crc32((const unsigned char *)gpt_head, sizeof(gpt_header));
			gpt_head->header_crc32 = backup_crc32_;
			if (calc_crc32_ != backup_crc32_) {
				eLIBs_printf("%s %d the Local GPT table is bad\n", __FILE__, __LINE__);
				eLIBs_memset(LocalGpt_Buf, 0, GPT_TABLE_SIZE); 
			}
		} else {
			eLIBs_printf("%s %d gpt_head->signature:%x not eq GPT_HEADER_SIGNATURE!\n", __FILE__, __LINE__, gpt_head->signature);
			eLIBs_memset(LocalGpt_Buf, 0, GPT_TABLE_SIZE); 
		}
	}
#ifdef UPDATE_DEBUG
	sunxi_gpt_dump(LocalGpt_Buf);
#endif	
    return EPDK_OK;
}
int Sunxi_Get_GptPart_info(char *Gpt_Buf, gpt_entry *pgpt_entry) {	
	int i, j;

	char char8_name[PARTNAME_SZ] = {0};
	gpt_header *gpt_head	 = (gpt_header *)(Gpt_Buf + GPT_HEAD_OFFSET);
	gpt_entry *entry	     = (gpt_entry *)(Gpt_Buf + GPT_ENTRY_OFFSET);

	for (i = 0; i < gpt_head->num_partition_entries; i++) {
		for (j = 0; j < PARTNAME_SZ; j++) {
			char8_name[j] = (char)(entry[i].partition_name[j]);
		}
		if (eLIBs_strcmp(UpdateStrTolower((char *)pgpt_entry->partition_name), UpdateStrTolower(char8_name)) == 0) {
			pgpt_entry->attributes = entry[i].attributes;
			pgpt_entry->starting_lba = entry[i].starting_lba;
			pgpt_entry->ending_lba = entry[i].ending_lba;	
#ifdef UPDATE_DEBUG
			eLIBs_printf("GPT:%-12s: %-12llx  %-12llx\n", char8_name,
				   entry[i].starting_lba, entry[i].ending_lba);
#endif
			return EPDK_OK;			
		}
	}
    return EPDK_FAIL;
}

int Sunxi_Verify_Mbr(void *buffer) {
	sunxi_mbr_t *mbr;
	char        *tmp_buf = (char *)buffer, *gpt_buf = NULL;
	int          i = 0, mbr_num = SUNXI_MBR_COPY_NUM;
	unsigned int crc = 0;
	tmp_buf = buffer;

#ifdef UPDATE_DEBUG
	eLIBs_printf("%s %s %d\n", __FILE__, __func__, __LINE__);
#endif		
	if (sunxi_boot_storage_type() == STORAGE_NOR) {
	   mbr_num = 1;
	}

	for (i = 0; i < mbr_num; i++) {
		mbr = (sunxi_mbr_t *)tmp_buf;
		crc = calc_crc32((const unsigned char *)(tmp_buf + 4), SUNXI_MBR_SIZE - 4);
#ifdef UPDATE_DEBUG
		eLIBs_printf("mbr->crc32:0x%x\n", mbr->crc32);
		eLIBs_printf("crc:0x%x\n", crc);
#endif		
		if(crc != mbr->crc32)
		{
			eLIBs_printf("mbr->crc32:0x%x\n", mbr->crc32);
			eLIBs_printf("crc:0x%x\n", crc);
			eLIBs_printf("%s %d the %d mbr table is bad\n", __FILE__, __LINE__, i);
			return EPDK_FAIL;
		}
		else
		{
			eLIBs_printf("the %d mbr table is ok\n", i);
			tmp_buf += SUNXI_MBR_SIZE;
		}
	}
#ifdef UPDATE_DEBUG
	sunxi_mbr_dump(buffer);
#endif
    return EPDK_OK;
}

int Sunxi_Verify_NewGpt(char *Gpt_Buf) {
	int ret = 0;
	__u64 item_size = 0;
	static sunxi_mbr_t Mbr = {0};
	__Update_Info_t info = {0};

	eLIBs_memset(&info, 0x00, sizeof(info));
	eLIBs_strncpy(info.MainType, "12345678", MAINTYPE_LEN);
	eLIBs_strncpy(info.subType, "1234567890___MBR", SUBTYPE_LEN);
	item_size = FW_GetItemSize(&info);
	eLIBs_printf("%s %d %s item_size:%d\n", __FILE__, __LINE__, __func__, item_size);
	info.Data = (__u8 *)esMEMS_Malloc(0, item_size);
	if (info.Data == NULL) {
		eLIBs_printf("Hal_malloc Fail Size:%d\n", item_size);
		return EPDK_FAIL;
	}
	eLIBs_memset(info.Data, 0xFF, item_size);
	FW_ImgItem_Open(&info);
	FW_GetItemData(&info, item_size);
	ret = sunxi_mbr_convert_to_gpt(info.Data, Gpt_Buf, sunxi_boot_storage_type());
#ifdef UPDATE_DEBUG
	sunxi_gpt_dump(Gpt_Buf);
#endif
	FW_ImgItem_Close();
	esMEMS_Mfree(0, info.Data);
	info.Data = NULL;
	return ret;
}
static int sunxi_sprite_verify_dlmap(void *buffer)
{
	sunxi_download_info *local_dlmap;
	char        *tmp_buf = (char *)buffer;

	tmp_buf = buffer;
   	local_dlmap = (sunxi_download_info *)tmp_buf;
    if(calc_crc32((const unsigned char *)(tmp_buf + 4), SUNXI_MBR_SIZE - 4) != local_dlmap->crc32)
    {
    	eLIBs_printf("downlaod map is bad\n");
    	return EPDK_FAIL;
    }

    return 0;
}

int Sunxi_Get_DownLoadFile_Is_Null_PartInfo(char *Gpt_Buf, sunxi_download_info *dl_map, gpt_entry *pgpt_entry) {	
	int i, j, Index, totoal = 0;
	dl_one_part_info		*part_info;

	char char8_name[PARTNAME_SZ] = {0};
	gpt_header *gpt_head	 = (gpt_header *)(Gpt_Buf + GPT_HEAD_OFFSET);
	gpt_entry *entry	     = (gpt_entry *)(Gpt_Buf + GPT_ENTRY_OFFSET);

	for (i = 0; i < gpt_head->num_partition_entries; i++) {
		
		eLIBs_memset(char8_name, 0x00, sizeof(char)*PARTNAME_SZ);
		for (j = 0; j < PARTNAME_SZ; j++) {
			char8_name[j] = (char)(entry[i].partition_name[j]);
		}
		/*在此处添加不需要处理的分区名称*/
		if (eLIBs_strcmp(UpdateStrTolower("Private"), UpdateStrTolower(char8_name)) == 0 
//			|| eLIBs_strcmp(UpdateStrTolower("Udisk"), UpdateStrTolower(char8_name)) == 0) {
		) {
			continue;
		}
			
		for (Index = 0, part_info = dl_map->one_part_info; Index < dl_map->download_count; Index++, part_info++) {
			if (eLIBs_strcmp(UpdateStrTolower(part_info->name), UpdateStrTolower(char8_name)) == 0) {
				break;			
			}
		}
		if (Index == dl_map->download_count) {
			eLIBs_strcpy((char *)pgpt_entry->partition_name, char8_name);
			pgpt_entry->attributes = entry[i].attributes;
			pgpt_entry->starting_lba = entry[i].starting_lba;
			pgpt_entry->ending_lba = entry[i].ending_lba;	
#ifdef UPDATE_DEBUG
			eLIBs_printf("GPT:%-12s %-12llx  %-12llx\n", pgpt_entry->partition_name,
				   pgpt_entry->starting_lba, pgpt_entry->ending_lba);
#endif
			pgpt_entry++;
			totoal++;
		}
	}
    return totoal;
}

int Sunxi_Get_download_map(sunxi_download_info *dl_map)
{
	__Update_Info_t info = {0};
		
	eLIBs_memset(&info, 0x00, sizeof(info));
	eLIBs_strncpy(info.MainType, "12345678", MAINTYPE_LEN);
	eLIBs_strncpy(info.subType, "1234567890DLINFO", SUBTYPE_LEN);
	FW_ImgItem_Open(&info);
	info.Data = (__u8 *)dl_map;
	FW_GetItemData(&info, sizeof(sunxi_download_info));
	FW_ImgItem_Close();
	if (sunxi_sprite_verify_dlmap(dl_map) == EPDK_FAIL) {
    	eLIBs_printf("downlaod map is bad\n");
    	return EPDK_FAIL;
	}
#ifdef UPDATE_DEBUG
	__dump_dlmap(dl_map);
#endif
	return EPDK_OK;
}

