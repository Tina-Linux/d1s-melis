
/*
 * zw
 * for csi & isp test
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include <getopt.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <sys/time.h>
//#include <sys/mman.h>
//#include <sys/ioctl.h>
#include <pthread.h>

//#include <melis/ioctl.h>
#include <dfs_posix.h>
//#include <v4l2-types.h>

#include <finsh.h>
#include <finsh_api.h>
#include <shell.h>
#include "../sunxi_camera_v2.h"

#define CLEAR(x) (memset(&(x), 0, sizeof (x)))
#define ALIGN_4K(x) (((x) + (4095)) & ~(4095))
#define ALIGN_16B(x) (((x) + (15)) & ~(15))

#define MAX_VIDEO_NUM 4

struct buffer {
	void *start[3];
	int length[3];
};

struct video_info {
	int id;
	int fd;
	char name[20];
	struct buffer *buffers;
	unsigned int width;
	unsigned int height;
	unsigned int nbuffers;
	unsigned int nplanes;
	unsigned int count;
};

static char path_name[20];
struct video_info info[MAX_VIDEO_NUM];

unsigned int out_fmt = 1;
unsigned int read_num = 20;
unsigned int req_frame_num = 5;
unsigned int fps = 30;
unsigned int wdr_mode = 0;

static int read_frame(int id)
{
	struct v4l2_buffer buf;
	char fdstr[50];
	int file_fd = 0;

	CLEAR(buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	buf.memory = V4L2_MEMORY_USERPTR;
	buf.length = info[id].nplanes;
	buf.m.planes = (struct v4l2_plane *)calloc(info[id].nplanes, sizeof(struct v4l2_plane));

	if (-1 == ioctl(info[id].fd, VIDIOC_DQBUF, &buf)) {
		free(buf.m.planes);
		return -1;
	}

	assert(buf.index < info[id].nbuffers);

	if ((info[id].count > 0) && (info[id].count % 4 == 0) && (read_num < 10000)) {
#if 1
		printf("file length = %d %d %d\n", info[id].buffers[buf.index].length[0],
		       info[id].buffers[buf.index].length[1],
		       info[id].buffers[buf.index].length[2]);
		printf("file start = %p %p %p\n", info[id].buffers[buf.index].start[0],
		       info[id].buffers[buf.index].start[1],
		       info[id].buffers[buf.index].start[2]);
#endif
		switch (info[id].nplanes) {
		case 1:
			sprintf(fdstr, "%s/fb%d_yuv%d_%d_%d.bin", path_name, id, out_fmt, info[id].width, info[id].height);
			file_fd = open(fdstr, O_WRONLY | O_CREAT | O_APPEND);
			write(file_fd, info[id].buffers[buf.index].start[0], info[id].buffers[buf.index].length[0]);
			close(file_fd);
			break;
		case 2:
			sprintf(fdstr, "%s/fb%d_yuv%d_%d_%d.bin", path_name, id, out_fmt, info[id].width, info[id].height);
			file_fd = open(fdstr, O_WRONLY | O_CREAT | O_APPEND);
			write(file_fd, info[id].buffers[buf.index].start[0], info[id].buffers[buf.index].length[0]);
			close(file_fd);
			file_fd = open(fdstr, O_WRONLY | O_CREAT | O_APPEND);
			write(file_fd, info[id].buffers[buf.index].start[1], info[id].buffers[buf.index].length[1]);
			close(file_fd);
			break;
		case 3:
			sprintf(fdstr, "%s/fb%d_yuv%d_%d_%d.bin", path_name, id, out_fmt, info[id].width, info[id].height);
			file_fd = open(fdstr, O_WRONLY | O_CREAT | O_APPEND);
			write(file_fd, info[id].buffers[buf.index].start[0], info[id].buffers[buf.index].length[0]);
			close(file_fd);
			file_fd = open(fdstr, O_WRONLY | O_CREAT | O_APPEND);
			write(file_fd, info[id].buffers[buf.index].start[1], info[id].buffers[buf.index].length[1]);
			close(file_fd);
			file_fd = open(fdstr, O_WRONLY | O_CREAT | O_APPEND);
			write(file_fd, info[id].buffers[buf.index].start[2], info[id].buffers[buf.index].length[2]);
			close(file_fd);
			break;
		default:
			break;
		}
	}

	if (-1 == ioctl(info[id].fd, VIDIOC_QBUF, &buf)) {
		free(buf.m.planes);
		return -1;
	}

	free(buf.m.planes);
	return 0;
}

static int req_frame_buffers(int id)
{
	unsigned int i;
	struct v4l2_requestbuffers req;

	CLEAR(req);
	req.count = req_frame_num;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	req.memory = V4L2_MEMORY_USERPTR;
	if (-1 == ioctl(info[id].fd, VIDIOC_REQBUFS, &req)) {
		printf("VIDIOC_REQBUFS error\n");
		return -1;
	}
	info[id].buffers = calloc(req.count, sizeof(struct buffer));

	for (info[id].nbuffers = 0; info[id].nbuffers < req.count; info[id].nbuffers++) {
		struct v4l2_buffer buf;
		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
		buf.memory = V4L2_MEMORY_USERPTR;
		buf.index = info[id].nbuffers;
		buf.length = info[id].nplanes;
		buf.m.planes = (struct v4l2_plane *)calloc(info[id].nplanes, sizeof(struct v4l2_plane));
		if (NULL == buf.m.planes) {
			printf("buf.m.planes calloc failed!\n");
			return -1;
		}
		if (-1 == ioctl(info[id].fd, VIDIOC_QUERYBUF, &buf)) {
			printf("VIDIOC_QUERYBUF error\n");
			free(buf.m.planes);
			return -1;
		}

		for (i = 0; i < info[id].nplanes; i++) {
			info[id].buffers[info[id].nbuffers].length[i] = buf.m.planes[i].length;
			info[id].buffers[info[id].nbuffers].start[i] = calloc(1, buf.m.planes[i].length);
			if (!info[id].buffers[info[id].nbuffers].start[i]) {
				printf("userptr malloc %d error\n", buf.m.planes[i].length);
				return -1;
			}
			buf.m.planes[i].m.userptr = info[id].buffers[info[id].nbuffers].start[i];
		}

		if (-1 == ioctl(info[id].fd, VIDIOC_QBUF, &buf)) {
			printf("VIDIOC_QBUF failed\n");
			free(buf.m.planes);
			return -1;
		}

		free(buf.m.planes);
	}

	return 0;
}

static int free_frame_buffers(int id)
{
	unsigned int i, j;

	for (i = 0; i < info[id].nbuffers; ++i) {
		for (j = 0; j < info[id].nplanes; j++)
			if (info[id].buffers[i].start[j])
				free(info[id].buffers[i].start[j]);
	}
	free(info[id].buffers);
	return 0;
}

static int camera_init(int id)
{
	struct v4l2_input inp;
	struct v4l2_streamparm parms;

	sprintf(info[id].name, "/dev/video%d", id);
	info[id].fd = open(info[id].name, O_RDWR /* required */  | O_NONBLOCK, 0);

	if (info[id].fd < 0) {
		printf("open %s falied\n", info[id].name);
		return -1;
	}
	printf("open %s fd = %d\n", info[id].name, info[id].fd);

	inp.index = 0;
	if (-1 == ioctl(info[id].fd, VIDIOC_S_INPUT, &inp)) {
		printf("VIDIOC_S_INPUT error!\n");
		return -1;
	}

	CLEAR(parms);
	parms.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	parms.parm.capture.timeperframe.numerator = 1;
	parms.parm.capture.timeperframe.denominator = fps;
	parms.parm.capture.capturemode = V4L2_MODE_VIDEO;
	parms.parm.capture.reserved[0] = 0;/*when different video have the same sensor source, 1:use sensor current win, 0:find the nearest win*/
	parms.parm.capture.reserved[1] = wdr_mode;/*2:command, 1: wdr, 0: normal*/

	if (-1 == ioctl(info[id].fd, VIDIOC_S_PARM, &parms)) {
		printf("VIDIOC_S_PARM error\n");
		return -1;
	}

	return 0;
}

