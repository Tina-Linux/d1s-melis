/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                               mod_mmp sub-system
*
*                                      (c) Copyright 2008-2009, kevin China
*                                               All Rights Reserved
*
* File   : mod_ginkgo.h
* Version: V1.0
* By     : kevin
* Date   : 2009-3-21 17:53
*********************************************************************************************************
*/
#ifndef _MOD_GINKGO_H_
#define _MOD_GINKGO_H_

#include <typedef.h>
//#include "cfgs.h"

#include "mod_cedar.h"
#define GINKGO_SPECTRUM_LEVEL           (32)    //spectrum level, __u16 for save every level

typedef enum __GINKGO_CMD_SET           /* 媒体录制中间件命令列表(IOCtrol)                              */
{
    GINKGO_SET_MEDIAFILE = 0x0100,      /* 设置录制的媒体文件信息aux = (__ginkgo_media_file_inf *)      */
    GINKGO_CMD_GET_MESSAGE_CHN,         /* 获取返馈消息通道，return = (__krnl_event_t *)                */
    GINKGO_CMD_GET_ERROR_TYPE,          /* 获取媒体播放器的错误类型,                                    */

    GINKGO_CMD_START = 0x0200,          /* 启动录制                                                     */
    GINKGO_CMD_STOP,                    /* 停止录制                                                     */
    GINKGO_CMD_PAUSE,                   /* 暂停录制                                                     */
    GINKGO_CMD_GET_STATUS,              /* 获取媒体录制中间件当前状态                                   */

    GINKGO_CMD_GET_CUR_TIME = 0x0300,   /* 获取当前录制时间 return = current encode time(ms)            */
    GINKGO_CMD_SET_FILE_FMT,            /* 设置要录制的媒体文件的格式，aux = __ginkgo_media_file_fmt_t  */

    GINKGO_CMD_SET_ABSTYPE = 0x0400,    /* 设置音频流编码格式, aux = __ginkgo_audio_fmt_t, return = new abs type    */
    GINKGO_CMD_GET_ABSTYPE,             /* 获取音频流编码格式 return = __ginkgo_audio_fmt_t             */
    GINKGO_CMD_SET_AUDBPS,              /* 设置音频流的码率 aux = bps to be set, return = bps after set */
    GINKGO_CMD_GET_AUDBPS,              /* 获取音频流的码率 return = bps                                */
    GINKGO_CMD_SET_SAMPRATE,            /* 设置音频采样率   aux = sample rate, return = new sample rate after set   */
    GINKGO_CMD_GET_SAMPRATE,            /* 获取音频采样率   return = sample rate                        */
    GINKGO_CMD_SET_SAMPWIDTH,           /* 设置音频样本宽度   aux = __ginkgo_audio_samp_width_t, return = new value */
    GINKGO_CMD_GET_SAMPWIDTH,           /* 获取音频样本宽度   return = __ginkgo_audio_samp_width_t      */
    GINKGO_CMD_SET_CHN,                 /* 设置通道类型     aux = __ginkgo_audio_chn_t, return = new channel        */
    GINKGO_CMD_GET_CHN,                 /* 获取通道类型     return = __ginkgo_audio_chn_t               */
    GINKGO_CMD_SET_VOL,                 /* 设置录音音量     aux = MIN_VOLUME_VAL~MAX_VOLUME_VAL, return = new vol   */
    GINKGO_CMD_GET_VOL,                 /* 获取录音音量     return = volume                             */
    GINKGO_CMD_SET_AUDIO_SOURCE,        /* 设置录音音源     aux = __ginkgo_audio_src_t                  */
    GINKGO_CMD_GET_AUDIO_SOURCE,        /* 获取录音音源     return = __ginkgo_audio_src_t               */
    GINKGO_CMD_SET_SPECTRUM,            /* 设置是否打开频谱解析， 0-disable; !0-enable                  */
    GINKGO_CMD_GET_SPECTRUM,            /* 获取实时spectrum (i/o)pbuffer = spectrum (return ok or fail) */
    GINKGO_CMD_SET_AUDIOGAIN,             //设置软件录音时的增益值, aux = -20~20dB

    GINKGO_CMD_SET_VBSTYPE = 0x0500,    /* 设置视频流编码格式                                           */
    GINKGO_CMD_GET_VBSTYPE,             /* 获取视频流编码格式                                           */
    GINKGO_CMD_SET_VIDBITRATE,          /* 设置视频流的码率                                             */
    GINKGO_CMD_GET_VIDBITRATE,          /* 获取视频流的码率                                             */
    GINKGO_CMD_SET_VIDFPS,              /* 设置图像帧率     return = frame rate                         */
    GINKGO_CMD_GET_VIDFPS,              /* 获取图像帧率     return = frame rate                         */
    GINKGO_CMD_SET_FRAMESIZE,           /* 设置图像大小     aux = (width << 16) + height                */
    GINKGO_CMD_GET_FRAMESIZE,           /* 获取图象大小     return = (width << 16) + height             */
    GINKGO_CMD_SET_VIDEO_SOURCE,        /* 设置录像视频源     aux = __ginkgo_video_src_t                */
    GINKGO_CMD_GET_VIDEO_SOURCE,        /* 获取录像视频源     return = __ginkgo_audio_src_t             */

    GINKGO_CMD_SET_USER_FILEOP = 0x0600,  /*  设置用户的context,如回调函数,文件长度等, (pbuffer = __cedar_usr_file_op_t *)                                */

    GINKGO_CMD_

} __ginkgo_cmd_set_t;


