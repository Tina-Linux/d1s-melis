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
#include <stdlib.h>
#include <string.h>
#include <hal_gpio.h>
#include <hal_dma.h>
#include <hal_clk.h>
#include <hal_reset.h>
#include <hal_time.h>
#ifdef CONFIG_COMPONENTS_PM
#include <pm_devops.h>
#endif
#include <sound/snd_core.h>
#include <sound/snd_pcm.h>
#include <sound/snd_dma.h>
#include <sound/dma_wrap.h>
#include <sound/pcm_common.h>
#include <sound/common/snd_sunxi_common.h>

#include "sunxi-codec.h"
#include "sun20iw2-codec.h"

struct snd_codec sunxi_audiocodec_adc;

#ifdef CONFIG_COMPONENTS_PM
static struct audio_reg_label sunxi_reg_labels[] = {
	/* power reg */
	REG_LABEL(AC_POWER_CTRL),

	/* adc ana reg */
	REG_LABEL(AC_MBIAS_CTRL),
	REG_LABEL(AC_ADC_ANA_CTRL1),
	REG_LABEL(AC_ADC_ANA_CTRL2),
	REG_LABEL(AC_ADC_ANA_CTRL3),

	/* adc dig reg */
	REG_LABEL(AC_ADC_DIG_CTRL),
	REG_LABEL(AC_ADC_HPF_CTRL1),
	REG_LABEL(AC_ADC_HPF_CTRL2),
	REG_LABEL(AC_ADC_DIG_VOL),
	REG_LABEL(AC_ADC_RXFIFO_CTRL),
	REG_LABEL(AC_ADC_RXFIFO_STA),
	/* REG_LABEL(SUNXI_ADC_RXDATA), */
	REG_LABEL(AC_ADC_RXCNT),
	REG_LABEL(AC_ADC_DEBUG),
	REG_LABEL_END,
};
#endif

static struct sunxi_codec_param default_param = {
	.mic1gain	= 0x1f,
	.mic2gain	= 0x1f,
	.mic3gain	= 0x1f,
	.mic1_en	= true,
	.mic2_en	= true,
	.mic3_en	= true,
	.mad_bind_en	= false,
};

/* dac_clk_div = audio_pll_clk / (fs*128)
 * 0 -> div1
 * 1 -> div2
 * 2 -> div3
 * 3 -> div4
 * 4 -> div6
 * 5 -> div8
 * 6 -> div12
 * 7 -> div16
 * 8 -> div24
 */
/* audio_pll_clk -> 8.192M */
static const struct sample_rate sample_rate_conv_mad[] = {
	{32000, 1},	/* div2 */
	{16000, 3},	/* div4 */
	{8000, 5},	/* div8 */
};

/* audio_pll_clk -> 24.576M or 22.5792M */
static const struct sample_rate sample_rate_conv[] = {
	{384000, 0},	/* audio_pll_clk -> 49.152M */
	{352800, 0},	/* audio_pll_clk -> 45.1584M */
	{192000, 0},	/* audio_pll_clk -> 24.576M */
	{176400, 0},	/* audio_pll_clk -> 22.5792M */
	{96000, 1},
	{88200, 1},
	{64000, 2},
	{48000, 3},
	{44100, 3},
	{32000, 4},
	{24000, 5},
	{22050, 5},
	{16000, 6},
	{12000, 7},
	{11025, 7},
	{8000, 8},
};

static const char *const codec_switch_onoff[] = {"off", "on"};

static int snd_sunxi_clk_init(struct sunxi_codec_clk *clk);
static void snd_sunxi_clk_exit(struct sunxi_codec_clk *clk);
static int snd_sunxi_clk_enable(struct sunxi_codec_clk *clk);
static void snd_sunxi_clk_disable(struct sunxi_codec_clk *clk);
static int snd_sunxi_clk_set_parent(struct sunxi_codec_clk *clk);
static int snd_sunxi_clk_set_rate(struct sunxi_codec_clk *clk, int stream,
				  unsigned int freq_in, unsigned int freq_out);

/*
 * Configure DMA , Chan enable & Global enable
 */
#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
static void sunxi_codec_debug_chan_control(struct sunxi_codec_info *sunxi_codec, bool enable)
{
	struct sunxi_mad_priv *mad_priv = &sunxi_codec->mad_priv;

	snd_print("mad enable -> %d\n", enable);

	if(enable) {
		/*
		 * only for reset mad module by ccmu.
		 * should not be callback at dai_ops->trigger
		 * because it maybe running at interrupt!
		 */
		sunxi_mad_module_clk_enable(false);
		sunxi_mad_module_clk_enable(true);

		/* sunxi_mad_close();*/
		sunxi_mad_open();

		sunxi_mad_hw_params(mad_priv->audio_src_chan_num,
			mad_priv->sample_rate);

		sunxi_mad_audio_src_chan_num(mad_priv->audio_src_chan_num);
		sunxi_lpsd_chan_sel(mad_priv->lpsd_chan_sel);
		sunxi_mad_standby_chan_sel(mad_priv->standby_chan_sel);
		sunxi_mad_dma_type(SUNXI_MAD_DMA_IO);
		sunxi_sram_ahb1_threshole_init();
		sunxi_mad_sram_init();
		/*
		 * should set it after sram reset.
		 * path_sel: codec-2
		 */
		sunxi_mad_audio_source_sel(MAD_PATH_CODECADC, 1);
	} else {
		sunxi_mad_module_clk_enable(false);
	}
}

static int sunxi_codec_mad_enter_standby(struct sunxi_codec_info *sunxi_codec)
{
	struct snd_codec *codec = sunxi_codec->codec;
	/* struct sunxi_codec_param *param = &sunxi_codec->param; */

	sunxi_codec->capturing = 1;
	sunxi_codec_debug_chan_control(sunxi_codec, false);

	/* should be flush fifo */
	snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
			(1 << ADC_FIFO_FLUSH), (1 << ADC_FIFO_FLUSH));
	snd_codec_write(codec, AC_ADC_RXFIFO_STA,
			(1 << ADC_RXA_INT | 1 << ADC_RXO_INT));
	snd_codec_write(codec, AC_ADC_RXCNT, 0);

	snd_codec_update_bits(codec, AC_ADC_DIG_CTRL, 0x1<<ADC_GEN, 0x1<<ADC_GEN);

/*
	if (param->rx_sync_en) {
		snd_codec_update_bits(codec, SUNXI_ADC_FIFOC,
			(1 << RX_SYNC_EN_START), (1 << RX_SYNC_EN_START));
	}
*/

	snd_codec_update_bits(codec, AC_ADC_DIG_CTRL, 1 << MAD_DATA_EN, 1 << MAD_DATA_EN);

	sunxi_codec_debug_chan_control(sunxi_codec, true);
	return 0;
}

static void sunxi_codec_adc_mad_enable(struct snd_codec *codec, bool enable)
{
	struct sunxi_codec_info *sunxi_codec = codec->private_data;
	int ret = 0;
	bool mad_bind = false;

	snd_print("mad\n");

	ret = sunxi_mad_bind_get(MAD_PATH_CODECADC, &mad_bind);
	if (ret) {
		mad_bind = false;
		snd_err("get mad_bind failed, path: %d\n", MAD_PATH_CODECADC);
	}

	if (enable) {
		/* enable adc */
		if (mad_bind) {
			snd_print("mad bind\n");
			if (sunxi_codec->capturing) {
				snd_print("capturing\n");
				sunxi_mad_dma_type(SUNXI_MAD_DMA_IO);
				sunxi_mad_dma_enable(true);
			} else {
				snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
					(0x1 << MAD_DATA_EN),
					(0x1 << MAD_DATA_EN));

				sunxi_codec_debug_chan_control(sunxi_codec, true);

				snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
					0x1<<ADC_GEN, 0x1<<ADC_GEN);
				sunxi_mad_dma_type(SUNXI_MAD_DMA_IO);
				sunxi_mad_dma_enable(true);
				sunxi_codec->capturing = 1;
			}
		} else {
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
				1 << ADC_DRQ_EN, 1 << ADC_DRQ_EN);
		}
	} else {
		if (mad_bind) {
			snd_print("mad bind\n");
			snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
					0x1<<ADC_GEN, 0x0<<ADC_GEN);

			sunxi_mad_dma_enable(false);

			snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
				(0x1 << MAD_DATA_EN),
				(0x0 << MAD_DATA_EN));

			sunxi_codec->capturing = 0;
		} else {
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
				1 << ADC_DRQ_EN, 0 << ADC_DRQ_EN);
		}
	}
}

