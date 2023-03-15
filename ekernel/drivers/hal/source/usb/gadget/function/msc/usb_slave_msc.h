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
#ifndef __MSC_SLAVE_MSC_H
#define __MSC_SLAVE_MSC_H

#include <kapi.h>
#include <string.h>
#include <libc.h>
#include <log.h>


#define USB_MSC_EP0 (0)
#define USB_MSC_IN  (1)
#define USB_MSC_OUT (2)
#define MAX_COMMAND_SIZE        16       // Length of a SCSI Command Data Block
#define  DRIVER_VENDOR_ID           0x1F3A  /* 厂商ID */
#define  DRIVER_PRODUCT_ID          0x1000  /* 产品ID */

#define  DRIVER_MANUFACTURER        "USB Developer"         /* 厂商信息     */
#define  DRIVER_PRODUCT             "Mass Storage Device"   /* 产品信息     */
#define  DRIVER_VERSION             "20080411"              /* 产品序列号   */

//msc所处状态
enum fsg_state
{
    FSG_STATE_COMMAND_PHASE = -10,      // This one isn't used anywhere
    FSG_STATE_DATA_PHASE,
    FSG_STATE_STATUS_PHASE,

    FSG_STATE_IDLE = 0,
    FSG_STATE_ABORT_BULK_OUT,
    FSG_STATE_RESET,
    FSG_STATE_INTERFACE_CHANGE,
    FSG_STATE_CONFIG_CHANGE,
    FSG_STATE_DISCONNECT,
    FSG_STATE_EXIT,
    FSG_STATE_TERMINATED,
    FSG_STATE_HALFBUTTOM
};
enum data_direction
{
	DATA_DIR_UNKNOWN = 0,
	DATA_DIR_FROM_HOST,
	DATA_DIR_TO_HOST,
	DATA_DIR_NONE
};
#define    USB_PART_IDLE            0x00
#define    USB_PART_SET_CONFIG      0x01
#define    USB_PART_SUSPEND         0x02
#define    USB_PART_EXIT            0x03
#define    USB_PART_MOUNT           0x04
#define    USB_PART_RESET           0x05

struct mod_data_struct
{
    int num_filenames;
    int num_ros;
    unsigned int nluns;

    int removable;
    int can_stall;

    char *transport_parm;
    char *protocol_parm;
    unsigned short  vendor;
    unsigned short  product;
    unsigned short  release;
    unsigned int    buflen;

    int  transport_type;
    char *transport_name;
    int  protocol_type;
    char *protocol_name;
};
extern struct mod_data_struct  mod_data ;

struct usb_request
{
    void *buf;                      //buff的虚拟地址
    u32  length;                    //期望长度  //buff的长度
    u32  dma;                       //buff的物理地址
    u32  no_use_dma;                        //是否使用dma

    u8 no_interrupt;
    u8 zero;
    u8 short_not_ok;

    //usb_request_complete complete;  //req执行完毕后的callback

    void *context;              //记录了buff的属主, 即fsg_buffhd
    s32 status;                 //req执行的结果
    u32 actual;                 //实际已经传输的

    struct list_head *wrapper;  //req在list中的属主
    void         *iso_header;
};


enum fsg_buffer_type
{
    COMMAND_BUFFERS = 0,
    DATA_BUFFERS,
    NUM_BUFFERS
};

enum fsg_buffer_state
{
    BUF_STATE_EMPTY = 0,   //buffer空闲
    BUF_STATE_FULL,        //buffer已经被填充，等待处理
    BUF_STATE_BUSY         //buffer正在被填充
};

struct fsg_buffhd
{
    u32 bh_lock;                            //prorotect the struct
    u8  *buf;                               //缓冲区首地址
    volatile enum fsg_buffer_state  state;  //buff所处状态
    u32  bulk_out_intended_length;
    int in_len;
	int out_len;
    volatile int  inreq_busy;               //请求的状态
    volatile int  outreq_busy;
    volatile int  invalid;
};

enum
{
	USB_SLAVE_MSC_PART_PLUGIN = 0,
	USB_SLAVE_MSC_PART_PLUGOUT,
} ;
//-------------------------------------------------------
//
//    USB 默认 设备配置信息
//
//-------------------------------------------------------
#define  USB_DEVICE_VENDOR_INFORMATION      "USB 2.0"
#define  USB_DEVICE_PRODUCT_INFORMATION     "USB Flash Driver"
#define  USB_DEVICE_PRODUCT_REVISION        "1.00"

