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
#include <hal_mem.h>
#include <hal_cache.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <interrupt.h>
#include <init.h>
#include <hal_clk.h>
#include <hal_reset.h>
#include <hal_timer.h>
#include <melis/standby/standby.h>

#include "../common/di_utils.h"
#include "../common/di_debug.h"
#include "di_driver.h"
#include "di_fops.h"
#include "di_dev.h"
#include <aw_list.h>

#define DI_MODULE_NAME "deinterlace"

#define DI_VERSION_MAJOR 1
#define DI_VERSION_MINOR 0
#define DI_VERSION_PATCHLEVEL 0

#define SUNXI_DI_START 0x05400000

#define SUNXI_IRQ_DI 104

#define IS_ERR_OR_NULL(pointer) (pointer == NULL)

int debug_mask = DEBUG_LEVEL_INFO;
static struct di_driver_data *di_drvdata;
static unsigned int di_debug_mode;

hal_spinlock_t atomic_lock;

void atomic_set(int *wait_con, int data)
{
	u32 lock_data;

	lock_data = hal_spin_lock_irqsave(&atomic_lock);
	*wait_con = data;
	hal_spin_unlock_irqrestore(&atomic_lock, lock_data);
}

u32 atomic_read(int *wait_con)
{
	u32 lock_data;
	int result;

	lock_data = hal_spin_lock_irqsave(&atomic_lock);
	result = *wait_con;
	hal_spin_unlock_irqrestore(&atomic_lock, lock_data);
	return result;
}

unsigned int di_device_get_debug_mode(void)
{
	return di_debug_mode;
}

int di_drv_get_version(struct di_version *version)
{
	if (version) {
		version->version_major = DI_VERSION_MAJOR;
		version->version_minor = DI_VERSION_MINOR;
		version->version_patchlevel = DI_VERSION_PATCHLEVEL;
		version->ip_version = di_dev_get_ip_version();
		return 0;
	}
	return -EINVAL;
}

int di_mutex_lock(hal_sem_t sem)
{
	return hal_sem_wait(sem);
}

int di_mutex_unlock(hal_sem_t sem)
{
	return hal_sem_post(sem);
}

static int di_init_hw(struct di_driver_data *drvdata)
{
	di_dev_set_reg_base(drvdata->reg_base);
	return 0;
}

static int di_clk_enable(struct di_driver_data *drvdata)
{
	if (!IS_ERR_OR_NULL(drvdata->iclk)) {
		int ret;

		ret = hal_clock_enable(drvdata->clk_source);
		ret = hal_clk_set_rate(drvdata->iclk, 300000000);
		ret = hal_clock_enable(drvdata->iclk);

		if (ret) {
			DI_ERR("try to enable di clk failed!\n");
			return ret;
		}

		if (!IS_ERR_OR_NULL(drvdata->clk_bus))
			hal_clock_enable(drvdata->clk_bus);

		if (!IS_ERR_OR_NULL(drvdata->rst_bus_di))
			hal_reset_control_deassert(drvdata->rst_bus_di);
	} else {
		DI_INFO("di clk handle is invalid for enable\n");
	}
	return 0;
}

static int di_clk_disable(struct di_driver_data *drvdata)
{
	if (!IS_ERR_OR_NULL(drvdata->iclk)) {
		hal_clock_disable(drvdata->iclk);
		hal_clock_disable(drvdata->clk_source);
		if (!IS_ERR_OR_NULL(drvdata->rst_bus_di))
			hal_reset_control_assert(drvdata->rst_bus_di);

	} else {
		DI_INFO("di clk handle is invalid!\n");
	}

	return 0;
}

static int di_check_enable_device_locked(
	struct di_driver_data *drvdata)
{
	int ret = 0;

	DI_DEBUG("client_cnt=%d, pm_state=%d, dev_en=%d\n",
		drvdata->client_cnt, drvdata->pm_state, drvdata->dev_enable);

	if (drvdata->pm_state == DI_PM_STATE_SUSPEND)
		return 0;

	if ((drvdata->client_cnt > 0)
		&& (drvdata->dev_enable == false)) {
		ret = di_clk_enable(drvdata);
		if (ret)
			return ret;
		drvdata->dev_enable = true;
	} else if ((drvdata->client_cnt == 0)
		&& (drvdata->dev_enable == true)) {
		ret = di_clk_disable(drvdata);
		if (ret)
			return ret;
		drvdata->dev_enable = false;
	} else if (drvdata->client_cnt < 0) {
		DI_ERR("err client_cnt=%d\n", drvdata->client_cnt);
		return -EINVAL;
	}

	return 0;
}

