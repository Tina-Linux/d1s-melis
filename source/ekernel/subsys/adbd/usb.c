/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
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
#include <stdio.h>
#include <stdlib.h>
#include "adb.h"
#include "misc.h"
#include <usb/usb_gadget.h>

struct usb_handle {
	/*int (*write)(usb_handle *h, const void *data, int len);*/
	/*int (*read)(usb_handle *h, void *data, int len);*/
	int control;
	int bulk_out;	/* "out" from the host's perspective => source for adbd */
	int bulk_in;	/* "in" from the host's perspective => sink for adbd */
};


int usb_write(usb_handle *h, const void *data, int len)
{
	usb_gadget_function_write(h->bulk_in, (char *)data, len);
	return 0;
}

int usb_read(usb_handle *h, void *data, int len)
{
	int size, total = 0;
	char *buf = data;

	while (total < len) {
		size = usb_gadget_function_read(h->bulk_out, buf + total, len - total);
		if (size < 0) {
			adbd_err("usb read failed, return %d", size);
			return size;
		}
		total += size;
	}
	return len;
}
static usb_handle *h = NULL;
int usb_gadget_adb_init(void);
int usb_gadget_adb_deinit(void);
int usb_init(void)
{
	int ret;

	h = adb_calloc(1, sizeof(usb_handle));
	if (!h)
		fatal("no memory");

	h->control = -1;
	h->bulk_out = -1;
	h->bulk_in = -1;

	h->bulk_out = USB_ADB_OUT;
	h->bulk_in = USB_ADB_IN;

	ret = usb_gadget_adb_init();
	if (ret < 0) {
		adbd_err("usb_gadget_adb_init failed");
		goto err;
	}
	ret = usb_gadget_function_enable("adb");
	if (ret < 0) {
		adbd_err("enable adb function failed");
		goto err;
	}

	register_usb_transport(h, 0, 0, 1);
	return ret;
err:
	adb_free(h);
	return ret;
}

int usb_exit(void)
{
	unregister_usb_transport(h, 0, 0, 1);
	usb_gadget_function_disable("adb");
	usb_gadget_adb_deinit();
	adb_free(h);
	return 0;
}

