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
#include "cj_platform_cfg.h"

/* Macro JPEG_MPART_ENABLE defined in jpegenc.h */
#if JPEG_MPART_ENABLE
#define JPEG_MPART_SIZE			(50*1024)
#endif

static int read_whole_jpg(struct csi_ipeg_mem *jpeg_mem, int order)
{
	FILE* fd;
	long long res;
	void *addr;
	int size;
	char name[128];

	hal_log_info("%s line: %d addr = 0x%08x size = %d\n", __func__, __LINE__,
				jpeg_mem->buf.addr, jpeg_mem->buf.size);

	sprintf(name, "/data/test_%d.jpg", order);
	fd = fopen(name, "ab");
	if (fd < 0) {
		hal_log_info("open /data/test.jpg error %d\n", fd);
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

	return 0;
}

static int read_part_jpg(void *addr, int size, int order)
{
	FILE* fd;
	long long res;
	char name[128];

	hal_log_info("%s line: %d addr = 0x%08x size = %d\n", __func__, __LINE__, addr, size);

	sprintf(name, "/data/test_%d.jpg", order);
	fd = fopen(name, "ab");
	if (fd < 0) {
		hal_log_info("open /data/test.jpg error %d\n", fd);
		return -1;
	}

	res = fwrite(addr, size, 1, fd);
	if (res < 0) {
		hal_log_info("write fail(%d), line%d..\n", res, __LINE__);
		fclose(fd);
		return -1;
	}
	hal_log_info("write JPEG image ok\n");

	fclose(fd);

	return 0;
}

#if JPEG_MPART_ENABLE
static uint8_t* gmpartaddr[3];
static uint32_t gmpartsize[3];
static void jpeg_mpart_cb(struct csi_ipeg_mem *jpeg_mem)
{
	static uint32_t offset = 0;
	static int index = 0;
	hal_dcache_clean_invalidate((unsigned long)jpeg_mem->buf.addr +
			jpeg_mem->mpart_info.buff_offset, jpeg_mem->mpart_info.size); /* necessary operation */
	memcpy(gmpartaddr[index] + offset, jpeg_mem->buf.addr + jpeg_mem->mpart_info.buff_offset,
			jpeg_mem->mpart_info.size);
	offset += jpeg_mem->mpart_info.size;
	if (jpeg_mem->mpart_info.tail) { /*  encode one jpeg finish */
		gmpartsize[index] = offset;
		offset = 0;
		index++;
		if (index > 3)
			index = 0;
	}
}
#endif

static void main_test()
{
	struct csi_jpeg_fmt fmt;
	unsigned int count, i;
	struct csi_ipeg_mem *csi_mem;
	struct csi_ipeg_mem *jpeg_mem[3];
	unsigned int test_count;
	unsigned int timeout_msec;
	unsigned int j = 0;

/*	for(int i = 0x8000000; i < 0x8400000; i += 4)
		*(int *)i = 0;
	hal_log_info("lpsram init set 0x0");
	hal_msleep(2000);
*/

	fmt.width = 640;
	fmt.height = 480;
	fmt.line_mode = ONLINE_MODE;
	fmt.output_mode = PIX_FMT_OUT_MAX;
#if JPEG_MPART_ENABLE
	fmt.cb = &jpeg_mpart_cb;
#endif
	hal_csi_jpeg_set_fmt(&fmt);

	count = 3;

	if (hal_csi_jpeg_reqbuf(count) != 0) {
		return;
	}

	test_count = 3;

	hal_csi_jpeg_s_stream(1);

	hal_log_info("csi stream on!");

	timeout_msec = 2000;

#if JPEG_MPART_ENABLE
	for (i = 0; i < count; i++) {
		gmpartaddr[i] = malloc(JPEG_MPART_SIZE);
		hal_log_info("jpeg pic addr = %x\n", gmpartaddr[i]);
		memset(gmpartaddr[i], 0 , JPEG_MPART_SIZE);
	}
#endif

	while (test_count-- > 0) {
		jpeg_mem[j] = hal_jpeg_dqbuf(jpeg_mem[j], timeout_msec);
		hal_jpeg_qbuf();
		j++;
		if (j > count)
			j = 0;
	}
	hal_disable_irq(CSI_JPEG_IRQn);

#if JPEG_MPART_ENABLE
	for (i = 0; i < count; i++) {
		read_part_jpg(jpeg_mem[i]->buf.addr - JPEG_HEADER_LEN, JPEG_HEADER_LEN, i);
		read_part_jpg(gmpartaddr[i], gmpartsize[i], i);
		free(gmpartaddr[i]);
	}
#else
	for (i = 0; i < count; i++)
		read_whole_jpg(jpeg_mem[i], i);
#endif

	hal_csi_jpeg_s_stream(0);
	hal_csi_jpeg_freebuf();
	hal_log_info("csi stream off!!\n");

}

int cmd_csi_jpeg_online_test(int argc, const char **argv)
{
	hal_log_info("csi jpeg demo started\n");
	hal_csi_jpeg_probe();

	main_test();

	hal_csi_jpeg_remove();
	hal_log_info("csi jpeg demo over\n");

	return 0;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_csi_jpeg_online_test, hal_csi_jpeg_online, csi jpeg online encode test)
