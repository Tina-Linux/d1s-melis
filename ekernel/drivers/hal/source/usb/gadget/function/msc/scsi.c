/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY鈥橲 TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS鈥橲DK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY鈥橲 TECHNOLOGY.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <scsi.h>
#include <sunxi_hal_usb.h>

#include <storage.h>
#include <hal_log.h>

//extern usb_handle *h;

static inline u16 get_be16(u8 *buf)
{
    return ((__u16) buf[0] << 8) | ((__u16) buf[1]);
}

static inline u32 get_be32(u8 *buf)
{
    return ((__u32) buf[0] << 24) | ((__u32) buf[1] << 16) |
           ((__u32) buf[2] << 8) | ((__u32) buf[3]);
}

static inline void put_be16(__u8 *buf, __u16 val)
{
    buf[0] = (__u8)(val >> 8);
    buf[1] = (__u8)val;
}

static inline void put_be32(__u8 *buf, __u32 val)
{
    buf[0] = (__u8)(val >> 24);
    buf[1] = (__u8)(val >> 16);
    buf[2] = (__u8)(val >> 8);
    buf[3] = (__u8)val;
}


u32 is_msc_buffer_invalid(struct msc_slave_dev *fsg)
{
    u32 i = 0;
    struct fsg_buffhd *bh = NULL;

    //dequeue all req and clear buffer state
    for (i = 0; i < NUM_BUFFERS; ++i)
    {
        bh = &fsg->buffhds[i];

        //      hal_log_info("is_msc_buffer_invalid: bh->invalid = %d, bh->state = %d", bh->invalid, bh->state);
        //      hal_log_info("is_msc_buffer_invalid: bh->inreq_busy = %d, bh->outreq_busy = %d", bh->inreq_busy, bh->outreq_busy);

        if (bh->invalid)
        {
            return 1;
        }
    }

    return 0;
}

/*
*******************************************************************************
*                     USB_OS_READ
*
* Description:
*    设备读
*
* Parameters:
*    buff    :  存放读的结构
*    offset  :  读的起始地址,不是起始扇区
*    count   :  读的长度
*    fp      :  设备文件
*
* Return value:
*    返回读的大小
*
* note:
*    无
*
*******************************************************************************
*/
__u32 USB_OS_READ(void *buff, __u32 offset, __u32 count, void *fp)
{
    s32 read_result = 0;

    if ((buff == NULL) || (fp == NULL))
    {
        return 0;
    }

    //这里的接口的单位都为sector
    read_result = esFSYS_pread(buff, offset, count, (__hdle)fp);
    return read_result;
}

/*
*******************************************************************************
*                     USB_OS_WRITE
*
* Description:
*    设备写
*
* Parameters:
*    buff    :  存放读的结构
*    offset  :  读的起始地址,不是起始扇区
*    count   :  读的长度
*    fp      :  设备文件
*
* Return value:
*    返回读的大小
*
* note:
*    无
*
*******************************************************************************
*/
__u32 USB_OS_WRITE(void *buf, __u32 offset, __u32 count, void *fp)
{
    s32 write_result = 0;
    if ((buf == NULL) || (fp == NULL))
    {
        return 0;
    }

    //这里的接口的单位都为sector
    write_result = esFSYS_pwrite(buf, offset, count, (__hdle)fp);
    return write_result;
}

