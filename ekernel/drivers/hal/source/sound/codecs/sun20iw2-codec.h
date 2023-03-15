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
#ifndef _SUN20IW2_CODEC_H
#define _SUN20IW2_CODEC_H

#include <hal_clk.h>

#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
#include <sound/platform/sunxi-mad.h>
#endif

/* SUNXI_PR_CFG is to tear the acreg and dcreg, it is of no real meaning */
#define SUNXI_PR_CFG		(0xE000)
#define SUNXI_CODEC_BASE_ADDR	(0x4003D000ul)
#define SUNXI_CODEC_IRQ		55	/* to be confirmed */

/* BASE ADDR 0x4004B000, Analog PATH (0x4003D000 + 0xE00) */
#define AC_POWER_CTRL		(SUNXI_PR_CFG + 0x00)
#define AC_MBIAS_CTRL		(SUNXI_PR_CFG + 0x08)
#define AC_ADC_ANA_CTRL1	(SUNXI_PR_CFG + 0x0C)
#define AC_ADC_ANA_CTRL2	(SUNXI_PR_CFG + 0x10)
#define AC_ADC_ANA_CTRL3	(SUNXI_PR_CFG + 0x14)
#define AC_DAC_ANA_CTRL		(SUNXI_PR_CFG + 0x18)
#define AC_DHP_ANA_CTRL		(SUNXI_PR_CFG + 0x1C)

/* BASE ADDR 0x4004B000, ADC PATH (0x4003D000 + 0xE00) */
#define AC_ADC_DIG_CTRL		(SUNXI_PR_CFG + 0x100)
#define AC_ADC_HPF_CTRL1	(SUNXI_PR_CFG + 0x104)
#define AC_ADC_HPF_CTRL2	(SUNXI_PR_CFG + 0x108)
#define AC_ADC_DIG_VOL		(SUNXI_PR_CFG + 0x10C)
#define AC_ADC_RXFIFO_CTRL	(SUNXI_PR_CFG + 0x200)
#define AC_ADC_RXFIFO_STA	(SUNXI_PR_CFG + 0x204)
#define SUNXI_ADC_RXDATA	(SUNXI_PR_CFG + 0x208)
#define AC_ADC_RXCNT		(SUNXI_PR_CFG + 0x20C)
#define AC_ADC_DEBUG		(SUNXI_PR_CFG + 0x300)

/* BASE ADDR 0x4003D000, DAC PATH */
#define AC_DAC_DIG_CTRL		0x00
#define AC_DAC_DIG_VOL		0x04
#define AC_DAC_DPH_GAIN		0x08
#define AC_DAC_TXFIFO_CTRL	0x10
#define AC_DAC_TXFIFO_STA	0x14
#define SUNXI_DAC_TXDATA	0x18
#define AC_DAC_TXCNT		0x1C
#define AC_DAC_LBFIFO_CTRL	0x20
#define AC_DAC_LBFIFO_STA	0x24
#define AC_DAC_LBFIFO		0x28
#define AC_DAC_LBCNT		0x2C
#define AC_DAC_DEBUG		0x40

#define MAIN_EQ_EN_CTRL		0x100
#define MAIN_EQL_BQn_B0(n)	(0x104 + 0x14 * (n - 1))
#define MAIN_EQL_BQn_B1(n)	(0x108 + 0x14 * (n - 1))
#define MAIN_EQL_BQn_B2(n)	(0x10C + 0x14 * (n - 1))
#define MAIN_EQL_BQn_A1(n)	(0x110 + 0x14 * (n - 1))
#define MAIN_EQL_BQn_A2(n)	(0x114 + 0x14 * (n - 1))
#define MAIN_EQR_BQn_B0(n)	(0x300 + 0x14 * (n - 1))
#define MAIN_EQR_BQn_B1(n)	(0x304 + 0x14 * (n - 1))
#define MAIN_EQR_BQn_B2(n)	(0x308 + 0x14 * (n - 1))
#define MAIN_EQR_BQn_A1(n)	(0x30C + 0x14 * (n - 1))
#define MAIN_EQR_BQn_A2(n)	(0x310 + 0x14 * (n - 1))

#define POST_EQ_EN_CTRL		0x500
#define POST_EQ_BQn_B0(n)	(0x504 + 0x14 * (n - 1))
#define POST_EQ_BQn_B1(n)	(0x508 + 0x14 * (n - 1))
#define POST_EQ_BQn_B2(n)	(0x50C + 0x14 * (n - 1))
#define POST_EQ_BQn_A1(n)	(0x510 + 0x14 * (n - 1))
#define POST_EQ_BQn_A2(n)	(0x514 + 0x14 * (n - 1))

