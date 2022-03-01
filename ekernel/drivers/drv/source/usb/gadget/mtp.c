#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <kmsg.h>
#include <kapi.h>
#include <hal_sem.h>
#include <hal_mem.h>
#include <hal_thread.h>
#include "gadget.h"
#include <usb/sunxi_hal_udc.h>

#define     ECANCELED       125
typedef struct mtp_file_range
{
    /* file descriptor for file to transfer */
    __hdle    fp;
    /* offset in file for start of transfer */
    long long offset;
    /* number of bytes to transfer */
    int64_t     length;
    /* MTP command ID for data header,
     * used only for MTP_SEND_FILE_WITH_HEADER
     */
    uint16_t    command;
    /* MTP transaction ID for data header,
     * used only for MTP_SEND_FILE_WITH_HEADER
     */
    uint32_t    transaction_id;
}mtp_file_range_t;

struct mtp_event
{
    /* size of the event */
    size_t      length;
    /* event data to send */
    void        *data;
};
typedef enum __USBD_MTP_CMD_SET           // 升级命令列表(IOCtrol)
{
    MTP_SEND_FILE = 0x01,
    MTP_RECEIVE_FILE,
    MTP_SEND_EVENT,
	MTP_SEND_FILE_WITH_HEADER,
	MTP_EXIT
} __usbd_mtp_cmd_set_t;

#define STATE_OFFLINE               0   /* initial state, disconnected */
#define STATE_READY                 1   /* ready for userspace calls */
#define STATE_BUSY                  2   /* processing userspace calls */
#define STATE_CANCELED              3   /* transaction canceled by host */
#define STATE_ERROR                 4   /* error from completion routine */

#define MTP_RESPONSE_DEVICE_BUSY                                0x2019
#define MTP_RESPONSE_OK                                         0x2001

#define MTP_OS_STRING_ID   0xEE
#define MTP_REQ_CANCEL     0x64
#define MTP_REQ_GET_DEVICE_STATUS   0x67

#define __constant_cpu_to_le16(x) (( unsigned short)(x))
#define __constant_cpu_to_le32(x) (( unsigned int)(x))
#define __s16_to_cpu(x) ((unsigned short)(x))
#define le16_to_cpu(x)  (unsigned short)__s16_to_cpu((unsigned short)(x))
#define __cpu_to_le16(x) ((unsigned short)(x))
typedef     unsigned int        le32;
#define __cpu_to_le32(x) (( le32)(x))

#define mtp_min(a, b)  ((a) < (b) ? (a) : (b))
#define mtp_max(a,b)   ((a) < (b) ? (b) : (a))

typedef struct _mtp_dev_
{
	int state;
	rt_device_t dev;

	hal_sem_t recv_thread_sem;
	hal_sem_t send_thread_sem;	
	hal_sem_t wait_sem;
	__hdle xfer_file;
	long long xfer_file_offset;
	int64_t xfer_file_length;	
	unsigned xfer_send_header;
	uint16_t xfer_command;
	uint32_t xfer_transaction_id;
	int xfer_result;
} mtp_dev_t;
static const uint16_t g_str_lang_id[] =
{
    0x0304, 0x0409
};

static const uint16_t g_str_manufacturer[] =
{
	0x0314, 'A', 'l', 'l', 'w', 'i', 'n', 'n', 'e', 'r'
};

static const uint16_t g_str_product[] =
{
	0x030c, 'M', 'e', 'l', 'i', 's'
};

static const uint16_t g_str_serialnumber[] =
{
	0x0312, '2', '0', '0', '8', '0', '4', '1', '1'
};

static const uint16_t g_str_config[] =
{
    0x0308, 'M', 't', 'p'
};

static const uint16_t g_str_interface[] =
{
    0x0308, 'M', 't', 'p'
};
/* Microsoft MTP OS String */
static unsigned char mtp_os_string[] = {
	18, /* sizeof(mtp_os_string) */
	USB_DT_STRING,
	/* Signature field: "MSFT100" */
	'M', 0, 'S', 0, 'F', 0, 'T', 0, '1', 0, '0', 0, '0', 0,
	/* vendor code */
	1,
	/* padding */
	0
};

