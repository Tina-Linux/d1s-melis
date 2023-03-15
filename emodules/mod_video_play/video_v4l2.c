#include <log.h>
#include <video/sunxi_display2.h>
#include <linux/videodev2.h>
#include <dfs_posix.h>
#include <linux/v4l2-controls.h>
#include "video_dev.h"
#include "video_v4l2.h"
#include "sunxi_di.h"
#include "drm_fourcc.h"


static video_buffer_t* buffers = NULL;
int g_di_fd;
struct di_process_fb_arg g_di_arg;

__u32 g_pixelformat = V4L2_PIX_FMT_NV12;

int v4l2_set_contrast(int contrast)
{
	struct v4l2_control ctl;
	int ret;
	if(g_video_ctrl->h_video_dev <= 0)
	{
		return EPDK_FAIL;
	}
	memset(&ctl, 0, sizeof(struct v4l2_control));
	ctl.id = V4L2_CID_CONTRAST;
	ctl.value = contrast;
	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_S_CTRL, &ctl);
	__log("set contrast:%u %s\n", contrast, (ret) ? "fail" : "successfully");
	return ret;
}

int v4l2_get_contrast(void)
{
	struct v4l2_control ctl;
	int ret;
	if(g_video_ctrl->h_video_dev <= 0)
	{
		return EPDK_FAIL;
	}
	memset(&ctl, 0, sizeof(struct v4l2_control));
	ctl.id = V4L2_CID_CONTRAST;
	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_G_CTRL, &ctl);
	__log("get contrast %s :%u\n", (ret) ? "fail" : "successfully", ret);
	return ret;
}
int v4l2_set_brightness(int brightness)
{
	struct v4l2_control ctl;
	int ret;
	if(g_video_ctrl->h_video_dev <= 0)
	{
		return EPDK_FAIL;
	}
	memset(&ctl, 0, sizeof(struct v4l2_control));
	ctl.id = V4L2_CID_BRIGHTNESS;
	ctl.value = brightness;
	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_S_CTRL, &ctl);
	__log("set brightness:%u %s\n", brightness, (ret) ? "fail" : "successfully");
	return ret;
}
int v4l2_get_brightness(void)
{
	struct v4l2_control ctl;
	int ret;
	if(g_video_ctrl->h_video_dev <= 0)
	{
		return EPDK_FAIL;
	}
	memset(&ctl, 0, sizeof(struct v4l2_control));
	ctl.id = V4L2_CID_BRIGHTNESS;
	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_G_CTRL, &ctl);
	__log("get brightness %s :%u\n", (ret) ? "fail" : "successfully", ret);
	return ret;
}
int v4l2_set_saturation(int saturation)
{
	struct v4l2_control ctl;
	int ret;
	if(g_video_ctrl->h_video_dev <= 0)
	{
		return EPDK_FAIL;
	}
	memset(&ctl, 0, sizeof(struct v4l2_control));
	ctl.id = V4L2_CID_SATURATION;
	ctl.value = saturation;
	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_S_CTRL, &ctl);
	__log("set saturation:%u %s\n", saturation, (ret) ? "fail" : "successfully");
	return ret;
}
int v4l2_get_saturation(void)
{
	struct v4l2_control ctl;
	int ret;
	if(g_video_ctrl->h_video_dev <= 0)
	{
		return EPDK_FAIL;
	}
	memset(&ctl, 0, sizeof(struct v4l2_control));
	ctl.id = V4L2_CID_SATURATION;
	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_G_CTRL, &ctl);
	__log("get saturation %s :%u\n", (ret) ? "fail" : "successfully", ret);
	return ret;
}

int v4l2_set_sharpness(int sharpness)
{
	struct v4l2_control ctl;
	int ret;
	if(g_video_ctrl->h_video_dev <= 0)
	{
		return EPDK_FAIL;
	}
	memset(&ctl, 0, sizeof(struct v4l2_control));
	ctl.id = V4L2_CID_SHARPNESS;
	ctl.value = sharpness;
	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_S_CTRL, &ctl);
	__log("set sharpness:%u %s\n", sharpness, (ret) ? "fail" : "successfully");
	return ret;
}

int v4l2_get_sharpness(void)
{
	struct v4l2_control ctl;
	int ret;

	if(g_video_ctrl->h_video_dev <= 0)
	{
		return EPDK_FAIL;
	}
	memset(&ctl, 0, sizeof(struct v4l2_control));
	ctl.id = V4L2_CID_SHARPNESS;
	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_G_CTRL, &ctl);
	__log("get sharpness %s :%u\n", (ret) ? "fail" : "successfully", ret);
	return ret;
}

