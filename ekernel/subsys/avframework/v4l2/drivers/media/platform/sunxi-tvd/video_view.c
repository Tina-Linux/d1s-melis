#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <rtthread.h>
#include <hal_mem.h>
#include <log.h>
#include <video/sunxi_display2.h>
#include <linux/videodev2.h>
#include <mman.h>
#include <pthread.h>
#include <finsh.h>
#include <finsh_api.h>
#include <shell.h>
#include <dfs_posix.h>
#include <mman.h>
#include <pthread.h>
#include <linux/v4l2-controls.h>


typedef enum
{
    TVD_PL_YUV420 					=	0,
    TVD_MB_YUV420					=	1,
    TVD_PL_YUV422					=	2,
}TVD_FMT_T;


struct tvd_dev dev;
struct buffer
{
    void   *start;
    size_t length;
};

struct size
{
	int width;
	int height;
};

static int fd;


static struct buffer *buffers = NULL;
static int nbuffers = 0;

static struct size disp_size;

#define CLEAR(x) memset (&(x), 0, sizeof (x))

struct test_layer_info {
	int screen_id;
	int layer_id;
	struct disp_layer_config layer_config;
	int width, height;/* screen size */
	int dispfh;/* device node handle */
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
	unsigned int pixformat;
	unsigned int in_sel;
	unsigned char brightness;
	unsigned char contrast;
	unsigned char saturation;
	struct test_layer_info layer_info;
	int frame_no_to_grap;
	FILE *raw_fp;
};

struct disp_screen
{
	int x;
	int y;
	int w;
	int h;
};

static int video_view_print_info(const struct tvd_dev *dev)
{
	if (!dev)
		return -1;
	printf("[tvd] channel id:%d\n", dev->ch_id);
	printf("[tvd] screen id:%d\n", dev->layer_info.screen_id);
	printf("[tvd] camera resolution: %d x %d\n", dev->width, dev->height);
	printf("[tvd] video system :%s\n", (dev->height == 576) ? "PAL" : "NTSC");
	printf("[tvd] interface :%s\n", (dev->interface) ? "YPbPr" : "CVBS");
	printf("[tvd] pixformat :%s\n",
	       (dev->pixformat == TVD_PL_YUV420) ? "TVD_PL_YUV420"
						 : "TVD_PL_YUV422");
	printf("[tvd] brightness:%u contrast:%u saturation:%u\n",
	       dev->brightness, dev->contrast, dev->saturation);
	if (dev->layer_info.screen_id >= 0) {
		printf("[disp] output_type:%d resolution:%d x %d\n",
		       dev->layer_info.output_type, dev->layer_info.width,
		       dev->layer_info.height);
		printf("[disp] screen scale resolution:%d x %d\n",
		       dev->layer_info.layer_config.info.screen_win.width,
		       dev->layer_info.layer_config.info.screen_win.height);
		printf("[disp] display channel:%d layer id:%d\n",
		       dev->layer_info.layer_config.channel,
		       dev->layer_info.layer_config.layer_id);
		printf("[disp] display pixformat:0x%x\n",
		       dev->layer_info.layer_config.info.fb.format);
	}
	return 0;
}

static struct disp_screen get_disp_screen(int w1, int h1, int w2, int h2)
{
	struct disp_screen screen;
	float r1,r2;
	r1 = (float)w1/(float)w2;
	r2 = (float)h1/(float)h2;
	if(r1 < r2){
		screen.w = w2*r1;
		screen.h = h2*r1;
	}else{
		screen.w = w2*r2;
		screen.h = h2*r2;
	}

	screen.x = (w1 - screen.w)/2;
	screen.y = (h1 - screen.h)/2;

	return screen;
}

static int disp_layer_clear_all(void)
{
	unsigned long args[4];
	int i = 0, j = 0, ret = -1;
	int chn = (dev.layer_info.screen_id == 0) ? 4 : 2;
	int lyr = 4;;
	struct disp_layer_config layer_cfg[chn][lyr];



	for (i = 0; i < chn; ++i) {
		for (j  = 0; j  < lyr; ++j ) {
			layer_cfg[i][j].enable = false;
			layer_cfg[i][j].channel = i;
			layer_cfg[i][j].layer_id = j;
			args[0] = dev.layer_info.screen_id;
			args[1] = (unsigned long)(&layer_cfg[i][j]);
			args[2] = 1;
			ret = ioctl(dev.layer_info.dispfh, DISP_LAYER_SET_CONFIG, (void *)args);
			if (ret) {
				printf("DISP_LAYER_SET_CONFIG fail!\n");
				goto OUT;
			}
			usleep(16000);
		}
	}



	ret = 0;

OUT:
	return ret;
}

