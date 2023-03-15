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
#include <sunxi_hal_usb.h>
#include <storage.h>
#include <kmsg.h>
#include <hal_log.h>
#include <hal_mem.h>
#include "scsi.h"
#include "usb_slave_msc.h"
#include "ch9.h"

#define __s16_to_cpu(x) ((unsigned short)(x))
#define le16_to_cpu(x)  (unsigned short)__s16_to_cpu((unsigned short)(x))

#define  USB_FSYS_POPEN(partname, pMode)                esFSYS_popen(partname, pMode)
#define  USB_FSYS_PCLOSE(hPart)                         esFSYS_pclose(hPart)
#define  USB_FSYS_PREAD(pdata, size, n, pPart)          esFSYS_pread(pdata, size, n, pPart)
#define  USB_FSYS_PWRITE(pdata, size, n, pPart)         esFSYS_pwrite(pdata, size, n, pPart)
#define  USB_FSYS_PIOCTRL(pPart, cmd, aux, pbuffer)     esFSYS_pioctrl(pPart, cmd, aux, pbuffer)

struct msc_slave_dev	*the_fsg = NULL;
struct mod_data_struct	mod_data;

u32  main_thread_id         = 0;  //主线程的id
u32  main_thread_exit       = 0;  //主线程结束标志
u8   is_main_thread_sleep   = 0;  //主线程是否第一次睡眠
u8   is_media_change        = 0;  //媒体介质是否发生了变化
u8   start_mount_part       = 0;  //开始挂载盘符标志
u8   IsSendPlugin           = 0;  //是否向应用程序发送了U

u32   part_thread_id = 0;            //分区管理线程的id

struct lun *curlun_back = NULL;   //备份当前lun
partition_manager_t  part_manager;  //分区管理
//u8   first_part_have_mount = 0;     //分区第一次被打开标志
reg_udisk_info_t  udisk_info;
rt_device_t msc_dev;
usb_handle *h = NULL;

int usb_gadget_msc_init(void);
int usb_gadget_msc_deinit(void);

void clear_msc_buffer_state(struct msc_slave_dev *fsg)
{
    u32 i = 0;
    struct fsg_buffhd *bh = NULL;

    //dequeue all req and clear buffer state
    for (i = 0; i < NUM_BUFFERS; ++i) {
        bh = &fsg->buffhds[i];
        //      DMSG_INFO("clear_msc_buffer_state: bh->invalid = %d, bh->state = %d", bh->invalid, bh->state);
        //      DMSG_INFO("clear_msc_buffer_state: bh->inreq_busy = %d, bh->outreq_busy = %d", bh->inreq_busy, bh->outreq_busy);
        //clear buffer
        ENTER_CRITICAL(bh->bh_lock);
        bh->invalid = 0;
        bh->state   = BUF_STATE_EMPTY;
        EXIT_CRITICAL(bh->bh_lock);
    }

    return;
}

int sleep_thread(struct msc_slave_dev *fsg)
{
    u8  ret = 0;

    esKRNL_SemPend(fsg->thread_event, 2000, &ret);
    if (ret != OS_NO_ERR) {
        printf("PANIC : sleep_thread() : ret = %d", ret);
        return -1;
    }

    return (ret ? -EINTR : 0);
}
void wakeup_thread(struct msc_slave_dev *fsg)
{
    OS_SEM_DATA sem_data;
    u8   ret = 0;
    //--<1>--获得信号量的计数

    ret = esKRNL_SemQuery(fsg->thread_event, &sem_data);
    if (ret != OS_NO_ERR) {
        hal_log_err("PANIC : wakeup_thread() : ret = %d", ret);
        return ;
    }

    //--<2>--OSCnt == 0，才能加1
    if (sem_data.OSCnt == 0) {
        esKRNL_SemPost(fsg->thread_event);
    }
}


static void _init_struct_mod_data(void)
{	
    memset(&mod_data, 0, sizeof(struct mod_data_struct));
    mod_data.transport_parm = "BBB";
    mod_data.protocol_parm  = "SCSI";
    mod_data.removable      = 0;
    mod_data.can_stall      = 1;
    mod_data.vendor         = DRIVER_VENDOR_ID;
    mod_data.product        = DRIVER_PRODUCT_ID;
    mod_data.release        = 0x2;
    mod_data.buflen         = USB_MAX_BUFF_LEN;
    mod_data.nluns          = MAX_LUNS;
}

static void _deinit_struct_mod_data(void)
{
    memset(&mod_data, 0, sizeof(struct mod_data_struct));
}

static int  check_parameters(void)
{
    /* Store the default values */
    mod_data.transport_type = USB_PR_BULK;
    mod_data.transport_name = "Bulk-only";
    mod_data.removable  = 1;
    mod_data.protocol_type = USB_SC_SCSI;
    mod_data.protocol_name = "Transparent SCSI";
    return 0;
}

void SendPlugIn2App(void)
{
    __epos_kmsg_t   *msg = NULL;

	/* msg must alloc memory by balloc function     */
    msg = (__epos_kmsg_t *)rt_malloc(sizeof(__epos_kmsg_t));
    if (!msg)
    {
        hal_log_err("malloc fail!\n");
        return;
    }

    msg->target       = KMSG_TGT_BROADCST;
    msg->message      = KMSG_VAL_DEV_PLUGIN;
    msg->prio         = KMSG_PRIO_LOW;
    msg->l.para       = KMSG_VAL_DEV_USBD;
    hal_log_info("send plugin msg to app!\n");
    esKSRV_SendMsgEx((void *)msg);
}