#define DRC_1B_ENA_CTRL		0x600
/* DRC_1B_XXX ... */
#define DRC_1B_ENA_OPT		0x660

#define DRC_3B_ENA_CTRL		0x700
/* DRC_3B_XXX ... */
#define DRC2_3B_SMOTH_CFG2	0x834

#define MAIN_EQ_REG_BEGAIN	MAIN_EQ_EN_CTRL
#define MAIN_EQ_REG_END		MAIN_EQR_BQn_A2(20)
#define POST_EQ_REG_BEGAIN	POST_EQ_EN_CTRL
#define POST_EQ_REG_END		POST_EQ_BQn_A2(5)
#define DRC_1B_REG_BEGAIN	DRC_1B_ENA_CTRL
#define DRC_1B_REG_END		DRC_1B_ENA_OPT
#define DRC_3B_REG_BEGAIN	DRC_3B_ENA_CTRL
#define DRC_3B_REG_END		DRC2_3B_SMOTH_CFG2

/* AC_POWER_CTRL	(SUNXI_PR_CFG + 0x00) */
#define ADDA_BIAS_EN		31
#define VRA1_SPEEDUP_TIME	20
#define VRA1_SPEEDUP_STA	19
#define ALDO_EN			18
#define ALDO_EN_VCTRL		15
#define ALDO_BYPASS		14
#define VRA1_EN			13
#define VRA1_VCTRL		11
#define VRA1_TRIM		7
#define IOPVRS			5
#define IOPDACS			3
#define IDHPS			1
#define ALDO_LQ			0

/* AC_MBIAS_CTRL	(SUNXI_PR_CFG + 0x08) */
#define MBIAS_EN		7
#define MBIAS_VOL_SEL		5
#define MBIAS_CHOPPER_EN	4
#define MBIAS_CHOPPER_CLK_SEL	2

/* AC_ADC_ANA_CTRL1	(SUNXI_PR_CFG + 0x0C) */
#define ADC1_EN			31
#define ADC_PGA_CHOPPER_EDGE	30
#define ADC1_DSM_DIS		29
#define ADC1_DSM_DEMOFF		28
#define ADC_DSM_EN_DITHER	27
#define ADC_DSM_DITHER_LVL	24
#define ADC1_DSM_SEL_OUT_EDGE	23
#define ADC1_DSM_OTA_IB_SEL	20
#define ADC1_DSM_OTA_CTRL	18
#define ADC1_PGA_OP_BIAS_CTRL	16
#define ADC1_PGA_OTA_IB_SEL	13
#define ADC1_PGA_GAIN_CTRL	8
#define ADC1_PGA_CHOPPER_EN		7
#define ADC1_PGA_CHOPPER_NOL_EN		6
#define ADC1_PGA_CHOPPER_CKSET		4
#define ADC1_PGA_CHOPPER_DELAY_SET	2
#define ADC1_PGA_CHOPPER_NOL_DELAY_SET	0

/* AC_ADC_ANA_CTRL2	(SUNXI_PR_CFG + 0x10) */
#define ADC2_EN			31
#define ADC2_DSM_DIS		29
#define ADC2_DSM_DEMOFF		28
#define ADC2_DSM_SEL_OUT_EDGE	23
#define ADC2_DSM_OTA_IB_SEL	20
#define ADC2_DSM_OTA_CTRL	18
#define ADC2_PGA_OP_BIAS_CTRL	16
#define ADC2_PGA_OTA_IB_SEL	13
#define ADC2_PGA_GAIN_CTRL	8
#define ADC2_PGA_CHOPPER_EN		7
#define ADC2_PGA_CHOPPER_NOL_EN		6
#define ADC2_PGA_CHOPPER_CKSET		4
#define ADC2_PGA_CHOPPER_DELAY_SET	2
#define ADC2_PGA_CHOPPER_NOL_DELAY_SET	0

