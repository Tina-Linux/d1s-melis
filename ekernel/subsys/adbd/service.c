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
#include <unistd.h>
#include "adb.h"
#include "adb_queue.h"
#include "adb_rb.h"
#include "adb_ev.h"
#include "adb_shell.h"
#include "misc.h"
#include "file_sync_service.h"
#include <sys/time.h>


static LIST_HEAD(g_adb_services);

static void *service_bootstrap_func(void *x)
{
	aservice *ser = x;

	adbd_debug("task handle:%p\n", rt_thread_self());
	ser->func(&ser->xfer_handle, (void *)&ser->cookie);
	/*free(ser);*/
	return NULL;
}

static void *create_subprocess(void *handle, const char *command, adb_thread_t *tid)
{
	shell_xfer_t *xfer = (shell_xfer_t *)handle;

#define SHELL_RB_SIZE	(1024*4)

	adbd_debug("");
	xfer->ev_handle = adb_event_init();
	xfer->rb_read_from_pc = adb_ringbuffer_init(256);
	xfer->rb_read_from_pc->xfer = xfer;
	xfer->rb_write_by_adb  = adb_ringbuffer_init(256);
	xfer->rb_write_by_adb->xfer = xfer;
	xfer->rb_write_by_shell = adb_ringbuffer_init(SHELL_RB_SIZE);
	xfer->rb_write_by_shell->xfer = xfer;
	xfer->schd = adb_schd_init(ADB_SCHD_READ|ADB_SCHD_ERROR);
	xfer->recv_close = 0;

	adb_shell_create(tid, command, (void *)xfer);
	adbd_info("thread(%p)\n", (void *)tid);
	/*adb_thread_create(tid, adb_shell_proc(command), "adbd-shell", (void *)xfer);*/
	return xfer;
}

static int shell_wait_ready(aservice *ser)
{
	shell_xfer_t *xfer = &ser->xfer_handle.shell_handle;
	int event;

	if (xfer->recv_close != 0) {
		adbd_info("shell xfer is recv_close");
		return -1;
	}
	event = adb_schd_wait(xfer->schd);
	if (event & ADB_SCHD_ERROR)
		return -1;
	return 0;
}

static void *shell_event_handle(void *arg)
{
	shell_xfer_t *xfer = (shell_xfer_t *)arg;
	aservice *ser = container_of((void *)xfer, struct aservice, xfer_handle);
	int bits;

	adbd_debug("");
	while (1) {
		adbd_debug("");
		bits = adb_event_get(xfer, NULL,
				ADB_EV_READ | ADB_EV_WRITE | ADB_EV_EXIT,
				0);
		adbd_debug("ADB EVENT GET bits:0x%x\n", bits);
		if (bits & ADB_EV_WRITE) {
#if 1
			int size, current = 0;
			int count = 3;
			apacket *p = get_apacket();
			while (1) {
				if (xfer->recv_close) {
					adbd_debug("get exit bit");
					if (!ser->has_send_close) {
						ser->has_send_close = 1;
						xfer->recv_close = 1;
						send_close(ser->localid, ser->remoteid, ser->transport);
					}
					return 0;
				}

				size = adb_ringbuffer_get(xfer->rb_write_by_shell,
						p->data + current, sizeof(p->data) - current, -1);
				/*adbd_err("size=%d", size);*/
				if (!size) {
					if (count-- < 0)
						break;
					continue;
				}
				count = 3;
				current += size;
			}
			if (!current) {
				put_apacket(p);
			} else {
				p->msg.command = A_WRTE;
				p->msg.arg0 = ser->localid;
				p->msg.arg1 = ser->remoteid;
				p->msg.data_length = current;
				if (!shell_wait_ready(ser))
					send_apacket(p, ser->transport);
				else
					put_apacket(p);
			}
#else
			while (1) {
				int size;
				apacket *p = get_apacket();
				adbd_err("");
				size = adb_ringbuffer_get(xfer->rb_write_by_shell,
						p->data, sizeof(p->data), -1);
				adbd_err("size=%d", size);
				if (!size) {
					put_apacket(p);
					break;
				}
				adbd_err("console write %u bytes", size);
			#if 0
				int aw_hexdump(const char *buf, int bytes);
				aw_hexdump(p->data, size);
			#endif
				/* send packet */
				adbd_debug("size=%u, data:0x%x,%x\n",
						size, p->data[0], p->data[1]);
				p->msg.command = A_WRTE;
				p->msg.arg0 = ser->localid;
				p->msg.arg1 = ser->remoteid;
				p->msg.data_length = size;
				shell_wait_ready(ser);
				send_apacket(p, ser->transport);
			}
#endif
		}
		if (bits & ADB_EV_READ) {
			adbd_debug("");
			while (1) {
				char buf[128];
				int size;
				size = adb_ringbuffer_get(xfer->rb_read_from_pc,
						buf, sizeof(buf), -1);
				adbd_debug("size:%d\n", size);
				if (!size) {
					break;
				}
				/* send to adb console */
				adb_ringbuffer_put(xfer->rb_write_by_adb, buf, size);
			}
		}
		if (bits & ADB_EV_EXIT || xfer->recv_close) {
			adbd_debug("get exit bit");
			if (!ser->has_send_close) {
				ser->has_send_close = 1;
				xfer->recv_close = 1;
				send_close(ser->localid, ser->remoteid, ser->transport);
			}
			break;
		}
		if (!bits) {
			adbd_err("somethine error, evnet return 0x%x", bits);
			break;
		}
	}
//	pthread_exit(NULL);
	return 0;
}