/* 向app发送PlugOut消息 */
void SendPlugOut2App(void)
{
    __epos_kmsg_t         *msg = NULL;

	/* msg must alloc memory by balloc function     */
    msg = (__epos_kmsg_t *)rt_malloc(sizeof(__epos_kmsg_t));
    if (!msg)
    {
        hal_log_err("malloc fail!\n");
        return;
    }

    msg->target       = KMSG_TGT_BROADCST;
    msg->message      = KMSG_VAL_DEV_PLUGOUT;
    msg->prio         = KMSG_PRIO_LOW;
    msg->l.para       = KMSG_VAL_DEV_USBD;
    hal_log_info("ERR: send plugout msg to app!\n");
    esKSRV_SendMsgEx((void *)msg);
}

static void part_wakeup_thread(partition_manager_t *part)
{
    esKRNL_SemPost(part->part_thd);
}

//限制: 只能在thread环境中使用,isr中是禁止的
static int part_sleep_thread(partition_manager_t *part)
{
    u8  ret = 0;
    esKRNL_SemPend(part->part_thd, 0, &ret);

    if (ret != OS_NO_ERR)
    {
        hal_log_err("PANIC : sleep_thread() : ret = %d", ret);
        return -1;
    }

    return (ret ? -EINTR : 0);
}

void  part_callback(u32   	state)
{
    u32  csr = 0;

	hal_log_info("state:%d\n", state);

	partition_manager_t *part = &part_manager;
    ENTER_CRITICAL(csr);
    part->usb_state = state;
    EXIT_CRITICAL(csr);

	part_wakeup_thread(part);
}

static int open_backing_file(struct lun *curlun,  char *file_name)
{
    __s32  ret = -1;
    __s32  partsize     = 0;
    __u32  sector_szie  = 0;
    //--<1>--打开nand设备文件

    hal_log_info("------------usbd open partion-----------------");

    curlun->filp = USB_FSYS_POPEN(file_name, "r+");
    if (curlun->filp == NULL) {
        hal_log_err("PANIC : open_backing_file() failed, file_name = %s", file_name);
        return -1;
    }

    hal_log_info("usbd: open partion %s seccussful, nand_mp = %p.", file_name, curlun->filp);
    //获得设备信息
    ret = USB_FSYS_PIOCTRL(curlun->filp, PART_IOC_USR_GETPARTSIZE, 0, &partsize);
    if (ret != EPDK_OK) {
        hal_log_err("PANIC : get %s partsize failed ", file_name);
		USB_FSYS_PCLOSE(curlun->filp);
		curlun->filp = NULL;
        return -1;
    }

    ret = USB_FSYS_PIOCTRL(curlun->filp, PART_IOC_USR_GETSCTSIZE, 0, &sector_szie);
    if (ret != EPDK_OK) {
        hal_log_err("PANIC : get %s sector_size failed ", file_name);
		USB_FSYS_PCLOSE(curlun->filp);
		curlun->filp = NULL;
        return -1;
    }

    /*SW_BKPT();*/
    hal_log_info("usbd: open device %s succeed, partsize=%x, sector_szie = %x", file_name, partsize, sector_szie);
    //获得设备文件句柄、设备大小
    if (sector_szie == 0) {
        curlun->sector_size = 512;
    } else {
        curlun->sector_size = sector_szie;
    }
	printf("%s %d %s file_name:%s curlun->filp:0x%p\n", __FILE__, __LINE__, __func__, file_name, curlun->filp);
	memset(curlun->name, 0x00, sizeof(curlun->name));
	strcpy(curlun->name, file_name);
    curlun->num_sectors = (u32)partsize;
    curlun->file_length = curlun->num_sectors * curlun->sector_size;
    curlun->registered  = 1;
    curlun->ro          = 0;   //默认可读可写
    return 0;
}

int close_backing_file(struct lun *curlun)
{
    if (curlun != NULL && curlun->filp != NULL) {
		int ret = 0;

		printf("%s %d %s curlun->name:%s\n", __FILE__, __LINE__, __func__, curlun->name);
        ret = USB_FSYS_PCLOSE((__hdle)(curlun->filp));
        if (ret != EPDK_OK) {
            hal_log_err("usbd close part failed");
			printf("%s %d %s\n", __FILE__, __LINE__, __func__);
            return -1;
        }

        msc_log("--------usbd close partion successful------");
        //清除lun的参数
        curlun->filp        = NULL;
        curlun->num_sectors = 0;
        curlun->sector_size = 0;
        curlun->file_length = 0;
        curlun->registered  = 0;
        curlun->ro          = 1;
		curlun->sense_data = SS_MEDIUM_NOT_PRESENT;
		printf("%s %d %s\n", __FILE__, __LINE__, __func__);
        return 0;
    } else if (curlun == NULL) {
		hal_log_err("usbd close part failed, because curlun == NULL ");
	} else {
	    hal_log_err("usbd close part failed, curlun->name:%s because file == NULL", curlun->name);
	}
	
    return -1;
}
static s32  save_not_mount_part(partition_manager_t *part, char *name)
{
	unsigned char index = 0;
	for (index = 0; index < MAX_LUNS; index++) {
		if (part->partition[index].used == 0) {
			memcpy(part->partition[index].name, name, strlen(name));
			part->partition[index].lun_number = index;
			part->partition[index].used = 1;
			part->partition[index].is_open = 0;
			break;
		}
	}
    if (index >= MAX_LUNS) {
        hal_log_err("ERR: save_not_mount_part name=\"%s\" failed", name);
        return -1;
    }
}

