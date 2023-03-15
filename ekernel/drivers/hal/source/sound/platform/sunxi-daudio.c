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
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <hal_dma.h>
#include <hal_timer.h>
#ifdef CONFIG_COMPONENTS_PM
#include <pm_devops.h>
#endif
#include <sound/snd_core.h>
#include <sound/snd_pcm.h>
#include <sound/snd_dma.h>
#include <sound/dma_wrap.h>
#include <sound/common/snd_sunxi_common.h>
#ifdef CONFIG_DRIVER_SYSCONFIG
#include <hal_cfg.h>
#include <script.h>
#endif

#include "sunxi-pcm.h"
#include "sunxi-daudio.h"

#if defined(SUNXI_DAUDIO_DEBUG_REG) || defined(CONFIG_COMPONENTS_PM)
static struct audio_reg_label sunxi_reg_labels[] = {
	REG_LABEL(SUNXI_DAUDIO_CTL),
	REG_LABEL(SUNXI_DAUDIO_FMT0),
	REG_LABEL(SUNXI_DAUDIO_FMT1),
	REG_LABEL(SUNXI_DAUDIO_INTSTA),
	/* REG_LABEL(SUNXI_DAUDIO_RXFIFO), */
	REG_LABEL(SUNXI_DAUDIO_FIFOCTL),
	REG_LABEL(SUNXI_DAUDIO_FIFOSTA),
	REG_LABEL(SUNXI_DAUDIO_INTCTL),
	/* REG_LABEL(SUNXI_DAUDIO_TXFIFO), */
	REG_LABEL(SUNXI_DAUDIO_CLKDIV),
	REG_LABEL(SUNXI_DAUDIO_TXCNT),
	REG_LABEL(SUNXI_DAUDIO_RXCNT),
	REG_LABEL(SUNXI_DAUDIO_CHCFG),
	REG_LABEL(SUNXI_DAUDIO_TX0CHSEL),
	REG_LABEL(SUNXI_DAUDIO_TX1CHSEL),
	REG_LABEL(SUNXI_DAUDIO_TX2CHSEL),
	REG_LABEL(SUNXI_DAUDIO_TX3CHSEL),
	REG_LABEL(SUNXI_DAUDIO_TX0CHMAP0),
	REG_LABEL(SUNXI_DAUDIO_TX0CHMAP1),
	REG_LABEL(SUNXI_DAUDIO_TX1CHMAP0),
	REG_LABEL(SUNXI_DAUDIO_TX1CHMAP1),
	REG_LABEL(SUNXI_DAUDIO_TX2CHMAP0),
	REG_LABEL(SUNXI_DAUDIO_TX2CHMAP1),
	REG_LABEL(SUNXI_DAUDIO_TX3CHMAP0),
	REG_LABEL(SUNXI_DAUDIO_TX3CHMAP1),
	REG_LABEL(SUNXI_DAUDIO_RXCHSEL),
	REG_LABEL(SUNXI_DAUDIO_RXCHMAP0),
	REG_LABEL(SUNXI_DAUDIO_RXCHMAP1),
	REG_LABEL(SUNXI_DAUDIO_RXCHMAP2),
	REG_LABEL(SUNXI_DAUDIO_RXCHMAP3),
	REG_LABEL(SUNXI_DAUDIO_DEBUG),
	REG_LABEL_END,
};
#endif
static int snd_sunxi_pa_pin_init(struct snd_platform *platform);
/*static void snd_sunxi_pa_pin_exit(struct snd_platform *platform);*/
static int snd_sunxi_pa_pin_enable(struct snd_platform *platform);
static void snd_sunxi_pa_pin_disable(struct snd_platform *platform);

static void sunxi_daudio_txctrl_enable(struct snd_platform *platform, bool enable)
{
	snd_print("\n");
	if (enable) {
		snd_platform_update_bits(platform, SUNXI_DAUDIO_CTL,
					(1 << SDO0_EN) | (1 << CTL_TXEN),
					(1 << SDO0_EN) | (1 << CTL_TXEN));
		snd_platform_update_bits(platform, SUNXI_DAUDIO_INTCTL,
					(1 << TXDRQEN), (1 << TXDRQEN));
	} else {
		snd_platform_update_bits(platform, SUNXI_DAUDIO_INTCTL,
					(1 << TXDRQEN), (0 << TXDRQEN));
		snd_platform_update_bits(platform, SUNXI_DAUDIO_CTL,
					(1 << SDO0_EN) | (1 << CTL_TXEN),
					(0 << SDO0_EN) | (0 << CTL_TXEN));
	}
}

static void sunxi_daudio_rxctrl_enable(struct snd_platform *platform, bool enable)
{
	snd_print("\n");
	if (enable) {
		snd_platform_update_bits(platform, SUNXI_DAUDIO_CTL,
				(1 << CTL_RXEN), (1 << CTL_RXEN));
		snd_platform_update_bits(platform, SUNXI_DAUDIO_INTCTL,
				(1 << RXDRQEN), (1 << RXDRQEN));
	} else {
		snd_platform_update_bits(platform, SUNXI_DAUDIO_INTCTL,
				(1 << RXDRQEN), (0 << RXDRQEN));
		snd_platform_update_bits(platform, SUNXI_DAUDIO_CTL,
				(1 << CTL_RXEN), (0 << CTL_RXEN));
	}
}

static int sunxi_daudio_global_enable(struct snd_platform *platform, bool enable)
{
	struct sunxi_daudio_info *sunxi_daudio = platform->private_data;
	/* uint32_t sr_level = 0; */

	snd_print("\n");
	/* sr_level = freert_write_lock_irqsave(&sunxi_daudio->gen_lock); */
	if (enable) {
		if (sunxi_daudio->global_enable++ == 0)
			snd_platform_update_bits(platform, SUNXI_DAUDIO_CTL,
					(1 << GLOBAL_EN), (1 << GLOBAL_EN));
	} else {
		if (--sunxi_daudio->global_enable == 0)
			snd_platform_update_bits(platform, SUNXI_DAUDIO_CTL,
					(1 << GLOBAL_EN), (0 << GLOBAL_EN));
	}
	/* freert_write_unlock_irqsave(&sunxi_daudio->gen_lock, sr_level); */

	return 0;
}

static void sunxi_rx_sync_enable(void *data, bool enable)
{
	struct snd_dai *dai = data;
	struct snd_platform *platform = dai->component;

	if (enable) {
		snd_platform_update_bits(platform, SUNXI_DAUDIO_CTL,
					 1 << RX_SYNC_EN_STA, 1 << RX_SYNC_EN_STA);
	} else {
		snd_platform_update_bits(platform, SUNXI_DAUDIO_CTL,
					 1 << RX_SYNC_EN_STA, 0 << RX_SYNC_EN_STA);
	}

}

static int sunxi_daudio_startup(struct snd_pcm_substream *substream,
				struct snd_dai *dai)
{
	struct snd_platform *platform = dai->component;
	struct sunxi_daudio_info *sunxi_daudio = platform->private_data;

	snd_print("\n");
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		dai->playback_dma_data = &sunxi_daudio->playback_dma_param;
	} else {
		dai->capture_dma_data = &sunxi_daudio->capture_dma_param;
	}

	snd_sunxi_pa_pin_enable(platform);

	return 0;
}

