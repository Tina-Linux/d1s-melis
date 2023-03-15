/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
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
#include <stdint.h>
#include <string.h>
#include <rtthread.h>
#include <hal_timer.h>
#include <hal_mem.h>
#include <log.h>
#include <video/sunxi_display2.h>
#include "disp_layer_cfg.h"
#include "disp_mem.h"

extern int msleep(int);
static u32 g_screen_index;

static int show_layer(struct test_layer_cfg *cfg, u32 width, u32 height,
		      u32 channel, u32 format, char *filename)
{
	if (!cfg || !filename)
		return -1;

	printf("show %s format:%ld\n", filename, format);
	disp_mem(0, width, height, 0, filename);

	cfg->mem_id = 0;
	cfg->screen_id = g_screen_index;
	cfg->layer_cfg.channel = channel;
	cfg->layer_id = 0;
	cfg->layer_cfg.layer_id = 0;

	cfg->layer_cfg.info.fb.format = format;
	cfg->layer_cfg.info.fb.size[0].width = width;
	cfg->layer_cfg.info.fb.size[0].height = height;
	cfg->layer_cfg.info.fb.crop.x = 0;
	cfg->layer_cfg.info.fb.crop.y = 0;
	cfg->layer_cfg.info.fb.crop.width = width;
	cfg->layer_cfg.info.fb.crop.height = height;
	cfg->layer_cfg.info.fb.align[0] = 4;
	cfg->layer_cfg.info.mode = 0;
	cfg->layer_cfg.info.alpha_mode = 1;
	cfg->layer_cfg.info.alpha_value = 255;
	cfg->layer_cfg.info.zorder = 0;
	cfg->layer_cfg.info.screen_win.x = 0;
	cfg->layer_cfg.info.screen_win.y = 0;

	disp_layer_cfg(cfg);
	msleep(1000);
	disp_layer_clear(g_screen_index, channel, 0);
	disp_mem_clear(0);
	return 0;
}

