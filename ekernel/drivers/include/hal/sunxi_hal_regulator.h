/*
 * Copyright (c) 2020 Allwinner Technology Co., Ltd. ALL rights reserved.
 */

#ifndef __SUNXI_HAL_REGULATOR_H__
#define __SUNXI_HAL_REGULATOR_H__
#include <hal_debug.h>
#include <hal_log.h>
#ifdef __cplusplus
extern "C" {
#endif

struct regulator_dev;

struct regulator_ops{
	int (*enable) (struct regulator_dev *);
	int (*disable) (struct regulator_dev *);
	int (*set_voltage) (struct regulator_dev *, int target_uV);
	int (*get_voltage) (struct regulator_dev *, int *vol_uV);
	int (*set_en) (struct regulator_dev *, unsigned int flags);
	int (*set_voltage_ext) (struct regulator_dev *, int target_uV, unsigned int status);
	int (*get_voltage_ext) (struct regulator_dev *, int *vol_uV,  unsigned int status);
};

struct regulator_dev{
	unsigned int flag;
	struct regulator_ops *ops;
	void *private;
};

struct regulator_linear_range {
	unsigned int min_uV;
	unsigned int min_sel;
	unsigned int max_sel;
	unsigned int uV_step;
};

struct regulator_desc {
	int min_uv;
	int max_uv;
	int step1_uv;
	int vol_reg;
	int vol_mask;
	int enable_reg;
	int enable_mask;
	int enable_val;
	int disable_val;
	const struct regulator_linear_range *linear_ranges;
	int n_linear_ranges;
	int *vtable;
	int vol_act_mask;
	int vol_slp_mask;
};

#define AXP_DESC(min, max, step1, vreg, vmask, ereg, emask)\
{                                             \
	.min_uv       = (min) * 1000,             \
	.max_uv       = (max) * 1000,             \
	.step1_uv     = (step1) * 1000,           \
	.vol_reg      = vreg,                     \
	.vol_mask    = (vmask),                  \
	.enable_reg   = ereg,                     \
	.enable_mask  = (emask),                  \
}

#define AXP_DESC_FIXED(volt)\
{                                             \
	.min_uv       = (volt) * 1000,             \
	.max_uv       = (volt) * 1000,             \
}

#define AXP_IO(min, max, step1, vreg, vmask, ereg, emask, eval, disval)\
{                                             \
	.min_uv       = (min) * 1000,             \
	.max_uv       = (max) * 1000,             \
	.step1_uv     = (step1) * 1000,           \
	.vol_reg      = vreg,                     \
	.vol_mask    = (vmask),                  \
	.enable_reg   = ereg,                     \
	.enable_mask  = (emask),                  \
	.enable_val   = eval,                     \
	.disable_val  = disval,                   \
}

#define AXP_DESC_RANGES(_ranges, vreg, vmask, ereg, emask)\
{                                             \
	.vol_reg      = vreg,                     \
	.vol_mask    = (vmask),                  \
	.enable_reg   = ereg,                     \
	.enable_mask  = (emask),                  \
	.linear_ranges  = (_ranges),              \
	.n_linear_ranges = ARRAY_SIZE(_ranges),   \
}

#define AXP_SW(ereg, emask)\
{                                             \
	.enable_reg   = ereg,                     \
	.enable_mask  = (emask),                  \
}

#define AXP_SEL(min, max, vreg, vmask, ereg, emask, table_name)\
{                                                    \
	.min_uv      = (min) * 1000,                     \
	.max_uv      = (max) * 1000,                     \
	.vol_reg     = vreg,                             \
	.vol_mask   = (vmask),                          \
	.enable_reg  = ereg,                             \
	.enable_mask = (emask),                          \
	.vtable      = (int *)&table_name##_table,       \
}

/* Initialize struct regulator_linear_range */
#define REGULATOR_LINEAR_RANGE(_min_uV, _min_sel, _max_sel, _step_uV)   \
{                                                                       \
	.min_uV         = _min_uV,                                      \
	.min_sel        = _min_sel,                                     \
	.max_sel        = _max_sel,                                     \
	.uV_step        = _step_uV,                                     \
}

/* soc_regulators */

#define SOC_DESC(min, max, step1, vreg, vmask_act, vmask_slp, ereg, emask)\
{                                             \
	.min_uv       = (int)min * 1000,             \
	.max_uv       = (int)max * 1000,             \
	.step1_uv     = (int)step1 * 1000,           \
	.vol_reg      = (int)vreg,                     \
	.vol_act_mask    = (int)vmask_act,                  \
	.vol_slp_mask    = (int)vmask_slp,                  \
	.enable_reg   = (int)ereg,                     \
	.enable_mask  = (int)emask,                  \
}

#define SOC_DESC_RANGES(_ranges, vreg, vmask_act, vmask_slp, ereg, emask)\
{                                             \
	.vol_reg      = (int)vreg,                     \
	.vol_act_mask    = (int)vmask_act,                  \
	.vol_slp_mask    = (int)vmask_slp,                  \
	.linear_ranges  = (_ranges),              \
	.n_linear_ranges = ARRAY_SIZE(_ranges),           \
	.enable_reg   = (int)ereg,                     \
	.enable_mask  = (int)emask,                  \
}

/* for request_flag */
#define TWI_PORT_SHIFT		8
#define AXP_ADDR_SHIFT		12
#define REGULATOR_TYPE_SHIFT	28

#define REGULATOR_ID(x)		((x) & AXP_ID_MASK)
#define TWI_PORT(x)		(((x) & TWI_PORT_MASK) >> TWI_PORT_SHIFT)
#define AXP_ADDR(x)		(((x) & AXP_ADDR_MASK) >> AXP_ADDR_SHIFT)
#define REGULATOR_TYPE(x)	((x) >> REGULATOR_TYPE_SHIFT)

#define AXP_ID_MASK		0x000ff
#define TWI_PORT_MASK		0x00f00
#define AXP_ADDR_MASK		0xff000

#define REGULATOR_GET(x, y)	(((x) << REGULATOR_TYPE_SHIFT) | (y))

enum REGULATOR_TYPE_ENUM{
	AXP2101_REGULATOR,
	PWM_REGULATOR,
	GPIO_REGULATOR,
	SOC_REGULATOR,
	UNKNOWN_REGULATOR,
};

enum SOC_REGULATOR_STATUS_ENUM{
	ACT_MODE,
	SLEEP_MODE,
};

enum SOC_REGULATOR_DCDC_SET_ENUM{
	DCDC_DEFAULT = 1,
	DCDC_OPEN,
	DCDC_CLOSE,
};

enum SOC_REGULATOR_LDO_EXT_SET_ENUM{
	LDO_EXT_OFF,
	LDO_EXT_WITH_TOP_LDO,
	LDO_EXT_ALWAYS_ON,
};

/*
 *         +---> regulator type
 *         |
 *         |                --->Rsv
 *         |               /             --->pmu addr
 *         |              /             /        --->twi port
 *         |             /             /        /           --->regulator id
 *         |            /             /        /           /
 *      +--------------------------------------------------------+
 *      |31   28|27             20|19     12|11    8|7          0|
 *      +--------------------------------------------------------+
 */

//FIXME
/*
typedef short			s16;
typedef long long int		s64;

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned long long int	u64;
*/
static inline int hal_regulator_get_voltage(struct regulator_dev *rdev,
						 int *vol_uV)
{
    	if(rdev && rdev->ops && rdev->ops->get_voltage)
	{
		return rdev->ops->get_voltage(rdev, vol_uV);
	}
	else
	{
		hal_soft_break();
		hal_log_err("fatal error.");
	}
	return 0;
}

static inline int hal_regulator_set_voltage(struct regulator_dev *rdev,
						 int target_uV)
{
    	if(rdev && rdev->ops && rdev->ops->set_voltage)
	{
		return rdev->ops->set_voltage(rdev, target_uV);
	}
	else
	{
		hal_soft_break();
		hal_log_err("fatal error.");
	}
	return 0;
}

static inline int hal_regulator_enable(struct regulator_dev *rdev)
{
    	if(rdev && rdev->ops && rdev->ops->enable)
	{
		return rdev->ops->enable(rdev);
	}
	else
	{
		hal_soft_break();
		hal_log_err("fatal error.");
	}
	return 0;
}

static inline int hal_regulator_disable(struct regulator_dev *rdev)
{
    	if(rdev && rdev->ops && rdev->ops->disable)
	{
		return rdev->ops->disable(rdev);
	}
	else
	{
		hal_soft_break();
		hal_log_err("fatal error.");
	}

	return 0;
}
/* ext */
static inline int hal_regulator_get_voltage_ext(struct regulator_dev *rdev,
						 int *vol_uV, unsigned int status)
{
    if(rdev && rdev->ops && rdev->ops->get_voltage_ext)
	{
		return rdev->ops->get_voltage_ext(rdev, vol_uV, status);
	}
	else
	{
		hal_soft_break();
		hal_log_err("fatal error.");
	}
	return 0;
}

static inline int hal_regulator_set_voltage_ext(struct regulator_dev *rdev,
						 int target_uV, unsigned int status)
{
    if(rdev && rdev->ops && rdev->ops->set_voltage_ext)
	{
		return rdev->ops->set_voltage_ext(rdev, target_uV, status);
	}
	else
	{
		hal_soft_break();
		hal_log_err("fatal error.");
	}
	return 0;
}

static inline int hal_regulator_set_able(struct regulator_dev *rdev, unsigned int flags)
{
    if(rdev && rdev->ops && rdev->ops->set_en)
	{
		return rdev->ops->set_en(rdev, flags);
	}
	else
	{
		hal_soft_break();
		hal_log_err("fatal error.");
	}
	return 0;
}

int hal_regulator_get(unsigned int request_flag, struct regulator_dev *rdev);
/* TODO fix void type */
/* TODO fix void type */

enum REGULATOR_ID_ENUM {
	AXP2101_ID_DCDC1 = 0,
	AXP2101_ID_DCDC2,
	AXP2101_ID_DCDC3,
	AXP2101_ID_DCDC4,
	AXP2101_ID_DCDC5,
	AXP2101_ID_ALDO1,
	AXP2101_ID_ALDO2,
	AXP2101_ID_ALDO3,
	AXP2101_ID_ALDO4,
	AXP2101_ID_BLDO1,
	AXP2101_ID_BLDO2,
	AXP2101_ID_DLDO1,
	AXP2101_ID_DLDO2,
	AXP2101_ID_CPUSLDO,
	AXP2101_ID_MAX,
};
extern const struct regulator_desc axp2101_regulators[];

enum REGULATOR_SOC_ID_ENUM {
	SOC_ID_DCDC = 0,
	SOC_ID_RTC,
	SOC_ID_EXT_LDO,
	SOC_ID_TOP_LDO,
	SOC_ID_AON_LDO,
	SOC_ID_APP_LDO,
	SOC_ID_DSP_LDO,
	SOC_ID_MAX,
};
extern const struct regulator_desc soc_regulators[];

int hal_axp_twi_init(struct regulator_dev *rdev);
int hal_soc_regulator_init(void);
/*int hal_axp_byte_read(struct regulator_dev *rdev, u8 reg, u8 *reg_val);
int hal_axp_byte_write(struct regulator_dev *rdev, u8 reg, u8 reg_val);
int hal_axp_byte_update(struct regulator_dev *rdev, u8 reg, u8 val, u8 mask);
*/

#ifdef __cplusplus
}
#endif

#endif /* __SUNXI_HAL_REGULATOR_H__ */