static int sunxi_daudio_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params, struct snd_dai *dai)
{
	struct snd_platform *platform = dai->component;
	struct sunxi_daudio_info *sunxi_daudio = platform->private_data;
	struct sunxi_daudio_param *param = &sunxi_daudio->param;
	int ret = 0;

	snd_print("\n");
	/* set cpu dai fmt */
	ret = snd_soc_dai_set_fmt(dai, param->audio_format
			| (param->signal_inversion << SND_SOC_DAIFMT_SIG_SHIFT)
			| (param->daudio_master << SND_SOC_DAIFMT_MASTER_SHIFT));
	if (ret < 0) {
		snd_err("cpu dai set fmt failed.\n");
		return -EINVAL;
	}

	switch (params_format(params)) {
	case SND_PCM_FORMAT_S16_LE:
		snd_platform_update_bits(platform,
			SUNXI_DAUDIO_FMT0,
			(SUNXI_DAUDIO_SR_MASK<<DAUDIO_SAMPLE_RESOLUTION),
			(SUNXI_DAUDIO_SR_16BIT<<DAUDIO_SAMPLE_RESOLUTION));
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			snd_platform_update_bits(platform,
				SUNXI_DAUDIO_FIFOCTL,
				(SUNXI_DAUDIO_TXIM_MASK<<TXIM),
				(SUNXI_DAUDIO_TXIM_VALID_LSB<<TXIM));
		else
			snd_platform_update_bits(platform,
				SUNXI_DAUDIO_FIFOCTL,
				(SUNXI_DAUDIO_RXOM_MASK<<RXOM),
				(SUNXI_DAUDIO_RXOM_EXPH<<RXOM));
		break;
	case SND_PCM_FORMAT_S24_LE:
		snd_platform_update_bits(platform, SUNXI_DAUDIO_FMT0,
				(SUNXI_DAUDIO_SR_MASK<<DAUDIO_SAMPLE_RESOLUTION),
				(SUNXI_DAUDIO_SR_24BIT<<DAUDIO_SAMPLE_RESOLUTION));
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			snd_platform_update_bits(platform,
					SUNXI_DAUDIO_FIFOCTL,
					(SUNXI_DAUDIO_TXIM_MASK<<TXIM),
					(SUNXI_DAUDIO_TXIM_VALID_LSB<<TXIM));
		else
			snd_platform_update_bits(platform,
					SUNXI_DAUDIO_FIFOCTL,
					(SUNXI_DAUDIO_RXOM_MASK<<RXOM),
					(SUNXI_DAUDIO_RXOM_EXP0<<RXOM));
		break;
	case SND_PCM_FORMAT_S32_LE:
		snd_platform_update_bits(platform, SUNXI_DAUDIO_FMT0,
				(SUNXI_DAUDIO_SR_MASK<<DAUDIO_SAMPLE_RESOLUTION),
				(SUNXI_DAUDIO_SR_32BIT<<DAUDIO_SAMPLE_RESOLUTION));
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			snd_platform_update_bits(platform,
					SUNXI_DAUDIO_FIFOCTL,
					(SUNXI_DAUDIO_TXIM_MASK<<TXIM),
					(SUNXI_DAUDIO_TXIM_VALID_LSB<<TXIM));
		else
			snd_platform_update_bits(platform,
					SUNXI_DAUDIO_FIFOCTL,
					(SUNXI_DAUDIO_RXOM_MASK<<RXOM),
					(SUNXI_DAUDIO_RXOM_EXPH<<RXOM));
		break;
	default:
		snd_err("unrecognized format\n");
		return -EINVAL;
	}

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		snd_platform_update_bits(platform, SUNXI_DAUDIO_CHCFG,
				(SUNXI_DAUDIO_TX_SLOT_MASK<<TX_SLOT_NUM),
				((params_channels(params)-1)<<TX_SLOT_NUM));
		snd_platform_write(platform,
			SUNXI_DAUDIO_TX0CHMAP0, SUNXI_DEFAULT_CHMAP0);
		snd_platform_write(platform,
			SUNXI_DAUDIO_TX0CHMAP1, SUNXI_DEFAULT_CHMAP1);
		snd_platform_update_bits(platform,
			SUNXI_DAUDIO_TX0CHSEL,
			(SUNXI_DAUDIO_TX_CHSEL_MASK<<TX_CHSEL),
			((params_channels(params)-1)<<TX_CHSEL));
		snd_platform_update_bits(platform,
			SUNXI_DAUDIO_TX0CHSEL,
			(SUNXI_DAUDIO_TX_CHEN_MASK<<TX_CHEN),
			((1<<params_channels(params))-1)<<TX_CHEN);
	} else {
		unsigned int SUNXI_DAUDIO_RXCHMAPX = 0;
		int index = 0;

		for (index = 0; index < 16; index++) {
			if (index >= 12)
				SUNXI_DAUDIO_RXCHMAPX = SUNXI_DAUDIO_RXCHMAP0;
			else if (index >= 8)
				SUNXI_DAUDIO_RXCHMAPX = SUNXI_DAUDIO_RXCHMAP1;
			else if (index >= 4)
				SUNXI_DAUDIO_RXCHMAPX = SUNXI_DAUDIO_RXCHMAP2;
			else
				SUNXI_DAUDIO_RXCHMAPX = SUNXI_DAUDIO_RXCHMAP3;
			snd_platform_update_bits(platform,
				SUNXI_DAUDIO_RXCHMAPX,
				DAUDIO_RXCHMAP(index),
				DAUDIO_RXCH_DEF_MAP(index));
		}
		snd_platform_update_bits(platform, SUNXI_DAUDIO_CHCFG,
				(SUNXI_DAUDIO_RX_SLOT_MASK<<RX_SLOT_NUM),
				((params_channels(params)-1)<<RX_SLOT_NUM));
		snd_platform_update_bits(platform, SUNXI_DAUDIO_RXCHSEL,
				(SUNXI_DAUDIO_RX_CHSEL_MASK<<RX_CHSEL),
				((params_channels(params)-1)<<RX_CHSEL));
	}

	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
		if (sunxi_daudio->asrc_en) {
			snd_print("sunxi daudio asrc enabled\n");
			snd_platform_update_bits(platform, SUNXI_DAUDIO_ASRC_MCLKCFG,
						(0x1 << DAUDIO_ASRC_MCLK_GATE),
						(0x1 << DAUDIO_ASRC_MCLK_GATE));
			snd_platform_update_bits(platform, SUNXI_DAUDIO_ASRC_MCLKCFG,
						(0xF << DAUDIO_ASRC_MCLK_RATIO),
						(0x1 << DAUDIO_ASRC_MCLK_RATIO));
			snd_platform_update_bits(platform, SUNXI_DAUDIO_ASRC_FSOUTCFG,
						(0x1 << DAUDIO_ASRC_FSOUT_GATE),
						(0x1 << DAUDIO_ASRC_FSOUT_GATE));
			snd_platform_update_bits(platform, SUNXI_DAUDIO_ASRC_FSOUTCFG,
						(0xF << DAUDIO_ASRC_FSOUT_CLKSRC),
						(0x0 << DAUDIO_ASRC_FSOUT_CLKSRC));
			snd_platform_update_bits(platform, SUNXI_DAUDIO_ASRC_FSOUTCFG,
						(0xF << DAUDIO_ASRC_FSOUT_CLKDIV1),
						(0xF << DAUDIO_ASRC_FSOUT_CLKDIV1));
			snd_platform_update_bits(platform, SUNXI_DAUDIO_ASRC_FSOUTCFG,
						(0xF << DAUDIO_ASRC_FSOUT_CLKDIV2),
						(0xA << DAUDIO_ASRC_FSOUT_CLKDIV2));
			snd_platform_update_bits(platform, SUNXI_DAUDIO_ASRC_FSIN_EXTCFG,
						(0x1 << DAUDIO_ASRC_FSIN_EXTEN),
						(0x1 << DAUDIO_ASRC_FSIN_EXTEN));
			snd_platform_update_bits(platform, SUNXI_DAUDIO_ASRC_FSIN_EXTCFG,
						(0xFF << DAUDIO_ASRC_FSIN_EXTCYCLE),
						(0xA << DAUDIO_ASRC_FSIN_EXTCYCLE));
			/* regmap_write(platform, SUNXI_DAUDIO_ASRC_MANCFG, 0); */
		}
	}

	return 0;
}

static int sunxi_daudio_set_sysclk(struct snd_dai *dai,
				int clk_id, unsigned int freq, int dir)
{
	int ret;
	struct snd_platform *platform = dai->component;
	struct sunxi_daudio_info *sunxi_daudio = platform->private_data;

	snd_print("\n");
	ret = snd_sunxi_daudio_clk_set_rate(&sunxi_daudio->clk, dir, freq, freq);
	if (ret < 0) {
		snd_err("snd_sunxi_daudio_clk_set_rate failed\n");
		return -1;
	}

	return 0;
}