#define transport_is_bbb()  (mod_data.transport_type == USB_PR_BULK)
#define transport_is_cbi()  (mod_data.transport_type == USB_PR_CBI)
#define protocol_is_scsi()  (mod_data.protocol_type == USB_SC_SCSI)
//usb_msc的基本信息
#define USB_MAX_BUFF_LEN     	(16 * 1024) //16k
#define USB_BULK_CB_WRAP_LEN    (31)
#define USB_BULK_CB_SIG         (0x43425355)  // Spells out USBC
#define USB_BULK_IN_FLAG        (0x80)

struct lun
{
	__hdle filp;					//文件节点，其实就是一个lun
	char  name[8];

	u32 sector_size;
	long long num_sectors;		//本lun的扇区数
	long long file_length;		//文件的长度

	unsigned char ro ;			//readonly标志, 0: 可读可写，1:只读
	unsigned char prevent_medium_removal ;
	unsigned char registered ;

	u32 sense_data; 			//检测键
	u32 sense_data_info;		//检测码
	u32 unit_attention_data;	//lun 被修复
};

#define MAX_LUNS      				(CONFIG_DRIVERS_USB_GADGET_MSC_MAX_LUNS)
#define MAX_PARTITION_NAME_LEN    	(8)

#define MSC_ATOMAIC_BITFLAGS_REGISTERED         0
#define MSC_ATOMAIC_BITFLAGS_CLEAR_BULK_HALTS   1
#define MSC_ATOMAIC_BITFLAGS_SUSPENDED          2

typedef struct  back_partition
{
    char  name[MAX_PARTITION_NAME_LEN];  //分区名
    s32   lun_number;                          //lun号
    u32   is_open;                       //分区是否已经打开
    u32   used;                          //当前back_partition是否被占用
} back_partition_t;

typedef struct  partition_manager
{
    u32   usb_state;
    u32   is_close;                 //是否关闭了所有分区
    u32   current_part_num;         //当前已经存在的分区数    
    __krnl_event_t *part_thd;
    back_partition_t  partition[MAX_LUNS];      //分区数组
} partition_manager_t;

struct msc_slave_dev
{
    u32 lock;       //prorotect the struct

    //struct usb_gadget *gadget;

    //struct usb_ep *ep0;
    //struct usb_request *ep0req;         //ep0请求
    volatile unsigned int ep0_req_tag;  //ctrl cmd 累计
    const char *ep0req_name;            //ep0的字符串名

    unsigned int  bulk_out_maxpacket;   //最大输出包
    enum fsg_state  state;              //msc_slave_dev的状态
    unsigned int exception_req_tag;     //外来请求标志

    u8 exception_happen_flag;           //exception发生的flash != 0表示发生
    u8 config, new_config;              //设备请求命令信息, wValue

    u8 running;                         //设备运行标志
    u8 bulk_in_enabled;                 //bulk_in 使能标志
    u8 bulk_out_enabled;                //bulk_out 使能标志

    u8 phase_error;                     //状态阶段时调用。其用途是告诉host此次cmd执行失败
    u8 short_packet_received;           //短包接收标志
    u8 bad_lun_okay;

    unsigned int  atomic_bitflags;

    struct fsg_buffhd  *next_buffhd_to_drain; //备份data buffer 的数据，供再使用
    struct fsg_buffhd  buffhds[NUM_BUFFERS];  //data buffer

    __krnl_event_t *thread_event;       //只是用来控制thread的sleep/wakeup
    __krnl_event_t *thread_notifier;    //用于thread和insmod/rmmod间的同步

    int cmnd_size;                 //命令长度
    u8  cmnd[MAX_COMMAND_SIZE];    //scsi命令
    enum data_direction data_dir;
    u32  data_size;                 //cbw中的数据长度
    u32  data_size_from_cmnd;       //scsi 命令请求的数据长度
    u32  tag;
    unsigned int    lun;
    u32  residue;                   //读/写 介质的剩余长度
    u32  usb_amount_left;           //读/写 fifo的剩余长度

    int  cbbuf_cmnd_size;
    u8   cbbuf_cmnd[MAX_COMMAND_SIZE];

    unsigned int    nluns;        //lun数目
    struct lun      *luns;        //指向lun结构array
    struct lun      *curlun;
};

void  part_callback(u32    	  state);


typedef struct tag_reg_udisk_info
{
    /* USB接口配置 */
    __u16 VID;                          /* 厂商ID           */
    __u32 PID;                          /* 产品ID           */
    __u8  Manufacturer[32];             /* 厂商信息         */
    __u8  Product[32];                  /* 产品信息         */
    __u8  Serial[32];                   /* 产品序列号       */

    /* USB设备配置 */
    __u8  VendorInformation[32];        /* u盘厂商ID        */
    __u8  ProductIdentification[32];    /* u盘厂品ID        */
    __u8  ProductRevision[32];          /* u盘产品版本号    */

    /* 其它 */
    __u32 mode;                         /* 配置模式         */
    __u8  udisk_serial[32];             /* 修正后的序列号   */
} reg_udisk_info_t;
extern reg_udisk_info_t  udisk_info;

