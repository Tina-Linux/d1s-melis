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

typedef void * usb_handle_t;
static LIST_HEAD(g_func_list);
static usb_handle_t g_usb_handle = NULL;

#define GADGET_READ     (0)
#define GADGET_WRITE    (1)

static struct usb_function_driver *usb_gadget_function_find(const char *name)
{
	struct usb_function_driver *fd = NULL;

	if (!name)
		return NULL;
	list_for_each_entry(fd, &g_func_list, list) {
		if (!strcmp(fd->name, name))
			return fd;
	}
	return NULL;
}

int usb_gadget_function_register(struct usb_function_driver *newf)
{
	struct usb_function_driver *fd = NULL;

	if (!newf)
		return -1;
	fd = usb_gadget_function_find(newf->name);
	if (fd != NULL)
		return -1;
	list_add_tail(&newf->list, &g_func_list);
	return 0;
}

int usb_gadget_function_unregister(struct usb_function_driver *fd)
{
	list_del(&fd->list);
	return 0;
}

static void usb_gadget_notifier(struct usb_function_driver *fd, int mode)
{
	int ret = 0;
	hal_sem_t schd = NULL;
	hal_sem_t finish_schd = NULL;

	gadget_debug("");
	if (mode == GADGET_READ) {
		schd = fd->read_schd;
		finish_schd = fd->read_finish_schd;
	} else if (mode == GADGET_WRITE) {
		schd = fd->write_schd;
		finish_schd = fd->write_finish_schd;
	} else {
		gadget_err("unknown mode");
	}

	if (!schd) {
		return;
	}
	ret |= hal_sem_post(schd);
	ret |= hal_sem_post(finish_schd);
	if (ret == 0) {
		return;
	} else {
		gadget_err("gadget notifier failed, mode=%d\n", mode);
	}
}

/*#define GADGET_WRITE_COMPLETE_DEBUG*/
#ifdef GADGET_WRITE_COMPLETE_DEBUG
int write_complete = 1;
#endif
static udc_errno_t usb_gadget_callback(uint8_t ep_addr,
				       udc_callback_event_t event,
				       void *data,
				       uint32_t len)
{
	uint8_t ep_idx;
	uint8_t is_in;
	udc_errno_t ret = UDC_ERRNO_SUCCESS;
	struct usb_ctrlrequest *crq;
	struct usb_function_driver *fd = (struct usb_function_driver *)g_usb_handle;

	/*gadget_debug("ep:0x%02x, event:%d, len:%d\n", ep_addr, event, len);*/

	ep_idx = ep_addr & 0x7f;
	is_in = ep_addr & USB_DIR_IN;

	if (ep_idx == 0) { /* handle ep0 */
		crq = (struct usb_ctrlrequest *)data;
		switch (event) {
		case UDC_EVENT_RX_STANDARD_REQUEST:
			/*ret = usb_adb_standard_request_handler(crq);*/
			if (fd->standard_req) {
				fd->standard_req(crq);
			}
			break;
		case UDC_EVENT_RX_CLASS_REQUEST:
			/*ret = usb_msg_class_request_handler(crq);*/
			if (fd->class_req)
				fd->class_req(crq);
			break;
		default:
			ret = UDC_ERRNO_CMD_NOT_SUPPORTED;
			break;
		}
	} else { /* handle ep1~4 */
		if (is_in) {
			/* TODO: maybe useless? */
		} else {
			gadget_debug("event:0x%x\n", event);
			switch (event) {
			case UDC_EVENT_RX_DATA:
				fd->read_size = len;
				usb_gadget_notifier(fd, GADGET_READ);
				break;
			case UDC_EVENT_TX_COMPLETE:
				gadget_debug("tx complete..\n");
				// write_complete = 1;
				usb_gadget_notifier(fd, GADGET_WRITE);
				break;
			default:
				ret = UDC_ERRNO_CMD_NOT_SUPPORTED;
				break;
			}
		}
	}

	return ret;
}

static int usb_gadget_init(struct usb_function_driver *fd)
{
	if (g_usb_handle != NULL || !fd->desc_init)
		return -1;
	// fd->read_schd = xSemaphoreCreateBinary();
	// fd->write_schd = xSemaphoreCreateBinary();
	fd->read_schd = hal_sem_create(1);
	fd->write_schd = hal_sem_create(1);
	fd->read_finish_schd = hal_sem_create(0);
	fd->write_finish_schd = hal_sem_create(0);
	memset(fd->strings, 0, USB_GADGET_MAX_IDX * sizeof(uint16_t *));

	if (fd->desc_init) {
		(*fd->desc_init)(fd);
	}

	hal_udc_init();
	hal_udc_register_callback(usb_gadget_callback);

	g_usb_handle = (usb_handle_t)fd;
	fd->enabled = 1;
	memset(fd->strings, 0, USB_GADGET_MAX_IDX * sizeof(uint16_t *));  // from rtos

	return 0;
}

static int usb_gadget_deinit(struct usb_function_driver *fd)
{
	fd->enabled = 0;

	// memset(fd->strings, 0, USB_GADGET_MAX_IDX * sizeof(uint16_t *));
	g_usb_handle = NULL;
	hal_udc_register_callback(NULL);
	hal_udc_deinit();
	if (fd->desc_deinit) {
		(*fd->desc_deinit)(fd);
	}
	hal_sem_delete(fd->read_schd);
	hal_sem_delete(fd->write_schd);
	hal_sem_delete(fd->read_finish_schd);
	hal_sem_delete(fd->write_finish_schd);
	return 0;
}