static const uint16_t *mtp_string_desc[USB_GADGET_MAX_IDX] =
{
    g_str_manufacturer,
    g_str_product,
    g_str_serialnumber,    
	g_str_lang_id,
    g_str_config,
    g_str_interface,
};
static struct usb_device_descriptor mtp_device_desc =
{
    .bLength = USB_DT_DEVICE_SIZE,
    .bDescriptorType = USB_DT_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x0,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = 64,
    .idVendor = 0x2717,
    .idProduct = 0xFF40,
    .bcdDevice = 0x0404,
    .iManufacturer = USB_GADGET_MANUFACTURER_IDX,
    .iProduct = USB_GADGET_PRODUCT_IDX,
    .iSerialNumber = USB_GADGET_SERIAL_IDX,
    .bNumConfigurations = 1
};

static struct usb_config_descriptor mtp_config_desc =
{
    .bLength = USB_DT_CONFIG_SIZE,
    .bDescriptorType = USB_DT_CONFIG,
    .wTotalLength = USB_DT_CONFIG_SIZE +
    USB_DT_INTERFACE_SIZE +
    USB_DT_ENDPOINT_SIZE +
    USB_DT_ENDPOINT_SIZE +
    USB_DT_ENDPOINT_SIZE,
    .bNumInterfaces = 1,
    .bConfigurationValue = 1,
    .iConfiguration = USB_GADGET_CONFIG_IDX,
    .bmAttributes = 0xc0,
    .bMaxPower = 0xfa /* 500mA */
};

static struct usb_interface_descriptor mtp_intf_desc =
{
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = USB_DT_INTERFACE,
    .bInterfaceNumber = 0x0,
    .bAlternateSetting = 0x0,
    .bNumEndpoints = 3,
    .bInterfaceClass = 0xff, /* Interface class */
    .bInterfaceSubClass = 0xff, /* Subclass */
    .bInterfaceProtocol = 0x0,
    .iInterface = USB_GADGET_INTERFACE_IDX
};

static struct usb_endpoint_descriptor mtp_highspeed_in_desc =
{
    .bLength                = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType        = USB_DT_ENDPOINT,
    .bEndpointAddress       = USB_DIR_IN | 0x01,
    .bmAttributes           = USB_ENDPOINT_XFER_BULK,
    .wMaxPacketSize         = 0x200,
};

static struct usb_endpoint_descriptor mtp_highspeed_out_desc =
{
    .bLength                = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType        = USB_DT_ENDPOINT,
    .bEndpointAddress       = USB_DIR_OUT | 0x02,
    .bmAttributes           = USB_ENDPOINT_XFER_BULK,
    .wMaxPacketSize         = 0x200,
};

static struct usb_endpoint_descriptor mtp_intr_desc =
{
    .bLength                = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType        = USB_DT_ENDPOINT,
    .bEndpointAddress       = USB_DIR_IN | 0x03,
    .bmAttributes           = USB_ENDPOINT_XFER_INT,
    .wMaxPacketSize         = 0x1c,
    .bInterval              = 0x06,
};
struct mtp_ext_config_desc_header {
	unsigned int	dwLength;
	unsigned short	bcdVersion;
	unsigned short	wIndex;
	unsigned char	bCount;
	unsigned char	reserved[7];
};

/* Microsoft Extended Configuration Descriptor Function Section */
struct mtp_ext_config_desc_function {
	unsigned char	bFirstInterfaceNumber;
	unsigned char	bInterfaceCount;
	unsigned char	compatibleID[8];
	unsigned char	subCompatibleID[8];
	unsigned char	reserved[6];
};

