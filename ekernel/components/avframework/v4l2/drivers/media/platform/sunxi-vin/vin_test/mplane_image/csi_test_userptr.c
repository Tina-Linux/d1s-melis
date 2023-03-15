#include <unistd.h>
//#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dfs_posix.h>
#include <hal_cache.h>

//#include <rtthread.h>
//#include <v4l2-types.h>
#include <v4l2-dev.h>
#include <v4l2-device.h>

#include <finsh.h>
#include <finsh_api.h>
#include <shell.h>
#include <video/sunxi_display2.h>
#include <hal_mem.h>

#include "../sunxi_camera_v2.h"

#define display_frame 1

typedef enum {
	TVD_PL_YUV420 = 0,
	TVD_MB_YUV420 = 1,
	TVD_PL_YUV422 = 2,
} TVD_FMT_T;

struct disp_screen {
	int x;
	int y;
	int w;
	int h;
};

struct test_layer_info {
	int screen_id;
	int layer_id;
	int mem_id;
	struct disp_layer_config layer_config;
	int addr_map;
	int width, height;/* screen size */
	rt_device_t dispfh;/* device node handle */
	int fh;/* picture resource file handle */
	int mem;
	int clear;/* is clear layer */
	char filename[32];
	int full_screen;
	unsigned int pixformat;
	enum disp_output_type output_type;
};

/**
 * tvd_dev info
 */
struct tvd_dev {
	unsigned int ch_id;
	unsigned int height;
	unsigned int width;
	unsigned int interface;
	unsigned int system;
	unsigned int row;
	unsigned int column;
	unsigned int ch0_en;
	unsigned int ch1_en;
	unsigned int ch2_en;
	unsigned int ch3_en;
	unsigned int pixformat;
	struct test_layer_info layer_info;
	int frame_no_to_grap;
	FILE *raw_fp;
};
struct tvd_dev dev;


struct buffer {
	void *start[3];
	int length[3];
};

int fd;
int sel = 0;
int width = 1920;
int height = 1080;
int fps = 60;
#if 1
int mode = 2;
int test_cnt = 10000;
int save_flag = 2;
#else
int mode = 2;
int test_cnt = 20;
int save_flag = 1;
#endif

struct v4l2_input inp;
struct v4l2_streamparm parms;
struct v4l2_format fmt;

int nplanes = 0;

struct v4l2_requestbuffers req;
struct v4l2_exportbuffer exp;

int req_frame_num = 4;
int i = 0;
struct buffer *buffers;
static unsigned int n_buffers;

struct v4l2_buffer buf;

enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
unsigned int count = 0;

int file_flag = 1;
int flags = 0;

static struct disp_screen get_disp_screen(int w1, int h1, int w2, int h2)
{
	struct disp_screen screen;
	float r1, r2;

	r1 = (float)w1/(float)w2;
	r2 = (float)h1/(float)h2;
	if (r1 < r2) {
		screen.w = w2*r1;
		screen.h = h2*r1;
	} else {
		screen.w = w2*r2;
		screen.h = h2*r2;
	}

	screen.x = (w1 - screen.w)/2;
	screen.y = (h1 - screen.h)/2;

	return screen;
}


int csi_test_disp_layer_clear(void)
{
#if display_frame

	struct disp_layer_config config;
	unsigned long arg[6];
	int ret = -1, i ,j;

	dev.layer_info.dispfh = rt_device_find("disp");
	if (!dev.layer_info.dispfh) {
		printf("open display device fail!\n");
		return -1;
	}

	memset(&config, 0, sizeof(struct disp_layer_config));
	arg[0] = 0;
	arg[1] = (unsigned long)&config;
	arg[2] = 1;
	arg[3] = 0;

	config.enable = false;
	config.channel = 0;
	config.layer_id = 0;
	ret = dev.layer_info.dispfh->control(dev.layer_info.dispfh, DISP_LAYER_SET_CONFIG,
			      (void *)arg);

	memset(&config, 0, sizeof(struct disp_layer_config));
	arg[0] = 0;
	arg[1] = (unsigned long)&config;
	arg[2] = 1;
	arg[3] = 0;

	config.enable = false;
	config.channel = 2;
	config.layer_id = 0;
	ret = dev.layer_info.dispfh->control(dev.layer_info.dispfh, DISP_LAYER_SET_CONFIG,
			      (void *)arg);
	usleep(20000);
	if (0 != ret)
		printf("fail to set layer cfg\n");


	return ret;
#else
	return 0;
#endif
}