static int disp_init(int width, int height, unsigned int pixformat)
{
	//	display_handle* disp = (display_handle*)display;
	unsigned int arg[6] = {0};
	int layer_id = 0;

	if (dev.layer_info.screen_id < 0)
		return 0;

	// open device /dev/disp
	if ((dev.layer_info.dispfh = open("/dev/disp", O_RDWR)) == -1) {
		printf("open display device fail!\n");
		return -1;
	}

	// get current output type
	arg[0] = dev.layer_info.screen_id;
	dev.layer_info.output_type = (enum disp_output_type)ioctl(
	    dev.layer_info.dispfh, DISP_GET_OUTPUT_TYPE, (void *)arg);
	if (dev.layer_info.output_type == DISP_OUTPUT_TYPE_NONE) {
		printf("the output type is DISP_OUTPUT_TYPE_NONE %d\n",
		       dev.layer_info.output_type);
		return -1;
	}

	dev.layer_info.pixformat = pixformat;
	dev.layer_info.layer_config.channel = 0;
	dev.layer_info.layer_config.layer_id = layer_id;
	dev.layer_info.layer_config.info.zorder = 16;
	dev.layer_info.layer_config.info.alpha_mode = 1;
	dev.layer_info.layer_config.info.alpha_value = 0xff;
	dev.layer_info.width =
	    ioctl(dev.layer_info.dispfh, DISP_GET_SCN_WIDTH, (void *)arg);
	dev.layer_info.height =
	    ioctl(dev.layer_info.dispfh, DISP_GET_SCN_HEIGHT, (void *)arg);

	dev.layer_info.layer_config.info.mode = LAYER_MODE_BUFFER;

	if (dev.layer_info.pixformat == TVD_PL_YUV420)
		dev.layer_info.layer_config.info.fb.format =
		    DISP_FORMAT_YUV420_SP_UVUV;
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
			dev.layer_info.layer_config.info.screen_win.width = 800;
			dev.layer_info.layer_config.info.screen_win.height =
			    480;
		}
	} else {
		/*struct disp_screen screen;*/
		get_disp_screen(dev.layer_info.width, dev.layer_info.height,
				width, height);
		dev.layer_info.layer_config.info.screen_win.x = 0; // screen.x;
		dev.layer_info.layer_config.info.screen_win.y = 0; // screen.y;
		dev.layer_info.layer_config.info.screen_win.width =
		    dev.layer_info.width;
		dev.layer_info.layer_config.info.screen_win.height =
		    dev.layer_info.height;
		// printf("x: %d, y: %d, w: %d, h:
		// %d\n",screen.x,screen.y,screen.w,screen.h);
	}
	disp_layer_clear_all();

	return 0;
}

static int disp_quit(void)
{
	int ret;
	unsigned long arg[6];

	if (dev.layer_info.screen_id < 0)
		return 0;

	dev.layer_info.layer_config.enable = 0;
	arg[0] = dev.layer_info.screen_id;
	arg[1] = (unsigned long)&dev.layer_info.layer_config;
	arg[2] = 0;
	ret = ioctl(dev.layer_info.dispfh, DISP_LAYER_SET_CONFIG, (void *)arg);
	if (0 != ret)
		printf("fail to set DISP_LAYER_SET_CONFIG\n");

	close(dev.layer_info.dispfh);
	memset(&dev.layer_info, 0, sizeof(struct test_layer_info));

	return 0;
}

static int disp_set_addr(int width, int height, unsigned int *addr)