void dump_cbw(struct bulk_cb_wrap *cbw)
{	
	printf("%s %d %s\n", __FILE__, __LINE__, __func__);
	printf("Signature:			%x \n", cbw->Signature);
	printf("Tag:	  			%x \n", cbw->Tag);
	printf("DataTransferLength: %x \n", cbw->DataTransferLength);
	printf("Flags: 				%x \n", cbw->Flags);
	printf("Lun: 				%x \n", cbw->Lun);
	printf("Length: 			%x \n", cbw->Length);
	printf("CDB:\n");
	{
		int i = 0;
		for(i = 0; i < 16 ;i++)
		{
			printf("%x ", cbw->CDB[i]);
		}
	}	
	printf("\n");
}
int received_cbw(struct msc_slave_dev *fsg, struct fsg_buffhd *bh)
{
    int len;
    struct bulk_cb_wrap *cbw = (struct bulk_cb_wrap *) bh->buf;
	len = bh->bulk_out_intended_length;

	hal_log_info("received_cbw");
    //cbw_len必须为31，cbw_Signature必须为0x43425355
    if (len != USB_BULK_CB_WRAP_LEN || cbw->Signature != (USB_BULK_CB_SIG))
    {
        hal_log_err("PANIC : invalid CBW: len = %u value =  0x%x", len, (cbw->Signature));
        return -EINVAL;
    }
//	dump_cbw(cbw);
    //判断CBW 内容是否有意义
    //如果逻辑单元号不在设备所支持的范围内，数据传输方向
    //无效，命令长度不在规定范围内
    //就视为此CBW 内容无效
    if (cbw->Lun >= CONFIG_DRIVERS_USB_GADGET_MSC_MAX_LUNS || cbw->Flags & ~USB_BULK_IN_FLAG ||
        cbw->Length < 6 || cbw->Length > MAX_COMMAND_SIZE)
    {
        hal_log_err("PANIC : non-meaningful CBW: lun = %u, flags = 0x%x cmdlen %u", cbw->Lun, cbw->Flags, cbw->Length);
        return -EINVAL;
    }

    /* Save the command for later */
    fsg->cmnd_size = cbw->Length;
    memcpy((void *)fsg->cmnd, (void *)cbw->CDB, fsg->cmnd_size);

    //--<5>--获得数据传输方向
    if (cbw->Flags & USB_BULK_IN_FLAG)
    {
        fsg->data_dir = DATA_DIR_TO_HOST;
    }
    else
    {
        fsg->data_dir = DATA_DIR_FROM_HOST;
    }

    //--<6>--获得cbw中数据传输长度
    fsg->data_size = (cbw->DataTransferLength); //获得长度
    if (fsg->data_size == 0)
    {
        fsg->data_dir = DATA_DIR_NONE;
    }

    fsg->lun = cbw->Lun;  //逻辑单元号
    fsg->tag = cbw->Tag;  //命令块标签
    return 0;
}
static int check_command(struct msc_slave_dev *fsg, int cmnd_size,
                         enum data_direction data_dir, unsigned int mask,
                         int needs_medium, const char *name)
{
    int  i;
    int  lun = fsg->cmnd[1] >> 5;
    struct lun  *curlun;

    hal_log_info("ck: (%x,%x,%x)", fsg->cmnd[0], get_be32(&fsg->cmnd[2]), get_be16(&fsg->cmnd[7]));
    //--<1>--check 协议
    if (protocol_is_scsi())
    {
        if (fsg->cmnd[0] == SC_INQUIRY)
        {
            //windowns中格式前后，SC_INQUIRY命令格式不一样
            cmnd_size = fsg->cmnd_size;
        }
    }
    else if (mod_data.protocol_type == USB_SC_RBC)
    {
        if (fsg->cmnd_size == 12)
        {
            cmnd_size = 12;
        }
    }
    else
    {
        cmnd_size = 12;
    }

    //--<2>--check data direction and size
    if (fsg->data_size_from_cmnd == 0)
    {
        data_dir = DATA_DIR_NONE;
    }

    if (fsg->data_dir == DATA_DIR_UNKNOWN)      // CB or CBI
    {
        fsg->data_dir = data_dir;
        fsg->data_size = fsg->data_size_from_cmnd;
    }
    else        // Bulk-only
    {
        if (fsg->data_size < fsg->data_size_from_cmnd)
        {
            fsg->data_size_from_cmnd = fsg->data_size;
            fsg->phase_error = 1;
        }
    }

    fsg->residue = fsg->usb_amount_left = fsg->data_size;

    /* Conflicting data directions is a phase error */
    if (fsg->data_dir != data_dir && fsg->data_size_from_cmnd > 0)
    {
        fsg->phase_error = 1;
        hal_log_err("PANIC : check_command() : dir error");
        return -EINVAL;
    }

    /* Verify the length of the command itself */
    if (cmnd_size != fsg->cmnd_size)
    {
        /* Special case workaround: MS-Windows issues REQUEST SENSE
         * with cbw->Length == 12 (it should be 6). */
        if (fsg->cmnd[0] == SC_REQUEST_SENSE && fsg->cmnd_size == 12)
        {
            cmnd_size = fsg->cmnd_size;
        }
        else
        {
            fsg->phase_error = 1;
            hal_log_err("PANIC : check_command() : cmd len");
            hal_log_err("cmnd_size = %x, fsg->cmnd_size = %x", cmnd_size, fsg->cmnd_size);
            return -EINVAL;
        }
    }

    //--<3>--check lun
    if (!transport_is_bbb()) {
		printf("%s %d %s lun:%d\n", __FILE__, __LINE__, __func__, lun);
        fsg->lun = lun;     // Use LUN from the command
    }

    /* Check the LUN */
    if ((int)(fsg->lun) >= 0 && fsg->lun < fsg->nluns) {
        fsg->curlun = curlun = &fsg->luns[fsg->lun];
        if (fsg->cmnd[0] != SC_REQUEST_SENSE) {
            curlun->sense_data = SS_NO_SENSE;
            curlun->sense_data_info = 0;
        }
    } else {
        fsg->curlun = curlun = NULL;
        fsg->bad_lun_okay = 0;
        /* INQUIRY and REQUEST SENSE commands are explicitly allowed
         * to use unsupported LUNs; all others may not. */
        if (fsg->cmnd[0] != SC_INQUIRY && fsg->cmnd[0] != SC_REQUEST_SENSE) {
            hal_log_err("PANIC : check_command() : unsupported LUN %d", fsg->lun);
            return -EINVAL;
        }
    }

    //--<3-1>--check lun sense data
    if (curlun && curlun->unit_attention_data != SS_NO_SENSE && fsg->cmnd[0] != SC_INQUIRY && fsg->cmnd[0] != SC_REQUEST_SENSE)
    {
        curlun->sense_data = SS_NO_SENSE ;
        curlun->unit_attention_data = SS_NO_SENSE;
        //do_interface以后unit_attention_data会被改为SS_RESET_OCCURRED
        //除了SC_INQUIRY和SC_REQUEST_SENSE命令，其他命令执行前都
        //应该清除unit_attention_data
        //return -EINVAL;
    }

    //--<3-2>--去掉lun 号，check cmnd 的其它位
    //vista的inquiry中fsg->cmnd[1]中的bit0被置成了0，因此这里
    //把inquiry 过滤掉
    fsg->cmnd[1] &= 0x1f;           // Mask away the LUN
#if 0
    for (i = 1; i < cmnd_size; ++i)
    {
        if (fsg->cmnd[i] && !(mask & (1 << i)))
        {
            if (curlun)
            {
                curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
            }

            hal_log_err("PANIC : check_command() : mask error");
            hal_log_err("fsg->cmnd[%d]=%x, mask=%d", i, fsg->cmnd[i], mask);
            return -EINVAL;
        }
    }
#else
    for (i = 1; i < cmnd_size; ++i) {
        if (fsg->cmnd[0] != SC_INQUIRY && curlun) {
            if (fsg->cmnd[i] && !(mask & (1 << i))) {
                if (curlun) {
                    curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
                }

                hal_log_err("PANIC : check_command() : mask error");
                hal_log_err("fsg->cmnd[%d]=%x, mask=%d", i, fsg->cmnd[i], mask);
                return -EINVAL;
            }
        }
    }
#endif

    //check 介质是否存在
    if (curlun && !backing_file_is_open(curlun) && needs_medium) {
        curlun->sense_data = SS_MEDIUM_NOT_PRESENT;
        //u盘弹出后，curlun==NULL，但是USB继续工作，因此直接返回
        //return -EINVAL;
		//printf("%s %d %s has error\n", __FILE__, __LINE__, __func__);
		//printf("%s %d %s fsg->lun:%d\n", __FILE__, __LINE__, __func__, fsg->lun);
		//printf("%s %d %s fsg->nluns:%d\n", __FILE__, __LINE__, __func__, fsg->nluns);
		//printf("%s %d %s curlun:0x%x\n", __FILE__, __LINE__, __func__, curlun);
		//printf("%s %d %s backing_file_is_open(curlun):0x%x\n", __FILE__, __LINE__, __func__, backing_file_is_open(curlun));
		//printf("%s %d %s needs_medium:0x%x\n", __FILE__, __LINE__, __func__, needs_medium);
    }

    if (curlun && is_media_change && (curlun_back->filp != NULL) && (curlun_back->filp == curlun->filp)) {
        is_media_change    = 0;
        curlun->sense_data = SS_NOT_READY_TO_READY_TRANSITION;
        hal_log_err("is_media_change = %d, sense_data = 0x%x", is_media_change, curlun->sense_data);
    }
//	printf("%s %d %s \n", __FILE__, __LINE__, __func__);
    return 0;
}

 /*
 *********************************************************************
 *					   do_inquiry
 *
 * Description:
 *	  inquiry 请求处理函数
 *
 * Parameters:
 *	  fsg  :  usb 设备驱动
 *	  bh   :  存放处理结果的缓冲区
 *
 * Return value:
 *	  返回inquiry命令的长度
 *
 * note:
 *	  无
 *
 *********************************************************************
 */
 static int do_inquiry(struct msc_slave_dev *fsg, struct fsg_buffhd *bh)
 {
	 u8  *buf	   = (u8 *) bh->buf;
	 char VendorInformation[32] = {0};
	 char ProductIdentification[32] = {0};
	 char ProductRevision[32] = {0};
 
	 if (udisk_info.VendorInformation[0] == (__u32)NULL)
	 {
		 memcpy(VendorInformation,
					   USB_DEVICE_VENDOR_INFORMATION,
					   sizeof(USB_DEVICE_VENDOR_INFORMATION));
	 }
	 else if (0 == strcmp("null", udisk_info.VendorInformation))
	 {
		 memset((VendorInformation),
					   0,
					   sizeof(VendorInformation));
	 }
	 else
	 {
		 memcpy(VendorInformation,
					   udisk_info.VendorInformation,
					   sizeof(udisk_info.VendorInformation));
	 }
 
	 if (udisk_info.ProductIdentification[0] == (__u32)NULL)
	 {
		 memcpy(ProductIdentification,
					   USB_DEVICE_PRODUCT_INFORMATION,
					   sizeof(USB_DEVICE_PRODUCT_INFORMATION));
	 }
	 else if (0 == strcmp("null", udisk_info.ProductIdentification))
	 {
		 memset((ProductIdentification),
					   0,
					   sizeof(ProductIdentification));
	 }
	 else
	 {
		 memcpy(ProductIdentification,
					   udisk_info.ProductIdentification,
					   sizeof(udisk_info.ProductIdentification));
	 }
 
	 if (udisk_info.ProductRevision[0] == (__u32)NULL)
	 {
		 memcpy(ProductRevision,
					   USB_DEVICE_PRODUCT_REVISION,
					   sizeof(USB_DEVICE_PRODUCT_REVISION));
	 }
	 else if (0 == strcmp("null", udisk_info.ProductRevision))
	 {
		 memset((ProductRevision),
					   0,
					   sizeof(ProductRevision));
	 }
	 else
	 {
		 memcpy(ProductRevision,
					   udisk_info.ProductRevision,
					   sizeof(udisk_info.ProductRevision));
	 }
 
	 if (!fsg->curlun)			 // Unsupported LUNs are okay
	 {
		 fsg->bad_lun_okay = 1;
		 memset(buf, 0, 36);
		 buf[0] = 0x7f; 		 // Unsupported, no device-type
		 return 36;
	 }
 
	 memset(buf, 0, 36);  // Non-removable, direct-access device
 
	 if (mod_data.removable)
	 {
		 buf[1] = 0x80;   //第7位为可移动位
	 }
 
	 buf[2] = 2;	 // ANSI SCSI level 2，设备支持scsi2
	 buf[3] = 2;	 // SCSI-2 INQUIRY data format
	 buf[4] = 31;	 // Additional length
	 // No special options
	 //vendor	 8byte
	 memcpy((buf + 8), VendorInformation, 8);
	 //product	 16byte
	 memcpy((buf + 16), ProductIdentification, 16);
	 //revision  4byte
	 memcpy((buf + 32), ProductRevision, 4);
	 hal_log_info("Device vendor	= %s", VendorInformation);
	 hal_log_info("Device product	= %s", ProductIdentification);
	 hal_log_info("Device revision = %s", ProductRevision);
	 return 36;
 }


 /*
 *********************************************************************
 *					   do_mode_select
 *
 * Description:
 *	  mode_select 请求处理函数
 *
 * Parameters:
 *	  fsg  :  usb 设备驱动
 *	  bh   :  存放处理结果的缓冲区
 *
 * Return value:
 *
 *
 * note:
 *	  无
 *
 *********************************************************************
 */
 static int do_mode_select(struct msc_slave_dev *fsg, struct fsg_buffhd *bh)
 {
	 struct lun  *curlun = fsg->curlun;
	 /* We don't support MODE SELECT */
	 curlun->sense_data = SS_INVALID_COMMAND;
	 return -EINVAL;
 }

 

 /*
 *********************************************************************
 *					   prevent_allow
 *
 * Description:
 *	  prevent_allow 请求处理函数
 *
 * Parameters:
 *	  fsg  :  usb 设备驱动
 *
 * Return value:
 *	  0  :	成功
 *	  !0 :	失败
 *
 * note:
 *	  无
 *
 *********************************************************************
 */
 static int do_prevent_allow(struct msc_slave_dev *fsg)
 {
	 struct lun  *curlun = fsg->curlun;
	 int	 prevent;
 
	 if (!mod_data.removable)
	 {
		 curlun->sense_data = SS_INVALID_COMMAND;
		 hal_log_err("PANIC : Prevent Allow : this lun.removable == 0,so can not accept this cmd!");
		 return -EINVAL;
	 }
 
	 //目前只支持prevent = 00b
	 prevent = fsg->cmnd[4] & 0x01;
 
	 if ((fsg->cmnd[4] & ~0x01) != 0)		 // Mask away Prevent
	 {
		 curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
		 return -EINVAL;
	 }
 
	 curlun->prevent_medium_removal = prevent;
	 return 0;
 }

 /*
 *********************************************************************
 *					   do_read_capacity
 *
 * Description:
 *	  read_capacity 请求处理函数
 *
 * Parameters:
 *	  fsg  :  usb 设备驱动
 *	  bh   :  存放处理结果的缓冲区
 *
 * Return value:
 *	  返回read_capacity命令的长度
 *
 * note:
 *	  无
 *
 *********************************************************************
 */
 static int do_read_capacity(struct msc_slave_dev *fsg, struct fsg_buffhd *bh)
 {
	 struct lun  *curlun = fsg->curlun;
	 u32  lba = get_be32(&fsg->cmnd[2]);
	 int  pmi = fsg->cmnd[8];
	 u8   *buf = (u8 *) bh->buf;
 
	 //Check the PMI and LBA fields
	 if (pmi > 1 || (pmi == 0 && lba != 0))
	 {
		 curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
		 hal_log_err("PANIC : Read Capacity : some fields should be zero!");
		 return -EINVAL;
	 }
 
	 if (curlun->num_sectors)
	 {
		 put_be32(&buf[0], curlun->num_sectors - 1); // Max logical block
		 put_be32(&buf[4], curlun->sector_size);	 // Block length
		 return 8;
	 }
	 else
	 {
		 memset(buf, 0, 12);
		 ENTER_CRITICAL(bh->bh_lock);
		 //bh->inreq->length = 0;
		 bh->in_len = 0;
		 EXIT_CRITICAL(bh->bh_lock);
		 return 0;
	 }
 }

 /*
 *********************************************************************
 *					   do_mode_sense
 *
 * Description:
 *	  mode_sense 请求处理函数
 *
 * Parameters:
 *	  fsg  :  usb 设备驱动
 *	  bh   :  存放处理结果的缓冲区
 *
 * Return value:
 *	  返回mode_sense命令的长度
 *
 * note:
 *	  无
 *
 *********************************************************************
 */
 static int do_mode_sense(struct msc_slave_dev *fsg, struct fsg_buffhd *bh)
 {
	 struct lun  *curlun = fsg->curlun;
	 int mscmnd = fsg->cmnd[0];
	 u8  *buf = (u8 *) bh->buf;
	 u8  *buf0 = buf;
	 int pc = 0;		  //PCF页控制字段
	 int page_code = 0;   //页代码
	 int changeable_values = 0, all_pages = 0;
	 int valid_page = 0;
	 int len = 0, limit = 0;
 
	 //解析mode sense的第1字节，DBD禁止块描述符
	 //当该位被置1时，在这页之前不发送块描述符。
	 if ((fsg->cmnd[1] & ~0x08) != 0)		 // Mask away DBD
	 {
		 curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
		 hal_log_err("PANIC : Mode Sense : cmd error!");
		 return -EINVAL;
	 }
 
	 //解析mode sense的第2字节，PCF页控制字段
	 //00b：当前值, 01b：可变值, 10b：缺省值, 11b：保存值
	 pc = fsg->cmnd[2] >> 6;
	 page_code = fsg->cmnd[2] & 0x3f;
 
	 if (pc == 3)
	 {
		 curlun->sense_data = SS_SAVING_PARAMETERS_NOT_SUPPORTED;
		 hal_log_err("PANIC : Mode Sense : cmd not supprt now!");
		 return -EINVAL;
	 }
 
	 changeable_values = (pc == 1);
	 all_pages = (page_code == 0x3f);
	 //模式参数头标的位 0~1: 数据长度 2:介质类型  3:设备特定  4~5: 保留
	 //6~7: 块描述符长度
	 memset(buf, 0, 8);
 
	 //是否写保护，带缓冲区
	 if (mscmnd == SC_MODE_SENSE_6)
	 {
		 buf[2] = (curlun->ro ? 0x80 : 0x00);		 // WP, DPOFUA
		 buf += 4;
		 limit = 255;
		 len = fsg->cmnd[4];
	 }
	 else		 // SC_MODE_SENSE_10
	 {
		 buf[3] = (curlun->ro ? 0x80 : 0x00);		 // WP, DPOFUA
		 buf += 8;
		 limit = mod_data.buflen;		 // Should really be mod_data.buflen
		 len = (fsg->cmnd[7] << 8) | fsg->cmnd[8];
	 }
 
	 memset(buf, 0, (len - (buf - buf0)));
 
	 //目前只支持0x3f和0x08页
	 if (all_pages != 1 && page_code != 0x08)
	 {
		 return len;
	 }
 
	 //处理0x3f和0x08页
	 if (page_code == 0x08 || all_pages)
	 {
		 valid_page = 1;
		 buf[0] = 0x08; 	 // Page code
		 buf[1] = 10;		 // Page length
		 memset(buf + 2, 0, 10);  // None of the fields are changeable
 
		 if (!changeable_values)
		 {
			 buf[2] = 0x04;  // Write cache enable,
			 // Read cache not disabled
			 // No cache retention priorities
			 put_be16(&buf[4], 0xffff);  // Don't disable prefetch
			 // Minimum prefetch = 0
			 put_be16(&buf[8], 0xffff);  // Maximum prefetch
			 put_be16(&buf[10], 0xffff); // Maximum prefetch ceiling
		 }
 
		 buf += 12;
	 }
 
	 /* Check that a valid page was requested and the mode data length
	  * isn't too long. */
	 if (!valid_page || len > limit)
	 {
		 hal_log_err("PANIC : Mode Sense : invalid field ,valid_page = %d , len = %d ,limit = %d",
					valid_page, len, limit);
		 curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
		 return -EINVAL;
	 }
 
	 /*  Store the mode data length */
	 if (mscmnd == SC_MODE_SENSE_6)
	 {
		 buf0[0] = len - 1;
	 }
	 else
	 {
		 put_be16(buf0, len - 2);
	 }
 
	 return len;
 }


 /*
 *********************************************************************
 *					   do_read_format_capacities
 *
 * Description:
 *	  read_format_capacities 请求处理函数
 *
 * Parameters:
 *	  fsg  :  usb 设备驱动
 *	  bh   :  存放处理结果的缓冲区
 *
 * Return value:
 *	  返回read_format_capacities命令的长度
 *
 * note:
 *	  无
 *
 *********************************************************************
 */
 static int do_read_format_capacities(struct msc_slave_dev *fsg, struct fsg_buffhd *bh)
 {
	 struct lun  *curlun = fsg->curlun;
	 u8 	 *buf = (u8 *) bh->buf;
 
	 if (curlun->num_sectors)
	 {
		 buf[0] = buf[1] = buf[2] = 0;
		 buf[3] = 8;	 // Only the Current/Maximum Capacity Descriptor
		 buf += 4;
		 put_be32(&buf[0], curlun->num_sectors);	 // Number of blocks
		 put_be32(&buf[4], 512);			 // Block length
		 buf[4] = 0x02; 				 // Current capacity
		 return 12;
	 }
	 else
	 {
		 memset(buf, 0, 12);
		 ENTER_CRITICAL(bh->bh_lock);
		 //bh->inreq->length = 0;
		 bh->in_len = 0;
		 EXIT_CRITICAL(bh->bh_lock);
		 return 0;
	 }
 }

 /*
 *********************************************************************
 *						   do_start_stop
 *
 * Description:
 *	  start_stop 请求处理函数
 *
 * Parameters:
 *	  fsg  :  usb 设备驱动
 *
 * Return value:
 *	  0  :	成功
 *	  !0 :	失败
 *
 * note:
 *	  无
 *
 *********************************************************************
 */
 static int do_start_stop(struct msc_slave_dev *fsg)
 {
	 struct lun  *curlun = fsg->curlun;
	 int	 loej, start;
 
	 if (!mod_data.removable)
	 {
		 curlun->sense_data = SS_INVALID_COMMAND;
		 hal_log_err("PANIC : Start Stop : this lun.removable == 0,so can not accept this cmd!");
		 return -EINVAL;
	 }
 
	 //immediately目前不支持
	 // int immed = fsg->cmnd[1] & 0x01;
	 loej = fsg->cmnd[4] & 0x02;
	 start = fsg->cmnd[4] & 0x01; //start 标志位
 
	 printf("%s %d %s \n", __FILE__, __LINE__, __func__);
	 //bit1，只有Immed有效
	 //bit4，只有LoEj, Start有效，u盘里power condition默认为START_VALID(0h)
	 if ((fsg->cmnd[1] & ~0x01) != 0 || 	 // Mask away Immed
		 (fsg->cmnd[4] & ~0x03) != 0)	 // Mask LoEj, Start
	 {
		 curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
		 hal_log_err("mask Immed, LoEj, Start failed");
		 return -EINVAL;
	 }
 
	 //start=0,loej=1时，才能移走介质
	 //此时USB仍然工作，只是介质没有了，不能进行读写等操作
	 if (!start) {
		 extern int close_backing_file(struct lun *curlun);
		 /* Are we allowed to unload the media? */
		 if (curlun->prevent_medium_removal) {
			 curlun->sense_data = SS_MEDIUM_REMOVAL_PREVENTED;
			 return -EINVAL;
		 }
 
		 if (loej)		 // Simulate an unload/eject
		 {
			close_backing_file(curlun);
		 }
	 }
	 else
	 {
		 if (!backing_file_is_open(curlun))
		 {
			 /*
			 ******************************
			 *不支持
			 ******************************
			 */
		 }
	 }
 
	 return 0;
 }

 /*
*********************************************************************
*                     do_request_sense
*
* Description:
*    request_sense 请求处理函数
*
* Parameters:
*    fsg  :  usb 设备驱动
*    bh   :  存放处理结果的缓冲区
*
* Return value:
*    返回request_sense命令的长度
*
* note:
*    无
*
*********************************************************************
*/
static int do_request_sense(struct msc_slave_dev *fsg, struct fsg_buffhd *bh)
{
    struct lun  *curlun = fsg->curlun;
    u8      *buf = (u8 *) bh->buf;
    u32     sd, sdinfo;

    if (!curlun)        // Unsupported LUNs are okay
    {
        fsg->bad_lun_okay = 1;
        sd = SS_LOGICAL_UNIT_NOT_SUPPORTED;
        sdinfo = 0;
    }
    else
    {
        sd = curlun->sense_data;
        sdinfo = curlun->sense_data_info;
        curlun->sense_data = SS_NO_SENSE;
        curlun->sense_data_info = 0;
    }
	//hal_log_err("sd=%x,sdinfo=%d curlun->sense_data=%x",sd,sdinfo,curlun->sense_data);

    memset(buf, 0, 18);
    buf[0] = 0x80 | 0x70;           // Valid, current error
    buf[2] = SK(sd);                //检测键
    put_be32(&buf[3], sdinfo);      // Sense information
    buf[7] = 18 - 8;                // Additional sense length
    buf[12] = ASC(sd);              //检测码
    buf[13] = ASCQ(sd);
    return 18;
}

 /*
 *********************************************************************
 *					   do_verify
 *
 * Description:
 *	  verify 请求处理函数
 *
 * Parameters:
 *	  fsg  :  usb 设备驱动
 *
 * Return value:
 *	  0  :	成功
 *	  !0 :	失败
 *
 * note:
 *	  无
 *
 * history:
 *	  以前的verify命令的响应是去读nand的每一个扇区，看扇区是否损坏。
 *	  由于USB看见的是nand的逻辑扇区，这些扇区没有坏块，所以没有必要
 * 去验证扇区的好坏，而改为验证扇区的个数是否正确。
 *
 *********************************************************************
 */
 static int do_verify(struct msc_slave_dev *fsg)
 {
	 struct lun  *curlun = fsg->curlun;
	 u32  lba;
	 u32  verification_length;
	 //--<1>--get lba
	 lba = get_be32(&fsg->cmnd[2]);
 
	 if (lba >= curlun->num_sectors)
	 {
		 curlun->sense_data = SS_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE;
		 return -EINVAL;
	 }
 
	 /* We allow DPO (Disable Page Out = don't save data in the
	  * cache) but we don't implement it. */
	 //--<2>--check
	 if ((fsg->cmnd[1] & ~0x10) != 0)
	 {
		 curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
		 return -EINVAL;
	 }
 
	 //--<3>--get length
	 verification_length = get_be16(&fsg->cmnd[7]);
 
	 if (verification_length == 0)
	 {
		 return -EIO;	 // No default reply
	 }
 
	 //--<4>--verify sectors must not more than max sectors
	 //verify的扇区个数为curlun->num_sectors，所以这里不能等于
	 if ((lba + verification_length) > curlun->num_sectors)
	 {
		 curlun->sense_data = SS_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE;
		 hal_log_err("ERR: lba=%x, len=%x", lba, verification_length);
		 hal_log_err("ERR: max=%x", curlun->num_sectors);
		 return -EINVAL;
	 }
 
	 return 0;
 }

 static int do_read(struct msc_slave_dev *fsg)
{
	struct lun  *curlun = fsg->curlun;  //当前的lun
	struct fsg_buffhd  *bh = NULL;			  //data buffer
	USB_OS_LONG_LONG	 file_offset = 0, file_offset_tmp = 0;	  //file 内的offset，读的起始地址
	USB_OS_LONG_LONG	 nread = 0;   //实际读到的数据长度
	int  rc = 0;
	u32  amount_left = 0; 	   //剩余长度
	unsigned int	amount = 0;    //每次读操作的长度
	u32  lba = 0; 			   //逻辑块地址
	u32  buff_len = (mod_data.buflen >> 9);

	//get LBA
	//根据read命令的类型，来获得相应的LBA
	//lba的高位存放在cmnd[]的低位中
	//printf("%s %d %s \n", __FILE__, __LINE__, __func__);
	if (fsg->cmnd[0] == SC_READ_6)
	{
		lba = (fsg->cmnd[1] << 16) | get_be16(&fsg->cmnd[2]);
	}
	else
	{
		lba = get_be32(&fsg->cmnd[2]);

		//cmnd[1]的4~3为分别为DPO和FUA
		if ((fsg->cmnd[1] & ~0x18) != 0)
		{
			curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
			hal_log_err("error\n");
			return -EINVAL;
		}
	}

	//超出了最大逻辑块号
	if (curlun->num_sectors && (lba >= curlun->num_sectors))
	{
		curlun->sense_data = SS_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE;
		hal_log_err("PANIC : do_read : lba too big = 0x%x > 0x%x", lba, curlun->num_sectors);
		return -EINVAL;
	}

	//read的起始地址
	file_offset = lba;
	amount_left = fsg->data_size_from_cmnd;

	if (amount_left == 0)
	{
		hal_log_err("ERR: do_read : data len from cmd = 0x%x == 0 ", amount_left);
		return -EIO;		  // No default reply
	}

	/* Carry out the file reads */
	for (;;)
	{
		//剩余长度和buff大小， 文件可提供的读的大小
		amount = USB_MIN((unsigned int) amount_left, buff_len);
		amount = USB_MIN((USB_OS_LONG_LONG) amount, curlun->num_sectors - file_offset);
		//bh可用
		bh = &fsg->buffhds[DATA_BUFFERS];
		//遇到了文件的尾部，就发送0包
		if (amount <= 0)
		{
			//媒体介质不存在的时候curlun->num_sectors=0，这里会改变
			//curlun->sense_data的状态，使得pc不断的发送检测命令
			//当pc再次发送read_capacity的时候curlun->sense_data的状态
			//回到SS_MEDIUM_NOT_PRESENT
			if (curlun->num_sectors != 0)
			{
				curlun->sense_data = SS_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE;
			}

			curlun->sense_data_info = file_offset;
			ENTER_CRITICAL(bh->bh_lock);

			if (bh->invalid)
			{
				bh->invalid = 0;
			}

			//bh->inreq->length = 0;
			bh->in_len = 0;
			bh->state = BUF_STATE_FULL;
			EXIT_CRITICAL(bh->bh_lock);
			break;
		}

		//reset 发生了，buff无效
		if (bh->invalid)
		{
			hal_log_err("ERR: read cmd: buffer invalid");
			curlun->sense_data_info = file_offset;
			curlun->sense_data = SS_RESET_DATA_PHASE_ERR;
			ENTER_CRITICAL(bh->bh_lock);
			bh->invalid		= 0;
			bh->state 		= BUF_STATE_EMPTY;
			EXIT_CRITICAL(bh->bh_lock);
			return -ECONNRESET;
		}

		//读数据
		file_offset_tmp = file_offset;
		nread = USB_OS_READ(bh->buf, file_offset_tmp, amount, curlun->filp);

		if (nread < 0)
		{
			hal_log_err("PANIC : error in file read count = %d < 0", (int) nread);
			nread = 0;
		}
		else if (nread < amount)
		{
			hal_log_err("PANIC : partial file read count = %d,real want = %d", (int) nread, amount);
		}

		file_offset  += nread;
		amount_left  -= nread;
		fsg->residue -= nread << 9;
		fsg->usb_amount_left -= nread << 9;
		//bh->inreq->length = nread << 9;
		bh->in_len = nread << 9;
		ENTER_CRITICAL(bh->bh_lock);
		bh->state = BUF_STATE_FULL;
		EXIT_CRITICAL(bh->bh_lock);

		//读到的数据少于要求的数据
		if (nread < amount)
		{
			curlun->sense_data = SS_UNRECOVERED_READ_ERROR;
			curlun->sense_data_info = file_offset;
			hal_log_err("ERR : realy read(%d) != want(%d)", (int) nread, amount);
			break;
		}

		//所有数据读完了
		if (amount_left == 0)
		{
			break;	  // No more left to read
		}

		//等待数据做完
		//bh->inreq->zero = 0;
		//start_transfer(fsg, fsg->bulk_in, bh->inreq,&bh->inreq_busy, &bh->state);
		usb_gadget_function_write(h->bulk_in, bh->buf, nread << 9);
		ENTER_CRITICAL(fsg->lock);
		bh->inreq_busy = 0;
		bh->state = BUF_STATE_EMPTY;
		EXIT_CRITICAL(fsg->lock);
		//唤醒线程，告诉其调用者，数据送完了
	}

	//printf("%s %d %s \n", __FILE__, __LINE__, __func__);
	return -EIO;		  // No default reply
}

 /*
 *********************************************************************
 *					   do_write
 *
 * Description:
 *	  write 请求处理函数
 *
 * Parameters:
 *	  fsg  :  usb 设备驱动
 *
 * Return value:
 *	  无
 *
 * note:
 *	  无
 *
 *********************************************************************
 */
 static int do_write(struct msc_slave_dev *fsg)
 {
	 struct lun *curlun = fsg->curlun;
	 struct fsg_buffhd *bh = NULL;
	 USB_OS_LONG_LONG usb_offset = 0, file_offset = 0, file_offset_tmp = 0; //写的起始地址
	 USB_OS_LONG_LONG nwritten = 0;   //实际写的长度
	 unsigned int amount = 0;		  //每次写的长度
	 int get_some_more = 0; 		  //usb接收数据标志
	 u32 amount_left_to_req = 0, amount_left_to_write = 0; //剩余的长度
	 u32 lba = 0;			 //写的起始扇区
	 int rc = 0;
	 u32  buff_len = mod_data.buflen >> 9;
 
	 if (curlun->ro)
	 {
		 curlun->sense_data = SS_WRITE_PROTECTED;
		 return -EINVAL;
	 }
 
	 //get LBA
	 if (fsg->cmnd[0] == SC_WRITE_6)
	 {
		 lba = (fsg->cmnd[1] << 16) | get_be16(&fsg->cmnd[2]);
	 }
	 else
	 {
		 lba = get_be32(&fsg->cmnd[2]);
 
		 //cmnd[1]的4~3为分别为DPO和FUA
		 if ((fsg->cmnd[1] & ~0x18) != 0)
		 {
			 curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
			 return -EINVAL;
		 }
	 }
 
	 if (curlun->num_sectors && (lba >= curlun->num_sectors))
	 {
		 curlun->sense_data = SS_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE;
		 hal_log_err("PANIC : write :lba too big = 0x%x > 0x%x", lba, curlun->num_sectors);
		 return -EINVAL;
	 }
 
	 /* Carry out the file writes */
	 get_some_more = 1;
	 file_offset = usb_offset = lba;
	 amount_left_to_write = fsg->data_size_from_cmnd;
	 amount_left_to_req = amount_left_to_write;
	 //printf("%s %d %s file_offset:%d amount_left_to_req:%d\n", __FILE__, __LINE__, __func__, file_offset, amount_left_to_req);
	 while (amount_left_to_write > 0)
	 {
		 //reset 发生了，buff无效
		 //这里是把所有的数据收完, 再去写nand。如果在准备写nand的时候去判断时间上就晚了
		 if (is_msc_buffer_invalid(fsg))
		 {
			 curlun->sense_data 	 = SS_RESET_DATA_PHASE_ERR;
			 curlun->sense_data_info = file_offset;
			 hal_log_err("-ECONNRESET");
			 return -ECONNRESET;
		 }
 
		 //接收USB数据
		 bh = &fsg->buffhds[DATA_BUFFERS];
		 //printf("%s %d %s bh->state:%d get_some_more:%d\n", __FILE__, __LINE__, __func__, bh->state, get_some_more);
		 if (bh->state == BUF_STATE_EMPTY && get_some_more)
		 {
			 amount = USB_MIN(amount_left_to_req, buff_len);
			 amount = USB_MIN((USB_OS_LONG_LONG) amount, curlun->num_sectors - usb_offset);
 
			 if (amount == 0)
			 {
				 get_some_more = 0;
				 curlun->sense_data = SS_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE;
				 curlun->sense_data_info = usb_offset;
				 continue;
			 }
 
			 //Get the next buffer
			 usb_offset 			 += amount;
			 amount_left_to_req 	 -= amount;
 
			 if (amount_left_to_req == 0)
			 {
				 get_some_more = 0;
			 }
 
			 /* amount is always divisible by 512, hence by
			  * the bulk-out maxpacket size */
			 fsg->usb_amount_left			 -= amount << 9;
			 bh->bulk_out_intended_length	 = amount << 9;
			 //bh->outreq->length 			 = amount << 9;
			 bh->out_len 			 		 = amount << 9;
			 //start_transfer(fsg, fsg->bulk_out, bh->outreq, &bh->outreq_busy, &bh->state);
			 memset(bh->buf, 0x00, bh->bulk_out_intended_length);
			 usb_gadget_function_read(h->bulk_out, bh->buf, bh->bulk_out_intended_length);
			 ENTER_CRITICAL(fsg->lock);
			 bh->outreq_busy = 0;
			 bh->state = BUF_STATE_FULL;
			 EXIT_CRITICAL(fsg->lock);
			 continue;
		 }
 
		 bh = &fsg->buffhds[DATA_BUFFERS];
		 if (bh->state == BUF_STATE_EMPTY && !get_some_more)
		 {
			 hal_log_err("ERR: do_write: want some more, but the buffer is empty");
			 break; 		 // We stopped early
		 }
 
		 if (bh->state == BUF_STATE_FULL)
		 {
			 ENTER_CRITICAL(bh->bh_lock);
			 bh->state = BUF_STATE_EMPTY;
			 EXIT_CRITICAL(bh->bh_lock);
#if 0
			 /* Did something go wrong with the transfer? */
			 if (bh->outreq->status != 0)
			 {
				 hal_log_err("ERR: write cmd: req is not valid, status = %d", bh->outreq->status);
				 curlun->sense_data = SS_COMMUNICATION_FAILURE;
				 curlun->sense_data_info = file_offset;
				 break;
			 }
#endif
			 //reset 发生了，buff无效
			 if (bh->invalid)
			 {
				 //hal_log_err("ERR: write cmd: buffer invalid, length = %d, actual = %d", bh->outreq->length, bh->outreq->actual);
				 msc_log("ERR: write cmd: buffer invalid, length = %d", bh->out_len);

				 ENTER_CRITICAL(bh->bh_lock);
				 bh->invalid = 0;
				 EXIT_CRITICAL(bh->bh_lock);
				 curlun->sense_data 	 = SS_RESET_DATA_PHASE_ERR;
				 curlun->sense_data_info = file_offset;
				 return -ECONNRESET;
			 }
#if 0
			 /* Did the host decide to stop early? */
			 if (bh->outreq->actual != bh->outreq->length)
			 {
				 fsg->short_packet_received = 1;
				 hal_log_err("PANIC : write : length = %d !, actual = %d", bh->outreq->length, bh->outreq->actual);
				 break;
			 }

			 amount = bh->outreq->actual >> 9;
#else
			 amount = bh->out_len >> 9;
#endif

			 if ((curlun->num_sectors - file_offset) < amount)
			 {
				 hal_log_err("PANIC : write 0x%x ; 0x%x beyond end 0x%x",
							amount, (unsigned long long) file_offset,
							(unsigned long long) curlun->num_sectors);
				 amount = curlun->num_sectors - file_offset;
			 }
 
			 //write to fs
			 file_offset_tmp = file_offset;
			 nwritten =  USB_OS_WRITE(bh->buf, file_offset_tmp, amount, curlun->filp);

			 if (nwritten < 0)
			 {
				 hal_log_err("PANIC : error in file write: %d < 0 ", (int) nwritten);
				 nwritten = 0;
			 }
			 else if (nwritten < amount)
			 {
				 hal_log_err("PANIC : partial file write: real = %d,want = %d", (int) nwritten, amount);
			 }
 
			 file_offset		  += nwritten;
			 amount_left_to_write -= nwritten;
			 fsg->residue		  -= nwritten << 9;
 
			 /* If an error occurred, report it and its position */
			 if (nwritten < amount)
			 {
				 curlun->sense_data = SS_WRITE_ERROR;
				 curlun->sense_data_info = file_offset;
				 hal_log_err("PANIC : write : want = %d != real = %d", amount, nwritten);
				 break;
			 }
 
			 continue;
		 }
 
		 //如果USB数据没有接收完，就等待
		 //printf("%s %d %s \n", __FILE__, __LINE__, __func__);
	 } 
	 return -EIO;		 // No default reply
 }


