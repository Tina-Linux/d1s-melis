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
#ifndef	__SUN20IW2_SPDIF_H_
#define	__SUN20IW2_SPDIF_H_

#define	SUNXI_SPDIF_MEMBASE (0x40045c00)

/*add this to surport the spdif receive IEC-61937 data */
#define CONFIG_SND_SUNXI_SPDIF_RX_IEC61937

/*------------------SPDIF EXP register definition--------------------*/
#define	SUNXI_SPDIF_EXP_CTL	0x40
#define	SUNXI_SPDIF_EXP_ISTA	0x44
#define	SUNXI_SPDIF_EXP_INFO0	0x48
#define	SUNXI_SPDIF_EXP_INFO1	0x4C
#define	SUNXI_SPDIF_EXP_DBG0	0x50
#define	SUNXI_SPDIF_EXP_DBG1	0x54
#define	SUNXI_SPDIF_EXP_VER	0x58

/* SUNXI_SPDIF_EXP_CTL register */
#define INSET_DET_NUM		0
#define INSET_DET_EN		8
#define SYNCW_BIT_EN		9
#define DATA_TYPE_BIT_EN	10
#define DATA_LEG_BIT_EN		11
#define AUDIO_DATA_BIT_EN	12
#define RX_MODE			13
#define RX_MODE_MAN		14
#define UNIT_SEL		15
#define RPOTBF_NUM		16
#define BURST_DATA_OUT_SEL	30

/* SUNXI_SPDIF_EXP_ISTA register */
#define INSET_INT		0
#define PAPB_CAP_INT		1
#define PCPD_CAP_INT		2
#define RPDB_ERR_INT		3
#define PC_DTYOE_CH_INT		4
#define PC_ERR_FLAG_INT		5
#define PC_BIT_CH_INT		6
#define PC_PAUSE_STOP_INT	7
#define PD_CHAN_INT		8
#define INSET_INT_EN		16
#define PAPB_CAP_INT_EN		17
#define PCPD_CAP_INT_EN		18
#define RPDB_ERR_INT_EN		19
#define PC_DTYOE_CH_INT_EN	20
#define PC_ERR_FLAG_INT_EN	21
#define PC_BIT_CH_INT_EN	22
#define PC_PAUSE_STOP_INT_EN	23
#define PD_CHAN_INT_EN		24

/* SUNXI_SPDIF_EXP_INFO0 register */
#define PD_DATA_INFO		0
#define PC_DATA_INFO		16

/* SUNXI_SPDIF_EXP_INFO1 register */
#define SAMPLE_RATE_VAL		0
#define RPOTBF_VAL		16

/* SUNXI_SPDIF_EXP_DBG0 register */
#define RE_DATA_COUNT_VAL	0
#define DATA_CAP_STA_MACHE	16

/* SUNXI_SPDIF_EXP_DBG1 register */
#define SAMPLE_RATE_COUNT	0
#define RPOTBF_COUNT		16

/* SUNXI_SPDIF_EXP_VER register */
#define MOD_VER			0

/*------------------------ PIN CONFIG FOR NORMAL ---------------------------*/
spdif_gpio_t g_spdif_gpio = {
	/* .clk	= {GPIOB(12), 3}, */
	//.out	= {GPIOA(17), 3},	/* use for uart0 rx */
	//.in	= {GPIOA(16), 3},	/* use for uart0 tx */
	.out	= {GPIOA(8), 3},
	.in	= {GPIOA(7), 3},
};

/*------------------------ CLK CONFIG FOR SUN20IW2 ---------------------------*/
struct sunxi_spdif_clk {
	struct reset_control *clk_rst;		/* RESET: RST_SPDIF */
	hal_clk_t clk_bus;			/* CCU: CLK_BUS_SPDIF */