static int sunxi_codec_mad_set_hw_params(struct snd_pcm_substream *substream,
					 struct snd_pcm_hw_params *params,
					 struct snd_dai *dai)
{
	struct snd_codec *codec = dai->component;
	struct sunxi_codec_info *sunxi_codec = codec->private_data;

	int ret = 0;
	bool mad_bind = false;

	snd_print("mad\n");

	ret = sunxi_mad_bind_get(MAD_PATH_CODECADC, &mad_bind);
	if (ret) {
		mad_bind = false;
		snd_err("get mad_bind failed, path: %d\n", MAD_PATH_CODECADC);
	}

	if (!mad_bind) {
		snd_print("mad unbind\n");
		return 0;
	}

	snd_print("mad bind\n");

	/*mad only supported 16k/48KHz samplerate when capturing*/
	if (params_format(params) != SND_PCM_FORMAT_S16_LE) {
		snd_err("unsupported mad sample bits!\n");
		return -EINVAL;
	}
	switch (params_channels(params)) {
	case 1:
	case 2:
	case 3:
	case 4:
		break;
	default:
		snd_err("unsupported mad channels.\n");
		return -EINVAL;
	}
	if ((params_rate(params) == 16000) ||
		(params_rate(params) == 48000)) {
		/* mad config */
		sunxi_codec->mad_priv.audio_src_chan_num = params_channels(params);
		sunxi_codec->mad_priv.sample_rate = params_rate(params);
	} else {
		snd_err("unsupported mad rate\n");
		return -EINVAL;
	}

	return 0;
}

/*mad_bind config*/
static int sunxi_codec_set_mad_bind(struct snd_kcontrol *kcontrol, unsigned long value)
{
	int ret = 0;
	bool mad_bind = false;

	if (kcontrol->type != SND_CTL_ELEM_TYPE_ENUMERATED)
		return -EINVAL;

	if (value >= kcontrol->items)
		return -EINVAL;

	if (kcontrol->private_data_type == SND_MODULE_CODEC) {
		/* struct snd_codec *codec = kcontrol->private_data; */
		/* struct sunxi_codec_info *sunxi_codec = codec->private_data; */
		mad_bind = value;
	}
	snd_info("mask:0x%x, items:%d, value:0x%x\n", kcontrol->mask, kcontrol->items, value);

	ret = sunxi_mad_bind_set(MAD_PATH_CODECADC, mad_bind);
	if (ret) {
		snd_err("set mad_bind failed, path: %d\n", MAD_PATH_CODECADC);
	}

	return 0;
}

static int sunxi_codec_get_mad_bind(struct snd_kcontrol *kcontrol, struct snd_ctl_info *info)
{
	unsigned long value = 0;
	int ret = 0;
	bool mad_bind = false;

	ret = sunxi_mad_bind_get(MAD_PATH_CODECADC, &mad_bind);
	if (ret) {
		mad_bind = false;
		snd_err("get mad_bind failed, path: %d\n", MAD_PATH_CODECADC);
	}

	if (kcontrol->type != SND_CTL_ELEM_TYPE_ENUMERATED)
		return -EINVAL;

	if (kcontrol->private_data_type == SND_MODULE_CODEC) {
		/* struct snd_codec *codec = kcontrol->private_data; */
		/* struct sunxi_codec_info *sunxi_codec = codec->private_data; */
		value = mad_bind;
	} else
		return -EINVAL;

	snd_kcontrol_to_snd_ctl_info(kcontrol, info, value);

	return 0;
}

static const char * const mad_bind_function[] = {"unbound", "mad_bind"};

/*lpsd channel sel*/
static int sunxi_codec_set_lpsd_chan(struct snd_kcontrol *kcontrol,
				unsigned long value)
{
	if (kcontrol->type != SND_CTL_ELEM_TYPE_ENUMERATED)
		return -EINVAL;

	if (value >= kcontrol->items)
		return -EINVAL;

	if (kcontrol->private_data_type == SND_MODULE_CODEC) {
		struct snd_codec *codec = kcontrol->private_data;
		struct sunxi_codec_info *sunxi_codec = codec->private_data;
		sunxi_codec->mad_priv.lpsd_chan_sel = value & kcontrol->mask;
		sunxi_lpsd_chan_sel(sunxi_codec->mad_priv.lpsd_chan_sel);
	}
	snd_info("mask:0x%x, items:%d, value:0x%x\n",
			kcontrol->mask, kcontrol->items, value);

	return 0;
}

static int sunxi_codec_get_lpsd_chan(struct snd_kcontrol *kcontrol, struct snd_ctl_info *info)
{
	unsigned int val = 0;

	if (kcontrol->type != SND_CTL_ELEM_TYPE_ENUMERATED) {
		snd_err("invalid kcontrol type = %d.\n", kcontrol->type);
		return -EINVAL;
	}

	if (kcontrol->private_data_type == SND_MODULE_CODEC) {
		struct snd_codec *codec = kcontrol->private_data;
		struct sunxi_codec_info *sunxi_codec = codec->private_data;
		val = sunxi_codec->mad_priv.lpsd_chan_sel;
	} else {
		snd_err("invalid kcontrol data type = %d.\n",
				kcontrol->private_data_type);
		return -EINVAL;
	}

	snd_kcontrol_to_snd_ctl_info(kcontrol, info, val);

	return 0;
}

static const char * const lpsd_chan_sel_function[] = {"0th_chan", "1st_chan",
		"2nd_chan", "3rd_chan", "4th_chan", "5th_chan",
		"6th_chan", "7th_chan"};

/*mad_standby channel sel*/
static int sunxi_codec_set_mad_standby_chan(struct snd_kcontrol *kcontrol, unsigned long value)
{
	if (kcontrol->type != SND_CTL_ELEM_TYPE_ENUMERATED)
		return -EINVAL;

	if (value >= kcontrol->items)
		return -EINVAL;

	if (kcontrol->private_data_type == SND_MODULE_CODEC) {
		struct snd_codec *codec = kcontrol->private_data;
		struct sunxi_codec_info *sunxi_codec = codec->private_data;
		sunxi_codec->mad_priv.standby_chan_sel = value;
		sunxi_mad_standby_chan_sel(sunxi_codec->mad_priv.standby_chan_sel);
	}

	snd_info("mask:0x%x, items:%d, value:0x%x\n",
			kcontrol->mask, kcontrol->items, value);

	return 0;
}

static int sunxi_codec_get_mad_standby_chan(struct snd_kcontrol *kcontrol,
					    struct snd_ctl_info *info)
{
	unsigned int val = 0;

	if (kcontrol->type != SND_CTL_ELEM_TYPE_ENUMERATED)
		return -EINVAL;

	if (kcontrol->private_data_type == SND_MODULE_CODEC) {
		struct snd_codec *codec = kcontrol->private_data;
		struct sunxi_codec_info *sunxi_codec = codec->private_data;
		val = sunxi_codec->mad_priv.standby_chan_sel;
	} else
		return -EINVAL;

	snd_kcontrol_to_snd_ctl_info(kcontrol, info, val);

	return 0;
}

static const char * const mad_standby_chan_sel_function[] = {
	"Zero_Chan", "Two_Chan", "Three_Chan", "Four_Chan",
};

static const char * const codec_mad_standby_control[] = {
	"RESUME", "SUSPEND",
};

