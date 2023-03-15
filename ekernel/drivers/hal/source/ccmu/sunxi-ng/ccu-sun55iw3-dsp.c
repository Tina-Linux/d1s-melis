// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2022 Allwinnertech
 */
#include "ccu.h"
#include "ccu_common.h"
#include "ccu_reset.h"
#include "ccu_div.h"
#include "ccu_gate.h"
#include "ccu_mp.h"
#include "ccu_mult.h"
#include "ccu_nk.h"
#include "ccu_nkm.h"
#include "ccu_nkmp.h"
#include "ccu_nm.h"

#include "ccu-sun55iw3-dsp.h"
#include <hal_clk.h>
#include <hal_reset.h>
/* ccu_des_start */

static struct ccu_nm pll_audio1_clk = {
	.enable		= BIT(27),
	.lock		= BIT(28),
	.n		= _SUNXI_CCU_MULT_MIN(8, 8, 12),
	.m		= _SUNXI_CCU_DIV(1, 1), /* output divider */
	.common		= {
		.reg		= 0x000C,
		.hw.init	= CLK_HW_INIT("pll-audio1", "dcxo24M",
					      &ccu_nm_ops,
					      CLK_SET_RATE_UNGATE |
					      CLK_IS_CRITICAL),
	},
};

static SUNXI_CCU_M(pll_audio1_clk_div2, "pll-audio1-div2",
		   "pll-audio1", 0x000C, 16, 3, 0);

static SUNXI_CCU_M(pll_audio1_clk_div5, "pll-audio1-div5",
		   "pll-audio1", 0x000C, 20, 3, 0);

static SUNXI_CCU_M_WITH_GATE(pll_audio_out_clk, "pll-audio-out",
				"pll-audio1-div2", 0x001C,
				0, 5, BIT(31), 0);

static const char * const dsp_parents[] = { "dcxo24M", "osc32k", "rc-16m", "pll-audio1-div5", "pll-audio1-div2" };

static SUNXI_CCU_M_WITH_MUX_GATE(dsp_dsp_clk, "dsp_dsp", dsp_parents, 0x0020,
				  0, 5,
				  24, 3,
				  BIT(31), 0);

static const char * const i2s_parents[] = { "pll-audio0-4x", "pll-audio1-div2", "pll-audio1-div5" };

static SUNXI_CCU_MP_WITH_MUX_GATE_NO_INDEX(i2s0_clk, "i2s0",
					i2s_parents, 0x002C,
					0, 5,	/* M */
					5, 5,	/* N */
					24, 3,	/* mux */
					BIT(31), 0);

static SUNXI_CCU_MP_WITH_MUX_GATE_NO_INDEX(i2s1_clk, "i2s1",
					i2s_parents, 0x0030,
					0, 5,	/* M */
					5, 5,	/* N */
					24, 3,	/* mux */
					BIT(31), 0);

static SUNXI_CCU_MP_WITH_MUX_GATE_NO_INDEX(i2s2_clk, "i2s2",
					i2s_parents, 0x0034,
					0, 5,	/* M */
					5, 5,	/* N */
					24, 3,	/* mux */
					BIT(31), 0);

static SUNXI_CCU_MP_WITH_MUX_GATE_NO_INDEX(i2s3_clk, "i2s3",
					i2s_parents, 0x0038,
					0, 5,	/* M */
					5, 5,	/* N */
					24, 3,	/* mux */
					BIT(31), 0);

static const char * const i2s3_asrc_parents[] = { "pll-peri1-600m", "pll-audio1-div2", "pll-audio1-div5" };

static SUNXI_CCU_MP_WITH_MUX_GATE_NO_INDEX(i2s3_asrc_clk, "i2s3-asrc",
					i2s3_asrc_parents, 0x003C,
					0, 5,	/* M */
					5, 5,	/* N */
					24, 3,	/* mux */
					BIT(31), 0);

static SUNXI_CCU_GATE(i2s0_bgr_clk, "i2s0-bgr",
			"dcxo24M",
			0x0040, BIT(0), 0);

static SUNXI_CCU_GATE(i2s1_bgr_clk, "i2s1-bgr",
			"dcxo24M",
			0x0040, BIT(1), 0);

static SUNXI_CCU_GATE(i2s2_bgr_clk, "i2s2-bgr",
			"dcxo24M",
			0x0040, BIT(2), 0);

static SUNXI_CCU_GATE(i2s3_bgr_clk, "i2s3-bgr",
			"dcxo24M",
			0x0040, BIT(3), 0);

