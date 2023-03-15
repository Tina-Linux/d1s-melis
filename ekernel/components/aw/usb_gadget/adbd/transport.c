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
#include <string.h>
#include "adb.h"
#include "misc.h"


static int read_packet(apacket **ppacket, atransport *t)
{
	int ret;
	ret = adb_dequeue(t->to_remote, (void **)ppacket, 0);
	return ret;
}

/* write msg to remote(usb, tcp/ip) */
static void *input_thread(void *_t)
{
	atransport *t = _t;
	apacket *p = NULL;
	int active = 0;

	adbd_debug("startint transport(%d), input thread",
		t->type);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	for (;;) {
		adbd_debug("");
		p = NULL;
		if (read_packet(&p, t)) {
			adbd_err("failed to read apacket from transport(%d)",
				t->type);
			break;
		}

		if(p == NULL)
		{
		    adbd_err("fatal error, unexpeced p is null.");
		    continue;
		}

		if (p->msg.command == A_SYNC) {
			if (p->msg.arg0 == 0) {
				adbd_debug("transport SYNC offline");
				put_apacket(p);
				break;
			} else {
				if (p->msg.arg1 == t->sync_token) {
					adbd_debug("transport SYNC online");
					active = 1;
				} else {
					adbd_debug("transport igoring SYNC");
				}
			}
		} else {
			if (active) {
				adbd_debug("transport got packet, sending to remote");
				t->write_to_remote(p, t);
			} else {
				adbd_err("transport ignoring packet while offline");
			}
		}
		put_apacket(p);
	}
	return 0;
}

static int write_packet(apacket *p, atransport *t)
{
	int ret = 0;
	/* write to queue */
	ret = adb_enqueue(t->to_remote, (void **)&p, 0);
	return ret;
}

void send_apacket(apacket *p, atransport *t)
{
	unsigned char *x;
        unsigned int sum;
        int count;

	p->msg.magic = p->msg.command ^ 0xffffffff;
        count = p->msg.data_length;
        x = (unsigned char *)p->data;
        sum = 0;
        while (count-- > 0)
                sum += *x++;
        p->msg.data_check = sum;

        print_apacket("send", p);

	if (write_packet(p, t) != 0)
		adbd_err("write packet failed\n");
	return;
}


void send_write_combine(const char *buf1, int size1,
			const char *buf2, int size2,
			unsigned int local, unsigned int remote, atransport *t)
{
	adbd_debug("");

	apacket *p = get_apacket();
	p->msg.command = A_WRTE;
	p->msg.arg0 = local;
	p->msg.arg1 = remote;
	memcpy(p->data, buf1, size1);
	memcpy(p->data + size1, buf2, size2);
	p->msg.data_length = size1 + size2;

	send_apacket(p, t);
}

void send_write(const char *buf, int size, unsigned int local, unsigned int remote, atransport *t)
{
	adbd_debug("");

	apacket *p = get_apacket();
	p->msg.command = A_WRTE;
	p->msg.arg0 = local;
	p->msg.arg1 = remote;
	memcpy(p->data, buf, size);
	p->msg.data_length = size;

	send_apacket(p, t);
}


int check_header(apacket *p)
{
	if (p->msg.magic != (p->msg.command ^ 0xffffffff)) {
		adbd_debug("invalid magic");
		return -1;
	}
	if (p->msg.data_length > MAX_PAYLOAD) {
		adbd_err("%d > MAX_PAYLOAD", p->msg.data_length);
		return -1;
	}
	return 0;
}

int check_data(apacket *p)
{
	unsigned int count, sum;
	unsigned char *x;

	count = p->msg.data_length;
	x = p->data;
	sum = 0;
	while (count-- > 0) {
		sum += *x++;
	}
	if (sum != p->msg.data_check)
		return -1;
	return 0;
}
apacket *g_packet = NULL;

/* read msg from remote(usb, tcp/ip) */
static void *output_thread(void *_t)
{
	atransport *t = _t;
	// apacket *p;

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	adbd_debug("startint transport(%d), output thread, SYNC online(%d)",
		t->type, t->sync_token + 1);

	g_packet = get_apacket();
	g_packet->msg.command = A_SYNC;
	g_packet->msg.arg0 = 1;
	g_packet->msg.arg1 = ++(t->sync_token);
	g_packet->msg.magic = A_SYNC ^ 0xffffffff;
	if (write_packet(g_packet, t)) {
		put_apacket(g_packet);
		adbd_err("failed to write SYNC packet");
		goto err;
	}

	adbd_debug("data pump started");
	for (;;) {		
		g_packet = get_apacket();
		if (g_packet == NULL) {
			continue;
		}
		if (t->read_from_remote(g_packet, t) != 0) {
			adbd_debug("remote read failed for transport(%d)", t->type);
			put_apacket(g_packet);
			continue;
		}
		handle_packet(g_packet, t);
	}

	adbd_debug("SYNC offline for transport(%d)", t->type);
	g_packet = get_apacket();
	g_packet->msg.command = A_SYNC;
	g_packet->msg.arg0 = 0;
	g_packet->msg.arg1 = 0;
	g_packet->msg.magic = A_SYNC ^ 0xffffffff;
	if(write_packet(g_packet, t)) {
		put_apacket(g_packet);
		adbd_err("failed to write SYNC packet");
		goto err;
	}

err:
	adbd_debug("transport(%d) output thread is exiting", t->type);
	/*kick_transport(t);	*/
	/*transport_unref(t)*/
	return 0;
}

static adb_thread_t output_thread_ptr; /* */
static adb_thread_t input_thread_ptr;
static void register_transport(atransport *t)
{
	if (adb_thread_create(&input_thread_ptr, input_thread, "adbd-input", t, ADB_THREAD_HIGH_PRIORITY)) {
		fatal("cannot create input thread");
	}
	if (adb_thread_create(&output_thread_ptr, output_thread, "adbd-output", t, ADB_THREAD_HIGH_PRIORITY)) {
		fatal("cannot create output thread");
	}
}
static void unregister_transport(atransport *t)
{
	adb_service_destroy_all();

	pthread_cancel(input_thread_ptr);
	pthread_join(input_thread_ptr, NULL);

	pthread_cancel(output_thread_ptr);
	pthread_join(output_thread_ptr, NULL);

	if(g_packet){
		put_apacket(g_packet);
		g_packet = NULL;
	}
}

static atransport *t = NULL;
void register_usb_transport(usb_handle *usb, const char *serial, const char *devpath, unsigned writeable)
{
	t = adb_calloc(1, sizeof(atransport));
	if (!t) {
		adbd_err("no memory\n");
		return;
	}

	adbd_debug("");

	init_usb_transport(t, usb, (writeable ? CS_OFFLINE : CS_NOPERM));

	t->to_remote = adb_queue_init();
	/* TODO: serial, devpath */
	register_transport(t);
}
void unregister_usb_transport(usb_handle *usb, const char *serial, const char *devpath, unsigned writeable)
{
	unregister_transport(t);
	adb_queue_release(t->to_remote);
	deinit_usb_transport(t, usb, CS_OFFLINE);
	adb_free(t);
}