bool di_drv_is_valid_client(struct di_client *c)
{
	struct di_driver_data *drvdata = di_drvdata;
	struct di_client *client;
	bool valid = false;

	if (c) {
		di_mutex_lock(drvdata->mlock);
		list_for_each_entry(client, &drvdata->clients, node) {
			if (client == c) {
				valid = true;
				break;
			}
		}
		di_mutex_unlock(drvdata->mlock);
	}

	if (!valid) {
		DI_ERR("invalid client[0x%p]\n", c);
	}

	return valid;
}

int di_drv_client_inc(struct di_client *c)
{
	struct di_driver_data *drvdata = di_drvdata;
	int client_cnt;

	di_mutex_lock(drvdata->mlock);
	client_cnt = drvdata->client_cnt + 1;

	if (client_cnt > DI_CLIENT_CNT_MAX) {
		di_mutex_unlock(drvdata->mlock);
		DI_ERR("%s: %d > max_clients[%d]\n",
			__func__, client_cnt, DI_CLIENT_CNT_MAX);
		return -EINVAL;
	}

	drvdata->client_cnt = client_cnt;
	list_add_tail(&c->node, &drvdata->clients);
	di_check_enable_device_locked(drvdata);
	di_mutex_unlock(drvdata->mlock);

	return 0;
}

int di_drv_client_dec(struct di_client *c)
{
	struct di_driver_data *drvdata = di_drvdata;
	int client_cnt;

	di_mutex_lock(drvdata->mlock);
	list_del(&c->node);

	client_cnt = drvdata->client_cnt;
	if (client_cnt > 0) {
		drvdata->client_cnt--;
		di_check_enable_device_locked(drvdata);
	} else {
		di_mutex_unlock(drvdata->mlock);
		DI_INFO("%s:client_cnt=%d\n", __func__, client_cnt);
		return -EINVAL;
	}

	di_mutex_unlock(drvdata->mlock);

	return 0;
}

static int wait_event_interruptible_hrtimeout(hal_wqueue_t *wait, bool condition, int msec_timeout)
{
	if (condition) {
		return 0;
	} else {
		return hal_wqueue_wait(wait, 0, msec_timeout);
	}
}

static int di_drv_wait2start(
	struct di_driver_data *drvdata, struct di_client *c)
{
	const u64 wait2start = c->timeout.wait4start;
	long ret = 0;
	u32 flags;
	u32 id;
	u32 wait_con = 0;

	flags = hal_spin_lock_irqsave(&drvdata->queue_lock);

	if (drvdata->task_cnt >= DI_TASK_CNT_MAX) {
		hal_spin_unlock_irqrestore(&drvdata->queue_lock, flags);
		DI_ERR("too many tasks %d\n", drvdata->task_cnt);
		return -EBUSY;
	}

	id = (drvdata->front + drvdata->task_cnt) % DI_TASK_CNT_MAX;
	drvdata->queue[id] = c;
	drvdata->task_cnt++;
	if (drvdata->state == DI_DRV_STATE_IDLE) {
		drvdata->state = DI_DRV_STATE_BUSY;
		atomic_set(&c->wait_con, DI_PROC_STATE_2START);
		hal_spin_unlock_irqrestore(&drvdata->queue_lock, flags);
		return 0;
	}

	atomic_set(&c->wait_con, DI_PROC_STATE_WAIT2START);

	hal_spin_unlock_irqrestore(&drvdata->queue_lock, flags);

	ret = wait_event_interruptible_hrtimeout(&c->wait,
		atomic_read(&c->wait_con) == DI_PROC_STATE_2START,
		(int)wait2start);

	if (atomic_read(&c->wait_con) != DI_PROC_STATE_2START) {
		flags = hal_spin_lock_irqsave(&drvdata->queue_lock);
		wait_con = atomic_read(&c->wait_con); /* check-again */
		if (wait_con != DI_PROC_STATE_2START) {
			drvdata->queue[id] = NULL;
			drvdata->task_cnt--;
			hal_spin_unlock_irqrestore(&drvdata->queue_lock, flags);
			DI_ERR("wait2start(%luns) fail, con=%u, ret(%ld)\n",
				wait2start, wait_con, ret);
			return -ETIMEDOUT;
		}
		hal_spin_unlock_irqrestore(&drvdata->queue_lock, flags);
	}

	return 0;
}

static int di_drv_wait4finish(
	struct di_driver_data *drvdata, struct di_client *c)
{
	long ret = 0;
	u32 flags;
	int wait_con = 0;
	const u64 wait4finish = c->timeout.wait4finish;

	ret = wait_event_interruptible_hrtimeout(&c->wait,
		atomic_read(&c->wait_con) != DI_PROC_STATE_WAIT4FINISH,
		(int)wait4finish);

