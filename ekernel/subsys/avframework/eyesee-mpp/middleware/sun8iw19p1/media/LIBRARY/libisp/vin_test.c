
/*
 ******************************************************************************
 *
 * snapshot.c
 *
 * Hawkview ISP - snapshot.c module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   3.0		  Yang Feng   	2016/03/17	VIDEO INPUT
 *
 *****************************************************************************
 */

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <device/video.h>
#include <device/isp_dev.h>

#define MAX_VIDEO_NUM 	4
#define MEDIA_DEVICE "/dev/media0"

struct hw_isp_media_dev *media = NULL;

char path_name[30];
int save_cnt = 20;
int test_count = 1;
int fps_cnt = 30;
int wdr_mode_t = 0;

struct vi_info {
	int Chn;
	int s32MilliSec;
	struct video_fmt vfmt;
	struct video_buffer buffer;
};

static void *loop_cap(void *pArg)
{
	struct vi_info *privCap = (struct vi_info *)pArg;
	struct isp_video_device *video = NULL;
	struct buffers_pool *pool = NULL;
	int i = 0;

	printf("current vi channel is = %d\n", privCap->Chn);

	if (privCap->Chn >= HW_VIDEO_DEVICE_NUM || NULL == media->video_dev[privCap->Chn]) {
		ISP_ERR("vi channel %d is invalid!\n", privCap->Chn);
		return NULL;
	} else {
		video = media->video_dev[privCap->Chn];
	}

	pool = buffers_pool_new(video);
	if (NULL == pool)
		return NULL;

	if (video_req_buffers(video, pool) < 0)
		return NULL;
	video_get_fmt(video, &privCap[privCap->Chn].vfmt);
	for (i = 0; i < privCap[privCap->Chn].vfmt.nbufs; i++)
		video_queue_buffer(video, i);

	if (video_stream_on(video) < 0)
		return NULL;

	i = 0;
	while (i < save_cnt) {
#if 0
		video_set_control(video, V4L2_CID_BRIGHTNESS, i%128);
		video_set_control(video, V4L2_CID_CONTRAST, i%128);
		video_set_control(video, V4L2_CID_SATURATION, i%4);
		video_set_control(video, V4L2_CID_HUE, i%128);
		video_set_control(video, V4L2_CID_AUTO_WHITE_BALANCE, i%2);
		video_set_control(video, V4L2_CID_EXPOSURE, i%128);
		video_set_control(video, V4L2_CID_AUTOGAIN, i%2);
		video_set_control(video, V4L2_CID_GAIN, i%128+16);
		video_set_control(video, V4L2_CID_POWER_LINE_FREQUENCY, i%4);
		video_set_control(video, V4L2_CID_HUE_AUTO, i%2);
		video_set_control(video, V4L2_CID_WHITE_BALANCE_TEMPERATURE, 2800+i%6500);
		video_set_control(video, V4L2_CID_SHARPNESS, i%64 - 32);
		video_set_control(video, V4L2_CID_CHROMA_AGC, i%2);
		video_set_control(video, V4L2_CID_COLORFX, i%16);

		video_set_control(video, V4L2_CID_AUTOBRIGHTNESS, i%2);
		video_set_control(video, V4L2_CID_BAND_STOP_FILTER, i%2);
		video_set_control(video, V4L2_CID_ILLUMINATORS_1, i%2);
		video_set_control(video, V4L2_CID_ILLUMINATORS_2, i%2);

		video_set_control(video, V4L2_CID_EXPOSURE_AUTO, i%4);
		video_set_control(video, V4L2_CID_EXPOSURE_ABSOLUTE, i%1000000);

		video_set_control(video, V4L2_CID_EXPOSURE_AUTO_PRIORITY, i%2);
		video_set_control(video, V4L2_CID_FOCUS_ABSOLUTE, i%128);
		video_set_control(video, V4L2_CID_FOCUS_RELATIVE, i%128);
		video_set_control(video, V4L2_CID_FOCUS_AUTO, i%2);

		video_set_control(video, V4L2_CID_AUTO_EXPOSURE_BIAS, i%4);
		video_set_control(video, V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE, i%10);

		video_set_control(video, V4L2_CID_WIDE_DYNAMIC_RANGE, i%2);
		video_set_control(video, V4L2_CID_IMAGE_STABILIZATION, i%2);

		video_set_control(video, V4L2_CID_ISO_SENSITIVITY, i%4);
		video_set_control(video, V4L2_CID_ISO_SENSITIVITY_AUTO, i%2);

		video_set_control(video, V4L2_CID_EXPOSURE_METERING, i%4);
		video_set_control(video, V4L2_CID_SCENE_MODE, i%2);

		video_set_control(video, V4L2_CID_3A_LOCK, i%8);
		video_set_control(video, V4L2_CID_AUTO_FOCUS_START, i%1);
		video_set_control(video, V4L2_CID_AUTO_FOCUS_STOP, i%1);
		video_set_control(video, V4L2_CID_AUTO_FOCUS_RANGE, i%4);

		video_set_control(video, V4L2_CID_HFLIP, i%2);
		video_set_control(video, V4L2_CID_VFLIP, i%2);
#endif
		if (video_wait_buffer(video, privCap->s32MilliSec) < 0)
			continue;

		if (video_dequeue_buffer(video, &privCap->buffer) < 0)
			goto disablech;

		printf("process channel(%d) frame %d\r\n", privCap->Chn, i);
		if (save_cnt < 10000) {
			if (test_count < 10) {
				if (video_save_frames(video, privCap->buffer.index, path_name) < 0)
				goto disablech;
			}
		} else {
			i = i % save_cnt;
		}
		if (video_queue_buffer(video, privCap->buffer.index) < 0)
			goto disablech;
		i++;
	}

disablech:
	if (video_stream_off(video) < 0)
		return NULL;
	if (video_free_buffers(video) < 0)
		return NULL;
	buffers_pool_delete(video);
vi_exit:
	return NULL;
}