static void subproc_waiter_service(void *xfer_handle, void *cookie)
{
	shell_xfer_t *xfer = (shell_xfer_t *)xfer_handle;
	struct aser_cookie *c = (struct aser_cookie *)cookie;
	adb_thread_t command_tid = c->tid;
	adb_thread_t event_tid;

	if (adb_thread_create(&event_tid, shell_event_handle, "adb-event",
				(void *)xfer, ADB_THREAD_HIGH_PRIORITY) < 0) {
		adbd_err("thread create failed, return\n");
		return;
	}
	adbd_debug("wait for adb shell thread(%ld) exit...\n", command_tid);
#if 1
	while (1) {
		int ret  = 0;
		struct timeval tm;
		struct timespec ts;
		/*pthread_timedjoin_np implement with gettimeofday*/
		gettimeofday(&tm, NULL);
		/* 1s timeout */
		ts.tv_sec = tm.tv_sec + 1;
		ts.tv_nsec = tm.tv_usec * 1000;
		/*adbd_err("set timeout: sec=%lu, nsec=%lu", ts.tv_sec, ts.tv_nsec);*/
		ret = pthread_timedjoin_np(command_tid, NULL, &ts);		
		if (!ret) {
			command_tid = -1;
			break;
		}
		if (ret == -1) {
			adbd_err("pthread_timedjoin_np reutrn -1");
		}
		if (xfer->recv_close != 0) {
			adbd_debug("thread[%s] maybe alive!!!\n", c->command_name);
#ifdef ADBD_RECORD_ALIVE_THREAD
//			record_alive_thread_add(c->command_name);
#endif
			pthread_cancel(command_tid);
			break;
		}
	}
#else
	/* for test*/
	while (1) {
		sleep(1);
		if (xfer->recv_close != 0) {
			adbd_err("thread[%s] maybe alive!!!\n", c->command_name);
#ifdef ADBD_RECORD_ALIVE_THREAD
			record_alive_thread_add(c->command_name);
#endif
			break;
		}
	}
#endif
	adbd_info("adb shell thread exit...\n");
	adb_event_set(xfer->ev_handle, ADB_EV_EXIT);
	adb_schd_wakeup(xfer->schd, ADB_SCHD_TIMEOUT);	
	adbd_info("wait for adb shell event thread(%ld) exit...\n", event_tid);	
	adb_thread_wait(event_tid, NULL);
	adbd_info("adb shell event thread exit...\n");
	adbd_info("adbd waiter service exit\n");
	return;
}

static int shell_enqueue(aservice *ser, apacket *p)
{
	shell_xfer_t *xfer = &ser->xfer_handle.shell_handle;

	adbd_debug("len:%u, data:0x%x\n", p->msg.data_length, p->data[0]);
	if (p->data[0] == 0x7f) {
		/*TODO backspace */
		adbd_debug("receive data-0x7f, force convert to 0x08\n");
		p->data[0] = 0x08;
	}
	
	if (xfer->recv_close == 0)
	{
		adb_ringbuffer_put(xfer->rb_read_from_pc, p->data, p->msg.data_length);
		adb_event_set(xfer->ev_handle, ADB_EV_READ);
	}
	put_apacket(p);
	return 0;
}

static int shell_ready(aservice *ser)
{
	shell_xfer_t *xfer = &ser->xfer_handle.shell_handle;

	adbd_debug("");
	adb_schd_wakeup(xfer->schd, ADB_SCHD_READ);
	return 0;
}

