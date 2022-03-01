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
#include <hal_timer.h>
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
#include <sunxi_hal_timer.h>
/* #include "hal_clk.h" */

#include "sunxi-codec.h"
#include "sun8iw20-codec.h"

struct snd_codec sunxi_audiocodec;

enum stream_type {
	PLAYBACK,
	CAPTURE,
};

static struct sunxi_codec_param default_param = {
	.digital_vol	= 0x0,
	.lineout_vol	= 0x0,
	.mic1gain	= 0x0,
	.mic2gain	= 0x0,
	.mic3gain	= 0x0,
	.lineingain	= 0x0,
	.gpio_spk	= GPIOB(7),
	.pa_msleep_time = 160,
	.pa_level	= GPIO_DATA_HIGH,
	.adcdrc_cfg     = 0,
	.adchpf_cfg     = 1,
	.dacdrc_cfg     = 0,
	.dachpf_cfg     = 0,
};

static const struct sample_rate sample_rate_conv[] = {
	{44100, 0},
	{48000, 0},
	{8000, 5},
	{32000, 1},
	{22050, 2},
	{24000, 2},
	{16000, 3},
	{11025, 4},
	{12000, 4},
	{192000, 6},
	{96000, 7},
	{88200, 7}, /* audio spec do not supply 88.2k */
};

/* get chip version */
static enum SUNXI_CHIP_VER {
	CHIP_VER_A = 0,
	CHIP_VER_B,
	CHIP_VER_C,
	CHIP_VER_XX,
} g_chip_ver;

static void sunxi_get_chip_ver(void)
{
	u32 version_reg;

	version_reg = hal_readl(0x03000024) & 0x3;

	if (version_reg == 0x0)
		g_chip_ver = CHIP_VER_A;
	else if (version_reg == 0x1)
		g_chip_ver =  CHIP_VER_B;
	else if (version_reg == 0x2)
		g_chip_ver =  CHIP_VER_C;
	else {
		g_chip_ver =  CHIP_VER_XX;
		snd_err("chip version other.\n");
	}
}

static int sunxi_get_adc_ch(struct snd_codec *codec)
{
	int ret = -1;
	uint32_t reg_val;
	struct sunxi_codec_info *sunxi_codec = codec->private_data;
	struct sunxi_codec_param *codec_param = &sunxi_codec->param;

	reg_val = snd_codec_read(codec, SUNXI_ADC1_ANA_CTL);
	if ((reg_val >> MIC_PGA_EN) & 0x01 || \
	    (reg_val >> FMINL_EN) & 0x01 || \
	    (reg_val >> LIENINL_EN) & 0x01) {
		codec_param->adc1_flag = true;
		ret = 1;
	}
	else {
		codec_param->adc1_flag = false;
	}

	reg_val = snd_codec_read(codec, SUNXI_ADC2_ANA_CTL);
	if ((reg_val >> MIC_PGA_EN) & 0x01 || \
	    (reg_val >> FMINR_EN) & 0x01 || \
	    (reg_val >> LIENINR_EN) & 0x01) {
		codec_param->adc2_flag = true;
		ret = 1;
	}
	else {
		codec_param->adc2_flag = false;
	}

	reg_val = snd_codec_read(codec, SUNXI_ADC3_ANA_CTL);
	if ((reg_val >> MIC_PGA_EN) & 0x01) {
		codec_param->adc3_flag = true;
		ret = 1;
	}
	else {
		codec_param->adc3_flag = false;
	}

	return ret;
}

