#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <rtthread.h>
#include <hal_mem.h>
#include <log.h>
#include <video/sunxi_display2.h>
#include "disp_layer_cfg.h"
#include "disp_mem.h"
int msleep(unsigned int msecs);

int disp_layer_clear(u32 screen_index, u32 chn, u32 layer)
{
	rt_device_t dispfh = NULL;
	struct disp_layer_config config;
	unsigned long arg[6];
	int ret = -1, i ,j;

	dispfh = rt_device_find("disp");
	if (!dispfh) {
		printf("open display device fail!\n");
		return -1;
	}

	memset(&config, 0, sizeof(struct disp_layer_config));

	arg[0] = screen_index;
	arg[1] = (unsigned long)&config;
	arg[2] = 1;
	arg[3] = 0;

	config.enable = false;
	config.channel = chn;
	config.layer_id = layer;
	ret = dispfh->control(dispfh, DISP_LAYER_SET_CONFIG,
			      (void *)arg);
	msleep(10);
	if (0 != ret)
		printf("fail to set layer cfg\n");


	return ret;

}

int disp_layer_clear_all(u32 screen_index)
{
	rt_device_t dispfh = NULL;
	struct disp_layer_config config;
	unsigned long arg[6];
	int ret = -1, i ,j;

	dispfh = rt_device_find("disp");
	if (!dispfh) {
		printf("open display device fail!\n");
		return -1;
	}

	memset(&config, 0, sizeof(struct disp_layer_config));

	arg[0] = screen_index;
	arg[1] = (unsigned long)&config;
	arg[2] = 1;
	arg[3] = 0;

	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 4; ++j) {
			config.enable = false;
			config.channel = i;
			config.layer_id = j;
			ret = dispfh->control(dispfh, DISP_LAYER_SET_CONFIG,
					      (void *)arg);
			msleep(10);
			if (0 != ret)
				printf("fail to set layer cfg\n");
		}
	}


	return ret;

}
int disp_layer_get_resolution(u32 screen_index, u32 *width, u32 *height)
{
	rt_device_t dispfh;
	unsigned long arg[6];

	if (!width || !height) {
		printf("NULL pointer!\n");
		return -1;
	}

	dispfh = rt_device_find("disp");
	if (!dispfh) {
		printf("open display device fail!\n");
		return -1;
	}

	arg[0] = screen_index;
	*width = dispfh->control(dispfh, DISP_GET_SCN_WIDTH, (void *)arg);
	*height = dispfh->control(dispfh, DISP_GET_SCN_HEIGHT, (void *)arg);

	return 0;
}