	if (atomic_read(&c->wait_con) != DI_PROC_STATE_FINISH) {
		flags = hal_spin_lock_irqsave(&drvdata->queue_lock);
		// wait_con = atomic_read(&c->wait_con); /* check-again */
		if (wait_con == DI_PROC_STATE_WAIT4FINISH) {
			di_dev_query_state_with_clear();
			drvdata->queue[drvdata->front] = NULL;
			drvdata->front = (drvdata->front + 1) % DI_TASK_CNT_MAX;
			drvdata->task_cnt--;
			drvdata->state = DI_DRV_STATE_IDLE;
		}
		hal_spin_unlock_irqrestore(&drvdata->queue_lock, flags);

		if (wait_con == DI_PROC_STATE_WAIT4FINISH) {
			DI_ERR("wait4finish(%luns) timeout, ret=%ld\n",
				wait4finish, ret);
			return ret ? ret : -ETIME;
		} else if (wait_con != DI_PROC_STATE_FINISH) {
			DI_ERR("wait4finish(%luns) err, ret=%ld, con=%u\n",
				wait4finish, ret, wait_con);
			return ret ? ret : -wait_con;
		}
	}

	DI_DEBUG("Processing frame %lu\n", c->proc_fb_seqno);
	c->proc_fb_seqno++;
	return 0;
}

/* caller must make sure c is valid */
int di_drv_process_fb(struct di_client *c)
{
	struct di_driver_data *drvdata = di_drvdata;
	int ret = 0;
	u32 flags;
	int i;

	ret = di_drv_wait2start(drvdata, c);

	if (ret)
		return ret;

	flags = hal_spin_lock_irqsave(&drvdata->queue_lock);
	atomic_set(&c->wait_con, DI_PROC_STATE_WAIT4FINISH);
	ret = di_dev_apply(c);

	hal_spin_unlock_irqrestore(&drvdata->queue_lock, flags);
	ret |= di_drv_wait4finish(drvdata, c);

	return ret;
}

static irqreturn_t di_irq_handler(int irq, void *dev_id)
{
	struct di_driver_data *drvdata = dev_id;
	u32 flags;
	struct di_client *c;
	int wait_con;
	u32 hw_state;

	if (irq != drvdata->irq_no) {
		DI_ERR("invalid irq no:%d %d\n", irq, drvdata->irq_no);
		return IRQ_NONE;
	}

	flags = hal_spin_lock_irqsave(&drvdata->queue_lock);

	hw_state = di_dev_query_state_with_clear();

	if (drvdata->task_cnt == 0) {
		DI_ERR("NO task count\n");
		hal_spin_unlock_irqrestore(&drvdata->queue_lock, flags);
		goto irq_out;
	}

	c = drvdata->queue[drvdata->front];
	wait_con = atomic_read(&c->wait_con);
	if (wait_con == DI_PROC_STATE_WAIT4FINISH) {
		DI_DEBUG("hw state : %u \n", hw_state);
		if (hw_state)
			atomic_set(&c->wait_con, DI_PROC_STATE_FINISH);
		else
			atomic_set(&c->wait_con, DI_PROC_STATE_FINISH_ERR);

		hal_wqueue_wakeup(&c->wait, NULL);

		drvdata->queue[drvdata->front] = NULL;
		drvdata->task_cnt--;
		drvdata->state = DI_DRV_STATE_IDLE;

		if (drvdata->task_cnt == 0)
			goto irq_out;

		drvdata->front = (drvdata->front + 1) % DI_TASK_CNT_MAX;
		c = drvdata->queue[drvdata->front];
		wait_con = atomic_read(&c->wait_con);
	}

	if (wait_con == DI_PROC_STATE_WAIT2START) {
		atomic_set(&c->wait_con, DI_PROC_STATE_2START);
		drvdata->state = DI_DRV_STATE_BUSY;
		hal_wqueue_wakeup(&c->wait, NULL);
	}

irq_out:
	hal_spin_unlock_irqrestore(&drvdata->queue_lock, flags);

	return IRQ_HANDLED;
}

/* unload resources of di device */
static void di_unload_resource(struct di_driver_data *drvdata)
{
	if (drvdata->irq_no != 0) {
		DI_INFO("maybe should ummap irq[%d]...\n", drvdata->irq_no);
	}

	if (!IS_ERR_OR_NULL(drvdata->clk_source))
		hal_clock_disable(drvdata->clk_source);
	if (!IS_ERR_OR_NULL(drvdata->iclk))
		hal_clock_disable(drvdata->iclk);
}

