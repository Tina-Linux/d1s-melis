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

#include "sunxi_wuptimer.h"
#include "platform_timer.h"
#include "aw_common.h"
#include <hal_clk.h>
#include <stdlib.h>
#include <hal_interrupt.h>
#ifdef CONFIG_COMPONENTS_PM
#include <pm_devops.h>
#endif

#ifdef CONFIG_DRIVERS_WAKEUP_TIMER

#ifdef CONFIG_COMPONENTS_PM
static u32 regs_addr[] = {
	TIMER_IRQ_EN_REG,
	TIMER_IRQ_ST_REG,
	TIMER_CTL_REG(0),
	TIMER_INTVAL_REG(0),
	TIMER_CTL_REG(1),
	TIMER_INTVAL_REG(1),
};
static u32 regs_backup[ARRAY_SIZE(regs_addr)];
#endif

static struct sunxi_timer g_timer[SUNXI_TMR_NUM];

static hal_irqreturn_t sunxi_timer_irq_handle(void *dev)
{
	struct sunxi_timer *timer = (struct sunxi_timer *)dev;

	/* clear pending */
	hal_writel((0x1 << timer->timer_id), (unsigned long)TIMER_IRQ_ST_REG);

	/*callback*/
	if (timer->callback != NULL)
	{
		timer->callback(timer->param);
	}

	return 0;
}

static void sunxi_wuptimer_sync(uint32_t timer)
{
	uint32_t old = hal_readl((unsigned long)TIMER_CNTVAL_REG(timer));

	while ((old - hal_readl((unsigned long)TIMER_CNTVAL_REG(timer))) < TIMER_SYNC_TICKS)
	{
		int i = 10;
		while (i--);
		break;
	}
}

void sunxi_wuptimer_stop(uint32_t timer)
{
	uint32_t val = hal_readl((unsigned long)TIMER_CTL_REG(timer));

	hal_writel(val & ~TIMER_CTL_ENABLE, (unsigned long)TIMER_CTL_REG(timer));

	sunxi_wuptimer_sync(timer);
}

void sunxi_wuptimer_start(uint32_t timer, bool periodic)
{
	uint32_t val = hal_readl((unsigned long)TIMER_CTL_REG(timer));

	if (periodic)
	{
		val &= ~TIMER_CTL_ONESHOT;
	}
	else
	{
		val |= TIMER_CTL_ONESHOT;
	}

	val |= TIMER_CTL_CLK_PRES(0);           //24M
	val &= ~TIMER_CTL_CLK_SRC(0x3);
	val |= TIMER_CTL_CLK_SRC(TIMER_CTL_CLK_SRC_OSC24M);

	hal_writel(val | TIMER_CTL_ENABLE | TIMER_CTL_RELOAD, (unsigned long)TIMER_CTL_REG(timer));
}

static void sunxi_wuptimer_setup(uint32_t tick, uint32_t timer)
{
	hal_writel(tick, (unsigned long)TIMER_INTVAL_REG(timer));
}

int sunxi_wuptimer_set_oneshot(uint32_t delay_us, uint32_t timer, timer_callback callback, void *callback_param)
{
	uint32_t tick = delay_us * 24;

	if (tick < g_timer[timer].min_delta_ticks || tick > g_timer[timer].max_delta_ticks)
	{
		return -1;
	}

	if (callback != NULL)
	{
		g_timer[timer].callback = callback;
		g_timer[timer].param = callback_param;
	}

	sunxi_wuptimer_stop(timer);

	sunxi_wuptimer_setup(tick, timer);

	sunxi_wuptimer_start(timer, false);

	return 0;
}

int sunxi_wuptimer_set_periodic(uint32_t delay_us, uint32_t timer, timer_callback callback, void *callback_param)
{
	uint32_t tick = delay_us * 24;

	if (tick < g_timer[timer].min_delta_ticks || tick > g_timer[timer].max_delta_ticks)
	{
		return -1;
	}

	if (callback != NULL)
	{
		g_timer[timer].callback = callback;
		g_timer[timer].param = callback_param;
	}

	sunxi_wuptimer_stop(timer);

	sunxi_wuptimer_setup(tick, timer);

	sunxi_wuptimer_start(timer, true);

	return 0;
}