static int sunxi_daudio_set_clkdiv(struct snd_dai *dai,
				int clk_id, int clk_div)
{
	struct snd_platform *platform = dai->component;
	struct sunxi_daudio_info *sunxi_daudio = platform->private_data;
	struct sunxi_daudio_param *param = &sunxi_daudio->param;
	unsigned int bclk_div, div_ratio;

	snd_print("\n");
	if (param->tdm_config)
		/* I2S/TDM two channel mode */
		div_ratio = clk_div/(2 * param->pcm_lrck_period);
	else
		/* PCM mode */
		div_ratio = clk_div / param->pcm_lrck_period;

	switch (div_ratio) {
	case	1:
		bclk_div = SUNXI_DAUDIO_BCLK_DIV_1;
		break;
	case	2:
		bclk_div = SUNXI_DAUDIO_BCLK_DIV_2;
		break;
	case	4:
		bclk_div = SUNXI_DAUDIO_BCLK_DIV_3;
		break;
	case	6:
		bclk_div = SUNXI_DAUDIO_BCLK_DIV_4;
		break;
	case	8:
		bclk_div = SUNXI_DAUDIO_BCLK_DIV_5;
		break;
	case	12:
		bclk_div = SUNXI_DAUDIO_BCLK_DIV_6;
		break;
	case	16:
		bclk_div = SUNXI_DAUDIO_BCLK_DIV_7;
		break;
	case	24:
		bclk_div = SUNXI_DAUDIO_BCLK_DIV_8;
		break;
	case	32:
		bclk_div = SUNXI_DAUDIO_BCLK_DIV_9;
		break;
	case	48:
		bclk_div = SUNXI_DAUDIO_BCLK_DIV_10;
		break;
	case	64:
		bclk_div = SUNXI_DAUDIO_BCLK_DIV_11;
		break;
	case	96:
		bclk_div = SUNXI_DAUDIO_BCLK_DIV_12;
		break;
	case	128:
		bclk_div = SUNXI_DAUDIO_BCLK_DIV_13;
		break;
	case	176:
		bclk_div = SUNXI_DAUDIO_BCLK_DIV_14;
		break;
	case	192:
		bclk_div = SUNXI_DAUDIO_BCLK_DIV_15;
		break;
	default:
		snd_err("unsupport clk_div\n");
		return -EINVAL;
	}
	/* setting bclk to driver external codec bit clk */
	snd_platform_update_bits(platform, SUNXI_DAUDIO_CLKDIV,
			(SUNXI_DAUDIO_BCLK_DIV_MASK<<BCLK_DIV),
			(bclk_div<<BCLK_DIV));

	return 0;
}

static int sunxi_daudio_init_fmt(struct snd_platform *platform, unsigned int fmt)
{
	unsigned int offset, mode;
	unsigned int lrck_polarity, brck_polarity;

	snd_print("\n");
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case	SND_SOC_DAIFMT_CBM_CFM:
		snd_platform_update_bits(platform, SUNXI_DAUDIO_CTL,
				(SUNXI_DAUDIO_LRCK_OUT_MASK<<LRCK_OUT),
				(SUNXI_DAUDIO_LRCK_OUT_DISABLE<<LRCK_OUT));
		break;
	case	SND_SOC_DAIFMT_CBS_CFS:
		snd_platform_update_bits(platform, SUNXI_DAUDIO_CTL,
				(SUNXI_DAUDIO_LRCK_OUT_MASK<<LRCK_OUT),
				(SUNXI_DAUDIO_LRCK_OUT_ENABLE<<LRCK_OUT));
		break;
	default:
		snd_err("unknown maser/slave format\n");
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case	SND_SOC_DAIFMT_I2S:
		offset = SUNXI_DAUDIO_TX_OFFSET_1;
		mode = SUNXI_DAUDIO_MODE_CTL_I2S;
		break;
	case	SND_SOC_DAIFMT_RIGHT_J:
		offset = SUNXI_DAUDIO_TX_OFFSET_0;
		mode = SUNXI_DAUDIO_MODE_CTL_RIGHT;
		break;
	case	SND_SOC_DAIFMT_LEFT_J:
		offset = SUNXI_DAUDIO_TX_OFFSET_0;
		mode = SUNXI_DAUDIO_MODE_CTL_LEFT;
		break;
	case	SND_SOC_DAIFMT_DSP_A:
		offset = SUNXI_DAUDIO_TX_OFFSET_1;
		mode = SUNXI_DAUDIO_MODE_CTL_PCM;
		break;
	case	SND_SOC_DAIFMT_DSP_B:
		offset = SUNXI_DAUDIO_TX_OFFSET_0;
		mode = SUNXI_DAUDIO_MODE_CTL_PCM;
		break;
	default:
		snd_err("format setting failed\n");
		return -EINVAL;
	}

	snd_platform_update_bits(platform, SUNXI_DAUDIO_CTL,
			(SUNXI_DAUDIO_MODE_CTL_MASK<<MODE_SEL),
			(mode<<MODE_SEL));
	snd_platform_update_bits(platform, SUNXI_DAUDIO_TX0CHSEL,
			(SUNXI_DAUDIO_TX_OFFSET_MASK<<TX_OFFSET),
			(offset<<TX_OFFSET));
	snd_platform_update_bits(platform, SUNXI_DAUDIO_RXCHSEL,
			(SUNXI_DAUDIO_RX_OFFSET_MASK<<RX_OFFSET),
			(offset<<RX_OFFSET));

	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		lrck_polarity = SUNXI_DAUDIO_LRCK_POLARITY_NOR;
		brck_polarity = SUNXI_DAUDIO_BCLK_POLARITY_NOR;
		break;
	case SND_SOC_DAIFMT_NB_IF:
		lrck_polarity = SUNXI_DAUDIO_LRCK_POLARITY_INV;
		brck_polarity = SUNXI_DAUDIO_BCLK_POLARITY_NOR;
		break;
	case SND_SOC_DAIFMT_IB_NF:
		lrck_polarity = SUNXI_DAUDIO_LRCK_POLARITY_NOR;
		brck_polarity = SUNXI_DAUDIO_BCLK_POLARITY_INV;
		break;
	case SND_SOC_DAIFMT_IB_IF:
		lrck_polarity = SUNXI_DAUDIO_LRCK_POLARITY_INV;
		brck_polarity = SUNXI_DAUDIO_BCLK_POLARITY_INV;
		break;
	default:
		snd_err("invert clk setting failed\n");
		return -EINVAL;
	}

	snd_platform_update_bits(platform, SUNXI_DAUDIO_FMT0,
			(1<<LRCK_POLARITY), (lrck_polarity<<LRCK_POLARITY));
	snd_platform_update_bits(platform, SUNXI_DAUDIO_FMT0,
			(1<<BRCK_POLARITY), (brck_polarity<<BRCK_POLARITY));

	return 0;
}

static int sunxi_daudio_set_fmt(struct snd_dai *dai, unsigned int fmt)
{
	struct snd_platform *platform = dai->component;

	snd_print("\n");
	sunxi_daudio_init_fmt(platform, fmt);

	return 0;
}

static int sunxi_daudio_prepare(struct snd_pcm_substream *substream,
				struct snd_dai *dai)
{
	struct snd_platform *platform = dai->component;
	struct sunxi_daudio_info *sunxi_daudio = platform->private_data;
	unsigned int i;