int disp_layer_cfg(struct test_layer_cfg *test_cfg)
{
	unsigned long arg[6];
	int id, width, height, ret, sw, sh, fb_width, fb_height, i, n;

	if (!test_cfg) {
		printf("Null pointer!\n");
		return -1;
	}

	if (1 == test_cfg->fullscreen) {
		/* full-screen display */
		printf("full screen display\n");
	}

	test_cfg->dispfh = rt_device_find("disp");
	if (!test_cfg->dispfh) {
		printf("open display device fail!\n");
		return -1;
	}

	if (test_cfg->clear) {
		printf("clear \n");

		test_cfg->layer_cfg.enable = 0;
		arg[0] = test_cfg->screen_id;
		arg[1] = (unsigned long)&test_cfg->layer_cfg;
		arg[2] = 1;
		arg[3] = 0;
		test_cfg->dispfh->control(test_cfg->dispfh,
					  DISP_LAYER_SET_CONFIG, (void *)arg);
		test_cfg->dispfh->close(test_cfg->dispfh);
		return 0;
	}

	arg[0] = test_cfg->screen_id;
	test_cfg->width = test_cfg->dispfh->control(
	    test_cfg->dispfh, DISP_GET_SCN_WIDTH, (void *)arg);
	test_cfg->height = test_cfg->dispfh->control(
	    test_cfg->dispfh, DISP_GET_SCN_HEIGHT, (void *)arg);

	fb_width = sw = test_cfg->layer_cfg.info.fb.size[0].width;
	fb_height = sh = test_cfg->layer_cfg.info.fb.size[0].height;

	if (test_cfg->layer_cfg.info.fb.flags & DISP_BF_STEREO_FP) {
		fb_width = fb_width;
		fb_height = fb_height / 2;
	}

	arg[0] = test_cfg->mem_id;
	arg[1] = 0;
	arg[2] = 0;
	arg[3] = 0;
	test_cfg->layer_cfg.info.fb.addr[0] = disp_mem_getadr(test_cfg->mem_id);

	test_cfg->layer_cfg.info.fb.size[0].width = fb_width;
	test_cfg->layer_cfg.info.fb.size[0].height = fb_height;
	if (test_cfg->layer_cfg.info.fb.format == DISP_FORMAT_YUV444_P) {

		test_cfg->layer_cfg.info.fb.addr[0] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0]);
		test_cfg->layer_cfg.info.fb.addr[1] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0] +
			  fb_width * fb_height);
		test_cfg->layer_cfg.info.fb.addr[2] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0] +
			  fb_width * fb_height * 2);
		test_cfg->layer_cfg.info.fb.trd_right_addr[0] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0] +
			  fb_width * fb_height * 3);
		test_cfg->layer_cfg.info.fb.trd_right_addr[1] =
		    (int)(test_cfg->layer_cfg.info.fb.trd_right_addr[0] +
			  fb_width * fb_height);
		test_cfg->layer_cfg.info.fb.trd_right_addr[2] =
		    (int)(test_cfg->layer_cfg.info.fb.trd_right_addr[0] +
			  fb_width * fb_height * 2);

		test_cfg->layer_cfg.info.fb.size[1].width =
		    test_cfg->layer_cfg.info.fb.size[0].width;
		test_cfg->layer_cfg.info.fb.size[1].height =
		    test_cfg->layer_cfg.info.fb.size[0].height;
		test_cfg->layer_cfg.info.fb.size[2].width =
		    test_cfg->layer_cfg.info.fb.size[0].width;
		test_cfg->layer_cfg.info.fb.size[2].height =
		    test_cfg->layer_cfg.info.fb.size[0].height;

	} else if (test_cfg->layer_cfg.info.fb.format == DISP_FORMAT_YUV422_P ||
		   test_cfg->layer_cfg.info.fb.format ==
		       DISP_FORMAT_YUV422_SP_UVUV ||
		   test_cfg->layer_cfg.info.fb.format ==
		       DISP_FORMAT_YUV422_SP_VUVU ||
		   test_cfg->layer_cfg.info.fb.format ==
		       DISP_FORMAT_YUV422_SP_UVUV ||
		   test_cfg->layer_cfg.info.fb.format ==
		       DISP_FORMAT_YUV422_SP_VUVU) {

		test_cfg->layer_cfg.info.fb.addr[0] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0]);
		test_cfg->layer_cfg.info.fb.addr[1] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0] +
			  fb_width * fb_height);
		test_cfg->layer_cfg.info.fb.addr[2] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0] +
			  fb_width * fb_height * 3 / 2);
		test_cfg->layer_cfg.info.fb.trd_right_addr[0] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0] +
			  fb_width * fb_height * 2);
		test_cfg->layer_cfg.info.fb.trd_right_addr[1] =
		    (int)(test_cfg->layer_cfg.info.fb.trd_right_addr[0] +
			  fb_width * fb_height);
		test_cfg->layer_cfg.info.fb.trd_right_addr[2] =
		    (int)(test_cfg->layer_cfg.info.fb.trd_right_addr[0] +
			  fb_width * fb_height * 3 / 2);

		test_cfg->layer_cfg.info.fb.size[1].width =
		    test_cfg->layer_cfg.info.fb.size[0].width / 2;
		test_cfg->layer_cfg.info.fb.size[1].height =
		    test_cfg->layer_cfg.info.fb.size[0].height;
		test_cfg->layer_cfg.info.fb.size[2].width =
		    test_cfg->layer_cfg.info.fb.size[0].width / 2;
		test_cfg->layer_cfg.info.fb.size[2].height =
		    test_cfg->layer_cfg.info.fb.size[0].height;

	} else if (test_cfg->layer_cfg.info.fb.format == DISP_FORMAT_YUV411_P ||
		   test_cfg->layer_cfg.info.fb.format ==
		       DISP_FORMAT_YUV411_SP_UVUV ||
		   test_cfg->layer_cfg.info.fb.format ==
		       DISP_FORMAT_YUV411_SP_VUVU ||
		   test_cfg->layer_cfg.info.fb.format ==
		       DISP_FORMAT_YUV411_SP_UVUV ||
		   test_cfg->layer_cfg.info.fb.format ==
		       DISP_FORMAT_YUV411_SP_VUVU) {

		test_cfg->layer_cfg.info.fb.addr[0] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0]);
		test_cfg->layer_cfg.info.fb.addr[1] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0] +
			  fb_width * fb_height);
		test_cfg->layer_cfg.info.fb.addr[2] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0] +
			  fb_width * fb_height * 5 / 4);
		test_cfg->layer_cfg.info.fb.trd_right_addr[0] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0] +
			  fb_width * fb_height * 3 / 2);
		test_cfg->layer_cfg.info.fb.trd_right_addr[1] =
		    (int)(test_cfg->layer_cfg.info.fb.trd_right_addr[0] +
			  fb_width * fb_height);
		test_cfg->layer_cfg.info.fb.trd_right_addr[2] =
		    (int)(test_cfg->layer_cfg.info.fb.trd_right_addr[0] +
			  fb_width * fb_height * 5 / 4);

		test_cfg->layer_cfg.info.fb.size[1].width =
		    test_cfg->layer_cfg.info.fb.size[0].width / 4;
		test_cfg->layer_cfg.info.fb.size[1].height =
		    test_cfg->layer_cfg.info.fb.size[0].height;
		test_cfg->layer_cfg.info.fb.size[2].width =
		    test_cfg->layer_cfg.info.fb.size[0].width / 4;
		test_cfg->layer_cfg.info.fb.size[2].height =
		    test_cfg->layer_cfg.info.fb.size[0].height;

	} else if (test_cfg->layer_cfg.info.fb.format == DISP_FORMAT_YUV420_P ||
		   test_cfg->layer_cfg.info.fb.format ==
		       DISP_FORMAT_YUV420_SP_UVUV ||
		   test_cfg->layer_cfg.info.fb.format ==
		       DISP_FORMAT_YUV420_SP_VUVU ||
		   test_cfg->layer_cfg.info.fb.format ==
		       DISP_FORMAT_YUV420_SP_UVUV ||
		   test_cfg->layer_cfg.info.fb.format ==
		       DISP_FORMAT_YUV420_SP_VUVU) {

		test_cfg->layer_cfg.info.fb.addr[0] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0]);
		test_cfg->layer_cfg.info.fb.addr[1] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0] +
			  fb_width * fb_height);
		test_cfg->layer_cfg.info.fb.addr[2] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0] +
			  fb_width * fb_height * 5 / 4);
		test_cfg->layer_cfg.info.fb.trd_right_addr[0] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0] +
			  fb_width * fb_height * 3 / 2);
		test_cfg->layer_cfg.info.fb.trd_right_addr[1] =
		    (int)(test_cfg->layer_cfg.info.fb.trd_right_addr[0] +
			  fb_width * fb_height);
		test_cfg->layer_cfg.info.fb.trd_right_addr[2] =
		    (int)(test_cfg->layer_cfg.info.fb.trd_right_addr[0] +
			  fb_width * fb_height * 5 / 4);

		test_cfg->layer_cfg.info.fb.size[1].width =
		    test_cfg->layer_cfg.info.fb.size[0].width / 2;
		test_cfg->layer_cfg.info.fb.size[1].height =
		    test_cfg->layer_cfg.info.fb.size[0].height / 2;
		test_cfg->layer_cfg.info.fb.size[2].width =
		    test_cfg->layer_cfg.info.fb.size[0].width / 2;
		test_cfg->layer_cfg.info.fb.size[2].height =
		    test_cfg->layer_cfg.info.fb.size[0].height / 2;
	} else {
		/* INTERLEAVED */
		test_cfg->layer_cfg.info.fb.addr[0] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0] +
			  fb_width * fb_height / 3 * 0);
		test_cfg->layer_cfg.info.fb.addr[1] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0] +
			  fb_width * fb_height / 3 * 1);
		test_cfg->layer_cfg.info.fb.addr[2] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0] +
			  fb_width * fb_height / 3 * 2);
		test_cfg->layer_cfg.info.fb.trd_right_addr[0] =
		    (int)(test_cfg->layer_cfg.info.fb.addr[0] +
			  fb_width * fb_height * 3 / 2);
		test_cfg->layer_cfg.info.fb.trd_right_addr[1] =
		    (int)(test_cfg->layer_cfg.info.fb.trd_right_addr[0] +
			  fb_width * fb_height);
		test_cfg->layer_cfg.info.fb.trd_right_addr[2] =
		    (int)(test_cfg->layer_cfg.info.fb.trd_right_addr[0] +
			  fb_width * fb_height * 3 / 2);

		test_cfg->layer_cfg.info.fb.size[1].width =
		    test_cfg->layer_cfg.info.fb.size[0].width;
		test_cfg->layer_cfg.info.fb.size[1].height =
		    test_cfg->layer_cfg.info.fb.size[0].height;
		test_cfg->layer_cfg.info.fb.size[2].width =
		    test_cfg->layer_cfg.info.fb.size[0].width;
		test_cfg->layer_cfg.info.fb.size[2].height =
		    test_cfg->layer_cfg.info.fb.size[0].height;
	}

	if ((0 == test_cfg->layer_cfg.info.screen_win.width) ||
	    (0 == test_cfg->layer_cfg.info.screen_win.height)) {
		if (1 == test_cfg->fullscreen) {
			/* full-screen display */
			printf("full screen display\n");
			test_cfg->layer_cfg.info.screen_win.width =
			    test_cfg->width;
			test_cfg->layer_cfg.info.screen_win.height =
			    test_cfg->height;
		} else {
			/* origin size display */
			/* cut out-of-screen part */
			if (test_cfg->layer_cfg.info.fb.crop.width >
			    test_cfg->width)
				test_cfg->layer_cfg.info.fb.crop.width =
				    test_cfg->width;
			if (test_cfg->layer_cfg.info.fb.crop.height >
			    test_cfg->height)
				test_cfg->layer_cfg.info.fb.crop.height =
				    test_cfg->height;

			test_cfg->layer_cfg.info.screen_win.width =
			    (unsigned int)
				test_cfg->layer_cfg.info.fb.crop.width;
			test_cfg->layer_cfg.info.screen_win.height =
			    (unsigned int)
				test_cfg->layer_cfg.info.fb.crop.height;
		}
	}
	test_cfg->layer_cfg.info.fb.crop.x = test_cfg->layer_cfg.info.fb.crop.x
					     << 32;
	test_cfg->layer_cfg.info.fb.crop.y = test_cfg->layer_cfg.info.fb.crop.y
					     << 32;
	test_cfg->layer_cfg.info.fb.crop.width =
	    test_cfg->layer_cfg.info.fb.crop.width << 32;
	test_cfg->layer_cfg.info.fb.crop.height =
	    test_cfg->layer_cfg.info.fb.crop.height << 32;

	test_cfg->layer_cfg.enable = 1;
	arg[0] = test_cfg->screen_id;
	arg[1] = (unsigned long)&test_cfg->layer_cfg;
	arg[2] = 1;
	arg[3] = 0;
	ret = test_cfg->dispfh->control(test_cfg->dispfh, DISP_LAYER_SET_CONFIG,
					(void *)arg);
	if (0 != ret)
		printf("fail to set layer cfg\n");

	return 0;
}
