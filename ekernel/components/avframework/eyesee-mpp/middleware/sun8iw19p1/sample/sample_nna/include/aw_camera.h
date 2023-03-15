/*
 * =====================================================================================
 *
 *       Filename:  csi_mplane.h
 *
 *    Description:  get camera data
 *
 *        Version:  1.0
 *        Created:  2019年08月08日 09时13分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jilinglin
 *        Company:  allwinnertech.com
 *
 * =====================================================================================
 */
#include "sunxi_camera_v2.h"
#include "sunxi_display2.h"

struct size {
	int width;
	int height;
};

typedef struct camera_buf {
	void *start[3];
	int length[3];
	int planes;
	int buf_id;
} camera_buf;

typedef struct v4l2_buf {
	struct v4l2_buffer vbuf;
    void *start[3];
	int length[3];
} v4l2_buf;

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
	int addr_map;
	int width, height;/* screen size */
	int dispfh;/* device node handle */
	int fh;/* picture resource file handle */
	int mem;
	int clear;/* is clear layer */
	char filename[32];
	int full_screen;
	unsigned int pixformat;
	disp_layer_config layer_config;
	disp_output_type output_type;
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

typedef struct AwCamera {
	struct tvd_dev dev;
	static char path_name[20];
	static char dev_name[20];
	static int fd = -1;
	static int isp0_fd = -1;
	static int isp1_fd = -1;

    v4l2_buf *buffers;
	unsigned int buffers_num;

	struct size input_size;

	unsigned int read_num;
	unsigned int count;
	unsigned int nplanes;
	unsigned int save_flag;
	int dev_id;
	unsigned int fps;
	unsigned int wdr_mode;

} AwCamera;

AwCamera* AwCameraCreate();
int AwCamerainit(AwCamera* self);
