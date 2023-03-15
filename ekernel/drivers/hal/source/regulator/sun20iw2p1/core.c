/*
 * Copyright 2019 AllWinnertech  Co., Ltd
 * frank@allwinnertech.com
 */
#include <sunxi_hal_regulator.h>
#include <sunxi_hal_regulator_private.h>
#include "../type.h"
#include "../ffs.h"
#include <stddef.h>
#include <hal_gpio.h>
#include <pm_devops.h>

#define SOC_PRCM_BASE     		  (0x40050000)
#define SOC_PRCM_DCDC_CTRL0       (SOC_PRCM_BASE+0x0004)
#define SOC_PRCM_DCDC_CTRL1       (SOC_PRCM_BASE+0x0008)
#define SOC_PRCM_DCDC_LDO_MODE    (SOC_PRCM_BASE+0x000c)
#define SOC_PRCM_RTC_LDO_CTRL     (SOC_PRCM_BASE+0x0020)
#define SOC_PRCM_EXT_LDO_CTRL     (SOC_PRCM_BASE+0x0024)
#define SOC_PRCM_TOP_LDO_CTRL     (SOC_PRCM_BASE+0x0028)
#define SOC_PRCM_AON_LDO_CTRL     (SOC_PRCM_BASE+0x0040)
#define SOC_PRCM_APP_LDO_CTRL     (SOC_PRCM_BASE+0x0044)
#define SOC_PRCM_DSP_LDO_CTRL     (SOC_PRCM_BASE+0x004c)

#define SOC_PMU_BASE     		  (0x40051400)
#define SOC_PMU_DCXO_TIME		  (SOC_PMU_BASE+0x0040)
#define SOC_PMU_DPLL_TIME		  (SOC_PMU_BASE+0x0044)
#define SOC_PMU_LDO_TIME		  (SOC_PMU_BASE+0x0048)
#define SOC_PMU_DIG_TIME		  (SOC_PMU_BASE+0x004c)
#define SOC_PMU_BANDGAP_TIME	  (SOC_PMU_BASE+0x0050)
#define SOC_PMU_DCDC_TIME		  (SOC_PMU_BASE+0x0054)

static const struct regulator_linear_range soc_rtc_ranges[] = {
	REGULATOR_LINEAR_RANGE(575000, 0x05, 0x05, 0),
	REGULATOR_LINEAR_RANGE(675000, 0x04, 0x04, 0),
	REGULATOR_LINEAR_RANGE(775000, 0x03, 0x03, 0),
	REGULATOR_LINEAR_RANGE(875000, 0x02, 0x02, 0),
	REGULATOR_LINEAR_RANGE(975000, 0x01, 0x01, 0),
	REGULATOR_LINEAR_RANGE(1075000, 0x00, 0x00, 0),
	REGULATOR_LINEAR_RANGE(1175000, 0x06, 0x07, 100000),
};

static const struct regulator_linear_range soc_ext_ldo_ranges[] = {
	REGULATOR_LINEAR_RANGE(2500000, 0x03, 0x03, 0),
	REGULATOR_LINEAR_RANGE(2800000, 0x02, 0x02, 0),
	REGULATOR_LINEAR_RANGE(3100000, 0x01, 0x01, 0),
	REGULATOR_LINEAR_RANGE(3300000, 0x00, 0x00, 0),
};

