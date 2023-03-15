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
#include <hal_time.h>
#include <sound/snd_core.h>
#include "mad-sun20iw2.h"

int snd_sunxi_mad_clk_enable(struct sunxi_mad_clk *clk)
{
	int ret;

	snd_print("\n");

	/* rst & bus */
	ret = hal_reset_control_deassert(clk->clk_rst_lpsd);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("mad clk_rst_lpsd clk_deassert failed.\n");
		goto err_deassert_clk_rst_lpsd;
	}
	ret = hal_reset_control_deassert(clk->clk_rst_mad);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("mad clk_rst_mad clk_deassert failed.\n");
		goto err_deassert_clk_rst_mad;
	}
	ret = hal_reset_control_deassert(clk->clk_rst_madcfg);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("mad clk_rst_madcfg clk_deassert failed.\n");
		goto err_deassert_clk_rst_madcfg;
	}

	/* pll */
	ret = hal_clock_enable(clk->clk_rc_hf_en);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("mad clk_rc_hf_en enable failed.\n");
		goto err_enable_clk_rc_hf_en;
	}
	ret = hal_clock_enable(clk->clk_aud_rco_div);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("mad clk_aud_rco_div enable failed.\n");
		goto err_enable_clk_aud_rco_div;
	}
	ret = hal_clock_enable(clk->clk_mad_lpsd);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("mad clk_mad_lpsd enable failed.\n");
		goto err_enable_clk_mad_lpsd;
	}
	ret = hal_clock_enable(clk->clk_mad_apb_gate);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("mad clk_mad_apb_gate enable failed.\n");
		goto err_enable_clk_mad_apb_gate;
	}
	ret = hal_clock_enable(clk->clk_mad_ahb_gate);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("mad clk_mad_ahb_gate enable failed.\n");
		goto err_enable_clk_mad_ahb_gate;
	}

	return HAL_CLK_STATUS_OK;

err_enable_clk_mad_ahb_gate:
	hal_clock_disable(clk->clk_mad_apb_gate);
err_enable_clk_mad_apb_gate:
	hal_clock_disable(clk->clk_mad_lpsd);
err_enable_clk_mad_lpsd:
	hal_clock_disable(clk->clk_aud_rco_div);
err_enable_clk_aud_rco_div:
	hal_clock_disable(clk->clk_rc_hf_en);
err_enable_clk_rc_hf_en:
	hal_reset_control_assert(clk->clk_rst_madcfg);
err_deassert_clk_rst_madcfg:
	hal_reset_control_assert(clk->clk_rst_mad);
err_deassert_clk_rst_mad:
	hal_reset_control_assert(clk->clk_rst_lpsd);
err_deassert_clk_rst_lpsd:
	return HAL_CLK_STATUS_ERROR;
}

void snd_sunxi_mad_clk_disable(struct sunxi_mad_clk *clk)
{
	snd_print("\n");

	hal_clock_disable(clk->clk_mad_ahb_gate);
	hal_clock_disable(clk->clk_mad_apb_gate);
	hal_clock_disable(clk->clk_mad_lpsd);
	hal_clock_disable(clk->clk_aud_rco_div);
	hal_clock_disable(clk->clk_rc_hf_en);
	hal_reset_control_assert(clk->clk_rst_lpsd);
	hal_reset_control_assert(clk->clk_rst_mad);
	hal_reset_control_assert(clk->clk_rst_madcfg);

	return;
}

/* RC_HF CLK SET */
#define REG_SYS_LFCLK_CTRL	0x40050080
#define BIT_RC_HF_TRIM		16
#define MASK_RC_HF_TRIM		(0xff << BIT_RC_HF_TRIM)
#define RANGE_RC_HF_TRIM_MIN	0x0
#define RANGE_RC_HF_TRIM_MAX	0xff

#define REG_HOSC_FREQ_DET	0x4004c480
#define BIT_HOSC_FREQ_DET	4
#define MASK_HOSC_FREQ_DET	(0xfffff << BIT_HOSC_FREQ_DET)
#define BIT_HOSC_FREQ_READY	1
#define MASK_HOSC_FREQ_READY	(0x1 << BIT_HOSC_FREQ_READY)
#define BIT_HOSC_FREQ_ENABLE	0
#define MASK_HOSC_FREQ_ENABLE	(0x1 << BIT_HOSC_FREQ_ENABLE)

