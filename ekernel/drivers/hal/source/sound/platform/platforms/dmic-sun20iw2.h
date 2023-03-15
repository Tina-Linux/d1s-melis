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
#ifndef	__SUN20IW2_DMIC_H_
#define	__SUN20IW2_DMIC_H_

#define	SUNXI_DMIC_MEMBASE (0x4004c000)

/*------------------------ PIN CONFIG FOR NORMAL ---------------------------*/
dmic_gpio_t g_dmic_gpio = {
	.clk	= {GPIOA(23), 5},
	.din0	= {GPIOA(19), 5},
	.din1	= {GPIOA(20), 5},
	.din2	= {GPIOA(21), 5},
	.din3	= {GPIOA(22), 5},
};

/*------------------------ CLK CONFIG FOR SUN20IW2 ---------------------------*/
struct sunxi_dmic_clk {
	struct reset_control *clk_rst;		/* AON_RESET: RST_DMIC */
	hal_clk_t clk_bus;			/* AON_CCU: CLK_BUS_DMIC */

	/* audio pll -> dmic(24.576MHz)
	 * CLK_PLL_AUDIO --> CLK_PLL_AUDIO1X -----> CLK_AUDPLL_HOSC_SEL --> CLK_CODEC_ADC_DIV --> CLK_CODEC_ADC_SEL1 --> CLK_DMIC_GATE
	 *               `-> CLK_PLL_AUDIO2X -----^
	 *                   CLK_HOSC ------------^
	 *                   CLK_DAUDIO_MCLK_IN --^
	 *
	 * audio pll -> dmic(22.5792MHz)
	 * CLK_DPLL1 --> CLK_CK1_AUD --> CLK_CK1_AUD_DIV --> CLK_CODEC_ADC_DIV --> CLK_CODEC_ADC_SEL1 --> CLK_DMIC_GATE
	 *
	 * audio pll -> dmic(8.192MHz)
	 * CLK_AUD_RCO_DIV --> CLK_CODEC_ADC_SEL1 --> CLK_DMIC_GATE
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

	/* module */
	hal_clk_t clk_adc_div;			/* AON_CCU: CLK_CODEC_ADC_DIV */
	hal_clk_t clk_adc_sel1;			/* AON_CCU: CLK_CODEC_ADC_SEL1 */
	hal_clk_t clk_dmic_gate;		/* AON_CCU: CLK_DMIC_GATE */
};

static inline int snd_sunxi_dmic_clk_enable(struct sunxi_dmic_clk *clk)
{
	int ret;

	snd_print("\n");

	/* rst & bus */
	ret = hal_reset_control_deassert(clk->clk_rst);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic clk_rst clk_deassert failed.\n");
		goto err_deassert_clk_rst;
	}
	ret = hal_clock_enable(clk->clk_bus);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic clk_bus enable failed.\n");
		goto err_enable_clk_bus;
	}

	/* pll */
	ret = hal_clock_enable(clk->clk_pll_audio);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic clk_pll_audio enable failed.\n");
		goto err_enable_clk_pll_audio;
	}
	/*
	ret = hal_clock_enable(clk->clk_hosc);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic clk_hosc enable failed.\n");
		goto err_enable_clk_hosc;
	}
	*/
	ret = hal_clock_enable(clk->clk_ck1_aud_div);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic clk_ck1_aud_div enable failed.\n");
		goto err_enable_clk_ck1_aud_div;
	}
	/*
	ret = hal_clock_enable(clk->clk_aud_rco_div);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic clk_aud_rco_div enable failed.\n");
		goto err_enable_clk_aud_rco_div;
	}
	*/
	ret = hal_clock_enable(clk->clk_dmic_gate);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic clk_dmic_gate enable failed.\n");
		goto err_enable_clk_dmic_gate;
	}

	return HAL_CLK_STATUS_OK;

err_enable_clk_dmic_gate:
/*
	hal_clock_disable(clk->clk_aud_rco_div);
err_enable_clk_aud_rco_div:
*/
	hal_clock_disable(clk->clk_ck1_aud_div);
err_enable_clk_ck1_aud_div:
/*
	hal_clock_disable(clk->clk_hosc);
err_enable_clk_hosc:
*/
	hal_clock_disable(clk->clk_pll_audio);
err_enable_clk_pll_audio:
	hal_clock_disable(clk->clk_bus);
err_enable_clk_bus:
	hal_reset_control_assert(clk->clk_rst);
err_deassert_clk_rst:
	return HAL_CLK_STATUS_ERROR;
}

