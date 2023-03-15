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
#ifndef	__SUN20IW2_DAUDIO_H_
#define	__SUN20IW2_DAUDIO_H_

#define	SUNXI_DAUDIO_BASE (0x40045800)

#define DAUDIO_NUM_MAX	1

/*------------------------ PIN CONFIG FOR NORMAL ---------------------------*/
#define DAUDIO0_PIN_MCLK \
{.gpio_pin = GPIOA(23), .mux = 2, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO0_PIN_BCLK \
{.gpio_pin = GPIOA(20), .mux = 2, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO0_PIN_LRCK \
{.gpio_pin = GPIOA(19), .mux = 2, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO0_PIN_DOUT0 \
{.gpio_pin = GPIOA(22), .mux = 2, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO0_PIN_DIN0 \
{.gpio_pin = GPIOA(21), .mux = 2, .driv_level = GPIO_DRIVING_LEVEL1}

#if CONFIG_PROJECT_XR875_PRO_C906
#define DAUDIO_PA_PIN0 \
{.pin = GPIOA(0), .level = GPIO_DATA_HIGH, .msleep = 0, .used = true}
#else
#define DAUDIO_PA_PIN0 \
{.pin = GPIOB(3), .level = GPIO_DATA_HIGH, .msleep = 0, .used = true}
#endif

/*
 * Daudio Params Setting
 *
 *daudio_master:
 *	1: SND_SOC_DAIFMT_CBM_CFM(codec clk & FRM master)		use
 *	2: SND_SOC_DAIFMT_CBS_CFM(codec clk slave & FRM master)		not use
 *	3: SND_SOC_DAIFMT_CBM_CFS(codec clk master & frame slave)	not use
 *	4: SND_SOC_DAIFMT_CBS_CFS(codec clk & FRM slave)		use
 *tdm_config:
 *	0 is pcm; 1 is i2s
 *audio_format:
 *	1:SND_SOC_DAIFMT_I2S(standard i2s format). use
 *	2:SND_SOC_DAIFMT_RIGHT_J(right justfied format).
 *	3:SND_SOC_DAIFMT_LEFT_J(left justfied format)
 *	4:SND_SOC_DAIFMT_DSP_A(pcm. MSB is available on 2nd BCLK rising edge after LRC rising edge). use
 *	5:SND_SOC_DAIFMT_DSP_B(pcm. MSB is available on 1nd BCLK rising edge after LRC rising edge)
 *signal_inversion:
 *	1:SND_SOC_DAIFMT_NB_NF(normal bit clock + frame)  use
 *	2:SND_SOC_DAIFMT_NB_IF(normal BCLK + inv FRM)
 *	3:SND_SOC_DAIFMT_IB_NF(invert BCLK + nor FRM)  use
 *	4:SND_SOC_DAIFMT_IB_IF(invert BCLK + FRM)
 *pcm_lrck_period	:16/32/64/128/256
 *msb_lsb_first		:0: msb first; 1: lsb first
 *sign_extend		:0: zero pending; 1: sign extend
 *slot_width_select	:8 bit width / 16 bit width / 32 bit width
 *frametype		:0: short frame = 1 clock width;  1: long frame = 2 clock width
 *mclk_div		:0: not output(normal setting this);
 *			:1/2/4/6/8/12/16/24/32/48/64/96/128/176/192:
 *			setting mclk as input clock to external codec,
 *			freq is pll_audio/mclk_div
 *tx_data_mode		:0: 16bit linear PCM; (use) 1: reserved;
 *			:2: 8bit u-law; (no use) 3: 8bit a-law (no use)
 *rx_data_mode		:0: 16bit linear PCM; (use) 1: reserved;
 *			:2: 8bit u-law; (no use) 3: 8bit a-law (no use)
 */

#define DAUDIO0_PARAMS \
{.tdm_num = 0, \
.daudio_master = 4, .audio_format = 1, .signal_inversion = 1, \
.pcm_lrck_period = 64, .slot_width_select = 32, \
.msb_lsb_first	= 0, .frametype = 0, \
.tx_data_mode = 0, .rx_data_mode = 0, \
.tdm_config = 1, .mclk_div = 2,\
.rx_sync_en = false,\
.rx_sync_ctl = false,\
}

struct daudio_pinctrl daudio0_pinctrl[] = {
	DAUDIO0_PIN_MCLK,
	DAUDIO0_PIN_BCLK,
	DAUDIO0_PIN_LRCK,
	DAUDIO0_PIN_DOUT0,
	DAUDIO0_PIN_DIN0,
};

struct pa_config daudio_pa_cfg[] = {
	DAUDIO_PA_PIN0,
};

struct sunxi_daudio_param daudio_param[] = {
	DAUDIO0_PARAMS,
};

/*------------------------ CLK CONFIG FOR SUN20IW2 ---------------------------*/
struct sunxi_daudio_clk {
	struct reset_control *clk_rst;		/* RESET: RST_I2S */
	hal_clk_t clk_bus;			/* CCU: CLK_BUS_I2S */

	/* audio pll -> i2s(24.576MHz)
	 * CLK_PLL_AUDIO --> CLK_PLL_AUDIO1X -----> CLK_AUDPLL_HOSC_SEL --> CLK_I2S
	 *               `-> CLK_PLL_AUDIO2X -----^
	 *                   CLK_HOSC ------------^
	 *                   CLK_DAUDIO_MCLK_IN --^
	 *
	 * audio pll -> i2s(22.5792MHz)
	 * CLK_DPLL1 --> CLK_CK1_AUD --> CLK_CK1_AUD_DIV --> CLK_I2S
	 *
	 * audio pll -> i2s asrc
	 * CK1_M33 --> CK_M33 -------------------> CLK_I2S_ASRC
	 * CK3_M33 --^                           ^
	 * CK1_HIFI5 --> CLK_CKPLL_HIFI5_SEL ----^
	 * CK3_HIFI5 --^                         ^
	 * CK3_HIFI5 ----------------------------^
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

	/* module */
	hal_clk_t clk_i2s;			/* AON_CCU: CLK_I2S */
	hal_clk_t clk_i2s_asrc;			/* AON_CCU: CLK_I2S_ASRC */
};

static inline int snd_sunxi_daudio_clk_enable(struct sunxi_daudio_clk *clk, uint8_t tdm_num)
{
	int ret;

	snd_print("\n");

	/* rst & bus */
	ret = hal_reset_control_deassert(clk->clk_rst);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio clk_rst clk_deassert failed.\n");
		goto err_deassert_clk_rst;
	}
	ret = hal_clock_enable(clk->clk_bus);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio clk_bus enable failed.\n");
		goto err_enable_clk_bus;
	}

	/* pll */
	ret = hal_clock_enable(clk->clk_pll_audio);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio clk_pll_audio enable failed.\n");
		goto err_enable_clk_pll_audio;
	}
	/*
	ret = hal_clock_enable(clk->clk_hosc);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio clk_hosc enable failed.\n");
		goto err_enable_clk_hosc;
	}
	*/
	ret = hal_clock_enable(clk->clk_ck1_aud_div);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio clk_ck1_aud_div enable failed.\n");
		goto err_enable_clk_ck1_aud_div;
	}
	ret = hal_clock_enable(clk->clk_i2s);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio clk_i2s enable failed.\n");
		goto err_enable_clk_i2s;
	}
	ret = hal_clock_enable(clk->clk_i2s_asrc);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio clk_i2s_asrc enable failed.\n");
		goto err_enable_clk_i2s_asrc;
	}

	return HAL_CLK_STATUS_OK;

