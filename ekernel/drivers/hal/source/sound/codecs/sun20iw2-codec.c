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
#include <sound/snd_core.h>
#include <sound/snd_pcm.h>
#include <sound/pcm_common.h>
#include <hal_gpio.h>
#include <sound/snd_dma.h>
#include <sound/dma_wrap.h>
#include <hal_dma.h>
#include <hal_clk.h>
#include <hal_reset.h>
#include <hal_time.h>
#include <sunxi_hal_timer.h>

#include "sunxi-codec.h"

struct snd_codec sunxi_audiocodec;

static struct sunxi_codec_param default_param = {
	.mic1gain	= 0x1f,
	.mic2gain	= 0x1f,
	.mic3gain	= 0x1f,
	.mic1_en	= true,
	.mic2_en	= true,
	.mic3_en	= true,
	/*
	.gpio_spk	= GPIOB(7),
	.pa_msleep_time = 160,
	.pa_level	= GPIO_DATA_HIGH,
	*/
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

const char *const codec_switch_onoff[] = {"off", "on"};

static int snd_sunxi_clk_init(struct sunxi_codec_clk *clk);
static void snd_sunxi_clk_exit(struct sunxi_codec_clk *clk);
static int snd_sunxi_clk_enable(struct sunxi_codec_clk *clk);
static void snd_sunxi_clk_disable(struct sunxi_codec_clk *clk);
static int snd_sunxi_clk_set_rate(struct sunxi_codec_clk *clk, int stream,
				  unsigned int freq_in, unsigned int freq_out);

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
	/* -64dB to 63dB, 0.5dB/step, if regvol==0 will mute */
	SND_CTL_KCONTROL("DACL dig volume", AC_DAC_DIG_VOL, DACL_DIG_VOL, 0xff),
	SND_CTL_KCONTROL("DACR dig volume", AC_DAC_DIG_VOL, DACR_DIG_VOL, 0xff),
	SND_CTL_KCONTROL("MIC1 volume", AC_ADC_ANA_CTRL1, ADC1_PGA_GAIN_CTRL, 0x1F),
	SND_CTL_KCONTROL("MIC2 volume", AC_ADC_ANA_CTRL2, ADC2_PGA_GAIN_CTRL, 0x1F),
	SND_CTL_KCONTROL("MIC3 volume", AC_ADC_ANA_CTRL3, ADC3_PGA_GAIN_CTRL, 0x1F),
	/* line out switch */
	SND_CTL_ENUM("LINEOUTL switch",
		     ARRAY_SIZE(codec_switch_onoff), codec_switch_onoff,
		     AC_DAC_DIG_CTRL, DACL_DIG_EN),
	SND_CTL_ENUM("LINEOUTR switch",
		     ARRAY_SIZE(codec_switch_onoff), codec_switch_onoff,
		     AC_DAC_DIG_CTRL, DACR_DIG_EN),
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

	/* *** DAC PATH *** */
	/* DAC data from TX mixer */
	snd_codec_update_bits(codec, AC_DAC_DIG_CTRL,
			      0x3<<DAC_PTN_SEL, 0x0<<DAC_PTN_SEL);