	/* audio pll -> spdif tx(24.576MHz)
	 * CLK_PLL_AUDIO --> CLK_PLL_AUDIO1X -----> CLK_AUDPLL_HOSC_SEL --> CLK_SPDIF_TX
	 *               `-> CLK_PLL_AUDIO2X -----^
	 *                   CLK_HOSC ------------^
	 *                   CLK_DAUDIO_MCLK_IN --^
	 *
	 * audio pll -> spdif tx(22.5792MHz)
	 * CLK_DPLL1 --> CLK_CK1_AUD --> CLK_CK1_AUD_DIV --> CLK_SPDIF_TX
	 *
	 * audio pll -> spdif rx(240MHz)
	 * CK1_M33 ------------------------------> CLK_SPDIF_RX
	 * CK3_M33 ------------------------------^
	 * CK1_HIFI5 --> CLK_CKPLL_HIFI5_SEL ----^
	 * CK3_HIFI5 --^                         ^
	 * CLK_CK1_C906 --> CLK_CKPLL_C906_SEL --^
	 * CLK_CK3_C906 --^
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

	hal_clk_t clk_ck1_m33;			/* AON_CCU: CLK_CK1_M33 */
	hal_clk_t clk_ck3_m33;			/* AON_CCU: CLK_CK3_M33 */
	hal_clk_t clk_ck1_hifi5;		/* AON_CCU: CK1_HIFI5 */
	hal_clk_t clk_ck3_hifi5;		/* AON_CCU: CK3_HIFI5 */
	hal_clk_t clk_ckpll_hifi5_sel;		/* AON_CCU: CLK_CKPLL_HIFI5_SEL */
	hal_clk_t clk_ck1_c906;			/* AON_CCU: CLK_CK1_C906 */
	hal_clk_t clk_ck3_c906;			/* AON_CCU: CLK_CK3_C906 */
	hal_clk_t clk_ckpll_c906_sel;		/* AON_CCU: CLK_CKPLL_C906_SEL */

	/* module */
	hal_clk_t clk_spdif_tx;			/* AON_CCU: CLK_SPDIF_TX */
	hal_clk_t clk_spdif_rx;			/* AON_CCU: CLK_SPDIF_RX */
};

static inline int snd_sunxi_spdif_clk_enable(struct sunxi_spdif_clk *clk)
{
	int ret;

	snd_print("\n");

	/* rst & bus */
	ret = hal_reset_control_deassert(clk->clk_rst);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_rst clk_deassert failed.\n");
		goto err_deassert_clk_rst;
	}
	ret = hal_clock_enable(clk->clk_bus);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_bus enable failed.\n");
		goto err_enable_clk_bus;
	}

	/* pll */
	ret = hal_clock_enable(clk->clk_pll_audio);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_pll_audio enable failed.\n");
		goto err_enable_clk_pll_audio;
	}
	/*
	ret = hal_clock_enable(clk->clk_hosc);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_hosc enable failed.\n");
		goto err_enable_clk_hosc;
	}
	*/
	ret = hal_clock_enable(clk->clk_ck1_aud_div);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_ck1_aud_div enable failed.\n");
		goto err_enable_clk_ck1_aud_div;
	}
	ret = hal_clock_enable(clk->clk_ck1_c906);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_ck1_c906 enable failed.\n");
		//goto err_enable_clk_ck1_aud_div;
	}
	ret = hal_clock_enable(clk->clk_ckpll_c906_sel);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_ckpll_c906_sel enable failed.\n");
		//goto err_enable_clk_ck1_aud_div;
	}

	ret = hal_clock_enable(clk->clk_spdif_tx);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_spdif_tx enable failed.\n");
		goto err_enable_clk_spdif_tx;
	}
	ret = hal_clock_enable(clk->clk_spdif_rx);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_spdif_rx enable failed.\n");
		goto err_enable_clk_spdif_rx;
	}

	return HAL_CLK_STATUS_OK;

err_enable_clk_spdif_rx:
	hal_clock_disable(clk->clk_spdif_tx);
err_enable_clk_spdif_tx:
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

static inline void snd_sunxi_spdif_clk_disable(struct sunxi_spdif_clk *clk)
{
	snd_print("\n");

	hal_clock_disable(clk->clk_spdif_rx);
	hal_clock_disable(clk->clk_spdif_tx);
	hal_clock_disable(clk->clk_ckpll_c906_sel);
	/* hal_clock_disable(clk->clk_ck1_c906); */	/* should not off when suspend */
	hal_clock_disable(clk->clk_ck1_aud_div);
	/* hal_clock_disable(clk->clk_hosc); */
	hal_clock_disable(clk->clk_pll_audio);
	hal_clock_disable(clk->clk_bus);
	hal_reset_control_assert(clk->clk_rst);

	return;
}