int usb_gadget_function_enable(const char *name)
{
	struct usb_function_driver *fd = NULL;

	if (!name)
		return -1;
	fd = usb_gadget_function_find(name);
	if (!fd) {
		gadget_err("usb gadget, can't find %s function\n", name);
		return -1;
	}
	return usb_gadget_init(fd);
}

int usb_gadget_function_disable(const char *name)
{
	struct usb_function_driver *fd = NULL;

	if (!name)
		return -1;
	fd = usb_gadget_function_find(name);
	if (!fd) {
		gadget_err("usb gadget, can't find %s function\n", name);
		return -1;
	}
	usb_gadget_deinit(fd);
	/* unsupport now */
	return 0;
}

int usb_gadget_function_read(int ep_idx, char *buf, int size)
{
	struct usb_function_driver *fd = (struct usb_function_driver *)g_usb_handle;
	int count;

	/*hal_usleep(10000);*/
	gadget_debug("ep_idx:%d\n", ep_idx);
	hal_sem_wait(fd->read_schd);
	count = hal_udc_ep_read(fd->ep_addr[ep_idx], buf, size);
	// gadget_debug("count=%d", count);
	// if (count >= 0) {
	// 	gadget_debug("read data:%d", count);
	// 	return count;
	// }
	gadget_debug("wait ep%u[%x], recv data finish\n", ep_idx, fd->ep_addr[ep_idx]);
	hal_sem_wait(fd->read_finish_schd);
	gadget_debug("receive %u bytes\n", fd->read_size);
	return fd->read_size;
}

/* BLOCK mode */
int usb_gadget_function_write(int ep_idx, char *buf, int size)
{
	struct usb_function_driver *fd = (struct usb_function_driver *)g_usb_handle;
	int count = 0;

	gadget_debug("write ep%u[0x%x], size=%u", ep_idx, fd->ep_addr[ep_idx], size);
#if 0
	while (count < size) {
		count = hal_udc_ep_write(fd->ep_addr[ep_idx], (void *)buf + total, size - total);
		if (count == UDC_ERRNO_TX_BUSY) {
			/*gadget_debug("udc ep write busy...");*/
			continue;
		}
		if (count < 0) {
			gadget_err("udc ep write failed, return %d\n", count);
			return count;
		}
		total += count;
		gadget_debug("gadget write %u bytes, size=%u, now total=%u",
				count, size, total);
	}
#else
	hal_sem_wait(fd->write_schd);
	gadget_debug("write data, len:%u", size);
	count = hal_udc_ep_write(fd->ep_addr[ep_idx], (void *)buf, size);
	// if (count < 0) {
	// 	gadget_err("hal_udc_ep_write failed, return %d", count);
	// 	return -1;
	// }
	gadget_debug("wait ep%u [%x], send data finish\n", ep_idx, fd->ep_addr[ep_idx]);
	hal_sem_wait(fd->write_finish_schd);
	gadget_debug("count=%d, size=%d", count, size);
	gadget_debug("write %u bytes complete", size);
#endif
	return size;
}

int usb_gadget_function_exit(void)
{
	struct usb_function_driver *fd = (struct usb_function_driver *)g_usb_handle;
	int val = 0;

	if (fd->write_schd) {
		hal_sem_getvalue(fd->write_schd, &val);
		if (val == 0) {
			hal_sem_post(fd->write_schd);
		}
	}
	if (fd->write_finish_schd) {
		hal_sem_getvalue(fd->write_finish_schd, &val);
		if (val == 0) {
			hal_sem_post(fd->write_finish_schd);
		}
	}
	if (fd->read_schd) {
		hal_sem_getvalue(fd->read_schd, &val);
		if (val == 0) {
			hal_sem_post(fd->read_schd);
		}
	}
	if (fd->read_finish_schd) {
		hal_sem_getvalue(fd->read_finish_schd, &val);
		if (val == 0) {
			hal_sem_post(fd->read_finish_schd);
		}
	}
	return 0;
}

int usb_gadget_string_set(struct usb_function_driver *fd, char *str, unsigned int idx)
{
	unsigned int slen, blen;
	char *buffer = NULL;
	int i;

	if (!fd || idx >= USB_GADGET_MAX_IDX)
		return -1;

	if (fd->enabled != 0) {
		gadget_err("usb function[%s] already enabled", fd->name);
		return -1;
	}
	slen = strlen(str);
	if (slen <= 0)
		return -1;
	blen = 2 + (2 * slen);
	buffer = malloc(blen);
	if (!buffer) {
		gadget_err("no memory");
		return -1;
	}
	buffer[0] = blen;
	buffer[1] = USB_DT_STRING;
	for (i = 0; i < slen; i++) {
		buffer[2 + 2 * i + 0] = str[i];
		buffer[2 + 2 * i + 1] = 0;
	}

	if (fd->strings[idx] != NULL)
		free(fd->strings[idx]);
	fd->strings[idx] = (uint16_t *)buffer;

	return 0;
}

int usb_gadget_function_string_set(char *name, char *str, unsigned int idx)
{
	struct usb_function_driver *fd;

	fd = usb_gadget_function_find(name);
	if (!fd)
		return -1;
	return usb_gadget_string_set(fd, str, idx);
}
