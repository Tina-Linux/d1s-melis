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
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <aw-alsa-lib/pcm.h>
#include <aw-alsa-lib/control.h>
#include <aw-alsa-lib/common.h>
#include <aw_common.h>
#include <sound/card.h>
#include <FreeRTOS.h>
#include <task.h>
#include <FreeRTOS-Plus-CLI/FreeRTOS_CLI.h>

#define AW_ALSA_LIB_TEST_ARGC 1U

#define AW_ALSA_LIB_TEST_TOTAL "1"
#define AW_ALSA_SNDDMIC        "hw:snddmic"
#define AW_ALSA_SNDDAUDIO0     "hw:snddaudio0"
#define AW_ALSA_SNDDAUDIO1     "hw:snddaudio1"
#define AW_ALSA_SNDDUMMY       "hw:snd-dummy"
#define AW_ALSA_LIB_TEST_CARD  AW_ALSA_SNDDMIC
#define AW_ALSA_DEFAULT_CARD   "default"

typedef int (*aw_alsa_test_case)(const int argc, const char **argv);

typedef struct {
	const char *func_desc;
	aw_alsa_test_case func;
	const int argc;
	const char **argv;
} aw_alsa_funclist_t;

#define AW_ALSA_LOG_COLOR_NONE		"\e[0m"
#define AW_ALSA_LOG_COLOR_RED		"\e[31m"

#define AW_ALSA_LIB_TEST_BEGIN()	printf("##### %s BEGIN #####\n", __func__)
#define AW_ALSA_LIB_TEST_END()		printf("##### %s END #####\n", __func__)
#define AW_ALSA_LIB_TEST_OK()		printf("##### %s Successful #####\n", __func__)
#define AW_ALSA_LIB_TEST_FAILED() \
	printf(AW_ALSA_LOG_COLOR_RED"##### %s Failed #####\n" \
		AW_ALSA_LOG_COLOR_NONE, __func__)

static int card_info_test(const int argc, const char *argv[])
{
	int ret = 0;
	int i = 0;

	AW_ALSA_LIB_TEST_BEGIN();
	ret = snd_card_num();
	if (ret < 0) {
		printf("no registered card...\n");
		goto err_func;
	}
	printf("======== Sound Card list ========\n");
	printf("%8s%20s\n", "card_num", "card_name");
	for (i = 0; i < ret; i++) {
		if (i != snd_card_index(snd_card_name(i))) {
			printf("card_num%d not equal index convert"
				"from name:%s\n", i, snd_card_name(i));
			goto err_func;
		}
		printf("%8d%20s\n", i, snd_card_name(i));
	}
	printf("\n");

	AW_ALSA_LIB_TEST_END();
	AW_ALSA_LIB_TEST_OK();
	return ret;
err_func:
	AW_ALSA_LIB_TEST_FAILED();
	return ret;
}

extern int sunxi_soundcard_init(void);
static int card_register_test(const int argc, const char *argv[])
{
	int ret = 0;
	AW_ALSA_LIB_TEST_BEGIN();

	ret = sunxi_soundcard_init();
	if (ret != 0) {
		printf("card register test failed\n");
		goto err_func;
	}
	AW_ALSA_LIB_TEST_END();
	AW_ALSA_LIB_TEST_OK();
	return ret;
err_func:
	AW_ALSA_LIB_TEST_FAILED();
	return ret;
}
static int card_unregister_test(const int argc, const char *argv[])
{
	int ret = 0;
	AW_ALSA_LIB_TEST_BEGIN();

	ret = snd_card_unregister_all();
	if (ret != 0) {
		printf("card unregister test failed\n");
		goto err_func;
	}
	AW_ALSA_LIB_TEST_END();
	AW_ALSA_LIB_TEST_OK();
	return ret;
err_func:
	AW_ALSA_LIB_TEST_FAILED();
	return ret;
}

static const char *card_register_unregister_argv[] =
	{"card_register_unregister_test", AW_ALSA_LIB_TEST_TOTAL};

static int card_register_unregister_test(const int argc, const char *argv[])
{
	int ret = 0;
	int count = 0, loop = 100;

	AW_ALSA_LIB_TEST_BEGIN();
	if (argc >= 2)
		loop = atoi(argv[1]);

	snd_card_unregister_all();
	printf("register/unregister card loop %d times...\n", loop);
	for (count = 0; count < loop; count++) {
		ret = sunxi_soundcard_init();
		if (ret != 0) {
			printf("card register failed, loop count:%d\n", count);
			goto err_func;
		}
		ret = snd_card_unregister_all();
		if (ret != 0) {
			printf("card unregister failed, loop count:%d\n", count);
			goto err_func;
		}
	}
	sunxi_soundcard_init();

	AW_ALSA_LIB_TEST_END();
	AW_ALSA_LIB_TEST_OK();
	return ret;
err_func:
	AW_ALSA_LIB_TEST_FAILED();
	return ret;
}

struct {
	int stream;
	snd_pcm_format_t format;
	unsigned int channels;
	unsigned int rate;
	snd_pcm_uframes_t period_frames;
	snd_pcm_uframes_t buffer_frames;
	unsigned int periods;
	unsigned int period_time;
} g_hw_params = {
	.stream		= SND_PCM_STREAM_PLAYBACK,
	.format		= SND_PCM_FORMAT_S16_LE,
	.channels	= 2,
	.rate		= 16000,
	.period_time	= 100000,
	.period_frames	= 512,
	.buffer_frames	= 4096,
	.periods	= 4,
};

struct {
	snd_pcm_uframes_t start_threshold;
	snd_pcm_uframes_t stop_threshold;
	snd_pcm_uframes_t avail_min;
} g_sw_params = {
	.start_threshold = 1,
	.stop_threshold  = 4096,
	.avail_min       = 512,
};

