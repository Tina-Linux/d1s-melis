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

#include "sunxi_htimer.h"
#include "aw_common.h"
#include <stdlib.h>
#include <io.h>
#include <hal_interrupt.h>

static struct sunxi_htimer *g_htimer;

static const int sunxi_htimer_irq_num[] =
{
	SUNXI_IRQ_HSTIMER0,
	SUNXI_IRQ_HSTIMER1,
};


static hal_irqreturn_t sunxi_htimer_irq_handle(void *data)
{
	struct sunxi_htimer *timer = (struct sunxi_htimer *)data;

	/* clear pending */
	writel((0x1 << timer->timer_id), HTIMER_IRQ_ST_REG);

	/*callback*/
	HTIMER_INFO("enter irq!\n");
	if (timer->callback != NULL)
	{
		timer->callback(timer->param);
	}

	return HAL_IRQ_OK;
}

static void sunxi_htimer_sync(uint32_t timer)
{
	uint32_t old = readl(HTIMER_CNTVAL_LO_REG(timer));

	while ((old - readl(HTIMER_CNTVAL_LO_REG(timer))) < HTIMER_SYNC_TICKS)
	{
		int i = 10;
		while (i--);
		break;
	}
}

void sunxi_htimer_stop(uint32_t timer)
{
	uint32_t val = readl(HTIMER_CTL_REG(timer));

	writel(val & ~HTIMER_CTL_ENABLE, HTIMER_CTL_REG(timer));

	sunxi_htimer_sync(timer);
}

void sunxi_htimer_start(uint32_t timer, bool periodic)
{
	uint32_t val = readl(HTIMER_CTL_REG(timer));

	if (periodic)
	{
		val &= ~HTIMER_CTL_ONESHOT;
	}
	else
	{
		val |= HTIMER_CTL_ONESHOT;
	}

	val |= HTIMER_CTL_CLK_PRES(1);          //100M

	writel(val | HTIMER_CTL_ENABLE | HTIMER_CTL_RELOAD, HTIMER_CTL_REG(timer));
}

static void sunxi_htimer_setup(uint32_t tick, uint32_t timer)
{
	writel(tick, HTIMER_INTVAL_LO_REG(timer));
}

int sunxi_htimer_set_oneshot(uint32_t delay_us, uint32_t timer, timer_callback callback, void *callback_param)
{
	uint32_t tick = delay_us * 100;



	if (tick < g_htimer[timer].min_delta_ticks || tick > g_htimer[timer].max_delta_ticks)
	{
		HTIMER_INFO("not support!\n");
		return -1;
	}

	if (callback != NULL)
	{
		g_htimer[timer].callback = callback;
		g_htimer[timer].param = callback_param;
	}

	sunxi_htimer_stop(timer);

	sunxi_htimer_setup(tick, timer);

	sunxi_htimer_start(timer, false);
}

int sunxi_htimer_set_periodic(uint32_t delay_us, uint32_t timer, timer_callback callback, void *callback_param)
{
	uint32_t tick = delay_us * 100;

	if (tick < g_htimer[timer].min_delta_ticks || tick > g_htimer[timer].max_delta_ticks)
	{
		HTIMER_INFO("not support!\n");
		return -1;
	}

	if (callback != NULL)
	{
		g_htimer[timer].callback = callback;
		g_htimer[timer].param = callback_param;
	}

	sunxi_htimer_stop(timer);

	sunxi_htimer_setup(tick, timer);

	sunxi_htimer_start(timer, true);

}

int sunxi_htimer_clk_init(void)
{
	int ret = 0;
	int i = 0;
#ifdef RST_HSTIMER_ID
	struct reset_control *reset;
	reset = hal_reset_control_get(RST_HSTIMER_TYPE, RST_HSTIMER_ID);
	if (!reset) {
		HTIMER_INFO("fail to get hstimer rst clk!\n");
		return -1;
	}

	ret = hal_reset_control_deassert(reset);
	if (ret) {
		HTIMER_INFO("fail to deassrt hstimer rst clk!\n");
		return ret;
	}

#endif

#ifdef CLK_BUS_HSTIMER_ID
	hal_clk_t clk;
	clk = hal_clock_get(CLK_BUS_HSTIMER_TYPE, CLK_BUS_HSTIMER_ID);
	if (!clk) {
		HTIMER_INFO("fail to get hstimer bus gate clk!\n");
		return -1;
	}
	ret = hal_clock_enable(clk);
	if (ret) {
		HTIMER_INFO("fail to enable hstimer bus gate clk!\n");
		return ret;
	}
#endif

#ifdef HSTIMER_NUM
	for(i = 0; i < HSTIMER_NUM; i++) {
		clk = hal_clock_get(clk_msg[i].type, clk_msg[i].id);
		if (!clk) {
			HTIMER_INFO("fail to get hstimer %d gate clk!\n", i);
			return -1;
		}
		ret = hal_clock_enable(clk);
		if (ret) {
			HTIMER_INFO("fail to enable hstimer %d gate clk!\n", i);
			return ret;
		}
	}
#endif

	return ret;
}

void sunxi_htimer_init(void)
{
	int i;
	int ret;
	struct sunxi_htimer *timer = NULL;
	uint32_t val;
	uint32_t size = ARRAY_SIZE(sunxi_htimer_irq_num);
	char irqname[32];

	for (i = 0; i < size; i++)
	{
		/* disable all hrtimer */
		val = readl(HTIMER_CTL_REG(i));
		writel(val & ~HTIMER_CTL_ENABLE, HTIMER_CTL_REG(i));
	}

	/* clear pending */
	writel(0x3, HTIMER_IRQ_ST_REG);

	timer = (struct sunxi_htimer *)malloc(size * sizeof(struct sunxi_htimer));
	if (timer == NULL)
	{
		HTIMER_INFO("alloc memory error!\n");
		return;
	}

	for (i = 0; i < size; i++)
	{
		timer[i].timer_id = i;
		timer[i].clk_rate = 24000000;      //ahb1,should get form clk driver
		timer[i].irq = sunxi_htimer_irq_num[i];
		timer[i].min_delta_ticks = HTIMER_SYNC_TICKS;
		timer[i].max_delta_ticks = 0xffffffff;
		timer[i].callback = NULL;
		timer[i].param = NULL;
		snprintf(irqname, 32, "htimer%d", i);
		ret = hal_request_irq(MAKE_IRQn(timer[i].irq, 0), sunxi_htimer_irq_handle, irqname, (void *)&timer[i]);
		if (ret < 0)
			HTIMER_INFO("hal_request_irq error!\n");
	}

	/*enable timer irq*/
	for (i = 0; i < size; i++)
	{
		val = readl(HTIMER_IRQ_EN_REG);
		val |= HTIMER_IRQ_EN(i);
		writel(val, HTIMER_IRQ_EN_REG);
	}

	/* enable irq */
	for (i = 0; i < size; i++)
	{
		hal_enable_irq(MAKE_IRQn(timer[i].irq, 0));
	}

	g_htimer = timer;

}