/* AC_ADC_ANA_CTRL3	(SUNXI_PR_CFG + 0x14) */
#define ADC3_EN			31
#define ADC3_MIC_MIX		30
#define ADC3_DSM_DIS		29
#define ADC3_DSM_DEMOFF		28
#define ADC3_DSM_SEL_OUT_EDGE	23
#define ADC3_DSM_OTA_IB_SEL	20
#define ADC3_DSM_OTA_CTRL	18
#define ADC3_PGA_OP_BIAS_CTRL	16
#define ADC3_PGA_OTA_IB_SEL	13
#define ADC3_PGA_GAIN_CTRL	8
#define ADC3_PGA_CHOPPER_EN		7
#define ADC3_PGA_CHOPPER_NOL_EN		6
#define ADC3_PGA_CHOPPER_CKSET		4
#define ADC3_PGA_CHOPPER_DELAY_SET	2
#define ADC3_PGA_CHOPPER_NOL_DELAY_SET	0

/* AC_DAC_ANA_CTRL	(SUNXI_PR_CFG + 0x18) */
#define DACL_EN			31
#define DACR_EN			30
#define CKDAC_DELAY_SET		28
#define DAC_PGA_CHOPPER_EN		7
#define DAC_PGA_CHOPPER_NOL_EN		6
#define DAC_PGA_CHOPPER_CKSET		4
#define DAC_PGA_CHOPPER_DELAY_SET	2
#define DAC_PGA_CHOPPER_NOL_DELAY_SET	0

/* AC_DHP_ANA_CTRL	(SUNXI_PR_CFG + 0x1C) */
#define DHPL_EN			31
#define DHPR_EN			30
#define DHP_SELPGA_EN		29
#define DHP_OPDRV_CUR		26
#define DHP_OI_CTRL		24
#define DHP_CH_OUT_EDGE		11
#define DHP_PGA_CHOPPER_EN		7
#define DHP_PGA_CHOPPER_NOL_EN		6
#define DHP_PGA_CHOPPER_CKSET		4
#define DHP_PGA_CHOPPER_DELAY_SET	2
#define DHP_PGA_CHOPPER_NOL_DELAY_SET	0

/* AC_ADC_DIG_CTRL	(SUNXI_PR_CFG + 0x100) */
#define MAD_DATA_EN		25	/* default disable */
#define RX_SYNC_EN_START	24	/* default disable */
#define RX_SYNC_EN		23	/* default disable */
#define ADC_GEN			22
#define ADC1_DIG_EN		21
#define ADC2_DIG_EN		20
#define ADC3_DIG_EN		19
#define ADC1_SRC_SEL		18	/* default AMIC */
#define ADC2_SRC_SEL		17	/* default AMIC */
#define ADC3_SRC_SEL		16	/* default AMIC */
#define MIC_O_SWP_1		15	/* default disable */
#define MIC_O_SWP_2		14	/* default disable */
#define DMIC_VOL_FIX		13	/* default disable */
#define ADC_CLK			9
#define ADC_OSR			7
#define ADC_PTN_SEL		5
#define ADC_FD_MODE		3	/* default disable */
#define ADC_FDT			0	/* default 1.7ms */

/* AC_ADC_HPF_CTRL1	(SUNXI_PR_CFG + 0x104) */
#define HPF1_EN			31	/* default disable */
#define HPF1_FORMAT		15
#define HPF2_EN			14	/* default disable */
#define HPF2_FORMAT		0

/* AC_ADC_HPF_CTRL2	(SUNXI_PR_CFG + 0x108) */
#define HPF3_EN			14	/* default disable */
#define HPF3_FORMAT		0

/* AC_ADC_DIG_VOL	(SUNXI_PR_CFG + 0x10C) */
#define ADC_DVC_ZCD_EN		24	/* default enable */
#define ADC1_DIG_VOL		16	/* default 0dB */
#define ADC2_DIG_VOL		8
#define ADC3_DIG_VOL		0

/* AC_ADC_RXFIFO_CTRL	(SUNXI_PR_CFG + 0x200) */
#define ADC1_FIFO_EN		16
#define ADC2_FIFO_EN		15
#define ADC3_FIFO_EN		14
#define ADC4_FIFO_EN		13	/* default disable */
#define ADC_SAMPLE_BITS		12
#define ADC_FIFO_MODE		11
#define ADC_FIFO_TRG_LEVEL	4	/* default 0x40 */
#define ADC_DRQ_EN		3
#define ADC_IRQ_EN		2	/* default disable */
#define ADC_OVERRUN_IRQ_EN	1	/* default disable */
#define ADC_FIFO_FLUSH		0

/* AC_ADC_RXFIFO_STA	(SUNXI_PR_CFG + 0x204) */
#define ADC_RX_EMPTY		31
#define MAD_DATA_ALIGN		30
#define	ADC_RXA_CNT		8
#define	ADC_RXA_INT		1
#define	ADC_RXO_INT		0

