#include <sys/ioctl.h>
#include <fcntl.h>

#include "usb_test.h"
#include "uvcvideo.h"

static int save_frame_to_file(void *str, void *start, int length)
{
	FILE *fp = NULL;

	fp = fopen(str, "wb+"); //save more frames
	if (!fp) {
		printf(" Open %s error\n", (char *)str);

		return -1;
	}

	if (fwrite(start, length, 1, fp)) {
		fclose(fp);

		return 0;
	} else {
		printf(" Write file fail (%s)\n", strerror(errno));
		fclose(fp);

		return -1;
	}

	return 0;
}

int usb_test_cmd_uvc(int argc, const char **argv)
{
	int fd;
	struct v4l2_capability cap;      /* Query device capabilities */
	struct v4l2_streamparm parms;    /* set streaming parameters */
	struct v4l2_format fmt;          /* try a format */
	struct v4l2_requestbuffers req;  /* Initiate Memory Mapping or User Pointer I/O */
	struct v4l2_buffer buf;          /* Query the status of a buffer */
	enum v4l2_buf_type type;
	int n_buffers;
	char source_data_path[64];
	int np;

	/* 1.open /dev/videoX node */
	fd = open("/dev/video", O_RDWR);

	/* 2.Query device capabilities */
	memset(&cap, 0, sizeof(cap));
	if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
		printf(" Query device capabilities fail!!!\n");
	} else {
		printf(" Querey device capabilities succeed\n");
		printf(" cap.driver=%s\n", cap.driver);
		printf(" cap.card=%s\n", cap.card);
		printf(" cap.bus_info=%s\n", cap.bus_info);
		printf(" cap.version=0x%08x\n", cap.version);
		printf(" cap.capabilities=0x%08x\n", cap.capabilities);
	}

	/* 7.set streaming parameters */
	memset(&parms, 0, sizeof(struct v4l2_streamparm));
	parms.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	parms.parm.capture.timeperframe.numerator = 1;
	parms.parm.capture.timeperframe.denominator = 30;
	if (ioctl(fd, VIDIOC_S_PARM, &parms) < 0) {
		printf(" Setting streaming parameters failed, numerator:%d denominator:%d\n",
			   parms.parm.capture.timeperframe.numerator,
			   parms.parm.capture.timeperframe.denominator);
		close(fd);
		return -1;
	}

	/* 9.set the data format */
	memset(&fmt, 0, sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = 1280;
	fmt.fmt.pix.height = 720;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
		printf(" setting the data format failed!\n");
		close(fd);
		return -1;
	}

	/* 10.Initiate Memory Mapping or User Pointer I/O */
	memset(&req, 0, sizeof(struct v4l2_requestbuffers));
	req.count = 3;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
		printf(" VIDIOC_REQBUFS failed\n");
		close(fd);
		return -1;
	}

	/* 11.Exchange a buffer with the driver */
	for (n_buffers = 0; n_buffers < req.count; n_buffers++) {
		memset(&buf, 0, sizeof(struct v4l2_buffer));

		buf.index = n_buffers;
		if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
			printf(" VIDIOC_QBUF error\n");

			close(fd);
			return -1;
		}
	}

	/* streamon */
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
		printf(" VIDIOC_STREAMON error! %s\n", strerror(errno));
	} else
		printf(" stream on succeed\n");

	np = 0;
	while (np < 5) {
		printf(" camera capture num is [%d]\n", np);

		/* wait uvc frame */
		memset(&buf, 0, sizeof(struct v4l2_buffer));

		if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
			printf(" VIDIOC_DQBUF error\n");

			goto EXIT;
		} else
			printf("*****DQBUF[%d] FINISH*****\n", buf.index);

		sprintf(source_data_path, "/data/source_frame_%d.jpg", np);
		save_frame_to_file(source_data_path, (void *)buf.mem_buf, buf.length);

		if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
			printf(" VIDIOC_QBUF error\n");

			goto EXIT;
		} else
			printf("************QBUF[%d] FINISH**************\n\n", buf.index);

		np++;
	}

	printf("\n\n Capture thread finish\n");

EXIT:
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(fd, VIDIOC_STREAMOFF, &type);

	memset(&req, 0, sizeof(struct v4l2_requestbuffers));
	req.count = 0;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	ioctl(fd, VIDIOC_REQBUFS, &req);

	close(fd);

	return 0;
}