#ifndef __VIDEO_DEV_H__
#define __VIDEO_DEV_H__

#include <kapi.h>
#include <emodules/mod_video_play.h>
#include <emodules/mod_display.h>
#include <log.h>
#include <linux/videodev2.h>
#include <linux/v4l2-controls.h>

int video_dev_init(void);
int video_dev_exit(void);
int video_start_show(void);
int video_stop_show(void);

__s32 Video_Play_MInit(void);
__s32 Video_Play_MExit(void);
__mp* Video_Play_MOpen(__u32 mid, __u32 mode);
__s32 Video_Play_MClose(__mp* mp);
__u32 Video_Play_MRead(void* pdata, __u32 size, __u32 n, __mp* mp);
__u32 Video_Play_MWrite(const void* pdata, __u32 size, __u32 n, __mp* mp);
long  Video_Play_MIoctrl(__mp* mp, __u32 cmd, __s32 aux, void* pbuffer);


typedef struct
{
    __u8                        mid;          // model id  模块ID
    __u32                       t_show_task;  //显示线程ID
    __u32                       t_check_task;  //检查制式线程ID
    __disp_layer_info_t         layer_para;  //显示参数
    int 						lock;
    int 						system;  //当前的制式 PAL还是NTSC
    int 						channel;  //当前视频输入的通道
    int 						frame_width;  //视频帧的宽
    int 						frame_height; //视频帧的高
    int 						scn_width; //窗口的宽
    int 						scn_height; //窗口的高
    int                         h_video_dev; //V4l2的文件描述符
    __mp* h_disp;   //显示句柄
    __u32                       h_lyr;
    int                     	h_rotate;
    __video_play_rotate_t       rotate_angle;
    //rect 						disp_area;
    __u8* disp_buffer;
    __u8* temp_buffer;
    __krnl_event_t* sem_para;
} __video_ctrl_t;

extern  __video_ctrl_t* g_video_ctrl;

#endif