static int camera_fmt_set(int id)
{
	struct v4l2_format fmt;

	CLEAR(fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	fmt.fmt.pix_mp.width = info[id].width;
	fmt.fmt.pix_mp.height = info[id].height;
	switch (out_fmt) {
	case 0: fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_SBGGR8; break;
	case 1: fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_YUV420M; break;
	case 2: fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_YUV420; break;
	case 3: fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_NV12M; break;
	case 4: fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_SBGGR10; break;
	case 5: fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_SBGGR12; break;
	default: fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_YUV420M; break;
	}
	fmt.fmt.pix_mp.field = V4L2_FIELD_NONE;

	if (-1 == ioctl(info[id].fd, VIDIOC_S_FMT, &fmt)) {
		printf("VIDIOC_S_FMT error!\n");
		return -1;
	}

	if (-1 == ioctl(info[id].fd, VIDIOC_G_FMT, &fmt)) {
		printf("VIDIOC_G_FMT error!\n");
		return -1;
	} else {
		info[id].nplanes = fmt.fmt.pix_mp.num_planes;
		printf("resolution got from sensor = %d*%d num_planes = %d\n",
		       fmt.fmt.pix_mp.width, fmt.fmt.pix_mp.height,
		       fmt.fmt.pix_mp.num_planes);
	}

	return 0;
}

static void *main_test(void *parg)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	int id = *(int *)parg;
	int ret = 0;

	if (-1 == camera_init(id)) {
		ret = -1;
		goto out;

	}

	if (-1 == camera_fmt_set(id)) {
		ret = -1;
		goto out;
	}

	if (-1 == req_frame_buffers(id)) {
		ret = -1;
		goto out;
	}

	if (-1 == ioctl(info[id].fd, VIDIOC_STREAMON, &type)) {
		printf("VIDIOC_STREAMON failed\n");
		ret = -1;
		goto out;
	} else {
		printf("VIDIOC_STREAMON ok\n");
	}

	info[id].count = read_num;

	while (info[id].count-- > 0) {
		for (;;) {
			fd_set fds;
			struct timeval tv;
			int r;

			FD_ZERO(&fds);
			FD_SET(info[id].fd, &fds);

			tv.tv_sec = 2;	/* Timeout. */
			tv.tv_usec = 0;
			r = select(info[id].fd + 1, &fds, NULL, NULL, &tv);

			if (-1 == r) {
				if (EINTR == errno)
					continue;
				printf("select err\n");
			}
			if (0 == r) {
				fprintf(stderr, "select timeout\n");
#ifdef TIMEOUT
				if (-1 == ioctl(info[id].fd, VIDIOC_STREAMOFF, &type))
					printf("VIDIOC_STREAMOFF failed\n");
				else
					printf("VIDIOC_STREAMOFF ok\n");
				free_frame_buffers(id);
				ret = -1;
				goto out;
#else
				continue;
#endif
			}

			if (!read_frame(id)) {
				break;
			} else {
				ret = -1;
				goto out;
			}
		}
	}

	if (-1 == ioctl(info[id].fd, VIDIOC_STREAMOFF, &type)) {
		printf("VIDIOC_STREAMOFF failed\n");
		ret = -1;
		goto out;
	} else {
		printf("VIDIOC_STREAMOFF ok\n");
	}

	free_frame_buffers(id);
out:

	if (ret == 0)
		printf("%s test done!!\n", info[id].name);
	else
		printf("%s test failed!!\n", info[id].name);

	sleep(1);

	close(info[id].fd);

	return NULL;
}