	snd_print("\n");
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		for (i = 0 ; i < SUNXI_DAUDIO_FTX_TIMES ; i++) {
			snd_platform_update_bits(platform,
				SUNXI_DAUDIO_FIFOCTL,
				(1 << FIFO_CTL_FTX), (1 << FIFO_CTL_FTX));
			hal_usleep(1000);
		}
		snd_platform_write(platform, SUNXI_DAUDIO_TXCNT, 0);
		if (sunxi_daudio->hub_mode && !sunxi_daudio->playback_en) {
			snd_platform_update_bits(platform,
					SUNXI_DAUDIO_CTL,
					(1 << SDO0_EN) | (1 << CTL_TXEN),
					(1 << SDO0_EN) | (1 << CTL_TXEN));
			sunxi_daudio_global_enable(platform, true);
			sunxi_daudio->playback_en = 1;
		}
	} else {
		snd_platform_update_bits(platform, SUNXI_DAUDIO_FIFOCTL,
				(1<<FIFO_CTL_FRX), (1<<FIFO_CTL_FRX));
		snd_platform_write(platform, SUNXI_DAUDIO_RXCNT, 0);
	}

	return 0;
}

static int sunxi_daudio_trigger(struct snd_pcm_substream *substream,
				int cmd, struct snd_dai *dai)
{
	struct snd_platform *platform = dai->component;
	struct sunxi_daudio_info *sunxi_daudio = platform->private_data;

	snd_print("\n");
	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			sunxi_daudio_txctrl_enable(platform, true);
			if (!sunxi_daudio->playback_en) {
				/* Global enable I2S/TDM module */
				sunxi_daudio_global_enable(platform, true);
				sunxi_daudio->playback_en = 1;
			}
		} else {
			sunxi_daudio_rxctrl_enable(platform, true);
			/* Global enable I2S/TDM module */
			sunxi_daudio_global_enable(platform, true);
			if (sunxi_daudio->param.rx_sync_en && sunxi_daudio->param.rx_sync_ctl) {
				sunxi_rx_sync_control(sunxi_daudio->param.rx_sync_domain,
						      sunxi_daudio->param.rx_sync_id, true);
			}
		}
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			sunxi_daudio_txctrl_enable(platform, false);
			if (sunxi_daudio->playback_en) {
				/* Global enable I2S/TDM module */
				sunxi_daudio_global_enable(platform, false);
				sunxi_daudio->playback_en = 0;
			}
		} else {
			sunxi_daudio_rxctrl_enable(platform, false);
			/* Global enable I2S/TDM module */
			sunxi_daudio_global_enable(platform, false);
			if (sunxi_daudio->param.rx_sync_en && sunxi_daudio->param.rx_sync_ctl) {
				sunxi_rx_sync_control(sunxi_daudio->param.rx_sync_domain,
						      sunxi_daudio->param.rx_sync_id, false);
			}
		}
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static void sunxi_daudio_shutdown(struct snd_pcm_substream *substream,
				struct snd_dai *dai)
{
	struct snd_platform *platform = dai->component;
	struct sunxi_daudio_info *sunxi_daudio = platform->private_data;

	snd_print("\n");

	snd_sunxi_pa_pin_disable(platform);

	if ((substream->stream == SNDRV_PCM_STREAM_PLAYBACK) && (sunxi_daudio->hub_mode)) {
		snd_platform_update_bits(platform, SUNXI_DAUDIO_CTL,
				(1 << SDO0_EN) | (1 << CTL_TXEN),
				(0 << SDO0_EN) | (0 << CTL_TXEN));
		if (sunxi_daudio->playback_en) {
			sunxi_daudio_global_enable(platform, false);
			sunxi_daudio->playback_en = 0;
		}
		return;
	}
}

static struct snd_dai_ops sunxi_daudio_dai_ops = {
	.startup = sunxi_daudio_startup,
	.set_sysclk = sunxi_daudio_set_sysclk,
	.set_clkdiv = sunxi_daudio_set_clkdiv,
	.set_fmt = sunxi_daudio_set_fmt,
	.hw_params = sunxi_daudio_hw_params,
	.prepare = sunxi_daudio_prepare,
	.trigger = sunxi_daudio_trigger,
	.shutdown = sunxi_daudio_shutdown,
};

static int sunxi_daudio_mclk_setting(struct snd_platform *platform)
{
	struct sunxi_daudio_info *sunxi_daudio = platform->private_data;
	struct sunxi_daudio_param *param = &sunxi_daudio->param;
	uint16_t mclk_div;

	snd_print("\n");
	if (!param->mclk_div) {
		snd_platform_update_bits(platform, SUNXI_DAUDIO_CLKDIV,
					(1 << MCLKOUT_EN), (0 << MCLKOUT_EN));
		return 0;
	}

	switch (param->mclk_div) {
	case 1:
		mclk_div = SUNXI_DAUDIO_MCLK_DIV_1;
		break;
	case 2:
		mclk_div = SUNXI_DAUDIO_MCLK_DIV_2;
		break;
	case 4:
		mclk_div = SUNXI_DAUDIO_MCLK_DIV_3;
		break;
	case 6:
		mclk_div = SUNXI_DAUDIO_MCLK_DIV_4;
		break;
	case 8:
		mclk_div = SUNXI_DAUDIO_MCLK_DIV_5;
		break;
	case 12:
		mclk_div = SUNXI_DAUDIO_MCLK_DIV_6;
		break;
	case 16:
		mclk_div = SUNXI_DAUDIO_MCLK_DIV_7;
		break;
	case 24:
		mclk_div = SUNXI_DAUDIO_MCLK_DIV_8;
		break;
	case 32:
		mclk_div = SUNXI_DAUDIO_MCLK_DIV_9;
		break;
	case 48:
		mclk_div = SUNXI_DAUDIO_MCLK_DIV_10;
		break;
	case 64:
		mclk_div = SUNXI_DAUDIO_MCLK_DIV_11;
		break;
	case 96:
		mclk_div = SUNXI_DAUDIO_MCLK_DIV_12;
		break;
	case 128:
		mclk_div = SUNXI_DAUDIO_MCLK_DIV_13;
		break;
	case 176:
		mclk_div = SUNXI_DAUDIO_MCLK_DIV_14;
		break;
	case 192:
		mclk_div = SUNXI_DAUDIO_MCLK_DIV_15;
		break;
	default:
		snd_err("unsupport  mclk_div\n");
		return -EINVAL;
	}

	snd_platform_update_bits(platform, SUNXI_DAUDIO_CLKDIV,
			(SUNXI_DAUDIO_MCLK_DIV_MASK<<MCLK_DIV),
			(mclk_div<<MCLK_DIV));
	snd_platform_update_bits(platform, SUNXI_DAUDIO_CLKDIV,
				(1<<MCLKOUT_EN), (1<<MCLKOUT_EN));

	return 0;
}

static int sunxi_daudio_init(struct snd_platform *platform)
{
	struct sunxi_daudio_info *sunxi_daudio = platform->private_data;
	struct sunxi_daudio_param *param = &sunxi_daudio->param;

	snd_print("\n");
	snd_platform_update_bits(platform, SUNXI_DAUDIO_FMT0,
				(1 << LRCK_WIDTH),
				(param->frametype << LRCK_WIDTH));
	snd_platform_update_bits(platform, SUNXI_DAUDIO_FMT0,
				(SUNXI_DAUDIO_LRCK_PERIOD_MASK) << LRCK_PERIOD,
				((param->pcm_lrck_period - 1) << LRCK_PERIOD));
	snd_platform_update_bits(platform, SUNXI_DAUDIO_FMT0,
				(SUNXI_DAUDIO_SLOT_WIDTH_MASK) << SLOT_WIDTH,
				(((param->slot_width_select >> 2) - 1) << SLOT_WIDTH));

	snd_platform_update_bits(platform, SUNXI_DAUDIO_FMT1,
				(0x1 << TX_MLS),
				(param->msb_lsb_first << TX_MLS));
	snd_platform_update_bits(platform, SUNXI_DAUDIO_FMT1,
				(0x1 << RX_MLS),
				(param->msb_lsb_first << RX_MLS));
	snd_platform_update_bits(platform, SUNXI_DAUDIO_FMT1,
				(0x3 << SEXT),
				(param->sign_extend << SEXT));
	snd_platform_update_bits(platform, SUNXI_DAUDIO_FMT1,
				(0x3 << TX_PDM),
				(param->tx_data_mode << TX_PDM));
	snd_platform_update_bits(platform, SUNXI_DAUDIO_FMT1,
				(0x3 << RX_PDM),
				(param->rx_data_mode << RX_PDM));

	snd_platform_update_bits(platform, SUNXI_DAUDIO_CTL,
				 1 <<  RX_SYNC_EN, 0 << RX_SYNC_EN);

	return sunxi_daudio_mclk_setting(platform);
}

