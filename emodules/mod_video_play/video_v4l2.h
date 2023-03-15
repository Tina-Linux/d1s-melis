#ifndef __VIDEO_V4L2_H__
#define __VIDEO_V4L2_H__

typedef struct
{
    void * start;
    unsigned int length;
    long long int timestamp;
}video_buffer_t;

#define FORMAT_PAL      0
#define FORMAT_NTSC		1

#define PAL_WIDTH		720
#define PAL_HEIGHT		576
#define NTSC_WIDTH		720
#define NTSC_HEIGHT		480

// #define FRAME_WIDTH     PAL_WIDTH
// #define FRAME_HEIGHT    PAL_HEIGHT

#define FRAME_PAL_SIZE      (PAL_WIDTH * PAL_HEIGHT * 3 / 2)

#define FRAME_NUM       5

int v4l2_init(int channel);
int v4l2_exit(void);
int v4l2_start(void);

int v4l2_config(int width,int height);
int v4l2_stop(void) ;

int v4l2_get_frame(unsigned long *addr);
int v4l2_update_frame(unsigned long *addr);
int di_process(u64 in_buf1,u64 in_buf2,u64 out_buf);
int v4l2_get_system(int *system, u32 *format);
int v4l2_set_contrast(int contrast);
int v4l2_get_contrast(void);
int v4l2_set_brightness(int brightness);
int v4l2_get_brightness(void);
int v4l2_set_saturation(int saturation);
int v4l2_get_saturation(void);
int v4l2_set_sharpness(int sharpness);
int v4l2_get_sharpness(void);
int v4l2_set_hue(int hue);
int v4l2_get_hue(void);
int v4l2_set_channel(int channel);
int v4l2_get_channel(void);


#endif