#ifdef SUNXI_CODEC_DAP_ENABLE
static void adcdrc_config(struct snd_codec *codec)
{
	/* Left peak filter attack time */
	snd_codec_write(codec, SUNXI_ADC_DRC_LPFHAT, (0x000B77BF >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_LPFLAT, 0x000B77BF & 0xFFFF);
	/* Right peak filter attack time */
	snd_codec_write(codec, SUNXI_ADC_DRC_RPFHAT, (0x000B77BF >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_RPFLAT, 0x000B77BF & 0xFFFF);
	/* Left peak filter release time */
	snd_codec_write(codec, SUNXI_ADC_DRC_LPFHRT, (0x00FFE1F8 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_LPFLRT, 0x00FFE1F8 & 0xFFFF);
	/* Right peak filter release time */
	snd_codec_write(codec, SUNXI_ADC_DRC_RPFHRT, (0x00FFE1F8 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_RPFLRT, 0x00FFE1F8 & 0xFFFF);

	/* Left RMS filter attack time */
	snd_codec_write(codec, SUNXI_ADC_DRC_LPFHAT, (0x00012BAF >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_LPFLAT, 0x00012BAF & 0xFFFF);
	/* Right RMS filter attack time */
	snd_codec_write(codec, SUNXI_ADC_DRC_RPFHAT, (0x00012BAF >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_RPFLAT, 0x00012BAF & 0xFFFF);

	/* smooth filter attack time */
	snd_codec_write(codec, SUNXI_ADC_DRC_SFHAT, (0x00017665 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_SFLAT, 0x00017665 & 0xFFFF);
	/* gain smooth filter release time */
	snd_codec_write(codec, SUNXI_ADC_DRC_SFHRT, (0x00000F04 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_SFLRT, 0x00000F04 & 0xFFFF);

	/* OPL */
	snd_codec_write(codec, SUNXI_ADC_DRC_HOPL, (0xFBD8FBA7 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_LOPL, 0xFBD8FBA7 & 0xFFFF);
	/* OPC */
	snd_codec_write(codec, SUNXI_ADC_DRC_HOPC, (0xF95B2C3F >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_LOPC, 0xF95B2C3F & 0xFFFF);
	/* OPE */
	snd_codec_write(codec, SUNXI_ADC_DRC_HOPE, (0xF45F8D6E >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_LOPE, 0xF45F8D6E & 0xFFFF);
	/* LT */
	snd_codec_write(codec, SUNXI_ADC_DRC_HLT, (0x01A934F0 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_LLT, 0x01A934F0 & 0xFFFF);
	/* CT */
	snd_codec_write(codec, SUNXI_ADC_DRC_HCT, (0x06A4D3C0 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_LCT, 0x06A4D3C0 & 0xFFFF);
	/* ET */
	snd_codec_write(codec, SUNXI_ADC_DRC_HET, (0x0BA07291 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_LET, 0x0BA07291 & 0xFFFF);
	/* Ki */
	snd_codec_write(codec, SUNXI_ADC_DRC_HKI, (0x00051EB8 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_LKI, 0x00051EB8 & 0xFFFF);
	/* Kc */
	snd_codec_write(codec, SUNXI_ADC_DRC_HKC, (0x00800000 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_LKC, 0x00800000 & 0xFFFF);
	/* Kn */
	snd_codec_write(codec, SUNXI_ADC_DRC_HKN, (0x01000000 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_LKN, 0x01000000 & 0xFFFF);
	/* Ke */
	snd_codec_write(codec, SUNXI_ADC_DRC_HKE, (0x0000F45F >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_LKE, 0x0000F45F & 0xFFFF);
}

static void adcdrc_enable(struct snd_codec *codec, bool on)
{
	if (on) {
		snd_codec_update_bits(codec, SUNXI_ADC_DAP_CTL,
			(0x1 << ADC_DRC0_EN | 0x1 << ADC_DRC1_EN),
			(0x1 << ADC_DRC0_EN | 0x1 << ADC_DRC1_EN));
	} else {
		snd_codec_update_bits(codec, SUNXI_ADC_DAP_CTL,
			(0x1 << ADC_DRC0_EN | 0x1 << ADC_DRC1_EN),
			(0x0 << ADC_DRC0_EN | 0x0 << ADC_DRC1_EN));
	}
}

static void adchpf_config(struct snd_codec *codec)
{
	/* HPF */
	snd_codec_write(codec, SUNXI_ADC_DRC_HHPFC, (0xFFFAC1 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_ADC_DRC_LHPFC, 0xFFFAC1 & 0xFFFF);
}

static void adchpf_enable(struct snd_codec *codec, bool on)
{
	if (on) {
		snd_codec_update_bits(codec, SUNXI_ADC_DAP_CTL,
			(0x1 << ADC_HPF0_EN | 0x1 << ADC_HPF1_EN),
			(0x1 << ADC_HPF0_EN | 0x1 << ADC_HPF1_EN));
	} else {
		snd_codec_update_bits(codec, SUNXI_ADC_DAP_CTL,
			(0x1 << ADC_HPF0_EN | 0x1 << ADC_HPF1_EN),
			(0x0 << ADC_HPF0_EN | 0x0 << ADC_HPF1_EN));
	}
}

static void dacdrc_config(struct snd_codec *codec)
{
	/* Left peak filter attack time */
	snd_codec_write(codec, SUNXI_DAC_DRC_LPFHAT, (0x000B77BF >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_LPFLAT, 0x000B77BF & 0xFFFF);
	/* Right peak filter attack time */
	snd_codec_write(codec, SUNXI_DAC_DRC_RPFHAT, (0x000B77F0 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_RPFLAT, 0x000B77F0 & 0xFFFF);

	/* Left peak filter release time */
	snd_codec_write(codec, SUNXI_DAC_DRC_LPFHRT, (0x00FFE1F8 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_LPFLRT, 0x00FFE1F8 & 0xFFFF);
	/* Right peak filter release time */
	snd_codec_write(codec, SUNXI_DAC_DRC_RPFHRT, (0x00FFE1F8 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_RPFLRT, 0x00FFE1F8 & 0xFFFF);

	/* Left RMS filter attack time */
	snd_codec_write(codec, SUNXI_DAC_DRC_LRMSHAT, (0x00012BB0 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_LRMSLAT, 0x00012BB0 & 0xFFFF);
	/* Right RMS filter attack time */
	snd_codec_write(codec, SUNXI_DAC_DRC_RRMSHAT, (0x00012BB0 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_RRMSLAT, 0x00012BB0 & 0xFFFF);

	/* smooth filter attack time */
	snd_codec_write(codec, SUNXI_DAC_DRC_SFHAT, (0x00017665 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_SFLAT, 0x00017665 & 0xFFFF);
	/* gain smooth filter release time */
	snd_codec_write(codec, SUNXI_DAC_DRC_SFHRT, (0x00000F04 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_SFLRT, 0x00000F04 & 0xFFFF);

	/* OPL */
	snd_codec_write(codec, SUNXI_DAC_DRC_HOPL, (0xFF641741 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_LOPL, 0xFF641741 & 0xFFFF);
	/* OPC */
	snd_codec_write(codec, SUNXI_DAC_DRC_HOPC, (0xF9E8E88C >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_LOPC, 0xF9E8E88C & 0xFFFF);
	/* OPE */
	snd_codec_write(codec, SUNXI_DAC_DRC_HOPE, (0xF5DE3D14 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_LOPE, 0xF5DE3D14 & 0xFFFF);
	/* LT */
	snd_codec_write(codec, SUNXI_DAC_DRC_HLT, (0x0336110B >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_LLT, 0x0336110B & 0xFFFF);
	/* CT */
	snd_codec_write(codec, SUNXI_DAC_DRC_HCT, (0x08BF6C28 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_LCT, 0x08BF6C28 & 0xFFFF);
	/* ET */
	snd_codec_write(codec, SUNXI_DAC_DRC_HET, (0x0C9F9255 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_LET, 0x0C9F9255 & 0xFFFF);
	/* Ki */
	snd_codec_write(codec, SUNXI_DAC_DRC_HKI, (0x001A7B96 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_LKI, 0x001A7B96 & 0xFFFF);
	/* Kc */
	snd_codec_write(codec, SUNXI_DAC_DRC_HKC, (0x00FD70A5 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_LKC, 0x00FD70A5 & 0xFFFF);
	/* Kn */
	snd_codec_write(codec, SUNXI_DAC_DRC_HKN, (0x010AF8B0 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_LKN, 0x010AF8B0 & 0xFFFF);
	/* Ke */
	snd_codec_write(codec, SUNXI_DAC_DRC_HKE, (0x06286BA0 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_LKE, 0x06286BA0 & 0xFFFF);
	/* MXG */
	snd_codec_write(codec, SUNXI_DAC_DRC_MXGHS, (0x035269E0 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_MXGLS, 0x035269E0 & 0xFFFF);
	/* MNG */
	snd_codec_write(codec, SUNXI_DAC_DRC_MNGHS, (0xF95B2C3F >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_MNGLS, 0xF95B2C3F & 0xFFFF);
	/* EPS */
	snd_codec_write(codec, SUNXI_DAC_DRC_EPSHC, (0x00025600 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_EPSLC, 0x00025600 & 0xFFFF);
}

static void dacdrc_enable(struct snd_codec *codec, bool on)
{
	if (on) {
		/* detect noise when ET enable */
		snd_codec_update_bits(codec, SUNXI_DAC_DRC_CTRL,
			(0x1 << DAC_DRC_NOISE_DET_EN),
			(0x1 << DAC_DRC_NOISE_DET_EN));

		/* 0x0:RMS filter; 0x1:Peak filter */
		snd_codec_update_bits(codec, SUNXI_DAC_DRC_CTRL,
			(0x1 << DAC_DRC_SIGNAL_SEL),
			(0x1 << DAC_DRC_SIGNAL_SEL));

		snd_codec_update_bits(codec, SUNXI_DAC_DRC_CTRL,
			(0x1 << DAC_DRC_GAIN_MAX_EN),
			(0x1 << DAC_DRC_GAIN_MAX_EN));

		snd_codec_update_bits(codec, SUNXI_DAC_DRC_CTRL,
			(0x1 << DAC_DRC_GAIN_MIN_EN),
			(0x1 << DAC_DRC_GAIN_MIN_EN));

		/* delay function enable */
		snd_codec_update_bits(codec, SUNXI_DAC_DRC_CTRL,
			(0x1 << DAC_DRC_DELAY_BUF_EN),
			(0x1 << DAC_DRC_DELAY_BUF_EN));

		snd_codec_update_bits(codec, SUNXI_DAC_DRC_CTRL,
			(0x1 << DAC_DRC_LT_EN),
			(0x1 << DAC_DRC_LT_EN));
		snd_codec_update_bits(codec, SUNXI_DAC_DRC_CTRL,
			(0x1 << DAC_DRC_ET_EN),
			(0x1 << DAC_DRC_ET_EN));

		snd_codec_update_bits(codec, SUNXI_DAC_DAP_CTL,
			(0x1 << DDAP_DRC_EN),
			(0x1 << DDAP_DRC_EN));
	} else {
		snd_codec_update_bits(codec, SUNXI_DAC_DAP_CTL,
			(0x1 << DDAP_DRC_EN),
			(0x0 << DDAP_DRC_EN));

		/* detect noise when ET enable */
		snd_codec_update_bits(codec, SUNXI_DAC_DRC_CTRL,
			(0x1 << DAC_DRC_NOISE_DET_EN),
			(0x0 << DAC_DRC_NOISE_DET_EN));

		/* 0x0:RMS filter; 0x1:Peak filter */
		snd_codec_update_bits(codec, SUNXI_DAC_DRC_CTRL,
			(0x1 << DAC_DRC_SIGNAL_SEL),
			(0x0 << DAC_DRC_SIGNAL_SEL));

		/* delay function enable */
		snd_codec_update_bits(codec, SUNXI_DAC_DRC_CTRL,
			(0x1 << DAC_DRC_DELAY_BUF_EN),
			(0x0 << DAC_DRC_DELAY_BUF_EN));

		snd_codec_update_bits(codec, SUNXI_DAC_DRC_CTRL,
			(0x1 << DAC_DRC_GAIN_MAX_EN),
			(0x0 << DAC_DRC_GAIN_MAX_EN));
		snd_codec_update_bits(codec, SUNXI_DAC_DRC_CTRL,
			(0x1 << DAC_DRC_GAIN_MIN_EN),
			(0x0 << DAC_DRC_GAIN_MIN_EN));

		snd_codec_update_bits(codec, SUNXI_DAC_DRC_CTRL,
			(0x1 << DAC_DRC_LT_EN),
			(0x0 << DAC_DRC_LT_EN));
		snd_codec_update_bits(codec, SUNXI_DAC_DRC_CTRL,
			(0x1 << DAC_DRC_ET_EN),
			(0x0 << DAC_DRC_ET_EN));
	}
}

static void dachpf_config(struct snd_codec *codec)
{
	/* HPF */
	snd_codec_write(codec, SUNXI_DAC_DRC_HHPFC, (0xFFFAC1 >> 16) & 0xFFFF);
	snd_codec_write(codec, SUNXI_DAC_DRC_LHPFC, 0xFFFAC1 & 0xFFFF);
}

static void dachpf_enable(struct snd_codec *codec, bool on)
{
	if (on) {
		snd_codec_update_bits(codec, SUNXI_DAC_DAP_CTL,
			(0x1 << DDAP_HPF_EN),
			(0x1 << DDAP_HPF_EN));
	} else {
		snd_codec_update_bits(codec, SUNXI_DAC_DAP_CTL,
			(0x1 << DDAP_HPF_EN),
			(0x0 << DDAP_HPF_EN));
	}
}
#endif

static const char * const codec_format_function[] = {
			"hub_disable", "hub_enable"};

static const char * const micin_select[] = {
			"MIC_DIFFER", "MIC_SINGLE"};

static const char * const codec_switch[] = {
			"Off", "On"};

static struct snd_kcontrol sunxi_codec_controls[] = {
	SND_CTL_ENUM((unsigned char*)"codec hub mode",
			ARRAY_SIZE(codec_format_function), codec_format_function,
			SUNXI_DAC_DPC, DAC_HUB_EN),

	/* global digital volume */
	SND_CTL_KCONTROL((unsigned char*)"digital volume",
			SUNXI_DAC_DPC, DVOL, 0x3F),

	/* DAC digital volume */
	SND_CTL_ENUM((unsigned char*)"DAC digital volume switch",
			ARRAY_SIZE(codec_switch), codec_switch,
			SUNXI_DAC_VOL_CTL, DAC_VOL_SEL),
	SND_CTL_KCONTROL((unsigned char*)"DACL digital volume",
			SUNXI_DAC_VOL_CTL, DAC_VOL_L, 0xFF),
	SND_CTL_KCONTROL((unsigned char*)"DACR digital volume",
			SUNXI_DAC_VOL_CTL, DAC_VOL_R, 0xFF),

	/* analog volume */
	SND_CTL_KCONTROL((unsigned char*)"MIC1 gain volume",
			SUNXI_ADC1_ANA_CTL, ADC_PGA_GAIN_CTL, 0x1F),
	SND_CTL_KCONTROL((unsigned char*)"MIC2 gain volume",
			SUNXI_ADC2_ANA_CTL, ADC_PGA_GAIN_CTL, 0x1F),
	SND_CTL_KCONTROL((unsigned char*)"MIC3 gain volume",
			SUNXI_ADC3_ANA_CTL, ADC_PGA_GAIN_CTL, 0x1F),
	SND_CTL_KCONTROL((unsigned char*)"FM Left gain volume",
			SUNXI_ADC1_ANA_CTL, FMINL_GAIN, 0x1),
	SND_CTL_KCONTROL((unsigned char*)"FM Right gain volume",
			SUNXI_ADC2_ANA_CTL, FMINR_GAIN, 0x1),
	SND_CTL_KCONTROL((unsigned char*)"Line Right gain volume",
			SUNXI_ADC2_ANA_CTL, LIENINL_GAIN, 0x1),
	SND_CTL_KCONTROL((unsigned char*)"Line Left gain volume",
			SUNXI_ADC2_ANA_CTL, LIENINR_GAIN, 0x1),
	SND_CTL_KCONTROL((unsigned char*)"HPOUT gain volume",
			SUNXI_HP_ANA_CTL, HP_GAIN, 0x7),

	/* ADC digital volume */
	SND_CTL_ENUM((unsigned char*)"ADC1_2 digital volume switch",
			ARRAY_SIZE(codec_switch), codec_switch,
			SUNXI_ADC_DIG_CTL, ADC1_2_VOL_EN),
	SND_CTL_ENUM((unsigned char*)"ADC3 digital volume switch",
			ARRAY_SIZE(codec_switch), codec_switch,
			SUNXI_ADC_DIG_CTL, ADC3_VOL_EN),
	SND_CTL_KCONTROL((unsigned char*)"ADC1 digital volume",
			SUNXI_ADC_VOL_CTL1, ADC1_VOL, 0xFF),
	SND_CTL_KCONTROL((unsigned char*)"ADC2 digital volume",
			SUNXI_ADC_VOL_CTL1, ADC2_VOL, 0xFF),
	SND_CTL_KCONTROL((unsigned char*)"ADC3 digital volume",
			SUNXI_ADC_VOL_CTL1, ADC3_VOL, 0xFF),

	/* mic input select */
	SND_CTL_ENUM((unsigned char*)"MIC1 input select",
			ARRAY_SIZE(micin_select), micin_select,
			SUNXI_ADC1_ANA_CTL, MIC_SIN_EN),
	SND_CTL_ENUM((unsigned char*)"MIC2 input select",
			ARRAY_SIZE(micin_select), micin_select,
			SUNXI_ADC2_ANA_CTL, MIC_SIN_EN),
	SND_CTL_ENUM((unsigned char*)"MIC3 input select",
			ARRAY_SIZE(micin_select), micin_select,
			SUNXI_ADC3_ANA_CTL, MIC_SIN_EN),
	/* ADC1 */
	SND_CTL_ENUM((unsigned char*)"MIC1 input switch",
			ARRAY_SIZE(codec_switch), codec_switch,
			SUNXI_ADC1_ANA_CTL, MIC_PGA_EN),
	SND_CTL_ENUM((unsigned char*)"FM input L switch",
			ARRAY_SIZE(codec_switch), codec_switch,
			SUNXI_ADC1_ANA_CTL, FMINL_EN),
	SND_CTL_ENUM((unsigned char*)"LINE input L switch",
			ARRAY_SIZE(codec_switch), codec_switch,
			SUNXI_ADC1_ANA_CTL, LIENINL_EN),
	/* ADC2 */
	SND_CTL_ENUM((unsigned char*)"MIC2 input switch",
			ARRAY_SIZE(codec_switch), codec_switch,
			SUNXI_ADC2_ANA_CTL, MIC_PGA_EN),
	SND_CTL_ENUM((unsigned char*)"FM input R switch",
			ARRAY_SIZE(codec_switch), codec_switch,
			SUNXI_ADC2_ANA_CTL, FMINR_EN),
	SND_CTL_ENUM((unsigned char*)"LINE input R switch",
			ARRAY_SIZE(codec_switch), codec_switch,
			SUNXI_ADC2_ANA_CTL, LIENINR_EN),
	/* ADC3 */
	SND_CTL_ENUM((unsigned char*)"MIC3 input switch",
			ARRAY_SIZE(codec_switch), codec_switch,
			SUNXI_ADC3_ANA_CTL, MIC_PGA_EN),
};

static void sunxi_codec_init(struct snd_codec *codec)
{
	struct sunxi_codec_info *sunxi_codec = codec->private_data;
	struct sunxi_codec_param *param = &sunxi_codec->param;

	/* get chp version */
	sunxi_get_chip_ver();

	/* In order to ensure that the ADC sampling is normal,
	 * the A chip SOC needs to always open HPLDO and RMC_EN
	 */
	if (g_chip_ver == CHIP_VER_A) {
		snd_codec_update_bits(codec, SUNXI_POWER_ANA_CTL,
			(0x1<<HPLDO_EN), (0x1<<HPLDO_EN));
		snd_codec_update_bits(codec, SUNXI_RAMP_ANA_CTL,
			(0x1<<RMCEN), (0x1<<RMCEN));
	} else {
		snd_codec_update_bits(codec, SUNXI_POWER_ANA_CTL,
			(0x1<<HPLDO_EN), (0x0<<HPLDO_EN));
		snd_codec_update_bits(codec, SUNXI_RAMP_ANA_CTL,
			(0x1<<RMCEN), (0x0<<RMCEN));
	}

	/* Disable HPF(high passed filter) */
	snd_codec_update_bits(codec, SUNXI_DAC_DPC,
			(1 << HPF_EN), (0x0 << HPF_EN));

	/* Enable ADCFDT to overcome niose at the beginning */
	snd_codec_update_bits(codec, SUNXI_ADC_FIFOC,
			(0x7 << ADCDFEN), (0x7 << ADCDFEN));

	/* init the mic pga and vol params */
	snd_codec_update_bits(codec, SUNXI_DAC_ANA_CTL,
			0x1F << LINEOUT_VOL,
			param->lineout_vol << LINEOUT_VOL);

	/* DAC_VOL_SEL enable */
	snd_codec_update_bits(codec, SUNXI_DAC_VOL_CTL,
			(0x1 << DAC_VOL_SEL), (0x1 << DAC_VOL_SEL));
	snd_codec_update_bits(codec, SUNXI_DAC_DPC,
			0x3F << DVOL,
			param->digital_vol << DVOL);

	snd_codec_update_bits(codec, SUNXI_ADC1_ANA_CTL,
			0x1F << ADC_PGA_GAIN_CTL,
			param->mic1gain << ADC_PGA_GAIN_CTL);
	snd_codec_update_bits(codec, SUNXI_ADC2_ANA_CTL,
			0x1F << ADC_PGA_GAIN_CTL,
			param->mic2gain << ADC_PGA_GAIN_CTL);
	snd_codec_update_bits(codec, SUNXI_ADC3_ANA_CTL,
			0x1F << ADC_PGA_GAIN_CTL,
			param->mic3gain << ADC_PGA_GAIN_CTL);
#ifdef SUNXI_CODEC_DAP_ENABLE
	if (param->dacdrc_cfg || param->dachpf_cfg) {
		snd_codec_update_bits(codec, SUNXI_DAC_DAP_CTL,
				(0x1 << DDAP_EN), (0x1 << DDAP_EN));
	}

	if (param->adcdrc_cfg || param->adchpf_cfg) {
		snd_codec_update_bits(codec, SUNXI_ADC_DAP_CTL,
				(0x1 << ADC_DAP0_EN | 0x1 << ADC_DAP1_EN),
				(0x1 << ADC_DAP0_EN | 0x1 << ADC_DAP1_EN));
	}

	if (param->adcdrc_cfg) {
		adcdrc_config(codec);
		adcdrc_enable(codec, 1);
	}
	if (param->adchpf_cfg) {
		adchpf_config(codec);
		adchpf_enable(codec, 1);
	}
	if (param->dacdrc_cfg) {
		dacdrc_config(codec);
		dacdrc_enable(codec, 1);
	}
	if (param->dachpf_cfg) {
		dachpf_config(codec);
		dachpf_enable(codec, 1);
	}
#endif
}

static int sunxi_codec_dapm_control(struct snd_pcm_substream *substream,
				struct snd_dai *dai, int onoff)
{
	struct snd_codec *codec = dai->component;
	struct sunxi_codec_info *sunxi_codec = codec->private_data;
	struct sunxi_codec_param *param = &sunxi_codec->param;

	if (substream->dapm_state == onoff)
		return 0;
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		/*
		 * Playback:
		 * Playback --> DAC --> DAC_DIFFER --> HPOUT --> External Speaker
		 */
		if (onoff) {
			if (g_chip_ver == CHIP_VER_A) {
			snd_codec_update_bits(codec, SUNXI_HP_ANA_CTL,
					(0x1<<HPFB_BUF_EN) | (0x1<<HPFB_IN_EN),
					(0x1<<HPFB_BUF_EN) | (0x1<<HPFB_IN_EN));
			/* ramp out enable */
			snd_codec_update_bits(codec, SUNXI_HP_ANA_CTL,
					(0x1<<RAMP_OUT_EN) | (0x1<<RSWITCH),
					(0x1<<RAMP_OUT_EN) | (0x1<<RSWITCH));
			/* digital DAC enable */
			snd_codec_update_bits(codec, SUNXI_DAC_DPC,
					(0x1<<EN_DAC), (0x1<<EN_DAC));
			/* analog DAC enable */
			snd_codec_update_bits(codec, SUNXI_DAC_ANA_CTL,
					(0x1<<DACLEN) | (0x1<<DACREN),
					(0x1<<DACLEN) | (0x1<<DACREN));
			/* HEADPONEOUT */
			snd_codec_update_bits(codec, SUNXI_HP_ANA_CTL,
				(0x1<<HP_DRVEN) | (0x1<<HP_DRVOUTEN),
				(0x1<<HP_DRVEN) | (0x1<<HP_DRVOUTEN));
			/* Playback on */
			snd_codec_update_bits(codec, SUNXI_POWER_ANA_CTL,
					(0x1<<HPLDO_EN), (0x1<<HPLDO_EN));
			} else {
			/* digital DAC enable */
			snd_codec_update_bits(codec, SUNXI_DAC_DPC,
					(0x1<<EN_DAC), (0x1<<EN_DAC));
			snd_codec_update_bits(codec, SUNXI_RAMP_ANA_CTL,
					(0x1<<RDEN), (0x1<<RDEN));
			}
			if (param->gpio_spk > 0) {
				hal_gpio_set_direction(param->gpio_spk, GPIO_MUXSEL_OUT);
				hal_gpio_set_data(param->gpio_spk, param->pa_level);
				hal_msleep(param->pa_msleep_time);
			}
		} else {
			/* Playback off */
			if (param->gpio_spk > 0) {
				hal_gpio_set_direction(param->gpio_spk, GPIO_MUXSEL_OUT);
				hal_gpio_set_data(param->gpio_spk, !param->pa_level);
			}
			if (g_chip_ver == CHIP_VER_A) {
			/* HEADPONE_EN */
			snd_codec_update_bits(codec, SUNXI_HP_ANA_CTL,
					(0x1<<HP_DRVEN), (0x0<<HP_DRVEN));
			/* power off */
			/* note: HPLDO is always open to avoid KEY_ADC sampling fail */
			/* snd_codec_update_bits(codec, SUNXI_POWER_ANA_CTL, */
			/* 		(0x1<<HPLDO_EN), (0x0<<HPLDO_EN)); */
			hal_msleep(30);
			/* HEADPONE_OUT */
			snd_codec_update_bits(codec, SUNXI_HP_ANA_CTL,
					(0x1<<HP_DRVOUTEN), (0x0<<HP_DRVOUTEN));
			/* analog DAC */
			snd_codec_update_bits(codec, SUNXI_DAC_ANA_CTL,
					(0x1<<DACLEN) | (0x1<<DACREN),
					(0x0<<DACLEN) | (0x0<<DACREN));
			/* digital DAC */
			snd_codec_update_bits(codec, SUNXI_DAC_DPC,
					(0x1<<EN_DAC), (0x0<<EN_DAC));

			snd_codec_update_bits(codec, SUNXI_HP_ANA_CTL,
					(0x1<<RAMP_OUT_EN) | (0x1<<RSWITCH),
					(0x0<<RAMP_OUT_EN) | (0x0<<RSWITCH));

			snd_codec_update_bits(codec, SUNXI_HP_ANA_CTL,
					(0x1<<HPFB_BUF_EN) | (0x1<<HPFB_IN_EN),
					(0x0<<HPFB_BUF_EN) | (0x0<<HPFB_IN_EN));
			} else {
			/* digital DAC enable */
			snd_codec_update_bits(codec, SUNXI_DAC_DPC,
					(0x1<<EN_DAC), (0x0<<EN_DAC));
			snd_codec_update_bits(codec, SUNXI_RAMP_ANA_CTL,
					(0x1<<RDEN), (0x0<<RDEN));
			}
		}
	} else {
		/*
		 * Capture:
		 * Capture1 <-- ADC1 <-- Input Mixer <-- LINEINL PGA <-- LINEINL
		 * Capture1 <-- ADC1 <-- Input Mixer <-- FMINL PGA <-- FMINL
		 *
		 * Capture2 <-- ADC2 <-- Input Mixer <-- LINEINR PGA <-- LINEINR
		 * Capture2 <-- ADC2 <-- Input Mixer <-- FMINR PGA <-- FMINR
		 *
		 * Capture3 <-- ADC3 <-- Input Mixer <-- MIC3 PGA <-- MIC3
		 */
		unsigned int channels = 0;
		channels = substream->runtime->channels;

		snd_print("channels = %u\n", channels);
		if (onoff) {
			/* Capture on */
			hal_msleep(100);
			/* digital ADC enable */
			snd_codec_update_bits(codec, SUNXI_ADC_FIFOC,
					(0x1<<EN_AD), (0x1<<EN_AD));

			if (channels > 3 || channels < 1) {
				snd_err("unknown channels:%u\n", channels);
				return -EINVAL;
			}

			if (param->adc1_flag)
				snd_codec_update_bits(codec, SUNXI_ADC1_ANA_CTL,
						      0x1<<ADC_EN,
						      0x1<<ADC_EN);
			if (param->adc2_flag)
				snd_codec_update_bits(codec, SUNXI_ADC2_ANA_CTL,
						      0x1<<ADC_EN,
						      0x1<<ADC_EN);
			if (param->adc3_flag)
				snd_codec_update_bits(codec, SUNXI_ADC3_ANA_CTL,
						      0x1<<ADC_EN,
						      0x1<<ADC_EN);
		} else {
			/* Capture off */
			if (channels > 3 || channels < 1) {
				snd_err("unknown channels:%u\n", channels);
				return -EINVAL;
			}

			if (param->adc1_flag)
				snd_codec_update_bits(codec, SUNXI_ADC1_ANA_CTL,
						      0x1<<ADC_EN,
						      0x0<<ADC_EN);
			if (param->adc2_flag)
				snd_codec_update_bits(codec, SUNXI_ADC2_ANA_CTL,
						      0x1<<ADC_EN,
						      0x0<<ADC_EN);
			if (param->adc3_flag)
				snd_codec_update_bits(codec, SUNXI_ADC3_ANA_CTL,
						      0x1<<ADC_EN,
						      0x0<<ADC_EN);
			/* digital ADC enable */
			snd_codec_update_bits(codec, SUNXI_ADC_FIFOC,
					(0x1<<EN_AD), (0x0<<EN_AD));
		}
	}
	substream->dapm_state = onoff;
	return 0;
}

static int sunxi_codec_startup(struct snd_pcm_substream *substream,
		struct snd_dai *dai)
{
	struct snd_codec *codec = dai->component;
	struct sunxi_codec_info *sunxi_codec = codec->private_data;

	snd_print("\n");

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		sunxi_codec->stream_type = PLAYBACK;
	} else {
		sunxi_codec->stream_type = CAPTURE;
	}

	return 0;
}


static int sunxi_codec_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params, struct snd_dai *dai)
{
	struct snd_codec *codec = dai->component;
	struct sunxi_codec_info *sunxi_codec = codec->private_data;
	struct sunxi_codec_param *codec_param = &sunxi_codec->param;
	int i = 0;

	snd_print("\n");
	switch (params_format(params)) {
	case	SND_PCM_FORMAT_S16_LE:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			snd_codec_update_bits(codec, SUNXI_DAC_FIFOC,
				(3 << FIFO_MODE), (3 << FIFO_MODE));
			snd_codec_update_bits(codec, SUNXI_DAC_FIFOC,
				(1 << TX_SAMPLE_BITS), (0 << TX_SAMPLE_BITS));
		} else {
			snd_codec_update_bits(codec, SUNXI_ADC_FIFOC,
				(1 << RX_FIFO_MODE), (1 << RX_FIFO_MODE));
			snd_codec_update_bits(codec, SUNXI_ADC_FIFOC,
				(1 << RX_SAMPLE_BITS), (0 << RX_SAMPLE_BITS));
		}
		break;
	case	SND_PCM_FORMAT_S24_LE:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			snd_codec_update_bits(codec, SUNXI_DAC_FIFOC,
				(3 << FIFO_MODE), (0 << FIFO_MODE));
			snd_codec_update_bits(codec, SUNXI_DAC_FIFOC,
				(1 << TX_SAMPLE_BITS), (1 << TX_SAMPLE_BITS));
		} else {
			snd_codec_update_bits(codec, SUNXI_ADC_FIFOC,
				(1 << RX_FIFO_MODE), (0 << RX_FIFO_MODE));
			snd_codec_update_bits(codec, SUNXI_ADC_FIFOC,
				(1 << RX_SAMPLE_BITS), (1 << RX_SAMPLE_BITS));
		}
		break;
	default:
		break;
	}

	for (i = 0; i < ARRAY_SIZE(sample_rate_conv); i++) {
		if (sample_rate_conv[i].samplerate == params_rate(params)) {
			if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
				snd_codec_update_bits(codec, SUNXI_DAC_FIFOC,
					(0x7 << DAC_FS),
					(sample_rate_conv[i].rate_bit << DAC_FS));
			} else {
				if (sample_rate_conv[i].samplerate > 48000)
					return -EINVAL;
				snd_codec_update_bits(codec, SUNXI_ADC_FIFOC,
					(0x7 << ADC_FS),
					(sample_rate_conv[i].rate_bit<<ADC_FS));
			}
		}
	}

	/* reset the adchpf func setting for different sampling */
	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
		if (codec_param->adchpf_cfg) {
			if (params_rate(params) == 16000) {

				snd_codec_write(codec, SUNXI_ADC_DRC_HHPFC,
						(0x00F623A5 >> 16) & 0xFFFF);

				snd_codec_write(codec, SUNXI_ADC_DRC_LHPFC,
							0x00F623A5 & 0xFFFF);

			} else if (params_rate(params) == 44100) {

				snd_codec_write(codec, SUNXI_ADC_DRC_HHPFC,
						(0x00FC60DB >> 16) & 0xFFFF);

				snd_codec_write(codec, SUNXI_ADC_DRC_LHPFC,
							0x00FC60DB & 0xFFFF);
			} else {
				snd_codec_write(codec, SUNXI_ADC_DRC_HHPFC,
						(0x00FCABB3 >> 16) & 0xFFFF);

				snd_codec_write(codec, SUNXI_ADC_DRC_LHPFC,
							0x00FCABB3 & 0xFFFF);
			}
		}
	}

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		switch (params_channels(params)) {
		case 1:
			snd_codec_update_bits(codec, SUNXI_DAC_FIFOC,
					(1<<DAC_MONO_EN), 1<<DAC_MONO_EN);
			break;
		case 2:
			snd_codec_update_bits(codec, SUNXI_DAC_FIFOC,
					(1<<DAC_MONO_EN), (0<<DAC_MONO_EN));
			break;
		default:
			snd_err("cannot support the channels:%u.\n",
				params_channels(params));
			return -EINVAL;
		}
	} else {
		if (sunxi_get_adc_ch(codec) < 0) {
			snd_err("capture only support 1~3 channel\n");
			return -EINVAL;
		}
		if (codec_param->adc1_flag)
			snd_codec_update_bits(codec, SUNXI_ADC_DIG_CTL,
					      0x1<<ADC1_CHANNEL_EN,
					      0x1<<ADC1_CHANNEL_EN);
		else
			snd_codec_update_bits(codec, SUNXI_ADC_DIG_CTL,
					      0x1<<ADC1_CHANNEL_EN,
					      0x0<<ADC1_CHANNEL_EN);
		if (codec_param->adc2_flag)
			snd_codec_update_bits(codec, SUNXI_ADC_DIG_CTL,
					      0x1<<ADC2_CHANNEL_EN,
					      0x1<<ADC2_CHANNEL_EN);
		else
			snd_codec_update_bits(codec, SUNXI_ADC_DIG_CTL,
					      0x1<<ADC2_CHANNEL_EN,
					      0x0<<ADC2_CHANNEL_EN);
		if (codec_param->adc3_flag)
			snd_codec_update_bits(codec, SUNXI_ADC_DIG_CTL,
					      0x1<<ADC3_CHANNEL_EN,
					      0x1<<ADC3_CHANNEL_EN);
		else
			snd_codec_update_bits(codec, SUNXI_ADC_DIG_CTL,
					      0x1<<ADC3_CHANNEL_EN,
					      0x0<<ADC3_CHANNEL_EN);
	}

	return 0;
}

static int sunxi_codec_set_sysclk(struct snd_dai *dai,
		int clk_id, unsigned int freq, int dir)
{
	struct snd_codec *codec = dai->component;
	struct sunxi_codec_info *sunxi_codec = codec->private_data;

	snd_print("\n");

	if (sunxi_codec->stream_type == PLAYBACK) {
		if (freq == 24576000) {
			hal_clk_set_parent(sunxi_codec->moduleclk, sunxi_codec->pllclk1);
		} else {
			hal_clk_set_parent(sunxi_codec->moduleclk, sunxi_codec->pllclk);
		}
		hal_clk_set_rate(sunxi_codec->moduleclk, freq);
	}

	if (sunxi_codec->stream_type == CAPTURE) {
		if (freq == 24576000) {
			hal_clk_set_parent(sunxi_codec->moduleclk1, sunxi_codec->pllclk1);
		} else {
			hal_clk_set_parent(sunxi_codec->moduleclk1, sunxi_codec->pllclk);
		}
		hal_clk_set_rate(sunxi_codec->moduleclk1, freq);
	}

	return 0;
}

static void sunxi_codec_shutdown(struct snd_pcm_substream *substream,
		struct snd_dai *dai)
{
	return ;
}

static int sunxi_codec_prepare(struct snd_pcm_substream *substream,
		struct snd_dai *dai)
{
	struct snd_codec *codec = dai->component;

	snd_print("\n");

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		snd_codec_update_bits(codec, SUNXI_DAC_FIFOC,
			(1 << FIFO_FLUSH), (1 << FIFO_FLUSH));
		snd_codec_write(codec, SUNXI_DAC_FIFOS,
			(1 << DAC_TXE_INT | 1 << DAC_TXU_INT | 1 << DAC_TXO_INT));
		snd_codec_write(codec, SUNXI_DAC_CNT, 0);
	} else {
		snd_codec_update_bits(codec, SUNXI_ADC_FIFOC,
				(1 << ADC_FIFO_FLUSH), (1 << ADC_FIFO_FLUSH));
		snd_codec_write(codec, SUNXI_ADC_FIFOS,
				(1 << ADC_RXA_INT | 1 << ADC_RXO_INT));
		snd_codec_write(codec, SUNXI_ADC_CNT, 0);
	}

	return 0;
}

static int sunxi_codec_trigger(struct snd_pcm_substream *substream,
		int cmd, struct snd_dai *dai)
{
	struct snd_codec *codec = dai->component;

	snd_print("\n");
	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			snd_codec_update_bits(codec, SUNXI_DAC_FIFOC,
				(1 << DAC_DRQ_EN), (1 << DAC_DRQ_EN));
		}
		else if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
			snd_codec_update_bits(codec, SUNXI_ADC_FIFOC,
				(1 << ADC_DRQ_EN), (1 << ADC_DRQ_EN));
		}
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			snd_codec_update_bits(codec, SUNXI_DAC_FIFOC,
				(1 << DAC_DRQ_EN), (0 << DAC_DRQ_EN));
		}
		else if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
			snd_codec_update_bits(codec, SUNXI_ADC_FIFOC,
				(1 << ADC_DRQ_EN), (0 << ADC_DRQ_EN));
		}
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static struct snd_dai_ops sun8iw20_codec_dai_ops = {
	.startup	= sunxi_codec_startup,
	.hw_params	= sunxi_codec_hw_params,
	.shutdown	= sunxi_codec_shutdown,
	.set_sysclk	= sunxi_codec_set_sysclk,
	.trigger	= sunxi_codec_trigger,
	.prepare	= sunxi_codec_prepare,
	.dapm_control   = sunxi_codec_dapm_control,
};

static struct snd_dai sun8iw20_codec_dai[] = {
	{
		.name	= "sun8iw20codec",
		.playback = {
			.stream_name = "Playback",
			.channels_min = 1,
			.channels_max = 2,
			.rates	= SNDRV_PCM_RATE_8000_192000
				| SNDRV_PCM_RATE_KNOT,
			.formats = SNDRV_PCM_FMTBIT_S16_LE
				| SNDRV_PCM_FMTBIT_S24_LE,
			.rate_min       = 8000,
			.rate_max       = 192000,
		},
		.capture = {
			.stream_name = "Capture",
			.channels_min = 1,
			.channels_max = 3,
			.rates = SNDRV_PCM_RATE_8000_48000
				| SNDRV_PCM_RATE_KNOT,
			.formats = SNDRV_PCM_FMTBIT_S16_LE
				| SNDRV_PCM_FMTBIT_S24_LE,
			.rate_min       = 8000,
			.rate_max       = 48000,
		},
		.ops = &sun8iw20_codec_dai_ops,
	},
};

static void sunxi_jack_detect(void *param)
{
	struct sunxi_codec_info *sunxi_codec = param;
	struct sunxi_codec_param *codec_param = &sunxi_codec->param;
	gpio_data_t gpio_status;

	if (codec_param->gpio_spk > 0) {
		hal_gpio_set_direction(codec_param->gpio_spk, GPIO_MUXSEL_OUT);
		hal_gpio_get_data(SUNXI_CODEC_JACK_DET, &gpio_status);
		if (gpio_status) {
			/* jack plug out */
			hal_gpio_set_data(codec_param->gpio_spk, codec_param->pa_level);
		} else {
			/* jack plug in */
			hal_gpio_set_data(codec_param->gpio_spk, !codec_param->pa_level);
		}
	}
}

static irqreturn_t sunxi_jack_irq_handler(int irq, void *ptr)
{
	struct sunxi_codec_info *sunxi_codec = ptr;

	hal_timer_set_oneshot(SUNXI_TMR0, 300000, sunxi_jack_detect, sunxi_codec);

	return 0;
}

static int sun8iw20_codec_probe(struct snd_codec *codec)
{
	struct sunxi_codec_info *sunxi_codec = NULL;
	struct reset_control *reset;
	hal_clk_status_t ret;

	if (!codec->codec_dai)
		return -1;

	sunxi_codec = snd_malloc(sizeof(struct sunxi_codec_info));
	if (!sunxi_codec) {
		snd_err("no memory\n");
		return -ENOMEM;
	}

	codec->private_data = (void *)sunxi_codec;

	snd_print("codec para init\n");
	/* get codec para from board config? */
	sunxi_codec->param = default_param;
	sunxi_codec->stream_type = PLAYBACK;
	codec->codec_base_addr = (void *)SUNXI_CODEC_BASE_ADDR;
	codec->codec_dai->component = codec;

	reset = hal_reset_control_get(HAL_SUNXI_RESET, RST_BUS_AUDIO_CODEC);
	hal_reset_control_deassert(reset);
	hal_reset_control_put(reset);

	sunxi_codec->busclk = hal_clock_get(HAL_SUNXI_CCU, CLK_BUS_AUDIO_CODEC);
	sunxi_codec->moduleclk = hal_clock_get(HAL_SUNXI_CCU, CLK_AUDIO_DAC);
	sunxi_codec->moduleclk1 = hal_clock_get(HAL_SUNXI_CCU, CLK_AUDIO_ADC);
	sunxi_codec->pllclk = hal_clock_get(HAL_SUNXI_CCU, CLK_PLL_AUDIO0);
	sunxi_codec->pllclk1 = hal_clock_get(HAL_SUNXI_CCU, CLK_PLL_AUDIO1_DIV5);

	hal_clk_set_parent(sunxi_codec->moduleclk, sunxi_codec->pllclk);
	hal_clk_set_parent(sunxi_codec->moduleclk1, sunxi_codec->pllclk);

	hal_clk_set_rate(sunxi_codec->pllclk, 22579200);
	hal_clk_set_rate(sunxi_codec->pllclk1, 614400000);

	ret = hal_clock_enable(sunxi_codec->pllclk);
	if (ret != HAL_CLK_STATUS_OK)
	    snd_err("pllclk clock enable failed.\n");
	ret = hal_clock_enable(sunxi_codec->pllclk1);
	if (ret != HAL_CLK_STATUS_OK)
	    snd_err("pllclk1 clock enable failed.\n");
	ret = hal_clock_enable(sunxi_codec->busclk);
	if (ret != HAL_CLK_STATUS_OK)
	    snd_err("busclk clock enable failed.\n");
	ret = hal_clock_enable(sunxi_codec->moduleclk);
	if (ret != HAL_CLK_STATUS_OK)
	    snd_err("moduleclk clock enable failed.\n");
	ret = hal_clock_enable(sunxi_codec->moduleclk1);
	if (ret != HAL_CLK_STATUS_OK)
	    snd_err("moduleclk1 clock enable failed.\n");

	sunxi_codec_init(codec);
#ifdef SUNXI_ADC_DAUDIO_SYNC
	adc_daudio_sync_codec = codec;
#endif

	hal_timer_init(SUNXI_TMR0);
	if (hal_gpio_to_irq(SUNXI_CODEC_JACK_DET, &sunxi_codec->irq) < 0)
	    snd_err("sunxi jack gpio to irq err.\n");
	if (hal_gpio_irq_request(sunxi_codec->irq, sunxi_jack_irq_handler, IRQ_TYPE_EDGE_FALLING, sunxi_codec) < 0)
	    snd_err("sunxi jack irq request err.\n");
	sunxi_jack_detect(sunxi_codec);
	if (hal_gpio_irq_enable(sunxi_codec->irq) < 0)
	    snd_err("sunxi jack irq enable err.\n");

	return 0;
}

static int sun8iw20_codec_remove(struct snd_codec *codec)
{
	struct sunxi_codec_info *sunxi_codec = codec->private_data;
	struct sunxi_codec_param *param = &sunxi_codec->param;

	if (param->adcdrc_cfg)
		adcdrc_enable(codec, 0);
	if (param->adchpf_cfg)
		adchpf_enable(codec, 0);
	if (param->dacdrc_cfg)
		dacdrc_enable(codec, 0);
	if (param->dachpf_cfg)
		dachpf_enable(codec, 0);
	hal_clock_disable(sunxi_codec->moduleclk);
	hal_clock_disable(sunxi_codec->moduleclk1);
	hal_clock_disable(sunxi_codec->pllclk);
	hal_clock_disable(sunxi_codec->pllclk1);

	hal_gpio_irq_disable(sunxi_codec->irq);
	hal_gpio_irq_free(sunxi_codec->irq);
	hal_timer_uninit(SUNXI_TMR0);
	snd_free(sunxi_codec);
	codec->private_data = NULL;

	return 0;
}

struct snd_codec sunxi_audiocodec = {
	.name		= "audiocodec",
	.codec_dai	= sun8iw20_codec_dai,
	.codec_dai_num  = ARRAY_SIZE(sun8iw20_codec_dai),
	.private_data	= NULL,
	.probe		= sun8iw20_codec_probe,
	.remove		= sun8iw20_codec_remove,
	.controls       = sunxi_codec_controls,
	.num_controls   = ARRAY_SIZE(sunxi_codec_controls),
};