static int sunxi_daudio_dai_probe(struct snd_dai *dai)
{
	struct snd_platform *platform = dai->component;

	snd_print("\n");
	dai->playback_dma_data = 0;
	dai->capture_dma_data = 0;

	sunxi_daudio_init(platform);

	return 0;
}

static struct snd_dai sunxi_daudio_dai = {
	.id		= 1,
	.name		= "sunxi-daudio-cpudai",
	.playback	= {
		.stream_name	= "Playback",
		.channels_min	= 1,
		.channels_max	= 8,
		.rates		= SUNXI_DAUDIO_RATES,
		.formats	= SNDRV_PCM_FMTBIT_S16_LE
				| SNDRV_PCM_FMTBIT_S24_LE
				| SNDRV_PCM_FMTBIT_S32_LE,
		.rate_min	= 8000,
		.rate_max	= 192000,
	},
	.capture	= {
		.stream_name	= "Capture",
		.channels_min	= 1,
		.channels_max	= 8,
		.rates		= SUNXI_DAUDIO_RATES,
		.formats	= SNDRV_PCM_FMTBIT_S16_LE
				| SNDRV_PCM_FMTBIT_S24_LE
				| SNDRV_PCM_FMTBIT_S32_LE,
		.rate_min	= 8000,
		.rate_max	= 192000,
	},
	.probe		= sunxi_daudio_dai_probe,
	.ops		= &sunxi_daudio_dai_ops,
};

static int sunxi_get_tx_hub_mode(struct snd_kcontrol *kcontrol, struct snd_ctl_info *info)
{
	unsigned int reg_val;
	unsigned long value = 0;

	snd_print("\n");
	if (kcontrol->type != SND_CTL_ELEM_TYPE_ENUMERATED) {
		snd_err("invalid kcontrol type = %d.\n", kcontrol->type);
		return -EINVAL;
	}

	if (kcontrol->private_data_type == SND_MODULE_PLATFORM) {
		struct snd_platform *platform = kcontrol->private_data;

		reg_val = snd_platform_read(platform, SUNXI_DAUDIO_FIFOCTL);
		value = ((reg_val & (1 << HUB_EN)) ? 1 : 0);
	} else {
		snd_err("invalid kcontrol data type = %d.\n", kcontrol->private_data_type);
		return -EINVAL;
	}

	snd_kcontrol_to_snd_ctl_info(kcontrol, info, value);

	return 0;
}

static int sunxi_set_tx_hub_mode(struct snd_kcontrol *kcontrol, unsigned long value)
{
	snd_print("\n");
	if (kcontrol->type != SND_CTL_ELEM_TYPE_ENUMERATED) {
		snd_err("invalid kcontrol type = %d.\n", kcontrol->type);
		return -EINVAL;
	}

	if (value >= kcontrol->items) {
		snd_err("invalid kcontrol items = %ld.\n", value);
		return -EINVAL;
	}

	if (kcontrol->private_data_type == SND_MODULE_PLATFORM) {
		struct snd_platform *platform = kcontrol->private_data;
		struct sunxi_daudio_info *sunxi_daudio = platform->private_data;

		snd_platform_update_bits(platform, SUNXI_DAUDIO_FIFOCTL,
					 1 << HUB_EN, value << HUB_EN);
		sunxi_daudio->hub_mode = value;
	} else {
		snd_err("invalid kcontrol data type = %d.\n", kcontrol->private_data_type);
	}
	snd_info("mask:0x%x, items:%d, value:0x%x\n", kcontrol->mask, kcontrol->items, value);

	return 0;
}

static int sunxi_get_rx_sync_mode(struct snd_kcontrol *kcontrol, struct snd_ctl_info *info)
{
	unsigned long value = 0;

	snd_print("\n");
	if (kcontrol->type != SND_CTL_ELEM_TYPE_ENUMERATED) {
		snd_err("invalid kcontrol type = %d.\n", kcontrol->type);
		return -EINVAL;
	}

	if (kcontrol->private_data_type == SND_MODULE_PLATFORM) {
		struct snd_platform *platform = kcontrol->private_data;
		struct sunxi_daudio_info *sunxi_daudio = platform->private_data;

		value = sunxi_daudio->param.rx_sync_ctl;
	} else {
		snd_err("invalid kcontrol data type = %d.\n", kcontrol->private_data_type);
		return -EINVAL;
	}

	snd_kcontrol_to_snd_ctl_info(kcontrol, info, value);

	return 0;
}

static int sunxi_set_rx_sync_mode(struct snd_kcontrol *kcontrol, unsigned long value)
{
	struct snd_platform *platform = kcontrol->private_data;
	struct sunxi_daudio_info *sunxi_daudio = platform->private_data;
	struct sunxi_daudio_param *param = &sunxi_daudio->param;

	snd_print("\n");
	if (kcontrol->type != SND_CTL_ELEM_TYPE_ENUMERATED) {
		snd_err("invalid kcontrol type = %d.\n", kcontrol->type);
		return -EINVAL;
	}

	if (value >= kcontrol->items) {
		snd_err("invalid kcontrol items = %ld.\n", value);
		return -EINVAL;
	}

	if (kcontrol->private_data_type != SND_MODULE_PLATFORM) {
		snd_err("invalid kcontrol data type = %d.\n", kcontrol->private_data_type);
		return 0;
	}

	if (value) {
		if (param->rx_sync_ctl) {
			return 0;
		}
		param->rx_sync_ctl = true;
		snd_platform_update_bits(platform, SUNXI_DAUDIO_CTL, 1 <<  RX_SYNC_EN, 1 << RX_SYNC_EN);
		sunxi_rx_sync_startup(param->rx_sync_domain, param->rx_sync_id, (void *)platform->cpu_dai,
				      sunxi_rx_sync_enable);
	} else {
		if (!param->rx_sync_ctl) {
			return 0;
		}
		sunxi_rx_sync_shutdown(param->rx_sync_domain, param->rx_sync_id);
		snd_platform_update_bits(platform, SUNXI_DAUDIO_CTL, 1 <<  RX_SYNC_EN, 0 << RX_SYNC_EN);
		sunxi_daudio->param.rx_sync_ctl = false;
	}

	snd_info("mask:0x%x, items:%d, value:0x%x\n", kcontrol->mask, kcontrol->items, value);

	return 0;
}

static int sunxi_daudio_get_asrc_function(struct snd_kcontrol *kcontrol,
				struct snd_ctl_info *info)
{
	unsigned int reg_val;
	unsigned long value = 0;

	snd_print("\n");
	if (kcontrol->type != SND_CTL_ELEM_TYPE_ENUMERATED) {
		snd_err("invalid kcontrol type = %d.\n", kcontrol->type);
		return -EINVAL;
	}

	if (kcontrol->private_data_type == SND_MODULE_PLATFORM) {
		struct snd_platform *platform = kcontrol->private_data;

		reg_val = snd_platform_read(platform, SUNXI_DAUDIO_ASRC_ASRCEN);
		value = ((reg_val & (1 << DAUDIO_ASRC_ASRCEN)) ? 1 : 0);
	} else {
		snd_err("invalid kcontrol data type = %d.\n",
				kcontrol->private_data_type);
		return -EINVAL;
	}

	snd_kcontrol_to_snd_ctl_info(kcontrol, info, value);

	return 0;
}