int do_scsi_command(struct msc_slave_dev *fsg)
{
    struct fsg_buffhd   *bh;
    int         rc;
    int         reply = -EINVAL;  //命令返回的数据长度
    int         i;
    static char     unknown[16] = "unknown cmd";
    /* Wait for the next buffer to become available for data or status */

	bh = &fsg->buffhds[DATA_BUFFERS];
    fsg->phase_error = 0;
    fsg->short_packet_received = 0;

    //响应命令请求
	//printf("fsg->cmnd[0]=0x%x\n",fsg->cmnd[0]);
    switch (fsg->cmnd[0]) {
        case SC_READ_6:
            fsg->data_size_from_cmnd = fsg->cmnd[4];  //数据长度
            if ((reply = check_command(fsg, 6, DATA_DIR_TO_HOST, (7 << 1) | (1 << 4), 1, "READ(6)")) == 0) {
                reply = do_read(fsg);
            }
            break;
        case SC_READ_10:
            fsg->data_size_from_cmnd = get_be16(&fsg->cmnd[7]);
            if ((reply = check_command(fsg, 10, DATA_DIR_TO_HOST, (1 << 1) | (0xf << 2) | (3 << 7), 1, "READ(10)")) == 0) {
                reply = do_read(fsg);
            }
            break;

        case SC_READ_12:
            fsg->data_size_from_cmnd = get_be32(&fsg->cmnd[6]);
            if ((reply = check_command(fsg, 12, DATA_DIR_TO_HOST, (1 << 1) | (0xf << 2) | (0xf << 6), 1, "READ(12)")) == 0) {
                reply = do_read(fsg);
            }
            break;

        case SC_WRITE_6:
            i = fsg->cmnd[4];
            fsg->data_size_from_cmnd = (i == 0 ? 256 : i);
            if ((reply = check_command(fsg, 6, DATA_DIR_FROM_HOST, (7 << 1) | (1 << 4), 1,  "WRITE(6)")) == 0) {
                reply = do_write(fsg);
            }
            break;

        case SC_WRITE_10:
            fsg->data_size_from_cmnd = get_be16(&fsg->cmnd[7]);
            if ((reply = check_command(fsg, 10, DATA_DIR_FROM_HOST,  (1 << 1) | (0xf << 2) | (3 << 7), 1, "WRITE(10)")) == 0) {
                reply = do_write(fsg);
            }
            break;

        case SC_WRITE_12:
            fsg->data_size_from_cmnd = get_be32(&fsg->cmnd[6]);
            if ((reply = check_command(fsg, 12, DATA_DIR_FROM_HOST, (1 << 1) | (0xf << 2) | (0xf << 6), 1, "WRITE(12)")) == 0) {
                reply = do_write(fsg);
            }
            break;

        case SC_INQUIRY:
            fsg->data_size_from_cmnd = fsg->cmnd[4];
            if ((reply = check_command(fsg, 6, DATA_DIR_TO_HOST, (1 << 4), 0, "INQUIRY")) == 0) {
                reply = do_inquiry(fsg, bh);
            } else {
                hal_log_err("PANIC : INQUIRY : not pass check_command ,ret = %d", reply);
            }

            break;

        case SC_MODE_SELECT_6:
            fsg->data_size_from_cmnd = fsg->cmnd[4];
            if ((reply = check_command(fsg, 6, DATA_DIR_FROM_HOST, (1 << 1) | (1 << 4), 0, "MODE SELECT(6)")) == 0) {
                reply = do_mode_select(fsg, bh);
            }

            break;

        case SC_MODE_SELECT_10:
            fsg->data_size_from_cmnd = get_be16(&fsg->cmnd[7]);
            if ((reply = check_command(fsg, 10, DATA_DIR_FROM_HOST, (1 << 1) | (3 << 7), 0, "MODE SELECT(10)")) == 0) {
                reply = do_mode_select(fsg, bh);
            }

            break;

        case SC_MODE_SENSE_6:
            fsg->data_size_from_cmnd = fsg->cmnd[4];
            if ((reply = check_command(fsg, 6, DATA_DIR_TO_HOST, (1 << 1) | (1 << 2) | (1 << 4), 0, "MODE SENSE(6)")) == 0) {
                reply = do_mode_sense(fsg, bh);
            } else {
                printf("PANIC : MODE_SENSE : not pass check_command ,ret = %d", reply);
            }

            break;

        case SC_MODE_SENSE_10:
            fsg->data_size_from_cmnd = get_be16(&fsg->cmnd[7]);
            if ((reply = check_command(fsg, 10, DATA_DIR_TO_HOST, (1 << 1) | (1 << 2) | (3 << 7), 0, "MODE SENSE(10)")) == 0) {
                reply = do_mode_sense(fsg, bh);
            }

            break;

        case SC_PREVENT_ALLOW_MEDIUM_REMOVAL:
            fsg->data_size_from_cmnd = 0;
            if ((reply = check_command(fsg, 6, DATA_DIR_NONE, (1 << 4), 0, "PREVENT-ALLOW MEDIUM REMOVAL")) == 0) {
                reply = do_prevent_allow(fsg);
            }

            break;

        case SC_READ_CAPACITY:
            fsg->data_size_from_cmnd = 8;
            if ((reply = check_command(fsg, 10, DATA_DIR_TO_HOST, (0xf << 2) | (1 << 8), 1, "READ CAPACITY")) == 0) {
                reply = do_read_capacity(fsg, bh);
            }

            break;

        case SC_READ_FORMAT_CAPACITIES:
            fsg->data_size_from_cmnd = get_be16(&fsg->cmnd[7]);
            if ((reply = check_command(fsg, 10, DATA_DIR_TO_HOST, (3 << 7), 1, "READ FORMAT CAPACITIES")) == 0) {
                reply = do_read_format_capacities(fsg, bh);
            } else {
                int i = 0;
                hal_log_err("PANIC : READ FORMAT CAPACITIES : not pass check_command ,ret = %d", reply);
                i = 0;

                while (i < 12)
                {
                    hal_log_err("%x    %x    %x    %x", fsg->cmnd[i + 0], fsg->cmnd[i + 1], fsg->cmnd[i + 2], fsg->cmnd[i + 3]);
                    i += 4;
                };
            }

            break;

        case SC_REQUEST_SENSE:
            fsg->data_size_from_cmnd = fsg->cmnd[4];
			//hal_log_err("fsg->data_size_from_cmnd=%d",fsg->data_size_from_cmnd);
            if ((reply = check_command(fsg, 6, DATA_DIR_TO_HOST, (1 << 4), 0, "REQUEST SENSE")) == 0) {
				//hal_log_err("1 reply=%d",reply);
				reply = do_request_sense(fsg, bh);
            }
			//hal_log_err("2 reply=%d",reply);

            break;

        case SC_START_STOP_UNIT:
            fsg->data_size_from_cmnd = 0;
            if ((reply = check_command(fsg, 6, DATA_DIR_NONE, (1 << 1) | (1 << 4), 0, "START-STOP UNIT")) == 0) {
				printf("%s %d %s \n", __FILE__, __LINE__, __func__);
                reply = do_start_stop(fsg);
            }

            break;

        case SC_SYNCHRONIZE_CACHE:
            fsg->data_size_from_cmnd = 0;
            if ((reply = check_command(fsg, 10, DATA_DIR_NONE, (0xf << 2) | (3 << 7), 1, "SYNCHRONIZE CACHE")) == 0) {
                break; //暂不支持
            }

        case SC_TEST_UNIT_READY:
            fsg->data_size_from_cmnd = 0;
            reply = check_command(fsg, 6, DATA_DIR_NONE, 0, 1, "TEST UNIT READY");
            break;

        /* Although optional, this command is used by MS-Windows.  We
         * support a minimal version: BytChk must be 0. */
        case SC_VERIFY:
            fsg->data_size_from_cmnd = 0;
            if ((reply = check_command(fsg, 10, DATA_DIR_NONE, (1 << 1) | (0xf << 2) | (3 << 7), 1, "VERIFY")) == 0) {
                reply = do_verify(fsg);
            }

            /*reply = 0;*/
            break;

        /* Some mandatory commands that we recognize but don't implement.
         * They don't mean much in this setting.  It's left as an exercise
         * for anyone interested to implement RESERVE and RELEASE in terms
         * of Posix locks. */
        case SC_FORMAT_UNIT:
        case SC_RELEASE:
        case SC_RESERVE:
        case SC_SEND_DIAGNOSTIC:

        // Fall through
        default:
            fsg->data_size_from_cmnd = 0;
     	    if ((reply = check_command(fsg, fsg->cmnd_size, DATA_DIR_UNKNOWN, 0xff, 0, unknown)) == 0)
            {
                fsg->curlun->sense_data = SS_INVALID_COMMAND;
                reply = -EINVAL;
            }

            break;
    }

    if (reply == -ECONNRESET)
    {
        printf("ERR: reset during do scsi command");
        raise_exception(fsg, FSG_STATE_HALFBUTTOM);
        return -1;
    }

    /* Set up the single reply buffer for finish_reply() */
    //判断cmd 执行的结果
    if (reply == -EINVAL)
    {
        reply = 0;    // Error reply length
    }

    if (reply >= 0 && fsg->data_dir == DATA_DIR_TO_HOST)
    {
        reply = USB_MIN((u32) reply, fsg->data_size_from_cmnd);
        ENTER_CRITICAL(bh->bh_lock);
        //bh->inreq->length = reply;
		bh->in_len = reply;
        bh->state = BUF_STATE_FULL;
        EXIT_CRITICAL(bh->bh_lock);
        fsg->residue -= reply;
    }   // Otherwise it's already set

    return 0;
}