/* TODO local id*/
static unsigned int gLocalID = 0;
static int create_shell_service(const char *name, aservice *ser)
{
	adb_thread_t tid,subtid;
	char thread_name[64];

	ser->func = subproc_waiter_service;
	ser->service_type = ADB_SERVICE_TYPE_SHELL;

	ser->enqueue = shell_enqueue;
	ser->ready = shell_ready;

	create_subprocess((void *)&ser->xfer_handle.shell_handle, name, &subtid);
	ser->cookie.tid = subtid;
	if (name != NULL)
		strncpy(ser->cookie.command_name, name, sizeof(ser->cookie.command_name)-1);

	adbd_debug("create_subprocess xfer_handle:%p, subtid:%p", &ser->xfer_handle, &ser->cookie);
	snprintf(thread_name, sizeof(thread_name), "adbd-shell-ser-%d", ser->localid);
	adb_thread_create(&tid, service_bootstrap_func, thread_name, ser, ADB_THREAD_NORMAL_PRIORITY);
	ser->service_tid = tid;
	adbd_debug("service thread started...");
	return 0;
}

static int sync_enqueue(aservice *ser, apacket *p)
{
	sync_xfer *xfer = &ser->xfer_handle.sync_handle;
	adbd_info("enqueue");
	adb_enqueue_event(xfer->queue_recv, ADB_EV_READ, p, 0);
	return 0;
}

static int sync_ready(aservice *ser)
{
	sync_xfer *xfer = &ser->xfer_handle.sync_handle;

	adbd_debug("");
	adb_schd_wakeup(xfer->schd, ADB_SCHD_READ);
	return 0;
}

static int create_sync_service(void (*func)(void *handle, void *cookie), aservice *ser)
{
	adb_thread_t tid;
	char thread_name[64];

	ser->func = func;
	ser->service_type = ADB_SERVICE_TYPE_SYNC;
	ser->xfer_handle.sync_handle.queue_recv = adb_queue_event_init();
	ser->xfer_handle.sync_handle.queue_send = adb_queue_init();
	ser->xfer_handle.sync_handle.schd = adb_schd_init(ADB_SCHD_READ|ADB_SCHD_ERROR);
	ser->xfer_handle.sync_handle.p = NULL;
	ser->xfer_handle.sync_handle.offset = 0;

	ser->enqueue = sync_enqueue;
	ser->ready = sync_ready;

	snprintf(thread_name, sizeof(thread_name), "adbd-sync-ser-%d", ser->localid);
	/*adb_thread_create_detach(&tid, service_bootstrap_func, thread_name, ser, ADB_THREAD_NORMAL_PRIORITY);*/
	adb_thread_create(&tid, service_bootstrap_func, thread_name, ser, ADB_THREAD_NORMAL_PRIORITY);
	ser->service_tid = tid;

	adbd_debug("service thread started...");
	return 0;
}
#ifdef ADB_SUPPORT_SOCKET_API
static int create_socket_loopback_service(void (*func)(void *handle, void *cookie),
					int port, aservice *ser)
{
	adb_thread_t tid;
	char thread_name[64];
	int s;

	ser->func = func;
	ser->service_type = ADB_SERVICE_TYPE_SOCKET;

	s = socket_loopback_client(port);
	if (s < 0) {
		return -1;
	}
	ser->xfer_handle.socket_handle.s = s;
	ser->xfer_handle.socket_handle.schd = adb_schd_init(ADB_SCHD_READ);

	ser->enqueue = socket_enqueue;
	ser->ready = socket_ready;

	snprintf(thread_name, sizeof(thread_name), "adbd-socket-ser-%d", ser->localid);
	adb_thread_create(&tid, service_bootstrap_func, thread_name, ser, ADB_THREAD_HIGH_PRIORITY);
	ser->service_tid = tid;

	adbd_debug("service thread started...");
	return 0;
}
#endif
aservice * adb_service_create(const char *name, unsigned int remoteid, atransport *t)
{
	int ret = -1;
	aservice *ser;

	ser = adb_malloc(sizeof(aservice));
	if (!ser)
		fatal("no memory");
	memset(ser, 0, sizeof(aservice));
	ser->transport = t;
	ser->remoteid = remoteid;
	ser->localid = ++gLocalID;
	ser->has_send_close = 0;
	INIT_LIST_HEAD(&ser->list);

	adbd_debug("adb shell name:%s\n", name);
	if (!strncmp(name, "reboot:efex", strlen("reboot:efex"))) {
		char command_str[64] = "reboot efex";
		ret = create_shell_service(command_str, ser);
	} else if (!strncmp(name, "shell:", 6)) {
		if (name[6]) {
			ret = create_shell_service(name + 6, ser);
		} else {
			ret = create_shell_service(NULL, ser);
		}
	} else if (!strncmp(name, "sync:", 5)) {
		ret = create_sync_service(file_sync_service, ser);
#ifdef ADB_SUPPORT_SOCKET_API
	} else if (!strncmp(name, "tcp:", 4)) {
		int port = atoi(name + 4);
		name = strchr(name + 4, ':');
		if (name == 0) {
			ret = create_socket_loopback_service(socket_recv_service, port, ser);
		} else {
			adbd_err("unknown name");
			ret = -1;
		}
#endif
	} else {
		adbd_err("unsupport command:%s\n", name);
		ret = -1;
	}
	if (ret != 0) {
		adb_free(ser);
		gLocalID--;
		return NULL;
	}
	list_add(&ser->list, &g_adb_services);
	return ser;
}