static int sunxi_daudio_set_asrc_function(struct snd_kcontrol *kcontrol,
				unsigned long value)
{
	snd_print("\n");
	if (kcontrol->type != SND_CTL_ELEM_TYPE_ENUMERATED) {
		snd_err("invalid kcontrol type = %d.\n", kcontrol->type);
		return -EINVAL;
	}

	if (value >= kcontrol->items) {
		snd_err("invalid kcontrol items = %ld.\n", value);
		return -EINVAL;
	}

	if (kcontrol->private_data_type == SND_MODULE_PLATFORM) {
		struct snd_platform *platform = kcontrol->private_data;
		struct sunxi_daudio_info *sunxi_daudio = platform->private_data;

		snd_platform_update_bits(platform, SUNXI_DAUDIO_ASRC_ASRCEN,
				(1 << DAUDIO_ASRC_ASRCEN), (value << DAUDIO_ASRC_ASRCEN));
		sunxi_daudio->asrc_en = value;
	} else {
		snd_err("invalid kcontrol data type = %d.\n",
				kcontrol->private_data_type);
	}
	snd_info("mask:0x%x, items:%d, value:0x%x\n",
			kcontrol->mask, kcontrol->items, value);

	return 0;
}

static const char *sunxi_switch_text[] = {"Off", "On"};

/* pcm Audio Mode Select */
static struct snd_kcontrol sunxi_daudio_controls[] = {
	SND_CTL_ENUM_EXT("tx hub mode",
			 ARRAY_SIZE(sunxi_switch_text), sunxi_switch_text,
			 SND_CTL_ENUM_AUTO_MASK,
			 sunxi_get_tx_hub_mode,
			 sunxi_set_tx_hub_mode),
	SND_CTL_ENUM_EXT("rx sync mode",
			 ARRAY_SIZE(sunxi_switch_text), sunxi_switch_text,
			 SND_CTL_ENUM_AUTO_MASK,
			 sunxi_get_rx_sync_mode,
			 sunxi_set_rx_sync_mode),
	SND_CTL_ENUM("loopback debug",
		     ARRAY_SIZE(sunxi_switch_text), sunxi_switch_text,
		     SUNXI_DAUDIO_CTL, LOOP_EN),
	SND_CTL_ENUM_EXT("sunxi daudio asrc function",
			 ARRAY_SIZE(sunxi_switch_text), sunxi_switch_text,
			 SND_CTL_ENUM_AUTO_MASK,
			 sunxi_daudio_get_asrc_function,
			 sunxi_daudio_set_asrc_function),
};

static int sunxi_daudio_gpio_init(struct snd_platform *platform, bool enable)
{
	struct sunxi_daudio_info *sunxi_daudio = platform->private_data;
#ifdef CONFIG_DRIVER_SYSCONFIG
	user_gpio_set_t gpio_cfg[6] = {0};
	int count, i, ret = 0;
	char daudio_name[16];
	gpio_pin_t daudio_pin;

	sprintf(daudio_name, "daudio%d", sunxi_daudio->param.tdm_num);
	count = Hal_Cfg_GetGPIOSecKeyCount(daudio_name);
	if (!count) {
		snd_err("[daudio%d] sys_config has no GPIO\n", sunxi_daudio->param.tdm_num);
	}
	Hal_Cfg_GetGPIOSecData(daudio_name, gpio_cfg, count);

	for (i = 0; i < count; i++) {
		daudio_pin = (gpio_cfg[i].port - 1) * 32 + gpio_cfg[i].port_num;
		if (enable) {
			ret = hal_gpio_pinmux_set_function(daudio_pin, gpio_cfg[i].mul_sel);
			if (ret != 0)
				snd_err("daudio%d pinmux[%d] set failed.\n",
					sunxi_daudio->param.tdm_num, daudio_pin);
			ret = hal_gpio_set_driving_level(daudio_pin, gpio_cfg[i].drv_level);
			if (ret != 0)
				snd_err("daudio%d driv_level = %d set failed.\n",
					sunxi_daudio->param.tdm_num, daudio_pin);
		} else {
			ret = hal_gpio_pinmux_set_function(daudio_pin, GPIO_MUXSEL_DISABLED);
			if (ret != 0)
				snd_err("daudio%d pinmux[%d] set failed.\n",
					sunxi_daudio->param.tdm_num, daudio_pin);
		}
	}

	return ret;
#else
	int i, ret;

	switch (sunxi_daudio->param.tdm_num) {
	default:
	case 0:
		sunxi_daudio->pinctrl = daudio0_pinctrl;
		sunxi_daudio->pinctrl_num = ARRAY_SIZE(daudio0_pinctrl);
		break;
#if DAUDIO_NUM_MAX > 1
	case 1:
		sunxi_daudio->pinctrl = daudio1_pinctrl;
		sunxi_daudio->pinctrl_num = ARRAY_SIZE(daudio1_pinctrl);
		break;
#endif
#if DAUDIO_NUM_MAX > 2
	case 2:
		sunxi_daudio->pinctrl = daudio2_pinctrl;
		sunxi_daudio->pinctrl_num = ARRAY_SIZE(daudio2_pinctrl);
		break;
#endif
#if DAUDIO_NUM_MAX > 3
	case 3:
		sunxi_daudio->pinctrl = daudio3_pinctrl;
		sunxi_daudio->pinctrl_num = ARRAY_SIZE(daudio3_pinctrl);
		break;
#endif
	}
	snd_print("daudio%d pinctrl_num = %d.\n", sunxi_daudio->param.tdm_num, sunxi_daudio->pinctrl_num);

	if (enable) {
		for (i = 0; i < sunxi_daudio->pinctrl_num; i++) {
			ret = hal_gpio_pinmux_set_function(sunxi_daudio->pinctrl[i].gpio_pin,
						sunxi_daudio->pinctrl[i].mux);
			if (ret != 0) {
				snd_err("daudio%d pinmux[%d] set failed.\n",
						sunxi_daudio->param.tdm_num,
						sunxi_daudio->pinctrl[i].gpio_pin);
			}
			ret = hal_gpio_set_driving_level(sunxi_daudio->pinctrl[i].gpio_pin,
						sunxi_daudio->pinctrl[i].driv_level);
			if (ret != 0) {
				snd_err("daudio%d driv_level = %d set failed.\n",
						sunxi_daudio->param.tdm_num,
						sunxi_daudio->pinctrl[i].driv_level);
			}
		}
	} else {
		for (i = 0; i < sunxi_daudio->pinctrl_num; i++) {
			ret = hal_gpio_pinmux_set_function(sunxi_daudio->pinctrl[i].gpio_pin,
						GPIO_MUXSEL_DISABLED);
			if (ret != 0) {
				snd_err("daudio%d pinmux[%d] set failed.\n",
						sunxi_daudio->param.tdm_num,
						sunxi_daudio->pinctrl[i].gpio_pin);
			}
		}
	}

	return 0;
#endif
}

static int snd_sunxi_pa_pin_init(struct snd_platform *platform)
{
	struct sunxi_daudio_info *sunxi_daudio = platform->private_data;

	sunxi_daudio->pa_cfg = daudio_pa_cfg;
	sunxi_daudio->pa_cfg_num = ARRAY_SIZE(daudio_pa_cfg);

	snd_sunxi_pa_pin_disable(platform);

	return 0;
}
#if 0
static void snd_sunxi_pa_pin_exit(struct snd_platform *platform)
{
	UNUSED(platform);
}
#endif

static int snd_sunxi_pa_pin_enable(struct snd_platform *platform)
{
	struct sunxi_daudio_info *sunxi_daudio = platform->private_data;
	struct pa_config *pa_cfg = sunxi_daudio->pa_cfg;
	int i;

	if (!pa_cfg)
		return 0;

	for (i = 0; i < sunxi_daudio->pa_cfg_num; i++) {
		if (!pa_cfg[i].used)
			continue;
		hal_gpio_set_direction(pa_cfg[i].pin, GPIO_MUXSEL_OUT);
		hal_gpio_set_data(pa_cfg[i].pin, pa_cfg[i].level);
		hal_msleep(pa_cfg[i].msleep);
	}

	return 0;
}

