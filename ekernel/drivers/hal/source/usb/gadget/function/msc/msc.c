#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sunxi_hal_usb.h>
#include "gadget.h"

#define EOPNOTSUPP      95      /* Operation not supported on transport endpoint */

#define  DRIVER_VENDOR_ID           0x1F3A  /* 厂商ID */
#define  DRIVER_PRODUCT_ID          0x1000  /* 产品ID */


static const uint16_t g_str_lang_id[] = {
	0x0304, 0x0409
};

static const uint16_t g_str_manufacturer[] = {
	0x0314, 'A', 'l', 'l', 'w', 'i', 'n', 'n', 'e', 'r'
};

static const uint16_t g_str_product[] = {
	0x030a, 'T', 'i', 'n', 'a',
};

static const uint16_t g_str_serialnumber[] = {
	0x0312, '2', '0', '0', '8', '0', '4', '1', '1'
};

static const uint16_t g_str_config[] = {
	0x0302,
};

static const uint16_t g_str_interface[] = {
	0x031c, 'M', 'S', 'C', ' ', 'I', 'n', 't', 'e', 'r', 'f', 'a', 'c', 'e'
};

static const uint16_t *msc_string_desc[USB_GADGET_MAX_IDX] = {
	g_str_manufacturer,
	g_str_product,
	g_str_serialnumber,
	g_str_lang_id,
	g_str_config,
	g_str_interface,
};
static struct usb_device_descriptor msc_device_desc = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0200,
	.bDeviceClass = 0x0,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64,
	.idVendor = DRIVER_VENDOR_ID,
	.idProduct = DRIVER_PRODUCT_ID,
	.bcdDevice = 0x2,
	.iManufacturer = USB_GADGET_MANUFACTURER_IDX,
	.iProduct = USB_GADGET_PRODUCT_IDX,
	.iSerialNumber = USB_GADGET_SERIAL_IDX,
	.bNumConfigurations = 1
};

static struct usb_config_descriptor msc_config_desc = {
	.bLength = USB_DT_CONFIG_SIZE,
	.bDescriptorType = USB_DT_CONFIG,
	.wTotalLength = USB_DT_CONFIG_SIZE + USB_DT_INTERFACE_SIZE +
					USB_DT_ENDPOINT_SIZE + USB_DT_ENDPOINT_SIZE,
	.bNumInterfaces = 1,
	.bConfigurationValue = 1,
	.iConfiguration = USB_GADGET_CONFIG_IDX,
	.bmAttributes = 0x80,
	.bMaxPower = 0xfa /* 500mA */
};

static struct usb_interface_descriptor msc_intf_desc = {
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0x0,
	.bAlternateSetting = 0x0,
	.bNumEndpoints = 2,
	.bInterfaceClass = USB_CLASS_MASS_STORAGE, /* Interface class */
	.bInterfaceSubClass = 0x06, /* Subclass */
	.bInterfaceProtocol = 0x50, /* Bulk-Only Protocol */
	.iInterface = USB_GADGET_INTERFACE_IDX
};

static struct usb_endpoint_descriptor msc_ep_bulk_out = {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	/*.bEndpointAddress = 0x1 | USB_DIR_OUT,*/
	.bEndpointAddress =  0x2 | USB_DIR_OUT,
	.bmAttributes = USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize = 0x200, /* 512 Bytes */
};

static struct usb_endpoint_descriptor msc_ep_bulk_in = {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress =  0x1 | USB_DIR_IN,
	.bmAttributes = USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize = 0x200, /* 512 Bytes */
};

int msc_standard_req(struct usb_ctrlrequest *crq)
{
	int value = -EOPNOTSUPP;
	uint16_t w_index = crq->wIndex;
	uint16_t w_value = crq->wValue;
	extern void set_config_call(uint16_t w_value);

//	printf("standard req:0x%x,wValue:0x%x,bRequestType=0x%x\n",  crq->bRequest, crq->wValue,crq->bRequestType);
	switch (crq->bRequest) {
	case USB_REQ_SET_CONFIGURATION:
		/* init bulk-in ep */
		hal_udc_ep_enable(msc_ep_bulk_in.bEndpointAddress,
			msc_ep_bulk_in.wMaxPacketSize,
			msc_ep_bulk_in.bmAttributes & USB_ENDPOINT_XFERTYPE_MASK);
		hal_udc_ep_enable(msc_ep_bulk_out.bEndpointAddress,
                        msc_ep_bulk_out.wMaxPacketSize,
                        msc_ep_bulk_out.bmAttributes & USB_ENDPOINT_XFERTYPE_MASK);
		set_config_call(w_value);
		break;
	default:
		break;
	}
	return 0;
}