int32_t blockdev_unregister_callback(int32_t u_arg, int32_t s_arg) {
	if (the_fsg != NULL) {
		unsigned char index = 0;
		unsigned char name[12] = {0};
	    struct lun   *curlun = NULL;
		sprintf(name, "%c:\\", u_arg);
		printk("%s %d %s name:%s\n", __FILE__, __LINE__, __func__, name);
		for (index = 0; index < MAX_LUNS; index++) {
			printk("%s %d %s u_arg:%s\n", __FILE__, __LINE__, __func__, part_manager.partition[index].name);
			if (part_manager.partition[index].used == 1 && strncmp(part_manager.partition[index].name, name, strlen(name)) == 0) {
	            curlun = &the_fsg->luns[part_manager.partition[index].lun_number];
	            close_backing_file(curlun);
				memset(part_manager.partition[index].name, 0x00, strlen(part_manager.partition[index].name));
				part_manager.partition[index].used = 0;
				break;
			}
		}
	}
}
int32_t blockdev_register_callback(int32_t u_arg, int32_t s_arg) {
	if (the_fsg != NULL) {
		unsigned char name[12] = {0};
		
		sprintf(name, "%c:\\", u_arg);
		save_not_mount_part(&part_manager, name);			
		start_mount_part = 1;
		part_callback(USB_PART_MOUNT);
	}
}

/*
*********************************************************************
*
*
* Description:
*
*
* Parameters:
*
*
*
* Return value:
*
*
* note:
*
*
*********************************************************************
*/


static s32  free_mount_part_by_name(partition_manager_t *part, char *name)
{
	int ret = 0;
	unsigned char index = 0;
    struct lun   *curlun = NULL;
	
	for (index = 0; index < MAX_LUNS; index++) {
		printf("%s %d %s name:%s\n", __FILE__, __LINE__, __func__, name);
		if (part->partition[index].used == 1 && strcmp(part->partition[index].name, name) == 0) {
			printf("%s %d %s name:%s\n", __FILE__, __LINE__, __func__, name);
			printf("%s %d %s name:%s\n", __FILE__, __LINE__, __func__, part->partition[index].name);
            curlun = &the_fsg->luns[part->partition[index].lun_number];
            ret = close_backing_file(curlun);
			memset(part->partition[index].name, 0x00, strlen(part->partition[index].name));
			part->partition[index].used = 0;
			break;
		}
	}
    if (index >= MAX_LUNS)
    {
        hal_log_err("ERR: save_not_mount_part name=\"%s\" failed", name);
        return -1;
    }
}
static s32  free_all_mount_part(partition_manager_t *part)
{
	int ret = 0;
	unsigned char index = 0;
    struct lun   *curlun = NULL;
	
	printf("%s %d %s\n", __FILE__, __LINE__, __func__);
	for (index = 0; index < MAX_LUNS; index++) {
		if (part->partition[index].used == 1) {
            curlun = &the_fsg->luns[part->partition[index].lun_number];
			printf("%s %d %s\n", __FILE__, __LINE__, __func__);
            ret = close_backing_file(curlun);
			memset(&part->partition[index], 0x00, sizeof(back_partition_t));
			break;
		}
	}
}

s32 is_partition_valid(partition_manager_t *part, s32 type)
{
    return part->partition[type].used;
}

static s32  usbd_mount_part(partition_manager_t *part, struct msc_slave_dev  *fsg)
{
    struct lun   *curlun = NULL;
    int          ret = 0, i = 0;

    //--<1>--挂载分区
	printf("%s %d %s\n", __FILE__, __LINE__, __func__);
    for (i = 0; i < MAX_LUNS; i++) {
		printf("%s %d %s i:%d\n", __FILE__, __LINE__, __func__, i);
		printf("%s %d %s name:%s\n", __FILE__, __LINE__, __func__, part->partition[i].name);
		printf("%s %d %s lun_number:%d\n", __FILE__, __LINE__, __func__, part->partition[i].lun_number);
		printf("%s %d %s is_open:%d\n", __FILE__, __LINE__, __func__, part->partition[i].is_open);
		printf("%s %d %s used:%d\n", __FILE__, __LINE__, __func__, part->partition[i].used);
        if (part->partition[i].used == 1 && part->partition[i].is_open == 0) {
            curlun = &fsg->luns[part->partition[i].lun_number];
            ret = open_backing_file(curlun, part->partition[i].name);
            if (ret == 0) {
                is_media_change = 1;
				curlun_back = curlun;
                part->partition[i].is_open = 1;
            }
            else
            {
                hal_log_err("ERR: partiton=\"%s\" lun_number=%d open fail", part->partition[i].name, part->partition[i].lun_number);
                continue;
            }
        }
    }
    return 0;
}

