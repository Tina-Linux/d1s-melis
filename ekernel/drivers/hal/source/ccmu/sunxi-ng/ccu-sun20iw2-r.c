// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 huangzhenwei@allwinnertech.com
 */
#include "ccu.h"
#include "ccu_common.h"
#include "ccu_reset.h"

#include "ccu_div.h"
#include "ccu_gate.h"
#include "ccu_mp.h"
#include "ccu_nm.h"

#include "ccu-sun20iw2-r.h"

static CLK_FIXED_FACTOR(hosc_div32k_clk, "hosc-div32k", "hosc", 1250, 1, 0);
static CLK_FIXED_FACTOR(rcosc_div32k_clk, "rcosc-div32k", "rc-16m", 500, 1, 0);
/* System LFCLK Control Register */
static SUNXI_CCU_GATE(osc32k_en_clk, "osc32k-en", "osc12M", 0x080, BIT(31), 0);
static SUNXI_CCU_GATE(rc32k_en_clk, "rc32k-en", "osc12M", 0x080, BIT(30), 0);
static SUNXI_CCU_GATE(rc_hf_en_clk, "rc-hf-en", "osc12M", 0x080, BIT(29), 0);
static const char *const sys_32k_parents[] = { "fix-losc", "rccal-32k" };
static SUNXI_CCU_MUX(sys_32k_sel_clk, "sys-32k-sel", sys_32k_parents, 0x080, 28, 1, 0);
static const char *const ble_sel_parents[] = { "fix-losc", "rccal-32k" };
static SUNXI_CCU_MUX(ble_sel_clk, "ble-sel", ble_sel_parents, 0x080, 27, 1, 0);
static const char *const sysrtc_sel_parents[] = { "fix-losc", "rccal-32k","hosc-div32k" };
static SUNXI_CCU_MUX(sysrtc32k_clk, "sysrtc32k", sysrtc_sel_parents, 0x080, 25, 2, 0);
static const char *const lf_sel_parents[] = { "rc32k-en", "fix-losc" };
static SUNXI_CCU_MUX(lf_sel_clk, "lf-sel", lf_sel_parents, 0x080, 24, 1, 0);
static const char *const pad_parents[] = {"rcosc-div32k", "fix-losc", "rccal-32k", "hosc"};
static SUNXI_CCU_MUX(pad_clk, "pad", pad_parents, 0x080, 1, 2 , 0);
static SUNXI_CCU_M_WITH_GATE(pad_out_clk, "pad-out", "pad", 0x080,
		3, 13,
		BIT(0), 0);
/* BLE RCOSC Calibration Control Register0 */
static SUNXI_CCU_GATE(rccal32k_clk, "rccal-32k", "rc-16m", 0x144, BIT(29), 0);
static SUNXI_CCU_GATE(rco_wup_en_clk, "rco-wup-en", "hosc", 0x144, BIT(16), 0);

/* BLE RCOSC Calibrantion Control Register1 */
static SUNXI_CCU_GATE(rco_wup_mode_sel_clk, "rco-wup-mode-sel", "hosc", 0x148, BIT(0), 0);

/* BLE CLK32 AUTO Switch Register */
static const char *const div_clk_parents[] = { "rc-hf-en", "ble-32m" };
static SUNXI_CCU_MUX_WITH_GATE(div_clk, "div", div_clk_parents, 0x14c, 1, 1, BIT(4), 0);
static SUNXI_CCU_GATE(auto_switch_clk, "32k-auto-switch", "hosc", 0x14c, BIT(0), 0);

static struct ccu_common *sun20iw2_r_ccu_clks[] =
{
	&osc32k_en_clk.common,
	&rc32k_en_clk.common,
	&rc_hf_en_clk.common,
	&sys_32k_sel_clk.common,
	&ble_sel_clk.common,
	&sysrtc32k_clk.common,
	&lf_sel_clk.common,
	&pad_clk.common,
	&pad_out_clk.common,
	&rccal32k_clk.common,
	&rco_wup_en_clk.common,
	&rco_wup_mode_sel_clk.common,
	&div_clk.common,
	&auto_switch_clk.common,
};

static struct clk_hw_onecell_data sun20iw2_r_hw_clks =
{
	.hws    = {
		[CLK_HOSC_DIV_32K]         = &hosc_div32k_clk.hw,
		[CLK_RCOSE_DIV_32K]         = &rcosc_div32k_clk.hw,
		[CLK_OSC32K_EN]         = &osc32k_en_clk.common.hw,
		[CLK_RC32K_EN]         = &rc32k_en_clk.common.hw,
		[RC_HF_EN]         = &rc_hf_en_clk.common.hw,
		[CLK_SYS_32K_SEL]         = &sys_32k_sel_clk.common.hw,
		[CLK_BLE_SEL]         = &ble_sel_clk.common.hw,
		[CLK_SYSRTC32K]		= &sysrtc32k_clk.common.hw,
		[CLK_LF_SEL]		= &lf_sel_clk.common.hw,
		[CLK_PAD]         = &pad_clk.common.hw,
		[CLK_PAD_OUT]         = &pad_out_clk.common.hw,
		[CLK_RCCAL32K]         = &rccal32k_clk.common.hw,
		[CLK_RCO_WUP_EN]         = &rco_wup_en_clk.common.hw,
		[CLK_RCO_WUP_MODE_SEL]         = &rco_wup_mode_sel_clk.common.hw,
		[CLK_DIV]		= &div_clk.common.hw,
		[CLK_32K_AUTO_SWITCH]		= &auto_switch_clk.common.hw,
	},
	.num    = CLK_R_NUMBER,
};

static struct ccu_reset_map sun20iw2_r_ccu_resets[] =
{
	[RST_IS_WATCHDOG_ALL]  =  { 0x0c4, BIT(8) },
	[RST_IS_PMU]	=  { 0x0c4, BIT(1) },
	[RST_IS_PWRON]      =  { 0x0c4, BIT(0) },
	[RST_RCO_CALIB]   =  { 0x144, BIT(28) },
};

static const struct sunxi_ccu_desc sun20iw2_r_ccu_desc =
{
	.ccu_clks   = sun20iw2_r_ccu_clks,
	.num_ccu_clks   = ARRAY_SIZE(sun20iw2_r_ccu_clks),

	.hw_clks    = &sun20iw2_r_hw_clks,
	.clk_type   = HAL_SUNXI_R_CCU,

	.resets     = sun20iw2_r_ccu_resets,
	.reset_type = HAL_SUNXI_R_RESET,
	.num_resets = ARRAY_SIZE(sun20iw2_r_ccu_resets),
};

int sunxi_r_ccu_init(void)
{
	unsigned long reg = (unsigned long)SUNXI20_R_CCU_BASE;
	u32 reg_val;
	int ret;

	/* set LFCLK_SRC_SEL 1 */
	reg_val = readl(SUNXI20_R_CCU_BASE + PRCM_SYS_LFCLK_CTRL);
	reg_val |= BIT(24);
	writel(reg_val, SUNXI20_R_CCU_BASE + PRCM_SYS_LFCLK_CTRL);

	ret = ccu_common_init(reg, &sun20iw2_r_ccu_desc);
	sunxi_ccu_sleep_init((void *)reg, sun20iw2_r_ccu_clks,
			ARRAY_SIZE(sun20iw2_r_ccu_clks),
			NULL, 0);
	return ret;

}