static void snd_sunxi_pa_pin_disable(struct snd_platform *platform)
{
	struct sunxi_daudio_info *sunxi_daudio = platform->private_data;
	struct pa_config *pa_cfg = sunxi_daudio->pa_cfg;
	int i;

	if (!pa_cfg)
		return;

	for (i = 0; i < sunxi_daudio->pa_cfg_num; i++) {
		if (!pa_cfg[i].used)
			continue;
		hal_gpio_set_direction(pa_cfg->pin, GPIO_MUXSEL_OUT);
		hal_gpio_set_data(pa_cfg->pin, !pa_cfg->level);
	}
}

/* suspend and resume */
#ifdef CONFIG_COMPONENTS_PM
static unsigned int snd_read_func(void *data, unsigned int reg)
{
	struct snd_platform *platform;

	if (!data) {
		snd_err("data is invailed\n");
		return 0;
	}

	platform = data;
	return snd_platform_read(platform, reg);
}

static void snd_write_func(void *data, unsigned int reg, unsigned int val)
{
	struct snd_platform *platform;

	if (!data) {
		snd_err("data is invailed\n");
		return;
	}

	platform = data;
	snd_platform_write(platform, reg, val);
}

static int sunxi_daudio_suspend(struct pm_device *dev, suspend_mode_t mode)
{
	struct snd_platform *platform = dev->data;
	struct sunxi_daudio_info *sunxi_daudio = platform->private_data;

	snd_print("\n");

	snd_sunxi_save_reg(sunxi_reg_labels, (void *)platform, snd_read_func);
	snd_sunxi_daudio_clk_disable(&sunxi_daudio->clk);

	return 0;
}

static int sunxi_daudio_resume(struct pm_device *dev, suspend_mode_t mode)
{
	struct snd_platform *platform = dev->data;
	struct sunxi_daudio_info *sunxi_daudio = platform->private_data;

	snd_print("\n");

	snd_sunxi_daudio_clk_enable(&sunxi_daudio->clk, sunxi_daudio->param.tdm_num);
	sunxi_daudio_init(platform);
	snd_sunxi_echo_reg(sunxi_reg_labels, (void *)platform, snd_write_func);

	return 0;
}

struct pm_devops pm_daudio_ops = {
	.suspend = sunxi_daudio_suspend,
	.resume = sunxi_daudio_resume,
};

struct pm_device pm_daudio = {
	.name = "snddaudio",
	.ops = &pm_daudio_ops,
};
#endif

static int sunxi_daudio_param_set(struct sunxi_daudio_param *param, int tdm_num)
{
	char daudio_name[16];
	int ret;
#ifdef CONFIG_DRIVER_SYSCONFIG
	int32_t tmp_val;
#endif

	snd_print("\n");

	if (tdm_num > DAUDIO_NUM_MAX) {
		snd_err("tdm_num:%u overflow.\n", tdm_num);
		ret = -EFAULT;
	}

#ifdef CONFIG_DRIVER_SYSCONFIG
	snprintf(daudio_name, 8,"daudio%d", tdm_num);
	ret = Hal_Cfg_GetKeyValue(daudio_name, "tdm_num", (int32_t *)&tmp_val, 1);
	if (ret) {
		snd_print("daudio:tdm_num miss.\n");
		param->tdm_num = daudio_param[tdm_num].tdm_num;
	} else {
		param->tdm_num = tmp_val;
	}
	if (param->tdm_num > DAUDIO_NUM_MAX) {
		snd_err("tdm_num:%u overflow.\n", param->tdm_num);
		ret = -EFAULT;
	}

	ret = Hal_Cfg_GetKeyValue(daudio_name, "pcm_lrck_period", (int32_t *)&tmp_val, 1);
	if (ret) {
		snd_print("daudio:pcm_lrck_period miss.\n");
		param->pcm_lrck_period = daudio_param[tdm_num].pcm_lrck_period;
	} else {
		param->pcm_lrck_period = tmp_val;
	}

	ret = Hal_Cfg_GetKeyValue(daudio_name, "slot_width_select", (int32_t *)&tmp_val, 1);
	if (ret) {
		snd_print("daudio:slot_width_select miss.\n");
		param->slot_width_select = daudio_param[tdm_num].slot_width_select;
	} else {
		param->slot_width_select = tmp_val;
	}

	ret = Hal_Cfg_GetKeyValue(daudio_name, "msb_lsb_first", (int32_t *)&tmp_val, 1);
	if (ret) {
		snd_print("daudio:msb_lsb_first miss.\n");
		param->msb_lsb_first = daudio_param[tdm_num].msb_lsb_first;
	} else {
		param->msb_lsb_first = tmp_val;
	}

	ret = Hal_Cfg_GetKeyValue(daudio_name, "frametype", (int32_t *)&tmp_val, 1);
	if (ret) {
		snd_print("daudio:frametype miss.\n");
		param->frametype = daudio_param[tdm_num].frametype;
	} else {
		param->frametype = tmp_val;
	}

	ret = Hal_Cfg_GetKeyValue(daudio_name, "tx_data_mode", (int32_t *)&tmp_val, 1);
	if (ret) {
		snd_print("daudio:tx_data_mode miss.\n");
		param->tx_data_mode = daudio_param[tdm_num].tx_data_mode;
	} else {
		param->tx_data_mode = tmp_val;
	}

	ret = Hal_Cfg_GetKeyValue(daudio_name, "rx_data_mode", (int32_t *)&tmp_val, 1);
	if (ret) {
		snd_print("daudio:rx_data_mode miss.\n");
		param->rx_data_mode = daudio_param[tdm_num].rx_data_mode;
	} else {
		param->rx_data_mode = tmp_val;
	}

	ret = Hal_Cfg_GetKeyValue(daudio_name, "tdm_config", (int32_t *)&tmp_val, 1);
	if (ret) {
		snd_print("daudio:tdm_config miss.\n");
		param->tdm_config = daudio_param[tdm_num].tdm_config;
	} else {
		param->tdm_config = tmp_val;
	}

	ret = Hal_Cfg_GetKeyValue(daudio_name, "mclk_div", (int32_t *)&tmp_val, 1);
	if (ret) {
		snd_print("daudio:mclk_div miss.\n");
		param->mclk_div = daudio_param[tdm_num].mclk_div;
	} else {
		param->mclk_div = tmp_val;
	}

	ret = Hal_Cfg_GetKeyValue(daudio_name, "rx_sync_en", (int32_t *)&tmp_val, 1);
	if (ret) {
		snd_print("daudio:rx_sync_en miss.\n");
		param->rx_sync_en = daudio_param[tdm_num].rx_sync_en;
	} else {
		param->rx_sync_en = tmp_val;
	}

	ret = Hal_Cfg_GetKeyValue(daudio_name, "rx_sync_ctl", (int32_t *)&tmp_val, 1);
	if (ret) {
		snd_print("daudio:rx_sync_ctl miss.\n");
		param->rx_sync_ctl = daudio_param[tdm_num].rx_sync_ctl;
	} else {
		param->rx_sync_ctl = tmp_val;
	}

	ret = Hal_Cfg_GetKeyValue(daudio_name, "daudio_master", (int32_t *)&tmp_val, 1);
	if (ret) {
		snd_print("daudio:daudio_master miss.\n");
		param->daudio_master = daudio_param[tdm_num].daudio_master;
	} else {
		param->daudio_master = tmp_val;
	}

	ret = Hal_Cfg_GetKeyValue(daudio_name, "audio_format", (int32_t *)&tmp_val, 1);
	if (ret) {
		snd_print("daudio:audio_format miss.\n");
		param->audio_format = daudio_param[tdm_num].audio_format;
	} else {
		param->audio_format = tmp_val;
	}

	ret = Hal_Cfg_GetKeyValue(daudio_name, "signal_inversion", (int32_t *)&tmp_val, 1);
	if (ret) {
		snd_print("daudio:signal_inversion miss.\n");
		param->signal_inversion = daudio_param[tdm_num].signal_inversion;
	} else {
		param->signal_inversion = tmp_val;
	}
#else
	*param = daudio_param[tdm_num];
#endif

	return 0;
}

