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
#ifndef	__SUN8IW20_DAUDIO_H_
#define	__SUN8IW20_DAUDIO_H_

#define	SUNXI_DAUDIO_BASE (0x02032000)

#define DAUDIO_NUM_MAX	3

/*------------------------ CLK CONFIG FOR NORMAL ---------------------------*/
#define SUNXI_DAUDIO_CLK_PLL_AUDIO	CLK_PLL_AUDIO0
#define SUNXI_DAUDIO_CLK_PLL_AUDIO1	CLK_PLL_AUDIO1_DIV5
#define SUNXI_DAUDIO_CLK_I2S_ASRC	CLK_I2S2_ASRC

#define SUNXI_DAUDIO_CLK_I2S0		CLK_I2S0
#define SUNXI_DAUDIO_CLK_BUS_I2S0	CLK_BUS_I2S0
#define SUNXI_DAUDIO_CLK_RST_I2S0	RST_BUS_I2S0

#define SUNXI_DAUDIO_CLK_I2S1		CLK_I2S1
#define SUNXI_DAUDIO_CLK_BUS_I2S1	CLK_BUS_I2S1
#define SUNXI_DAUDIO_CLK_RST_I2S1	RST_BUS_I2S1

#define SUNXI_DAUDIO_CLK_I2S2		CLK_I2S2
#define SUNXI_DAUDIO_CLK_BUS_I2S2	CLK_BUS_I2S2
#define SUNXI_DAUDIO_CLK_RST_I2S2	RST_BUS_I2S2

#define SUNXI_DAUDIO_CLK_I2S3		0
#define SUNXI_DAUDIO_CLK_BUS_I2S3	0
#define SUNXI_DAUDIO_CLK_RST_I2S3	0

/*------------------------ PIN CONFIG FOR NORMAL ---------------------------*/