int main_test(int ch_num, int width, int height, int out_fmt)
{
	pthread_t thid[MAX_VIDEO_NUM];
	int ret, i, ch = -1;
	struct vi_info privCap[MAX_VIDEO_NUM];
	pthread_attr_t attr;

	if (media == NULL) {
		media = isp_md_open(MEDIA_DEVICE);
		if (media == NULL) {
			ISP_PRINT("unable to open media device %s\n", MEDIA_DEVICE);
			return -1;
		}
	} else {
		ISP_PRINT("mpi_vi already init\n");
	}

	if (ch_num > MAX_VIDEO_NUM)
		ch_num = MAX_VIDEO_NUM;

	if (ch_num == 0 || ch_num == 1) {
		ch = ch_num;
		ch_num = 2;
	}

	for(i = 0; i < ch_num; i++) {
		if (i != ch && ch != -1)
			continue;
		memset(&privCap[i], 0, sizeof(struct vi_info));
		/*Set Dev ID and Chn ID*/
		privCap[i].Chn = i;
		privCap[i].s32MilliSec = 2000;

		privCap[i].vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
		privCap[i].vfmt.memtype = V4L2_MEMORY_MMAP;
		switch (out_fmt) {
		case 0: privCap[i].vfmt.format.pixelformat = V4L2_PIX_FMT_SBGGR8; break;
		case 1: privCap[i].vfmt.format.pixelformat = V4L2_PIX_FMT_YUV420M; break;
		case 2: privCap[i].vfmt.format.pixelformat = V4L2_PIX_FMT_YUV420; break;
		case 3: privCap[i].vfmt.format.pixelformat = V4L2_PIX_FMT_NV12M; break;
		default: privCap[i].vfmt.format.pixelformat = V4L2_PIX_FMT_YUV420M; break;
		}
		privCap[i].vfmt.format.field = V4L2_FIELD_NONE;
		privCap[i].vfmt.format.width = width;
		privCap[i].vfmt.format.height = height;
		privCap[i].vfmt.nbufs = 5;
		privCap[i].vfmt.nplanes = 3;
		privCap[i].vfmt.fps = fps_cnt;
		privCap[i].vfmt.capturemode = V4L2_MODE_VIDEO;
		privCap[i].vfmt.use_current_win = 0;
		privCap[i].vfmt.wdr_mode = wdr_mode_t;

		if (isp_video_open(media, i) < 0) {
			printf("isp_video_open vi%d failed\n", i);
			return -1;
		}

		if (video_set_fmt(media->video_dev[i], &privCap[i].vfmt) < 0) {
			printf("video_set_fmt failed\n");
			return -1;
		}

		video_get_fmt(media->video_dev[i], &privCap[i].vfmt);

		/*Call Video Thread*/
		attr.stackaddr = NULL;
		attr.stacksize = 0x3000;
		attr.inheritsched = PTHREAD_INHERIT_SCHED;
		attr.schedpolicy = SCHED_FIFO;
		attr.schedparam.sched_priority = 22;
		attr.detachstate = PTHREAD_CREATE_JOINABLE;
		ret = pthread_create(&thid[i], &attr, loop_cap, (void *)&privCap[i]);
		if (ret < 0) {
			printf("pthread_create loop_cap Chn[%d] failed.\n", i);
			continue;
		}
	}

	for(i = 0; i < ch_num; i++) {
		if (i != ch && ch != -1)
			continue;
		printf("video%d wait to exit!!!\n", i);
		pthread_join(thid[i], NULL);
	}

	for(i = 0; i < ch_num; i++) {
		if (i != ch && ch != -1)
			continue;
		isp_video_close(media, i);
	}

	if (media)
		isp_md_close(media);
	media = NULL;

	return 0;
}