/* MTP Extended Configuration Descriptor */
struct {
	struct mtp_ext_config_desc_header	header;
	struct mtp_ext_config_desc_function    function;
} mtp_ext_config_desc = {
	.header = {
		.dwLength = __constant_cpu_to_le32(sizeof(mtp_ext_config_desc)),
		.bcdVersion = __constant_cpu_to_le16(0x0100),
		.wIndex = __constant_cpu_to_le16(4),
		.bCount = __constant_cpu_to_le16(1),
	},
	.function = {
		.bFirstInterfaceNumber = 0,
		.bInterfaceCount = 1,
		.compatibleID = { 'M', 'T', 'P' },
	},
};
struct mtp_device_status {
	unsigned short	wLength;
	unsigned short	wCode;
};
struct mtp_data_header {
	unsigned int	length; /* length of packet, including this header */
	unsigned short	type; /* container type (2 for data packet) */
	unsigned short	command;/* MTP command code */
	unsigned int	transaction_id;/* MTP transaction ID */
};
int mtp_exit = 0;
static mtp_dev_t mtp_dev;
static rt_thread_t recv_task = NULL;
static rt_thread_t send_task = NULL;
#define MTP_BULK_BUFFER_SIZE       16384
#define INTR_BUFFER_SIZE           28
#define MTP_MAX_BUFF_SIZE        	(512*1024)

static int mtp_send_event(mtp_dev_t *dev, struct mtp_event *event)
{
	int ret;
	int length = event->length;

//	printf("mtp_send_event(%zu)\n", event->length);
	if (length < 0 || length > INTR_BUFFER_SIZE)
		return -EINVAL;
	if (dev->state == STATE_OFFLINE)
		return -ENODEV;
	ret = usb_gadget_function_write(3, event->data, length);
	return ret;
}

static void receive_file_work(void *pArg)
{
    while (1)
    {
    	int val = 0, ret = 0;
		unsigned char *data_buff = NULL;
    	unsigned int recv_len = 0, write_count = 0, write_len = 0, offset = 0;
		
		hal_sem_wait(mtp_dev.recv_thread_sem);

		offset = mtp_dev.xfer_file_offset;
		data_buff = hal_malloc(mtp_min(mtp_dev.xfer_file_length, MTP_MAX_BUFF_SIZE));

		while (write_count < mtp_dev.xfer_file_length) {	
			gadget_debug("recv_len:%d xfer_file_length:%d\n", recv_len, mtp_dev.xfer_file_length);
			if (mtp_dev.state == STATE_CANCELED) {
				/* report cancelation to userspace */
				mtp_dev.state = STATE_READY;
				ret = -ECANCELED;
				goto next;
			}
			memset(data_buff, 0x00, mtp_min(mtp_dev.xfer_file_length, MTP_MAX_BUFF_SIZE));
			if (mtp_exit) {
				gadget_err("mtp_exit \n");
				break;
			}	
			usb_gadget_function_read(2, data_buff, mtp_min(mtp_dev.xfer_file_length - write_count, MTP_MAX_BUFF_SIZE));
			if (mtp_exit) {
				gadget_err("mtp_exit \n");
				break;
			}
			esFSYS_fseek(mtp_dev.xfer_file, offset, 0);
retry_write:			
			write_count = esFSYS_fwrite(data_buff, sizeof(unsigned char), mtp_min(mtp_dev.xfer_file_length - recv_len, MTP_MAX_BUFF_SIZE), mtp_dev.xfer_file);
			write_len += write_count;
			if (write_len < mtp_min(mtp_dev.xfer_file_length - recv_len, MTP_MAX_BUFF_SIZE))
			{
				goto retry_write;
			}

			offset += mtp_min(mtp_dev.xfer_file_length - recv_len, MTP_MAX_BUFF_SIZE);
			recv_len += mtp_min(mtp_dev.xfer_file_length - recv_len, MTP_MAX_BUFF_SIZE);
			gadget_debug("recv_len:%d xfer_file_length:%d\n", recv_len, mtp_dev.xfer_file_length);
		}
next:		
		hal_free(data_buff);
		mtp_dev.xfer_result = 0;
		hal_sem_getvalue(mtp_dev.wait_sem, &val);
		if (val == 0)
		{
			hal_sem_post(mtp_dev.wait_sem);
		}		
    }
    return ;
}