static int card_open_close_test(const int argc, const char *argv[])
{
	int ret = 0;
	int count = 0;
	char *stream_name = NULL;
	int loop = 1;
	const char *card_name = NULL;
	snd_pcm_t *handle = NULL;

	AW_ALSA_LIB_TEST_BEGIN();

	if (argc == 3) {
		loop = atoi(argv[1]);
		card_name = argv[2];
	}
	if (card_name == NULL)
		card_name = AW_ALSA_DEFAULT_CARD;

	printf("card open/close loop %d times...\n", loop);

	stream_name = (g_hw_params.stream == SND_PCM_STREAM_PLAYBACK) ?
				"playback" : "capture";

	for (count = 0; count < loop; count++) {
		ret = snd_pcm_open(&handle, card_name, g_hw_params.stream, 0);
		if (ret < 0) {
			printf("snd_pcm_open %s failed, loop[%d]:%d\n",
				stream_name, loop, count);
			goto err_func;
		}
		ret = snd_pcm_close(handle);
		if (ret < 0) {
			printf("snd_pcm_close %s failed, loop[%d]:%d\n",
				stream_name, loop, count);
			goto err_func;
		}
		handle = NULL;
	}
	AW_ALSA_LIB_TEST_END();

	AW_ALSA_LIB_TEST_OK();
	return 0;

err_func:
	if (handle != NULL)
		snd_pcm_close(handle);
	AW_ALSA_LIB_TEST_FAILED();
	return ret;
}

static const char *card_play_open_close_argv[] =
	{"card_play_open_close", AW_ALSA_LIB_TEST_TOTAL, AW_ALSA_LIB_TEST_CARD};

static int card_play_open_close(const int argc, const char *argv[])
{
	g_hw_params.stream = SND_PCM_STREAM_PLAYBACK;
	if (!strcmp(argv[2], AW_ALSA_SNDDMIC)) {
		AW_ALSA_LIB_TEST_BEGIN();
		AW_ALSA_LIB_TEST_END();
		AW_ALSA_LIB_TEST_OK();
		return 0;
	}
	return card_open_close_test(argc, argv);
}

static const char *card_capture_open_close_argv[] =
	{"card_capture_open_close", AW_ALSA_LIB_TEST_TOTAL, AW_ALSA_LIB_TEST_CARD};

static int card_capture_open_close(const int argc, const char *argv[])
{
	g_hw_params.stream = SND_PCM_STREAM_CAPTURE;
	return card_open_close_test(argc, argv);
}

static const char *card_hw_params_test_argv[] =
	{"card_hw_params_test", AW_ALSA_LIB_TEST_TOTAL, AW_ALSA_LIB_TEST_CARD};

static int card_hw_params_test(const int argc, const char *argv[])
{
	int ret = 0;
	snd_pcm_t *handle = NULL;
	snd_pcm_hw_params_t *params = NULL;
	snd_pcm_format_t format;
	unsigned int channels, rate;
	snd_pcm_uframes_t period_frames, buffer_frames;
	unsigned int periods, period_time;
	int count = 0;
	int loop = 1;
	int temp_stream = g_hw_params.stream;
	const char *card_name = NULL;

	AW_ALSA_LIB_TEST_BEGIN();

	if (argc == 3) {
		loop = atoi(argv[1]);
		card_name = argv[2];
	}
	if (card_name == NULL)
		card_name = AW_ALSA_DEFAULT_CARD;

	if (!strcmp(card_name, AW_ALSA_SNDDMIC)) {
		g_hw_params.stream = SND_PCM_STREAM_CAPTURE;
	}

	for (count = 0; count < loop; count++) {
		ret = snd_pcm_open(&handle, card_name, g_hw_params.stream, 0);
		if (ret < 0) {
			printf("snd_pcm_open stream:%d failed, loop[%d]:%d\n",
				g_hw_params.stream, loop, count);
			goto err_func;
		}

		snd_pcm_hw_params_alloca(&params);
		ret = snd_pcm_hw_params_any(handle, params);
		if (ret < 0) {
			printf("no configurations available\n");
			goto err_func;
		}

		ret = snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
		if (ret < 0) {
			printf("hw_params_set_access failed.\n");
			goto err_func;
		}

		ret = snd_pcm_hw_params_set_format(handle, params, g_hw_params.format);
		if (ret < 0) {
			printf("hw_params_set_format failed.\n");
			goto err_func;
		}

		ret = snd_pcm_hw_params_set_channels(handle, params, g_hw_params.channels);
		if (ret < 0) {
			printf("hw_params_set_channels failed.\n");
			goto err_func;
		}

		ret = snd_pcm_hw_params_set_rate(handle, params, g_hw_params.rate, 0);
		if (ret < 0) {
			printf("hw_params_set_rate failed.\n");
			goto err_func;
		}

		if (g_hw_params.period_frames != 0) {
			ret = snd_pcm_hw_params_set_period_size(handle, params, g_hw_params.period_frames, 0);
			if (ret < 0) {
				printf("hw_params_set_period_size failed.\n");
				goto err_func;
			}
		} else if (g_hw_params.period_time != 0) {
			ret = snd_pcm_hw_params_set_period_time(handle, params, g_hw_params.period_time, 0);
			if (ret < 0) {
				printf("hw_params_set_period_time failed.\n");
				goto err_func;
			}
		}
		if (g_hw_params.buffer_frames != 0) {
			ret = snd_pcm_hw_params_set_buffer_size(handle, params, g_hw_params.buffer_frames);
			if (ret < 0) {
				printf("hw_params_set_period_size failed.\n");
				goto err_func;
			}
		} else if (g_hw_params.periods != 0) {
			ret = snd_pcm_hw_params_set_periods(handle, params, g_hw_params.periods, 0);
			if (ret < 0) {
				printf("hw_params_set_periods failed.\n");
				goto err_func;
			}
		}
		ret = snd_pcm_hw_params(handle, params);
		if (ret < 0) {
			printf("Unable to install hw params!\n");
			goto err_func;
		}

		printf("Setting:\n");
		printf("format             = %u\n", g_hw_params.format);
		printf("channels           = %u\n", g_hw_params.channels);
		printf("rate               = %u\n", g_hw_params.rate);
		printf("period_frames      = %lu\n", g_hw_params.period_frames);
		printf("buffer_frames      = %lu\n", g_hw_params.buffer_frames);
		printf("period_time        = %u\n", g_hw_params.period_time);
		printf("periods            = %u\n", g_hw_params.periods);
		printf("\n");

		snd_pcm_hw_params_get_format(params, &format);
		snd_pcm_hw_params_get_channels(params, &channels);
		snd_pcm_hw_params_get_rate(params, &rate, 0);
		snd_pcm_hw_params_get_period_size(params, &period_frames, 0);
		snd_pcm_hw_params_get_buffer_size(params, &buffer_frames);
		snd_pcm_hw_params_get_period_time(params, &period_time, 0);
		snd_pcm_hw_params_get_periods(params, &periods, 0);

		printf("After Setting:\n");
		printf("format             = %u\n", format);
		printf("channels           = %u\n", channels);
		printf("rate               = %u\n", rate);
		printf("period_frames      = %lu\n", period_frames);
		printf("buffer_frames      = %lu\n", buffer_frames);
		printf("period_time        = %u\n", period_time);
		printf("periods            = %u\n", periods);
		printf("\n");

		if ((g_hw_params.format != format) ||
			(g_hw_params.channels != channels) ||
			(g_hw_params.rate != rate) ||
			(period_frames * periods != buffer_frames) ||
			(period_time != (period_frames * 1000000/rate))) {
			printf("Failed Test total[%d]:%d\n", loop, count);
			goto err_func;
		}
		snd_pcm_close(handle);
	}
	AW_ALSA_LIB_TEST_END();
	AW_ALSA_LIB_TEST_OK();
	g_hw_params.stream = temp_stream;
	return ret;
err_func:
	snd_pcm_close(handle);
	AW_ALSA_LIB_TEST_FAILED();
	g_hw_params.stream = temp_stream;
	return ret;
}