/*
*********************************************************************
*   零包处理
*
*********************************************************************
*/
static int pad_with_zeros(struct msc_slave_dev *fsg)
{
    struct fsg_buffhd   *bh = &fsg->buffhds[DATA_BUFFERS];
    //u32 nkeep = bh->inreq->length;
    u32 nkeep = bh->in_len;
    u32 nsend;
    int rc;
    printf("info: pad_with_zeros");
    ENTER_CRITICAL(bh->bh_lock);
    bh->state = BUF_STATE_EMPTY;        // For the first iteration
    EXIT_CRITICAL(bh->bh_lock);
    fsg->usb_amount_left = nkeep + fsg->residue;

    while (fsg->usb_amount_left > 0)
    {
        while (bh->state != BUF_STATE_EMPTY)
        {
			printf("%s %d %s \n", __FILE__, __LINE__, __func__);
            if ((rc = sleep_thread(fsg)) != 0)
            {
                return rc;
            }
        }

        nsend = USB_MIN(fsg->usb_amount_left, (u32) mod_data.buflen);
        memset(bh->buf + nkeep, 0, nsend - nkeep);
        //bh->inreq->length = nsend;
        //bh->inreq->zero = 0;
        bh->in_len = nsend;
        //start_transfer(fsg, fsg->bulk_in, bh->inreq,&bh->inreq_busy, &bh->state);
		usb_gadget_function_write(h->bulk_in, bh->buf,  bh->in_len);
		ENTER_CRITICAL(fsg->lock);
		bh->inreq_busy = 0;
		bh->state = BUF_STATE_EMPTY;
		EXIT_CRITICAL(fsg->lock);
        fsg->usb_amount_left -= nsend;
        fsg->residue         -= nsend;
        nkeep                = 0;
    }

    return 0;
}