static inline void snd_sunxi_dmic_clk_disable(struct sunxi_dmic_clk *clk)
{
	snd_print("\n");

	hal_clock_disable(clk->clk_dmic_gate);
	/* hal_clock_disable(clk->clk_aud_rco_div); */
	hal_clock_disable(clk->clk_ck1_aud_div);
	/* hal_clock_disable(clk->clk_hosc); */
	hal_clock_disable(clk->clk_pll_audio);
	hal_clock_disable(clk->clk_bus);
	hal_reset_control_assert(clk->clk_rst);

	return;
}

static inline int snd_sunxi_dmic_clk_init(struct sunxi_dmic_clk *clk)
{
	int ret;

	snd_print("\n");

	/* rst & bus */
	clk->clk_rst = hal_reset_control_get(HAL_SUNXI_AON_RESET, RST_DMIC);
	if (!clk->clk_rst) {
		snd_err("dmic clk_rst hal_reset_control_get failed\n");
		goto err_get_clk_rst;
	}
	clk->clk_bus = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_BUS_DMIC);
	if (!clk->clk_bus) {
		snd_err("dmic clk_bus hal_clock_get failed\n");
		goto err_get_clk_bus;
	}

	/* pll clk -> 24.576MHz */
	clk->clk_pll_audio = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_PLL_AUDIO);
	if (!clk->clk_pll_audio) {
		snd_err("dmic clk_pll_audio hal_clock_get failed\n");
		goto err_get_clk_pll_audio;
	}
	clk->clk_pll_audio1x = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_PLL_AUDIO1X);
	if (!clk->clk_pll_audio1x) {
		snd_err("dmic clk_pll_audio1x hal_clock_get failed\n");
		goto err_get_clk_pll_audio1x;
	}
	clk->clk_pll_audio2x = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_PLL_AUDIO2X);
	if (!clk->clk_pll_audio2x) {
		snd_err("dmic clk_pll_audio2x hal_clock_get failed\n");
		goto err_get_clk_pll_audio2x;
	}
	clk->clk_hosc = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_HOSC);
	if (!clk->clk_hosc) {
		snd_err("dmic clk_hosc hal_clock_get failed\n");
		goto err_get_clk_hosc;
	}

	clk->clk_audpll_hosc_sel = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_AUDPLL_HOSC_SEL);
	if (!clk->clk_audpll_hosc_sel) {
		snd_err("dmic clk_audpll_hosc_sel hal_clock_get failed\n");
		goto err_get_clk_audpll_hosc_sel;
	}

	/* pll clk -> 22.5792MHz */
	clk->clk_ck1_aud_div = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_CK1_AUD_DIV);
	if (!clk->clk_ck1_aud_div) {
		snd_err("dmic clk_ck1_aud_div hal_clock_get failed\n");
		goto err_get_clk_ck1_aud_div;
	}

	/* pll clk -> 8.192M */
	clk->clk_aud_rco_div = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_AUD_RCO_DIV);
	if (!clk->clk_aud_rco_div) {
		snd_err("dmic clk_aud_rco_div hal_clock_get failed\n");
		goto err_get_clk_aud_rco_div;
	}

	/* module clk */
	clk->clk_adc_div = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_CODEC_ADC_DIV);
	if (!clk->clk_adc_div) {
		snd_err("dmic clk_adc_div hal_clock_get failed\n");
		goto err_get_clk_adc_div;
	}
	clk->clk_adc_sel1 = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_CODEC_ADC_SEL1);
	if (!clk->clk_adc_sel1) {
		snd_err("dmic clk_adc_sel1 hal_clock_get failed\n");
		goto err_get_clk_adc_sel1;
	}
	clk->clk_dmic_gate = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_DMIC_GATE);
	if (!clk->clk_dmic_gate) {
		snd_err("dmic clk_dmic_gate hal_clock_get failed\n");
		goto err_get_clk_dmic_gate;
	}

	/* default: (24.576MHz)
	 * CLK_PLL_AUDIO --> CLK_PLL_AUDIO1X --> CLK_AUDPLL_HOSC_SEL --> CLK_CODEC_ADC_DIV --> CLK_CODEC_ADC_SEL1 --> CLK_DMIC_GATE
	 *
	 * pll tree: (22.5792MHz)
	 * CLK_DPLL1 --> CLK_CK1_AUD --> CLK_CK1_AUD_DIV --> CLK_CODEC_ADC_DIV --> CLK_CODEC_ADC_SEL1 --> CLK_DMIC_GATE
	 *
	 * pll tree: (8.192MHz)
	 * CLK_AUD_RCO_DIV --> CLK_CODEC_ADC_SEL1 --> CLK_CODEC_ADC_GATE
	 */
	ret = hal_clk_set_parent(clk->clk_audpll_hosc_sel, clk->clk_pll_audio1x);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic clk_pll_audio1x -> clk_audpll_hosc_sel clk_set_parent failed.\n");
		goto err_set_parent_clk;
	}

	ret = hal_clk_set_parent(clk->clk_adc_div, clk->clk_audpll_hosc_sel);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic clk_audpll_hosc_sel -> clk_adc_div clk_set_parent failed.\n");
		goto err_set_parent_clk;
	}
	ret = hal_clk_set_parent(clk->clk_adc_sel1, clk->clk_adc_div);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic clk_adc_div -> clk_adc_sel1 clk_set_parent failed.\n");
		goto err_set_parent_clk;
	}

	/* note: Enable and then set the freq to avoid clock lock errors */
	ret = snd_sunxi_dmic_clk_enable(clk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic snd_sunxi_dmic_clk_enable failed.\n");
		goto err_clk_enable;
	}

	/* pll div limit */
	ret = hal_clk_set_rate(clk->clk_pll_audio, 98333333);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("set clk_pll_audio rate failed\n");
		goto err_set_rate_clk;
	}
	ret = hal_clk_set_rate(clk->clk_adc_div, 24583333);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("set clk_adc_div rate failed\n");
		goto err_set_rate_clk;
	}
	ret = hal_clk_set_rate(clk->clk_ck1_aud_div, 22588236);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("set clk_ck1_aud_div rate failed\n");
		goto err_set_rate_clk;
	}

	return HAL_CLK_STATUS_OK;

