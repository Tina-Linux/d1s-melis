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
#include "adb.h"


static int remote_read(apacket *p, atransport *t)
{
	int size = 0;

	size = usb_read(t->usb, &p->msg, sizeof(amessage));
	if (size != sizeof(amessage)) {
		adbd_err("expect size=%d, actual read size=%d", sizeof(amessage), size);
		adbd_err("remote usb: read terminated(message)");
		return -1;
	}
	if (check_header(p)) {
		adbd_debug("check header failed");
		return -1;
	}
	if (p->msg.data_length > 0) {
		int residue = p->msg.data_length;
		unsigned char *data = p->data;
		int aw_hexdump(const char *buf, int bytes);
		/*aw_hexdump(&p->msg, sizeof(p->msg));*/
		adbd_debug("command:0x%x, data_length:%u\n", p->msg.command, p->msg.data_length);
		while (residue > 0) {
			size = residue > 512 ? 512 : residue;
			size = usb_read(t->usb, data, size);
			residue -= size;
			data += size;
			adbd_debug("size:%d, residue:%d\n", size, residue);
		}
	}
	if (check_data(p)) {
		adbd_err("check data failed");
		return -1;
	}
	return 0;
}

static int remote_write(apacket *p, atransport *t)
{
	if (usb_write(t->usb, &p->msg, sizeof(amessage))) {
		adbd_debug("remote usb: write terminated(message)");
		return -1;
	}
	if (p->msg.data_length == 0)
		return 0;

	if (usb_write(t->usb, p->data, p->msg.data_length)) {
		adbd_debug("remote usb: write terminated(data)");
		return -1;
	}
	return 0;
}

static void remote_close(atransport *t)
{
	return;
}

static void remote_kick(atransport *t)
{
	return;
}

int init_usb_transport(atransport *t, usb_handle *h, int state)
{
	adbd_debug("transport: usb");
	t->close = remote_close;
	t->kick = remote_kick;
	t->read_from_remote = remote_read;
	t->write_to_remote = remote_write;
	t->sync_token = 1;
	t->connection_state = state;
	t->usb = h;
	return 0;
}
int deinit_usb_transport(atransport *t, usb_handle *h, int state)
{
	adbd_debug("transport: usb");
	t->close = NULL;
	t->kick = NULL;
	t->read_from_remote = NULL;
	t->write_to_remote = NULL;
	t->sync_token = 0;
	t->connection_state = state;
	t->usb = NULL;
	return 0;
}