{
	unsigned long arg[6];
	int ret;


	if (dev.layer_info.pixformat == TVD_PL_YUV420) {
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
		dev.layer_info.layer_config.info.fb.addr[0] = (*addr);
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
		     width * height * 5 / 4);
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
		dev.layer_info.layer_config.info.fb.addr[0] = (*addr);
		dev.layer_info.layer_config.info.fb.addr[1] =
		    (dev.layer_info.layer_config.info.fb.addr[0] + width * height);
		dev.layer_info.layer_config.info.fb.addr[2] =
		    dev.layer_info.layer_config.info.fb.addr[0] +
		    width * height * 2 / 2;
		dev.layer_info.layer_config.info.fb.trd_right_addr[0] =
		    (dev.layer_info.layer_config.info.fb.addr[0] +
		     width * height * 2);
		dev.layer_info.layer_config.info.fb.trd_right_addr[1] =
		    (dev.layer_info.layer_config.info.fb.addr[0] + width * height);
	}

	dev.layer_info.layer_config.enable = 1;

	arg[0] = dev.layer_info.screen_id;
	arg[1] = (unsigned long)&dev.layer_info.layer_config;
	arg[2] = 1;
	ret = ioctl(dev.layer_info.dispfh, DISP_LAYER_SET_CONFIG, (void*)arg);
	if(0 != ret)
		printf("disp_set_addr fail to set layer info\n");

	return 0;
}


static void save_frame_to_file(const void *p, int size, unsigned int frame_index)
{
	int ret = -1;
	char filename[80];

	sprintf(filename, "/mnt/F/video%u_frame%u_%ux%u_fmt%u.yuv", dev.ch_id,
		frame_index, dev.width, dev.height, dev.pixformat);
	dev.raw_fp=fopen(filename,"w+");
	if (!dev.raw_fp) {
		perror(filename);
	} else {
		printf("size:%u\n", size);
		ret = fwrite(p, size, 1, dev.raw_fp);
		if (ret != 1) {
			perror("write error\n");
		}
		fflush(dev.raw_fp);
		fclose(dev.raw_fp);
	}

}


static int read_frame(unsigned int frame_index)
{
	struct v4l2_buffer buf;
	int ret = -1;

	memset(&buf, 0, sizeof(struct v4l2_buffer));


	buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	ret = ioctl(fd, VIDIOC_DQBUF, &buf);
	if (ret)
		printf("VIDIOC_DQBUF fail!:%d\n", ret);
	if (dev.layer_info.screen_id < 0)
		save_frame_to_file(buffers[buf.index].start, buf.bytesused, frame_index);
	else
		disp_set_addr(disp_size.width, disp_size.height, &buf.m.offset);

	ret = ioctl(fd, VIDIOC_QBUF, &buf);
	if (ret)
		printf("VIDIOC_QBUF fail!:%d\n", ret);
	return ret;
}