//reg_udisk_info_t  udisk_info;


#ifndef unlikely
#define unlikely(x)             __builtin_expect ((x), 0)
#endif

struct usb_handle {
	/*int (*write)(usb_handle *h, const void *data, int len);*/
	/*int (*read)(usb_handle *h, void *data, int len);*/
	int control;
	int bulk_out;	/* "out" from the host's perspective => source for adbd */
	int bulk_in;	/* "in" from the host's perspective => sink for adbd */
};

typedef struct usb_handle usb_handle;
int usb_msc_init(void);
int usb_msc_exit(void);
//#define MEM_DEBUG
#if 0
void *malloc_wrapper(size_t size);
void *calloc_wrapper(size_t nmemb, size_t size);
void *realloc_wrapper(void *ptr, size_t size);
char *strdup_wrapper(const char *s);
void free_wrapper(void *ptr);
#define msc_malloc(size)                malloc_wrapper(size)
#define msc_calloc(num, size)               calloc_wrapper(num, size)
#define msc_realloc(ptr, size)              realloc_wrapper(ptr, size)
#define msc_strdup(s)                   strdup_wrapper(s)
//#define adb_scandir(dirp,namelist,filter,compar)  scandir_wrapper(dirp,namelist,filter,compar)
#define msc_free(ptr)                   free_wrapper(ptr)
void memleak_print(void);
#else
#define msc_malloc(size)                malloc(size)
#define msc_calloc(num, size)               calloc(num, size)
#define msc_realloc(ptr, size)              realloc(ptr, size)
#define msc_strdup(s)                   strdup(s)
//#define adb_scandir(dirp,namelist,filter,compar)  scandir(dirp,namelist,filter,compar)
#define msc_free(ptr)                   free(ptr)
#endif

//#define ADB_THREAD_HIGH_PRIORITY    (20)
//#define ADB_THREAD_NORMAL_PRIORITY  (21)




#define USB_OS_LONG_LONG        int
#if 0
/* 字符串操作 */
#define  USB_OS_STRLEN(str)                             eLIBs_strlen(str)
#define  USB_OS_STRCPY(pdest, psrc)                     eLIBs_strcpy(pdest, psrc)
#define  USB_OS_STRNCPY(pdest, psrc, len_max)           eLIBs_strncpy(pdest, psrc, len_max)
#define  USB_OS_STRCAT(pdest, pstr_cat)                 eLIBs_strcat(pdest, pstr_cat)
#define  USB_OS_STRNCAT(pdest, pstr_cat, len_max)       eLIBs_strncat(pdest, pstr_cat, len_max)
#define  USB_OS_STRCMP(p1_str, p2_str)                  eLIBs_strcmp(p1_str, p2_str)
#define  USB_OS_STRNCMP(p1_str, p2_str, len_max)        eLIBs_strncmp(p1_str, p2_str, len_max)
#define  USB_OS_STRCHR(pstr, srch_char)                 eLIBs_strchr(pstr, srch_char)
#define  USB_OS_STRNCHR(pstr, len_max, srch_char)       eLIBs_strnchr(pstr, len_max, srch_char)
#define  USB_OS_STRCHRLAST(pstr, srch_char)             eLIBs_strchrlast(pstr, srch_char)
#define  USB_OS_STRSTR(pstr, srch_str)                  eLIBs_strstr(pstr, srch_str)

#define  USB_OS_eLIBs_int2str_dec(input, str)           eLIBs_int2str_dec(input, str)
#define  USB_OS_eLIBs_int2str_hex(input, str, hex_flag) eLIBs_int2str_hex(input, str, hex_flag)
#define  USB_OS_eLIBs_uint2str_dec(input, str)          eLIBs_uint2str_dec(input, str)
#define  USB_OS_eLIBs_ramdom(max)                       eLIBs_ramdom(max)

#define  USB_OS_ENTER_CRITICAL(sr)                      ENTER_CRITICAL(sr)
#define  USB_OS_CPU_SR_Restore(sr)                      EXIT_CRITICAL(sr)

//==mem===================
//普通分配
#define  USB_OS_MALLOC(size)                            (esMEMS_Malloc(0 , size))
#define  USB_OS_FREE(addr )                             (esMEMS_Mfree(0 ,(void *)addr ))