static int sunxi_codec_set_mad_standby_control(struct snd_kcontrol *kcontrol, unsigned long value)
{
	int ret = 0;
	bool mad_bind = false;

	snd_print("mad\n");

	ret = sunxi_mad_bind_get(MAD_PATH_CODECADC, &mad_bind);
	if (ret) {
		mad_bind = false;
		snd_err("get mad_bind failed, path: %d\n", MAD_PATH_CODECADC);
	}

	if (kcontrol->type != SND_CTL_ELEM_TYPE_ENUMERATED)
		return -EINVAL;

	if (value >= kcontrol->items)
		return -EINVAL;

	if (kcontrol->private_data_type == SND_MODULE_CODEC) {
		struct snd_codec *codec = kcontrol->private_data;
		struct sunxi_codec_info *sunxi_codec = codec->private_data;
		if (mad_bind) {
			switch (value) {
			case 0:
			default:
				sunxi_mad_resume_external();
				sunxi_codec->mad_priv.mad_suspend = 0;
				break;
			case 1:
				/* maybe using it at app to waiting lspd status is 0 */
				#if 0
				while (sunxi_mad_get_lpsd_status())
					msleep(100);
				#endif
				sunxi_codec_mad_enter_standby(sunxi_codec);
				sunxi_mad_suspend_external();
				sunxi_codec->mad_priv.mad_suspend = 1;
				break;
			}
		}
	}
	snd_info("mask:0x%x, items:%d, value:0x%x\n", kcontrol->mask, kcontrol->items, value);

	return 0;
}

static int sunxi_codec_get_mad_standby_control(struct snd_kcontrol *kcontrol,
					       struct snd_ctl_info *info)
{
	unsigned int val = 0;

	if (kcontrol->type != SND_CTL_ELEM_TYPE_ENUMERATED)
		return -EINVAL;

	if (kcontrol->private_data_type == SND_MODULE_CODEC) {
		struct snd_codec *codec = kcontrol->private_data;
		struct sunxi_codec_info *sunxi_codec = codec->private_data;
		val = sunxi_codec->mad_priv.mad_suspend;
	} else
		return -EINVAL;

	snd_kcontrol_to_snd_ctl_info(kcontrol, info, val);

	return 0;
}
#endif

static int sunxi_set_mic_ch(struct snd_kcontrol *kcontrol, unsigned long value)
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

	if (kcontrol->private_data_type == SND_MODULE_CODEC) {
		struct snd_codec *codec = kcontrol->private_data;
		struct sunxi_codec_info *sunxi_codec = codec->private_data;
		struct sunxi_codec_param *codec_param = &sunxi_codec->param;

		if (kcontrol->mask == SND_CTL_ENUM_MIC1_MASK)
			codec_param->mic1_en = value;
		else if (kcontrol->mask == SND_CTL_ENUM_MIC2_MASK)
			codec_param->mic2_en = value;
		else if (kcontrol->mask == SND_CTL_ENUM_MIC3_MASK)
			codec_param->mic3_en = value;
	} else {
		snd_err("invalid kcontrol data type = %d.\n", kcontrol->private_data_type);
	}
	snd_info("mask:0x%x, items:%d, value:0x%x\n", kcontrol->mask, kcontrol->items, value);

	return 0;
}

static int sunxi_get_mic_ch(struct snd_kcontrol *kcontrol, struct snd_ctl_info *info)
{
	unsigned int val = 0;

	snd_print("\n");

	if (kcontrol->type != SND_CTL_ELEM_TYPE_ENUMERATED) {
		snd_err("invalid kcontrol type = %d.\n", kcontrol->type);
		return -EINVAL;
	}

	if (kcontrol->private_data_type == SND_MODULE_CODEC) {
		struct snd_codec *codec = kcontrol->private_data;
		struct sunxi_codec_info *sunxi_codec = codec->private_data;
		struct sunxi_codec_param *codec_param = &sunxi_codec->param;

		if (kcontrol->mask == SND_CTL_ENUM_MIC1_MASK)
			val = codec_param->mic1_en << 0;
		else if (kcontrol->mask == SND_CTL_ENUM_MIC2_MASK)
			val = codec_param->mic2_en << 1;
		else if (kcontrol->mask == SND_CTL_ENUM_MIC3_MASK)
			val = codec_param->mic3_en << 2;
	} else {
		snd_err("%s invalid kcontrol data type = %d.\n", __func__,
			kcontrol->private_data_type);
	}

	snd_kcontrol_to_snd_ctl_info(kcontrol, info, val);

	return 0;
}

static struct snd_kcontrol sunxi_codec_controls[] = {
#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
	SND_CTL_ENUM_EXT("bind mad function",
		ARRAY_SIZE(mad_bind_function),
		mad_bind_function,
		SND_CTL_ENUM_AUTO_MASK,
		sunxi_codec_get_mad_bind, sunxi_codec_set_mad_bind),
	SND_CTL_ENUM_EXT("lpsd channel sel function",
		ARRAY_SIZE(lpsd_chan_sel_function),
		lpsd_chan_sel_function,
		SND_CTL_ENUM_AUTO_MASK,
		sunxi_codec_get_lpsd_chan, sunxi_codec_set_lpsd_chan),
	SND_CTL_ENUM_EXT("mad standby channel sel function",
		ARRAY_SIZE(mad_standby_chan_sel_function),
		mad_standby_chan_sel_function,
		SND_CTL_ENUM_AUTO_MASK,
		sunxi_codec_get_mad_standby_chan,
		sunxi_codec_set_mad_standby_chan),
	SND_CTL_ENUM_EXT("mad standby control",
		ARRAY_SIZE(codec_mad_standby_control),
		codec_mad_standby_control,
		SND_CTL_ENUM_AUTO_MASK,
		sunxi_codec_get_mad_standby_control,
		sunxi_codec_set_mad_standby_control),
#endif
	SND_CTL_KCONTROL("MIC1 volume", AC_ADC_ANA_CTRL1, ADC1_PGA_GAIN_CTRL, 0x1F),
	SND_CTL_KCONTROL("MIC2 volume", AC_ADC_ANA_CTRL2, ADC2_PGA_GAIN_CTRL, 0x1F),
	SND_CTL_KCONTROL("MIC3 volume", AC_ADC_ANA_CTRL3, ADC3_PGA_GAIN_CTRL, 0x1F),
	/* mic in switch */
	SND_CTL_ENUM_EXT("MIC1 switch",
		     ARRAY_SIZE(codec_switch_onoff), codec_switch_onoff,
		     SND_CTL_ENUM_MIC1_MASK,
		     sunxi_get_mic_ch, sunxi_set_mic_ch),
	SND_CTL_ENUM_EXT("MIC2 switch",
		     ARRAY_SIZE(codec_switch_onoff), codec_switch_onoff,
		     SND_CTL_ENUM_MIC2_MASK,
		     sunxi_get_mic_ch, sunxi_set_mic_ch),
	SND_CTL_ENUM_EXT("MIC3 switch",
		     ARRAY_SIZE(codec_switch_onoff), codec_switch_onoff,
		     SND_CTL_ENUM_MIC3_MASK,
		     sunxi_get_mic_ch, sunxi_set_mic_ch),
};

static int sunxi_get_adc_ch(struct snd_codec *codec)
{
	int ret = -1;
	struct sunxi_codec_info *sunxi_codec = codec->private_data;
	struct sunxi_codec_param *codec_param = &sunxi_codec->param;

	snd_print("\n");

	if (codec_param->mic1_en) {
		codec_param->adc1_en = true;
		ret = 1;
	}
	else {
		codec_param->adc1_en = false;
	}

	if (codec_param->mic2_en) {
		codec_param->adc2_en = true;
		ret = 1;
	}
	else {
		codec_param->adc2_en = false;
	}

	if (codec_param->mic3_en) {
		codec_param->adc3_en = true;
		ret = 1;
	}
	else {
		codec_param->adc3_en = false;
	}

	return ret;
}

