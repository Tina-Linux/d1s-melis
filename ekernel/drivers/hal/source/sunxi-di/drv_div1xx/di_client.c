/*
 * Copyright (c) 2020-2031 Allwinnertech Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include "di_client.h"
#include "../common/di_utils.h"
#include "../common/di_debug.h"
#include "sunxi_di.h"
#include <stdlib.h>
#include <sys/time.h>

#define DI_TNR_BUF_ALIGN_LEN 16
#define DI_MD_BUF_ALIGN_LEN 32

extern int di_drv_get_version(struct di_version *version);
extern int di_drv_client_inc(struct di_client *c);
extern int di_drv_client_dec(struct di_client *c);
extern int di_drv_is_valid_client(struct di_client *c);
extern int di_drv_process_fb(struct di_client *c);
extern struct di_client *find_client_by_name(char *name);
extern void atomic_set(int *wait_con, int data);

static int di_client_alloc_mbuf(struct di_mapped_buf **mbuf, u32 size)
{
	struct di_mapped_buf *p = *mbuf;

	if (p != NULL) {
		u32 size_alloced = size;

		if (p->size_alloced != size_alloced) {
			di_dma_buf_unmap_free(p);
			p = NULL;
			*mbuf = NULL;
		} else {
			p->size_request = size;
		}
	}
	if (p == NULL) {
		p = di_dma_buf_alloc_map(size);
		if (p == NULL)
			return -ENOMEM;
	}
	memset((void *)p->vir_addr, 0, p->size_alloced);
	*mbuf = p;

	return 0;
}

static void di_client_free_mbuf(struct di_mapped_buf **mbuf)
{
	if ((*mbuf)->size_alloced) {
		di_dma_buf_unmap_free(*mbuf);
		*mbuf = NULL;
	}
}

static int di_client_setup_md_buf(struct di_client *c)
{
	return di_client_alloc_mbuf(&c->md_buf,
		DI_ALIGN(c->fb_arg.size.width, 2)
		* DI_ALIGN(c->fb_arg.size.height, 4));
}

static int di_client_init(struct di_client *c)
{
	DI_DEBUG("%s: %s\n", c->name, __func__);

	c->proc_fb_seqno = 0;
	atomic_set(&c->wait_con, DI_PROC_STATE_IDLE);

	return 0;
}

/*checking and correcting di_client para before running proccess_fb*/
int di_client_check_para(struct di_client *c, void *data)
{
	DI_DEBUG("%s: %s\n", c->name, __func__);

	if ((c->fb_arg.size.height == 0)
		|| ((c->fb_arg.size.height & 0x1) != 0)
		|| (c->fb_arg.size.width == 0)
		|| ((c->fb_arg.size.width & 0x1) != 0)) {
		DI_ERR("%s: invalid size W(%d)xH(%d)\n",
			c->name, c->fb_arg.size.height,
			c->fb_arg.size.width);
		goto err_out;
	}

	atomic_set(&c->wait_con, DI_PROC_STATE_IDLE);

	return 0;

err_out:
	return -EINVAL;
}

static bool di_client_check_fb_arg(struct di_client *c,
	struct di_process_fb_arg *fb_arg)
{
	DI_DEBUG("%s topFieldFirst[%s]\n",
		fb_arg->is_interlace ? "Interlace" : "P",
		fb_arg->field_order ? "Y" : "N");

	/* TODO: add more check ? */
	return true;
}

static int di_client_get_fb(struct di_client *c,
	struct di_dma_item **dma_fb_item, struct di_fb *fb)
{
	struct di_dma_item *dma_fb = *dma_fb_item;

	DI_DEBUG("%s: buf_offset[0x%lx,0x%lx,0x%lx],"
		"fd=%d,size[%d,%d]\n",
		c->name,
		fb->buf.offset.y_offset, fb->buf.offset.cb_offset,
		fb->buf.offset.cr_offset,
		fb->dma_buf_fd,
		fb->size.width, fb->size.height);

	if (fb->buf.addr.y_addr && fb->buf.addr.cb_addr) {
		DI_DEBUG("%s: On phy_addr_buf method\n", c->name);
	}

	*dma_fb_item = dma_fb;
	DI_DEBUG("\n");

	return 0;
}

