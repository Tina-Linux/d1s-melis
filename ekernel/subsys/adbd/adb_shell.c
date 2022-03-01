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
#include <msh.h>
#include <cli_console.h>
/*#include "finsh_parser.h"*/
#if 0
static void *thread_test(void *arg)
{
	shell_xfer_t *xfer = (shell_xfer_t *)arg;
	const char test_info[] =
		"abdsdfsadf\n"
		"1231124214\r\n"
		"123123123123\n";
#if 0
	while (1) {
		adbd_debug("");
		sleep(4);
		adb_event_set(xfer->ev_handle, ADB_EV_READ);
	}
#else
	adbd_debug("");
	adb_ringbuffer_put(xfer->rb_write, (void *)test_info, strlen(test_info) + 1);
	adb_event_set(xfer->ev_handle, ADB_EV_WRITE);
	adbd_debug("");
	adbd_debug("thread exit");
	adb_event_set(xfer->ev_handle, ADB_EV_EXIT);
#endif
}
#endif


typedef struct adb_shell_arg {
	char *command;
	void *arg;
} adb_shell_arg;

int adb_console_write(const void * buf, size_t len, void * private_data)
{
	shell_xfer_t *xfer = (shell_xfer_t *)private_data;
	int size, current = 0;

	while (current < len) {
		size = adb_ringbuffer_put(xfer->rb_write_by_shell, buf + current, len - current);
		adb_event_set(xfer->ev_handle, ADB_EV_WRITE);
		if (size == (len - current))
			break;
		current += size;
		int msleep(unsigned int msecs);
		msleep(2);
	}

	return len;
}

int adb_console_read(void * buf, size_t len, void * private_data)
{
	shell_xfer_t *xfer = (shell_xfer_t *)private_data;
	int size;

	size = adb_ringbuffer_get(xfer->rb_write_by_adb, buf, len, 1000);

	if (size <= 0)
		*(char *)buf = 0;
	return 1;
}

int adb_console_init(void *private_data)
{
	return 1;
}

int adb_console_deinit(void *private_data)
{
	/*shell_xfer_t *xfer = (shell_xfer_t *)private_data;*/
	return 1;
}

static device_console adb_console =
{
    .name = "adb-console",
    .write = adb_console_write,
    .read = adb_console_read,
    .init = adb_console_init,
    .deinit = adb_console_deinit
};

typedef void * TaskHandle_t;
static void *adb_shell_command(void *_arg)
{
	adb_shell_arg *arg = (adb_shell_arg *)_arg;
	shell_xfer_t *xfer = (shell_xfer_t *)arg->arg;
	TaskHandle_t handle;

	xfer->console = cli_console_create(&adb_console,
				"adb-cli-console", (void *)xfer);
	cli_console_init(xfer->console);
	handle = rt_thread_self();
	cli_task_set_console(handle, xfer->console);

    msh_exec(arg->command, strlen(arg->command));
	adb_free(arg->command);
	adb_free(arg);

	/*xTaskSetConsole(handle, get_default_console);*/
	adb_event_set(xfer->ev_handle, ADB_EV_WRITE);
	adb_event_set(xfer->ev_handle, ADB_EV_EXIT);
	//pthread_exit(NULL);
	return NULL;
}

void *finsh_thread_entry( void *Parameter );
static int adb_shell_interactive(adb_thread_t *tid, void *arg)
{
	shell_xfer_t *xfer = (shell_xfer_t *)arg;

	xfer->console = cli_console_create(&adb_console,
				"adb-cli-console", (void *)xfer);

#if 0
	cli_console_task_create(console, tid, ADB_THREAD_STACK_SIZE, ADB_THREAD_NORMAL_PRIORITY);
#else
	if(!cli_console_check_invalid(xfer->console))
	{
		return -1;
	}
	cli_console_init(xfer->console);
	adb_thread_create(tid, (adb_thread_func_t)finsh_thread_entry, "adb-shell",
			(void *)xfer->console, ADB_THREAD_NORMAL_PRIORITY);
	/*pthread_set_console(*tid, xfer->console);*/
	xfer->console->task = NULL;
	xfer->console->alive = 1;
#endif
	return 0;
}

void adb_shell_create(adb_thread_t *tid, const char *command, void *_arg)
{
#if 1
	if (command) {
		char thread_name[RT_NAME_MAX];
		memset(thread_name, 0, RT_NAME_MAX);
		adb_shell_arg *arg = adb_calloc(1, sizeof(adb_shell_arg));
		if (!arg)
			fatal("no memory");
		arg->arg = _arg;
		arg->command = adb_strdup(command);
		if (!arg->command)
			fatal("no memory");

		snprintf(thread_name, sizeof(thread_name), "%s", arg->command);
		adbd_debug("thread name:%s\n", thread_name);
		adb_thread_create(tid, adb_shell_command, thread_name,
					arg, ADB_THREAD_NORMAL_PRIORITY);
	} else {
		adb_shell_interactive(tid, _arg);
	}

#else
	adb_thread_create(tid, thread_test, "adbd-shell", _arg);
#endif
	return;
}