static void sunxi_codec_init(struct snd_codec *codec)
{
	struct sunxi_codec_info *sunxi_codec = codec->private_data;
	struct sunxi_codec_param *param = &sunxi_codec->param;

	snd_print("\n");

	/* *** ANA PATH *** */
	snd_codec_write(codec, AC_POWER_CTRL, 0x801c382a);	/* ldo calibration */
	/* thd+n best performance */
	snd_codec_update_bits(codec, AC_ADC_ANA_CTRL1,
			      0xff<<ADC1_PGA_CHOPPER_NOL_DELAY_SET,
			      0xfc<<ADC1_PGA_CHOPPER_NOL_DELAY_SET);
	snd_codec_update_bits(codec, AC_ADC_ANA_CTRL2,
			      0xff<<ADC2_PGA_CHOPPER_NOL_DELAY_SET,
			      0xfc<<ADC2_PGA_CHOPPER_NOL_DELAY_SET);
	snd_codec_update_bits(codec, AC_ADC_ANA_CTRL3,
			      0xff<<ADC3_PGA_CHOPPER_NOL_DELAY_SET,
			      0xfc<<ADC3_PGA_CHOPPER_NOL_DELAY_SET);

	/* *** ADC PATH *** */
	/* ADC data from RX mixer */
	snd_codec_update_bits(codec, AC_ADC_DIG_CTRL, 0x3 << ADC_PTN_SEL, 0x0 << ADC_PTN_SEL);

	/* *** debug *** */
	/* ADC data fixed as 0x5a5a5a */
	/*
	snd_codec_update_bits(codec, AC_ADC_DIG_CTRL, 0x3<<ADC_PTN_SEL, 0x1<<ADC_PTN_SEL);
	*/

	/* mic gain */
	snd_codec_update_bits(codec, AC_ADC_ANA_CTRL1,
			      0x1F<<ADC1_PGA_GAIN_CTRL,
			      (param->mic1gain)<<ADC1_PGA_GAIN_CTRL);
	snd_codec_update_bits(codec, AC_ADC_ANA_CTRL2,
			      0x1F<<ADC2_PGA_GAIN_CTRL,
			      (param->mic2gain)<<ADC2_PGA_GAIN_CTRL);
	snd_codec_update_bits(codec, AC_ADC_ANA_CTRL3,
			      0x1F<<ADC3_PGA_GAIN_CTRL,
			      (param->mic3gain)<<ADC3_PGA_GAIN_CTRL);
}

static int sunxi_codec_dapm_control(struct snd_pcm_substream *substream, struct snd_dai *dai, int onoff)
{
	struct snd_codec *codec = dai->component;
	struct sunxi_codec_info *sunxi_codec = codec->private_data;
	struct sunxi_codec_param *codec_param = &sunxi_codec->param;

	snd_print("\n");

#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
	int ret = 0;
	bool mad_bind = false;

	snd_print("mad\n");

	ret = sunxi_mad_bind_get(MAD_PATH_CODECADC, &mad_bind);
	if (ret) {
		mad_bind = false;
		snd_err("get mad_bind failed, path: %d\n", MAD_PATH_CODECADC);
	}
#endif

	if (substream->dapm_state == onoff)
		return 0;
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		snd_err("unspport pcm stream playback\n");
	} else {
		/*
		 * Capture:
		 * Capture <-- ADC1 <-- Input Mixer <-- MIC1 PGA <-- MIC1
		 * Capture <-- ADC2 <-- Input Mixer <-- MIC2 PGA <-- MIC2
		 * Capture <-- ADC3 <-- Input Mixer <-- MIC3 PGA <-- MIC3
		 *
		 */
		unsigned int channels = 0;
		channels = substream->runtime->channels;

		if (onoff) {
			if (channels > 3 || channels < 1) {
				snd_err("unknown channels:%u\n", channels);
				return -EINVAL;
			}
			/* power on */
			snd_codec_update_bits(codec, AC_MBIAS_CTRL, 1 << MBIAS_EN, 1 << MBIAS_EN);
			/* sleep 100ms to remove pop */
			hal_msleep(100);
			/* ana ADC enable */
			if (codec_param->adc1_en)
				snd_codec_update_bits(codec, AC_ADC_ANA_CTRL1,
						1 << ADC1_EN, 1 << ADC1_EN);
			if (codec_param->adc2_en)
				snd_codec_update_bits(codec, AC_ADC_ANA_CTRL2,
						1 << ADC2_EN, 1 << ADC2_EN);
			if (codec_param->adc3_en)
				snd_codec_update_bits(codec, AC_ADC_ANA_CTRL3,
						1 << ADC3_EN, 1 << ADC3_EN);
			/* digital ADC global enable */
#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
			snd_print("mad\n");
			if (mad_bind == 0) {
				snd_print("mad unbind\n");
				snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
						      1<<ADC_GEN, 1<<ADC_GEN);
			}
#else
			snd_codec_update_bits(codec, AC_ADC_DIG_CTRL, 0x1<<ADC_GEN, 0x1<<ADC_GEN);
#endif
		} else {
			/* digital ADC global disable */
			snd_codec_update_bits(codec, AC_ADC_DIG_CTRL, 0x1<<ADC_GEN, 0x0<<ADC_GEN);
			/* ana ADC disable */
			snd_codec_update_bits(codec, AC_ADC_ANA_CTRL1, 1 << ADC1_EN, 0 << ADC1_EN);
			snd_codec_update_bits(codec, AC_ADC_ANA_CTRL2, 1 << ADC2_EN, 0 << ADC2_EN);
			snd_codec_update_bits(codec, AC_ADC_ANA_CTRL3, 1 << ADC3_EN, 0 << ADC3_EN);
			/* power off */
			snd_codec_update_bits(codec, AC_MBIAS_CTRL, 1 << MBIAS_EN, 0 << MBIAS_EN);
		}
	}
	substream->dapm_state = onoff;
	return 0;
}

static int sunxi_codec_startup(struct snd_pcm_substream *substream, struct snd_dai *dai)
{
	snd_print("\n");

	return 0;
}

static int sunxi_codec_hwrate_div_check(bool is_resume, unsigned int sample_rates,
					enum SUNXI_CODEC_CLK_WORK_MODE *clk_work_mode)
{
	int i;
	bool mach_conv = false;

	if (sample_rates > 96000) {
		snd_err("cannot support hwrate_div rates:%u.\n", sample_rates);
		return -EINVAL;
	}

	if (is_resume) {
		for (i = 0; i < ARRAY_SIZE(sample_rate_conv); i++) {
			if (sample_rate_conv[i].samplerate == sample_rates) {
				mach_conv = true;
				break;
			}
		}
		if (mach_conv) {
			*clk_work_mode = SUNXI_CODEC_CLK_WORK_RESUME;
			return i;
		} else {
			return -EINVAL;
		}
	} else {
		for (i = 0; i < ARRAY_SIZE(sample_rate_conv_mad); i++) {
			if (sample_rate_conv_mad[i].samplerate == sample_rates) {
				mach_conv = true;
				break;
			}
		}
		if (mach_conv) {
			*clk_work_mode = SUNXI_CODEC_CLK_WORK_SUSPEND_LOW;
			return i;
		}

		for (i = 0; i < ARRAY_SIZE(sample_rate_conv); i++) {
			if (sample_rate_conv[i].samplerate == sample_rates) {
				mach_conv = true;
				break;
			}
		}
		if (mach_conv) {
			*clk_work_mode = SUNXI_CODEC_CLK_WORK_SUSPEND_HIGH;
			return i;
		} else {
			return -EINVAL;
		}
	}

	return i;
}

static int sunxi_codec_hwrate_set(struct snd_codec *codec, bool is_resume,
				  int stream, unsigned int sample_rates)
{
	int i;
	unsigned int rate_bit;
	struct sunxi_codec_info *sunxi_codec = codec->private_data;
	struct sunxi_codec_clk *clk = &sunxi_codec->clk;

	if (sample_rates == 0) {
		snd_print("unnned set hw rate\n");
		return 0;
	}

	i = sunxi_codec_hwrate_div_check(is_resume, sample_rates, &clk->clk_work_mode);
	if (i < 0) {
		snd_err("unsupport hw rate\n");
		return -EINVAL;
	}

	switch (clk->clk_work_mode) {
	case SUNXI_CODEC_CLK_WORK_RESUME:
	case SUNXI_CODEC_CLK_WORK_SUSPEND_HIGH:
		rate_bit = sample_rate_conv[i].rate_bit;
		break;
	case SUNXI_CODEC_CLK_WORK_SUSPEND_LOW:
		rate_bit = sample_rate_conv_mad[i].rate_bit;
		break;
	default:
		snd_err("clk work mode invaild\n");
		break;
	}

	if (stream == SNDRV_PCM_STREAM_PLAYBACK) {
		snd_err("unspport pcm stream playback\n");
	} else {
		/* for digital clk */
		snd_codec_update_bits(codec, AC_ADC_DIG_CTRL, 0xf << ADC_CLK, rate_bit << ADC_CLK);
		/* for analog clk */
		switch (sample_rates) {
		case 96000:
		case 88200:
			snd_codec_update_bits(codec, AC_ADC_DIG_CTRL, 3 << ADC_OSR, 1 << ADC_OSR);
		default:
			snd_codec_update_bits(codec, AC_ADC_DIG_CTRL, 3 << ADC_OSR, 0 << ADC_OSR);
		}
	}

	return 0;
}