#define RC_HF_FREQ_REF		60000.0f	/* 60KHz */
#define HOSC_FREQ		40000000.0f	/* 40MHz */

static int snd_sunxi_update_bit(unsigned int reg, unsigned int mask, unsigned int value)
{
	unsigned int old, new;

	old = snd_readl(reg);
	new = (old & ~mask) | (value & mask);
	return snd_writel(new, reg);
}

/* rc_hf_rate = hosc_rate / (hosc_freq_det_val / rc_hf_ref_rate) */
static void snd_sunxi_set_rc_hf_rate(unsigned int rate, unsigned int det_time)
{
	unsigned int reg_temp;
	unsigned int det_val;
	unsigned int low_val, high_val, trim_val;
	float rc_hf_rate = 0.0f;

	low_val = RANGE_RC_HF_TRIM_MIN;
	high_val = RANGE_RC_HF_TRIM_MAX;
	trim_val = low_val + (low_val + high_val) / 2;
	while(1) {
		snd_sunxi_update_bit(REG_HOSC_FREQ_DET, MASK_HOSC_FREQ_ENABLE, (0 << BIT_HOSC_FREQ_ENABLE));
		snd_sunxi_update_bit(REG_SYS_LFCLK_CTRL, MASK_RC_HF_TRIM, (trim_val << BIT_RC_HF_TRIM));
		snd_sunxi_update_bit(REG_HOSC_FREQ_DET, MASK_HOSC_FREQ_ENABLE, (1 << BIT_HOSC_FREQ_ENABLE));

		while (1) {
			hal_msleep(det_time);
			reg_temp = snd_readl(REG_HOSC_FREQ_DET);
			if (reg_temp & MASK_HOSC_FREQ_READY)
				break;
		}

		det_val = (reg_temp & MASK_HOSC_FREQ_DET) >> BIT_HOSC_FREQ_DET;
		rc_hf_rate = (HOSC_FREQ * RC_HF_FREQ_REF) / (float)det_val;
		if ((high_val - low_val) < 2) {
			snd_info("rc_hf rate -> %f\n", rc_hf_rate);
			break;
		}

		if (rc_hf_rate < rate) {
			low_val = trim_val;
			trim_val += (high_val - trim_val) / 2;
		} else {
			high_val = trim_val;
			trim_val -= (trim_val - low_val) / 2;
		}
	}
}