//物理连续,npage为多少个1k page
#define  USB_OS_PAGE_MALLOC(npage)                      (esMEMS_Palloc(npage,0))
#define  USB_OS_PAGE_FREE(addr , npage)                 (esMEMS_Pfree((void *)addr , (__u32)npage))

#define  USB_OS_MEMSET(addr , value , size)             eLIBs_memset(addr , value , size)
//#define  USB_OS_MEMCPY(dest , source , count)           eLIBs_memcpy(dest , source ,count)

#define  USB_OS_SEMI_DEL_NO_PEND                        OS_DEL_NO_PEND
#define  USB_OS_SEMI_DEL_ALWAYS                         OS_DEL_ALWAYS
#define  USB_OS_KERNEL_EVENT                            __krnl_event_t
#define  USB_OS_KERNEL_SOFT_TIMER                       __krnl_stmr_t

#define  USB_OS_SemCreate(x)                            esKRNL_SemCreate(x)
#define  USB_OS_SemDel(x,opt,err)                       esKRNL_SemDel(x,opt,err)
#define  USB_OS_SemPend(x,y,z)                          esKRNL_SemPend(x,y,z)
#define  USB_OS_SemPost(x)                              esKRNL_SemPost(x)
#define  USB_OS_SemQuery                                esKRNL_SemQuery

//进程管理
#define  USB_OS_THREAD_CREATE(thread_proc , proc_para , stack_size , id_priolevel)   esKRNL_TCreate(thread_proc , proc_para , stack_size , id_priolevel)
#define  USB_OS_THREAD_DELETE(p)                        esKRNL_TDel(p)

#define  USB_OS_esKRNL_TaskDelReq(p)                    esKRNL_TaskDelReq(p)
#define  USB_OS_esEXEC_PDel(p)                          esEXEC_PDel(p)
#define  USB_OS_esKRNL_TimeDly(time)                    esKRNL_TimeDly(time)

//===================
//irq,   return 全为u32
#define  USB_OS_ISR_INSTALL(irq_nr_u8 ,p_isr , p_arg)   esINT_InsISR(irq_nr_u8, p_isr, p_arg)
#define  USB_OS_ISR_UNINSTALL(irq_nr_u8 ,p_isr )        esINT_UniISR(irq_nr_u8, p_isr)
#define  USB_OS_ISR_ENABLE_INT(irq_nr_u8)               esINT_EnableINT(irq_nr_u8)
#define  USB_OS_ISR_DISABLE_INT(irq_nr_u8)              esINT_DisableINT(irq_nr_u8)

//cache
#define  USB_OS_CLEAN_DCACHE(addr, len)                 eLIBs_CleanDCacheRegion(addr, len)
#define  USB_OS_FLUSH_DCACHE(addr, len)                 eLIBs_FlushDCacheRegion(addr, len)

//dma
#define  USB_OS_DMA_Request(user, type)                 esDMA_Request(user, type)
#define  USB_OS_DMA_Release(dma)                        esDMA_Release(dma)
#define  USB_OS_DMA_Setting(dma, pArg)                  esDMA_Setting(dma, pArg)
#define  USB_OS_DMA_Start(dma, saddr, daddr, len)       esDMA_Start(dma, saddr, daddr, len)
#define  USB_OS_DMA_Stop(dma)                           esDMA_Stop(dma)
#define  USB_OS_DMA_Restart(dma)                        esDMA_Restart(dma)
#define  USB_OS_DMA_QueryStat(dma)                      esDMA_QueryStat(dma)
#define  USB_OS_DMA_QuerySrc(dma)                       esDMA_QuerySrc(dma)
#define  USB_OS_DMA_QueryDst(dma)                       esDMA_QueryDst(dma)
#define  USB_OS_DMA_EnableINT(dma, type)                esDMA_EnableINT(dma, type)
#define  USB_OS_DMA_DisableINT(dma, type)               esDMA_DisableINT(dma, type)
#define  USB_OS_DMA_QueryRestCount(dma)                 esDMA_QueryRestCount(dma)
#define  USB_OS_DMA_ChangeMode(dma, mode)               esDMA_ChangeMode(dma, mode)
#define  USB_OS_DMA_RegDmaHdler(dma, type, hdler, arg)  esDMA_RegDmaHdler(dma, type, hdler, arg)
#define  USB_OS_DMA_UnregDmaHdler(dma, type, hdler)     esDMA_UnregDmaHdler(dma, type, hdler)
#define  USB_OS_DMA_Information(void)                   esDMA_Information(void)

#define  USB_OS_VA2PA(addr)                             esMEMS_VA2PA(addr)

