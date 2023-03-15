/*
 * Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
 *
 * Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
 * the the people's Republic of China and other countries.
 * All Allwinner Technology Co.,Ltd. trademarks are used with permission.
 *
 * DISCLAIMER
 * THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
 * IF YOU NEED TO INTEGRATE THIRD PARTY'S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
 * IN ALLWINNERS'SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
 * ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
 * ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <hal_log.h>
#include <hal_cmd.h>

#ifdef CONFIG_DRIVERS_TIMER
#include <sunxi_hal_timer.h>
#endif

#ifdef CONFIG_DRIVERS_HTIMER
#include <sunxi_hal_htimer.h>
#endif

#ifdef CONFIG_DRIVERS_TIMER
static void hal_timer_irq_callback(void *param)
{
	hal_log_err("timer interrupt!!\n");
}

int cmd_test_hal_timer(int argc, char **argv)
{
	int timer = SUNXI_TMR0;
	int delay_ms = 5 * 1000;

	if (argc >= 2) {
		if (strcmp("-h", argv[1]) == 0) {
			printf("usage: hal_timer <timer> <delay_ms>\n");
			return 0;
		}
	}

	if (atoi(argv[1]) < SUNXI_TMR_NUM)
		timer = SUNXI_TMR0 + atoi(argv[1]);

	if (argc >= 3)
		delay_ms = atoi(argv[2]);

	printf("test timer%d, wait %fs for interrupt\n", timer, delay_ms/1000.0);
	hal_timer_init(timer);
	hal_timer_set_oneshot(timer, delay_ms * 1000, hal_timer_irq_callback, NULL);
	return 0;
}
FINSH_FUNCTION_EXPORT_CMD(cmd_test_hal_timer, hal_timer, timer hal APIs tests)
#endif

#ifdef CONFIG_DRIVERS_WAKEUP_TIMER
static void hal_wuptimer_irq_callback(void *param)
{
	hal_log_info("wuptimer interrupt!!\n");
}

int cmd_test_hal_wuptimer(int argc, char **argv)
{
	int timer = SUNXI_TMR0;
	int delay_ms = 5 * 1000;
	if (argc >= 2) {
		if (strcmp("-h", argv[1]) == 0) {
			printf("usage: hal_wuptimer <timer> <delay_ms>\n");
			return 0;
		}
	}

	if (atoi(argv[1]) < SUNXI_TMR_NUM)
		timer = SUNXI_TMR0 + atoi(argv[1]);

	if (argc >= 3)
		delay_ms = atoi(argv[2]);

	printf("test wuptimer%d, wait %fs for interrupt\n", timer, delay_ms/1000.0);
	hal_wuptimer_init(timer);
	hal_wuptimer_set_oneshot(timer, delay_ms * 1000, hal_wuptimer_irq_callback, NULL);
	return 0;
}
FINSH_FUNCTION_EXPORT_CMD(cmd_test_hal_wuptimer, hal_wuptimer, wup timer hal APIs tests)
#endif

#ifdef CONFIG_DRIVERS_HTIMER
static void hal_htimer_irq_callback(void *param)
{
	hal_log_err("htimer interrupt!!\n");
}

int cmd_test_hal_htimer(int argc, char **argv)
{
	int ret;
	int htimer = HAL_HRTIMER0;
	int delay_ms = 5 * 1000;

	if (argc >= 2) {
		if (strcmp("-h", argv[1]) == 0) {
			printf("usage: hal_htimer <timer> <delay_ms>\n");
			return 0;
		}
	}

	if (atoi(argv[1]) < HAL_HRTIMER_NUM)
		htimer = HAL_HRTIMER0 + atoi(argv[1]);

	if (argc >= 3)
		delay_ms = atoi(argv[2]);

	printf("test htimer%d, wait %fs for interrupt\n", htimer, delay_ms/1000.0);
	hal_htimer_init();
	ret = hal_htimer_set_oneshot(htimer, delay_ms * 1000, hal_htimer_irq_callback, NULL);
	if (ret)
		hal_log_err("hal_htimer_set_oneshot fail!!\n");
	else
		hal_log_err("hal_htimer_set_oneshot OK!!\n");

	return 0;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_test_hal_htimer, hal_htimer, htimer hal APIs tests)
#endif