int snd_sunxi_mad_clk_init(struct sunxi_mad_clk *clk)
{
	int ret;

	snd_print("\n");

	/* rst & bus */
	clk->clk_rst_lpsd = hal_reset_control_get(HAL_SUNXI_AON_CCU, RST_LPSD);
	if (!clk->clk_rst_lpsd) {
		snd_err("mad clk_rst_lpsd hal_reset_control_get failed\n");
		goto err_get_clk_rst_lpsd;
	}
	clk->clk_rst_mad = hal_reset_control_get(HAL_SUNXI_AON_CCU, RST_MAD);
	if (!clk->clk_rst_mad) {
		snd_err("mad clk_rst_mad hal_reset_control_get failed\n");
		goto err_get_clk_rst_mad;
	}
	clk->clk_rst_madcfg = hal_reset_control_get(HAL_SUNXI_AON_CCU, RST_MADCFG);
	if (!clk->clk_rst_madcfg) {
		snd_err("mad clk_rst_madcfg hal_reset_control_get failed\n");
		goto err_get_clk_rst_madcfg;
	}

	/* pll clk -> 8.192M */
	clk->clk_rc_hf_en = hal_clock_get(HAL_SUNXI_R_CCU, RC_HF_EN);
	if (!clk->clk_rc_hf_en) {
		snd_err("mad clk_rc_hf_en hal_clock_get failed\n");
		goto err_get_clk_rc_hf_en;
	}
	clk->clk_aud_rco_div = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_AUD_RCO_DIV);
	if (!clk->clk_aud_rco_div) {
		snd_err("mad clk_aud_rco_div hal_clock_get failed\n");
		goto err_get_clk_aud_rco_div;
	}
	clk->clk_adc_sel1 = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_CODEC_ADC_SEL1);
	if (!clk->clk_adc_sel1) {
		snd_err("codec clk_adc_sel1 hal_clock_get failed\n");
		goto err_get_clk_adc_sel1;
	}

	/* module clk */
	clk->clk_mad_lpsd = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_MAD_LPSD);
	if (!clk->clk_mad_lpsd) {
		snd_err("mad clk_mad_lpsd hal_clock_get failed\n");
		goto err_get_clk_mad_lpsd;
	}
	clk->clk_mad_apb_gate = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_MAD_APB_GATE);
	if (!clk->clk_mad_apb_gate) {
		snd_err("mad clk_mad_apb_gate hal_clock_get failed\n");
		goto err_get_clk_mad_apb_gate;
	}
	clk->clk_mad_ahb_gate = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_MAD_AHB_GATE);
	if (!clk->clk_mad_ahb_gate) {
		snd_err("mad clk_mad_ahb_gate hal_clock_get failed\n");
		goto err_get_clk_mad_ahb_gate;
	}

	clk->mad_clk		= clk->clk_mad_apb_gate;
	clk->mad_cfg_clk	= clk->clk_mad_ahb_gate;
	clk->mad_ad_clk		= clk->clk_mad_ahb_gate;
	clk->lpsd_clk		= clk->clk_mad_lpsd;

	/* default(system init unuse): (8.192MHz)
	 * CLK_AUD_RCO_DIV --> CLK_CODEC_ADC_SEL1 --> CLK_MAD_LPSD
	 */
	/* ret = hal_clk_set_parent(clk->clk_adc_sel1, clk->clk_aud_rco_div);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic clk_aud_rco_div -> clk_adc_sel1 clk_set_parent failed.\n");
		goto err_set_parent_clk;
	} */

	/* note: Enable and then set the freq to avoid clock lock errors */
	ret = snd_sunxi_mad_clk_enable(clk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("mad snd_sunxi_mad_clk_enable failed.\n");
		goto err_clk_enable;
	}

	/* set rate */
	ret = hal_clk_set_rate(clk->clk_mad_lpsd, 512000);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("set clk_mad_lpsd rate failed\n");
		goto err_set_rate_clk;
	}

	/* TODO: set RC_HF_CLK to 8.192MHz */
	snd_sunxi_set_rc_hf_rate(8192000, 15);

	return HAL_CLK_STATUS_OK;

err_set_rate_clk:
err_clk_enable:
/*err_set_parent_clk:*/
	hal_clock_put(clk->clk_mad_ahb_gate);
err_get_clk_mad_ahb_gate:
	hal_clock_put(clk->clk_mad_apb_gate);
err_get_clk_mad_apb_gate:
	hal_clock_put(clk->clk_mad_lpsd);
err_get_clk_mad_lpsd:
	hal_clock_put(clk->clk_adc_sel1);
err_get_clk_adc_sel1:
	hal_clock_put(clk->clk_aud_rco_div);
err_get_clk_aud_rco_div:
	hal_clock_put(clk->clk_rc_hf_en);
err_get_clk_rc_hf_en:
	hal_reset_control_put(clk->clk_rst_madcfg);
err_get_clk_rst_madcfg:
	hal_reset_control_put(clk->clk_rst_mad);
err_get_clk_rst_mad:
	hal_reset_control_put(clk->clk_rst_lpsd);
err_get_clk_rst_lpsd:
	return HAL_CLK_STATUS_ERROR;
}

void snd_sunxi_mad_clk_exit(struct sunxi_mad_clk *clk)
{
	snd_print("\n");

	snd_sunxi_mad_clk_disable(clk);
	hal_clock_put(clk->clk_mad_ahb_gate);
	hal_clock_put(clk->clk_mad_apb_gate);
	hal_clock_put(clk->clk_mad_lpsd);
	hal_clock_put(clk->clk_adc_sel1);
	hal_clock_put(clk->clk_aud_rco_div);
	hal_clock_put(clk->clk_rc_hf_en);
	hal_reset_control_put(clk->clk_rst_madcfg);
	hal_reset_control_put(clk->clk_rst_mad);
	hal_reset_control_put(clk->clk_rst_lpsd);

	return;
}

int snd_sunxi_mad_clk_set_rate(struct sunxi_mad_clk *clk, int stream,
			       unsigned int freq_in, unsigned int freq_out)
{
	(void)clk;
	(void)stream;
	(void)freq_in;
	(void)freq_out;

	return HAL_CLK_STATUS_OK;
}