/*------------------------ PIN CONFIG FOR FPGA VERIFY -----------------------*/
/* daudio0 pin config */
#define DAUDIO0_PIN_MCLK \
{.gpio_pin = GPIOB(29), .mux = 2, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO0_PIN_BCLK \
{.gpio_pin = GPIOB(23), .mux = 2, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO0_PIN_LRCK \
{.gpio_pin = GPIOB(24), .mux = 2, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO0_PIN_DOUT0 \
{.gpio_pin = GPIOB(25), .mux = 2, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO0_PIN_DIN	\
{.gpio_pin = GPIOB(22), .mux = 2, .driv_level = GPIO_DRIVING_LEVEL1}

/* daudio1 pin config */
#define DAUDIO1_PIN_MCLK \
{.gpio_pin = GPIOG(11), .mux = 2, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO1_PIN_BCLK \
{.gpio_pin = GPIOG(13), .mux = 2, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO1_PIN_LRCK \
{.gpio_pin = GPIOG(12), .mux = 2, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO1_PIN_DOUT0 \
{.gpio_pin = GPIOG(15), .mux = 2, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO1_PIN_DIN	\
{.gpio_pin = GPIOG(14), .mux = 2, .driv_level = GPIO_DRIVING_LEVEL1}

/* daudio2 pin config */
#define DAUDIO2_PIN_MCLK \
{.gpio_pin = GPIOB(29), .mux = 4, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO2_PIN_BCLK \
{.gpio_pin = GPIOB(23), .mux = 4, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO2_PIN_LRCK \
{.gpio_pin = GPIOB(24), .mux = 4, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO2_PIN_DOUT0 \
{.gpio_pin = GPIOB(25), .mux = 4, .driv_level = GPIO_DRIVING_LEVEL1}
#define DAUDIO2_PIN_DIN	\
{.gpio_pin = GPIOB(22), .mux = 4, .driv_level = GPIO_DRIVING_LEVEL1}

#define DAUDIO_PA_PIN0 \
{.pin = 0, .level = GPIO_DATA_HIGH, .msleep = 0, .used = false}

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
.pcm_lrck_period = 128, .slot_width_select = 32, \
.msb_lsb_first	= 0, .frametype = 0, \
.tx_data_mode = 0, .rx_data_mode = 0, \
.tdm_config = 1, .mclk_div = 1,\
.rx_sync_en = false,\
.rx_sync_ctl = false,\
}

#define DAUDIO1_PARAMS \
{.tdm_num = 1, \
.daudio_master = 4, .audio_format = 1, .signal_inversion = 1, \
.pcm_lrck_period = 128, .slot_width_select = 32, \
.msb_lsb_first	= 1, .frametype = 1, \
.tx_data_mode = 0, .rx_data_mode = 0, \
.tdm_config = 1, .mclk_div = 1,\
.rx_sync_en = true,\
.rx_sync_ctl = false,\
}

#define DAUDIO2_PARAMS \
{.tdm_num = 2, \
.daudio_master = 4, .audio_format = 1, .signal_inversion = 1, \
.pcm_lrck_period = 128, .slot_width_select = 32, \
.msb_lsb_first	= 0, .frametype = 0, \
.tx_data_mode = 0, .rx_data_mode = 0, \
.tdm_config = 1, .mclk_div = 1,\
.rx_sync_en = false,\
.rx_sync_ctl = false,\
}

struct daudio_pinctrl daudio0_pinctrl[] = {
	DAUDIO0_PIN_MCLK,
	DAUDIO0_PIN_BCLK,
	DAUDIO0_PIN_LRCK,
	DAUDIO0_PIN_DOUT0,
	/* DAUDIO0_PIN_DOUT1, */
	DAUDIO0_PIN_DIN,
};

struct daudio_pinctrl daudio1_pinctrl[] = {
	DAUDIO1_PIN_MCLK,
	DAUDIO1_PIN_BCLK,
	DAUDIO1_PIN_LRCK,
	DAUDIO1_PIN_DOUT0,
	/* DAUDIO1_PIN_DOUT1, */
	DAUDIO1_PIN_DIN,
};

struct daudio_pinctrl daudio2_pinctrl[] = {
	DAUDIO2_PIN_MCLK,
	DAUDIO2_PIN_BCLK,
	DAUDIO2_PIN_LRCK,
	DAUDIO2_PIN_DOUT0,
	/* DAUDIO2_PIN_DOUT1, */
	DAUDIO2_PIN_DIN,
};

struct pa_config daudio_pa_cfg[] = {
	DAUDIO_PA_PIN0,
};

struct sunxi_daudio_param daudio_param[] = {
	DAUDIO0_PARAMS,
	DAUDIO1_PARAMS,
	DAUDIO2_PARAMS,
};

/*------------------------ CLK CONFIG FOR SUN8IW20 ---------------------------*/
struct sunxi_daudio_clk {
	struct reset_control *rstclk;

	hal_clk_t pllclk;
	hal_clk_t moduleclk;
	hal_clk_t busclk;
	hal_clk_t pllclk1;
	hal_clk_t asrcclk;
};

static inline int snd_sunxi_daudio_clk_enable(struct sunxi_daudio_clk *clk, uint8_t tdm_num)
{
	int ret;

	ret = hal_reset_control_deassert(clk->rstclk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio clk_deassert rstclk failed.\n");
		goto err_daudio_rstclk_deassert;
	}

	ret = hal_clock_enable(clk->busclk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio bus clk_enable busclk failed.\n");
		goto err_daudio_busclk_enable;
	}
	ret = hal_clock_enable(clk->pllclk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio%d clk_enable pllclk failed.\n", tdm_num);
		goto err_daudio_pllclk_enable;
	}
	ret = hal_clock_enable(clk->moduleclk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio%d clk_enable moduleclk failed.\n", tdm_num);
		goto err_daudio_moduleclk_enable;
	}
	ret = hal_clock_enable(clk->pllclk1);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio%d clk_enable pllclk1 failed.\n", tdm_num);
		goto err_daudio_moduleclk_enable;
	}
	ret = hal_clock_enable(clk->asrcclk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio%d clk_enable asrcclk failed.\n", tdm_num);
		goto err_daudio_moduleclk_enable;
	}

	return HAL_CLK_STATUS_OK;

err_daudio_moduleclk_enable:
	hal_clock_disable(clk->pllclk);
	hal_clock_put(clk->pllclk);
err_daudio_pllclk_enable:
	hal_clock_disable(clk->busclk);
	hal_clock_put(clk->busclk);
err_daudio_busclk_enable:
	hal_reset_control_assert(clk->rstclk);
	hal_reset_control_put(clk->rstclk);
err_daudio_rstclk_deassert:
	return HAL_CLK_STATUS_ERROR;
}

static inline void snd_sunxi_daudio_clk_disable(struct sunxi_daudio_clk *clk)
{
	hal_clock_disable(clk->busclk);
	hal_clock_disable(clk->moduleclk);
	hal_clock_disable(clk->pllclk);
	hal_clock_disable(clk->pllclk1);
	hal_clock_disable(clk->asrcclk);
	hal_reset_control_assert(clk->rstclk);

	return;
}

static inline int snd_sunxi_daudio_clk_init(struct sunxi_daudio_clk *clk, uint8_t tdm_num)
{
	int ret;
	hal_reset_type_t reset_type = HAL_SUNXI_RESET;
	hal_clk_type_t clk_type = HAL_SUNXI_CCU;

	snd_print("\n");
	switch (tdm_num) {
	case 0:
		clk->moduleclk = hal_clock_get(clk_type, SUNXI_DAUDIO_CLK_I2S0);
		clk->busclk = hal_clock_get(clk_type, SUNXI_DAUDIO_CLK_BUS_I2S0);
		clk->rstclk = hal_reset_control_get(reset_type, SUNXI_DAUDIO_CLK_RST_I2S0);
		break;
#if DAUDIO_NUM_MAX > 1
	case 1:
		clk->moduleclk = hal_clock_get(clk_type, SUNXI_DAUDIO_CLK_I2S1);
		clk->busclk = hal_clock_get(clk_type, SUNXI_DAUDIO_CLK_BUS_I2S1);
		clk->rstclk = hal_reset_control_get(reset_type, SUNXI_DAUDIO_CLK_RST_I2S1);
		break;
#endif
#if DAUDIO_NUM_MAX > 2
	case 2:
		clk->moduleclk = hal_clock_get(clk_type, SUNXI_DAUDIO_CLK_I2S2);
		clk->busclk = hal_clock_get(clk_type, SUNXI_DAUDIO_CLK_BUS_I2S2);
		clk->rstclk = hal_reset_control_get(reset_type, SUNXI_DAUDIO_CLK_RST_I2S2);
		break;
#endif
#if DAUDIO_NUM_MAX > 3
	case 3:
		clk->moduleclk = hal_clock_get(clk_type, SUNXI_DAUDIO_CLK_I2S3);
		clk->busclk = hal_clock_get(clk_type, SUNXI_DAUDIO_CLK_BUS_I2S3);
		clk->rstclk = hal_reset_control_get(reset_type, SUNXI_DAUDIO_CLK_RST_I2S3);
		break;
#endif
	default:
		snd_err("tdm_num:%u overflow\n", tdm_num);
		ret = -EFAULT;
		goto err_daudio_get_moduleclk;
	}

	clk->pllclk = hal_clock_get(clk_type, SUNXI_DAUDIO_CLK_PLL_AUDIO);
	clk->pllclk1 = hal_clock_get(clk_type, SUNXI_DAUDIO_CLK_PLL_AUDIO1);
	clk->asrcclk = hal_clock_get(clk_type, SUNXI_DAUDIO_CLK_I2S_ASRC);

	hal_clk_set_rate(clk->pllclk, 22579200);
	hal_clk_set_rate(clk->pllclk1, 614400000);

	ret = hal_clk_set_parent(clk->asrcclk, clk->pllclk1);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio[%d] asrc clk_set_parent failed.\n", tdm_num);
		goto err_daudio_moduleclk_set_parent;
	}

	ret = hal_clk_set_parent(clk->moduleclk, clk->pllclk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio[%d] clk_set_parent failed.\n", tdm_num);
		goto err_daudio_moduleclk_set_parent;
	}

	ret = snd_sunxi_daudio_clk_enable(clk, tdm_num);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("daudio snd_sunxi_daudio_clk_enable failed.\n");
		goto err_clk_enable;
	}

	return HAL_CLK_STATUS_OK;

err_clk_enable:
err_daudio_moduleclk_set_parent:
err_daudio_get_moduleclk:
	return HAL_CLK_STATUS_ERROR;
}

static inline void snd_sunxi_daudio_clk_exit(struct sunxi_daudio_clk *clk)
{
	snd_sunxi_daudio_clk_disable(clk);

	hal_clock_put(clk->busclk);
	hal_clock_put(clk->moduleclk);
	hal_clock_put(clk->pllclk);
	hal_clock_put(clk->pllclk1);
	hal_clock_put(clk->asrcclk);
	hal_reset_control_put(clk->rstclk);

	return;
}

static inline int snd_sunxi_daudio_clk_set_rate(struct sunxi_daudio_clk *clk, int stream,
						unsigned int freq_in, unsigned int freq_out)
{
	int ret;

	(void)stream;
	(void)freq_in;

	if (freq_out == 24576000) {
		hal_clk_set_parent(clk->moduleclk, clk->pllclk1);
	} else {
		hal_clk_set_parent(clk->moduleclk, clk->pllclk);
	}

	if (hal_clk_set_rate(clk->moduleclk, freq_out)) {
		snd_err("set pllclk rate %u failed\n", freq_out);
		return -EINVAL;
	}

	if (hal_clk_set_rate(clk->asrcclk, 98304000)) {
		snd_err("set pllclk rate %u failed\n", freq_out);
		return -EINVAL;
	}

	return HAL_CLK_STATUS_OK;
}

#endif	/* __SUN8IW20_DAUDIO_H_ */