	/* *** ADC PATH *** */
	/* ADC data from RX mixer */
	snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
			      0x3<<ADC_PTN_SEL, 0x0<<ADC_PTN_SEL);

	/* *** debug *** */
	/* ADC data fixed as 0x5a5a5a */
	/*
	snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
			      0x3<<ADC_PTN_SEL, 0x1<<ADC_PTN_SEL);
	*/
	/* lineout en */
	snd_codec_update_bits(codec, AC_DAC_DIG_CTRL,
			      0x1<<DACL_DIG_EN, 0x1<<DACL_DIG_EN);
	snd_codec_update_bits(codec, AC_DAC_DIG_CTRL,
			      0x1<<DACR_DIG_EN, 0x1<<DACR_DIG_EN);

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

	if (substream->dapm_state == onoff)
		return 0;
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		/*
		 * Playback:
		 * Playback --> DAC1 -->  LINEOUT1 --> External Speaker
		 * Playback --> DAC2 -->  LINEOUT2 --> External Speaker
		 */
		if (onoff) {
			/* digital DAC global enable */
			snd_codec_update_bits(codec, AC_DAC_DIG_CTRL,
					0x1<<DAC_GEN, 0x1<<DAC_GEN);
			/* analog DAC enable */
			snd_codec_update_bits(codec, AC_DAC_ANA_CTRL,
					(0x1<<DACL_EN) | (0x1<<DACR_EN),
					(0x1<<DACL_EN) | (0x1<<DACR_EN));
			/*
			if (param->gpio_spk > 0) {
				hal_gpio_set_direction(param->gpio_spk, GPIO_MUXSEL_OUT);
				hal_gpio_set_data(param->gpio_spk, param->pa_level);
				hal_msleep(param->pa_msleep_time);
			}
			*/
		} else {
			/*
			if (param->gpio_spk > 0) {
				hal_gpio_set_direction(param->gpio_spk, GPIO_MUXSEL_OUT);
				hal_gpio_set_data(param->gpio_spk, !param->pa_level);
			}
			*/
			/* analog DAC */
			snd_codec_update_bits(codec, AC_DAC_ANA_CTRL,
					(0x1<<DACL_EN) | (0x1<<DACR_EN),
					(0x0<<DACL_EN) | (0x0<<DACR_EN));
			/* digital DAC global disable */
			snd_codec_update_bits(codec, AC_DAC_DIG_CTRL,
					0x1<<DAC_GEN, 0x0<<DAC_GEN);
		}
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
			snd_codec_update_bits(codec, AC_MBIAS_CTRL,
					0x1<<MBIAS_EN, 0x1<<MBIAS_EN);
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
			snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
					0x1<<ADC_GEN, 0x1<<ADC_GEN);
		} else {
			/* digital ADC global disable */
			snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
					0x1<<ADC_GEN, 0x0<<ADC_GEN);
			/* ana ADC disable */
			snd_codec_update_bits(codec, AC_ADC_ANA_CTRL1,
					1 << ADC1_EN, 0 << ADC1_EN);
			snd_codec_update_bits(codec, AC_ADC_ANA_CTRL2,
					1 << ADC2_EN, 0 << ADC2_EN);
			snd_codec_update_bits(codec, AC_ADC_ANA_CTRL3,
					1 << ADC3_EN, 0 << ADC3_EN);
			/* power off */
			snd_codec_update_bits(codec, AC_MBIAS_CTRL,
					0x1<<MBIAS_EN, 0x0<<MBIAS_EN);
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


static int sunxi_codec_hw_params(struct snd_pcm_substream *substream,
				 struct snd_pcm_hw_params *params,
				 struct snd_dai *dai)
{
	struct snd_codec *codec = dai->component;
	struct sunxi_codec_info *sunxi_codec = codec->private_data;
	struct sunxi_codec_param *codec_param = &sunxi_codec->param;
	int i;
	unsigned int simple_rates;
	unsigned int simple_channels;

	snd_print("\n");