static int disp_init(int width, int height, unsigned int pixformat)
{
#if display_frame
	/* display_handle* disp = (display_handle*)display; */
	unsigned int arg[6] = {0};
	int layer_id = 0;
	struct disp_screen screen;

	dev.layer_info.screen_id = 0;

	if (dev.layer_info.screen_id < 0)
		return 0;

	csi_test_disp_layer_clear();
	/* open disp */
	arg[0] = dev.layer_info.screen_id;
	arg[1] = 1;
	arg[2] = 4;
	dev.layer_info.dispfh->control(
	    dev.layer_info.dispfh, DISP_DEVICE_SWITCH, (void *)arg);

	/* get current output type */
	arg[0] = dev.layer_info.screen_id;
	dev.layer_info.output_type = (enum disp_output_type)dev.layer_info.dispfh->control(dev.layer_info.dispfh,
			DISP_GET_OUTPUT_TYPE, (void *)arg);
	if (dev.layer_info.output_type == DISP_OUTPUT_TYPE_NONE) {
		printf("the output type is DISP_OUTPUT_TYPE_NONE %d\n",
		       dev.layer_info.output_type);
		return -1;
	}

	dev.layer_info.pixformat = pixformat;
	dev.layer_info.layer_config.channel = 0;
	dev.layer_info.layer_config.layer_id = layer_id;
	dev.layer_info.layer_config.info.zorder = 1;
	dev.layer_info.layer_config.info.alpha_mode = 1;
	dev.layer_info.layer_config.info.alpha_value = 0xff;
	dev.layer_info.width =
	    dev.layer_info.dispfh->control(dev.layer_info.dispfh, DISP_GET_SCN_WIDTH, (void *)arg);
	dev.layer_info.height =
	    dev.layer_info.dispfh->control(dev.layer_info.dispfh, DISP_GET_SCN_HEIGHT, (void *)arg);

	dev.layer_info.layer_config.info.mode = LAYER_MODE_BUFFER;

	if (dev.layer_info.pixformat == TVD_PL_YUV420)
		dev.layer_info.layer_config.info.fb.format = DISP_FORMAT_YUV420_P;  /*DISP_FORMAT_YUV420_P ---- V4L2_PIX_FMT_YUV420M*/
		    //DISP_FORMAT_YUV420_SP_UVUV;  /*DISP_FORMAT_YUV420_SP_UVUV ---- V4L2_PIX_FMT_NV12*/
	else
		dev.layer_info.layer_config.info.fb.format =
		    DISP_FORMAT_YUV422_SP_VUVU;

	if (dev.layer_info.full_screen == 0 && width < dev.layer_info.width &&
	    height < dev.layer_info.height) {
		dev.layer_info.layer_config.info.screen_win.x =
		    (dev.layer_info.width - width) / 2;
		dev.layer_info.layer_config.info.screen_win.y =
		    (dev.layer_info.height - height) / 2;
		if (!dev.layer_info.layer_config.info.screen_win.width) {
			dev.layer_info.layer_config.info.screen_win.width = width;
			dev.layer_info.layer_config.info.screen_win.height =
			    height;
		}
	} else {
		/* struct disp_screen screen; */
		screen = get_disp_screen(dev.layer_info.width, dev.layer_info.height,
				width, height);
		dev.layer_info.layer_config.info.screen_win.x = 0; /* screen.x; */
		dev.layer_info.layer_config.info.screen_win.y = 0; /* screen.y; */
		dev.layer_info.layer_config.info.screen_win.width =
		    dev.layer_info.width;
		dev.layer_info.layer_config.info.screen_win.height =
		    dev.layer_info.height;
		//printf("x: %d, y: %d, w: %d, h: %d\n",screen.x,screen.y,screen.w,screen.h);
	}
#endif
	return 0;
}

static int disp_set_addr(int width, int height, struct v4l2_buffer *buf)