void part_manager_thd(void)
{
    struct msc_slave_dev *fsg = the_fsg;
    partition_manager_t *part = &part_manager;
    s32 ret = 0;

    while (1) {
        //--<1>--杀死线程
        if (esKRNL_TaskDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ) {
            printf("%s %d %s: usbd: part thread exit", __FILE__, __LINE__, __func__);
            esKRNL_TDel(OS_PRIO_SELF);
        }

        //--<3>--
	//printf("%s %d %s: part->usb_state = 0x%x\n", __FILE__, __LINE__, __func__, part->usb_state);
        switch (part->usb_state) {
            case USB_PART_IDLE:
                part_sleep_thread(part);
                break;

            case USB_PART_RESET:
                //--<3-1>--向monitor发reset消息
                hal_log_err("WRN: send reset msg to usb_monitor");

                //--<3-3>--置usb_state的状态
                ENTER_CRITICAL(fsg->lock);
                part->usb_state = USB_PART_IDLE;
                part->is_close = 0;
                EXIT_CRITICAL(fsg->lock);
				if (!IsSendPlugin) {
					SendPlugIn2App();
					IsSendPlugin = 1;
				}
                //--<3-4>--线程睡眠
                part_sleep_thread(part);
                break;

            case USB_PART_SUSPEND:
                //--<3-2>--向monitor发消息
                hal_log_err("WRN: send suspend msg to usb_monitor");

                ENTER_CRITICAL(fsg->lock);
                part->usb_state = USB_PART_IDLE;
                part->is_close = 0;
                EXIT_CRITICAL(fsg->lock);
				hal_log_err("part_sleep_thread");
				if (IsSendPlugin) {
					SendPlugOut2App();
					IsSendPlugin = 0;
				}
                part_sleep_thread(part);
				hal_log_err("part_sleep_thread ok");
                break;

            case USB_PART_EXIT:
                //退出线程
                break;

            case USB_PART_MOUNT:
                if (start_mount_part) {
                    ENTER_CRITICAL(fsg->lock);
                    usbd_mount_part(part, fsg);
                    part->usb_state = USB_PART_IDLE;
                    EXIT_CRITICAL(fsg->lock);
                } else {
                    hal_log_info("not allow start_mount_part");
                    part->usb_state = USB_PART_IDLE;
                }

                break;

            default:
                hal_log_err("part->usb_state=%x is invalid", part->usb_state);
                part->usb_state = USB_PART_IDLE;
        }
    }
}
static int inline exception_in_progress(struct msc_slave_dev *fsg)
{
    return (fsg->state > FSG_STATE_IDLE);
}

static void _set_fsg_state(struct msc_slave_dev *fsg, enum fsg_state state)
{
    ENTER_CRITICAL(fsg->lock);
    if (!exception_in_progress(fsg)) {
        fsg->state = state;
    }
    EXIT_CRITICAL(fsg->lock);
}
void raise_exception(struct msc_slave_dev *fsg, enum fsg_state new_state)
{
    u32 lock = 0;
    //高级exception会覆盖低级的

	printf("%s %d %s new_state:%d\n", __FILE__, __LINE__, __func__, new_state);
	ENTER_CRITICAL(lock);
    if (fsg->state <= new_state) {
        fsg->exception_req_tag = fsg->ep0_req_tag;
        fsg->state = new_state;
        fsg->exception_happen_flag = 1; //flag the exception
    }
    EXIT_CRITICAL(lock);
}

/*
*********************************************************************
*                     send_status
*
* Description:
*    发送状态
*
* Parameters:
*    fsg  :  usb 设备驱动
*
* Return value:
*    0   : 成功
*    !0  : 失败
*
* note:
*    无
*
*********************************************************************
*/
static int send_status(struct msc_slave_dev *fsg)
{
    struct lun      *curlun = fsg->curlun;
    struct fsg_buffhd   *bh = &fsg->buffhds[COMMAND_BUFFERS];
    int  rc;
    u8   status = USB_STATUS_PASS;
    u32  sd;    //sense data

	/* 使用buffer之前没有必要检测invalid, 及时出现invalid sense_data也可以把这次出错汇报给主机 */
    //sense data
//	printf("%s %d %s start\n", __FILE__, __LINE__, __func__);
    if (curlun) {
        sd = curlun->sense_data;
    } else if (fsg->bad_lun_okay) {
        sd = SS_NO_SENSE;
    } else {
        sd = SS_LOGICAL_UNIT_NOT_SUPPORTED;
    }

    //根据dev 的状态和sense data ，来判断命令的执行结果
    if (fsg->phase_error) {
        status = USB_STATUS_PHASE_ERROR;
        sd = SS_INVALID_COMMAND;
    } else if (sd != SS_NO_SENSE) {
        status = USB_STATUS_FAIL;
    }

    //如果bulk_only,  bulk 命令状态封包处理
    //printf("%s %d %s status:%d\n", __FILE__, __LINE__, __func__, status);
    //printf("%s %d %s sd:%d\n", __FILE__, __LINE__, __func__, sd);
    if (transport_is_bbb()) {
        struct bulk_cs_wrap *csw = (struct bulk_cs_wrap *) bh->buf;
        /* Store and send the Bulk-only CSW */
		memset(bh->buf, 0x00, USB_BULK_CB_WRAP_LEN);
        csw->Signature = (USB_BULK_CS_SIG);
        csw->Tag = fsg->tag;
        csw->Residue = (fsg->residue);
        csw->Status = status;
		bh->in_len = USB_BULK_CS_WRAP_LEN;
		usb_gadget_function_write(h->bulk_in, bh->buf, bh->in_len);
		ENTER_CRITICAL(fsg->lock);
		bh->inreq_busy = 0;
		bh->state = BUF_STATE_EMPTY;
		EXIT_CRITICAL(fsg->lock);
	}
    else
    {
        hal_log_err("send status : transport not support now!");
    }
//	printf("%s %d %s end\n", __FILE__, __LINE__, __func__);
    return 0;
}

