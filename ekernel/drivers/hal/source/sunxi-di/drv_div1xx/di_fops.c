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

#include "sunxi_di.h"
#include "di_client.h"
#include "../common/di_debug.h"

#include <stdlib.h>
#include <string.h>

typedef int di_ioctl_t(struct di_client *c, void *data);

static unsigned int g_c_num = 0;
static void *private_data;
extern struct di_client *find_client_by_num(unsigned int client_num);
extern int di_client_destroy(void *client, void *data);

static unsigned int di_client_get_number(struct di_client *c, void *data);

struct di_ioctl_desc {
	unsigned int cmd;
	di_ioctl_t *func;
	const char *name;
};

#define DI_IOCTL_DEF(ioctl, _func)	\
	[DI_IOCTL_NR(ioctl)] = {		\
		.cmd = ioctl,			\
		.func = (di_ioctl_t *)_func,	\
		.name = #ioctl			\
	}

/* Ioctl table */
static const struct di_ioctl_desc di_ioctls[] = {
	DI_IOCTL_DEF(DI_IOC_GET_VERSION, di_client_get_version),
	DI_IOCTL_DEF(DI_IOC_SET_TIMEOUT, di_client_set_timeout),
	DI_IOCTL_DEF(DI_IOC_PROCESS_FB, di_client_process_fb),
	DI_IOCTL_DEF(DI_IOC_DESTROY, di_client_destroy),

	DI_IOCTL_DEF(DI_IOC_MEM_REQUEST, di_client_mem_request),
	DI_IOCTL_DEF(DI_IOC_MEM_RELEASE, di_client_mem_release),
	DI_IOCTL_DEF(DI_IOC_GET_CLIENT, di_client_get_number),
};

#define DI_IOCTL_COUNT	sizeof(di_ioctls) / sizeof(di_ioctls[0])

long sunxi_di_control(unsigned int cmd, void *arg)
{
	int ret = -EINVAL;
	struct di_client *c = NULL;
	unsigned int c_num = 0;  // di_client's number
	const struct di_ioctl_desc *ioctl = NULL;
	unsigned int nr = DI_IOCTL_NR(cmd);
	char stack_kdata[128];
	char *kdata = NULL;
	unsigned int in_size, out_size, ksize;

	in_size = out_size = ksize = _IOC_SIZE(cmd);

	if ((cmd & IOC_IN) == 0)
		in_size = 0;
	if ((cmd & IOC_OUT) == 0)
		out_size = 0;

	if (ksize <= sizeof(stack_kdata)) {
		kdata = stack_kdata;
	} else {
		kdata = hal_malloc(ksize);
		if (!kdata) {
			ret = -ENOMEM;
			goto err;
		}
	}

	if ((nr >= DI_IOCTL_COUNT)) {
		return -EINVAL;
	}

	ioctl = &di_ioctls[nr];
	if (!ioctl->func) {
		ret = -EINVAL;
		goto err;
	}

	if (in_size > 0) {
		memcpy(kdata, arg, in_size);
	}

	// Get client by client_number
	switch (cmd) {
	case DI_IOC_SET_TIMEOUT:{
		c_num = ((struct di_timeout_ns *)kdata)->client_number;
		break;
	}
	case DI_IOC_PROCESS_FB:{
		c_num = ((struct di_process_fb_arg *)kdata)->client_number;
		break;
	}
	case DI_IOC_DESTROY:{
		c_num = *(unsigned int *)kdata;
		break;
	}
	default:
		break;
	}

	if (DI_IOC_MEM_REQUEST != cmd && DI_IOC_MEM_RELEASE != cmd
			&& DI_IOC_GET_VERSION != cmd  && DI_IOC_GET_CLIENT != cmd ) {
		DI_INFO("client_number : %u \n ", c_num);
		c = find_client_by_num(c_num);

		if ((c == NULL)) {
			g_c_num = 0;
			return -EINVAL;
		}

		DI_INFO("[DEBUG] c->timeout.. %lu, %lu \n", c->timeout.wait4start, c->timeout.wait4finish);
	}

	ret = ioctl->func(c, (void *)kdata);
	if (!ret && out_size > 0)
		memcpy(arg, kdata, out_size);

err:
	if (kdata != stack_kdata)
		hal_free(kdata);

	if(cmd == DI_IOC_DESTROY)
	{
		g_c_num = 0;
    }
	return ret;
}

hal_spinlock_t num_lock;

static unsigned int client_num = 1;
unsigned int sunxi_di_open(void)
{
	void *client;
	char debug_name[64];
	unsigned int c_num;
	unsigned int num_lock_data;

/*	snprintf(debug_name, 64, "client_%u",
		task_pid_nr(current->group_leader));*/
	if(g_c_num)
	{
		return 0;
	}
	num_lock_data = hal_spin_lock_irqsave(&num_lock);
	c_num = client_num;
	g_c_num = client_num;
	snprintf(debug_name, 64, "client_%u",
		client_num++);
	hal_spin_unlock_irqrestore(&num_lock, num_lock_data);
	client = di_client_create(debug_name);
	if (client == NULL)
		return -EINVAL;
	private_data = client;
	//return c_num;
	return 0;
}

// The logic of release di driver is included in di_client_destroy
int sunxi_di_release(void)
{
	g_c_num = 0;
	return 0;
}

static unsigned int di_client_get_number(struct di_client *c, void *data)
{
		unsigned int *temp = (unsigned int *)data;
		*temp = g_c_num;
		return 0;
}