aservice *find_adb_service(unsigned int localid)
{
	aservice *ser = NULL;
	list_for_each_entry(ser, &g_adb_services, list) {
		if (ser->localid == localid) {
			return ser;
		}
	}
	return NULL;
}

void adb_service_destroy(unsigned int localid)
{
	aservice *ser = NULL;

	ser = find_adb_service(localid);
	if (!ser) {
		return;
	}
	list_del(&ser->list);

	switch (ser->service_type) {
	case ADB_SERVICE_TYPE_SHELL: {
			shell_xfer_t *xfer = &ser->xfer_handle.shell_handle;
			/*TODO kill thread(waiter,event_handle,adb_shell,subthread) */
			/*xTaskSetConsole(top_task, NULL);*/
#if 1
#else
			vCommandConsoleStop(xfer->console);
#endif

#if 0
			adbd_debug("kill shell command");
			pthread_cancel(ser->cookie.tid);
#endif
			xfer->recv_close = 1;
			adbd_debug("wait for shell waiter service exit...");
			cli_console_deinit(xfer->console);
//			pthread_cancel(ser->cookie.tid);
//			adb_thread_wait(ser->cookie.tid, NULL);
//			pthread_cancel(ser->service_tid);
			adb_thread_wait(ser->service_tid, NULL);
			adbd_debug("detect shell waiter service exit...");
			if (!ser->has_send_close) {
				send_close(ser->localid, ser->remoteid, ser->transport);
			}
			adbd_debug("");
			cli_console_destory(xfer->console);
			adb_event_release(xfer->ev_handle);
			adb_ringbuffer_release(xfer->rb_read_from_pc);
			adb_ringbuffer_release(xfer->rb_write_by_adb);
			adb_ringbuffer_release(xfer->rb_write_by_shell);
			adb_schd_release(xfer->schd);
		}
		break;
	case ADB_SERVICE_TYPE_SYNC: {
		sync_xfer *xfer = &ser->xfer_handle.sync_handle;
#if 0
		if (!ser->has_send_close)
			send_close(ser->localid, ser->remoteid, ser->transport);
#else
		adb_enqueue_event(xfer->queue_recv, ADB_EV_EXIT, NULL, 0);
		/* only used for do_recv(adb pull) */
		adbd_info("send schd to ensure do_recv exit...");
		adb_schd_wakeup(xfer->schd, ADB_SCHD_ERROR);
		adbd_info("wait for sync service exit...");
		pthread_cancel(ser->service_tid);
		adb_thread_wait(ser->service_tid, NULL);
		adbd_info("detect sync service exit...");
		if (!ser->has_send_close) {
			adbd_info("\n");
			send_close(ser->localid, ser->remoteid, ser->transport);
		}
#endif
		/* clear queue_recv buffer */
		while (1) {
			apacket *p = NULL;
			if (!adb_dequeue_event(xfer->queue_recv, NULL, (void **)&p, 1)) {
				if (p != NULL)
					put_apacket(p);
				continue;
			}
			break;
		}
		adb_queue_event_release(xfer->queue_recv);
		adb_queue_release(xfer->queue_send);
		adb_schd_release(xfer->schd);
		}
		break;
	case ADB_SERVICE_TYPE_SOCKET: {
		socket_xfer *xfer = &ser->xfer_handle.socket_handle;

		xfer->recv_close = 1;
		adbd_debug("wait for socket service exit...");
		pthread_cancel(ser->service_tid);
		adb_thread_wait(ser->service_tid, NULL);
		adbd_debug("detect socket service exit...");
		if (!ser->has_send_close) {
			send_close(ser->localid, ser->remoteid, ser->transport);
		}
		if (xfer->s > 0)
			close(xfer->s);
		adb_schd_release(xfer->schd);
	}
		break;
	default:
		adbd_err("unknown adb service type:%d\n", ser->service_type);
		break;
	}
	adb_free(ser);
	return;
}

void adb_service_destroy_all(void)
{
	aservice *ser = NULL, *tmp_ser = NULL;
	list_for_each_entry_safe(ser, tmp_ser, &g_adb_services, list) {
		adb_service_destroy(ser->localid);
	}
	return ;
}