/* parse and load resources of di device */
static int di_parse_dt(struct di_driver_data *drvdata)
{
	int ret = 0;
	hal_clk_type_t clk_type;
	hal_clk_type_t clk_bus;
	hal_clk_type_t clk_parent;
	hal_reset_type_t reset_type;

	/* clk */
	clk_type = HAL_SUNXI_CCU;
	drvdata->iclk = hal_clock_get(clk_type, CLK_DI);

	reset_type = HAL_SUNXI_RESET;
	u32 reset_id = RST_BUS_DI;
	drvdata->rst_bus_di = hal_reset_control_get(reset_type, reset_id);

	clk_bus = HAL_SUNXI_CCU;
	drvdata->clk_bus = hal_clock_get(clk_bus, CLK_BUS_DI);

	clk_parent = HAL_SUNXI_CCU;
	drvdata->clk_source = hal_clock_get(clk_parent, CLK_PLL_PERIPH0_2X);

	hal_clk_set_parent(drvdata->iclk, drvdata->clk_source);

	/* irq */
	drvdata->irq_no = SUNXI_IRQ_DI;
	ret = request_irq(drvdata->irq_no, di_irq_handler, 0, "di", drvdata);
	if (ret) {
		DI_ERR("devm_request_irq failed\n");
		goto err_out;
	}

	enable_irq(SUNXI_IRQ_DI);

	DI_DEBUG("di irq_no=%u\n", drvdata->irq_no);

	/* reg */
	drvdata->reg_base = (void *) SUNXI_DI_START;
	DI_DEBUG("di reg_base=0x%p\n", drvdata->reg_base);

err_out:
	return ret;
}

struct di_client *find_client_by_name(char *name)
{
	struct di_driver_data *drvdata = di_drvdata;
	struct di_client *client;
	bool valid = false;

	if (name) {
		di_mutex_lock(drvdata->mlock);
		list_for_each_entry(client, &drvdata->clients, node) {
			if (!strcmp(client->name, name)) {
				di_mutex_unlock(drvdata->mlock);
				return client;
			}
		}
		di_mutex_unlock(drvdata->mlock);
	}

	DI_ERR("client[%s] is not found in drvdata->clients\n", name);

	return NULL;

}

#ifdef CONFIG_STANDBY
/**
 * @desc      This function suspend the di
 * @param     null
 */
int di_suspend(void *param)
{
        struct di_driver_data *drvdata = di_drvdata;

        if (drvdata->state == DI_DRV_STATE_BUSY) {
                DI_INFO(TAG"drv busy on suspend !\n");
	}

        di_mutex_lock(drvdata->mlock);
        drvdata->pm_state = DI_PM_STATE_SUSPEND;
        if (drvdata->dev_enable == true) {
                if (di_clk_disable(drvdata))
                        drvdata->dev_enable = false;
        }
        di_mutex_unlock(drvdata->mlock);
        return 0;
}

/**
 * @desc     This function resume the di
 * @param    null
 */
static int di_resume(void *param)
{
		struct di_driver_data *drvdata = di_drvdata;

		di_mutex_lock(drvdata->mlock);
		if (drvdata->client_cnt > 0) {
				if (di_clk_enable(drvdata))
						drvdata->dev_enable = true;
		}
		drvdata->pm_state = DI_PM_STATE_RESUME;
		di_mutex_unlock(drvdata->mlock);
		return 0;
}

static void di_register_pm_dev_notify(void)
{
		register_pm_dev_notify(di_suspend, di_resume, NULL);
}
#else
static void di_register_pm_dev_notify(void)
{
}
#endif

int di_probe(void)
{
	int ret = 0;
	struct di_driver_data *drvdata = NULL;
	int value;

	drvdata = hal_malloc(sizeof(*drvdata));
	if (drvdata == NULL) {
		DI_ERR("malloc for drvdata failed\n");
		return -ENOMEM;
	}

	memset(drvdata, 0, sizeof(*drvdata));

	ret = di_parse_dt(drvdata);
	if (ret)
		goto probe_done;

	hal_reset_control_deassert(drvdata->rst_bus_di);

	ret = di_init_hw(drvdata);
	if (ret)
		goto probe_done;

	value = 1;
	drvdata->mlock = hal_sem_create(value);

	INIT_LIST_HEAD(&drvdata->clients);

	di_drvdata = drvdata;

	do {
		struct di_version version;

		di_drv_get_version(&version);
		pr_info("version[%d.%d.%d], ip=0x%x\n",
			version.version_major,
			version.version_minor,
			version.version_patchlevel,
			version.ip_version);
	} while (0);

	di_register_pm_dev_notify();

	return 0;

probe_done:
	if (ret) {
		// di_dev_exit();
		di_unload_resource(drvdata);
		free(drvdata);
		// dev_err(&pdev->dev, "probe failed, errno %d!\n", ret);
	}

	return ret;

}