static int msc_get_serialnumber(struct usb_function_driver *fd)
{
	int ret = -1;
	char serialnumber[64];

	memset(serialnumber, 0, sizeof(serialnumber));
	/* TODO get serialnumber */
	/*ret = get_serialnumber(serialnumbfer, sizeof(serialnumber));*/
	if (ret < 0) {
		/* use chipid as serialnumber */
		unsigned int chipid[4] = {0};
		unsigned int sernum[3] = {0};
		//if (hal_get_chipid((unsigned char *)chipid) < 0)
		//	return -1;
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

int msc_desc_init(struct usb_function_driver *fd)
{
	uint32_t config_desc_len = 0;
	int i;
	void *buf = NULL;
	uint16_t *str;
	extern int msc_class_req(struct usb_ctrlrequest *crq);

	config_desc_len = msc_config_desc.bLength + msc_intf_desc.bLength
			+ msc_ep_bulk_out.bLength + msc_ep_bulk_in.bLength;
	gadget_debug("config len:%u", config_desc_len);
	fd->config_desc = calloc(1, config_desc_len);
	if (!fd->config_desc) {
		gadget_err("no memory.\n");
		goto err;
	}

	buf = fd->config_desc;
	memcpy(buf, &msc_config_desc, msc_config_desc.bLength);
	buf += msc_config_desc.bLength;
	memcpy(buf, &msc_intf_desc, msc_intf_desc.bLength);
	buf += msc_intf_desc.bLength;
	memcpy(buf, &msc_ep_bulk_out, msc_ep_bulk_out.bLength);
	buf += msc_ep_bulk_out.bLength;
	memcpy(buf, &msc_ep_bulk_in, msc_ep_bulk_in.bLength);

	hal_udc_device_desc_init(&msc_device_desc);
	hal_udc_config_desc_init(fd->config_desc, config_desc_len);
	/*FlushDcacheRegion(fd->config_desc, config_desc_len);*/

	msc_get_serialnumber(fd);

	for (i = 0; i < USB_GADGET_MAX_IDX; i++) {
		str = fd->strings[i] ? fd->strings[i] : (uint16_t *)msc_string_desc[i];
		hal_udc_string_desc_init(str);
	}

	fd->class_req = msc_class_req;
	fd->standard_req = msc_standard_req;

	fd->ep_addr = calloc(3, sizeof(uint8_t));
	if (!fd->ep_addr) {
		gadget_err("no memory.\n");
		goto err;
	}
	fd->ep_addr[0] = 0;
	fd->ep_addr[1] = msc_ep_bulk_in.bEndpointAddress;
	fd->ep_addr[2] = msc_ep_bulk_out.bEndpointAddress;

	return 0;
err:
	if (fd->config_desc) {
		free(fd->config_desc);
		fd->config_desc = NULL;
	}

	if (fd->ep_addr) {
		free(fd->ep_addr);
		fd->ep_addr = NULL;
	}

	return 0;
}
int msc_desc_deinit(struct usb_function_driver *fd)
{
	if (fd->config_desc) {
		free(fd->config_desc);
		fd->config_desc = NULL;
	}

	if (fd->ep_addr) {
		free(fd->ep_addr);
		fd->ep_addr = NULL;
	}

	for (int i = 0; i < USB_GADGET_MAX_IDX; i++) {
		if (fd->strings[i] != NULL) {
			free(fd->strings[i]);
			fd->strings[i] = NULL;
		}
	}

	return 0;
}

struct usb_function_driver msc_usb_func = {
	.name = "msc",
	.desc_init = msc_desc_init,
	.desc_deinit = msc_desc_deinit,
};

int usb_gadget_msc_init(void)
{
	return usb_gadget_function_register(&msc_usb_func);
}
int usb_gadget_msc_deinit(void)
{
	return usb_gadget_function_unregister(&msc_usb_func);
}