/*
*********************************************************************
*   丢弃无效的数据
*
*********************************************************************
*/
static int throw_away_data(struct msc_slave_dev *fsg)
{
    struct fsg_buffhd   *bh;
    u32         amount;
    int         rc;

    //return 0;
    while ((bh = &fsg->buffhds[DATA_BUFFERS])->state != BUF_STATE_EMPTY ||
           fsg->usb_amount_left > 0)
    {
//        printf("throw_away_data: fsg->usb_amount_left = %d, fsg->cmnd_size = %d, fsg->cbbuf_cmnd_size = %d\n", fsg->usb_amount_left, fsg->cmnd_size, fsg->cbbuf_cmnd_size);
//        printf("throw_away_data: fsg->data_size = %d, fsg->data_size_from_cmnd = %d, fsg->residue = %d\n", fsg->data_size, fsg->data_size_from_cmnd, fsg->residue);
//        printf("throw_away_data: fsg->state = %d, fsg->short_packet_received = %d\n", fsg->state, fsg->short_packet_received);

        /* Throw away the data in a filled buffer */
        if (bh->state == BUF_STATE_FULL)
        {
            ENTER_CRITICAL(bh->bh_lock);
            bh->state = BUF_STATE_EMPTY;
            EXIT_CRITICAL(bh->bh_lock);
#if 0
            /* A short packet or an error ends everything */
            if (bh->outreq->actual != bh->outreq->length || bh->outreq->status != 0)
            {
                hal_log_err("ERR: throw_away_data: req done error. actual = %d, length = %d, status = %d",
                           bh->outreq->actual, bh->outreq->length, bh->outreq->status);
                raise_exception(fsg, FSG_STATE_ABORT_BULK_OUT);
                return -EINTR;
            }
#endif
            if (fsg->usb_amount_left == 0 && fsg->residue != 0)
            {
                hal_log_err("ERR: throw_away_data: data not done. usb_amount_left = %d, residue = %d", fsg->usb_amount_left, fsg->residue);
                raise_exception(fsg, FSG_STATE_ABORT_BULK_OUT);
                return -EINTR;
            }

            continue;
        }

        /* Try to submit another request if we need one */
        bh = &fsg->buffhds[DATA_BUFFERS];

        if (bh->state == BUF_STATE_EMPTY && fsg->usb_amount_left > 0)
        {
            amount = USB_MIN(fsg->usb_amount_left, (u32) mod_data.buflen);
            /* amount is always divisible by 512, hence by
             * the bulk-out maxpacket size */
            //bh->outreq->length = bh->bulk_out_intended_length = amount;
            bh->out_len = bh->bulk_out_intended_length = amount;
//            printf("throw_away_data: length = %d", bh->out_len);
            //start_transfer(fsg, fsg->bulk_out, bh->outreq, &bh->outreq_busy, &bh->state);
			memset(bh->buf, 0x00, bh->bulk_out_intended_length);
            usb_gadget_function_read(h->bulk_out, bh->buf, bh->out_len);
			ENTER_CRITICAL(fsg->lock);
			 bh->outreq_busy = 0;
			 bh->state = BUF_STATE_FULL;
			 EXIT_CRITICAL(fsg->lock);
            fsg->usb_amount_left -= amount;
            fsg->residue         -= amount;
            continue;
        }

        /* Otherwise wait for something to happen */
        if ((rc = sleep_thread(fsg)) != 0)
        {
            return rc;
        }
    }

    return 0;
}
int get_next_command(struct msc_slave_dev *fsg)
{
    int rc = 0;
	int len = 0;
    
    //目前只支持bulk_only
    if (transport_is_bbb())
    {
	    struct fsg_buffhd *bh = &fsg->buffhds[COMMAND_BUFFERS];
		if (bh == NULL)
	    {
	        msc_log("get bh is null.\n");
	        return -EFAULT;
	    }
//		printf("%s %d %s get command\n", __FILE__, __LINE__, __func__);
		memset(bh->buf, 0x00, USB_BULK_CB_WRAP_LEN);
		//msc_log("get_next_command.\n");
    	bh->bulk_out_intended_length = usb_gadget_function_read(h->bulk_out, bh->buf, USB_BULK_CB_WRAP_LEN);
		//唤醒线程，告诉其调用者，数据接收完毕
		ENTER_CRITICAL(fsg->lock);
		bh->outreq_busy = 0;
		bh->state = BUF_STATE_FULL;
		EXIT_CRITICAL(fsg->lock);
		//msc_log("get_next_command ok.\n");

		rc = received_cbw(fsg, bh);
//		printf("%s %d %s rc:%d\n", __FILE__, __LINE__, __func__, rc);
    }
	else            // USB_PR_CB or USB_PR_CBI
    {
        printf("PANIC : CB or CBI transport now support now");
        return -EOPNOTSUPP;
    }

    //__here__
    return rc;
}