/*
*********************************************************************
*                     handle_exception
*
* Description:
*    driver状态处理
*
* Parameters:
*    fsg_      :  usb 设备驱动
*    new_state :  msc 的状态
*
* Return value:
*    无
*
* note:
*    无
*
*********************************************************************
*/
static void handle_exception(struct msc_slave_dev *fsg)
{
    int i;
    struct fsg_buffhd *bh;
    enum fsg_state old_state;
    unsigned int exception_req_tag;
    int rc;
    msc_log("INFO: exception happen, state = %x", fsg->state);

    //清除例外发生标志
    if (fsg->exception_happen_flag != 0) {
        ENTER_CRITICAL(fsg->lock);
        fsg->exception_happen_flag = 0;
        EXIT_CRITICAL(fsg->lock);
    } else {
        hal_log_err("PANIC : enter exception_handle,but exception_happen_flag == 0");
    }
#if 0
    /* Cancel all the pending transfers */
    for (i = 0; i < NUM_BUFFERS; ++i) {
        bh = &fsg->buffhds[i];
    }

    //Clear out the controller's fifos
    if (fsg->bulk_in_enabled) {
        usb_ep_fifo_flush(fsg->bulk_in);
    }

    if (fsg->bulk_out_enabled) {
        usb_ep_fifo_flush(fsg->bulk_out);
    }
#endif
    /* Reset the I/O buffer states and pointers, the SCSI
     * state, and the exception.  Then invoke the handler. */
    ENTER_CRITICAL(fsg->lock);

    /* Reset the I/O buffer states and pointers */
    for (i = 0; i < NUM_BUFFERS; ++i) {
        fsg->buffhds[i].state = BUF_STATE_EMPTY;
    }

    exception_req_tag = fsg->exception_req_tag;
    old_state = fsg->state;

    if (old_state == FSG_STATE_ABORT_BULK_OUT) {
        fsg->state = FSG_STATE_STATUS_PHASE;
    } else {
        for (i = 0; i < fsg->nluns; ++i) {
            fsg->luns[i].prevent_medium_removal  = 0;
            fsg->luns[i].sense_data              = SS_NO_SENSE;
            fsg->luns[i].unit_attention_data     = SS_NO_SENSE;
            fsg->luns[i].sense_data_info         = 0;
        }
        fsg->state = FSG_STATE_IDLE;
    }

    EXIT_CRITICAL(fsg->lock);
	hal_log_info("old_state = %x", old_state);

    /* Carry out any extra actions required for the exception */
    switch (old_state) {
        default:
            hal_log_err("ERR: unkown fsg state(%d1)", old_state);
            break;

        case FSG_STATE_HALFBUTTOM:
            hal_log_err("ERR: something wrong is happen, must halfbuttom dispose");
            //清除所有buffer的状态
            clear_msc_buffer_state(fsg);
            ENTER_CRITICAL(fsg->lock);
            fsg->state = FSG_STATE_IDLE;
            EXIT_CRITICAL(fsg->lock);
            break;

        case FSG_STATE_ABORT_BULK_OUT:
            //如果产生了异常bulk_out 中断，就向主机报告状态
			printf("%s %d %s \n", __FILE__, __LINE__, __func__);
            send_status(fsg);
            ENTER_CRITICAL(fsg->lock);

            if (fsg->state == FSG_STATE_STATUS_PHASE) {
                fsg->state = FSG_STATE_IDLE;
            }

            EXIT_CRITICAL(fsg->lock);
            break;

        case FSG_STATE_RESET:

            //发生reset，就清fsg->atomic_bitflags 的BULK_HALTS，并且ep set_halt
            /* In case we were forced against our will to halt a
             * bulk endpoint, clear the halt now.  (The SuperH UDC
             * requires this.) */
            if (usb_atomic_test_and_clear_bit(MSC_ATOMAIC_BITFLAGS_CLEAR_BULK_HALTS,
                                              &fsg->atomic_bitflags)) {
                //usb_ep_clear_halt(fsg->bulk_in);
                //usb_ep_clear_halt(fsg->bulk_out);
            }

            if (transport_is_bbb()) {
                if (fsg->ep0_req_tag == exception_req_tag) {
                    //ep0_queue(fsg); // Complete the status stage
                }
            }

            break;

        case FSG_STATE_INTERFACE_CHANGE:
            //rc = do_set_interface(fsg, 0);

            if (fsg->ep0_req_tag != exception_req_tag) {
                break;
            }

            if (rc != 0)    //STALL on errors
            {
                //fsg_set_halt(fsg, fsg->ep0);
            }
            else            //Complete the status stage
            {
                //ep0_queue(fsg);
            }

            break;

        case FSG_STATE_CONFIG_CHANGE:
            break;

        case FSG_STATE_DISCONNECT:
            //do_set_config(fsg, 0);      // Unconfigured state
            break;

        case FSG_STATE_EXIT:
        case FSG_STATE_TERMINATED:
            //do_set_config(fsg, 0);          // Free resources
            ENTER_CRITICAL(fsg->lock);
            fsg->state = FSG_STATE_TERMINATED;  // Stop the thread
            EXIT_CRITICAL(fsg->lock);
            break;
    }

    return;
}

static rt_err_t sunxi_msc_dev_open(rt_device_t dev, rt_uint16_t oflag) 
{
	hal_log_info("here\n");
    return 0;
}
static rt_err_t sunxi_msc_dev_close(rt_device_t dev) 
{
	hal_log_info("here\n");
    return 0;
}
static rt_size_t sunxi_msc_dev_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size) 
{
	hal_log_info("here\n");
    return 0;
}
static rt_size_t sunxi_msc_dev_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
	hal_log_info("here\n");
    return 0;
}
static rt_err_t sunxi_msc_dev_control(rt_device_t dev, int cmd, void *args)
{
	int ret = -EINVAL;

	switch (cmd) {
		case USB_SLAVE_MSC_PART_PLUGIN:  
			save_not_mount_part(&part_manager, args);			
			start_mount_part = 1;			
			part_callback(USB_PART_MOUNT);
		break;
		case USB_SLAVE_MSC_PART_PLUGOUT:
			free_mount_part_by_name(&part_manager, args);
			break;
		default:
			hal_log_err("cmd:%x not support\n", cmd);
			break;
	}
    return 0;
}