/* SUNXI_ADC_RXDATA	(SUNXI_PR_CFG + 0x208) */
#define	ADC_RX_DATA		0

/* AC_ADC_RXCNT		(SUNXI_PR_CFG + 0x20C) */
#define	ADC_RX_CNT		0

/* AC_ADC_DEBUG		(SUNXI_PR_CFG + 0x300) */
#define	ADC_DEBUG_KEY		16
#define	DSM_DITHER_CTRL		4
#define	ADC_DEBUG_EN		3
#define	ADC_DEBUG_MODE_SEL	0

/* AC_DAC_DIG_CTRL	0x00 */
#define DAC_GEN			17
#define TXL_MIX_CTRL		15	/* default TXL */
#define TXR_MIX_CTRL		13	/* default TXR */
#define DACL_DIG_EN		12
#define DACR_DIG_EN		11
#define DAC_CLK			7
#define DAC_OSR			5
#define DAC_PTN_SEL		3	/* default TX */
#define DAC_DSDM_DITHER_SGM	0

/* AC_DAC_DIG_VOL	0x04 */
#define DAC_DVC_ZCD_EN		16	/* default enable */
#define DACL_DIG_VOL		8	/* default 0dB */
#define DACR_DIG_VOL		0	/* default 0dB */

/* AC_DAC_DPH_GAIN	0x08 */
#define DHP_OUTPUT_GAIN		8
#define DAC_MUTE_DET_TIME	5
#define DAC_ATT_STEP		3
#define LOUT_AUTO_ATT		2
#define LOUT_AUTO_MUTE		1
#define DHP_GAIN_ZC_DEN		0

/* AC_DAC_TXFIFO_CTRL	0x10 */
#define DAC_SEND_LASAT		26
#define DAC_FIFO_MODE		24
#define DAC_DRQ_CLR_CNT		21
#define DAC_FIFO_TRG_LEVEL	7
#define DAC_MONO_EN		6
#define DAC_SAMPLE_BITS		5
#define DAC_DRQ_EN		4
#define DAC_IRQ_EN		3
#define DAC_UNDERRUN_IRQ_EN	2
#define DAC_OVERRUN_IRQ_EN	1
#define DAC_FIFO_FLUSH		0

/* AC_DAC_TXFIFO_STA	0x14 */
#define DAC_TX_EMPTY		23
#define	DAC_TXE_CNT		8
#define	DAC_TXE_INT		3
#define	DAC_TXU_INT		2
#define	DAC_TXO_INT		1

/* SUNXI_DAC_TXDATA	0x18 */
#define	DAC_TX_DATA		0

/* AC_DAC_TXCNT		0x1C */
#define	DAC_TX_CNT		0

/* AC_DAC_LBFIFO_CTRL	0x20 */
#define	DACL_LB_EN		14
#define	DACR_LB_EN		13
#define	DAC_LB_SAMPLE_BITS	12
#define DAC_LB_FIFO_MODE	11
#define DAC_LB_FIFO_TRG_LEVEL	4
#define DAC_LB_DRQ_EN		3
#define DAC_LB_IRQ_EN		2
#define DAC_LB_OVERRUN_IRQ_EN	1
#define DAC_LB_FIFO_FLUSH	0

/* AC_DAC_LBFIFO_STA	0x24 */
#define DAC_LB_TX_EMPTY		31
#define	DAC_LBA_CNT		8
#define	DAC_LBA_INT		1
#define	DAC_LBO_INT		0

/* AC_DAC_LBFIFO	0x28 */
#define	DAC_RX_DATA		0

/* AC_DAC_LBCNT		0x2C */
#define	DAC_RX_CNT		0

/* AC_DAC_DEBUG		0x40 */
#define	DAC_DEBUG_KEY		16
#define	DAC_DEBUG_EN		2
#define	DAC_DEBUG_MODE_SEL	0