/*
**************************************************************
*                                  finish_reply
*
*描述:
*      scsi命令后处理
*
*参数:
*         fsg                  : usb 设备
*
*返回值:
*         0   : 成功
*         !0 : 失败
*
*说明
*
*
**************************************************************
*/
int finish_reply(struct msc_slave_dev *fsg)
{
    struct fsg_buffhd   *bh = &fsg->buffhds[DATA_BUFFERS];
    int  rc = 0;

	//printf("%s %d %s fsg->data_dir:%d\n", __FILE__, __LINE__, __func__, fsg->data_dir);
    switch (fsg->data_dir)
    {
        case DATA_DIR_NONE:
            break;          // Nothing to send

        /* If we don't know whether the host wants to read or write,
         * this must be CB or CBI with an unknown command.  We mustn't
         * try to send or receive any data.  So stall both bulk pipes
         * if we can and wait for a reset. */
        case DATA_DIR_UNKNOWN:

            //没有指明传输方向，而且允许stal，就halt ep
            if (mod_data.can_stall)
            {
                //fsg_set_halt(fsg, fsg->bulk_out);
                //rc = halt_bulk_in_endpoint(fsg);
            }

            break;

        /* All but the last buffer of data must have already been sent */
        //发送最后一个包
        case DATA_DIR_TO_HOST:
            if (fsg->data_size == 0)  //cbw要求传输的长度
            {
                ;       // Nothing to send
            }
            else if (fsg->residue == 0)
            {
                //最后一个包在这里发的
                /* If there's no residue, simply send the last buffer */
                //bh->inreq->zero = 0;
                //start_transfer(fsg, fsg->bulk_in, bh->inreq, &bh->inreq_busy, &bh->state);
				usb_gadget_function_write(h->bulk_in, bh->buf, bh->in_len);
				ENTER_CRITICAL(fsg->lock);
				bh->inreq_busy = 0;
				bh->state = BUF_STATE_EMPTY;
				EXIT_CRITICAL(fsg->lock);
            }
            else
            {
                /* For Bulk-only, if we're allowed to stall then send the
	                 * short packet and halt the bulk-in endpoint.  If we can't
	                 * stall, pad out the remaining data with 0's. */
                if (mod_data.can_stall)
                {
                    //bh->inreq->zero = 1;
                    //start_transfer(fsg, fsg->bulk_in, bh->inreq,&bh->inreq_busy, &bh->state);
					usb_gadget_function_write(h->bulk_in, bh->buf, bh->in_len);
					ENTER_CRITICAL(fsg->lock);
					bh->inreq_busy = 0;
					bh->state = BUF_STATE_EMPTY;
					EXIT_CRITICAL(fsg->lock);
                }
                else
                {
                    rc = pad_with_zeros(fsg);
                }
            }

            break;

        /* We have processed all we want from the data the host has sent.
         * There may still be outstanding bulk-out requests. */
        case DATA_DIR_FROM_HOST:
			//printf("%s %d %s residue:%d\n", __FILE__, __LINE__, __func__, fsg->residue);
			//printf("%s %d %s short_packet_received:%d\n", __FILE__, __LINE__, __func__, fsg->short_packet_received);
            if (fsg->residue == 0)
            {
                ;       // Nothing to receive
                /* Did the host stop sending unexpectedly early? */
            }
            else if (fsg->short_packet_received)
            {
                raise_exception(fsg, FSG_STATE_ABORT_BULK_OUT);
                rc = -EINTR;
            }
            else
            {
               /* host发送过来的数据没有全部收完, 并且ep没有stall,
                   	就把剩余数据收回来丢掉, 在csw阶段告诉host错误 */
                rc = throw_away_data(fsg);
            }

            break;
    }

    return rc;
}