int video_mainloop(int id, unsigned int pixformat)
{
	char dev_name[32];
	struct v4l2_format fmt;
	struct v4l2_frmsizeenum frmsize;
	int i, ret = -1;
	enum v4l2_buf_type type;
	struct v4l2_control ctl;

	snprintf(dev_name,32, "/dev/video%d", id);
	if ((fd = open(dev_name, O_RDWR | O_NONBLOCK, 0)) < 0) {
		printf("can't open %s(%s)\n", dev_name, strerror(errno));
		goto open_err;
	}

	memset(&frmsize, 0, sizeof(struct v4l2_frmsizeenum));
	frmsize.pixel_format = V4L2_PIX_FMT_NV12;
	frmsize.index = 0;

	i = 0;
        while (!ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize)) {
		frmsize.index++;
	}


	/* set input input index */
	ret = ioctl(fd, VIDIOC_S_INPUT, &dev.in_sel);
	printf("VIDIOC_S_INPUT:%u %s\n", dev.in_sel, (ret)?"fail":"successfully");

	/* set image format */
	memset(&fmt, 0, sizeof(struct v4l2_format));


	fmt.type				 = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = 720;
	fmt.fmt.pix.height = 480;//576;
	if (pixformat == TVD_PL_YUV420)
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV12;
	else
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV61;

	usleep(200000);
	memset(&fmt, 0, sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	while (ioctl(fd, VIDIOC_G_FMT, &fmt)) {
		printf("get signal failed.\n");
		usleep(1000);
	}


	if (pixformat == TVD_PL_YUV420)
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV12;
	else
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV61;
	if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
			printf("set image format failed\n");
			goto err;
	}

	usleep(100000);
	disp_size.width = fmt.fmt.pix.width;
	disp_size.height = fmt.fmt.pix.height;
	dev.width = fmt.fmt.pix.width;
	dev.height = fmt.fmt.pix.height;

	struct v4l2_requestbuffers req;
	CLEAR (req);
	req.count               = 5;
	req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory              = V4L2_MEMORY_MMAP;

	 //申请缓冲，count是申请的数量，注意，释放缓冲实际在VIDIOC_STREAMOFF内完成了。
	ret = ioctl(fd, VIDIOC_REQBUFS, &req);
	if (ret)
		printf("VIDIOC_REQBUFS ret:%d\n", ret);


	buffers = calloc(req.count, sizeof(struct buffer));
	for (nbuffers = 0; nbuffers < req.count; nbuffers++) {
		struct v4l2_buffer buf;

		memset(&buf, 0, sizeof(struct v4l2_buffer));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = nbuffers;

		if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
			printf("VIDIOC_QUERYBUF error\n");
			goto buffer_rel;
		}

		buffers[nbuffers].start =
		    mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED,
			 fd, buf.m.offset);
		buffers[nbuffers].length = buf.length;
		if (buffers[nbuffers].start == MAP_FAILED) {
			printf("mmap failed\n");
			goto buffer_rel;
		}
	}

	for (i = 0; i < nbuffers; i++) {
		struct v4l2_buffer buf;

        	memset(&buf, 0, sizeof(struct v4l2_buffer));
        	buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index  = i;
		if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
			printf("VIDIOC_QBUF error\n");
			goto unmap;
		}
	}
	if (fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_NV61 ||
	    fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_NV16)
		pixformat = TVD_PL_YUV422;
	else
		pixformat = TVD_PL_YUV420;


	ret = disp_init(disp_size.width, disp_size.height, pixformat);
	if (ret)
		return 2;

	video_view_print_info(&dev);
	memset(&ctl, 0, sizeof(struct v4l2_control));
	if(dev.brightness > 0) {
		ctl.id = V4L2_CID_BRIGHTNESS;
		ctl.value = dev.brightness;
		ret = ioctl(fd, VIDIOC_S_CTRL, &ctl);
		if (ret)
			printf("Set brightness :%u fail:ret:%d\n", dev.brightness, ret);
	}

	ctl.id = V4L2_CID_BRIGHTNESS;
	ret = ioctl(fd, VIDIOC_G_CTRL, &ctl);
	if (ret)
		printf("Get brightness fail:ret:%d\n", ret);
	else
		dev.brightness = ctl.value;

	if(dev.contrast > 0) {
		ctl.id = V4L2_CID_CONTRAST;
		ctl.value = dev.contrast;
		ret = ioctl(fd, VIDIOC_S_CTRL, &ctl);
		if (ret)
			printf("Set contrast :%u fail:ret:%d\n", dev.contrast, ret);
	}

	ctl.id = V4L2_CID_CONTRAST;
	ret = ioctl(fd, VIDIOC_G_CTRL, &ctl);
	if (ret)
		printf("Get contrast fail:ret:%d\n", ret);
	else
		dev.contrast = ctl.value;

	if(dev.saturation > 0) {
		ctl.id = V4L2_CID_SATURATION;
		ctl.value = dev.saturation;
		ret = ioctl(fd, VIDIOC_S_CTRL, &ctl);
		if (ret)
			printf("Set saturation :%u fail:ret:%d\n", dev.saturation, ret);
	}

	ctl.id = V4L2_CID_SATURATION;
	ret = ioctl(fd, VIDIOC_G_CTRL, &ctl);
	if (ret)
		printf("Get saturation fail:ret:%d\n", ret);
	else
		dev.saturation = ctl.value;



	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
		printf("VIDIOC_STREAMON error\n");
		goto disp_exit;
	}


	unsigned int frame_index = 0;
	while (1) {
		fd_set fds;
		struct timeval tv;
		int r;

		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		/* timeout */
		tv.tv_sec  = 2;
		tv.tv_usec = 0;

		r = select(fd + 1, &fds, NULL, NULL, &tv);
		if (r == -1) {
			if (errno == EINTR) {
				continue;
			}

			printf("select error\n");
		}

		if (r == 0) {
			printf("select timeout\n");
			goto stream_off;
		}

		read_frame(frame_index);
		--dev.frame_no_to_grap;
		++frame_index;
		if(dev.frame_no_to_grap == 0)
			break;
	}

stream_off:
	ioctl(fd, VIDIOC_STREAMOFF, &type);