static int di_client_get_fbs(struct di_client *c)
{
	struct di_process_fb_arg *fb_arg = &c->fb_arg;

	if (di_client_get_fb(c, &c->in_fb0,
			&fb_arg->in_fb0)) {
		DI_ERR("di_client_get_fb:in_fb0 failed\n");
		return -EINVAL;
	}
	if (di_client_get_fb(c, &c->in_fb1,
			&fb_arg->in_fb1)) {
		DI_ERR("di_client_get_fb:in_fb1 failed\n");
		return -EINVAL;
	}
	if (di_client_get_fb(c, &c->in_fb2,
			&fb_arg->in_fb2)) {
		DI_ERR("di_client_get_fb:in_fb2 failed\n");
		return -EINVAL;
	}
	if (di_client_get_fb(c, &c->out_fb0,
			&fb_arg->out_fb0)) {
		DI_ERR("di_client_get_fb:out_fb0 failed\n");
		return -EINVAL;
	}
	if (di_client_get_fb(c, &c->out_fb1,
			&fb_arg->out_fb1)) {
		DI_ERR("di_client_get_fb:out_fb1 failed\n");
		return -EINVAL;
	}

	if (fb_arg->tnr_mode.mode > 0) {
		if (di_client_get_fb(c, &c->out_tnr_fb,
				&fb_arg->out_tnr_fb)) {
			DI_ERR("di_client_get_fb:out_tnr_fb failed\n");
			return -EINVAL;
		}
	}

	return 0;
}

static bool di_client_workmode_is_change(struct di_client *c,
			struct di_process_fb_arg *fb_arg)
{
	struct di_process_fb_arg *current_fb_arg = &c->fb_arg;

	if ((fb_arg->is_interlace != current_fb_arg->is_interlace)
		|| (fb_arg->field_order != current_fb_arg->field_order)
		|| (fb_arg->pixel_format != current_fb_arg->pixel_format)
		|| (fb_arg->size.width != current_fb_arg->size.width)
		|| (fb_arg->size.height != current_fb_arg->size.height))
		return true;

	return false;
}


static void di_dump_fb_arg(struct di_process_fb_arg *arg)
{
	DI_INFO("is_interlace:%u field_prder:%u\n", arg->is_interlace, arg->field_order);
	DI_INFO("SIZE:(%ux%u) format:%u\n", arg->size.width, arg->size.height, arg->pixel_format);
	DI_INFO("DIMODE:%u output_mode:%u\n", arg->di_mode, arg->output_mode);
	DI_INFO("tnr:mode:%u level:%u\n", arg->tnr_mode.mode, arg->tnr_mode.level);

	DI_INFO("\ninFB0:\n");
	if (arg->in_fb0.dma_buf_fd > 0) {
		DI_INFO("dmafd:%d offset[%lu, %lu, %lu]\n",
			arg->in_fb0.dma_buf_fd,
			arg->in_fb0.buf.offset.y_offset,
			arg->in_fb0.buf.offset.cb_offset,
			arg->in_fb0.buf.offset.cr_offset);
	} else {
		DI_INFO("addr[%lu, %lu, %lu]\n",
			arg->in_fb0.buf.addr.y_addr,
			arg->in_fb0.buf.addr.cb_addr,
			arg->in_fb0.buf.addr.cr_addr);
	}

	DI_INFO("\ninFB1:\n");
	if (arg->in_fb1.dma_buf_fd > 0) {
		DI_INFO("dmafd:%d offset[%lu, %lu, %lu]\n",
			arg->in_fb1.dma_buf_fd,
			arg->in_fb1.buf.offset.y_offset,
			arg->in_fb1.buf.offset.cb_offset,
			arg->in_fb1.buf.offset.cr_offset);
	} else {
		DI_INFO("addr[%lu, %lu, %lu]\n",
			arg->in_fb1.buf.addr.y_addr,
			arg->in_fb1.buf.addr.cb_addr,
			arg->in_fb1.buf.addr.cr_addr);
	}

	DI_INFO("\noutFB0:\n");
	if (arg->out_fb0.dma_buf_fd > 0) {
		DI_INFO("dmafd:%d offset[%lu, %lu, %lu]\n",
			arg->out_fb0.dma_buf_fd,
			arg->out_fb0.buf.offset.y_offset,
			arg->out_fb0.buf.offset.cb_offset,
			arg->out_fb0.buf.offset.cr_offset);
	} else {
		DI_INFO("addr[%lu, %lu, %lu]\n",
			arg->out_fb0.buf.addr.y_addr,
			arg->out_fb0.buf.addr.cb_addr,
			arg->out_fb0.buf.addr.cr_addr);
	}

	DI_INFO("\noutFB1:\n");
	if (arg->out_fb1.dma_buf_fd > 0) {
		DI_INFO("dmafd:%d offset[%lu, %lu, %lu]\n",
			arg->out_fb1.dma_buf_fd,
			arg->out_fb1.buf.offset.y_offset,
			arg->out_fb1.buf.offset.cb_offset,
			arg->out_fb1.buf.offset.cr_offset);
	} else {
		DI_INFO("addr[%lu, %lu, %lu]\n",
			arg->out_fb1.buf.addr.y_addr,
			arg->out_fb1.buf.addr.cb_addr,
			arg->out_fb1.buf.addr.cr_addr);
	}
}