typedef enum __GINKGO_FEEDBACK_MSG          /* 定义媒体录制中间件反馈给应用程序的消息类型               */
{
    GINKGO_ERROR_NOP                 =   0, /* 空消息                                                   */

    //ginkgo feed back error type
    GINKGO_ERROR_CREATE_FILE_FAIL    = -20, /* 创建媒体文件失败                                         */
    GINKGO_ERROR_FILE_FMT_UNKNOWN,          /* 无法识别当前媒体文件格式                                 */
    GINKGO_ERROR_FILE_NOT_SUPPORT,          /* 不支持当前媒体文件格式录制                               */
    GINKGO_ERROR_NO_FILE_INPUT,             /* 未设置要录制的媒体文件                                   */
    GINKGO_ERROR_WRITE_FILE_FAIL,           /* 写入文件数据失败                                         */
    GINKGO_ERROR_DISK_FULL,                 /* 磁盘满                                                   */

    GINKGO_ERROR_START_PLG_FAIL      = -40, /* 启动插件失败                                             */
    GINKGO_ERROR_AENC_ERROR,                /* 音频编码器错误                                           */
    GINKGO_ERROR_VENC_ERROR,                /* 视频编码器错误                                           */

    GINKGO_ERROR_UNKOWN              = -60, /* 未知错误                                                 */
    GINKGO_ERROR_REQ_MEMORY_FAIL,           /* 申请内存失败                                             */

    //ginkgo feed back status type
    GINKGO_FEDBAK_NO_ERROR           =  1,  /* 空消息                                                   */

    GINKGO_FEDBAK_

} __ginkgo_feedback_msg_t;


typedef enum __GINKGO_STATUS                /* the status of ginkgo             */
{
    GINKGO_STAT_IDLE = 0,                   /* idle                             */
    GINKGO_STAT_RECORD,                     /* record                           */
    GINKGO_STAT_PAUSE,                      /* pause                            */

    GINKGO_STAT_

} __ginkgo_status_t;


typedef enum __GINKGO_MEDIA_FILE_FMT
{
    GINKGO_MEDIA_FILE_FMT_NONE = CEDAR_MEDIA_FILE_FMT_NONE,

    //define audio file format
    GINKGO_MEDIA_FILE_FMT_MP3   = CEDAR_MEDIA_FILE_FMT_MP3, //0x0010,    /* *.mp3, 音频文件                      */
    GINKGO_MEDIA_FILE_FMT_WAV   = CEDAR_MEDIA_FILE_FMT_WAV,           /* *.wav, 音频文件                      */
    //define video file file format
    GINKGO_MEDIA_FILE_FMT_AVI = CEDAR_MEDIA_FILE_FMT_AVI, //0x0020,  /* *.avi, 电影视频文件                  */
    GINKGO_MEDIA_FILE_FMT_MP4 = CEDAR_MEDIA_FILE_FMT_MP4, /* *.mp4, mpeg-4影片视频文件            */

    GINKGO_MEDIA_FILE_FMT_

} __ginkgo_media_file_fmt_t;


typedef enum __GINKGO_AUDIO_FMT
{
    GINKGO_ABS_TYPE_NONE            = CEDAR_ABS_TYPE_NONE, //0,           /* 没有音频流                           */

    GINKGO_ABS_TYPE_ADPCM           = CEDAR_ABS_TYPE_ADPCM,              /* adpcm                                */
    GINKGO_ABS_TYPE_MPEG12_LAYER3   = CEDAR_ABS_TYPE_MPEG12_LAYER3,      /* mp3                                  */
    GINKGO_ABS_TYPE_MPEG_AAC_LC     = CEDAR_ABS_TYPE_MPEG_AAC_LC,        /* aac                                  */
    GINKGO_ABS_TYPE_

} __ginkgo_audio_fmt_t;


