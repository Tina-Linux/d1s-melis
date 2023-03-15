/*
 * Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
 *
 * Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
 * the the people's Republic of China and other countries.
 * All Allwinner Technology Co.,Ltd. trademarks are used with permission.
 *
 * DISCLAIMER
 * THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
 * IF YOU NEED TO INTEGRATE THIRD PARTY'S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR
 * MPEGLA, ETC.) IN ALLWINNERS'SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE
 * TO OBTAIN ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES. ALLWINNER SHALL
 * HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
 * COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
 * YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY'S TECHNOLOGY.
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
#include <FreeRTOS.h>
#include <queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <awlog.h>
#include <hal_msgbox.h>

#include <console.h>

#define RECEIVE_QUEUE_LENGTH 16
#define RECEIVE_QUEUE_WAIT_MS 100

struct msgbox_demo {
	int remote_id;
	int read_ch;
	int write_ch;
	QueueHandle_t recv_queue;
};

static void print_help_msg(void)
{
	printf("\n");
	printf("USAGE:\n");
	printf("  hal_msgbox <REQUIRED_ARGUMENTS> [OPTIONS]\n");
	printf("\n");
	printf("REQUIRED_ARGUMENTS:\n");
	printf("  -E REMOTE_ID: specify remote id\n");
	printf("  -R READ_CH  : specify read channel\n");
	printf("  -W WRITE_CH : specify write channel\n");
	printf("OPTIONS:\n");
	printf("  -s MESSAGE  : send MESSAGE\n");
	printf("  -r          : receive messages\n");
	printf("  -t TIMEOUT  : exit in TIMEOUT seconds when receive\n");
	printf("e.g. (communicate with remote 0, use read channel 3 and write channel 3):\n");
	printf("  hal_msgbox -E 0 -R 3 -W 3 -s \"hello\" : send string \"hello\"\n");
	printf("  hal_msgbox -E 0 -R 3 -W 3 -r           : receive messages (default in 10 seconds)\n");
	printf("  hal_msgbox -E 0 -R 3 -W 3 -r -t 20     : receive messages in 20 seconds\n");
	printf("\n");
}

static int recv_callback(unsigned long data, void *private_data)
{
	printf("Receive callback (data: 0x%lx)\n", data);
	struct msgbox_demo *demo = private_data;
	BaseType_t ret = xQueueSendFromISR(demo->recv_queue, &data, NULL);
	if (ret == errQUEUE_FULL) {
		printf("recv_queue is full\n");
		return -1;
	}
	return 0;
}

static int cmd_hal_msgbox(int argc, char *argv[])
{
	int ret = 0;
	int c;

	struct msgbox_demo demo= {
		.remote_id = -1,
		.read_ch = -1,
		.write_ch = -1,
		.recv_queue = NULL,
	};
	struct msg_endpoint ept;

	TickType_t start_ticks, current_ticks;
	int do_send = 0;
	const char *data_send= NULL;
	int do_recv = 0;
	int timeout_sec = 10;
	uint32_t data_recv;

	if (argc <= 1) {
		print_help_msg();
		ret = -1;
		goto out;
	}

	while ((c = getopt(argc, argv, "hs:rt:E:W:R:")) != -1) {
		switch (c) {
		case 'h' :
			print_help_msg();
			ret = 0;
			goto out;
		case 'E':
			demo.remote_id = atoi(optarg);
			break;
		case 'R':
			demo.read_ch = atoi(optarg);
			break;
		case 'W':
			demo.write_ch = atoi(optarg);
			break;
		case 's':
			do_send = 1;
			data_send = optarg;
			break;
		case 'r':
			do_recv = 1;
			break;
		case 't':
			timeout_sec = atoi(optarg);
			break;
		default:
			print_help_msg();
			ret = -1;
			goto out;
		}
	}

	if (demo.remote_id < 0 || demo.read_ch < 0 || demo.write_ch < 0) {
		printf("Error. Please specify remote id, read channel and write channel\n");
		print_help_msg();
		ret = -1;
		goto out;
	}

	printf("remote id: %d, write channel: %d, read channel: %d\n",
			demo.remote_id, demo.write_ch, demo.read_ch);

	if (do_recv) {
		demo.recv_queue = xQueueCreate(RECEIVE_QUEUE_LENGTH, sizeof(uint32_t));
		if (!demo.recv_queue) {
			printf("Failed to create receive queue\n");
			ret = -1;
			goto out;
		}
		ept.rec = (void *)recv_callback;
		ept.private = &demo;
	}

	ret = hal_msgbox_alloc_channel(&ept, demo.remote_id, demo.read_ch, demo.write_ch);
	if (ret != 0) {
		printf("Failed to allocate msgbox channel\n");
		goto delete_recv_queue;
	}

	if (do_send) {
		ret = hal_msgbox_channel_send(&ept, (unsigned char *)data_send, strlen(data_send));
		if (ret != 0) {
			printf("Failed to send message\n");
			goto free_channel;
		}
	}

	if (do_recv) {
		printf("hal_msgbox will exit in %d seconds\n", timeout_sec);
		start_ticks = xTaskGetTickCount();
		printf("start_ticks: %u\n", start_ticks);

		while (1) {
			if (pdTRUE == xQueueReceive(demo.recv_queue, &data_recv,
						RECEIVE_QUEUE_WAIT_MS / portTICK_PERIOD_MS)) {
				printf("Received from queue: 0x%x\n", data_recv);
			}
			current_ticks = xTaskGetTickCount();
			if ((current_ticks - start_ticks) * portTICK_PERIOD_MS
					>= timeout_sec * 1000) {
				printf("current_ticks: %u\n", current_ticks);
				break;
			}
		}
	}

	printf("hal_msgbox exited\n");
	ret = 0;

free_channel:
	hal_msgbox_free_channel(&ept);
delete_recv_queue:
	if (do_recv) {
		vQueueDelete(demo.recv_queue);
	}
out:
	return ret;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_hal_msgbox, hal_msgbox, hal msgbox);