int di_client_process_fb(struct di_client *c,
	struct di_process_fb_arg *fb_arg)
{
	int ret = 0;
	bool is_change = false;
	struct timeval tv;
	long int t0 = 0, t1 = 0, t2 = 0, t3 = 0;

	di_dump_fb_arg(fb_arg);

	gettimeofday(&tv, NULL);
	t0 = tv.tv_usec;

	if (!di_client_check_fb_arg(c, fb_arg)) {
		DI_ERR("%s: check_fb_arg fail\n", c->name);
		return -EINVAL;
	}

	is_change = di_client_workmode_is_change(c, fb_arg);
	if (is_change) {
		DI_INFO("di workmode changes, init client\n");
		di_client_init(c);
	}

	memcpy((void *)&c->fb_arg, fb_arg, sizeof(c->fb_arg));

	if (is_change && (fb_arg->di_mode == DI_INTP_MODE_MOTION)) {
		DI_INFO("di workmode changes, creat and setup md flag buf\n");
		di_client_setup_md_buf(c);
	}
	ret = di_client_get_fbs(c);
	if (ret < 0) {
		DI_ERR("di_client_get_fbs failed!\n");
		return ret;
	}

	gettimeofday(&tv, NULL);
	t1 = tv.tv_usec;

	ret = di_drv_process_fb(c);

	gettimeofday(&tv, NULL);
	t2 = tv.tv_usec;

	gettimeofday(&tv, NULL);
	t3 = tv.tv_usec;


	DI_TEST("total:%ldus     t0~t1:%ldus  t1~t2:%ldus  t2~t3:%ldus\n",
		(t3 - t0),
		(t1 - t0),
		(t2 - t1),
		(t3 - t2));
	return ret;
}

int di_client_get_version(struct di_client *c,
	struct di_version *version)
{
	return di_drv_get_version(version);
}

int di_client_set_timeout(struct di_client *c,
	struct di_timeout_ns *timeout)
{
	DI_DEBUG("%s:wait4start=%lu,wait4finish=%lu\n",
		c->name, timeout->wait4start, timeout->wait4finish);
	if (timeout->wait4start > 0)
		c->timeout.wait4start = timeout->wait4start;
	if (timeout->wait4finish > 0)
		c->timeout.wait4finish = timeout->wait4finish;

	return 0;
}

void *di_client_create(const char *name)
{
	struct di_client *client;
	int name_len;
	if (!name) {
		DI_ERR("%s: Name cannot be null\n", __func__);
		return NULL;
	}

	client = hal_malloc(sizeof(*client));
	if (client == NULL) {
		DI_ERR("kzalloc for client%s fail\n", name);
		return NULL;
	}

	memset(client, 0, sizeof(*client));

	name_len = strlen(name) + 1;
	client->name = hal_malloc(name_len);

	if (client->name == NULL) {
		hal_free(client);
		DI_ERR("kstrdup for name(%s) fail\n", name);
		return NULL;
	}

	memset(client->name, 0, name_len);
	memcpy(client->name, name, name_len);

	INIT_LIST_HEAD(&client->node);

	client->timeout.wait4start = 3 * 1000;  // 3000 msec
	client->timeout.wait4finish = 3 * 1000;  // 3000 msec
	hal_wqueue_init(&client->wait);

	/*
	atomic_set(&client->wait_con, DI_PROC_STATE_IDLE);
	*/

	if (di_drv_client_inc(client)) {
		hal_free(client);
		return NULL;
	}

	return client;
}

int di_client_destroy(void *client, void *data)
{
	struct di_client *c = (struct di_client *)client;

	if (!di_drv_is_valid_client(c)) {
		DI_ERR("%s, invalid client(%p)\n", __func__, c);
		return -1;
	}
	di_drv_client_dec(c);

	if (c->md_buf)
		di_client_free_mbuf(&c->md_buf);

	hal_free(c->name);
	hal_free(c);

	return 0;
}

struct di_client *find_client_by_num(unsigned int client_num)
{
	char client_name[64] = {0};
	struct di_client *client_in_drvdata = NULL;
	snprintf(client_name, 64, "client_%u", client_num);

	// find client by name
	client_in_drvdata = find_client_by_name(client_name);

	return client_in_drvdata;
}

int di_client_mem_request(struct di_client *c, void *data)
{
	struct di_mem_arg *mem = (struct di_mem_arg *)data;
	int ret;

	ret = di_mem_request(mem->size, &mem->phys_addr);

	if (ret < 0)
		return -1;

	mem->handle = ret;

	return 0;
}

int di_client_mem_release(struct di_client *c, void *data)
{
	struct di_mem_arg *mem = (struct di_mem_arg *)data;

	return mem->handle = di_mem_release(mem->handle);
}
