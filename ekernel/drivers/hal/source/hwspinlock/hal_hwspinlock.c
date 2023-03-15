#include <sunxi_hal_common.h>
#include <hal_hwspinlock.h>
#include <hal_clk.h>
#include <hal_reset.h>
#ifdef CONFIG_COMPONENTS_PM
#include "pm_devops.h"
#endif
#include "hwspinlock.h"
/* Possible values of SPINLOCK_LOCK_REG */
#define SPINLOCK_NOTTAKEN               (0)     /* free */
#define SPINLOCK_TAKEN                  (1)     /* locked */
#define SPINLOCK_NUM			(32)	/* max lock num */

enum hwspinlock_err {
	HWSPINLOCK_OK = 0,
	HWSPINLOCK_ERR = -1,
	HWSPINLOCK_EXCEED_MAX = -2,
	HWSPINLOCK_PM_ERR = -3,
};

#ifdef CONFIG_COMPONENTS_PM
/*
 * Other drivers which use hwspinlock get function
 * must put hwspinlock in suspend,
 * Because It will be invalid after hwspinlock resume
 */
struct hwspinlock_hardware_resource {
	hal_clk_t clk;
	struct reset_control *reset;
};
struct hwspinlock_hardware_resource priv;

static int hal_hwspinlock_suspend(struct pm_device *dev, suspend_mode_t mode)
{
	if ((!priv.reset) || (!priv.clk))
		return HWSPINLOCK_PM_ERR;

	hal_reset_control_assert(priv.reset);
	hal_clock_disable(priv.clk);

	return 0;
}

static int hal_hwspinlock_resume(struct pm_device *dev, suspend_mode_t mode)
{
	if ((!priv.reset) || (!priv.clk))
		return HWSPINLOCK_PM_ERR;

	hal_reset_control_deassert(priv.reset);
	hal_clock_enable(priv.clk);

	return 0;
}

static struct pm_devops hwspinlock_devops = {
	.suspend = hal_hwspinlock_suspend,
	.resume = hal_hwspinlock_resume,
};

static struct pm_device hwspinlock_dev = {
	.name = "hwspinlock",
	.ops = &hwspinlock_devops,
};
#endif

void hal_hwspinlock_init(void)
{
#ifdef INIT_IN_ARM
	/* hwspinlock clk should init in arm */
#else
	hal_clk_status_t ret;
	hal_reset_type_t reset_type = HAL_SUNXI_RESET;
	u32 reset_id = RST_SPINLOCK;
	hal_clk_type_t clk_type = HAL_SUNXI_CCU;
	hal_clk_id_t clk_id = CLK_BUS_SPINLOCK ;
	hal_clk_t clk;
	struct reset_control *reset;

	reset = hal_reset_control_get(reset_type, reset_id);
	hal_reset_control_deassert(reset);

	clk = hal_clock_get(clk_type, clk_id);
	ret = hal_clock_enable(clk);
#endif
#ifdef CONFIG_COMPONENTS_PM
	priv.clk = clk;
	priv.reset = reset;
	pm_devops_register(&hwspinlock_dev);
#endif
}

int hal_hwspinlock_check_taken(int num)
{
	return !!(readl(SPINLOCK_STATUS_REG) & (1 << num));
}

int hal_hwspinlock_get(int num)
{
	unsigned long addr = SPINLOCK_LOCK_REG(num);
	int status;

	if (num > SPINLOCK_NUM)
		return HWSPINLOCK_EXCEED_MAX;

	status = readl(addr);

	if (status == SPINLOCK_NOTTAKEN)
		return HWSPINLOCK_OK;

	return HWSPINLOCK_ERR;
}

int hal_hwspinlock_put(int num)
{
	unsigned long addr = SPINLOCK_LOCK_REG(num);

	if (num > SPINLOCK_NUM)
		return HWSPINLOCK_EXCEED_MAX;

	writel(SPINLOCK_NOTTAKEN, addr);

	return HWSPINLOCK_OK;
}

void hal_hwspin_lock(int num)
{
	if (num > SPINLOCK_NUM)
		return;

	while(hal_hwspinlock_get(num) != HWSPINLOCK_OK);
}

void hal_hwspin_unlock(int num)
{
	hal_hwspinlock_put(num);
}