void set_config_call(u16 w_value)
{
	struct msc_slave_dev *fsg = the_fsg;
	
	fsg->new_config = w_value;
    fsg->state = FSG_STATE_CONFIG_CHANGE;

    //主线程刚创建的时候去唤醒
    if (is_main_thread_sleep) {
        wakeup_thread(fsg);   //对应msc_main_thread中的sleep_thread
    }
	
}

void msc_main_thread(void *fsg_)
{
	struct msc_slave_dev *fsg = (struct msc_slave_dev *) fsg_;
    u8 ret = 0;
    int ret_cmd = 0;
	
	esKRNL_SemPend(fsg->thread_notifier, 0, &ret);
	if (!fsg->running) {
        is_main_thread_sleep = 1;
		esKRNL_SemPend(fsg->thread_event, 0, &ret);
        is_main_thread_sleep = 0;
    }
	
	if (!IsSendPlugin) {
		SendPlugIn2App();
		IsSendPlugin = 1;
	}

	while (fsg->state != FSG_STATE_TERMINATED) {
		if (main_thread_exit == 1) {
            hal_log_err("main thread exit");
            fsg->state = FSG_STATE_TERMINATED;
            break;
        }
//		if (fsg->exception_happen_flag != 0)
//        {
//            handle_exception(fsg);
//            continue;
//        }
		//hal_log_err("get_next_command");
		ret_cmd = get_next_command(fsg);
		if (ret_cmd == -1) {
            hal_log_err("ERR: get cbw time out %d !", ret_cmd);
            part_callback(USB_PART_SUSPEND);
        } else if (ret_cmd) {
            hal_log_err("ERR: get cbw failed %d", ret_cmd);
            esKRNL_TimeDly(1);
            continue;
        }
		//hal_log_err("_set_fsg_state");
		//--<2-4>--数据阶段
		_set_fsg_state(fsg, FSG_STATE_DATA_PHASE);
        if (do_scsi_command(fsg) || finish_reply(fsg)) {
            hal_log_err("ERR: do scsi command failed");
            esKRNL_TimeDly(100);
            clear_msc_buffer_state(fsg);
            main_thread_exit = 1;
            continue;
        }
        //--<2-5>--状态阶段
        _set_fsg_state(fsg, FSG_STATE_STATUS_PHASE);
//		printf("%s %d %s \n", __FILE__, __LINE__, __func__);
        if (send_status(fsg)) {
            hal_log_err("ERR: send csw failed");
            clear_msc_buffer_state(fsg);
            continue;
        }

        //--<2-6>--数据阶段和状态阶段处理完了，变为空闲状态
        _set_fsg_state(fsg, FSG_STATE_IDLE);
		//hal_log_err("_set_fsg_state ok");
	}
    //--<4>--通知msc_cleanup
    esKRNL_SemPost(fsg->thread_notifier);

    //--<5>--杀死线程
    while (1) {
        if (esKRNL_TaskDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ) {
            hal_log_info("usbd: %s exit", __func__);
            esKRNL_TDel(OS_PRIO_SELF);
        }
        //给线程一点时间删除
        esKRNL_TimeDly(1);
    }
}

static int _adjust_bytes_2_npage_1k(int bytes)
{
    return (bytes + (1024 - 1)) >> 10;
}


static void *usb_alloc_buffer(unsigned bytes)
{
    void *retval = 0;
    int npage_1k = 0;

    if (bytes == 0) {
        hal_log_err("PANIC : usbd_alloc_buffer() input == NULL");
        return NULL;
    }
    //调整
    npage_1k = _adjust_bytes_2_npage_1k(bytes);
    retval = (void *)esMEMS_Palloc(npage_1k, 0);

    if (retval == NULL) {
        hal_log_err("PANIC : usbd_alloc_buffer() malloc fail");
        return NULL;
    }

    return retval;
}


/*
*******************************************************************************
*                     usbd_alloc_buffer
*
* Description:
*    释放申请的存储空间
*
* Parameters:
*    ep  :  目标ep
*    buf ： 存储空间首地址
*    dma :  dma地址
*    bytes : 存储空间的长度
*
* Return value:
*    无
*
* note:
*    无
*
*******************************************************************************
*/
static void usbd_free_buffer(void *buf, u32 bytes)
{
    if (bytes == 0) {
        hal_log_err("PANIC : usbd_free_buffer() input == NULL");
        return ;
    }
    //调整
    esMEMS_Pfree(buf, _adjust_bytes_2_npage_1k(bytes));
    buf = NULL;
}