{
	unsigned long arg[6];
	int ret;

	if (dev.layer_info.pixformat == TVD_PL_YUV420) {
		/* printf("******YUV420!\n"); */
		dev.layer_info.layer_config.info.fb.size[0].width = width;
		dev.layer_info.layer_config.info.fb.size[0].height = height;
		dev.layer_info.layer_config.info.fb.size[1].width = width / 2;
		dev.layer_info.layer_config.info.fb.size[1].height = height / 2;
		dev.layer_info.layer_config.info.fb.size[2].width = width / 2;
		dev.layer_info.layer_config.info.fb.size[2].height = height / 2;
		dev.layer_info.layer_config.info.fb.crop.width =
		    (unsigned long long)width << 32;
		dev.layer_info.layer_config.info.fb.crop.height =
		    (unsigned long long)height << 32;

		//dev.layer_info.layer_config.info.fb.addr[0] = buf->m.planes[0].m.mem_offset;
		//dev.layer_info.layer_config.info.fb.addr[1] = buf->m.planes[1].m.mem_offset;
		//dev.layer_info.layer_config.info.fb.addr[2] = buf->m.planes[2].m.mem_offset;

		/*V4L2_PIX_FMT_YUV420M*/
		//dev.layer_info.layer_config.info.fb.addr[0] = __va_to_pa(buf->m.planes[0].m.userptr);
		//dev.layer_info.layer_config.info.fb.addr[1] = __va_to_pa(buf->m.planes[1].m.userptr);
		//dev.layer_info.layer_config.info.fb.addr[2] = __va_to_pa(buf->m.planes[2].m.userptr);

		/*V4L2_PIX_FMT_YUV420*/
		hal_dcache_clean(buf->m.planes[0].m.userptr, buf->m.planes[0].length);
		dev.layer_info.layer_config.info.fb.addr[0] = __va_to_pa(buf->m.planes[0].m.userptr);
		dev.layer_info.layer_config.info.fb.addr[1] = __va_to_pa(buf->m.planes[0].m.userptr) + width*height;
		dev.layer_info.layer_config.info.fb.addr[2] = __va_to_pa(buf->m.planes[0].m.userptr) + width*height + width*height/4;

		/* dev.layer_info.layer_config.info.fb.addr[0] = (*addr);
		dev.layer_info.layer_config.info.fb.addr[1] =
		    (dev.layer_info.layer_config.info.fb.addr[0] + width * height);
		dev.layer_info.layer_config.info.fb.addr[2] =
		    dev.layer_info.layer_config.info.fb.addr[0] +
		    width * height * 5 / 4;
		dev.layer_info.layer_config.info.fb.trd_right_addr[0] =
		    (dev.layer_info.layer_config.info.fb.addr[0] +
		     width * height * 3 / 2);
		dev.layer_info.layer_config.info.fb.trd_right_addr[1] =
		    (dev.layer_info.layer_config.info.fb.addr[0] + width * height);
		dev.layer_info.layer_config.info.fb.trd_right_addr[2] =
		    (dev.layer_info.layer_config.info.fb.addr[0] +
		     width * height * 5 / 4); */
	} else {
		dev.layer_info.layer_config.info.fb.size[0].width = width;
		dev.layer_info.layer_config.info.fb.size[0].height = height;
		dev.layer_info.layer_config.info.fb.size[1].width = width / 2;
		dev.layer_info.layer_config.info.fb.size[1].height = height;
		dev.layer_info.layer_config.info.fb.size[2].width = width / 2;
		dev.layer_info.layer_config.info.fb.size[2].height = height;
		dev.layer_info.layer_config.info.fb.crop.width =
		    (unsigned long long)width << 32;
		dev.layer_info.layer_config.info.fb.crop.height =
		    (unsigned long long)height << 32;

		//dev.layer_info.layer_config.info.fb.addr[0] = buf->m.planes[0].m.mem_offset;
		//dev.layer_info.layer_config.info.fb.addr[1] = buf->m.planes[1].m.mem_offset;
		//dev.layer_info.layer_config.info.fb.addr[2] = buf->m.planes[2].m.mem_offset;
		dev.layer_info.layer_config.info.fb.addr[0] = __va_to_pa(buf->m.planes[0].m.userptr);
		dev.layer_info.layer_config.info.fb.addr[1] = __va_to_pa(buf->m.planes[1].m.userptr);
		dev.layer_info.layer_config.info.fb.addr[2] = __va_to_pa(buf->m.planes[2].m.userptr);

		/* dev.layer_info.layer_config.info.fb.addr[0] = (*addr);
		dev.layer_info.layer_config.info.fb.addr[1] =
		    (dev.layer_info.layer_config.info.fb.addr[0] + width * height);
		dev.layer_info.layer_config.info.fb.addr[2] =
		    dev.layer_info.layer_config.info.fb.addr[0] +
		    width * height * 2 / 2;
		dev.layer_info.layer_config.info.fb.trd_right_addr[0] =
		    (dev.layer_info.layer_config.info.fb.addr[0] +
		     width * height * 2);
		dev.layer_info.layer_config.info.fb.trd_right_addr[1] =
		    (dev.layer_info.layer_config.info.fb.addr[0] + width * height); */
	}

	dev.layer_info.layer_config.enable = 1;

	arg[0] = dev.layer_info.screen_id;
	arg[1] = (unsigned long)&dev.layer_info.layer_config;
	arg[2] = 1;
	arg[3] = 0;
	ret = dev.layer_info.dispfh->control(dev.layer_info.dispfh, DISP_LAYER_SET_CONFIG, (void *)arg);
	if (ret != 0)
		printf("disp_set_addr fail to set layer info\n");

	return 0;
}