static void send_file_work(void *pArg)
{
    while (1)
    {
    	int val = 0, ret = 0;
    	unsigned char *data_buff = NULL;		
		struct mtp_data_header *header = NULL;
    	unsigned int send_len = 0, read_len = 0, read_count = 0, offset = 0, hdr_size = 0, sendZLP = 0;

		hal_sem_wait(mtp_dev.send_thread_sem);

		if (mtp_dev.xfer_send_header) {
			hdr_size = sizeof(struct mtp_data_header);
			mtp_dev.xfer_file_length += hdr_size;
		} else {
			hdr_size = 0;
		}

		/* we need to send a zero length packet to signal the end of transfer
		 * if the transfer size is aligned to a packet boundary.
		 */
		if ((mtp_dev.xfer_file_length & (mtp_highspeed_in_desc.wMaxPacketSize - 1)) == 0)
			sendZLP = 1;
		
		data_buff = hal_malloc(mtp_min(mtp_dev.xfer_file_length, MTP_MAX_BUFF_SIZE));
		offset = mtp_dev.xfer_file_offset;
		
		while (send_len < mtp_dev.xfer_file_length) {
			if (mtp_dev.state == STATE_CANCELED) {
				/* report cancelation to userspace */
				ret = -ECANCELED;
				gadget_err("mtp_dev.state:%d go finish\n", mtp_dev.state);
				goto next;
			}
			gadget_debug("send_len:%d xfer_file_length:%d\n", send_len, mtp_dev.xfer_file_length);
			memset(data_buff, 0x00, mtp_min(mtp_dev.xfer_file_length, MTP_MAX_BUFF_SIZE));
			read_len = mtp_min(mtp_dev.xfer_file_length - send_len, MTP_MAX_BUFF_SIZE);
			if (hdr_size) {
				/* prepend MTP data header */
				header = (struct mtp_data_header *)data_buff;
				header->length = __cpu_to_le32(mtp_dev.xfer_file_length);
				header->type = __cpu_to_le16(2); /* data packet */
				header->command = __cpu_to_le16(mtp_dev.xfer_command);
				header->transaction_id = __cpu_to_le32(mtp_dev.xfer_transaction_id);
				read_len = read_len - hdr_size;
			}

			esFSYS_fseek(mtp_dev.xfer_file, offset, 0);
			read_count = esFSYS_fread(data_buff + hdr_size, sizeof(unsigned char), read_len, mtp_dev.xfer_file);
			if (read_count < 0)
			{
				gadget_err("read file fail!\n");
				goto next;
			}
			gadget_debug("send_len:%d read_count:%d!\n", send_len, read_count);
			if (mtp_exit) {
				gadget_err("mtp_exit \n");
				break;
			}			
			usb_gadget_function_write(1, data_buff, read_count + hdr_size);
			send_len += (read_count + hdr_size);
			offset += read_count;
			hdr_size = 0;
			gadget_debug("send_len:%d xfer_file_length:%d\n", send_len, mtp_dev.xfer_file_length);
		}
next:	
		hal_free(data_buff);
		mtp_dev.xfer_result = ret;
		hal_sem_getvalue(mtp_dev.wait_sem, &val);
		if (val == 0)
		{
			hal_sem_post(mtp_dev.wait_sem);
		}
    }
    return ;
}

