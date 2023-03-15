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

#include <aw-alsa-lib/pcm_config.h>
#include <aw-alsa-lib/pcm_plugin.h>

#define SND_PCM_HW_CONFIG(name, device) \
{ \
	.card_name	= #name,  \
	.device_num	= device, \
}

#define DEFINE_SND_PCM_HW_CONFIG(name, device) \
	const snd_pcm_hw_config_t snd_##name##_hw_config = \
				SND_PCM_HW_CONFIG(name, device);

static DEFINE_SND_PCM_HW_CONFIG(audiocodec, 0);
static DEFINE_SND_PCM_HW_CONFIG(audiocodecadc, 0);
static DEFINE_SND_PCM_HW_CONFIG(audiocodecdac, 0);
static DEFINE_SND_PCM_HW_CONFIG(snddaudio0, 0);
static DEFINE_SND_PCM_HW_CONFIG(snddaudio1, 0);
static DEFINE_SND_PCM_HW_CONFIG(snddaudio2, 0);
static DEFINE_SND_PCM_HW_CONFIG(snddaudio3, 0);
static DEFINE_SND_PCM_HW_CONFIG(snddmic, 0);
static DEFINE_SND_PCM_HW_CONFIG(sndspdif, 0);


const snd_pcm_config_t snd_pcm_global_configs[] = {
	SND_PCM_CONFIG("hw:audiocodec", "hw", &snd_audiocodec_hw_config),
	SND_PCM_CONFIG("hw:audiocodecadc", "hw", &snd_audiocodecadc_hw_config),
	SND_PCM_CONFIG("hw:audiocodecdac", "hw", &snd_audiocodecdac_hw_config),
        SND_PCM_CONFIG("hw:snddaudio0", "hw", &snd_snddaudio0_hw_config),
        SND_PCM_CONFIG("hw:snddaudio1", "hw", &snd_snddaudio1_hw_config),
        SND_PCM_CONFIG("hw:snddaudio2", "hw", &snd_snddaudio2_hw_config),
        SND_PCM_CONFIG("hw:snddaudio3", "hw", &snd_snddaudio3_hw_config),
        SND_PCM_CONFIG("hw:snddmic", "hw", &snd_snddmic_hw_config),
        SND_PCM_CONFIG("hw:sndspdif", "hw", &snd_sndspdif_hw_config),
	SND_PCM_CONFIG("default", "hw", &snd_audiocodec_hw_config),
};

REGISTER_SND_PCM_GLOBAL_CONFIGS(snd_pcm_global_configs);
