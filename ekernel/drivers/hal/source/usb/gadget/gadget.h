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

#ifndef __GADGET_H__
#define __GADGET_H__

#include "ch9.h"
#include "sunxi_hal_usb.h"
#include "usb_os_platform.h"

#if 0
#define gadget_info(fmt, args...) \
	printf("[GADGET-INFO][%s] line:%d " fmt "\n", __func__, __LINE__, ##args)

#define gadget_debug(fmt, args...) \
	printf("[GADGET-DEBUG][%s] line:%d " fmt "\n", __func__, __LINE__, ##args)
#else
#define gadget_info(fmt, args...)
#define gadget_debug(fmt, args...)
#endif

#define gadget_err(fmt, args...) \
	printf("[GADGET-ERROR][%s] line:%d " fmt "\n", __func__, __LINE__, ##args)

// typedef sem_t usb_schd_t;

typedef struct {
	unsigned int length;
	char buffer;
} usb_queue_t;

struct usb_function_driver {
	const char *name;
	struct list_head list;
	hal_sem_t read_schd;
	hal_sem_t read_finish_schd;
	hal_sem_t write_finish_schd;
	hal_sem_t write_schd;
	uint8_t *ep_addr;
	usb_queue_t *read_queue;
	usb_queue_t *write_queue;
	void *config_desc;

	int (*desc_init)(struct usb_function_driver *);
	int (*desc_deinit)(struct usb_function_driver *);
	int (*standard_req)(struct usb_ctrlrequest *);
	int (*class_req)(struct usb_ctrlrequest *);

	int read_size;

	int enabled;

	uint16_t *strings[USB_GADGET_MAX_IDX];
};

int usb_gadget_function_register(struct usb_function_driver *newf);
int usb_gadget_function_unregister(struct usb_function_driver *fd);
int usb_gadget_string_set(struct usb_function_driver *ugf, char *str, unsigned int idx);
int usb_gadget_function_exit(void);
#endif /* __GADGET_H__ */
