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

#include "gadget.h"

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
	0x031c, 'A', 'D', 'B', ' ', 'I', 'n', 't', 'e', 'r', 'f', 'a', 'c', 'e'
};

static const uint16_t *adb_string_desc[USB_GADGET_MAX_IDX] = {
	g_str_lang_id,
	g_str_manufacturer,
	g_str_product,
	g_str_serialnumber,
	g_str_config,
	g_str_interface,
};

static struct usb_device_descriptor adb_device_desc = {
	.bLength            = USB_DT_DEVICE_SIZE,
	.bDescriptorType    = USB_DT_DEVICE,
	.bcdUSB             = 0x0200,
	.bDeviceClass       = 0x0,
	.bDeviceSubClass    = 0,
	.bDeviceProtocol    = 0,
	.bMaxPacketSize0    = 64,
	.idVendor           = 0x18d1,
	.idProduct          = 0xd002,
	.bcdDevice          = 0x0409,
	.iManufacturer      = USB_GADGET_MANUFACTURER_IDX,
	.iProduct           = USB_GADGET_PRODUCT_IDX,
	.iSerialNumber      = USB_GADGET_SERIAL_IDX,
	.bNumConfigurations = 1
};

static struct usb_config_descriptor adb_config_desc = {
	.bLength         = USB_DT_CONFIG_SIZE,
	.bDescriptorType = USB_DT_CONFIG,
	.wTotalLength    = USB_DT_CONFIG_SIZE +
			   USB_DT_INTERFACE_SIZE +
			   USB_DT_ENDPOINT_SIZE +
			   USB_DT_ENDPOINT_SIZE,
	.bNumInterfaces      = 1,
	.bConfigurationValue = 1,
	.iConfiguration      = USB_GADGET_CONFIG_IDX,
	.bmAttributes        = 0xc0,
	.bMaxPower           = 0xfa                    /* 500mA */
};

static struct usb_interface_descriptor adb_intf_desc = {
	.bLength            = USB_DT_INTERFACE_SIZE,
	.bDescriptorType    = USB_DT_INTERFACE,
	.bInterfaceNumber   = 0x0,
	.bAlternateSetting  = 0x0,
	.bNumEndpoints      = 2,
	.bInterfaceClass    = 0xff, /* Interface class */
	.bInterfaceSubClass = 0x42, /* Subclass */
	.bInterfaceProtocol = 0x1,  /* Bulk-Only Protocol */
	.iInterface         = USB_GADGET_INTERFACE_IDX
};

static struct usb_endpoint_descriptor adb_ep_bulk_out = {
	.bLength         = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	/*.bEndpointAddress = 0x1 | USB_DIR_OUT,*/
	.bEndpointAddress = 0x2 | USB_DIR_OUT,
	.bmAttributes     = USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize   = 0x0200, /* 512 Bytes */
	.bInterval        = 0
};

static struct usb_endpoint_descriptor adb_ep_bulk_in = {
	.bLength          = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType  = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x1 | USB_DIR_IN,
	.bmAttributes     = USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize   = 0x0200, /* 512 Bytes */
	.bInterval        = 0
};

int adb_standard_req(struct usb_ctrlrequest *crq)
{
	gadget_debug("[%s] line:%d standard req:%u\n", __func__, __LINE__, crq->bRequest);
	switch (crq->bRequest) {
	case USB_REQ_SET_CONFIGURATION:
		/* init bulk-in ep */
		hal_udc_ep_enable(adb_ep_bulk_in.bEndpointAddress,
				  adb_ep_bulk_in.wMaxPacketSize,
				  adb_ep_bulk_in.bmAttributes & USB_ENDPOINT_XFERTYPE_MASK);
		hal_udc_ep_enable(adb_ep_bulk_out.bEndpointAddress,
				  adb_ep_bulk_out.wMaxPacketSize,
				  adb_ep_bulk_out.bmAttributes & USB_ENDPOINT_XFERTYPE_MASK);
		break;
	default:
		break;
	}
	return 0;
}

int adb_class_req(struct usb_ctrlrequest *crq)
{
	return 0;
}

static int adb_get_serialnumber(struct usb_function_driver *fd)
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
		// if (hal_get_chipid((unsigned char *)chipid) < 0)
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

int adb_desc_init(struct usb_function_driver *fd)
{
	uint32_t config_desc_len = 0;
	int i;
	void *buf = NULL;
	uint16_t *str;

	config_desc_len = adb_config_desc.bLength + adb_intf_desc.bLength
			+ adb_ep_bulk_out.bLength + adb_ep_bulk_in.bLength;
	gadget_debug("config len:%u", config_desc_len);
	fd->config_desc = calloc(1, config_desc_len);
	if (!fd->config_desc) {
		gadget_err("no memory.\n");
		goto err;
	}

	buf = fd->config_desc;
	memcpy(buf, &adb_config_desc, adb_config_desc.bLength);
	buf += adb_config_desc.bLength;
	memcpy(buf, &adb_intf_desc, adb_intf_desc.bLength);
	buf += adb_intf_desc.bLength;
	memcpy(buf, &adb_ep_bulk_out, adb_ep_bulk_out.bLength);
	buf += adb_ep_bulk_out.bLength;
	memcpy(buf, &adb_ep_bulk_in, adb_ep_bulk_in.bLength);

	hal_udc_device_desc_init(&adb_device_desc);
	hal_udc_config_desc_init(fd->config_desc, config_desc_len);
	/*FlushDcacheRegion(fd->config_desc, config_desc_len);*/

	adb_get_serialnumber(fd);

	for (i = 0; i < USB_GADGET_MAX_IDX; i++) {
		str = fd->strings[i] ? fd->strings[i] : (uint16_t *)adb_string_desc[i];
		hal_udc_string_desc_init(str);
	}

	fd->class_req = adb_class_req;
	fd->standard_req = adb_standard_req;

	fd->ep_addr = calloc(3, sizeof(uint8_t));
	if (!fd->ep_addr) {
		gadget_err("no memory.\n");
		goto err;
	}
	fd->ep_addr[0] = 0;
	fd->ep_addr[1] = adb_ep_bulk_in.bEndpointAddress;
	fd->ep_addr[2] = adb_ep_bulk_out.bEndpointAddress;

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

int adb_desc_deinit(struct usb_function_driver *fd)
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

struct usb_function_driver adb_usb_func = {
	.name        = "adb",
	.desc_init   = adb_desc_init,
	.desc_deinit = adb_desc_deinit,
};

int usb_gadget_adb_init(void)
{
	return usb_gadget_function_register(&adb_usb_func);
}

int usb_gadget_adb_deinit(void)
{
	return usb_gadget_function_unregister(&adb_usb_func);
}