	/* simple bit */
	switch (params_format(params)) {
	case	SND_PCM_FORMAT_S16_LE:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			snd_codec_update_bits(codec, AC_DAC_TXFIFO_CTRL,
				1 << DAC_FIFO_MODE, 0 << DAC_FIFO_MODE);
			snd_codec_update_bits(codec, AC_DAC_TXFIFO_CTRL,
				1 << DAC_SAMPLE_BITS, 0 << DAC_SAMPLE_BITS);
		} else {
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
				1 << ADC_FIFO_MODE, 1 << ADC_FIFO_MODE);
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
				1 << ADC_SAMPLE_BITS, 0 << ADC_SAMPLE_BITS);
		}
		break;
	case	SND_PCM_FORMAT_S24_LE:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			snd_codec_update_bits(codec, AC_DAC_TXFIFO_CTRL,
				1 << DAC_FIFO_MODE, 0 << DAC_FIFO_MODE);
			snd_codec_update_bits(codec, AC_DAC_TXFIFO_CTRL,
				1 << DAC_SAMPLE_BITS, 1 << DAC_SAMPLE_BITS);
		} else {
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
				1 << ADC_FIFO_MODE, 1 << ADC_FIFO_MODE);
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
				1 << ADC_SAMPLE_BITS, 1 << ADC_SAMPLE_BITS);
		}
		break;
	default:
		snd_err("cannot support bits:%u.\n", params_format(params));
		return -EINVAL;
	}

	/* simple rate */
	simple_rates = params_rate(params);
	for (i = 0; i < ARRAY_SIZE(sample_rate_conv); i++) {
		if (sample_rate_conv[i].samplerate == simple_rates)
			break;
	}
	if (sample_rate_conv[i].samplerate != simple_rates) {
		snd_err("cannot support play rates:%u.\n", simple_rates);
		return -EINVAL;
	}
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		/* for digital clk */
		snd_codec_update_bits(codec, AC_DAC_DIG_CTRL, 0xf << DAC_CLK,
			sample_rate_conv[i].rate_bit << DAC_CLK);
		/* for analog clk */
		switch (simple_rates) {
		case 384000:
		case 352800:
		case 192000:
		case 176400:
			snd_codec_update_bits(codec, AC_DAC_DIG_CTRL,
				3 << DAC_OSR, 2 << DAC_OSR);
		case 96000:
		case 88200:
			snd_codec_update_bits(codec, AC_DAC_DIG_CTRL,
				3 << DAC_OSR, 1 << DAC_OSR);
		default:
			snd_codec_update_bits(codec, AC_DAC_DIG_CTRL,
				3 << DAC_OSR, 0 << DAC_OSR);
		}
	} else {
		if (simple_rates > 96000) {
			snd_err("cannot support capture rates:%u.\n", simple_rates);
			return -EINVAL;
		}
		/* for digital clk */
		snd_codec_update_bits(codec, AC_ADC_DIG_CTRL, 0xf << ADC_CLK,
			sample_rate_conv[i].rate_bit << ADC_CLK);
		/* for analog clk */
		switch (simple_rates) {
		case 96000:
		case 88200:
			snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
				3 << ADC_OSR, 1 << ADC_OSR);
		default:
			snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
				3 << ADC_OSR, 0 << ADC_OSR);
		}
	}

	/* simple channel */
	simple_channels = params_channels(params);
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		switch (simple_channels) {
		case 1:
			snd_codec_update_bits(codec, AC_DAC_TXFIFO_CTRL,
					1 << DAC_MONO_EN, 1 << DAC_MONO_EN);
			break;
		case 2:
			snd_codec_update_bits(codec, AC_DAC_TXFIFO_CTRL,
					1 << DAC_MONO_EN, 0 << DAC_MONO_EN);
			break;
		default:
			snd_err("cannot support the channels:%u.\n", simple_channels);
			return -EINVAL;
		}
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