static inline int snd_sunxi_spdif_clk_init(struct sunxi_spdif_clk *clk)
{
	int ret;

	snd_print("\n");

	/* rst & bus */
	clk->clk_rst = hal_reset_control_get(HAL_SUNXI_RESET, RST_SPDIF);
	if (!clk->clk_rst) {
		snd_err("spdif clk_rst hal_reset_control_get failed\n");
		goto err_get_clk_rst;
	}
	clk->clk_bus = hal_clock_get(HAL_SUNXI_CCU, CLK_BUS_SPDIF);
	if (!clk->clk_bus) {
		snd_err("spdif clk_bus hal_clock_get failed\n");
		goto err_get_clk_bus;
	}

	/* audio pll -> spdif tx(24.576MHz) */
	clk->clk_pll_audio = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_PLL_AUDIO);
	if (!clk->clk_pll_audio) {
		snd_err("spdif clk_pll_audio hal_clock_get failed\n");
		goto err_get_clk_pll_audio;
	}
	clk->clk_pll_audio1x = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_PLL_AUDIO1X);
	if (!clk->clk_pll_audio1x) {
		snd_err("spdif clk_pll_audio1x hal_clock_get failed\n");
		goto err_get_clk_pll_audio1x;
	}
	clk->clk_pll_audio2x = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_PLL_AUDIO2X);
	if (!clk->clk_pll_audio2x) {
		snd_err("spdif clk_pll_audio2x hal_clock_get failed\n");
		goto err_get_clk_pll_audio2x;
	}
	clk->clk_hosc = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_HOSC);
	if (!clk->clk_hosc) {
		snd_err("spdif clk_hosc hal_clock_get failed\n");
		goto err_get_clk_hosc;
	}

	clk->clk_audpll_hosc_sel = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_AUDPLL_HOSC_SEL);
	if (!clk->clk_audpll_hosc_sel) {
		snd_err("spdif clk_audpll_hosc_sel hal_clock_get failed\n");
		goto err_get_clk_audpll_hosc_sel;
	}

	/* audio pll -> spdif tx(22.5792MHz) */
	clk->clk_ck1_aud_div = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_CK1_AUD_DIV);
	if (!clk->clk_ck1_aud_div) {
		snd_err("spdif clk_ck1_aud_div hal_clock_get failed\n");
		goto err_get_clk_ck1_aud_div;
	}

	/* audio pll -> spdif rx(200MHz) */
	clk->clk_ck1_c906 = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_CK1_C906);
	if (!clk->clk_ck1_c906) {
		snd_err("spdif clk_ck1_c906 hal_clock_get failed\n");
		//goto err_get_clk_ck1_aud_div;
	}
	clk->clk_ckpll_c906_sel = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_CKPLL_C906_SEL);
	if (!clk->clk_ckpll_c906_sel) {
		snd_err("spdif clk_ckpll_c906_sel hal_clock_get failed\n");
		//goto err_get_clk_ck1_aud_div;
	}

	/* module clk */
	clk->clk_spdif_tx = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_SPDIF_TX);
	if (!clk->clk_spdif_tx) {
		snd_err("spdif clk_spdif_tx hal_clock_get failed\n");
		goto err_get_clk_spdif_tx;
	}
	clk->clk_spdif_rx = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_SPDIF_RX);
	if (!clk->clk_spdif_rx) {
		snd_err("spdif clk_spdif_rx hal_clock_get failed\n");
		goto err_get_clk_spdif_rx;
	}

	/* default: audio pll -> spdif tx(24.576MHz)
	 * CLK_PLL_AUDIO --> CLK_PLL_AUDIO1X --> CLK_AUDPLL_HOSC_SEL --> CLK_SPDIF_TX
	 */
	ret = hal_clk_set_parent(clk->clk_audpll_hosc_sel, clk->clk_pll_audio1x);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_pll_audio1x -> clk_audpll_hosc_sel clk_set_parent failed.\n");
		goto err_set_parent_clk;
	}
	ret = hal_clk_set_parent(clk->clk_spdif_tx, clk->clk_audpll_hosc_sel);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_audpll_hosc_sel -> clk_spdif_tx clk_set_parent failed.\n");
		goto err_set_parent_clk;
	}

	/* default: audio pll -> spdif rx(200MHz)
	 * CLK_CK1_C906 --> CLK_CKPLL_C906_SEL --> CLK_SPDIF_RX
	 */
	ret = hal_clk_set_parent(clk->clk_ckpll_c906_sel, clk->clk_ck1_c906);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_ck1_c906 -> clk_ckpll_c906_sel clk_set_parent failed.\n");
		//goto err_set_parent_clk;
	}
	ret = hal_clk_set_parent(clk->clk_spdif_rx, clk->clk_ckpll_c906_sel);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif clk_ckpll_c906_sel -> clk_spdif_rx clk_set_parent failed.\n");
		//goto err_set_parent_clk;
	}

	/* note: Enable and then set the freq to avoid clock lock errors */
	ret = snd_sunxi_spdif_clk_enable(clk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("spdif snd_sunxi_spdif_clk_enable failed.\n");
		goto err_clk_enable;
	}

	/* pll div limit */
	ret = hal_clk_set_rate(clk->clk_pll_audio, 98333333);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("set clk_pll_audio rate failed\n");
		goto err_set_rate_clk;
	}
	ret = hal_clk_set_rate(clk->clk_ck1_aud_div, 22588236);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("set clk_ck1_aud_div rate failed\n");
		goto err_set_rate_clk;
	}

	/* TODO: SPDIF_RX */
	ret = hal_clk_set_rate(clk->clk_spdif_rx, 240000000);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("set clk_spdif_rx rate failed\n");
		//goto err_set_rate_clk;
	}

	return HAL_CLK_STATUS_OK;