/* daudio probe */
static int sunxi_daudio_platform_probe(struct snd_platform *platform)
{
	int tdm_num = platform->type - SND_PLATFORM_TYPE_DAUDIO0;
	struct sunxi_daudio_info *sunxi_daudio = NULL;
	int ret = 0;

	snd_print("\n");
	sunxi_daudio = snd_malloc(sizeof(struct sunxi_daudio_info));
	if (!sunxi_daudio) {
		snd_err("no memory\n");
		return -ENOMEM;
	}

	platform->private_data = (void *)sunxi_daudio;
	platform->cpu_dai->component = platform;
	sunxi_daudio->platform = platform;

	ret = sunxi_daudio_param_set(&sunxi_daudio->param, tdm_num);
	if (ret) {
		ret = -EFAULT;
		goto err_daudio_get_param;
	}

	if (sunxi_daudio->param.rx_sync_en) {
		sunxi_daudio->param.rx_sync_domain = RX_SYNC_SYS_DOMAIN;
		sunxi_daudio->param.rx_sync_id =
			sunxi_rx_sync_probe(sunxi_daudio->param.rx_sync_domain);
		if (sunxi_daudio->param.rx_sync_id < 0) {
			snd_err("sunxi_rx_sync_probe failed.\n");
			return -EINVAL;
		}
		snd_info("sunxi_rx_sync_probe successful. domain=%d, id=%d\n",
			 sunxi_daudio->param.rx_sync_domain, sunxi_daudio->param.rx_sync_id);
	}

	/* mem base */
	platform->mem_base = (void *)SUNXI_DAUDIO_BASE + (0x1000 * sunxi_daudio->param.tdm_num);

	/* clk */
	ret = snd_sunxi_daudio_clk_init(&sunxi_daudio->clk, sunxi_daudio->param.tdm_num);
	if (ret != 0) {
		snd_err("snd_sunxi_daudio_clk_init failed\n");
		goto err_daudio_set_clock;
	}

	/* pinctrl */
	sunxi_daudio_gpio_init(platform, true);

	/* init some params */
	sunxi_daudio->hub_mode = 0;
	sunxi_daudio->playback_en = 0;
	sunxi_daudio->capture_en = 0;

	/* dma config */
	sunxi_daudio->playback_dma_param.src_maxburst = 4;
	sunxi_daudio->playback_dma_param.dst_maxburst = 4;
	sunxi_daudio->playback_dma_param.dma_addr =
			platform->mem_base + SUNXI_DAUDIO_TXFIFO;
	sunxi_daudio->capture_dma_param.src_maxburst = 4;
	sunxi_daudio->capture_dma_param.dst_maxburst = 4;
	sunxi_daudio->capture_dma_param.dma_addr =
			platform->mem_base + SUNXI_DAUDIO_RXFIFO;
	switch (sunxi_daudio->param.tdm_num) {
	case 0:
		SUNXI_DAUDIO_DRQDST(sunxi_daudio, 0);
		SUNXI_DAUDIO_DRQSRC(sunxi_daudio, 0);
		break;
#if DAUDIO_NUM_MAX > 1
	case 1:
		SUNXI_DAUDIO_DRQDST(sunxi_daudio, 1);
		SUNXI_DAUDIO_DRQSRC(sunxi_daudio, 1);
		break;
#endif
#if DAUDIO_NUM_MAX > 2
	case 2:
		SUNXI_DAUDIO_DRQDST(sunxi_daudio, 2);
		SUNXI_DAUDIO_DRQSRC(sunxi_daudio, 2);
		break;
#endif
#if DAUDIO_NUM_MAX > 3
	case 3:
		SUNXI_DAUDIO_DRQDST(sunxi_daudio, 3);
		SUNXI_DAUDIO_DRQSRC(sunxi_daudio, 3);
		break;
#endif
	default:
		snd_err("tdm_num:%u overflow\n", sunxi_daudio->param.tdm_num);
		ret = -EFAULT;
		goto err_daudio_tdm_num_over;
	}

	snd_sunxi_pa_pin_init(platform);

#ifdef CONFIG_COMPONENTS_PM
	pm_daudio.data = (void *)platform;
	ret = pm_devops_register(&pm_daudio);
	if (ret) {
		snd_err("pm_devops_register failed\n");
	}
#endif

	return 0;

err_daudio_tdm_num_over:
err_daudio_set_clock:
	snd_sunxi_daudio_clk_exit(&sunxi_daudio->clk);
err_daudio_get_param:
	snd_free(sunxi_daudio);
	return ret;
}

static int sunxi_daudio_platform_remove(struct snd_platform *platform)
{
	struct sunxi_daudio_info *sunxi_daudio;

	snd_print("\n");
	sunxi_daudio = platform->private_data;
	if (!sunxi_daudio)
		return 0;

	if (sunxi_daudio->param.rx_sync_en)
		sunxi_rx_sync_remove(sunxi_daudio->param.rx_sync_domain);

	snd_sunxi_daudio_clk_exit(&sunxi_daudio->clk);
	sunxi_daudio_gpio_init(platform, false);

	snd_free(sunxi_daudio);
	platform->private_data = NULL;

	return 0;
}

int snd_platform_daudio_register(struct snd_platform *platform, int num)
{
	int ret = 0;

	snd_print("\n");
	if (num >= DAUDIO_NUM_MAX) {
		snd_err("num=%d, but the number of daudio is %d\n",
			num, DAUDIO_NUM_MAX);
		return -1;
	}
	platform->name = snd_malloc(DAUDIO_NAME_LEN);
	if (!platform->name) {
		snd_err("no memory\n");
		return -ENOMEM;
	}
	snprintf(platform->name, DAUDIO_NAME_LEN, "daudio%d-cpudai", num);
	snd_print("platform->name:%s\n", platform->name);

	platform->ops = &sunxi_pcm_ops;
	platform->pcm_new = sunxi_pcm_new;
	platform->pcm_free = sunxi_pcm_free;
	platform->cpu_dai = snd_malloc(sizeof(struct snd_dai));
	if (!platform->cpu_dai) {
		snd_err("cpu_dai malloc failed.\n");
		ret = -ENOMEM;
		goto err_cpu_dai_malloc;
	}
	memcpy(platform->cpu_dai, &sunxi_daudio_dai, sizeof(struct snd_dai));

	platform->probe = sunxi_daudio_platform_probe;
	platform->remove = sunxi_daudio_platform_remove;

	platform->num_controls = ARRAY_SIZE(sunxi_daudio_controls);
	platform->controls = snd_malloc(sizeof(struct snd_kcontrol) * platform->num_controls);
	if (!platform->controls) {
		snd_err("controls malloc failed.\n");
		ret = -ENOMEM;
		goto err_control_malloc;
	}
	memcpy(platform->controls, sunxi_daudio_controls,
			sizeof(struct snd_kcontrol) * platform->num_controls);

	return 0;

err_control_malloc:
	snd_free(platform->cpu_dai);
err_cpu_dai_malloc:
	snd_free(platform->name);
	return ret;
}

/* #define SUNXI_DAUDIO_DEBUG_REG */

#ifdef SUNXI_DAUDIO_DEBUG_REG
/* for debug */
int cmd_daudio_dump(int argc, char *argv[])
{
	int daudio_num = 0;
	void *membase;
	int i = 0;

	if (argc == 2) {
		daudio_num = atoi(argv[1]);
	}
	membase = (void *)SUNXI_DAUDIO_BASE + (0x1000 * daudio_num);

	while (sunxi_reg_labels[i].name != NULL) {
		printf("%-20s[0x%03x]: 0x%-10x\n",
			sunxi_reg_labels[i].name,
			sunxi_reg_labels[i].address,
			snd_readl(membase + sunxi_reg_labels[i].address));
		i++;
	}
}
FINSH_FUNCTION_EXPORT_CMD(cmd_daudio_dump, daudio, daudio dump reg);
#endif /* SUNXI_DAUDIO_DEBUG_REG */