static int sunxi_codec_hw_params(struct snd_pcm_substream *substream,
				 struct snd_pcm_hw_params *params,
				 struct snd_dai *dai)
{
	struct snd_codec *codec = dai->component;
	struct sunxi_codec_info *sunxi_codec = codec->private_data;
	struct sunxi_codec_param *codec_param = &sunxi_codec->param;
	struct sunxi_codec_clk *clk = &sunxi_codec->clk;
	int ret;
	unsigned int sample_rates;
	unsigned int sample_channels;

	snd_print("\n");

#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
	bool mad_bind = false;

	snd_print("mad\n");

	ret = sunxi_mad_bind_get(MAD_PATH_CODECADC, &mad_bind);
	if (ret) {
		mad_bind = false;
		snd_err("get mad_bind failed, path: %d\n", MAD_PATH_CODECADC);
	}
#endif

	/* sample bit */
	switch (params_format(params)) {
	case	SND_PCM_FORMAT_S16_LE:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			snd_err("unspport pcm stream playback\n");
		} else {
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
				1 << ADC_FIFO_MODE, 1 << ADC_FIFO_MODE);
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
				1 << ADC_SAMPLE_BITS, 0 << ADC_SAMPLE_BITS);
		}
		break;
	case	SND_PCM_FORMAT_S24_LE:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			snd_err("unspport pcm stream playback\n");
		} else {
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
				1 << ADC_FIFO_MODE, 0 << ADC_FIFO_MODE);
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
				1 << ADC_SAMPLE_BITS, 1 << ADC_SAMPLE_BITS);
		}
		break;
	default:
		snd_err("cannot support bits:%u.\n", params_format(params));
		return -EINVAL;
	}

	/* sample rate */
	sample_rates = params_rate(params);
	clk->sample_rates = sample_rates;
	ret = sunxi_codec_hwrate_set(codec, true, substream->stream, sample_rates);
	if (ret) {
		snd_err("sunxi_codec_hwrate_set failed\n");
		return ret;
	}

	/* sample channel */
	sample_channels = params_channels(params);
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		snd_err("unspport pcm stream playback\n");
	} else {
		if (sunxi_get_adc_ch(codec) < 0) {
			snd_err("capture only support 1~3 channel\n");
			return -EINVAL;
		}
		if (codec_param->adc1_en) {
			snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
					1 << ADC1_DIG_EN, 1 << ADC1_DIG_EN);
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
					1 << ADC1_FIFO_EN, 1 << ADC1_FIFO_EN);
		} else {
			snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
					1 << ADC1_DIG_EN, 0 << ADC1_DIG_EN);
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
					1 << ADC1_FIFO_EN, 0 << ADC1_FIFO_EN);
		}
		if (codec_param->adc2_en) {
			snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
					1 << ADC2_DIG_EN, 1 << ADC2_DIG_EN);
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
					1 << ADC2_FIFO_EN, 1 << ADC2_FIFO_EN);
		} else {
			snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
					1 << ADC2_DIG_EN, 0 << ADC2_DIG_EN);
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
					1 << ADC2_FIFO_EN, 0 << ADC2_FIFO_EN);
		}
		if (codec_param->adc3_en) {
			snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
					1 << ADC3_DIG_EN, 1 << ADC3_DIG_EN);
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
					1 << ADC3_FIFO_EN, 1 << ADC3_FIFO_EN);
		} else {
			snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
					1 << ADC3_DIG_EN, 0 << ADC3_DIG_EN);
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
					1 << ADC3_FIFO_EN, 0 << ADC3_FIFO_EN);
		}
	}

#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
	snd_print("mad\n");
	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
		return sunxi_codec_mad_set_hw_params(substream, params, dai);
	}
#endif

	return 0;
}

static int sunxi_codec_set_sysclk(struct snd_dai *dai, int clk_id, unsigned int freq, int dir)
{
	int ret;
	struct snd_codec *codec = dai->component;
	struct sunxi_codec_info *sunxi_codec = codec->private_data;

	snd_print("\n");

	ret = snd_sunxi_clk_set_rate(&sunxi_codec->clk, dir, freq, freq);
	if (ret < 0) {
		snd_err("snd_sunxi_clk_set_rate failed\n");
		return -1;
	}

	return 0;
}

#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
static void sunxi_codec_mad_shutdown(struct snd_pcm_substream *substream, struct snd_dai *dai)
{
	struct snd_codec *codec = dai->component;
	struct sunxi_codec_info *sunxi_codec = codec->private_data;
	int ret = 0;
	bool mad_bind = false;

	snd_print("mad\n");

	ret = sunxi_mad_bind_get(MAD_PATH_CODECADC, &mad_bind);
	if (ret) {
		mad_bind = false;
		snd_err("get mad_bind failed, path: %d\n", MAD_PATH_CODECADC);
	}

	if (mad_bind) {
		snd_print("mad bind\n");
		snd_codec_update_bits(codec,
				AC_ADC_DIG_CTRL,
				(0x1 << MAD_DATA_EN),
				(0x0 << MAD_DATA_EN));
		sunxi_mad_audio_source_sel(MAD_PATH_CODECADC, 0);

		/* should close the ADC_EN */
		snd_codec_update_bits(codec, AC_ADC_DIG_CTRL, 1<<ADC_GEN, 0<<ADC_GEN);
		snd_codec_update_bits(codec, AC_ADC_DIG_CTRL, 1 << ADC1_DIG_EN, 0 << ADC1_DIG_EN);
		snd_codec_update_bits(codec, AC_ADC_DIG_CTRL, 1 << ADC2_DIG_EN, 0 << ADC2_DIG_EN);
		snd_codec_update_bits(codec, AC_ADC_DIG_CTRL, 1 << ADC3_DIG_EN, 0 << ADC3_DIG_EN);

		sunxi_codec->capturing = 0;
		sunxi_mad_close();
//		mad_bind = 0;
#ifndef MAD_CLK_ALWAYS_ON
		sunxi_mad_module_clk_enable(false);
#endif
	}
}
#endif

static void sunxi_codec_shutdown(struct snd_pcm_substream *substream, struct snd_dai *dai)
{
	struct snd_codec *codec = dai->component;

	snd_print("\n");

	/*
	 * Capture:
	 * Capture <-- ADC1 <-- Input Mixer <-- MIC1 PGA <-- MIC1
	 * Capture <-- ADC2 <-- Input Mixer <-- MIC2 PGA <-- MIC2
	 * Capture <-- ADC3 <-- Input Mixer <-- MIC3 PGA <-- MIC3
	 */
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		snd_err("unspport pcm stream playback\n");
	} else {
		/* digital ADC global disable */
		snd_codec_update_bits(codec, AC_ADC_DIG_CTRL, 0x1<<ADC_GEN, 0x0<<ADC_GEN);
		/* ana ADC disable */
		snd_codec_update_bits(codec, AC_ADC_ANA_CTRL1, 1 << ADC1_EN, 0 << ADC1_EN);
		snd_codec_update_bits(codec, AC_ADC_ANA_CTRL2, 1 << ADC2_EN, 0 << ADC2_EN);
		snd_codec_update_bits(codec, AC_ADC_ANA_CTRL3, 1 << ADC3_EN, 0 << ADC3_EN);
		/* power off */
		snd_codec_update_bits(codec, AC_MBIAS_CTRL, 1 << MBIAS_EN, 0 << MBIAS_EN);

#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
		snd_print("mad\n");
		sunxi_codec_mad_shutdown(substream, dai);
#endif
	}

	return;
}