int v4l2_set_hue(int hue)
{
	struct v4l2_control ctl;
	int ret;

	if(g_video_ctrl->h_video_dev <= 0)
	{
		return EPDK_FAIL;
	}
	memset(&ctl, 0, sizeof(struct v4l2_control));
	ctl.id = V4L2_CID_HUE;
	ctl.value = hue;
	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_S_CTRL, &ctl);
	__log("set hue:%u %s\n", hue, (ret) ? "fail" : "successfully");
	return ret;
}

int v4l2_get_hue(void)
{
	struct v4l2_control ctl;
	int ret;

	if(g_video_ctrl->h_video_dev <= 0)
	{
		return EPDK_FAIL;
	}
	memset(&ctl, 0, sizeof(struct v4l2_control));
	ctl.id = V4L2_CID_HUE;
	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_G_CTRL, &ctl);
	__log("get hue %s :%u\n", (ret) ? "fail" : "successfully", ret);
	return ret;
}


int v4l2_set_channel(int channel)
{
	int ret = -1;
	if(g_video_ctrl->h_video_dev <= 0)
	{
		return EPDK_FAIL;
	}
	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_S_INPUT, &channel);
	__log("VIDIOC_S_INPUT:%u %s\n", channel, (ret) ? "fail" : "successfully");
	return ret;
}
int v4l2_get_channel(void)
{
	int ret = -1;
	int channel = -1;
	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_G_INPUT, &channel);
	__log("VIDIOC_G_INPUT:%s id=%u\n", (ret) ? "fail" : "successfully", channel);
	return channel;
}

int v4l2_get_system(int* system, u32* format)
{
	struct v4l2_format fmt;
	int ret;

	if(g_video_ctrl->h_video_dev <= 0)
	{
		return EPDK_FAIL;
	}
	memset(&fmt, 0, sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_G_FMT, &fmt);
	if ((fmt.fmt.pix.width == PAL_WIDTH) && (fmt.fmt.pix.height == PAL_HEIGHT))
	{
		*system = FORMAT_PAL;
	}
	else if ((fmt.fmt.pix.width == NTSC_WIDTH) && (fmt.fmt.pix.height == NTSC_HEIGHT))
	{
		*system = FORMAT_NTSC;
	}
	*format = fmt.fmt.pix.pixelformat;
	return ret;
}

int v4l2_set_pixelformat(__u32 pixelformat)
{
	g_pixelformat = pixelformat;
	return EPDK_OK;
}

/*
*********************************************************************************************************
*                                     v4l2_get_frame
*
* Description: get a frame from V4L2
*
* Arguments  : addr: The address here does not require the user to allocate memory. It will assign addr to the frame address of v4l2.
*
*
* Returns    : return EPDK_OK if succeed
*
*********************************************************************************************************
*/
int v4l2_get_frame(unsigned long* addr)
{
	fd_set fds;
	struct timeval tv;
	int r;
	struct v4l2_buffer buf;
	int ret = EPDK_FAIL;
	int ret2;
	static int cnt = 0;
	int i;

	if(g_video_ctrl->h_video_dev <= 0)
	{
		return EPDK_FAIL;
	}

	FD_ZERO(&fds);
	FD_SET(g_video_ctrl->h_video_dev, &fds);

	/* timeout */
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	r = select(g_video_ctrl->h_video_dev + 1, &fds, NULL, NULL, &tv);
	if (r <= 0)
	{
		return EPDK_FAIL;
	}

	memset(&buf, 0, sizeof(struct v4l2_buffer));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_USERPTR;
	ret2 = ioctl(g_video_ctrl->h_video_dev, VIDIOC_DQBUF, &buf);

	for (i = 0;i < FRAME_NUM;i++)
	{
		if ((buf.m.userptr == (unsigned long)buffers[i].start) &&
			(buf.length == buffers[i].length))
		{

			*addr = buf.m.userptr;
			ret = EPDK_OK;
		}
	}
	*addr = buf.m.userptr;
	ret2 = ioctl(g_video_ctrl->h_video_dev, VIDIOC_QBUF, &buf);
	return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     v4l2_update_frame
*
* Description: Get a di processed video frame
*
* Arguments  : addr: This address must be allocated enough space for it！！！！！！
*
*
* Returns    : return EPDK_OK if succeed
*
*********************************************************************************************************
*/
int v4l2_update_frame(unsigned long* addr)
{
	fd_set fds;
	struct timeval tv;
	int r;
	struct v4l2_buffer buf;
	static struct v4l2_buffer temp_buf;
	int ret = EPDK_FAIL;
	static int cnt = 0;
	int i;

	if(g_video_ctrl->h_video_dev <= 0 || g_di_fd <= 0)
	{
		return EPDK_FAIL;
	}
	// FD_ZERO(&fds);
	// FD_SET(g_video_ctrl->h_video_dev, &fds);

	// /* timeout */
	// tv.tv_sec  = 2;
	// tv.tv_usec = 0;
	// r = select(g_video_ctrl->h_video_dev + 1, &fds, NULL, NULL, &tv); //未释放1
	// if (r <= 0)
	// {
	// 	return EPDK_FAIL;
	// }

	memset(&buf, 0, sizeof(struct v4l2_buffer));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_USERPTR;
	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_DQBUF, &buf);
	if (ret != 0)
	{
		return EPDK_FAIL;
	}

	static int is_first_frame = 1;
	if (is_first_frame)
	{
		memcpy((void*)*addr, (void*)buf.m.userptr, buf.length);
		memcpy(&temp_buf, &buf, sizeof(struct v4l2_buffer));
		is_first_frame = 0;
		return EPDK_OK;
	}

// static int test = 0;
// if(test < 80)
// {
// 	FILE* fp = 0;
// 	fp = fopen("F:\\testsssssssss.yuv420_sp","w+");
// 	if(fp != NULL)
// 	{
// 		fwrite(buf.m.userptr,FRAME_PAL_SIZE,1,fp);
// 	}
// 	else{
// 		__log("open file fp is: %p",fp);
// 	}
// 	fclose(fp);
// 	test++;
// }
	di_process(temp_buf.m.userptr, buf.m.userptr, *addr);

	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_QBUF, &temp_buf);
	memcpy(&temp_buf, &buf, sizeof(struct v4l2_buffer));

	return EPDK_OK;
}