static const char * const spdif_tx_parents[] = { "pll-audio0-4x", "pll-audio1-div2", "pll-audio1-div5" };

static SUNXI_CCU_MP_WITH_MUX_GATE_NO_INDEX(spdif_tx_clk, "spdif-tx",
					spdif_tx_parents, 0x0044,
					0, 5,	/* M */
					5, 5,	/* N */
					24, 3,	/* mux */
					BIT(31), 0);

static const char * const spdif_rx_parents[] = { "pll-peri1-600m", "pll-audio1-div2", "pll-audio1-div5" };

static SUNXI_CCU_MP_WITH_MUX_GATE_NO_INDEX(spdif_rx_clk, "spdif-rx",
					spdif_rx_parents, 0x0048,
					0, 5,	/* M */
					5, 5,	/* N */
					24, 3,	/* mux */
					BIT(31), 0);

static SUNXI_CCU_GATE(bus_spdif_clk, "bus-spdif",
			"dcxo24M",
			0x004C, BIT(0), 0);

static const char * const dmic_parents[] = { "pll-audio0-4x", "pll-audio1-div2", "pll-audio1-div5" };

static SUNXI_CCU_MP_WITH_MUX_GATE_NO_INDEX(dmic_clk, "dmic",
					dmic_parents, 0x0050,
					0, 5,	/* M */
					5, 5,	/* N */
					24, 3,	/* mux */
					BIT(31), 0);

static SUNXI_CCU_GATE(dmic_bus_clk, "dmic-bus",
			"dcxo24M",
			0x0054, BIT(0), 0);

static const char * const audio_codec_dac_parents[] = { "pll-audio0-4x", "pll-audio1-div2", "pll-audio1-div5" };

static SUNXI_CCU_MP_WITH_MUX_GATE_NO_INDEX(audio_codec_dac_clk, "audio-codec-dac",
					audio_codec_dac_parents, 0x0058,
					0, 5,	/* M */
					5, 5,	/* N */
					24, 3,	/* mux */
					BIT(31), 0);

static const char * const audio_codec_adc_parents[] = { "pll-audio0-4x", "pll-audio1-div2", "pll-audio1-div5" };

static SUNXI_CCU_MP_WITH_MUX_GATE_NO_INDEX(audio_codec_adc_clk, "audio-codec-adc",
					audio_codec_adc_parents, 0x005C,
					0, 5,	/* M */
					5, 5,	/* N */
					24, 3,	/* mux */
					BIT(31), 0);

static SUNXI_CCU_GATE(audio_codec_clk, "audio-codec",
		"dcxo24M",
		0x0060, BIT(0), 0);

static SUNXI_CCU_GATE(dsp_msg_clk, "dsp-msg",
		"dcxo24M",
		0x0068, BIT(0), 0);

static SUNXI_CCU_GATE(dsp_cfg_clk, "dsp-cfg",
		"dcxo24M",
		0x006C, BIT(0), 0);

static SUNXI_CCU_GATE(npu_aclk, "npu-aclk",
		"dcxo24M",
		0x0070, BIT(2), 0);

static SUNXI_CCU_GATE(npu_hclk, "npu-hclk",
		"dcxo24M",
		0x0070, BIT(1), 0);

static SUNXI_CCU_GATE(dsp_npu_clk, "dsp-npu",
		"dcxo24M",
		0x0070, BIT(0), 0);

static const char * const dsp_timer_parents[] = { "dcxo24M", "rtc32k", "rc-16m", "r-ahb" };

static struct ccu_div dsp_timer0_clk = {
	.enable		= BIT(0),
	.div		= _SUNXI_CCU_DIV_FLAGS(1, 3, CLK_DIVIDER_POWER_OF_TWO),
	.mux		= _SUNXI_CCU_MUX(4, 2),
	.common		= {
		.reg		= 0x0074,
		.hw.init	= CLK_HW_INIT_PARENTS("dsp-timer0", dsp_timer_parents, &ccu_div_ops, 0),
	},
};

static struct ccu_div dsp_timer1_clk = {
	.enable		= BIT(0),
	.div		= _SUNXI_CCU_DIV_FLAGS(1, 3, CLK_DIVIDER_POWER_OF_TWO),
	.mux		= _SUNXI_CCU_MUX(4, 2),
	.common		= {
		.reg		= 0x0078,
		.hw.init	= CLK_HW_INIT_PARENTS("dsp-timer1", dsp_timer_parents, &ccu_div_ops, 0),
	},
};