typedef enum __GINKGO_VIDEO_FMT
{
    GINKGO_VBS_TYPE_NONE = CEDAR_VBS_TYPE_NONE,           /* 没有视频流                           */

    GINKGO_VBS_TYPE_MJPG = CEDAR_VBS_TYPE_MJPEG,               /* motion jpeg                          */
    GINKGO_VBS_TYPE_XVID = CEDAR_VBS_TYPE_XVID,               /* mpeg4-xvid                           */
    GINKGO_VBS_TYPE_H264 = CEDAR_VBS_TYPE_H264,               /* h264                                 */

    GINKGO_VBS_TYPE_

} __ginkgo_video_fmt_t;


typedef enum __GINKGO_AUDIO_CHN         /* audio channel type                   */
{
    GINKGO_AUDIO_CHN_NONE = 0,          /* none audio channel                   */

    GINKGO_AUDIO_CHN_MONO,              /* mono                                 */
    GINKGO_AUDIO_CHN_STEREO,            /* stereo                               */

    GINKGO_AUDIO_CHN_

} __ginkgo_audio_chn_t;


typedef enum __GINKGO_AUDIO_SAMP_WIDTH
{
    //NOTE:we only support 16bits right now!!!
    GINKGO_AUDIO_SAMP_WIDTH_0   = 0,      /* invalid audio sample width         */

    GINKGO_AUDIO_SAMP_WIDTH_8   = 8,      /* 8 bits audio sample                */
    GINKGO_AUDIO_SAMP_WIDTH_16  = 16,     /* 16 bits audio sample               */
    GINKGO_AUDIO_SAMP_WIDTH_24  = 24,     /* 24 bits audio sample               */
    GINKGO_AUDIO_SAMP_WIDTH_32  = 32,     /* 32 bits audio sample               */

} __ginkgo_audio_samp_width_t;


typedef enum __GINKGO_AUDIO_SRC         /* audio source type                    */
{
    GINKGO_AUDIO_SRC_NONE = 0,          /* none audio source                    */

    GINKGO_AUDIO_SRC_MIC,               /* microphone record                    */
    GINKGO_AUDIO_SRC_LINEIN,            /* line-in record                       */
    GINKGO_AUDIO_SRC_FM,                /* fm record                            */

    GINKGO_AUDIO_SRC_

} __ginkgo_audio_src_t;


typedef enum __GINKGO_VIDEO_SRC         /* video source type                    */
{
    GINKGO_VIDEO_SRC_NONE = 0,          /* none video source                    */

    GINKGO_VIDEO_SRC_SENSOR,            /* sensor record                        */
    GINKGO_VIDEO_SRC_TVIN,              /* TV-in record                         */

    GINKGO_VIDEO_SRC_

} __ginkgo_video_src_t;


typedef struct __GINKGO_AUDIO_INF
{
    __u32       uAbsFmt;                /* 音频码流类型，if(uAbsFmt == GINKGO_ABS_TYPE_NONE), 将
                                           忽略其它相关音频参数                             */
    __u32       uVolume;                /* 录音的音量值，如果设置非法，则采用default值      */
    __u32       uChn;                   /* 录音的声道模式，如果设置非法，则采用default值, __ginkgo_audio_chn_t    */
    __u32       uSampWidth;             /* 录音样本点的位宽，如果设置非法，则采用default值  */
    __u32       uAudioBps;              /* 音频码流的码率，如果设置非法，则采用default值    */
    __u32       uAudioSampRate;         /* 音频码流的采样率，如果设置非法，则采用default值  */
    __u32       uAudioSource;           /* 音频源，如果设置非法，则采用default值, GINKGO_AUDIO_SRC_MIC */

} __ginkgo_audio_inf_t;


typedef struct __GINKGO_VIDEO_INF
{
    __u32       uVbsFmt;                /* 视频流类型，if(uVbsFmt == GINKGO_VBS_TYPE_NONE), 将
                                           忽略其它相关的视频参数                           */
    __u32       uVideoBps;              /* 视频码流的码率,如果设置非法，则采用default值     */
    __u32       uVideoFrmRate;          /* 视频码流的帧率,如果设置非法，则采用default值     */
    __u32       uVideoFrmSize;          /* 视频图像的大小,如果设置非法，则采用default值     */
    __u32       uVideoSource;           /* 视频源，如果设置非法，则采用default值            */

} __ginkgo_video_inf_t;


//the information of the media file which will be input to play
typedef struct __GINKGO_MEDIA_FILE_INF
{
    //要录制的文件格式
    __u32       uFileFmt;               /* 要录制的文件格式, __ginkgo_media_file_fmt_t  */
    char        *file_path;             /* 录制的媒体文件保存的绝对路径名，如果未设置，则保存到default路径  */

    //音频相关信息
    __ginkgo_audio_inf_t    audio_inf;  /* 音频录制格式信息                     */

    //视频相关信息
    __ginkgo_video_inf_t    video_inf;  /* 视频录制格式信息                     */

} __ginkgo_media_file_inf;



#endif  /* _MOD_GINKGO_H_ */