/*
*********************************************************************************************************
*                                     di_process
*
* Description: Convert two frames of data into one frame of data with di
*
* Arguments  :  in_buf1: Interlaced data input 1
*				in_buf2: Interlaced data input 2
*				out_buf: Line by line data output
*
* Returns    : return EPDK_OK if succeed
*
*********************************************************************************************************
*/
int di_process(u64 in_buf1, u64 in_buf2, u64 out_buf)
{
	int width, height, size_len, ret;
	if (g_di_fd < 0)
	{
		return -1;
	}
	if (g_video_ctrl->system == FORMAT_PAL)
	{
		width = 720;
		height = 576;
	}
	else
	{
		width = 720;
		height = 480;
	}
	size_len = width * height;

	g_di_arg.size.width = width;
	g_di_arg.size.height = height;

	g_di_arg.in_fb0.size.width = width;
	g_di_arg.in_fb0.size.height = height;
	g_di_arg.in_fb0.buf.addr.y_addr = in_buf1;
	g_di_arg.in_fb0.buf.addr.cb_addr = in_buf1 + size_len;
	g_di_arg.in_fb0.buf.addr.cr_addr = in_buf1 + size_len * 5 / 4;

	g_di_arg.in_fb1.size.width = width;
	g_di_arg.in_fb1.size.height = height;
	g_di_arg.in_fb1.buf.addr.y_addr = in_buf2;
	g_di_arg.in_fb1.buf.addr.cb_addr = in_buf2 + size_len;
	g_di_arg.in_fb1.buf.addr.cr_addr = in_buf2 + size_len * 5 / 4;

	g_di_arg.out_fb0.size.width = width;
	g_di_arg.out_fb0.size.height = height;
	g_di_arg.out_fb0.buf.addr.y_addr = out_buf;
	g_di_arg.out_fb0.buf.addr.cb_addr = out_buf + size_len;
	g_di_arg.out_fb0.buf.addr.cr_addr = out_buf + size_len * 5 / 4;

	g_di_arg.pixel_format = g_pixelformat;//目前仅支持yuv420格式的数据
	ret = ioctl(g_di_fd, DI_IOC_PROCESS_FB, (void*)&g_di_arg);

	return ret;
}