static int  fsg_alloc(void)
{
	int index = 0;
    __s32 ret = 0;
    unsigned char err = 0;
    struct msc_slave_dev *fsg = NULL;

    fsg = msc_malloc(sizeof(struct msc_slave_dev));
    if (!fsg) {
        printf("PANIC : malloc msc_slave_dev fail");
        ret = -ENOMEM;
        goto ERR;
    }
    memset(fsg, 0, sizeof(struct msc_slave_dev));

	fsg->thread_event = esKRNL_SemCreate(0);
    if (fsg->thread_event == NULL) {
        printf("PANIC : USB_OS_SemCreate fail");
        ret = -ENOMEM;
        goto ERR;
    }

	fsg->thread_notifier = esKRNL_SemCreate(0);
    if (fsg->thread_notifier == NULL) {
        printf("PANIC : USB_OS_SemCreate fail");
        ret = -ENOMEM;
        goto ERR;
    }

	//--<3>--为luns 分配内存资源
    fsg->nluns = USB_MAX(mod_data.nluns, 1);
    fsg->luns = malloc(fsg->nluns * sizeof(struct lun));
    if (!fsg->luns) {
        ret = -ENOMEM;
        goto ERR;
    }
    memset(fsg->luns, 0, fsg->nluns * sizeof(struct lun));

    for (index = 0; index < fsg->nluns; ++index) {
        fsg->luns[index].sense_data = SS_MEDIUM_NOT_PRESENT;
    }

    the_fsg                 = fsg;
    main_thread_id          = 0;
    part_thread_id          = 0;
    main_thread_exit        = 0;
    is_main_thread_sleep    = 0;
    IsSendPlugin            = 0;
    start_mount_part        = 0;
    is_media_change         = 0;
	curlun_back				= NULL;
    memset(&part_manager, 0, sizeof(part_manager));

    ENTER_CRITICAL(fsg->lock);
    part_manager.usb_state = USB_PART_IDLE;
    EXIT_CRITICAL(fsg->lock);

	part_manager.part_thd = esKRNL_SemCreate(0);
    if (part_manager.part_thd == NULL)
    {
        printf("PANIC : USB_OS_SemCreate fail");
        ret = -ENOMEM;
        goto ERR;
    }
    return 0;

ERR:
    if (fsg) {
		if (fsg->luns) {
			free(fsg->luns);
			fsg->luns = NULL;
		}
		
        if (fsg->thread_event) {
            esKRNL_SemDel(fsg->thread_event, OS_DEL_ALWAYS, &err);
            fsg->thread_event = NULL;
        }

        if (fsg->thread_notifier) {
            esKRNL_SemDel(fsg->thread_notifier, OS_DEL_ALWAYS, &err);
            fsg->thread_notifier = NULL;
        }
        msc_free(fsg);
        fsg = NULL;
    }
    return ret;
}

/*
*******************************************************
* msc资源释放
*
*******************************************************
*/
static void fsg_free(struct msc_slave_dev *fsg)
{
    unsigned char err = 0;

    if (fsg->luns)
     {
        free(fsg->luns);
        fsg->luns = NULL;
    }

    if (fsg->thread_event)
    {
        esKRNL_SemDel(fsg->thread_event, OS_DEL_ALWAYS, &err);
        fsg->thread_event = NULL;
    } 

    if (fsg->thread_notifier)
    {
        esKRNL_SemDel(fsg->thread_notifier, OS_DEL_ALWAYS, &err);
        fsg->thread_notifier = NULL;
    } 	
    msc_free(fsg);
    the_fsg = NULL;
}
static void close_all_backing_files(struct msc_slave_dev *fsg)
{
    int i;

    for (i = 0; i < fsg->nluns; ++i) {
        if (fsg->luns[i].filp != NULL) {
            close_backing_file(&fsg->luns[i]);
        }
    }

    return ;
}

static s32  msc_cleanup(void)
{
    struct msc_slave_dev    *fsg = the_fsg;
    u8  ret = 0;
	struct fsg_buffhd *bh;
	int  i;

	//--<1>--结束工作线程
	if (fsg->state != FSG_STATE_TERMINATED) {

		ENTER_CRITICAL(fsg->lock);
		main_thread_exit = 1;
		//fsg->state = FSG_STATE_TERMINATED;
		EXIT_CRITICAL(fsg->lock);

		//--<1-1>--主线程在等控制传输而睡眠
		if (is_main_thread_sleep) {
			wakeup_thread(fsg);
		}

		//--<1-3>--杀死主线程		
		while (esKRNL_TaskDelReq(main_thread_id) != OS_TASK_NOT_EXIST) {
			esKRNL_TimeDlyResume(main_thread_id);
			esKRNL_TimeDly(30);
		}
	}
	//--<1-4>--杀死分区管理线程
	part_callback(USB_PART_EXIT);

	while (esKRNL_TaskDelReq(part_thread_id) != OS_TASK_NOT_EXIST) {
		esKRNL_TimeDly(1);
	}
	esKRNL_SemDel(part_manager.part_thd, OS_DEL_ALWAYS, &ret);

	//--<3>--释放非ep0的data buffers
	for (i = 0; i < NUM_BUFFERS; ++i) {
		struct fsg_buffhd *bh = &the_fsg->buffhds[i];
		if (bh->buf != NULL) {
			if (i == COMMAND_BUFFERS) {
				usbd_free_buffer(bh->buf, USB_BULK_CB_WRAP_LEN);
			} else {
				usbd_free_buffer(bh->buf, mod_data.buflen);
			}
			bh->buf = NULL;
		}
	}

    // Wait for the thread to finish up
    esKRNL_SemPend(fsg->thread_notifier, 0, &ret);
    close_all_backing_files(fsg);
	memset(&part_manager, 0x00, sizeof(partition_manager_t));
	msc_log(" ");
    return -1;
}