err_clk_enable:
err_set_rate_clk:
err_set_parent_clk:
	hal_clock_put(clk->clk_spdif_rx);
err_get_clk_spdif_rx:
	hal_clock_put(clk->clk_spdif_tx);
err_get_clk_spdif_tx:
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

static inline void snd_sunxi_spdif_clk_exit(struct sunxi_spdif_clk *clk)
{
	snd_print("\n");

	snd_sunxi_spdif_clk_disable(clk);
	hal_clock_put(clk->clk_spdif_rx);
	hal_clock_put(clk->clk_spdif_tx);
	hal_clock_put(clk->clk_ckpll_c906_sel);
	hal_clock_put(clk->clk_ck1_c906);
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

static inline int snd_sunxi_spdif_clk_set_rate(struct sunxi_spdif_clk *clk, int stream,
					       unsigned int freq_in, unsigned int freq_out)
{
	int ret;
	unsigned int freq;
	hal_clk_t clk_spdif;

	(void)stream;
	(void)freq_in;

	if (stream  == SNDRV_PCM_STREAM_PLAYBACK) {
		clk_spdif = clk->clk_spdif_tx;
		goto playback;
	} else {
		clk_spdif = clk->clk_spdif_rx;
		goto capture;
	}

playback:
	if (freq_out == 24576000) {
		freq = 24583333;
		ret = hal_clk_set_parent(clk_spdif, clk->clk_audpll_hosc_sel);
		if (ret != HAL_CLK_STATUS_OK) {
			snd_err("spdif clk_audpll_hosc_sel -> clk_spdif clk_set_parent failed.\n");
			return HAL_CLK_STATUS_ERROR;
		}
	} else {
		freq = 22588236;
		ret = hal_clk_set_parent(clk_spdif, clk->clk_ck1_aud_div);
		if (ret != HAL_CLK_STATUS_OK) {
			snd_err("spdif clk_ck1_aud_div -> clk_spdif clk_set_parent failed.\n");
			return HAL_CLK_STATUS_ERROR;
		}
	}

	/* note: CLK_SPDIF_TX no need set rate */

	return HAL_CLK_STATUS_OK;

capture:
	return HAL_CLK_STATUS_OK;
}

#endif /* __SUN20IW2_SPDIF_H_ */