void create_task(void)
{
	recv_task = kthread_create(receive_file_work, NULL, "receive_file_work");
    if (recv_task == NULL)
    {
        printf("ERR: kthread_create failed\n");
        return ;
    }
	kthread_start(recv_task);
	
	send_task = kthread_create(send_file_work, NULL, "send_file_work");
    if (send_task == NULL)
    {
        printf("ERR: kthread_create failed\n");
        return ;
    }
	kthread_start(send_task);
}
static rt_err_t sunxi_mtp_open(rt_device_t dev, rt_uint16_t oflag) 
{
	gadget_debug("here\n");
    return 0;
}
static rt_err_t sunxi_mtp_close(rt_device_t dev) 
{
	gadget_debug("here\n");
    return 0;
}
static rt_size_t sunxi_mtp_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size) 
{
	if (mtp_exit) {
		gadget_debug("here\n");
		return -1;
	}
	gadget_debug("here");
	return usb_gadget_function_read(2, (char *)buffer, size);
}
static rt_size_t sunxi_mtp_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
	if (mtp_exit) {
		gadget_debug("here\n");
		return -1;
	}	
	gadget_debug("here");
    return usb_gadget_function_write(1,(char *)buffer, size);
}
static rt_err_t sunxi_mtp_control(rt_device_t dev, int cmd, void *args)
{
//	struct file *filp = NULL;
	int ret = -EINVAL;

	gadget_debug("cmd:%x\n", cmd);
	switch (cmd) {
		case MTP_SEND_FILE:
		case MTP_RECEIVE_FILE:
		case MTP_SEND_FILE_WITH_HEADER:
		{
			hal_sem_t task_sem;
			mtp_file_range_t *mfr = (mtp_file_range_t *)args;

			if (mtp_dev.state == STATE_CANCELED) {
				/* report cancelation to userspace */
				mtp_dev.state = STATE_READY;
	//			spin_unlock_irq(&dev->lock);
	//			ret = -ECANCELED;
	//			goto out;
			}
			if (mtp_dev.state == STATE_OFFLINE) {
	//			spin_unlock_irq(&dev->lock);
	//			ret = -ENODEV;
	//			goto out;
			}
			mtp_dev.state = STATE_BUSY;
	//		spin_unlock_irq(&dev->lock);

			/* write the parameters */
			mtp_dev.xfer_file = mfr->fp;
			mtp_dev.xfer_file_offset = mfr->offset;
			mtp_dev.xfer_file_length = mfr->length;

			if (cmd == MTP_SEND_FILE_WITH_HEADER) {
				task_sem = mtp_dev.send_thread_sem;
				mtp_dev.xfer_send_header = 1;
				mtp_dev.xfer_command = mfr->command;
				mtp_dev.xfer_transaction_id = mfr->transaction_id;
			} else if (cmd == MTP_SEND_FILE) {
				task_sem = mtp_dev.send_thread_sem;
				mtp_dev.xfer_send_header = 0;
			} else {
				task_sem = mtp_dev.recv_thread_sem;
			}

	//		/* We do the file transfer on a work queue so it will run
	//		 * in kernel context, which is necessary for vfs_read and
	//		 * vfs_write to use our buffers in the kernel address space.
	//		 */
			hal_sem_post(task_sem);
	//		queue_work(dev->wq, work);
	//		/* wait for operation to complete */
	//		flush_workqueue(dev->wq);
	//		fput(filp);

	//		/* read the result */
	//		smp_rmb();
			hal_sem_wait(mtp_dev.wait_sem);
			ret = mtp_dev.xfer_result;
	//		
		} break;
		case MTP_SEND_EVENT:
		{
			/* return here so we don't change dev->state below,
			 * which would interfere with bulk transfer state.
			 */
			ret = mtp_send_event(&mtp_dev, (struct mtp_event *)args);
		} break;
		case MTP_EXIT:
			mtp_exit = 1;
			usb_gadget_function_exit();
		break;
		default:
			gadget_err("cmd:%x not support\n", cmd);
			break;
	}
//fail:
//	spin_lock_irq(&dev->lock);
//	if (dev->state == STATE_CANCELED)
//		ret = -ECANCELED;
//	else if (dev->state != STATE_OFFLINE)
//		dev->state = STATE_READY;
//	spin_unlock_irq(&dev->lock);
//out:
//	mtp_unlock(&dev->ioctl_excl);
//	DBG(dev->cdev, "ioctl returning %d\n", ret);
    return 0;
}
int mtp_standard_req(struct usb_ctrlrequest *crq)
{
	int ret = 0;
    gadget_debug("standard req:%u\n", crq->bRequest);
    switch (crq->bRequest)
    {
        case USB_REQ_SET_CONFIGURATION:
            /* init bulk-in ep */
            hal_udc_ep_enable(mtp_highspeed_in_desc.bEndpointAddress,
                              mtp_highspeed_in_desc.wMaxPacketSize,
                              mtp_highspeed_in_desc.bmAttributes & USB_ENDPOINT_XFERTYPE_MASK);
            hal_udc_ep_enable(mtp_highspeed_out_desc.bEndpointAddress,
                              mtp_highspeed_out_desc.wMaxPacketSize,
                              mtp_highspeed_out_desc.bmAttributes & USB_ENDPOINT_XFERTYPE_MASK);
            hal_udc_ep_enable(mtp_intr_desc.bEndpointAddress,
                              mtp_intr_desc.wMaxPacketSize,
                              mtp_intr_desc.bmAttributes & USB_ENDPOINT_XFERTYPE_MASK);
			mtp_dev.state = STATE_READY;
            break;
        default:
			gadget_debug("%d bRequest Not Need to Proccess\n", crq->bRequest);
            break;
    }
//	printf("%s %d %s :\n", __FILE__, __LINE__, __func__);
    return 0;
}