static int cmd_csi_test(int argc, const char **argv)
{
	disp_init(width, height, TVD_PL_YUV420);

/////1. camera init
	fd = open("/dev/video0", O_RDWR /* required */  | O_NONBLOCK, 0);
	if (fd < 0) {
		printf("open falied\n");
		goto error;
	}
	printf("open /dev/video0 fd = %d\n", fd);


	inp.index = sel;
	if (-1 == ioctl(fd, VIDIOC_S_INPUT, &inp)) {
		printf("VIDIOC_S_INPUT %d error!\n", sel);
		goto error;
	}

	parms.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	parms.parm.capture.timeperframe.numerator = 1;
	parms.parm.capture.timeperframe.denominator = fps;
	parms.parm.capture.capturemode = V4L2_MODE_VIDEO;
	/* parms.parm.capture.capturemode = V4L2_MODE_IMAGE; */
	/*when different video have the same sensor source, 1:use sensor current win, 0:find the nearest win*/
	parms.parm.capture.reserved[0] = 0;
	parms.parm.capture.reserved[1] = 0;/*2:command, 1: wdr, 0: normal*/

	if (-1 == ioctl(fd, VIDIOC_S_PARM, &parms)) {
		printf("VIDIOC_S_PARM error\n");
		goto error;
	}


/////2. set format

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	fmt.fmt.pix_mp.width = width;
	fmt.fmt.pix_mp.height = height;
	switch (mode) {
	case 0:
		fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_SBGGR8;
		break;
	case 1:
		fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_YUV420M;
		break;
	case 2:
		fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_YUV420;
		break;
	case 3:
		fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_NV12M;
		break;
	case 4:
		fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_NV12;
		break;
	case 5:
		fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_SBGGR10;
		break;
	case 6:
		fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_SBGGR12;
		break;
	case 7:
		fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_LBC_2_5X;
		break;
	default:
		fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_YUV420M;
		break;
	}
	fmt.fmt.pix_mp.field = V4L2_FIELD_NONE;

	if (-1 == ioctl(fd, VIDIOC_S_FMT, &fmt)) {
		printf("VIDIOC_S_FMT error!\n");
		goto error;
	}

	if (-1 == ioctl(fd, VIDIOC_G_FMT, &fmt)) {
		printf("VIDIOC_G_FMT error!\n");
		goto error;
	} else {
		nplanes = fmt.fmt.pix_mp.num_planes;
		printf("resolution got from sensor = %ld*%ld num_planes = %d\n",
		       fmt.fmt.pix_mp.width, fmt.fmt.pix_mp.height,
		       fmt.fmt.pix_mp.num_planes);
	}


/////3. req frame buffer


	req.count = req_frame_num;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	req.memory = V4L2_MEMORY_USERPTR;
	if (-1 == ioctl(fd, VIDIOC_REQBUFS, &req)) {
		printf("VIDIOC_REQBUFS error\n");
		goto error;
	}

	buffers = calloc(req.count, sizeof(*buffers));


	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	buf.memory = V4L2_MEMORY_USERPTR;
	buf.length = nplanes;
	buf.m.planes = (struct v4l2_plane *)calloc(nplanes, sizeof(struct v4l2_plane));

	if (buf.m.planes == NULL) {
		printf("buf.m.planes calloc failed!\n");
		goto error;
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		buf.index = n_buffers;

		if (-1 == ioctl(fd, VIDIOC_QUERYBUF, &buf)) {
			printf("VIDIOC_QUERYBUF error\n");
			goto error;
		}

		for (i = 0; i < nplanes; i++) {
			buffers[n_buffers].length[i] = buf.m.planes[i].length;
			buffers[n_buffers].start[i] = hal_malloc(buf.m.planes[i].length);
			buf.m.planes[i].m.userptr = buffers[n_buffers].start[i];
			printf("buffers[%d] - start[%d] - 0x%x\n", n_buffers, i, (unsigned int)buffers[n_buffers].start[i]);
		}

		if (-1 == ioctl(fd, VIDIOC_QBUF, &buf)) {
			printf("VIDIOC_QBUF failed\n");
			goto error;
		}
	}

///// streamon

	if (-1 == ioctl(fd, VIDIOC_STREAMON, &type)) {
		printf("VIDIOC_STREAMON failed\n");
		return -1;
	} else
		printf("VIDIOC_STREAMON ok\n");

	count = test_cnt;
	while (count-- > 0) {
		for (;;) {
			fd_set fds;
			struct timeval tv;
			int r;

			FD_ZERO(&fds);
			FD_SET(fd, &fds);

			tv.tv_sec = 2; /* Timeout. */
			tv.tv_usec = 0;

			r = select(fd + 1, &fds, NULL, NULL, &tv);

			if (-1 == r) {
				if (errno == EINTR)
					continue;
				printf("select err\n");
			}

			if (r == 0) {
				fprintf(stderr, "select timeout\n");
				continue;
			}



			if (-1 == ioctl(fd, VIDIOC_DQBUF, &buf)) {
				printf("VIDIOC_DQBUF failed\n");
				goto error;
			}

			for (n_buffers = 0; n_buffers < req_frame_num; ++n_buffers) {
				for (i = 0; i < nplanes; i++) {
					if ((buf.m.planes[i].m.userptr == buffers[n_buffers].start[i]) &&
						(buf.m.planes[i].length == buffers[n_buffers].length[i]))
						flags = 1;
					else
						flags = 0;
					//printf("buffers[%d] - planes[%d] - 0x%x\n", n_buffers, i, buf.m.planes[i].m.userptr);
				}
				if (flags == 1)
					break;
			}

			//printf("n_buffers: %d, buf.index: %d\n", n_buffers, buf.index);


			if (file_flag == 1) {
				char fdstr[50];
				FILE *file_fd = NULL;
				char *dst = NULL;
				char path_name[]="/mnt/E";
				int dev_id = 0;
				int read_num = 1;

				if (save_flag == 0) {
					if ((count == read_num / 2) || ((count > 0) && (nplanes == 1))) {
						printf("file length = %d %d %d\n", buffers[buf.index].length[0],
						       buffers[buf.index].length[1],
						       buffers[buf.index].length[2]);
						printf("file start = %p %p %p\n", buffers[buf.index].start[0],
						       buffers[buf.index].start[1],
						       buffers[buf.index].start[2]);

						switch (nplanes) {
						case 1:
							sprintf(fdstr, "%s/fb%d_y%d_%d_%d_%u.bin", path_name, dev_id, mode, width, height, count);
							file_fd = fopen(fdstr, "w");
							fwrite(buffers[buf.index].start[0], buffers[buf.index].length[0], 1, file_fd);
							fclose(file_fd);
							break;
						case 2:
							sprintf(fdstr, "%s/fb%d_y%d_%d_%d.bin", path_name, dev_id, mode, width, height);
							file_fd = fopen(fdstr, "w");
							fwrite(buffers[buf.index].start[0], buffers[buf.index].length[0], 1, file_fd);
							fclose(file_fd);
							sprintf(fdstr, "%s/fb%d_uv%d_%d_%d.bin", path_name, dev_id, mode, width, height);
							file_fd = fopen(fdstr, "w");
							fwrite(buffers[buf.index].start[1], buffers[buf.index].length[1], 1, file_fd);
							fclose(file_fd);
							break;
						case 3:
							sprintf(fdstr, "%s/fb%d_y%d_%d_%d.bin", path_name, dev_id, mode, width, height);
							file_fd = fopen(fdstr, "w");
							fwrite(buffers[buf.index].start[0], buffers[buf.index].length[0], 1, file_fd);
							fclose(file_fd);

							sprintf(fdstr, "%s/fb%d_u%d_%d_%d.bin", path_name, dev_id, mode, width, height);
							file_fd = fopen(fdstr, "w");
							fwrite(buffers[buf.index].start[1], buffers[buf.index].length[1], 1, file_fd);
							fclose(file_fd);

							sprintf(fdstr, "%s/fb%d_v%d_%d_%d.bin", path_name, dev_id, mode, width, height);
							file_fd = fopen(fdstr, "w");
							fwrite(buffers[buf.index].start[2], buffers[buf.index].length[2], 1, file_fd);
							fclose(file_fd);
							break;
						default:
							break;
						}
					}
				}else if (save_flag == 1) {
					if ((count > 0)) {
						switch (nplanes) {
						case 1:
							sprintf(fdstr, "%s/fb%d_yuv%d_%d_%d.bin", path_name, dev_id, mode, width, height);
							file_fd = open(fdstr, O_WRONLY | O_CREAT | O_APPEND);
							if (file_fd < 0) {
								printf("open %s error\n", fdstr);
							}
							write(file_fd, buffers[buf.index].start[0], buffers[buf.index].length[0]);
							close(file_fd);
							break;
						case 2:
							sprintf(fdstr, "%s/fb%d_yuv%d_%d_%d.bin", path_name, dev_id, mode, width, height);
							file_fd = open(fdstr, O_WRONLY | O_CREAT | O_APPEND);
							if (file_fd < 0) {
								printf("open %s error\n", fdstr);
							}
							write(file_fd, buffers[buf.index].start[0], buffers[buf.index].length[0]);
							close(file_fd);
							file_fd = open(fdstr, O_WRONLY | O_CREAT | O_APPEND);
							if (file_fd < 0) {
								printf("open %s error\n", fdstr);
							}
							write(file_fd, buffers[buf.index].start[1], buffers[buf.index].length[1]);
							close(file_fd);
							break;
						case 3:
							sprintf(fdstr, "%s/fb%d_yuv%d_%d_%d.bin", path_name, dev_id, mode, width, height);
							file_fd = open(fdstr, O_WRONLY | O_CREAT | O_APPEND);
							if (file_fd < 0) {
								printf("open %s error\n", fdstr);
							}
							write(file_fd, buffers[buf.index].start[0], buffers[buf.index].length[0]);
							close(file_fd);
							file_fd = open(fdstr, O_WRONLY | O_CREAT | O_APPEND);
							if (file_fd < 0) {
								printf("open %s error\n", fdstr);
							}
							write(file_fd, buffers[buf.index].start[1], buffers[buf.index].length[1]);
							close(file_fd);
							file_fd = open(fdstr, O_WRONLY | O_CREAT | O_APPEND);
							if (file_fd < 0) {
								printf("open %s error\n", fdstr);
							}
							write(file_fd, buffers[buf.index].start[2], buffers[buf.index].length[2]);
							close(file_fd);
							break;
						default:
							break;
						}
					}
				}else if (save_flag == 2) {
					if (count <= 1)
						count = test_cnt;
					disp_set_addr(width, height, &buf);
				}
			}// flie_flag

			if (-1 == ioctl(fd, VIDIOC_QBUF, &buf)) {
				printf("VIDIOC_QBUF buf.index %d failed\n", buf.index);
				goto error;
			}
			//printf("VIDIOC_QBUF buf.index%d\n", buf.index);

			break;


		}
	}


///// streamoff
	if (-1 == ioctl(fd, VIDIOC_STREAMOFF, &type)) {
		printf("VIDIOC_STREAMOFF failed\n");
		return -1;
	} else
		printf("VIDIOC_STREAMOFF ok\n");

	csi_test_disp_layer_clear();
error:

	if (buf.m.planes)
		free(buf.m.planes);

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		for (i = 0; i < nplanes; i++) {
			if (buffers && buffers[n_buffers].start[i])
				free(buffers[n_buffers].start[i]);
		}
	}

	close(fd);

	return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_csi_test, __cmd_csi_test, csi test);