int v4l2_exit(void)
{
	int i;
	int ret = 0;
	if (buffers != NULL)
	{
		for (i = 0; i < FRAME_NUM; i++)
		{
			if (buffers[i].start != NULL)
			{
				__inf("delete start = %p len = %d", buffers[i].start, buffers[i].length);
				free(buffers[i].start);
				buffers[i].start = 0;
				buffers[i].length = 0;
			}
		}
		free(buffers);
		buffers = NULL;
	}
	if (g_video_ctrl->h_video_dev > 0)
	{
		close(g_video_ctrl->h_video_dev);
		g_video_ctrl->h_video_dev = -1;
	}

#if 1
	if (g_di_fd > 0)
	{
		ioctl(g_di_fd, DI_IOC_DESTROY, &g_di_arg.client_number);
		close(g_di_fd);
		g_di_fd = -1;
	}
#endif
	return EPDK_OK;
}




int v4l2_stop(void)
{
	enum v4l2_buf_type type;
	int ret;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_STREAMOFF, &type);
	if (ret == -1)
	{
		__err("VIDIOC_STREAMOFF error\n");
	}

	return ret;
}

int v4l2_start(void)
{
	enum v4l2_buf_type type;
	int ret;
	if(g_video_ctrl->h_video_dev <= 0)
	{
		return EPDK_FAIL;
	}
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_STREAMON, &type);
	if (ret == -1)
	{
		__err("VIDIOC_STREAMON error\n");
	}
	return ret;
}

int v4l2_init(int channel)
{
	struct v4l2_format fmt;
	struct v4l2_requestbuffers req;

	int i, ret = -1;

	if (g_video_ctrl->h_video_dev < 0)
	{
		g_video_ctrl->h_video_dev = open("/dev/video4", O_RDWR | O_NONBLOCK, 0);
	}

	if (g_video_ctrl->h_rotate < 0)
	{
		g_video_ctrl->h_rotate = open("/dev/g2d", O_WRONLY);
		__err("can't open g2d dev");
	}

	/* set input input index */
	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_S_INPUT, &channel);

	memset(&fmt, 0, sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//先随便设置一个参数，后面再改
	fmt.fmt.pix.pixelformat = g_pixelformat;
	if (g_video_ctrl->system == FORMAT_PAL)
	{
		fmt.fmt.pix.width = 720;
		fmt.fmt.pix.height = 576;
	}
	else
	{
		fmt.fmt.pix.width = 720;
		fmt.fmt.pix.height = 480;
	}
	if (ioctl(g_video_ctrl->h_video_dev, VIDIOC_S_FMT, &fmt) == -1)
	{
		__log("set image format failed\n");
	}

	memset(&req, 0x0, sizeof(struct v4l2_requestbuffers));
	req.count = FRAME_NUM;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_USERPTR; //让用户传buf进去，而不是让v4l2进行分配

	ret = ioctl(g_video_ctrl->h_video_dev, VIDIOC_REQBUFS, &req);
	if (ret)
	{
		__log("VIDIOC_REQBUFS ret:%d\n", ret);
	}

	buffers = calloc(req.count, sizeof(video_buffer_t));
	if (buffers == NULL)
	{
		__err("buffers calloc failed");
		return EPDK_FALSE;
	}

	for (i = 0; i < req.count; i++)
	{
		struct v4l2_buffer buf;

		buffers[i].start = malloc(fmt.fmt.pix.width * fmt.fmt.pix.height * 3 / 2);
		buffers[i].length = fmt.fmt.pix.width * fmt.fmt.pix.height * 3 / 2;
		if (buffers[i].start == NULL)
		{
			__err("malloc failed\n");
		}

		memset(&buf, 0, sizeof(struct v4l2_buffer));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_USERPTR;
		buf.index = i;
		buf.m.userptr = (unsigned long)buffers[i].start;
		buf.length = buffers[i].length;
		if (ioctl(g_video_ctrl->h_video_dev, VIDIOC_QBUF, &buf) == -1) //将缓冲帧放入队列
		{
			__err("VIDIOC_QBUF error\n");
		}
		__inf("start = %p len = %d", buffers[i].start, buffers[i].length);
	}

#if 1
	g_di_fd = open("/dev/di", O_WRONLY);
	__log("open di di_fd = %d\n", g_di_fd);

	// struct di_process_fb_arg g_di_arg;
	memset(&g_di_arg, 0, sizeof(struct di_process_fb_arg));
	ioctl(g_di_fd, DI_IOC_GET_CLIENT, &g_di_arg.client_number);
	g_di_arg.is_interlace = 1;
	g_di_arg.field_order = 0; //前场优先 0  , 后场优先 1
	g_di_arg.output_mode = 0; //输出一帧 0 ， 输出两帧  1
	g_di_arg.di_mode = 2;  //DI_INTP_MODE_MOTION(运动检测算法，常用) 2,  DI_INTP_MODE_BOB(场内插值算法，不常用) ：  1
#endif

	return 0;
}