const struct regulator_desc soc_regulators[] = {
	SOC_DESC(1000, 2500, 25, SOC_PRCM_DCDC_CTRL0, 0x3F000, 0x3F000, NULL, NULL),
	SOC_DESC_RANGES(soc_rtc_ranges, SOC_PRCM_RTC_LDO_CTRL, 0x70, 0x07, NULL, NULL),
	SOC_DESC_RANGES(soc_ext_ldo_ranges, SOC_PRCM_EXT_LDO_CTRL, 0x30, 0x30, SOC_PRCM_EXT_LDO_CTRL, 0x03),
	SOC_DESC(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
	SOC_DESC(600, 1375, 25, SOC_PRCM_AON_LDO_CTRL, 0x3E00, 0x1F0, SOC_PRCM_AON_LDO_CTRL, BIT(0)),
	SOC_DESC(600, 1375, 25, SOC_PRCM_APP_LDO_CTRL, 0x3E00, 0x1F0, SOC_PRCM_APP_LDO_CTRL, BIT(0)),
	SOC_DESC(600, 1375, 25, SOC_PRCM_DSP_LDO_CTRL, 0x1F0, 0x1F0, SOC_PRCM_DSP_LDO_CTRL, BIT(0)),
};

static int hal_voltage2val(struct regulator_desc *info, int voltage, uint32_t *reg_val)
{
	const struct regulator_linear_range *range;
	int i;

	if (!info->linear_ranges) {
		*reg_val = (voltage - info->min_uv + info->step1_uv - 1)
			/ info->step1_uv;
		return 0;
	}

	for (i = 0; i < info->n_linear_ranges; i++) {
		int linear_max_uV;

		range = &info->linear_ranges[i];
		linear_max_uV = range->min_uV +
			(range->max_sel - range->min_sel) * range->uV_step;

		if (!(voltage <= linear_max_uV && voltage >= range->min_uV))
			continue;

		/* range->uV_step == 0 means fixed voltage range */
		if (range->uV_step == 0) {
			*reg_val = 0;
		} else {
			*reg_val = (voltage - range->min_uV) / range->uV_step;
		}

		*reg_val += range->min_sel;
		return 0;
	}

	return -1;
}

static int hal_val2voltage(struct regulator_desc *info, u8 reg_val, int *voltage)
{
	const struct regulator_linear_range *range = info->linear_ranges;
	int i;

	if (!info->linear_ranges) {
		*voltage = info->min_uv + info->step1_uv * reg_val;
		return 0;
	}

	for (i = 0; i < info->n_linear_ranges; i++, range++) {
		if (!(reg_val <= range->max_sel && reg_val >= range->min_sel))
			continue;

		*voltage = (reg_val - range->min_sel) * range->uV_step +
			   range->min_uV;
		return 0;
	}

	return -1;
}

int hal_regulator_set_dcdc_detct(struct regulator_dev *rdev, unsigned int flags)
{
	uint32_t ret;

	ret = readl(SOC_PRCM_DCDC_CTRL0);
	ret &= ~(0x06);
	ret |= flags << 1;
	writel(ret, SOC_PRCM_DCDC_CTRL0);

	return 0;
}

int hal_regulator_set_vol(struct regulator_dev *rdev, int target_uV, unsigned int status)
{
	unsigned char id = REGULATOR_ID(rdev->flag);
	struct regulator_desc *pd = (struct regulator_desc *)rdev->private;
	struct regulator_desc *info = &pd[id];
	uint32_t ret;
	uint32_t val;
	int vol_mask;

	if (status == SLEEP_MODE)
		vol_mask = info->vol_slp_mask;
	else
		vol_mask = info->vol_act_mask;

	if (hal_voltage2val(info, target_uV, &val))
		return -1;

	val <<= ffs(vol_mask) - 1;
	ret = readl(info->vol_reg);
	ret &= ~(vol_mask);
	ret |= val;
	writel(ret, info->vol_reg);

	return 0;
}

int hal_regulator_get_vol(struct regulator_dev *rdev, int *vol_uV, unsigned int status)
{
	unsigned char id = REGULATOR_ID(rdev->flag);
	struct regulator_desc *pd = (struct regulator_desc *)rdev->private;
	struct regulator_desc *info = &pd[id];
	uint32_t val;
	int vol_mask;

	if (status == SLEEP_MODE)
		vol_mask = info->vol_slp_mask;
	else
		vol_mask = info->vol_act_mask;

	val = readl(info->vol_reg);
	val &= vol_mask;
	val >>= ffs(vol_mask) - 1;
	if (hal_val2voltage(info, val, vol_uV))
		return -1;

	return 0;
}

int hal_regulator_set_en(struct regulator_dev *rdev, unsigned int flags)
{
	unsigned char id = REGULATOR_ID(rdev->flag);
	struct regulator_desc *pd = (struct regulator_desc *)rdev->private;
	struct regulator_desc *info = &pd[id];
	uint32_t ret;

	if (REGULATOR_ID(rdev->flag) == SOC_ID_DCDC) {
		hal_regulator_set_dcdc_detct(rdev, flags);
		return 0;
	}
	if ((info->enable_reg == NULL) || (info->enable_mask == NULL)) {
		return 0;
	}

	ret = readl(info->enable_reg);
	ret &= ~(info->enable_mask);
	ret |= flags << ffs(info->enable_mask) - 1;
	writel(ret, info->enable_reg);

	return 0;
}

static struct regulator_ops soc_regulator_ops = {
	.set_en				= hal_regulator_set_en,
	.set_voltage_ext	= hal_regulator_set_vol,
	.get_voltage_ext	= hal_regulator_get_vol,
};

static int regulator_suspend(struct pm_device *dev, suspend_mode_t mode)
{
	return 0;
}
static int regulator_resume(struct pm_device *dev, suspend_mode_t mode)
{
	return 0;
}


static struct pm_devops regulator_devops = {
	.suspend = regulator_suspend,
	.resume = regulator_resume,
};

static struct pm_device pm_regulator = {
	.name = "regulator",
	.ops = &regulator_devops,
};

/* get regulator_dev*/
int hal_regulator_get(unsigned int request_flag, struct regulator_dev *rdev)
{
	int rc;
	rdev->flag = request_flag;
	/* set ops */
	switch (REGULATOR_TYPE(rdev->flag)) {
	case SOC_REGULATOR:
		rdev->private = (void *)soc_regulators;
		rdev->ops = &soc_regulator_ops;
		break;
	default:
		goto out;
	}

	return 0;

out:
	return -1;
}

int hal_soc_regulator_init(void)
{
	/* set GPIO */
	hal_gpio_pinmux_set_function(GPIO_PA23, GPIO_MUXSEL_FUNCTION3);
	/* set pm function*/
	pm_devops_register(&pm_regulator);
}