static void sunxi_codec_shutdown(struct snd_pcm_substream *substream, struct snd_dai *dai)
{
	struct snd_codec *codec = dai->component;

	snd_print("\n");

	/*
	 * Playback:
	 * Playback --> DAC1 -->  LINEOUT1 --> External Speaker
	 * Playback --> DAC2 -->  LINEOUT2 --> External Speaker
	 *
	 * Capture:
	 * Capture <-- ADC1 <-- Input Mixer <-- MIC1 PGA <-- MIC1
	 * Capture <-- ADC2 <-- Input Mixer <-- MIC2 PGA <-- MIC2
	 * Capture <-- ADC3 <-- Input Mixer <-- MIC3 PGA <-- MIC3
	 */
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
#if 0
		if (param->gpio_spk > 0)
			hal_gpio_set_data(param->gpio_spk, !param->pa_level);
#endif
		/* analog DAC */
		snd_codec_update_bits(codec, AC_DAC_ANA_CTRL,
				(0x1<<DACL_EN) | (0x1<<DACR_EN),
				(0x0<<DACL_EN) | (0x0<<DACR_EN));
		/* digital DAC global disable */
		snd_codec_update_bits(codec, AC_DAC_DIG_CTRL,
				0x1<<DAC_GEN, 0x0<<DAC_GEN);
	} else {
		/* digital ADC global disable */
		snd_codec_update_bits(codec, AC_ADC_DIG_CTRL,
				0x1<<ADC_GEN, 0x0<<ADC_GEN);
		/* ana ADC disable */
		snd_codec_update_bits(codec, AC_ADC_ANA_CTRL1,
				1 << ADC1_EN, 0 << ADC1_EN);
		snd_codec_update_bits(codec, AC_ADC_ANA_CTRL2,
				1 << ADC2_EN, 0 << ADC2_EN);
		snd_codec_update_bits(codec, AC_ADC_ANA_CTRL3,
				1 << ADC3_EN, 0 << ADC3_EN);
		/* power off */
		snd_codec_update_bits(codec, AC_MBIAS_CTRL,
				0x1<<MBIAS_EN, 0x0<<MBIAS_EN);
	}

	return;
}

static int sunxi_codec_prepare(struct snd_pcm_substream *substream, struct snd_dai *dai)
{
	struct snd_codec *codec = dai->component;

	snd_print("\n");

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		snd_codec_update_bits(codec, AC_DAC_TXFIFO_CTRL,
			(1 << DAC_FIFO_FLUSH), (1 << DAC_FIFO_FLUSH));
		snd_codec_write(codec, AC_DAC_TXFIFO_STA,
			(1 << DAC_TXE_INT | 1 << DAC_TXU_INT | 1 << DAC_TXO_INT));
		snd_codec_write(codec, AC_DAC_TXCNT, 0);
	} else {
		snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
			(1 << ADC_FIFO_FLUSH), (1 << ADC_FIFO_FLUSH));
		snd_codec_write(codec, AC_ADC_RXFIFO_STA,
			(1 << ADC_RXA_INT | 1 << ADC_RXO_INT));
		snd_codec_write(codec, AC_ADC_RXCNT, 0);
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
			snd_codec_update_bits(codec, AC_DAC_TXFIFO_CTRL,
				(1 << DAC_DRQ_EN), (1 << DAC_DRQ_EN));
		}
		else if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
			snd_codec_update_bits(codec, AC_ADC_RXFIFO_CTRL,
				(1 << ADC_DRQ_EN), (1 << ADC_DRQ_EN));
		}
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			snd_codec_update_bits(codec, AC_DAC_TXFIFO_CTRL,
				(1 << DAC_DRQ_EN), (0 << DAC_DRQ_EN));
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
		.name	= "sun20iw2codec",
		.playback = {
			.stream_name = "Playback",
			.channels_min = 1,
			.channels_max = 2,
			/* 800_384000  */
			.rates	= SNDRV_PCM_RATE_8000_192000
				| SNDRV_PCM_RATE_KNOT,
			.formats = SNDRV_PCM_FMTBIT_S16_LE
				| SNDRV_PCM_FMTBIT_S24_LE,
			.rate_min	= 8000,
			.rate_max	= 192000,
		},
		.capture = {
			.stream_name = "Capture",
			.channels_min = 1,
			.channels_max = 3,
			.rates = SNDRV_PCM_RATE_8000_96000
				| SNDRV_PCM_RATE_KNOT,
			.formats = SNDRV_PCM_FMTBIT_S16_LE
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
	ret = hal_reset_control_deassert(clk->clk_rst_dac);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_rst_dac clk_deassert failed.\n");
		goto err_deassert_clk_rst_dac;
	}
	ret = hal_reset_control_deassert(clk->clk_rst_adc);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_rst_adc clk_deassert failed.\n");
		goto err_deassert_clk_rst_adc;
	}
	ret = hal_clock_enable(clk->clk_bus_dac);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_bus_dac enable failed.\n");
		goto err_enable_clk_bus_dac;
	}
	ret = hal_clock_enable(clk->clk_bus_adc);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_bus_adc enable failed.\n");
		goto err_enable_clk_bus_adc;
	}

	/* pll */
	ret = hal_clock_enable(clk->clk_pll_audio);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_pll_audio enable failed.\n");
		goto err_enable_clk_pll_audio;
	}
	/*
	ret = hal_clock_enable(clk->clk_hosc);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_hosc enable failed.\n");
		goto err_enable_clk_hosc;
	}
	*/
	ret = hal_clock_enable(clk->clk_ck1_aud_div);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_ck1_aud_div enable failed.\n");
		goto err_enable_clk_ck1_aud_div;
	}
	/*
	ret = hal_clock_enable(clk->clk_aud_rco_div);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_aud_rco_div enable failed.\n");
		goto err_enable_clk_aud_rco_div;
	}
	*/
	ret = hal_clock_enable(clk->clk_dac);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_dac enable failed.\n");
		goto err_enable_clk_dac;
	}
	ret = hal_clock_enable(clk->clk_adc_gate);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_adc_gate enable failed.\n");
		goto err_enable_clk_adc_gate;
	}

	return HAL_CLK_STATUS_OK;