disp_exit:
    disp_quit();
unmap:
    for (i = 0; i < nbuffers; i++) {
        munmap(buffers[i].start, buffers[i].length);
    }
buffer_rel:
    free(buffers);
err:
    close(fd);
open_err:
    return -1;
}




void video_view_usage(void)
{
	printf("usage : ./video_view <-ch channel_number> [-fmt pixformat] "
	       "[-full 0/1] [-w width] [-h height] [-screen_id id] [-b brightness] [-t contrast] [-s saturation]\n");
	printf("\t-ch can be 4, 5, 6, 7 and 8\n");
	printf("\t-fmt can be 0(yuv420,default), 1(mb-yuv420) and "
	       "2(yuv422)\n");
	printf("\t-full: full screen flag, 1:full screen, 0:not full screen\n");
	printf("\t-w specify the width of screen display(default is 800), only "
	       "valid if is is not full screen\n");
	printf("\t-h specify the height of screen display(default is 480, only "
	       "valid if is is not full screen)\n");
	printf("\t-c: number of frame to grap(default 1) when screen_id equal to 1\n");
	printf("\t-screen_id: display index to show(default 0).It will grap "
	       "frame instead show in display device when screen id equal to 1 "
	       "\n");
	printf("\t-b: brightness. 0~255\n");
	printf("\t-t: contrast. 0~255\n");
	printf("\t-s: saturation. 0~255\n");
}

int parse_cmdline(int argc, char **argv, struct tvd_dev *p)
{
	int err = 1;
	int i = 0;

	if (!p)
		return -1;

	if (argc == 1) {
		video_view_usage();
		return 1;
	}

	while (i < argc) {
		if (!strcmp(argv[i], "-ch")) {
			if (argc > i + 1) {
				i += 1;
				p->ch_id = atoi(argv[i]);
				--err;
			} else
				++err;
		}

		if (!strcmp(argv[i], "-fmt")) {
			if (argc > i + 1) {
				i += 1;
				p->pixformat = atoi(argv[i]);
			}
		}

		if (!strcmp(argv[i], "-full")) {
			if (argc > i + 1) {
				i += 1;
				p->layer_info.full_screen = atoi(argv[i]);
			}
		}
		if (!strcmp(argv[i], "-w")) {
			if (argc > i + 1) {
				i += 1;
				p->layer_info.layer_config.info.screen_win.width = atoi(argv[i]);
			}
		}

		if (!strcmp(argv[i], "-b")) {
			if (argc > i + 1) {
				i += 1;
				p->brightness = atoi(argv[i]);
			}
		}

		if (!strcmp(argv[i], "-t")) {
			if (argc > i + 1) {
				i += 1;
				p->contrast = atoi(argv[i]);
			}
		}

		if (!strcmp(argv[i], "-s")) {
			if (argc > i + 1) {
				i += 1;
				p->saturation = atoi(argv[i]);
			}
		}

		if (!strcmp(argv[i], "-screen_id")) {
			if (argc > i + 1) {
				i += 1;
				p->layer_info.screen_id = atoi(argv[i]);
			}
		}
		if (!strcmp(argv[i], "-c")) {
			if (argc > i + 1) {
				i += 1;
				p->frame_no_to_grap = atoi(argv[i]);
			}
		}
		if (!strcmp(argv[i], "-i")) {
			if (argc > i + 1) {
				i += 1;
				p->in_sel = atoi(argv[i]);
			}
		}
		if (!strcmp(argv[i], "-h")) {
			if (argc > i + 1) {
				i += 1;
				p->layer_info.layer_config.info.screen_win.height = atoi(argv[i]);
			}
		}
		if (!strcmp(argv[i], "-help")) {
			video_view_usage();
			return 1;
		}

		i++;
	}

	if (err > 0) {
		video_view_usage();
		return -1;
	} else
		return 0;
}

int video_view(int argc, char *argv[])
{

	int ret = 0;
	memset(&dev, 0, sizeof(struct tvd_dev));
	memset(&dev.layer_info, 0, sizeof(struct test_layer_info));
	dev.frame_no_to_grap = -1;

	ret = parse_cmdline(argc, argv , &dev);
	if (ret)
		return 2;

	video_mainloop(dev.ch_id, dev.pixformat);

	return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(video_view, __cmd_video_view, video_view);