int mtp_class_req(struct usb_ctrlrequest *crq)
{
	int	value = -EOPNOTSUPP;
	unsigned short	w_index = le16_to_cpu(crq->wIndex);
	unsigned short	w_value = le16_to_cpu(crq->wValue);
	unsigned short	w_length = le16_to_cpu(crq->wLength);
	
	if (crq->bRequestType == (USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_DEVICE)
			&& crq->bRequest == USB_REQ_GET_DESCRIPTOR && (w_value >> 8) == USB_DT_STRING
			&& (w_value & 0xFF) == MTP_OS_STRING_ID) {
		value = (w_length < sizeof(mtp_os_string) ? w_length : sizeof(mtp_os_string));
		hal_udc_ep_set_buf(0, mtp_os_string, value);
//		hal_udc_ep_write(0, mtp_os_string, value);
	} else if ((crq->bRequestType & USB_TYPE_MASK) == USB_TYPE_VENDOR) {
		/* Handle MTP OS descriptor */
		gadget_debug("vendor request: %d index: %d value: %d length: %d\n", crq->bRequest, w_index, w_value, w_length);
		if (crq->bRequest == 1 && (crq->bRequestType & USB_DIR_IN) && (w_index == 4 || w_index == 5)) {
			value = (w_length < sizeof(mtp_ext_config_desc) ? w_length : sizeof(mtp_ext_config_desc));			
//			hal_udc_ep_write(0, &mtp_ext_config_desc, value);
			hal_udc_ep_set_buf(0, &mtp_ext_config_desc, value);
		}
	} else if ((crq->bRequestType & USB_TYPE_MASK) == USB_TYPE_CLASS) {
		gadget_debug("class request: %d index: %d value: %d length: %d\n", crq->bRequest, w_index, w_value, w_length);
		if (crq->bRequest == MTP_REQ_CANCEL && w_index == 0 && w_value == 0) {
			unsigned char respond[1024] = {0};
			if (mtp_dev.state == STATE_BUSY) {
				gadget_err("set state STATE_CANCELED\n");
				mtp_dev.state = STATE_CANCELED;
			}
			/* We need to queue a request to read the remaining
			 *  bytes, but we don't actually need to look at
			 * the contents.
			 */
			hal_udc_ep_set_buf(0, respond, w_length);
		} else if (crq->bRequest == MTP_REQ_GET_DEVICE_STATUS && w_index == 0 && w_value == 0) {
			struct mtp_device_status status = {0};//cdev->req->buf;
			status.wLength = __constant_cpu_to_le16(sizeof(struct mtp_device_status));
//			gadget_debug("MTP_REQ_GET_DEVICE_STATUS\n");
//			spin_lock_irqsave(&dev->lock, flags);
//			/* device status is "busy" until we report
//			 * the cancelation to userspace
//			 */
			if (mtp_dev.state == STATE_CANCELED) 
			{
				status.wCode = __cpu_to_le16(MTP_RESPONSE_DEVICE_BUSY);
//				status.wCode = __cpu_to_le16(MTP_RESPONSE_OK);
			}
			else 
			{
				status.wCode = __cpu_to_le16(MTP_RESPONSE_OK);
			}
//			spin_unlock_irqrestore(&dev->lock, flags);
			value = sizeof(status);
			hal_udc_ep_set_buf(0, &status, value);
		}
	}
    return 0;
}