static struct ccu_div dsp_timer2_clk = {
	.enable		= BIT(0),
	.div		= _SUNXI_CCU_DIV_FLAGS(1, 3, CLK_DIVIDER_POWER_OF_TWO),
	.mux		= _SUNXI_CCU_MUX(4, 2),
	.common		= {
		.reg		= 0x007C,
		.hw.init	= CLK_HW_INIT_PARENTS("dsp-timer2", dsp_timer_parents, &ccu_div_ops, 0),
	},
};

static struct ccu_div dsp_timer3_clk = {
	.enable		= BIT(0),
	.div		= _SUNXI_CCU_DIV_FLAGS(1, 3, CLK_DIVIDER_POWER_OF_TWO),
	.mux		= _SUNXI_CCU_MUX(4, 2),
	.common		= {
		.reg		= 0x0080,
		.hw.init	= CLK_HW_INIT_PARENTS("dsp-timer3", dsp_timer_parents, &ccu_div_ops, 0),
	},
};

static struct ccu_div dsp_timer4_clk = {
	.enable		= BIT(0),
	.div		= _SUNXI_CCU_DIV_FLAGS(1, 3, CLK_DIVIDER_POWER_OF_TWO),
	.mux		= _SUNXI_CCU_MUX(4, 2),
	.common		= {
		.reg		= 0x0084,
		.hw.init	= CLK_HW_INIT_PARENTS("dsp-timer4", dsp_timer_parents, &ccu_div_ops, 0),
	},
};

static struct ccu_div dsp_timer5_clk = {
	.enable		= BIT(0),
	.div		= _SUNXI_CCU_DIV_FLAGS(1, 3, CLK_DIVIDER_POWER_OF_TWO),
	.mux		= _SUNXI_CCU_MUX(4, 2),
	.common		= {
		.reg		= 0x0088,
		.hw.init	= CLK_HW_INIT_PARENTS("dsp-timer5", dsp_timer_parents, &ccu_div_ops, 0),
	},
};

static SUNXI_CCU_GATE(bus_dsp_timer_clk, "bus-dsp-timer",
		"dcxo24M",
		0x008C, BIT(0), 0);

static SUNXI_CCU_GATE(dsp_dma_clk, "dsp-dma",
		"dcxo24M",
		0x0104, BIT(0), 0);

static SUNXI_CCU_GATE(tzma0_clk, "tzma0",
		"dcxo24M",
		0x0108, BIT(0), 0);

static SUNXI_CCU_GATE(tzma1_clk, "tzma1",
		"dcxo24M",
		0x010C, BIT(0), 0);

static SUNXI_CCU_GATE(pubsram_clk, "pubsram",
		"dcxo24M",
		0x0114, BIT(0), 0);

static SUNXI_CCU_GATE(dsp_mclk, "dsp-mclk",
		"dcxo24M",
		0x011C, BIT(1), 0);

static SUNXI_CCU_GATE(dma_mclk, "dma-mclk",
		"dcxo24M",
		0x011C, BIT(0), 0);

static const char * const rv_parents[] = { "dcxo24m", "rtc-32k", "rc-16m" };

static SUNXI_CCU_MUX_WITH_GATE(rv_clk, "rv",
		rv_parents, 0x0120,
		27, 3,	/* mux */
		BIT(31), 0);

static SUNXI_CCU_GATE(rv_cfg_clk, "rv-cfg",
		"dcxo24M",
		0x0124, BIT(0), 0);

static SUNXI_CCU_GATE(riscv_msg_clk, "riscv-msg",
		"dcxo24M",
		0x0128, BIT(0), 0);

static const char * const pwm_parents[] = { "dcxo24m", "rtc-32k", "rc-16m" };

static SUNXI_CCU_MUX_WITH_GATE(dsp_pwm_clk, "dsp-pwm",
		pwm_parents, 0x0130,
		24, 3,	/* mux */
		BIT(31), 0);

static SUNXI_CCU_GATE(pwm_bgr_clk, "pwm-bgr",
		"dcxo24M",
		0x0134, BIT(0), 0);

static SUNXI_CCU_GATE(ahb_auto_clk, "ahb-auto",
		"dcxo24M",
		0x013C, BIT(24), 0);
/* ccu_des_end */


