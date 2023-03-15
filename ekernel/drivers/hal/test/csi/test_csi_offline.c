/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "FreeRTOS/_os_semaphore.h"
#include "FreeRTOS/_os_time.h"
#include "sunxi_hal_twi.h"
#include <fcntl.h>
#include <hal_cmd.h>
#include <hal_log.h>
#include <hal_thread.h>
#include "hal_csi_jpeg.h"
#include "jpegenc.h"

static int out_fmt = 0;
#define CSI_JPEG_IRQn	109  // rv
//#define CSI_JPEG_IRQn	93  // arm

static int read_frame(struct csi_ipeg_mem *csi_mem)
{
	FILE* fd;
	long long res;

	hal_disable_irq(CSI_JPEG_IRQn);  // when write bin, close irq, r/w too slow
	hal_log_info("%s line: %d addr = 0x%08x size = %d\n", __func__, __LINE__,
				csi_mem->buf.addr, csi_mem->buf.size);

	fd = fopen("/data/nv12.bin", "ab");
	if (fd < 0) {
		hal_log_info("open /data/nv12.bin error %d\n", fd);
		return -1;
	}

	res = fwrite(csi_mem->buf.addr, csi_mem->buf.size, 1, fd);
	if (res < 0) {
		hal_log_info("write fail(%d), line%d..\n", res, __LINE__);
		fclose(fd);
		return -1;
	}
	hal_log_info("write YUV image ok\n");

	fclose(fd);

	hal_enable_irq(CSI_JPEG_IRQn);	// after write bin, open irq, r/w too slow
	return 0;
}

static int read_jpg(struct csi_ipeg_mem *jpeg_mem)
{
	FILE* fd;
	long long res;
	void *addr;
	unsigned int size;
	char name[128];

	hal_disable_irq(CSI_JPEG_IRQn);  // cfm when write bin, close irq
	hal_log_info("%s line: %d addr = 0x%08x size = %d\n", __func__, __LINE__,
				jpeg_mem->buf.addr, jpeg_mem->buf.size);

	fd = fopen("/data/test.jpg", "ab");
	if (fd < 0) {
		hal_log_info("open /data/test_online.jpg error %d\n", fd);
		return -1;
	}

	addr = jpeg_mem->buf.addr - JPEG_HEADER_LEN;
	size = jpeg_mem->buf.size + JPEG_HEADER_LEN;

	res = fwrite(addr, size, 1, fd);
	if (res < 0) {
		hal_log_info("write fail(%d), line%d..\n", res, __LINE__);
		fclose(fd);
		return -1;
	}
	hal_log_info("write JPEG image ok\n");

	fclose(fd);
	hal_enable_irq(CSI_JPEG_IRQn);	// cfm after write bin, open irq

	return 0;
}

static void main_test()
{
	struct csi_jpeg_fmt fmt;
	unsigned int count;
	struct csi_ipeg_mem *csi_mem;
	struct csi_ipeg_mem *jpeg_mem;
	unsigned int test_count;
	unsigned int timeout_msec;

/*	for(int i = 0x8000000; i < 0x8400000; i += 4)
		*(int *)i = 0;
	hal_log_info("lpsram init set 0x0");
	hal_msleep(2000);
*/

	fmt.width = 640;
	fmt.height = 480;
	fmt.line_mode = OFFLINE_MODE;
	fmt.output_mode = PIX_FMT_OUT_MAX;
	hal_csi_jpeg_set_fmt(&fmt);

	count = 3;

	if (hal_csi_jpeg_reqbuf(count) != 0) {
		return ;
	}

	hal_csi_jpeg_s_stream(1);
	hal_log_info("csi stream on!");

	test_count = 200;
	timeout_msec = 2000;  // recommend 2s, 10s for pm test

	if (!out_fmt) {
		while (test_count-- > 0) {
			hal_log_info("test count = %d\n", test_count);
			csi_mem = hal_csi_dqbuf(csi_mem, timeout_msec);
			if (test_count == 1)
				read_frame(csi_mem);
			hal_csi_qbuf();
		}
	} else {
		while (test_count-- > 0) {
			hal_log_info("test count = %d\n", test_count);
			jpeg_mem = hal_jpeg_dqbuf(jpeg_mem, timeout_msec);
			if (test_count == 1)
				read_jpg(jpeg_mem);
			hal_jpeg_qbuf();
		}
	}

	hal_csi_jpeg_s_stream(0);
	hal_csi_jpeg_freebuf();
	hal_log_info("csi stream off!!\n");

}

struct rt_thread *thread;

static void csi_thread(void *data)
{
	hal_log_info("csi jpeg demo started\n");
	hal_csi_jpeg_probe();

	main_test();

	hal_csi_jpeg_remove();
	hal_log_info("csi jpeg demo over\n");

	kthread_stop(thread);

	return 0;
}

int cmd_csi_jpeg_offline_test(int argc, const char **argv)
{
	int ret;
	if (argc < 2)
	{
		hal_log_info("Usage: hal_csi_jpeg_offline num. num: 0 is nv12, 1 is jpeg\n");
	} else {
		out_fmt = strtol(argv[1], NULL, 0);
	}

	thread = kthread_create((void *)csi_thread, NULL, "csi_thread", 409600, HAL_THREAD_PRIORITY_SYS);
	kthread_start(thread);

	return 0;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_csi_jpeg_offline_test, hal_csi_jpeg_offline, csi jpeg offline encode test)