int disp_layer_format_test(int argc, char **argv)
{
	struct test_layer_cfg test_cfg;
	int i, sw, sh;
	/*main test start*/

	if (argc == 2) {
		g_screen_index = atoi(argv[1]);
	} else
		g_screen_index = 0;

	disp_layer_clear_all(g_screen_index);

	printf("request mem_id 0\n");

	printf("Start show layer\n");

	memset(&test_cfg, 0, sizeof(struct test_layer_cfg));
	show_layer(&test_cfg, 480, 320, 0, 3, "./pic/bike_480x320_150.bin");
	show_layer(&test_cfg, 640, 480, 0, 3, "./pic/bike_640x480_150.bin");
	show_layer(&test_cfg, 720, 480, 0, 3,"./pic/bike_720x480_150.bin");
	show_layer(&test_cfg, 720, 576, 0, 3, "./pic/bike_720x576_150.bin");
	show_layer(&test_cfg, 960, 640, 0, 3, "./pic/bike_960x640_150.bin");
	show_layer(&test_cfg, 1024, 600, 0, 3, "./pic/bike_1024x600_150.bin");
	show_layer(&test_cfg, 1024, 720, 0, 3, "./pic/bike_1024x720_150.bin");
	show_layer(&test_cfg, 1024, 768, 0, 3, "./pic/bike_1024x768_150.bin");
	show_layer(&test_cfg, 1280, 720, 0, 3, "./pic/bike_1280x720_150.bin");
	show_layer(&test_cfg, 500, 312, 0, 0, "./pic/argb8888.bin");
	show_layer(&test_cfg, 500, 312, 0, 1, "./pic/abgr8888.bin");
	show_layer(&test_cfg, 500, 312, 0, 2, "./pic/rgba8888.bin");
	show_layer(&test_cfg, 500, 312, 0, 3, "./pic/bgra8888.bin");

	show_layer(&test_cfg, 480, 320, 1, 3, "./pic/bike_480x320_150.bin");
	show_layer(&test_cfg, 640, 480, 1, 3, "./pic/bike_640x480_150.bin");
	show_layer(&test_cfg, 720, 480, 1, 3, "./pic/bike_720x480_150.bin");
	show_layer(&test_cfg, 720, 576, 1, 3,"./pic/bike_720x576_150.bin");
	show_layer(&test_cfg, 960, 640, 1, 3, "./pic/bike_960x640_150.bin");
	show_layer(&test_cfg, 1024, 600, 1, 3, "./pic/bike_1024x600_150.bin");
	show_layer(&test_cfg, 1024, 720, 1, 3, "./pic/bike_1024x720_150.bin");
	show_layer(&test_cfg, 1024, 768, 1, 3, "./pic/bike_1024x768_150.bin");
	show_layer(&test_cfg, 1280, 720, 1, 3, "./pic/bike_1280x720_150.bin");

	show_layer(&test_cfg, 500, 312, 1, 0, "./pic/argb8888.bin");
	show_layer(&test_cfg, 500, 312, 1, 1, "./pic/abgr8888.bin");
	show_layer(&test_cfg, 500, 312, 1, 2, "./pic/rgba8888.bin");
	show_layer(&test_cfg, 500, 312, 1, 3, "./pic/bgra8888.bin");

	show_layer(&test_cfg, 320, 240, 1, 72, "./pic/bike_320x240_020.bin");
	show_layer(&test_cfg, 640, 480, 0, 72, "./pic/bike_640x480_020.bin");
	show_layer(&test_cfg, 1280, 720, 0, 72, "./pic/bike_1280x720_020.bin");
	show_layer(&test_cfg, 1920, 1080, 0, 72, "./pic/bike_1920x1080_020.bin");

	show_layer(&test_cfg, 480, 320, 0, 76, "./pic/bike_480x320_220.bin");
	show_layer(&test_cfg, 640, 480, 0, 76, "./pic/bike_640x480_220.bin");
	show_layer(&test_cfg, 720, 480, 0, 76, "./pic/bike_720x480_220.bin");
	show_layer(&test_cfg, 720, 576, 0, 76, "./pic/bike_720x576_220.bin");
	show_layer(&test_cfg, 960, 640, 0, 76, "./pic/bike_960x640_220.bin");
	show_layer(&test_cfg, 1024, 600, 0, 76, "./pic/bike_1024x600_220.bin");
	show_layer(&test_cfg, 1024, 720, 0, 76, "./pic/bike_1024x720_220.bin");
	show_layer(&test_cfg, 1024, 768, 0, 76, "./pic/bike_1024x768_220.bin");
	show_layer(&test_cfg, 1920, 1080, 0, 76, "./pic/bike_1920x1080_220.bin");
	show_layer(&test_cfg, 1920, 1200, 0, 76, "./pic/bike_1920x1200_220.bin");

	show_layer(&test_cfg, 500, 312, 0, 16, "./pic/argb1555_500x312_ps0_ARGB.bin");

	show_layer(&test_cfg, 500, 312, 0, 16, "./pic/argb1555.bin");

	show_layer(&test_cfg, 500, 312, 0, 17, "./pic/abgr1555.bin");
	show_layer(&test_cfg, 500, 312, 0, 18, "./pic/rgba5551.bin");
	show_layer(&test_cfg, 500, 312, 0, 19, "./pic/bgra5551.bin");
	show_layer(&test_cfg, 500, 312, 0, 12, "./pic/argb4444_ps0_500x312_ARGB.bin");
	show_layer(&test_cfg, 500, 312, 0, 12, "./pic/argb4444.bin");
	show_layer(&test_cfg, 500, 312, 0, 13, "./pic/abgr4444.bin");
	show_layer(&test_cfg, 500, 312, 0, 14, "./pic/rgba4444.bin");
	show_layer(&test_cfg, 500, 312, 0, 15, "./pic/bgra4444.bin");
	show_layer(&test_cfg, 500, 312, 0, 10, "./pic/rgb565_ps0_500x312_ARGB.bin");
	show_layer(&test_cfg, 500, 312, 0, 10, "./pic/rgb565.bin");
	show_layer(&test_cfg, 500, 312, 0, 11, "./pic/bgr565.bin");

	disp_layer_clear_all(g_screen_index);
	msleep(100);

	return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(disp_layer_format_test, __cmd_disp_layer_format_test, disp_layer_format_test);