/* rst_def_start */
static struct ccu_reset_map sun55iw3_dsp_resets[] =
{
	[RST_BUS_DSP_I2S3]		= { 0x0040, BIT(19) },
	[RST_BUS_DSP_I2S2]		= { 0x0040, BIT(18) },
	[RST_BUS_DSP_I2S1]		= { 0x0040, BIT(17) },
	[RST_BUS_DSP_I2S0]		= { 0x0040, BIT(16) },
	[RST_BUS_DSP_SPDIF]		= { 0x004c, BIT(16) },
	[RST_BUS_DSP_DMIC]		= { 0x0054, BIT(16) },
	[RST_BUS_DSP_AUDIO_CODEC]	= { 0x0060, BIT(16) },
	[RST_BUS_DSP_MSG]		= { 0x0068, BIT(16) },
	[RST_BUS_DSP_CFG]		= { 0x006c, BIT(16) },
	[RST_BUS_DSP_NPU]		= { 0x0070, BIT(16) },
	[RST_BUS_DSP_TIME]		= { 0x008c, BIT(16) },
	[RST_BUS_DSP]			= { 0x0100, BIT(17) },
	[RST_BUS_DSP_DBG]		= { 0x0100, BIT(16) },
	[RST_BUS_DSP_DMA]		= { 0x0104, BIT(16) },
	[RST_BUS_DSP_PUBSRAM]		= { 0x0114, BIT(16) },
	[RST_BUS_DSP_RV_CORE]		= { 0x0124, BIT(18) },
	[RST_BUS_DSP_RV_APB_DB]		= { 0x0124, BIT(17) },
	[RST_BUS_DSP_RV_CFG]		= { 0x0124, BIT(16) },
	[RST_BUS_DSP_RV_MSG]		= { 0x0128, BIT(16) },
	[RST_BUS_DSP_PWM]		= { 0x0134, BIT(16) },
};
/* rst_def_end */

static struct ccu_common *sun55iw3_dsp_clks[] =
{
	&pll_audio1_clk.common,
	&pll_audio1_clk_div2.common,
	&pll_audio1_clk_div5.common,
	&pll_audio_out_clk.common,
	&dsp_dsp_clk.common,
	&i2s0_clk.common,
	&i2s1_clk.common,
	&i2s2_clk.common,
	&i2s3_clk.common,
	&i2s3_asrc_clk.common,
	&i2s0_bgr_clk.common,
	&i2s1_bgr_clk.common,
	&i2s2_bgr_clk.common,
	&i2s3_bgr_clk.common,
	&spdif_tx_clk.common,
	&spdif_rx_clk.common,
	&bus_spdif_clk.common,
	&dmic_clk.common,
	&dmic_bus_clk.common,
	&audio_codec_dac_clk.common,
	&audio_codec_adc_clk.common,
	&audio_codec_clk.common,
	&dsp_msg_clk.common,
	&dsp_cfg_clk.common,
	&npu_aclk.common,
	&npu_hclk.common,
	&dsp_npu_clk.common,
	&dsp_timer0_clk.common,
	&dsp_timer1_clk.common,
	&dsp_timer2_clk.common,
	&dsp_timer3_clk.common,
	&dsp_timer4_clk.common,
	&dsp_timer5_clk.common,
	&bus_dsp_timer_clk.common,
	&dsp_dma_clk.common,
	&tzma0_clk.common,
	&tzma1_clk.common,
	&pubsram_clk.common,
	&dsp_mclk.common,
	&dma_mclk.common,
	&rv_clk.common,
	&rv_cfg_clk.common,
	&riscv_msg_clk.common,
	&dsp_pwm_clk.common,
	&pwm_bgr_clk.common,
	&ahb_auto_clk.common,
};
/* ccu_def_start */