err_clk_enable:
err_set_rate_clk:
err_set_parent_clk:
	hal_clock_put(clk->clk_dmic_gate);
err_get_clk_dmic_gate:
	hal_clock_put(clk->clk_adc_sel1);
err_get_clk_adc_sel1:
	hal_clock_put(clk->clk_adc_div);
err_get_clk_adc_div:
	hal_clock_put(clk->clk_aud_rco_div);
err_get_clk_aud_rco_div:
	hal_clock_put(clk->clk_ck1_aud_div);
err_get_clk_ck1_aud_div:
	hal_clock_put(clk->clk_audpll_hosc_sel);
err_get_clk_audpll_hosc_sel:
	hal_clock_put(clk->clk_hosc);
err_get_clk_hosc:
	hal_clock_put(clk->clk_pll_audio2x);
err_get_clk_pll_audio2x:
	hal_clock_put(clk->clk_pll_audio1x);
err_get_clk_pll_audio1x:
	hal_clock_put(clk->clk_pll_audio);
err_get_clk_pll_audio:
	hal_clock_put(clk->clk_bus);
err_get_clk_bus:
	hal_reset_control_put(clk->clk_rst);
err_get_clk_rst:
	return HAL_CLK_STATUS_ERROR;
}

static inline void snd_sunxi_dmic_clk_exit(struct sunxi_dmic_clk *clk)
{
	snd_print("\n");

	snd_sunxi_dmic_clk_disable(clk);
	hal_clock_put(clk->clk_dmic_gate);
	hal_clock_put(clk->clk_adc_sel1);
	hal_clock_put(clk->clk_adc_div);
	hal_clock_put(clk->clk_aud_rco_div);
	hal_clock_put(clk->clk_ck1_aud_div);
	hal_clock_put(clk->clk_audpll_hosc_sel);
	hal_clock_put(clk->clk_hosc);
	hal_clock_put(clk->clk_pll_audio2x);
	hal_clock_put(clk->clk_pll_audio1x);
	hal_clock_put(clk->clk_pll_audio);
	hal_clock_put(clk->clk_bus);
	hal_reset_control_put(clk->clk_rst);

	return;
}

static inline int snd_sunxi_dmic_clk_set_rate(struct sunxi_dmic_clk *clk, int stream,
					      unsigned int freq_in, unsigned int freq_out)
{
	int ret;
	unsigned int freq;

	snd_print("freq_out -> %u\n", freq_out);

	(void)stream;
	(void)freq_in;

	if (freq_out == 24576000) {
		freq = 24583333;
		ret = hal_clk_set_parent(clk->clk_adc_div, clk->clk_audpll_hosc_sel);
		if (ret != HAL_CLK_STATUS_OK) {
			snd_err("dmic clk_audpll_hosc_sel -> clk_dmic clk_set_parent failed.\n");
			return HAL_CLK_STATUS_ERROR;
		}
	} else {
		freq = 22588236;
		ret = hal_clk_set_parent(clk->clk_adc_div, clk->clk_ck1_aud_div);
		if (ret != HAL_CLK_STATUS_OK) {
			snd_err("dmic clk_ck1_aud_div -> clk_dmic clk_set_parent failed.\n");
			return HAL_CLK_STATUS_ERROR;
		}
	}

	ret = hal_clk_set_rate(clk->clk_adc_div, freq);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("set clk_dmic rate %u failed\n", freq);
		return HAL_CLK_STATUS_ERROR;
	}

	return HAL_CLK_STATUS_OK;
}

#endif /* __SUN20IW2_DMIC_H_ */