#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
static int sunxi_codec_mad_prepare(struct snd_pcm_substream *substream, struct snd_dai *dai)
{
	struct snd_codec *codec = dai->component;
	struct sunxi_codec_info *sunxi_codec = codec->private_data;
	int ret = 0;
	bool mad_bind = false;

	snd_print("mad\n");

	ret = sunxi_mad_bind_get(MAD_PATH_CODECADC, &mad_bind);
	if (ret) {
		mad_bind = false;
		snd_err("get mad_bind failed, path: %d\n", MAD_PATH_CODECADC);
	}

	if (mad_bind) {
		snd_print("mad bind\n");
		if (sunxi_codec->capturing) {
			snd_err("capturing\n");
			return 0;
		} else {
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
				(1 << ADC_FIFO_FLUSH), (1 << ADC_FIFO_FLUSH));
			snd_codec_write(codec, AC_ADC_RXFIFO_STA,
				(1 << ADC_RXA_INT | 1 << ADC_RXO_INT));
			snd_codec_write(codec, AC_ADC_RXCNT, 0);
			sunxi_codec->capturing = 0;
			sunxi_codec_debug_chan_control(sunxi_codec, false);
		}
	} else {
		snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
			(1 << ADC_FIFO_FLUSH), (1 << ADC_FIFO_FLUSH));
		snd_codec_write(codec, AC_ADC_RXFIFO_STA,
			(1 << ADC_RXA_INT | 1 << ADC_RXO_INT));
		snd_codec_write(codec, AC_ADC_RXCNT, 0);
	}
	return 0;
}
#endif

static int sunxi_codec_prepare(struct snd_pcm_substream *substream, struct snd_dai *dai)
{
#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
#else
	struct snd_codec *codec = dai->component;
#endif

	snd_print("\n");

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		snd_err("unspport pcm stream playback\n");
	} else {
#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
		snd_print("mad\n");
		return sunxi_codec_mad_prepare(substream, dai);
#else
		snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
			(1 << ADC_FIFO_FLUSH), (1 << ADC_FIFO_FLUSH));
		snd_codec_write(codec, AC_ADC_RXFIFO_STA,
			(1 << ADC_RXA_INT | 1 << ADC_RXO_INT));
		snd_codec_write(codec, AC_ADC_RXCNT, 0);
#endif
	}

	return 0;
}

static int sunxi_codec_trigger(struct snd_pcm_substream *substream, int cmd, struct snd_dai *dai)
{
	struct snd_codec *codec = dai->component;

	snd_print("\n");

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			snd_err("unspport pcm stream playback\n");
		}
		else if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
			snd_print("mad\n");
			sunxi_codec_adc_mad_enable(codec, 1);
#else
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
				(1 << ADC_DRQ_EN), (1 << ADC_DRQ_EN));
#endif
		}
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			snd_err("unspport pcm stream playback\n");
		}
		else if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
				(1 << ADC_DRQ_EN), (0 << ADC_DRQ_EN));
		}
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static struct snd_dai_ops sun20iw2_codec_dai_ops = {
	.set_sysclk	= sunxi_codec_set_sysclk,
	.startup	= sunxi_codec_startup,
	.shutdown	= sunxi_codec_shutdown,
	.hw_params	= sunxi_codec_hw_params,
	.prepare	= sunxi_codec_prepare,
	.trigger	= sunxi_codec_trigger,
	.dapm_control   = sunxi_codec_dapm_control,
};

static struct snd_dai sun20iw2_codec_dai[] = {
	{
		.name = "sun20iw2codec-adc",
		.capture = {
			.stream_name	= "Capture",
			.channels_min	= 1,
			.channels_max	= 3,
			.rates		= SNDRV_PCM_RATE_8000_96000
					| SNDRV_PCM_RATE_KNOT,
			.formats	= SNDRV_PCM_FMTBIT_S16_LE
					| SNDRV_PCM_FMTBIT_S24_LE,
			.rate_min	= 8000,
			.rate_max	= 96000,
		},
		.ops = &sun20iw2_codec_dai_ops,
	},
};

static int snd_sunxi_clk_enable(struct sunxi_codec_clk *clk)
{
	int ret;

	snd_print("\n");

	/* rst & bus */
	ret = hal_reset_control_deassert(clk->clk_rst_adc);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_rst_adc clk_deassert failed.\n");
		goto err_deassert_clk_rst_adc;
	}
	ret = hal_clock_enable(clk->clk_bus_adc);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_bus_adc enable failed.\n");
		goto err_enable_clk_bus_adc;
	}

	/* enable high pll tree */
	ret = hal_clock_enable(clk->clk_pll_audio);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_pll_audio enable failed.\n");
		goto err_enable_clk_pll_audio;
	}
	ret = hal_clock_enable(clk->clk_ck1_aud_div);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_ck1_aud_div enable failed.\n");
		goto err_enable_clk_ck1_aud_div;
	}

	/* disable low pll tree */
	hal_clock_disable(clk->clk_aud_rco_div);

	/* enable module clk */
	ret = hal_clock_enable(clk->clk_adc_gate);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_adc_gate enable failed.\n");
		goto err_enable_clk_adc_gate;
	}

	return HAL_CLK_STATUS_OK;

err_enable_clk_adc_gate:
	hal_clock_disable(clk->clk_ck1_aud_div);
err_enable_clk_ck1_aud_div:
	hal_clock_disable(clk->clk_pll_audio);
err_enable_clk_pll_audio:
	hal_clock_disable(clk->clk_bus_adc);
err_enable_clk_bus_adc:
	hal_reset_control_assert(clk->clk_rst_adc);
err_deassert_clk_rst_adc:
	return HAL_CLK_STATUS_ERROR;
}

static void snd_sunxi_clk_disable(struct sunxi_codec_clk *clk)
{
	int ret;

	snd_print("\n");

	switch (clk->clk_work_mode) {
	case SUNXI_CODEC_CLK_WORK_RESUME:
		/* assert rst, disable bus & high pll tree & low pll tree */
		hal_clock_disable(clk->clk_adc_gate);
		hal_clock_disable(clk->clk_aud_rco_div);
		hal_clock_disable(clk->clk_ck1_aud_div);
		hal_clock_disable(clk->clk_pll_audio);
		hal_clock_disable(clk->clk_bus_adc);
		hal_reset_control_assert(clk->clk_rst_adc);
		break;
	case SUNXI_CODEC_CLK_WORK_SUSPEND_LOW:
		/* keep busclk on & deassert rst, disable high pll tree, enable low pll tree */
		hal_clock_disable(clk->clk_ck1_aud_div);
		hal_clock_disable(clk->clk_pll_audio);
		ret = hal_clock_enable(clk->clk_aud_rco_div);
		if (ret != HAL_CLK_STATUS_OK) {
			snd_err("codec clk_aud_rco_div enable failed.\n");
		}
		break;
	case SUNXI_CODEC_CLK_WORK_SUSPEND_HIGH:
		/* keep busclk on & deassert rst & high pll tree on, disable low pll tree */
		hal_clock_disable(clk->clk_aud_rco_div);
		break;
	default:
		snd_err("clk work mode invaild\n");
		break;
	}

	return;
}