static struct clk_hw_onecell_data sun55iw3_hw_clks =
{
    .hws    = {
		[CLK_PLL_DSP_AUDIO1]		= &pll_audio1_clk.common.hw,
		[CLK_PLL_DSP_AUDIO1_DIV2]		= &pll_audio1_clk_div2.common.hw,
		[CLK_PLL_DSP_AUDIO1_DIV5]		= &pll_audio1_clk_div5.common.hw,
		[CLK_PLL_DSP_AUDIO_OUT]		= &pll_audio_out_clk.common.hw,
		[CLK_DSP_DSP]			= &dsp_dsp_clk.common.hw,
		[CLK_DSP_I2S0]			= &i2s0_clk.common.hw,
		[CLK_DSP_I2S1]			= &i2s1_clk.common.hw,
		[CLK_DSP_I2S2]			= &i2s2_clk.common.hw,
		[CLK_DSP_I2S3]			= &i2s3_clk.common.hw,
		[CLK_DSP_I2S3_ASRC]			= &i2s3_asrc_clk.common.hw,
		[CLK_BUS_DSP_I2S0]			= &i2s0_bgr_clk.common.hw,
		[CLK_BUS_DSP_I2S1]			= &i2s1_bgr_clk.common.hw,
		[CLK_BUS_DSP_I2S2]			= &i2s2_bgr_clk.common.hw,
		[CLK_BUS_DSP_I2S3]			= &i2s3_bgr_clk.common.hw,
		[CLK_DSP_SPDIF_TX]			= &spdif_tx_clk.common.hw,
		[CLK_DSP_SPDIF_RX]			= &spdif_rx_clk.common.hw,
		[CLK_BUS_DSP_SPDIF]			= &bus_spdif_clk.common.hw,
		[CLK_DSP_DMIC]			= &dmic_clk.common.hw,
		[CLK_BUS_DSP_DMIC]			= &dmic_bus_clk.common.hw,
		[CLK_DSP_AUDIO_CODEC_DAC]		= &audio_codec_dac_clk.common.hw,
		[CLK_DSP_AUDIO_CODEC_ADC]		= &audio_codec_adc_clk.common.hw,
		[CLK_BUS_DSP_AUDIO_CODEC]		= &audio_codec_clk.common.hw,
		[CLK_BUS_DSP_MSG]			= &dsp_msg_clk.common.hw,
		[CLK_BUS_DSP_CFG]			= &dsp_cfg_clk.common.hw,
		[CLK_BUS_DSP_NPU_ACLK]			= &npu_aclk.common.hw,
		[CLK_BUS_DSP_NPU_HCLK]			= &npu_hclk.common.hw,
		[CLK_BUS_DSP_NPU]			= &dsp_npu_clk.common.hw,
		[CLK_DSP_TIMER0]			= &dsp_timer0_clk.common.hw,
		[CLK_DSP_TIMER1]			= &dsp_timer1_clk.common.hw,
		[CLK_DSP_TIMER2]			= &dsp_timer2_clk.common.hw,
		[CLK_DSP_TIMER3]			= &dsp_timer3_clk.common.hw,
		[CLK_DSP_TIMER4]			= &dsp_timer4_clk.common.hw,
		[CLK_DSP_TIMER5]			= &dsp_timer5_clk.common.hw,
		[CLK_BUS_DSP_TIMER]			= &bus_dsp_timer_clk.common.hw,
		[CLK_BUS_DSP_DMA]			= &dsp_dma_clk.common.hw,
		[CLK_BUS_DSP_TZMA0]			= &tzma0_clk.common.hw,
		[CLK_BUS_DSP_TZMA1]			= &tzma1_clk.common.hw,
		[CLK_BUS_DSP_PUBSRAM]			= &pubsram_clk.common.hw,
		[CLK_BUS_DSP_MBUS]			= &dsp_mclk.common.hw,
		[CLK_BUS_DSP_DMA_MBUS]			= &dma_mclk.common.hw,
		[CLK_DSP_RV]			= &rv_clk.common.hw,
		[CLK_BUS_DSP_RV_CFG]			= &rv_cfg_clk.common.hw,
		[CLK_BUS_DSP_RISCV_MSG]			= &riscv_msg_clk.common.hw,
		[CLK_DSP_PWM]			= &dsp_pwm_clk.common.hw,
		[CLK_BUS_DSP_PWM]			= &pwm_bgr_clk.common.hw,
		[CLK_BUS_DSP_AHB_AUTO]			= &ahb_auto_clk.common.hw,
    },
    .num = CLK_DSP_NUMBER,
};
/* ccu_def_end */

static const struct sunxi_ccu_desc sun55iw3_dsp_desc =
{
	.ccu_clks   = sun55iw3_dsp_clks,
	.num_ccu_clks   = ARRAY_SIZE(sun55iw3_dsp_clks),

	.hw_clks    = &sun55iw3_hw_clks,
	.clk_type   = HAL_SUNXI_DSP,

	.resets     = sun55iw3_dsp_resets,
	.reset_type = HAL_SUNXI_DSP_RESET,
	.num_resets = ARRAY_SIZE(sun55iw3_dsp_resets),
};

__attribute__((weak)) int sunxi_rtc_ccu_init(void)
{
	return 0;
}

int sunxi_dsp_init(void)
{
	unsigned long reg = (unsigned long)SUNXI_DSP_CCU_BASE;
	int ret;

	ret = ccu_common_init(reg, &sun55iw3_dsp_desc);

	return ret;
}