#if 0
static int mtp_get_serialnumber(struct usb_function_driver *fd)
{
    int ret = -1;
    char serialnumber[64];

    memset(serialnumber, 0, sizeof(serialnumber));
    /* TODO get serialnumber */
    /*ret = get_serialnumber(serialnumbfer, sizeof(serialnumber));*/
    if (ret < 0)
    {
        /* use chipid as serialnumber */
        unsigned int chipid[4] = {0};
        unsigned int sernum[3] = {0};
        //if (hal_get_chipid((unsigned char *)chipid) < 0)
        //  return -1;
        /* high 76bits for serial number */
        sernum[0] = (chipid[1] >> 20) & 0xFFF;
        sernum[1] = chipid[2];
        sernum[2] = chipid[3];
        sprintf(serialnumber, "%03x%08x%08x",
                sernum[0], sernum[1], sernum[2]);
        /*printf("len:%d, serialnumber:%s\n", strlen(serialnumber), serialnumber);*/
    }

    return usb_gadget_string_set(fd, serialnumber, USB_GADGET_SERIAL_IDX);
}
#endif

int mtp_desc_init(struct usb_function_driver *fd)
{
    uint32_t config_desc_len = 0;
    int i = 0, ret = 0;
    void *buf = NULL;
    uint16_t *str;

	memset(&mtp_dev, 0x00, sizeof(mtp_dev_t));
	mtp_dev.state = STATE_OFFLINE;
	mtp_dev.dev= rt_device_create(RT_Device_Class_USBDevice, 0);
	if (!mtp_dev.dev)
	{
		gadget_err("rt_device_create fail\n");
		return ret;
	}
	//mkdevice
	mtp_dev.dev->open = sunxi_mtp_open;
	mtp_dev.dev->close = sunxi_mtp_close;
	mtp_dev.dev->read = sunxi_mtp_read;
	mtp_dev.dev->write = sunxi_mtp_write;
	mtp_dev.dev->control = sunxi_mtp_control;

    config_desc_len = mtp_config_desc.bLength + mtp_intf_desc.bLength
                      + mtp_highspeed_in_desc.bLength + mtp_highspeed_out_desc.bLength + mtp_intr_desc.bLength;
    gadget_debug("config len:%ld\n", config_desc_len);
    fd->config_desc = calloc(1, config_desc_len);
    if (!fd->config_desc)
    {
        gadget_err("no memory.\n");
        goto err;
    }

    buf = fd->config_desc;
    memcpy(buf, &mtp_config_desc, mtp_config_desc.bLength);
    buf += mtp_config_desc.bLength;
    memcpy(buf, &mtp_intf_desc, mtp_intf_desc.bLength);
    buf += mtp_intf_desc.bLength;
    memcpy(buf, &mtp_highspeed_in_desc, mtp_highspeed_in_desc.bLength);
    buf += mtp_highspeed_in_desc.bLength;
    memcpy(buf, &mtp_highspeed_out_desc, mtp_highspeed_out_desc.bLength);
    buf += mtp_intr_desc.bLength;
    memcpy(buf, &mtp_intr_desc, mtp_intr_desc.bLength);

    hal_udc_device_desc_init(&mtp_device_desc);
    hal_udc_config_desc_init(fd->config_desc, config_desc_len);
    /*FlushDcacheRegion(fd->config_desc, config_desc_len);*/

#if 0
    mtp_get_serialnumber(fd);
#endif

    for (i = 0; i < USB_GADGET_MAX_IDX; i++)
    {
        str = fd->strings[i] ? fd->strings[i] : (uint16_t *)mtp_string_desc[i];
        hal_udc_string_desc_init(str);
    }

    fd->class_req = mtp_class_req;
    fd->standard_req = mtp_standard_req;

    fd->ep_addr = calloc(4, sizeof(uint8_t));
    if (!fd->ep_addr)
    {
        gadget_err("no memory.\n");
        goto err;
    }
    fd->ep_addr[0] = 0;
    fd->ep_addr[1] = mtp_highspeed_in_desc.bEndpointAddress;
    fd->ep_addr[2] = mtp_highspeed_out_desc.bEndpointAddress;
    fd->ep_addr[3] = mtp_intr_desc.bEndpointAddress;
	mtp_dev.send_thread_sem = hal_sem_create(0);
	mtp_dev.recv_thread_sem = hal_sem_create(0);
	mtp_dev.wait_sem = hal_sem_create(0);
	create_task();
	ret = rt_device_register(mtp_dev.dev, "mtp", RT_DEVICE_FLAG_RDWR);
	if (ret != RT_EOK)
	{
		rt_device_destroy(mtp_dev.dev);
		goto err;
	}
    return 0;
err:
	if (recv_task)
	{
		kthread_stop(recv_task);
	}

	if (send_task)
	{
		kthread_stop(send_task);
	}
	
	if (mtp_dev.wait_sem)
	{
		hal_sem_delete(mtp_dev.wait_sem);
	}	

	if (mtp_dev.recv_thread_sem)
	{
		hal_sem_delete(mtp_dev.recv_thread_sem);
	}
	
	if (mtp_dev.send_thread_sem)
	{
		hal_sem_delete(mtp_dev.send_thread_sem);
	}

	if (mtp_dev.dev)
	{
		rt_device_destroy(mtp_dev.dev);
	}

    if (fd->config_desc)
    {
        free(fd->config_desc);
        fd->config_desc = NULL;
    }

    if (fd->ep_addr)
    {
        free(fd->ep_addr);
        fd->ep_addr = NULL;
    }

    return 0;
}
int mtp_desc_deinit(struct usb_function_driver *fd)
{
	if (recv_task)
	{
		kthread_stop(recv_task);
	}
	if (send_task)
	{
		kthread_stop(send_task);
	}
	if (mtp_dev.wait_sem)
	{
		hal_sem_delete(mtp_dev.wait_sem);
	}	

	if (mtp_dev.recv_thread_sem)
	{
		hal_sem_delete(mtp_dev.recv_thread_sem);
	}
	
	if (mtp_dev.send_thread_sem)
	{
		hal_sem_delete(mtp_dev.send_thread_sem);
	}

	if (mtp_dev.dev)
	{
		rt_device_unregister(mtp_dev.dev);
		rt_device_destroy(mtp_dev.dev);
	}

    if (fd->config_desc)
    {
        free(fd->config_desc);
        fd->config_desc = NULL;
    }

    if (fd->ep_addr)
    {
        free(fd->ep_addr);
        fd->ep_addr = NULL;
    }
}