#ifdef CONFIG_COMPONENTS_PM
static inline void save_regs(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(regs_addr); i++)
		regs_backup[i] = readl(regs_addr[i]);
}

static inline void restore_regs(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(regs_addr); i++)
		writel(regs_backup[i], regs_addr[i]);
}
static int hal_wuptimer_suspend(struct pm_device *dev, suspend_mode_t mode)
{
	uint32_t timer = (uint32_t)dev->data;
	save_regs();
	sunxi_wuptimer_stop(timer);
}

static int hal_wuptimer_resume(struct pm_device *dev, suspend_mode_t mode)
{
	uint32_t timer = (uint32_t)dev->data;
	restore_regs();
	sunxi_wuptimer_start(timer, 0);

	return 0;
}

struct pm_devops pm_wuptimer_ops = {
	.suspend = hal_wuptimer_suspend,
	.resume = hal_wuptimer_resume,
};
#endif

void sunxi_wuptimer_init(hal_timer_id_t id)
{
	int ret;
	uint32_t val;
	char irqname[32];

	/* disable all hrtimer */
	val = hal_readl((unsigned long)TIMER_CTL_REG(id));
	hal_writel(val & ~TIMER_CTL_ENABLE, (unsigned long)TIMER_CTL_REG(id));

	/* clear pending */
	hal_writel((0x1 << id), (unsigned long)TIMER_IRQ_ST_REG);

	g_timer[id].timer_id = id;
	g_timer[id].clk_rate = 24000000;      //ahb1,should get form clk driver
#ifdef TIMER_APB1_CLK
	hal_clk_t ahb1 = hal_clock_get(TIMER_APB_CLK_TYPE, TIMER_APB1_CLK);
	if (ahb1)
		g_timer[id].clk_rate = hal_clk_get_rate(ahb1);
	hal_clock_put(ahb1);
#endif
	g_timer[id].irq = SUNXI_IRQ_WUPTMR(id);
	g_timer[id].min_delta_ticks = TIMER_SYNC_TICKS;
	g_timer[id].max_delta_ticks = 0xffffffff;
	g_timer[id].callback = NULL;
	g_timer[id].param = NULL;

	snprintf(irqname, 32, "wuptimer%d", id);

	if (hal_request_irq(g_timer[id].irq, sunxi_timer_irq_handle, "irqname", &g_timer[id]) < 0)
	{
		return ;
	}

	/*enable timer irq*/
	val = hal_readl((unsigned long)TIMER_IRQ_EN_REG);
	val |= TIMER_IRQ_EN(id);
	hal_writel(val, (unsigned long)TIMER_IRQ_EN_REG);

	/* enable irq */
	hal_enable_irq(g_timer[id].irq);

	ret = pm_set_wakeirq(g_timer[id].irq);
	if (ret)
		printf("pm_set_wakeirq fail\n");

#ifdef CONFIG_COMPONENTS_PM
	g_timer[id].pm.data = (void *)id;
	g_timer[id].pm.name = "sunxi_wuptimer";
	g_timer[id].pm.ops = &pm_wuptimer_ops;
	pm_devops_register(&g_timer[id].pm);
#endif
}

void sunxi_wuptimer_uninit(hal_timer_id_t id)
{
	int ret;
#ifdef CONFIG_COMPONENTS_PM
	pm_devops_unregister(&g_timer[id].pm);
#endif
	ret = pm_clear_wakeirq(g_timer[id].irq);
	if (ret)
		LPUART_ERR("pm_clear_wakeirq fail\n");
	hal_disable_irq(g_timer[id].irq);
	hal_free_irq(g_timer[id].irq);
}
#endif