err_enable_clk_adc_gate:
	hal_clock_disable(clk->clk_dac);
err_enable_clk_dac:
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
	hal_clock_disable(clk->clk_bus_adc);
err_enable_clk_bus_adc:
	hal_clock_disable(clk->clk_bus_dac);
err_enable_clk_bus_dac:
	hal_reset_control_assert(clk->clk_rst_adc);
err_deassert_clk_rst_adc:
	hal_reset_control_assert(clk->clk_rst_dac);
err_deassert_clk_rst_dac:
	return HAL_CLK_STATUS_ERROR;
}

static void snd_sunxi_clk_disable(struct sunxi_codec_clk *clk)
{
	snd_print("\n");

	hal_clock_disable(clk->clk_adc_gate);
	hal_clock_disable(clk->clk_dac);
	/* hal_clock_disable(clk->clk_aud_rco_div); */
	hal_clock_disable(clk->clk_ck1_aud_div);
	/* hal_clock_disable(clk->clk_hosc); */
	hal_clock_disable(clk->clk_pll_audio);
	hal_clock_disable(clk->clk_bus_adc);
	hal_clock_disable(clk->clk_bus_dac);
	hal_reset_control_assert(clk->clk_rst_adc);
	hal_reset_control_assert(clk->clk_rst_dac);

	return;
}