/* MAIN_EQ_EN_CTRL	0x100 */
#define	M_EQL_EN		4
#define	M_EQR_EN		0
/* MAIN_EQL_BQn_B0(n)	(0x104 + 0x14 * (n - 1)) */
#define	M_EQL_B0_CFT		0
/* MAIN_EQL_BQn_B1(n)	(0x108 + 0x14 * (n - 1)) */
#define	M_EQL_B1_CFT		0
/* MAIN_EQL_BQn_B2(n)	(0x10C + 0x14 * (n - 1)) */
#define	M_EQL_B2_CFT		0
/* MAIN_EQL_BQn_A1(n)	(0x110 + 0x14 * (n - 1)) */
#define	M_EQL_A1_CFT		0
/* MAIN_EQL_BQn_A2(n)	(0x114 + 0x14 * (n - 1)) */
#define	M_EQL_A2_CFT		0
/* MAIN_EQR_BQn_B0(n)	(0x300 + 0x14 * (n - 1)) */
#define	M_EQR_B0_CFT		0
/* MAIN_EQR_BQn_B1(n)	(0x304 + 0x14 * (n - 1)) */
#define	M_EQR_B1_CFT		0
/* MAIN_EQR_BQn_B2(n)	(0x308 + 0x14 * (n - 1)) */
#define	M_EQR_B2_CFT		0
/* MAIN_EQR_BQn_A1(n)	(0x30C + 0x14 * (n - 1)) */
#define	M_EQR_A1_CFT		0
/* MAIN_EQR_BQn_A2(n)	(0x310 + 0x14 * (n - 1)) */
#define	M_EQR_A2_CFT		0

/* POST_EQ_EN_CTRL	0x500 */
#define	P_EQ_EN			0
/* POST_EQ_BQn_B0(n)	(0x504 + 0x14 * (n - 1)) */
#define	P_EQ_B0_CFT		0
/* POST_EQ_BQn_B1(n)	(0x508 + 0x14 * (n - 1)) */
#define	P_EQ_B1_CFT		0
/* POST_EQ_BQn_B2(n)	(0x50C + 0x14 * (n - 1)) */
#define	P_EQ_B2_CFT		0
/* POST_EQ_BQn_A1(n)	(0x510 + 0x14 * (n - 1)) */
#define	P_EQ_A1_CFT		0
/* POST_EQ_BQn_A2(n)	(0x514 + 0x14 * (n - 1)) */
#define	P_EQ_A2_CFT		0

/* DRC_1B_ENA_CTRL	0x600 */
#define	DRC_1B_CAL_EN		1
#define	DRC_1B_HPF_EN		0

/* DRC_3B_ENA_CTRL	0x700 */
#define	DRC_3B_DELAY_EN		2
#define	DRC_3B_CAL_EN		1
#define	DRC_3B_HPF_EN		0

#define CODEC_REG_LABEL(constant)	{#constant, constant, 0}
#define CODEC_REG_LABEL_END		{NULL, 0, 0}

/* priv param */
#define SND_CTL_ENUM_LINEOUTL_MASK	1
#define SND_CTL_ENUM_LINEOUTR_MASK	2
#define SND_CTL_ENUM_MIC1_MASK		1
#define SND_CTL_ENUM_MIC2_MASK		2
#define SND_CTL_ENUM_MIC3_MASK		4

struct sunxi_codec_param {
	uint8_t digital_vol;
	uint8_t lineout_vol;

	/* playback */
	bool lineoutl_en;
	bool lineoutr_en;

	bool playback_dapm;

	int16_t gpio_spk;
	int16_t gpio_spk_power;
	int16_t pa_msleep_time;
	uint8_t pa_level;

	/* capture */
	uint8_t mic1gain;
	uint8_t mic2gain;
	uint8_t mic3gain;
	uint8_t adcgain;

	bool mic1_en;	/* for adc1 */
	bool mic2_en;	/* for adc2 */
	bool mic3_en;	/* for adc3 */

	bool adc1_en;
	bool adc2_en;
	bool adc3_en;

	bool mad_bind_en;
};

enum SUNXI_CODEC_CLK_WORK_MODE {
	SUNXI_CODEC_CLK_WORK_RESUME = 0,
	SUNXI_CODEC_CLK_WORK_SUSPEND_LOW,
	SUNXI_CODEC_CLK_WORK_SUSPEND_HIGH,
};

struct sunxi_codec_clk {
	struct reset_control *clk_rst_dac;	/* AON_RESET: RST_CODEC_DAC */
	struct reset_control *clk_rst_adc;	/* AON_RESET: RST_CODEC_ADC */
	hal_clk_t clk_bus_dac;			/* AON_CCU: CLK_BUS_CODEC_DAC */
	hal_clk_t clk_bus_adc;			/* AON_CCU: CLK_BUS_CODEC_ADC */

