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

#include <string.h>
#include <aw-alsa-lib/pcm_config.h>

int _snd_pcm_hw_open(snd_pcm_t **pcmp, const snd_pcm_config_t *pcm_config,
		snd_pcm_stream_t stream, int mode);

static _snd_pcm_open_func_t _snd_pcm_open_funcs[] = {
	{ "hw", _snd_pcm_hw_open },
};

const snd_pcm_config_t *snd_pcm_config_get_config(const char *name)
{
	const snd_pcm_config_t *pcm_config = NULL;
	size_t i = 0;

	for (i = 0; i < _snd_pcm_global_configs_size; ++i) {
		if (0 == strcmp(name, _snd_pcm_global_configs[i].name)) {
			pcm_config = &(_snd_pcm_global_configs[i]);
			break;
		}
	}
	return pcm_config;
}

snd_pcm_open_func_t snd_pcm_config_get_open_func(const char *type)
{
	snd_pcm_open_func_t open_func = NULL;
	size_t size = sizeof(_snd_pcm_open_funcs) / sizeof(_snd_pcm_open_funcs[0]);
	size_t i = 0;

	for (i = 0; i < size; ++i) {
		if (0 == strcmp(type, _snd_pcm_open_funcs[i].type)) {
			open_func = _snd_pcm_open_funcs[i].func;
			break;
		}
	}
	return open_func;
}