int cmd_vin_test(int argc __attribute__((__unused__)), char *argv[] __attribute__((__unused__)))
{
	int ret, n = 0;
	int ch_num = 0;
	int width = 640;
	int height = 480;
	int out_fmt = 1;

	memset(path_name, 0, sizeof (path_name));

	if (argc == 1) {
		sprintf(path_name, "/mnt/sdcard");
	} else if (argc == 2) {
		sprintf(path_name, "/mnt/sdcard");
		ch_num = atoi(argv[1]);
	} else if (argc == 4) {
		ch_num = atoi(argv[1]);
		width = atoi(argv[2]);
		height = atoi(argv[3]);
		sprintf(path_name, "/mnt/sdcard");
	} else if (argc == 5) {
		ch_num = atoi(argv[1]);
		width = atoi(argv[2]);
		height = atoi(argv[3]);
		sprintf(path_name, "%s", argv[4]);
	} else if (argc == 6) {
		ch_num = atoi(argv[1]);
		width = atoi(argv[2]);
		height = atoi(argv[3]);
		sprintf(path_name, "%s", argv[4]);
		out_fmt = atoi(argv[5]);
	} else if (argc == 7) {
		ch_num = atoi(argv[1]);
		width = atoi(argv[2]);
		height = atoi(argv[3]);
		sprintf(path_name, "%s", argv[4]);
		out_fmt = atoi(argv[5]);
		save_cnt = atoi(argv[6]);
	} else if (argc == 8) {
		ch_num = atoi(argv[1]);
		width = atoi(argv[2]);
		height = atoi(argv[3]);
		sprintf(path_name, "%s", argv[4]);
		out_fmt = atoi(argv[5]);
		save_cnt = atoi(argv[6]);
		test_count = atoi(argv[7]);
	} else if (argc == 9) {
		ch_num = atoi(argv[1]);
		width = atoi(argv[2]);
		height = atoi(argv[3]);
		sprintf(path_name, "%s", argv[4]);
		out_fmt = atoi(argv[5]);
		save_cnt = atoi(argv[6]);
		test_count = atoi(argv[7]);
		fps_cnt = atoi(argv[8]);
	} else if (argc == 10) {
		ch_num = atoi(argv[1]);
		width = atoi(argv[2]);
		height = atoi(argv[3]);
		sprintf(path_name, "%s", argv[4]);
		out_fmt = atoi(argv[5]);
		save_cnt = atoi(argv[6]);
		test_count = atoi(argv[7]);
		fps_cnt = atoi(argv[8]);
		wdr_mode_t = atoi(argv[9]);
	} else {
		printf("please select the ch_num: 1~4 ......\n");
		scanf("%d", &ch_num);

		printf("please input the resolution: width height......\n");
		scanf("%d %d", &width, &height);

		printf("please input the frame saving path......\n");
		scanf("%15s", path_name);

		printf("please input the test out_fmt: 0~3......\n");
		scanf("%d", &out_fmt);

		printf("please input the save_cnt: >=1......\n");
		scanf("%d", &save_cnt);
	}

	for (n = 0; n < test_count; n++) {
		ret = main_test(ch_num, width, height, out_fmt);
		printf("vin isp test %d, return %d\n", n + 1, ret);
	}

	return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_vin_test, __cmd_vin_test, vin test);