//file system
#define  USB_FSYS_FOPEN(filename, pMode)                esFSYS_fopen(filename, pMode)
#define  USB_FSYS_FCLOSE(hFile)                         esFSYS_fclose(hFile)
#define  USB_FSYS_FREAD(pdata, size, n, hFile)          esFSYS_fread(pdata, size, n, hFile)
#define  USB_FSYS_FWRITE(pdata, size, n, hFile)         esFSYS_fwrite(pdata, size, n, hFile)

//分区操作
#define  USB_STRORAGE_DEVICE_INFO                       __dev_blkinfo_t
#define  USB_FSYS_POPEN(partname, pMode)                esFSYS_popen(partname, pMode)
#define  USB_FSYS_PCLOSE(hPart)                         esFSYS_pclose(hPart)
#define  USB_FSYS_PREAD(pdata, size, n, pPart)          esFSYS_pread(pdata, size, n, pPart)
#define  USB_FSYS_PWRITE(pdata, size, n, pPart)         esFSYS_pwrite(pdata, size, n, pPart)
#define  USB_FSYS_PIOCTRL(pPart, cmd, aux, pbuffer)     esFSYS_pioctrl(pPart, cmd, aux, pbuffer)

//时钟管理
#define  USB_esCLK_Open(mclkno)                         esCLK_OpenMclk(mclkno)
#define  USB_esCLK_Close(hMclk)                         esCLK_CloseMclk(hMclk)
#define  USB_esCLK_OnOff(hMclk, onoff)                  esCLK_MclkOnOff(hMclk, onoff)


//注册一个设备
#define  USB_OS_DEV_DEVOPS                                  __dev_devop_t
#define  USB_OS_DEV_REG(node_class, name, dev_ops, p_arg)   esDEV_DevReg(node_class, name, dev_ops, p_arg)
#define  USB_OS_DEV_UNREG(dev_handle)                       esDEV_DevUnreg(dev_handle)

#define  USB_OS_EPDK_SUCCESS            0   //EPDK_SUCCESS
#define  USB_OS_NO_ERR                  OS_NO_ERR
#endif


#define USB_MIN(x,y)    ((x) < (y) ? (x) : (y))
#define USB_MAX(x,y)    ((x > (y)) ? (x) : (y))

static  int  inline usb_atomic_read32(int *addr)
{
    int sr = 0 ;
    int ret = 0;
    ENTER_CRITICAL(sr);
    ret = *addr;
    EXIT_CRITICAL(sr);
    return ret;
}
void clear_msc_buffer_state(struct msc_slave_dev *fsg);

/*
***********************************************
* 将值写入内存
*
***********************************************
*/
static  void  inline usb_atomic_write32(int *addr, int value)
{
    int sr = 0 ;
    ENTER_CRITICAL(sr);
    *addr = value;
    EXIT_CRITICAL(sr);
}

/*
***********************************************
* 将操作数的某位置1
*
***********************************************
*/
static  void  inline usb_atomic_set_bit(int bit_nr, volatile u32 *addr)
{
    int sr = 0 ;
    int mask;
    mask = 1 << (bit_nr & 0x1f);
    ENTER_CRITICAL(sr);
    (*addr) |= mask;
    EXIT_CRITICAL(sr);
}

/*
***********************************************
* 将操作数的某位清零
*
***********************************************
*/
static  void  inline usb_atomic_clear_bit(int bit_nr, volatile u32 *addr)
{
    int sr = 0 ;
    int mask;
    mask = 1 << (bit_nr & 0x1f);
    ENTER_CRITICAL(sr);
    *addr &= ~mask;
    EXIT_CRITICAL(sr);
}

/*
***********************************************
* 将操作数的某位清零
*
***********************************************
*/
static int  inline  usb_atomic_test_and_clear_bit(int bit_nr, volatile u32 *addr)
{
    int mask, retval;
    int sr = 0 ;
    mask = 1 << (bit_nr & 0x1f);
    ENTER_CRITICAL(sr);
    retval = ((mask & *addr) != 0);
    *addr &= ~mask;
    EXIT_CRITICAL(sr);
    return retval;
}

extern struct lun *curlun_back;
extern u8   is_media_change;  //媒体介质是否发生了变化
extern int sleep_thread(struct msc_slave_dev *fsg);
extern void wakeup_thread(struct msc_slave_dev *fsg);

extern void raise_exception(struct msc_slave_dev *fsg, enum fsg_state new_state);

extern usb_handle *h;
#define msc_log(fmt, args...)  \
    printf("[%s] line:%d " fmt "\n", __func__, __LINE__, ##args)

#define DMSG_PANIC printf
#endif /* __MSC_H */