	/* audio pll -> codec dac&adc(24.576MHz)
	 * CLK_PLL_AUDIO --> CLK_PLL_AUDIO1X -----> CLK_AUDPLL_HOSC_SEL --> CLK_CODEC_DAC
	 *               `-> CLK_PLL_AUDIO2X -----^                     `-> CLK_CODEC_ADC_DIV --> CLK_CODEC_ADC_SEL1 --> CLK_CODEC_ADC_GATE
	 *                   CLK_HOSC ------------^
	 *                   CLK_DAUDIO_MCLK_IN --^
	 *
	 * note:
	 *    clk name                        enable  u-parent  d-parent  pllrate  modulerate
	 * 1. CLK_LDO1_EN,CLK_LDO2_EN
	 * 2. CLK_PLL_AUDIO                     +                            +
	 * 3. CLK_PLL_AUDIO1X,CLK_PLL_AUDIO2X                       +
	 * 4. CLK_AUDPLL_HOSC_SEL                         +         +
	 * 5. CLK_CODEC_DAC                     +         +
	 * 6. CLK_CODEC_ADC_DIV                           +         +                   +
	 * 7. CLK_CODEC_ADC_SEL1                          +
	 * 8. CLK_CODEC_ADC_GATE                +
	 *
	 * audio pll -> codec dac&adc(22.5792MHz)
	 * CLK_DPLL1 --> CLK_CK1_AUD --> CLK_CK1_AUD_DIV --> CLK_CODEC_DAC
	 *                                               `-> CLK_CODEC_ADC_DIV --> CLK_CODEC_ADC_SEL1 --> CLK_CODEC_ADC_GATE
	 *
	 * note:
	 *    clk name                        enable  u-parent  d-parent  pllrate  modulerate
	 * 1. CLK_DPLL1,CLK_CK1_AUD
	 * 2. CLK_CK1_AUD_DIV                   +                             +
	 * 3. CLK_CODEC_DAC                     +         +                             +
	 * 4. CLK_CODEC_ADC_DIV                           +         +                   +
	 * 5. CLK_CODEC_ADC_SEL1                          +
	 * 6. CLK_CODEC_ADC_GATE                +
	 *
	 * audio pll -> codec adc(8.192MHz)
	 * CLK_AUD_RCO_DIV --> CLK_CODEC_ADC_SEL1 --> CLK_CODEC_ADC_GATE
	 *
	 * note:
	 * SEL clk -> set parent clk only
	 * GATE clk -> enable & disable only
	 */
	hal_clk_t clk_pll_audio;		/* AON_CCU: CLK_PLL_AUDIO      -> 24.576M*4       */
	hal_clk_t clk_pll_audio1x;		/* AON_CCU: CLK_PLL_AUDIO1X    -> 24.576M         */
	hal_clk_t clk_pll_audio2x;		/* AON_CCU: CLK_PLL_AUDIO2X    -> 24.576M*2       */
	hal_clk_t clk_hosc;			/* AON_CCU: CLK_HOSC           -> 24.576M or 40M  */
	/* hal_clk_t clk_daudio_mclk_in; */	/* AON_CCU: CLK_DAUDIO_MCLK_IN -> from extern pad */
	hal_clk_t clk_audpll_hosc_sel;		/* AON_CCU: CLK_AUDPLL_HOSC_SEL */

	/* hal_clk_t clk_dpll1; */		/* AON_CCU: CLK_DPLL1          -> 22.5792M        */
	/* hal_clk_t clk_ck1_aud; */		/* AON_CCU: CLK_CK1_AUD */
	hal_clk_t clk_ck1_aud_div;		/* AON_CCU: CLK_CK1_AUD_DIV */

	hal_clk_t clk_aud_rco_div;		/* AON_CCU: CLK_AUD_RCO_DIV    -> 8.192M          */

	/* module dac */
	hal_clk_t clk_dac;			/* AON_CCU: CLK_CODEC_DAC */

	/* module adc */
	hal_clk_t clk_adc_div;			/* AON_CCU: CLK_CODEC_ADC_DIV */
	hal_clk_t clk_adc_sel1;			/* AON_CCU: CLK_CODEC_ADC_SEL1 */
	hal_clk_t clk_adc_gate;			/* AON_CCU: CLK_CODEC_ADC_GATE */

	enum SUNXI_CODEC_CLK_WORK_MODE clk_work_mode;
	unsigned int pll_freq_resume;
	unsigned int pll_freq_suspend;
	unsigned int sample_rates;
};

struct sunxi_codec_info {
	struct snd_codec *codec;

	void *codec_base_addr;
	struct sunxi_codec_clk clk;
	struct sunxi_codec_param param;

#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
	int capturing;
	struct sunxi_mad_priv mad_priv;
#endif
};

#endif /* __SUN20IW2_CODEC_H */