static int snd_sunxi_clk_init(struct sunxi_codec_clk *clk)
{
	int ret;

	snd_print("\n");

	/* rst & bus */
	clk->clk_rst_adc = hal_reset_control_get(HAL_SUNXI_AON_RESET, RST_CODEC_ADC);
	if (!clk->clk_rst_adc) {
		snd_err("codec clk_rst_adc hal_reset_control_get failed\n");
		goto err_get_clk_rst_adc;
	}
	clk->clk_bus_adc = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_BUS_CODEC_ADC);
	if (!clk->clk_bus_adc) {
		snd_err("codec clk_bus_adc hal_clock_get failed\n");
		goto err_get_clk_bus_adc;
	}

	/* high pll clk -> 24.576M */
	clk->clk_pll_audio = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_PLL_AUDIO);
	if (!clk->clk_pll_audio) {
		snd_err("codec clk_pll_audio hal_clock_get failed\n");
		goto err_get_clk_pll_audio;
	}
	clk->clk_pll_audio1x = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_PLL_AUDIO1X);
	if (!clk->clk_pll_audio1x) {
		snd_err("codec clk_pll_audio1x hal_clock_get failed\n");
		goto err_get_clk_pll_audio1x;
	}
	clk->clk_pll_audio2x = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_PLL_AUDIO2X);
	if (!clk->clk_pll_audio2x) {
		snd_err("codec clk_pll_audio2x hal_clock_get failed\n");
		goto err_get_clk_pll_audio2x;
	}
	clk->clk_hosc = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_HOSC);
	if (!clk->clk_hosc) {
		snd_err("codec clk_hosc hal_clock_get failed\n");
		goto err_get_clk_hosc;
	}

	clk->clk_audpll_hosc_sel = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_AUDPLL_HOSC_SEL);
	if (!clk->clk_audpll_hosc_sel) {
		snd_err("codec clk_audpll_hosc_sel hal_clock_get failed\n");
		goto err_get_clk_audpll_hosc_sel;
	}

	/* high pll clk -> 22.5792M */
	clk->clk_ck1_aud_div = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_CK1_AUD_DIV);
	if (!clk->clk_ck1_aud_div) {
		snd_err("codec clk_ck1_aud_div hal_clock_get failed\n");
		goto err_get_clk_ck1_aud_div;
	}

	/* low pll clk -> 8.192M */
	clk->clk_aud_rco_div = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_AUD_RCO_DIV);
	if (!clk->clk_aud_rco_div) {
		snd_err("codec clk_aud_rco_div hal_clock_get failed\n");
		goto err_get_clk_aud_rco_div;
	}

	/* module adc clk */
	clk->clk_adc_div = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_CODEC_ADC_DIV);
	if (!clk->clk_adc_div) {
		snd_err("codec clk_adc_div hal_clock_get failed\n");
		goto err_get_clk_adc_div;
	}
	clk->clk_adc_sel1 = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_CODEC_ADC_SEL1);
	if (!clk->clk_adc_sel1) {
		snd_err("codec clk_adc_sel1 hal_clock_get failed\n");
		goto err_get_clk_adc_sel1;
	}
	clk->clk_adc_gate = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_CODEC_ADC_GATE);
	if (!clk->clk_adc_gate) {
		snd_err("codec clk_adc_gate hal_clock_get failed\n");
		goto err_get_clk_adc_gate;
	}

	/* pll tree: (24.576MHz)(default)
	 * CLK_PLL_AUDIO --> CLK_PLL_AUDIO1X --> CLK_AUDPLL_HOSC_SEL --> CLK_CODEC_ADC_DIV --> CLK_CODEC_ADC_SEL1 --> CLK_CODEC_ADC_GATE
	 *
	 * pll tree: (22.5792MHz)
	 * CLK_DPLL1 --> CLK_CK1_AUD --> CLK_CK1_AUD_DIV --> CLK_CODEC_ADC_DIV --> CLK_CODEC_ADC_SEL1 --> CLK_CODEC_ADC_GATE
	 *
	 * pll tree: (8.192MHz)
	 * CLK_AUD_RCO_DIV --> CLK_CODEC_ADC_SEL1 --> CLK_CODEC_ADC_GATE
	 */
	ret = hal_clk_set_parent(clk->clk_audpll_hosc_sel, clk->clk_pll_audio1x);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_pll_audio1x -> clk_audpll_hosc_sel clk_set_parent failed.\n");
		goto err_set_parent_clk;
	}

	ret = hal_clk_set_parent(clk->clk_adc_div, clk->clk_audpll_hosc_sel);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_audpll_hosc_sel -> clk_adc_div clk_set_parent failed.\n");
		goto err_set_parent_clk;
	}

	ret = hal_clk_set_parent(clk->clk_adc_sel1, clk->clk_adc_div);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_adc_div -> clk_adc_sel1 clk_set_parent failed.\n");
		goto err_set_parent_clk;
	}

	/* note: Enable and then set the freq to avoid clock lock errors */
	ret = snd_sunxi_clk_enable(clk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec snd_sunxi_clk_enable failed.\n");
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
	hal_clock_put(clk->clk_adc_gate);
err_get_clk_adc_gate:
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
	hal_clock_put(clk->clk_bus_adc);
err_get_clk_bus_adc:
	hal_reset_control_put(clk->clk_rst_adc);
err_get_clk_rst_adc:
	return HAL_CLK_STATUS_ERROR;
}

static void snd_sunxi_clk_exit(struct sunxi_codec_clk *clk)
{
	snd_print("\n");

	snd_sunxi_clk_disable(clk);
	hal_clock_put(clk->clk_adc_gate);
	hal_clock_put(clk->clk_adc_sel1);
	hal_clock_put(clk->clk_adc_div);
	hal_clock_put(clk->clk_aud_rco_div);
	hal_clock_put(clk->clk_ck1_aud_div);
	hal_clock_put(clk->clk_audpll_hosc_sel);
	hal_clock_put(clk->clk_hosc);
	hal_clock_put(clk->clk_pll_audio2x);
	hal_clock_put(clk->clk_pll_audio1x);
	hal_clock_put(clk->clk_bus_adc);
	hal_reset_control_put(clk->clk_rst_adc);

	return;
}

static int snd_sunxi_clk_set_parent(struct sunxi_codec_clk *clk)
{
	int ret;

	switch (clk->clk_work_mode) {
	case SUNXI_CODEC_CLK_WORK_RESUME:
	case SUNXI_CODEC_CLK_WORK_SUSPEND_HIGH:
		if (clk->pll_freq_resume == 24583333) {
			ret = hal_clk_set_parent(clk->clk_adc_div, clk->clk_audpll_hosc_sel);
			if (ret != HAL_CLK_STATUS_OK) {
				snd_err("codec clk_audpll_hosc_sel -> clk_adc_div clk_set_parent failed\n");
				return HAL_CLK_STATUS_ERROR;
			}
		} else if (clk->pll_freq_resume == 22588236) {
			ret = hal_clk_set_parent(clk->clk_adc_div, clk->clk_ck1_aud_div);
			if (ret != HAL_CLK_STATUS_OK) {
				snd_err("codec clk_ck1_aud_div -> clk_adc_div clk_set_parent failed.\n");
				return HAL_CLK_STATUS_ERROR;
			}
		} else {
			snd_err("unsupport pll freq work in resume: %u\n", clk->pll_freq_resume);
			return HAL_CLK_STATUS_ERROR;
		}
		ret = hal_clk_set_parent(clk->clk_adc_sel1, clk->clk_adc_div);
		if (ret != HAL_CLK_STATUS_OK) {
			snd_err("codec clk_adc_div -> clk_adc_sel1 clk_set_parent failed.\n");
			return HAL_CLK_STATUS_ERROR;
		}
		break;
	case SUNXI_CODEC_CLK_WORK_SUSPEND_LOW:
		ret = hal_clk_set_parent(clk->clk_adc_sel1, clk->clk_aud_rco_div);
		if (ret != HAL_CLK_STATUS_OK) {
			snd_err("codec clk_aud_rco_div -> clk_adc_sel1 clk_set_parent failed.\n");
			return HAL_CLK_STATUS_ERROR;
		}
		break;
	default:
		snd_err("clk work mode invaild\n");
		return HAL_CLK_STATUS_ERROR;
		break;
	}

	return HAL_CLK_STATUS_OK;
}

static int snd_sunxi_clk_set_rate(struct sunxi_codec_clk *clk, int stream,
				  unsigned int freq_in, unsigned int freq_out)
{
	int ret;
	unsigned int freq;

	(void)freq_in;

	if (stream == SNDRV_PCM_STREAM_PLAYBACK) {
		snd_err("unspport pcm stream playback\n");
		return HAL_CLK_STATUS_ERROR;
	}

	if (freq_out == 24576000 || freq_out == 24583333)
		freq = 24583333;
	else
		freq = 22588236;
	clk->pll_freq_resume = freq;
	clk->clk_work_mode = SUNXI_CODEC_CLK_WORK_RESUME;

	ret = snd_sunxi_clk_set_parent(clk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("snd_sunxi_clk_set_parent failed\n");
		return HAL_CLK_STATUS_ERROR;
	}

	/* note: CLK_CODEC_DAC no need set rate */
	if (stream != SNDRV_PCM_STREAM_PLAYBACK) {
		ret = hal_clk_set_rate(clk->clk_adc_div, freq);
		if (ret != HAL_CLK_STATUS_OK) {
			snd_err("set clk_adc_div rate %u failed\n", freq);
			return HAL_CLK_STATUS_ERROR;
		}
	}

	return HAL_CLK_STATUS_OK;
}

/* suspend and resume */
#ifdef CONFIG_COMPONENTS_PM
static unsigned int snd_read_func(void *data, unsigned int reg)
{
	struct snd_codec *codec;

	if (!data) {
		snd_err("data is invailed\n");
		return 0;
	}
	codec = data;

	return snd_codec_read(codec, reg);
}