static int snd_sunxi_clk_init(struct sunxi_codec_clk *clk)
{
	int ret;

	snd_print("\n");

	/* rst & bus */
	clk->clk_rst_dac = hal_reset_control_get(HAL_SUNXI_AON_RESET, RST_CODEC_DAC);
	if (!clk->clk_rst_dac) {
		snd_err("codec clk_rst_dac hal_reset_control_get failed\n");
		goto err_get_clk_rst_dac;
	}
	clk->clk_rst_adc = hal_reset_control_get(HAL_SUNXI_AON_RESET, RST_CODEC_ADC);
	if (!clk->clk_rst_adc) {
		snd_err("codec clk_rst_adc hal_reset_control_get failed\n");
		goto err_get_clk_rst_adc;
	}
	clk->clk_bus_dac = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_BUS_CODEC_DAC);
	if (!clk->clk_bus_dac) {
		snd_err("codec clk_bus_dac hal_clock_get failed\n");
		goto err_get_clk_bus_dac;
	}
	clk->clk_bus_adc = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_BUS_CODEC_ADC);
	if (!clk->clk_bus_adc) {
		snd_err("codec clk_bus_adc hal_clock_get failed\n");
		goto err_get_clk_bus_adc;
	}

	/* pll clk -> 24.576M */
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

	/* pll clk -> 22.5792M */
	clk->clk_ck1_aud_div = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_CK1_AUD_DIV);
	if (!clk->clk_ck1_aud_div) {
		snd_err("codec clk_ck1_aud_div hal_clock_get failed\n");
		goto err_get_clk_ck1_aud_div;
	}

	/* pll clk -> 8.192M */
	clk->clk_aud_rco_div = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_AUD_RCO_DIV);
	if (!clk->clk_aud_rco_div) {
		snd_err("codec clk_aud_rco_div hal_clock_get failed\n");
		goto err_get_clk_aud_rco_div;
	}

	/* module dac clk */
	clk->clk_dac = hal_clock_get(HAL_SUNXI_AON_CCU, CLK_CODEC_DAC);
	if (!clk->clk_dac) {
		snd_err("codec clk_dac hal_clock_get failed\n");
		goto err_get_clk_dac;
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

	/* pll tree: (24.576MHz)
	 * CLK_PLL_AUDIO --> CLK_PLL_AUDIO1X --> CLK_AUDPLL_HOSC_SEL --> CLK_CODEC_DAC
	 * CLK_PLL_AUDIO --> CLK_PLL_AUDIO1X --> CLK_AUDPLL_HOSC_SEL --> CLK_CODEC_ADC_DIV --> CLK_CODEC_ADC_SEL1 --> CLK_CODEC_ADC_GATE
	 *
	 * pll tree: (22.5792MHz)
	 * CLK_DPLL1 --> CLK_CK1_AUD --> CLK_CK1_AUD_DIV --> CLK_CODEC_DAC
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

	ret = hal_clk_set_parent(clk->clk_dac, clk->clk_audpll_hosc_sel);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("codec clk_audpll_hosc_sel -> clk_dac clk_set_parent failed.\n");
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
	hal_clock_put(clk->clk_dac);
err_get_clk_dac:
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
	hal_clock_put(clk->clk_bus_dac);
err_get_clk_bus_dac:
	hal_reset_control_put(clk->clk_rst_adc);
err_get_clk_rst_adc:
	hal_reset_control_put(clk->clk_rst_dac);
err_get_clk_rst_dac:
	return HAL_CLK_STATUS_ERROR;
}

static void snd_sunxi_clk_exit(struct sunxi_codec_clk *clk)
{
	snd_print("\n");

	snd_sunxi_clk_disable(clk);
	hal_clock_put(clk->clk_adc_gate);
	hal_clock_put(clk->clk_adc_sel1);
	hal_clock_put(clk->clk_adc_div);
	hal_clock_put(clk->clk_dac);
	hal_clock_put(clk->clk_aud_rco_div);
	hal_clock_put(clk->clk_ck1_aud_div);
	hal_clock_put(clk->clk_audpll_hosc_sel);
	hal_clock_put(clk->clk_hosc);
	hal_clock_put(clk->clk_pll_audio2x);
	hal_clock_put(clk->clk_pll_audio1x);
	hal_clock_put(clk->clk_bus_adc);
	hal_clock_put(clk->clk_bus_dac);
	hal_reset_control_put(clk->clk_rst_adc);
	hal_reset_control_put(clk->clk_rst_dac);

	return;
}

static int snd_sunxi_clk_set_rate(struct sunxi_codec_clk *clk, int stream,
				  unsigned int freq_in, unsigned int freq_out)
{
	int ret;
	unsigned int freq;
	hal_clk_t clk_codec;

	snd_print("freq_out -> %u\n", freq_out);

	(void)freq_in;

	if (stream == SNDRV_PCM_STREAM_PLAYBACK)
		clk_codec = clk->clk_dac;
	else
		clk_codec = clk->clk_adc_div;

	if (freq_out == 24576000) {
		freq = 24583333;
		ret = hal_clk_set_parent(clk_codec, clk->clk_audpll_hosc_sel);
		if (ret != HAL_CLK_STATUS_OK) {
			snd_err("codec clk_audpll_hosc_sel -> clk_codec clk_set_parent failed.\n");
			return HAL_CLK_STATUS_ERROR;
		}
	} else {
		freq = 22588236;
		ret = hal_clk_set_parent(clk_codec, clk->clk_ck1_aud_div);
		if (ret != HAL_CLK_STATUS_OK) {
			snd_err("codec clk_ck1_aud_div -> clk_codec clk_set_parent failed.\n");
			return HAL_CLK_STATUS_ERROR;
		}
	}

	/* note: CLK_CODEC_DAC no need set rate */
	if (stream != SNDRV_PCM_STREAM_PLAYBACK) {
		ret = hal_clk_set_rate(clk_codec, freq);
		if (ret != HAL_CLK_STATUS_OK) {
			snd_err("set clk_codec rate %u failed\n", freq);
			return HAL_CLK_STATUS_ERROR;
		}
	}

	return HAL_CLK_STATUS_OK;
}

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

	sunxi_codec->param = default_param;
	codec->codec_base_addr = (void *)SUNXI_CODEC_BASE_ADDR;
	codec->codec_dai->component = codec;

	ret = snd_sunxi_clk_init(&sunxi_codec->clk);
	if (ret != 0) {
		snd_err("snd_sunxi_clk_init failed\n");
		goto err_codec_set_clock;
	}

	sunxi_codec_init(codec);

	return 0;