err_enable_clk_i2s_asrc:
	hal_clock_disable(clk->clk_i2s);
err_enable_clk_i2s:
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

static inline void snd_sunxi_daudio_clk_disable(struct sunxi_daudio_clk *clk)
{
	snd_print("\n");

	hal_clock_disable(clk->clk_i2s_asrc);
	hal_clock_disable(clk->clk_i2s);
	hal_clock_disable(clk->clk_ck1_aud_div);
	/* hal_clock_disable(clk->clk_hosc); */
	hal_clock_disable(clk->clk_pll_audio);
	hal_clock_disable(clk->clk_bus);
	hal_reset_control_assert(clk->clk_rst);

	return;
}

static inline int snd_sunxi_daudio_clk_init(struct sunxi_daudio_clk *clk, uint8_t tdm_num)
{
	int ret;

	snd_print("\n");

	/* rst & bus */
	clk->clk_rst = hal_reset_control_get(HAL_SUNXI_RESET, RST_I2S);
	if (!clk->clk_rst) {
		snd_err("daudio clk_rst hal_reset_control_get failed\n");
		goto err_get_clk_rst;
	}
	clk->clk_bus = hal_clock_get(HAL_SUNXI_CCU, CLK_BUS_I2S);
	if (!clk->clk_bus) {
		snd_err("daudio clk_bus hal_clock_get failed\n");
		goto err_get_clk_bus;
	}

	/* pll clk -> 24.576MHz */
	clk->clk_pll_audio = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_PLL_AUDIO);
	if (!clk->clk_pll_audio) {
		snd_err("daudio clk_pll_audio hal_clock_get failed\n");
		goto err_get_clk_pll_audio;
	}
	clk->clk_pll_audio1x = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_PLL_AUDIO1X);
	if (!clk->clk_pll_audio1x) {
		snd_err("daudio clk_pll_audio1x hal_clock_get failed\n");
		goto err_get_clk_pll_audio1x;
	}
	clk->clk_pll_audio2x = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_PLL_AUDIO2X);
	if (!clk->clk_pll_audio2x) {
		snd_err("daudio clk_pll_audio2x hal_clock_get failed\n");
		goto err_get_clk_pll_audio2x;
	}
	clk->clk_hosc = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_HOSC);
	if (!clk->clk_hosc) {
		snd_err("daudio clk_hosc hal_clock_get failed\n");
		goto err_get_clk_hosc;
	}

	clk->clk_audpll_hosc_sel = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_AUDPLL_HOSC_SEL);
	if (!clk->clk_audpll_hosc_sel) {
		snd_err("daudio clk_audpll_hosc_sel hal_clock_get failed\n");
		goto err_get_clk_audpll_hosc_sel;
	}

	/* pll clk -> 22.5792MHz */
	clk->clk_ck1_aud_div = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_CK1_AUD_DIV);
	if (!clk->clk_ck1_aud_div) {
		snd_err("daudio clk_ck1_aud_div hal_clock_get failed\n");
		goto err_get_clk_ck1_aud_div;
	}

	/* module clk */
	clk->clk_i2s = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_I2S);
	if (!clk->clk_i2s) {
		snd_err("daudio clk_i2s hal_clock_get failed\n");
		goto err_get_clk_i2s;
	}
	clk->clk_i2s_asrc = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_I2S_ASRC);
	if (!clk->clk_i2s_asrc) {
		snd_err("daudio clk_i2s_asrc hal_clock_get failed\n");
		goto err_get_clk_i2s_asrc;
	}

	/* default: (24.576MHz)
	 * CLK_PLL_AUDIO --> CLK_PLL_AUDIO1X --> CLK_AUDPLL_HOSC_SEL --> CLK_I2S
	 *
	 * pll tree: (22.5792MHz)
	 * CLK_DPLL1 --> CLK_CK1_AUD --> CLK_CK1_AUD_DIV --> CLK_I2S
	 */
	ret = hal_clk_set_parent(clk->clk_audpll_hosc_sel, clk->clk_pll_audio1x);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio clk_pll_audio1x -> clk_audpll_hosc_sel clk_set_parent failed.\n");
		goto err_set_parent_clk;
	}
	ret = hal_clk_set_parent(clk->clk_i2s, clk->clk_audpll_hosc_sel);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio clk_audpll_hosc_sel -> clk_i2s clk_set_parent failed.\n");
		goto err_set_parent_clk;
	}

	/* note: Enable and then set the freq to avoid clock lock errors */
	ret = snd_sunxi_daudio_clk_enable(clk, tdm_num);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio snd_sunxi_daudio_clk_enable failed.\n");
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

	return HAL_CLK_STATUS_OK;