static void snd_write_func(void *data, unsigned int reg, unsigned int val)
{
	struct snd_codec *codec;

	if (!data) {
		snd_err("data is invailed\n");
		return;
	}
	codec = data;

	snd_codec_write(codec, reg, val);
}

#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
static int sunxi_codec_prepared(struct pm_device *dev, suspend_mode_t mode)
{
	int ret;
	struct snd_codec *codec;
	struct sunxi_codec_info *sunxi_codec;
	struct sunxi_codec_clk *clk;

	bool mad_bind = false;

	if (!dev->data) {
		snd_err("data is invailed\n");
		return 0;
	}
	codec = dev->data;
	sunxi_codec = codec->private_data;
	clk = &sunxi_codec->clk;

	ret = sunxi_mad_bind_get(MAD_PATH_CODECADC, &mad_bind);
	if (ret) {
		mad_bind = false;
		snd_err("get mad_bind failed, path: %d\n", MAD_PATH_CODECADC);
		return 0;
	}
	if (!mad_bind) {
		snd_print("mad unbind\n");
		return 0;
	}

	ret = sunxi_codec_hwrate_set(codec, false, SNDRV_PCM_STREAM_CAPTURE, clk->sample_rates);
	if (ret) {
		snd_err("sunxi_codec_hwrate_set to suspend failed\n");
		return ret;
	}

	sunxi_codec_mad_enter_standby(sunxi_codec);
	sunxi_mad_suspend_external();
	sunxi_codec->mad_priv.mad_suspend = 1;

	return 0;
}

static int sunxi_codec_resume_early(struct pm_device *dev, suspend_mode_t mode)
{
	int ret;
	struct snd_codec *codec;
	struct sunxi_codec_info *sunxi_codec;

	bool mad_bind = false;

	if (!dev->data) {
		snd_err("data is invailed\n");
		return 0;
	}
	codec = dev->data;
	sunxi_codec = codec->private_data;

	ret = sunxi_mad_bind_get(MAD_PATH_CODECADC, &mad_bind);
	if (ret) {
		mad_bind = false;
		snd_err("get mad_bind failed, path: %d\n", MAD_PATH_CODECADC);
		return 0;
	}
	if (!mad_bind) {
		snd_print("mad unbind\n");
		return 0;
	}

	sunxi_mad_resume_external();
	sunxi_codec->mad_priv.mad_suspend = 0;

	return 0;
}
#else
#define sunxi_codec_prepared		NULL
#define sunxi_codec_resume_early	NULL
#endif

static int sunxi_codec_suspend(struct pm_device *dev, suspend_mode_t mode)
{
	struct snd_codec *codec = dev->data;
	struct sunxi_codec_info *sunxi_codec = codec->private_data;

	snd_sunxi_save_reg(sunxi_reg_labels, (void *)codec, snd_read_func);
	snd_sunxi_clk_disable(&sunxi_codec->clk);

	return 0;
}

static int sunxi_codec_resume(struct pm_device *dev, suspend_mode_t mode)
{
	struct snd_codec *codec = dev->data;
	struct sunxi_codec_info *sunxi_codec = codec->private_data;
	struct sunxi_codec_clk *clk = &sunxi_codec->clk;
	int ret;
	unsigned int pll_freq = clk->pll_freq_resume;

	snd_sunxi_clk_enable(&sunxi_codec->clk);
	ret = snd_sunxi_clk_set_rate(clk, SNDRV_PCM_STREAM_CAPTURE, pll_freq, pll_freq);
	if (ret < 0) {
		snd_err("snd_sunxi_clk_set_rate failed\n");
		return -1;
	}

	sunxi_codec_init(codec);
	snd_sunxi_echo_reg(sunxi_reg_labels, (void *)codec, snd_write_func);

	ret = sunxi_codec_hwrate_set(codec, true, SNDRV_PCM_STREAM_CAPTURE, clk->sample_rates);
	if (ret) {
		snd_err("sunxi_codec_hwrate_set to resume failed\n");
		return ret;
	}

	return 0;
}

struct pm_devops pm_audiocodec_adc_ops = {
	.prepared	= sunxi_codec_prepared,
	.suspend	= sunxi_codec_suspend,
	.resume_early	= sunxi_codec_resume_early,
	.resume		= sunxi_codec_resume,
};

struct pm_device pm_audiocodec_adc = {
	.name = "audiocodecadc",
	.ops = &pm_audiocodec_adc_ops,
};
#endif

/* audiocodec probe */
static int sun20iw2_codec_probe(struct snd_codec *codec)
{
	struct sunxi_codec_info *sunxi_codec = NULL;
	hal_clk_status_t ret;

	snd_print("\n");

	if (!codec->codec_dai)
		return -1;

	sunxi_codec = snd_malloc(sizeof(struct sunxi_codec_info));
	if (!sunxi_codec) {
		snd_err("no memory\n");
		return -ENOMEM;
	}

	codec->private_data = (void *)sunxi_codec;
	sunxi_codec->codec = codec;

	sunxi_codec->param = default_param;
	codec->codec_base_addr = (void *)SUNXI_CODEC_BASE_ADDR;
	codec->codec_dai->component = codec;

	ret = snd_sunxi_clk_init(&sunxi_codec->clk);
	if (ret != 0) {
		snd_err("snd_sunxi_clk_init failed\n");
		goto err_codec_set_clock;
	}

	sunxi_codec_init(codec);

#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
	snd_print("mad\n");
	ret = sunxi_mad_platform_probe();
	if (ret != 0) {
		snd_err("sunxi_mad_platform_probe failed.\n");
		goto err_codec_mad_platform_probe;
	}
	ret = sunxi_mad_bind_set(MAD_PATH_CODECADC, default_param.mad_bind_en);
	if (ret) {
		snd_err("set mad_bind failed, path: %d\n", MAD_PATH_CODECADC);
		goto err_codec_mad_platform_probe;
	}
#endif

#ifdef CONFIG_COMPONENTS_PM
	pm_audiocodec_adc.data = (void *)codec;
	ret = pm_devops_register(&pm_audiocodec_adc);
	if (ret) {
		snd_err("pm_devops_register failed\n");
	}
#endif

	return 0;

#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
err_codec_mad_platform_probe:
#endif
err_codec_set_clock:
	snd_sunxi_clk_exit(&sunxi_codec->clk);

	return -1;
}

static int sun20iw2_codec_remove(struct snd_codec *codec)
{
	struct sunxi_codec_info *sunxi_codec = codec->private_data;

	snd_print("\n");

#ifdef CONFIG_SND_PLATFORM_SUNXI_MAD
	snd_print("mad\n");
	sunxi_mad_platform_remove();
#endif

	snd_sunxi_clk_exit(&sunxi_codec->clk);

	snd_free(sunxi_codec);
	codec->private_data = NULL;

	return 0;
}

/*
 * capture : period_bytes=320*(16*3/8)=1920, buffer_bytes=1920*4=7680
 */
static struct snd_pcm_hardware sun20iw2_hardware[2] = {
	{ },
	{	/* SNDRV_PCM_STREAM_CAPTURE */
		.info			= SNDRV_PCM_INFO_INTERLEAVED
					| SNDRV_PCM_INFO_BLOCK_TRANSFER
					| SNDRV_PCM_INFO_MMAP
					| SNDRV_PCM_INFO_MMAP_VALID
					| SNDRV_PCM_INFO_PAUSE
					| SNDRV_PCM_INFO_RESUME,
		.buffer_bytes_max	= 7680,
		.period_bytes_min	= 256,
		.period_bytes_max	= 1920,
		.periods_min		= 2,
		.periods_max		= 4,
	},
};

struct snd_codec sunxi_audiocodec_adc = {
	.name		= "audiocodecadc",
	.codec_dai	= sun20iw2_codec_dai,
	.codec_dai_num  = ARRAY_SIZE(sun20iw2_codec_dai),
	.private_data	= NULL,
	.probe		= sun20iw2_codec_probe,
	.remove		= sun20iw2_codec_remove,
	.controls       = sunxi_codec_controls,
	.num_controls   = ARRAY_SIZE(sunxi_codec_controls),
	.hw 		= sun20iw2_hardware,
	.capture_only	= true,
};