err_codec_set_clock:
	snd_sunxi_clk_exit(&sunxi_codec->clk);

	return -1;
}

static int sun20iw2_codec_remove(struct snd_codec *codec)
{
	struct sunxi_codec_info *sunxi_codec = codec->private_data;

	snd_print("\n");

	snd_sunxi_clk_exit(&sunxi_codec->clk);

	snd_free(sunxi_codec);
	codec->private_data = NULL;

	return 0;
}

/*
 * playback: period_bytes=960*(16*2/8)=3840, buffer_bytes=3840*4=15360
 * capture: period_bytes=320*(16*3/8)=1920, buffer_bytes=1920*4=7680
 */
static struct snd_pcm_hardware sun20iw2_hardware[2] = {
	{	/* SNDRV_PCM_STREAM_PLAYBACK */
		.info			= SNDRV_PCM_INFO_INTERLEAVED
					| SNDRV_PCM_INFO_BLOCK_TRANSFER
					| SNDRV_PCM_INFO_MMAP
					| SNDRV_PCM_INFO_MMAP_VALID
					| SNDRV_PCM_INFO_PAUSE
					| SNDRV_PCM_INFO_RESUME,
		.buffer_bytes_max	= 15360,
		.period_bytes_min	= 256,
		.period_bytes_max	= 3840,
		.periods_min		= 2,
		.periods_max		= 4,
	},
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

struct snd_codec sunxi_audiocodec = {
	.name		= "audiocodec",
	.codec_dai	= sun20iw2_codec_dai,
	.codec_dai_num  = ARRAY_SIZE(sun20iw2_codec_dai),
	.private_data	= NULL,
	.probe		= sun20iw2_codec_probe,
	.remove		= sun20iw2_codec_remove,
	.controls       = sunxi_codec_controls,
	.num_controls   = ARRAY_SIZE(sunxi_codec_controls),
	.hw 		= sun20iw2_hardware,
};