struct usb_function_driver mtp_usb_func =
{
    .name = "mtp",
	.desc_init = mtp_desc_init,
	.desc_deinit = mtp_desc_deinit,
};

int usb_gadget_mtp_init(void)
{
    return usb_gadget_function_register(&mtp_usb_func);
}
int usb_gadget_mtp_deinit(void)
{
	usb_gadget_function_unregister(&mtp_usb_func);
	return 0;
}

/* 向app发送PlugIn消息 */
void SendPlugIn2App(void)
{
    __epos_kmsg_t   *msg = NULL;
    /* msg must alloc memory by balloc function     */
    msg = (__epos_kmsg_t *)rt_malloc(sizeof(__epos_kmsg_t));

    if (!msg)
    {
        printf("malloc fail!\n");
        return;
    }

    msg->target       = KMSG_TGT_BROADCST;
    msg->message      = KMSG_VAL_DEV_PLUGIN;
    msg->prio         = KMSG_PRIO_LOW;
    msg->l.para       = KMSG_VAL_DEV_USBD;
//    printf("ERR: send plugin msg to app!\n");
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
        printf("malloc fail!\n");
        return;
    }

    msg->target       = KMSG_TGT_BROADCST;
    msg->message      = KMSG_VAL_DEV_PLUGOUT;
    msg->prio         = KMSG_PRIO_LOW;
    msg->l.para       = KMSG_VAL_DEV_USBD;
//    printf("ERR: send plugout msg to app!\n");
    esKSRV_SendMsgEx((void *)msg);
}