int cmd_csi_mop_userptr(int argc, char *argv[])
{
	int i, ret;
	int out_num = 0;
	int width = 640;
	int height = 480;
	pthread_t thid[MAX_VIDEO_NUM];
	int priv[MAX_VIDEO_NUM];
	pthread_attr_t attr[MAX_VIDEO_NUM];


#if 1 // init global variables
	memset(path_name, 0, 20);
	memset(info, 0, sizeof(struct video_info) * MAX_VIDEO_NUM);

	out_fmt = 1;
	read_num = 20;
	req_frame_num = 5;
	fps = 30;
	wdr_mode = 0;
#endif

	if (argc == 1) {
		sprintf(path_name, "/mnt/sdcard");
	} else if (argc == 2) {
		out_num = atoi(argv[1]);
		sprintf(path_name, "/mnt/sdcard");
	} else if (argc == 4) {
		out_num = atoi(argv[1]);
		width = atoi(argv[2]);
		height = atoi(argv[3]);
		sprintf(path_name, "/mnt/sdcard");
	} else if (argc == 5) {
		out_num = atoi(argv[1]);
		width = atoi(argv[2]);
		height = atoi(argv[3]);
		sprintf(path_name, "%s", argv[4]);
	} else if (argc == 6) {
		out_num = atoi(argv[1]);
		width = atoi(argv[2]);
		height = atoi(argv[3]);
		sprintf(path_name, "%s", argv[4]);
		out_fmt = atoi(argv[5]);
	} else if (argc == 7) {
		out_num = atoi(argv[1]);
		width = atoi(argv[2]);
		height = atoi(argv[3]);
		sprintf(path_name, "%s", argv[4]);
		out_fmt = atoi(argv[5]);
		read_num = atoi(argv[6]);
	} else if (argc == 8) {
		out_num = atoi(argv[1]);
		width = atoi(argv[2]);
		height = atoi(argv[3]);
		sprintf(path_name, "%s", argv[4]);
		out_fmt = atoi(argv[5]);
		read_num = atoi(argv[6]);
		fps = atoi(argv[7]);
	} else if (argc == 9) {
		out_num = atoi(argv[1]);
		width = atoi(argv[2]);
		height = atoi(argv[3]);
		sprintf(path_name, "%s", argv[4]);
		out_fmt = atoi(argv[5]);
		read_num = atoi(argv[6]);
		fps = atoi(argv[7]);
		wdr_mode = atoi(argv[8]);
	} else {
		printf("please select the total number of output ......\n");
		scanf("%d", &out_num);

		printf("please input the resolution: width height......\n");
		scanf("%d %d", &width, &height);

		printf("please input the frame saving path......\n");
		scanf("%15s", path_name);
	}

	if (out_num > MAX_VIDEO_NUM)
		out_num = MAX_VIDEO_NUM;

	for (i = 0; i < out_num; i++) {
		info[i].id = i;
		info[i].width = width;
		info[i].height = height;
		priv[i] = i;

#define DEFAULT_PRIORITY 14

		attr[i].stackaddr = NULL;
		attr[i].stacksize = 409600;
		attr[i].inheritsched = PTHREAD_INHERIT_SCHED;
		attr[i].schedpolicy = SCHED_FIFO;
		attr[i].schedparam.sched_priority = DEFAULT_PRIORITY;
		attr[i].detachstate = PTHREAD_CREATE_JOINABLE;

		ret = pthread_create(&thid[i], &attr[i], main_test, (void *)&priv[i]);

		if (ret < 0) {
			printf("pthread_create thread %d failed.\n", i);
			continue;
		} else {
			printf("pthread_create thread %d ok.\n", i);
		}
		sleep(1);
	}

	for(i = 0; i < out_num; i++) {
		pthread_join(thid[i], NULL);
	}

	return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_csi_mop_userptr, __cmd_csi_mop_userptr, csi multi output userptr);