int usb_msc_init(void)
{
	struct msc_slave_dev  *fsg = NULL;

	int ret;
	struct usb_request	*req;
	int 		i;

	h = msc_calloc(1, sizeof(usb_handle));
	if (!h) {
		printf("no memory");
		return -1;
	}

	h->control = -1;
	h->bulk_out = -1;
	h->bulk_in = -1;

	h->bulk_out = USB_MSC_OUT;
	h->bulk_in = USB_MSC_IN;

	ret = usb_gadget_msc_init();
	if (ret < 0) {
		printf("usb_gadget_msc_init failed");
		goto error;
	}
	ret = usb_gadget_function_enable("msc");
	if (ret < 0) {
		printf("enable msc function failed");
		goto error1;
	}

	_init_struct_mod_data();
	ret = fsg_alloc();
	if (ret != 0) {
		printf("fsg_alloc failed");
		goto fsg_error;
	}
	fsg = the_fsg;

	//--<1>--传递外部参数
	check_parameters();
	
	msc_dev = rt_device_create(RT_Device_Class_USBDevice, 0);
	if (!msc_dev)
	{
		hal_log_err("rt_device_create fail\n");
		goto fsg_error;
	}
	//mkdevice
	msc_dev->open = sunxi_msc_dev_open;
	msc_dev->close = sunxi_msc_dev_close;
	msc_dev->read = sunxi_msc_dev_read;
	msc_dev->write = sunxi_msc_dev_write;
	msc_dev->control = sunxi_msc_dev_control;
	ret = rt_device_register(msc_dev, "msc", RT_DEVICE_FLAG_RDWR);
	if (ret != RT_EOK) {
		printf("%s %d %s rt_device_register fail ret:%d\n", __FILE__, __LINE__, __func__, ret);
		goto error_msc_dev;
	}

    //--<2>-- 获得lun 的个数	
	for (i = 0; i < NUM_BUFFERS; ++i) {
        struct fsg_buffhd *bh = &the_fsg->buffhds[i];
		if (i == COMMAND_BUFFERS) {
			bh->buf = usb_alloc_buffer(USB_BULK_CB_WRAP_LEN);
		} else {
			bh->buf = usb_alloc_buffer(mod_data.buflen);
		}
		
        if (!bh->buf) {
            hal_log_err("PANIC : malloc buff fail");
            goto out;
        }
    }
	main_thread_id = esKRNL_TCreate(msc_main_thread, the_fsg, 0x4000, 0x03);
	if (main_thread_id == OS_NO_ERR)
    {
        hal_log_err("PANIC : can not create main thread,err code = %d", main_thread_id);
        ret = main_thread_id;
        goto out;
    }

	part_thread_id = esKRNL_TCreate(part_manager_thd, NULL, 0x2000, 0x03);
	if (part_thread_id == OS_NO_ERR)
	{
		msc_log("PANIC : can not create part thread,err code = %d", part_thread_id);
		ret = part_thread_id;
		goto part_manager_err;
	}

    esKRNL_SemPost(fsg->thread_notifier);	
	return ret;
part_manager_err:
	
out:
	for (i = 0; i < NUM_BUFFERS; ++i) {
		struct fsg_buffhd *bh = &the_fsg->buffhds[i];
		if (bh->buf != NULL) {
			if (i == COMMAND_BUFFERS) {
				usbd_free_buffer(bh->buf, USB_BULK_CB_WRAP_LEN);
			} else {
				usbd_free_buffer(bh->buf, mod_data.buflen);
			}
		}
	}
error_msc_dev:
	rt_device_unregister(msc_dev);
	rt_device_destroy(msc_dev);
	msc_dev =NULL;
fsg_error:
	fsg_free(fsg);
	_deinit_struct_mod_data();
	usb_gadget_function_disable("msc");
error1:
	usb_gadget_msc_deinit();
error:
	if(h != NULL)
	{
		msc_free(h);
		h = NULL;
	}

	return ret;
}

int usb_msc_exit(void) {
	msc_log("usb_msc_exit");

	if (IsSendPlugin) {
		SendPlugOut2App();
		IsSendPlugin = 0;
	}
	msc_cleanup();

	fsg_free(the_fsg);
	
	if (msc_dev) {
		rt_device_unregister(msc_dev);
		rt_device_destroy(msc_dev);
		msc_dev =NULL;
	}

	usb_gadget_function_disable("msc");
	usb_gadget_msc_deinit();
	if (h != NULL) {
		msc_free(h);
		h = NULL;
	}

	return 0;
}
#define  USB_BULK_RESET_REQUEST         0xff
#define  USB_BULK_GET_MAX_LUN_REQUEST           0xfe

int msc_class_req(struct usb_ctrlrequest *crq)
{
	static char temp = 0;
	int value = -EOPNOTSUPP;
    u16 w_index = le16_to_cpu(crq->wIndex);
    u16 w_length = le16_to_cpu(crq->wLength);

	printf("msc_class_req bRequest:%x, wValue:%d, bRequestType=%d\n",  crq->bRequest, crq->wValue,crq->bRequestType);	
	if (transport_is_bbb()) {
		switch (crq->bRequest) {
			//复位
			case USB_BULK_RESET_REQUEST:
				if (crq->bRequestType != (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE)) {
					break;
				}

				if (crq->wIndex != 0) {
					value = -EDOM;
					break;
				}

				/* Raise an exception to stop the current operation
				* and reinitialize our state. */
//				raise_exception(fsg, FSG_STATE_RESET);
//				value = DELAYED_STATUS;
				break;
			//获得最大逻辑单元号
			case USB_BULK_GET_MAX_LUN_REQUEST:
				if (crq->bRequestType != (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE)) {
					break;
				}

				if (crq->wIndex != 0) {
					value = -EDOM;
					break;
				}

				temp = the_fsg->nluns - 1;
				hal_log_err("temp:%d\n",  temp);
				hal_udc_ep_set_buf(0, &temp, 1);
				value = 1;
				break;
		}
	}
	if (value == -EOPNOTSUPP) {
		hal_log_err("PANIC : unknown class-specific control req " "%02x.%02x v%04x i%04x l%u", crq->bRequestType, crq->bRequest,
				   le16_to_cpu(crq->wValue), w_index, w_length);
	}
	return value;
}