err_clk_enable:
err_set_rate_clk:
err_set_parent_clk:
	hal_clock_put(clk->clk_i2s_asrc);
err_get_clk_i2s_asrc:
	hal_clock_put(clk->clk_i2s);
err_get_clk_i2s:
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

static inline void snd_sunxi_daudio_clk_exit(struct sunxi_daudio_clk *clk)
{
	snd_print("\n");

	snd_sunxi_daudio_clk_disable(clk);
	hal_clock_put(clk->clk_i2s_asrc);
	hal_clock_put(clk->clk_i2s);
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

static inline int snd_sunxi_daudio_clk_set_rate(struct sunxi_daudio_clk *clk, int stream,
						unsigned int freq_in, unsigned int freq_out)
{
	int ret;
	//unsigned int freq;

	(void)stream;
	(void)freq_in;

	snd_print("freq_out -> %u\n", freq_out);

	if (freq_out == 24576000) {
		//freq = 24583333;
		ret = hal_clk_set_parent(clk->clk_i2s, clk->clk_audpll_hosc_sel);
		if (ret != HAL_CLK_STATUS_OK) {
			snd_err("daudio clk_audpll_hosc_sel -> clk_i2s clk_set_parent failed.\n");
			return HAL_CLK_STATUS_ERROR;
		}
	} else {
		//freq = 22588236;
		ret = hal_clk_set_parent(clk->clk_i2s, clk->clk_ck1_aud_div);
		if (ret != HAL_CLK_STATUS_OK) {
			snd_err("daudio clk_ck1_aud_div -> clk_i2s clk_set_parent failed.\n");
			return HAL_CLK_STATUS_ERROR;
		}
	}

	/* note: CLK_I2S no need set rate
	ret = hal_clk_set_rate(clk->clk_i2s, freq);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("set clk_i2s rate %u failed\n", freq);
		return HAL_CLK_STATUS_ERROR;
	}
	*/

	return HAL_CLK_STATUS_OK;
}

#endif	/* __SUN20IW2_DAUDIO_H_ */