#define card_hw_params_test_detail(type) \
	int i; \
	for (i = 0; i < ARRAY_SIZE(type##_array); i++) { \
		printf(#type" %u test:\n", type##_array[i]); \
		g_hw_params.type = type##_array[i]; \
		ret = card_hw_params_test( \
			    ARRAY_SIZE(card_hw_params_##type##_argv), \
			    card_hw_params_##type##_argv); \
		if (ret < 0) { \
			printf(#type" %u test failed\n", type##_array[i]); \
			goto err_func; \
		} \
	}

static const char *card_hw_params_format_argv[] =
	{"card_hw_params_format", AW_ALSA_LIB_TEST_TOTAL, AW_ALSA_LIB_TEST_CARD};

static int card_hw_params_format(const int argc, const char *argv[])
{
	int ret = 0;
	snd_pcm_format_t format_bak = g_hw_params.format;
	snd_pcm_format_t format_array[] = {
		SND_PCM_FORMAT_S16_LE,
		SND_PCM_FORMAT_S24_LE,
		SND_PCM_FORMAT_S32_LE,
	};
	card_hw_params_test_detail(format);
err_func:
	g_hw_params.format = format_bak;
	return ret;
}

static const char *card_hw_params_channels_argv[] =
	{"card_hw_params_channels", AW_ALSA_LIB_TEST_TOTAL, AW_ALSA_LIB_TEST_CARD};

static int card_hw_params_channels(const int argc, const char *argv[])
{
	int ret = 0;
	unsigned int channels_bak = g_hw_params.channels;
	unsigned int channels_array[] = {
		1,2,3,4,5,6,7,8,
	};

	card_hw_params_test_detail(channels);
err_func:
	g_hw_params.channels = channels_bak;
	return ret;
}

static const char *card_hw_params_rate_argv[] =
	{"card_hw_params_rate", AW_ALSA_LIB_TEST_TOTAL, AW_ALSA_LIB_TEST_CARD};

static int card_hw_params_rate(const int argc, const char *argv[])
{
	int ret = 0;
	unsigned int rate_bak = g_hw_params.rate;
	unsigned int rate_array[] = {
		8000,16000,48000,44100,96000,192000,
	};

	card_hw_params_test_detail(rate);
err_func:
	g_hw_params.rate = rate_bak;
	return ret;
}

static const char *card_hw_params_period_frames_argv[] =
	{"card_hw_params_period_size", AW_ALSA_LIB_TEST_TOTAL, AW_ALSA_LIB_TEST_CARD};

static int card_hw_params_period_size(const int argc, const char *argv[])
{
	int ret = 0;
	unsigned int period_frames_bak = g_hw_params.period_frames;
	unsigned int buffer_frames_bak = g_hw_params.buffer_frames;
	unsigned int period_time_bak = g_hw_params.period_time;
	unsigned int period_frames_array[] = {
		256,512,1024,2048,
	};
	g_hw_params.period_time = 0;
	g_hw_params.buffer_frames = 0;

	card_hw_params_test_detail(period_frames);
err_func:
	g_hw_params.period_frames = period_frames_bak;
	g_hw_params.buffer_frames = buffer_frames_bak;
	g_hw_params.period_time = period_time_bak;
	return ret;
}

static const char *card_hw_params_period_time_argv[] =
	{"card_hw_params_period_time", AW_ALSA_LIB_TEST_TOTAL, AW_ALSA_LIB_TEST_CARD};

static int card_hw_params_period_time(const int argc, const char *argv[])
{
	int ret = 0;
	unsigned int period_frames_bak = g_hw_params.period_frames;
	unsigned int buffer_frames_bak = g_hw_params.buffer_frames;
	unsigned int period_time_bak = g_hw_params.period_time;
	unsigned int period_time_array[] = {
		50000,70000,100000,
	};
	g_hw_params.period_frames = 0;
	g_hw_params.buffer_frames = 0;

	card_hw_params_test_detail(period_time);
err_func:
	g_hw_params.period_frames = period_frames_bak;
	g_hw_params.buffer_frames = buffer_frames_bak;
	g_hw_params.period_time = period_time_bak;
	return ret;
}

static const char *card_hw_params_buffer_frames_argv[] =
	{"card_hw_params_buffer_size", AW_ALSA_LIB_TEST_TOTAL, AW_ALSA_LIB_TEST_CARD};

static int card_hw_params_buffer_size(const int argc, const char *argv[])
{
	int ret = 0;
	unsigned int buffer_frames_bak = g_hw_params.buffer_frames;
	unsigned int periods_bak = g_hw_params.periods;
	unsigned int buffer_frames_array[] = {
		1024,2048,4096,8192,
	};

	g_hw_params.periods = 0;
	card_hw_params_test_detail(buffer_frames);
err_func:
	g_hw_params.buffer_frames = buffer_frames_bak;
	g_hw_params.periods = periods_bak;
	return ret;
}

static const char *card_hw_params_periods_argv[] =
	{"card_hw_params_periods", AW_ALSA_LIB_TEST_TOTAL, AW_ALSA_LIB_TEST_CARD};

static int card_hw_params_periods(const int argc, const char *argv[])
{
	int ret = 0;
	unsigned int buffer_frames_bak = g_hw_params.buffer_frames;
	unsigned int periods_bak = g_hw_params.periods;
	unsigned int periods_array[] = {
		2,3,4,5,6,7,8,
	};

	g_hw_params.buffer_frames = 0;
	card_hw_params_test_detail(periods);
err_func:
	g_hw_params.buffer_frames = buffer_frames_bak;
	g_hw_params.periods = periods_bak;
	return ret;
}

static const char *card_sw_params_test_argv[] =
	{"card_sw_params_test", AW_ALSA_LIB_TEST_TOTAL, AW_ALSA_LIB_TEST_CARD};

static int card_sw_params_test(const int argc, const char *argv[])
{
	int ret = 0;
	snd_pcm_t *handle = NULL;
	snd_pcm_sw_params_t *sw_params = NULL;
	snd_pcm_hw_params_t *hw_params = NULL;
	snd_pcm_format_t format;
	unsigned int channels, rate;
	unsigned int periods;
	snd_pcm_uframes_t period_frames, buffer_frames;
	snd_pcm_uframes_t start_threshold;
	snd_pcm_uframes_t stop_threshold;
	snd_pcm_uframes_t avail_min;
	int count = 0;
	int loop = 1;
	int temp_stream = g_hw_params.stream;
	const char *card_name = NULL;

	AW_ALSA_LIB_TEST_BEGIN();

	if (argc == 3) {
		loop = atoi(argv[1]);
		card_name = argv[2];
	}
	if (card_name == NULL)
		card_name = AW_ALSA_DEFAULT_CARD;

	if (!strcmp(card_name, AW_ALSA_SNDDMIC)) {
		g_hw_params.stream = SND_PCM_STREAM_CAPTURE;
	}

	for (count = 0; count < loop; count++) {
		snd_pcm_open(&handle, AW_ALSA_LIB_TEST_CARD, g_hw_params.stream, 0);

		snd_pcm_hw_params_alloca(&hw_params);
		ret = snd_pcm_hw_params_any(handle, hw_params);
		if (ret < 0) {
			printf("no configurations available\n");
			goto err_func;
		}

		printf("HW Setting:\n");
		printf("format             = %u\n", g_hw_params.format);
		printf("channels           = %u\n", g_hw_params.channels);
		printf("rate               = %u\n", g_hw_params.rate);
		printf("period_frames      = %lu\n", g_hw_params.period_frames);
		printf("buffer_frames      = %lu\n", g_hw_params.buffer_frames);
		printf("\n");
		snd_pcm_hw_params_set_access(handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
		snd_pcm_hw_params_set_format(handle, hw_params, g_hw_params.format);
		snd_pcm_hw_params_set_channels(handle, hw_params, g_hw_params.channels);
		snd_pcm_hw_params_set_rate(handle, hw_params, g_hw_params.rate, 0);
		snd_pcm_hw_params_set_period_size(handle, hw_params, g_hw_params.period_frames, 0);
		snd_pcm_hw_params_set_buffer_size(handle, hw_params, g_hw_params.buffer_frames);
		ret = snd_pcm_hw_params(handle, hw_params);
		if (ret < 0) {
			printf("Unable to install hw prams!\n");
			goto err_func;
		}

		printf("SW Setting:\n");
		printf("start_threshold    = %lu\n", g_sw_params.start_threshold);
		printf("stop_threshold     = %lu\n", g_sw_params.stop_threshold);
		printf("avail_min          = %lu\n", g_sw_params.avail_min);
		printf("\n");

		snd_pcm_sw_params_alloca(&sw_params);
		snd_pcm_sw_params_current(handle, sw_params);
		if (g_sw_params.start_threshold == 0) {
			snd_pcm_sw_params_set_start_threshold(handle, sw_params,
				(g_hw_params.stream == SND_PCM_STREAM_PLAYBACK) ?
				g_hw_params.period_frames : 1);
		} else {
			snd_pcm_sw_params_set_start_threshold(handle, sw_params,
					g_sw_params.start_threshold);
		}
		snd_pcm_sw_params_set_stop_threshold(handle, sw_params, g_sw_params.stop_threshold);
		snd_pcm_sw_params_set_avail_min(handle, sw_params, g_sw_params.avail_min);
		ret = snd_pcm_sw_params(handle ,sw_params);
		if (ret < 0) {
			printf("Unable to install sw prams!\n");
			goto err_func;
		}

		snd_pcm_hw_params_get_format(hw_params, &format);
		snd_pcm_hw_params_get_channels(hw_params, &channels);
		snd_pcm_hw_params_get_rate(hw_params, &rate, 0);
		snd_pcm_hw_params_get_period_size(hw_params, &period_frames, 0);
		snd_pcm_hw_params_get_periods(hw_params, &periods, 0);
		snd_pcm_hw_params_get_buffer_size(hw_params, &buffer_frames);

		snd_pcm_sw_params_get_start_threshold(sw_params, &start_threshold);
		snd_pcm_sw_params_get_stop_threshold(sw_params, &stop_threshold);
		snd_pcm_sw_params_get_avail_min(sw_params, &avail_min);

		printf("After Setting:\n");
		printf("format             = %u\n", format);
		printf("channels           = %u\n", channels);
		printf("rate               = %u\n", rate);
		printf("period_frames      = %lu\n", period_frames);
		printf("buffer_frames      = %lu\n", buffer_frames);
		printf("start_threshold    = %lu\n", start_threshold);
		printf("stop_threshold     = %lu\n", stop_threshold);
		printf("avail_min          = %lu\n", avail_min);

		if ((g_hw_params.format != format) ||
			(g_hw_params.channels != channels) ||
			(g_hw_params.rate != rate) ||
			(period_frames * periods != buffer_frames) ||
			(g_sw_params.start_threshold != start_threshold) ||
			(g_sw_params.stop_threshold != stop_threshold) ||
			(g_sw_params.avail_min != avail_min)) {
			printf("Failed Test total[%d]:%d\n", loop, count);
			goto err_func;
		}
		snd_pcm_close(handle);
	}
	AW_ALSA_LIB_TEST_END();
	AW_ALSA_LIB_TEST_OK();
	g_hw_params.stream = temp_stream;
	return ret;
err_func:
	snd_pcm_close(handle);
	AW_ALSA_LIB_TEST_FAILED();
	g_hw_params.stream = temp_stream;
	return ret;
}

#define card_sw_params_test_detail(type) \
	int i; \
	for (i = 0; i < ARRAY_SIZE(type##_array); i++) { \
		printf(#type" %lu test:\n", type##_array[i]); \
		g_sw_params.type = type##_array[i]; \
		ret = card_sw_params_test( \
			    ARRAY_SIZE(card_sw_params_##type##_argv), \
			    card_sw_params_##type##_argv); \
		if (ret < 0) { \
			printf(#type" %lu test failed\n", type##_array[i]); \
			goto err_func; \
		} \
	}

static const char *card_sw_params_start_threshold_argv[] =
	{"card_sw_params_start_threshold", AW_ALSA_LIB_TEST_TOTAL, AW_ALSA_LIB_TEST_CARD};

static int card_sw_params_start_threshold(const int argc, const char *argv[])
{
	int ret = 0;
	snd_pcm_uframes_t start_threshold_bak = g_sw_params.start_threshold;
	snd_pcm_uframes_t start_threshold_array[] = {
		1, 256, 512, 1024,
	};

	card_sw_params_test_detail(start_threshold);
err_func:
	g_sw_params.start_threshold = start_threshold_bak;
	return ret;
}

static const char *card_sw_params_stop_threshold_argv[] =
	{"card_sw_params_stop_threshold", AW_ALSA_LIB_TEST_TOTAL, AW_ALSA_LIB_TEST_CARD};

static int card_sw_params_stop_threshold(const int argc, const char *argv[])
{
	int ret = 0;
	snd_pcm_uframes_t stop_threshold_bak = g_sw_params.stop_threshold;
	snd_pcm_uframes_t stop_threshold_array[] = {
		512, 1024, 2048, 4096, 8192,
	};

	card_sw_params_test_detail(stop_threshold);
err_func:
	g_sw_params.stop_threshold = stop_threshold_bak;
	return ret;
}

static const char *card_sw_params_avail_min_argv[] =
	{"card_sw_params_avail_min", AW_ALSA_LIB_TEST_TOTAL, AW_ALSA_LIB_TEST_CARD};

static int card_sw_params_avail_min(const int argc, const char *argv[])
{
	int ret = 0;
	snd_pcm_uframes_t avail_min_bak = g_sw_params.avail_min;
	snd_pcm_uframes_t avail_min_array[] = {
		512, 1024, 2048
	};

	card_sw_params_test_detail(avail_min);
err_func:
	g_sw_params.avail_min = avail_min_bak;
	return ret;
}

/*
 * PCM STATUS
 * (1) Open
 *     SNDRV_PCM_STATE_OPEN = 0,
 * (2) Setup installed
 *     SNDRV_PCM_STATE_SETUP = 1,
 * Ready to start
 *     SNDRV_PCM_STATE_PREPARED = 2,
 * (3) Running
 *     SNDRV_PCM_STATE_RUNNING = 3,
 * (4) Stopped: underrun (playback) or overrun (capture) detected
 *     SNDRV_PCM_STATE_XRUN = 4,
 * (5) Draining: running (playback) or stopped (capture)
 *     SNDRV_PCM_STATE_DRAINING = 5,
 * (6) Paused
 *     SNDRV_PCM_STATE_PAUSED = 6,
 * (7) Hardware is suspended
 *     SNDRV_PCM_STATE_SUSPENDED = 7,
 * (8) Hardware is disconnected
 *     SNDRV_PCM_STATE_DISCONNECTED = 8,
 */
static const char *card_pcm_state_test_argv[] =
	{"card_pcm_state_test", AW_ALSA_LIB_TEST_TOTAL, AW_ALSA_LIB_TEST_CARD};

static int card_pcm_state_test(const int argc, const char *argv[])
{
	int ret = 0;
	snd_pcm_t *handle = NULL;
	snd_pcm_sw_params_t *sw_params = NULL;
	snd_pcm_hw_params_t *params = NULL;
	snd_pcm_state_t state = -1;
	snd_pcm_uframes_t boundary = 0;
	snd_pcm_sframes_t delay = 0;
	int count = 0;
	int loop = 1;
	int temp_stream = g_hw_params.stream;
	const char *card_name = NULL;

	/* open, setup, prepare, running, pause, drain */
	AW_ALSA_LIB_TEST_BEGIN();

	if (argc == 3) {
		loop = atoi(argv[1]);
		card_name = argv[2];
	}
	if (card_name == NULL)
		card_name = AW_ALSA_DEFAULT_CARD;

	if (!strcmp(card_name, AW_ALSA_SNDDMIC)) {
		g_hw_params.stream = SND_PCM_STREAM_CAPTURE;
	}

	for (count = 0; count < loop; count++) {
		snd_pcm_open(&handle, AW_ALSA_LIB_TEST_CARD, g_hw_params.stream, 0);
		state = snd_pcm_state(handle);
		if (state != SNDRV_PCM_STATE_OPEN) {
			printf("After snd_pcm_open, state is %d\n", state);
			goto err_func;
		}
		snd_pcm_hw_params_alloca(&params);
		ret = snd_pcm_hw_params_any(handle, params);
		if (ret < 0) {
			printf("no configurations available\n");
			goto err_func;
		}

		snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
		snd_pcm_hw_params_set_format(handle, params, g_hw_params.format);
		snd_pcm_hw_params_set_channels(handle, params, g_hw_params.channels);
		snd_pcm_hw_params_set_rate(handle, params, g_hw_params.rate, 0);
		snd_pcm_hw_params_set_period_size(handle, params, g_hw_params.period_frames, 0);
		snd_pcm_hw_params_set_periods(handle, params, g_hw_params.periods, 0);

		/* it will call snd_pcm_prepare */
		ret = snd_pcm_hw_params(handle, params);
		if (ret < 0) {
			printf("Unable to install hw prams!\n");
			goto err_func;
		}
		state = snd_pcm_state(handle);
		/* alsalib snd_pcm_hw_params alsa perform snd_pcm_prepare */
		if (state != SNDRV_PCM_STATE_PREPARED) {
			printf("After snd_pcm_hw_params, state is %d\n", state);
			goto err_func;
		}

		snd_pcm_sw_params_alloca(&sw_params);
		snd_pcm_sw_params_current(handle, sw_params);

		snd_pcm_sw_params_get_boundary(sw_params, &boundary);
		snd_pcm_sw_params_set_start_threshold(handle, sw_params, g_hw_params.period_frames);
		snd_pcm_sw_params_set_avail_min(handle, sw_params, g_sw_params.avail_min);
		snd_pcm_sw_params_set_stop_threshold(handle, sw_params, boundary);
		snd_pcm_sw_params_set_silence_size(handle, sw_params, boundary);

		ret = snd_pcm_sw_params(handle ,sw_params);
		if (ret < 0) {
			printf("Unable to install sw prams!\n");
			goto err_func;
		}

		snd_pcm_start(handle);
		state = snd_pcm_state(handle);
		if (state != SNDRV_PCM_STATE_RUNNING) {
			printf("After snd_pcm_start, state is %d\n", state);
			goto err_func;
		}

		vTaskDelay(pdMS_TO_TICKS(100));
		ret = snd_pcm_delay(handle, &delay);
		if (ret < 0)
			printf("snd_pcm_delay failed, return %ld\n", delay);

		printf("pcm delay:%ld\n", delay);
		vTaskDelay(pdMS_TO_TICKS(100));
		ret = snd_pcm_delay(handle, &delay);
		if (ret < 0)
			printf("snd_pcm_delay failed, return %ld\n", delay);

		printf("pcm delay:%ld\n", delay);
		vTaskDelay(pdMS_TO_TICKS(200));
		ret = snd_pcm_delay(handle, &delay);
		if (ret < 0)
			printf("snd_pcm_delay failed, return %ld\n", delay);

		printf("pcm delay:%ld\n", delay);
		ret = snd_pcm_hw_params_can_pause(params);
		printf("%s Support pause...\n", ret ? "" : "Can't");

		snd_pcm_pause(handle, 1);
		state = snd_pcm_state(handle);
		if (state != SNDRV_PCM_STATE_PAUSED) {
			printf("After snd_pcm_pause push, state is %d\n", state);
			goto err_func;
		}
#if 1
		snd_pcm_pause(handle, 0);
		state = snd_pcm_state(handle);
		if (state != SNDRV_PCM_STATE_RUNNING) {
			printf("After snd_pcm_pause release, state is %d\n", state);
			goto err_func;
		}
#endif

#if 0
		snd_pcm_drain(handle);
		state = snd_pcm_state(handle);
		if (state != SNDRV_PCM_STATE_DRAINING) {
			printf("After snd_pcm_drain, state is %d\n", state);
			goto err_func;
		}
#endif
#if 1
		snd_pcm_drop(handle);
		state = snd_pcm_state(handle);
		if (state != SNDRV_PCM_STATE_SETUP) {
			printf("After snd_pcm_drop, state is %d\n", state);
			goto err_func;
		}
#endif

		snd_pcm_close(handle);
	}
	AW_ALSA_LIB_TEST_END();
	AW_ALSA_LIB_TEST_OK();
	g_hw_params.stream = temp_stream;
	return ret;
err_func:
	printf("Failed Test total[%d]:%d\n", loop, count);
	snd_pcm_close(handle);
	AW_ALSA_LIB_TEST_FAILED();
	g_hw_params.stream = temp_stream;
	return ret;
}

#if 0
static int ctl_info_test(int argc, char *argv[])
{
	int num, i, ret = 0;
	snd_ctl_info_t info;

	AW_ALSA_LIB_TEST_BEGIN();
	num = snd_ctl_num("audiocodec");
	if (num <= 0)
		goto err;

	for (i = 0; i < num;i ++) {
		memset(&info, 0, sizeof(snd_ctl_info_t));
		ret = snd_ctl_get_bynum("audiocodec", i, &info);
		if (ret < 0) {
			printf("get %d elem failed\n", i);
			goto err;
		}
		printf("numid=%u, name=\'%s\'\n", info.id, info.name);
		printf("value=%lu, min=%u, max=%u\n",
				info.value, info.min, info.max);
	}
	AW_ALSA_LIB_TEST_END();
	AW_ALSA_LIB_TEST_OK();
	return ret;
err:
	AW_ALSA_LIB_TEST_FAILED();
	return ret;
}

static int ctl_get_test(int argc, char *argv[])
{
	int ret = 0;
	int numid = 0;
	snd_ctl_info_t info0, info1;

	AW_ALSA_LIB_TEST_BEGIN();
	ret = snd_ctl_get_bynum("audiocodec", numid, &info0);
	if (ret < 0) {
		printf("get %d elem failed\n", numid);
		goto err;
	}
	ret = snd_ctl_get("audiocodec", info0.name, &info1);
	if (ret < 0) {
		printf("get %s elem failed\n", info0.name);
		goto err;
	}

	if (info0.id != info1.id) {
		printf("info id not equal. something error...\n");
		goto err;
	}

	AW_ALSA_LIB_TEST_END();
	AW_ALSA_LIB_TEST_OK();
	return ret;
err:
	AW_ALSA_LIB_TEST_FAILED();
	return ret;
}

static int ctl_set_test(int argc, char *argv[])
{
	int ret = 0;
	int numid = 0;
	snd_ctl_info_t info0, info1;

	AW_ALSA_LIB_TEST_BEGIN();
	ret = snd_ctl_get_bynum("audiocodec", numid, &info0);
	if (ret < 0) {
		printf("get %d elem failed\n", numid);
		goto err;
	}
	ret = snd_ctl_set_bynum("audiocodec", info0.id, !info0.value);
	if (ret < 0) {
		printf("set %d elem failed\n", numid);
		goto err;
	}

	ret = snd_ctl_get_bynum("audiocodec", numid, &info1);
	if (ret < 0) {
		printf("get %d elem failed\n", numid);
		goto err;
	}

	if (info0.value == info1.value) {
		printf("info id equal. something error...\n");
		goto err;
	}
	/* recover ctl elem value */
	snd_ctl_set("audiocodec", info0.name, info0.value);

	AW_ALSA_LIB_TEST_END();
	AW_ALSA_LIB_TEST_OK();
	return ret;
err:
	AW_ALSA_LIB_TEST_FAILED();
	return ret;
}

static int ctl_add_test(int argc, char *argv[])
{
	int ret;
	snd_ctl_info_t info0 = {
		.name 	= "Ctl Test 0",
		.value 	= 3,
		.min 	= 0,
		.max 	= 100,
	};
	unsigned long array[3] = {5, 6, 7};
	snd_ctl_info_t info1 = {
		.name 	 	= "Ctl Test 1",
		.min 	 	= 1,
		.max 	 	= 10,
		.count   	= ARRAY_SIZE(array),
		.private_data 	= array,
	};
	snd_ctl_info_t info;

	AW_ALSA_LIB_TEST_BEGIN();
	ret = snd_ctl_add("audiocodec", &info0);
	if (ret < 0) {
		printf("add ctl info0 failed\n");
		goto err;
	}
	ret = snd_ctl_add("audiocodec", &info1);
	if (ret < 0) {
		printf("add ctl info1 failed\n");
		goto err;
	}

	ret = snd_ctl_get("audiocodec", info0.name, &info);
	if (ret < 0) {
		printf("get %s elem failed\n", info0.name);
		goto err;
	}
	printf("info0 numid=%d, name:%s\n", info.id, info.name);
	printf("value=%lu, min=%d, max=%d\n", info.value, info.min, info.max);
	printf("\n");

	printf("try to set value 1\n");
	ret = snd_ctl_set("audiocodec", info.name, 1);
	if (ret < 0) {
		printf("set %s elem failed\n", info.name);
		goto err;
	}

	ret = snd_ctl_set_multi_args("audiocodec", info1.name, 3, 7, 8, 9);
	if (ret < 0) {
		printf("set %s elem failed\n", info1.name);
		goto err;
	}

	ret = snd_ctl_get("audiocodec", info1.name, &info);
	if (ret < 0) {
		printf("get %s elem failed\n", info0.name);
		goto err;
	}
	printf("info0 numid=%d, name:%s\n", info.id, info.name);
	printf("value=%lu, min=%d, max=%d\n", info.value, info.min, info.max);
	if (info.count > 1) {
		int i;
		for (i = 0; i < info.count; i++)
			printf("value[%d]=%lu ", i, info.private_data[i]);
		printf("\n");
	}
	printf("\n");

	AW_ALSA_LIB_TEST_END();
	AW_ALSA_LIB_TEST_OK();
	return ret;
err:
	AW_ALSA_LIB_TEST_FAILED();
	return ret;
}

static int ctl_remove_test(int argc, char *argv[])
{
	int ret;
	snd_ctl_info_t info = {
		.name 	= "Ctl Remove Test",
		.value 	= 1,
		.min 	= 0,
		.max 	= 1,
	};
	snd_ctl_info_t info_read;

	AW_ALSA_LIB_TEST_BEGIN();
	ret = snd_ctl_add("audiocodec", &info);
	if (ret < 0) {
		printf("add ctl info0 failed\n");
		goto err;
	}

	ret = snd_ctl_get("audiocodec", info.name, &info_read);
	if (ret < 0) {
		printf("get %s elem failed\n", info.name);
		goto err;
	}

	ret = snd_ctl_remove("audiocodec", info_read.id);
	if (ret != 0) {
		printf("ctl remove failed\n");
		goto err;
	}

	AW_ALSA_LIB_TEST_END();
	AW_ALSA_LIB_TEST_OK();
	return ret;
err:
	AW_ALSA_LIB_TEST_FAILED();
	return ret;

}
#endif

aw_alsa_funclist_t aw_alsalib_funclist[] = {
	{"card info",			card_info_test, 0, NULL},
	{"card register",		card_register_test, 0, NULL},
	{"card unregister",		card_unregister_test, 0, NULL},
	{"card register/unregister",	card_register_unregister_test,
					ARRAY_SIZE(card_register_unregister_argv),
					card_register_unregister_argv},
	{"card play_open_close",	card_play_open_close,
					ARRAY_SIZE(card_play_open_close_argv),
					card_play_open_close_argv},
	{"card capture_open_close",	card_capture_open_close,
					ARRAY_SIZE(card_capture_open_close_argv),
					card_capture_open_close_argv},
	{"card hw_params",		card_hw_params_test,
					ARRAY_SIZE(card_hw_params_test_argv),
					card_hw_params_test_argv},
	{"card format",			card_hw_params_format,
					ARRAY_SIZE(card_hw_params_format_argv),
					card_hw_params_format_argv},
	{"card channels",		card_hw_params_channels,
					ARRAY_SIZE(card_hw_params_channels_argv),
					card_hw_params_channels_argv},
	{"card rate",			card_hw_params_rate,
					ARRAY_SIZE(card_hw_params_rate_argv),
					card_hw_params_rate_argv},
	{"card period_size",		card_hw_params_period_size,
					ARRAY_SIZE(card_hw_params_period_frames_argv),
					card_hw_params_period_frames_argv},
	{"card period_time",		card_hw_params_period_time,
					ARRAY_SIZE(card_hw_params_period_time_argv),
					card_hw_params_period_time_argv},
	{"card buffer_size",		card_hw_params_buffer_size,
					ARRAY_SIZE(card_hw_params_buffer_frames_argv),
					card_hw_params_buffer_frames_argv},
	{"card periods",		card_hw_params_periods,
					ARRAY_SIZE(card_hw_params_periods_argv),
					card_hw_params_periods_argv},
	{"card sw_params",		card_sw_params_test,
					ARRAY_SIZE(card_sw_params_test_argv),
					card_sw_params_test_argv},
	{"card start_threshold",	card_sw_params_start_threshold,
					ARRAY_SIZE(card_sw_params_start_threshold_argv),
					card_sw_params_start_threshold_argv},
	{"card stop_threshold",		card_sw_params_stop_threshold,
					ARRAY_SIZE(card_sw_params_stop_threshold_argv),
					card_sw_params_stop_threshold_argv},
	{"card avail_min",		card_sw_params_avail_min,
					ARRAY_SIZE(card_sw_params_avail_min_argv),
					card_sw_params_avail_min_argv},
	{"card pcm state", 		card_pcm_state_test,
					ARRAY_SIZE(card_pcm_state_test_argv),
					card_pcm_state_test_argv},
#if 0
	{"ctl info", 				ctl_info_test},
	{"ctl get", 				ctl_get_test},
	{"ctl set", 				ctl_set_test},
	{"ctl add", 				ctl_add_test},
	{"ctl remove", 				ctl_remove_test},
#endif
};

#ifdef CONFIG_COMPONENTS_FREERTOS_CLI
static BaseType_t cmd_aw_alsa_test_handler(char *pcWriteBuffer, size_t xWriteBufferLen,
				const char *pcCommandString)
{
	int i = 0;
	BaseType_t xReturn = pdTRUE;
	const char *pcParameter;
	BaseType_t xParameterStringLength;
	int aw_alas_case_num = 0;
	static UBaseType_t uxParameterNumber = 0;
	aw_alsa_funclist_t *aw_alsalib_func = NULL;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	(void) pcCommandString;
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	if (uxParameterNumber < AW_ALSA_LIB_TEST_ARGC) {
		/* Obtain the parameter string. */
		pcParameter = FreeRTOS_CLIGetParameter(
					/* The command string itself. */
					pcCommandString,
					/* Return the next parameter. */
					uxParameterNumber,
					/* Store the parameter string length. */
					&xParameterStringLength);

		/* Sanity check something was returned. */
		configASSERT(pcParameter);
		xReturn = pdPASS;

		if (++uxParameterNumber < AW_ALSA_LIB_TEST_ARGC)
			return xReturn;

		printf("====== AW ALSA Lib Test ======\n");
		aw_alas_case_num = ARRAY_SIZE(aw_alsalib_funclist);
		for (i = 0; i < aw_alas_case_num; i++) {
			aw_alsalib_func = &aw_alsalib_funclist[i];
			printf("\n\t==============================\n");
			printf("\t  Test Case: %s\n", aw_alsalib_func->func_desc);
			printf("\t==============================\n");
			aw_alsalib_func->func(aw_alsalib_func->argc, aw_alsalib_func->argv);
		}
		uxParameterNumber = 0;
		printf("==============================\n");
		xReturn = pdFALSE;
	}
	return xReturn;
}

/*
 * Structure that defines the command.
 *
 * The command that causes pxCommandInterpreter to be executed.
 *     For example "help".  Must be all lower case.
 * <1> const char * const pcCommand;
 *
 * String that describes how to use the command.
 *     Should start with the command itself, and end with "\r\n".
 *     For example "help: Returns a list of all the commands\r\n".
 * <2> const char * const pcHelpString;
 *
 * A pointer to the callback function that will return the output generated by the command.
 * <3> const pdCOMMAND_LINE_CALLBACK pxCommandInterpreter;
 *
 * Commands expect a fixed number of parameters, which may be zero.
 * <4> int8_t cExpectedNumberOfParameters;
 */
static const CLI_Command_Definition_t cmd_aw_alsa_test = {
	"aw_alsa_test",
	"\r\naw_alsa_test\r\n Test AW ALSA Lib API\r\n",
	cmd_aw_alsa_test_handler, AW_ALSA_LIB_TEST_ARGC - 1
};

void cmd_aw_alsa_test_register( void )
{
	/* Register all the command line commands defined immediately above. */
	FreeRTOS_CLIRegisterCommand(&cmd_aw_alsa_test);
}
#endif
