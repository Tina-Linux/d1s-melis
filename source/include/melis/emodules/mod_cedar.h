/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                               mod_mmp sub-system
*
*                                (c) Copyright 2006-2007, Steven.ZGJ China
*                                               All Rights Reserved
*
* File   : mod_cedar.h
* Version: V1.0
* By     : steven.zgj
* update : kevin.z  2008-11-2 15:43
* update : kevin.z  2008-12-12 16:10
*********************************************************************************************************
*/
#ifndef _MOD_CEDAR_H_
#define _MOD_CEDAR_H_
#include <typedef.h>
#include <misc/fb.h>
#include <ktype.h>
#include <kconfig.h>

#define CEDAR_TAG_INF_SIZE      (240)       //(256)注意:省下的16字节必须也用上       /* AUD_TAG_INF_SIZE, buffer size for audio tag information                    */
#define CEDAR_FPATH_SIZE        (512)    /* 媒体文件路径名长度最大值                                 */

//define variable for audio render interface
#define SPECT_GROUP_WIDTH       (32)    /* 一组频谱内的频段数，应用程序参考该值处理频谱宽度             */
#define MAX_SPEC_GROUP_NUM      (10)    /* 频谱输出的最大组数，应用程序参考该值申请空间                 */
#define USER_DEFINE_EQ_SIZE     (10)    /* 用户自定义EQ时，需要传输的一组参数长度                       */

#define AUDIO_VPS_SLOWEST       (-4)    /* 音频变速播放的最慢速度值                                     */
#define AUDIO_VPS_NOMAL         (0)     /* 音频正常播放速度值                                           */
#define AUDIO_VPS_FASTEST       (10)    /* 音频变速播放的最快速度值                                     */

#define MIN_VOLUME_VAL          (0)     /* 音量最小值，此时为静音状态                                   */
#define MAX_VOLUME_VAL          (100)    /* 音量最大值，只有在音量范围内的值被认为有效，否则为非法值     */

#define CEDAR_FASTEST_FRSPEED   (128)   /* 快进、快退的最大倍速                                         */
#define CEDAR_SLOWEST_FRSPEED   (1)     /* 快进、快退的最小倍速                                         */

#ifdef  CONFIG_SOC_SUN3IW1
#define CEDAR_FASTEST_PITCH     (12)   /* 快进、快退的最大倍速                                         */
#define CEDAR_SLOWEST_PITCH     (-12)     /* 快进、快退的最小倍速*/
#endif

#define CEDAR_FRPIC_HOLDTIME_LEAST      (30)    /* 快进、快退状态下，图片显示停留最小时间，(30ms)       */
#define CEDAR_FRPIC_HOLDTIME_LARGEST    (5000)  /* 快进、快退状态下，图片显示停留最大时间，(5000ms)     */

#define MAX_LANG_CHAR_SIZE  (32)

typedef enum __CEDAR_CMD_SET            /* 媒体播放中间件命令列表(IOCtrol)                              */
{
    CEDAR_CMD_SET_MEDIAFILE = 0x0100,   /* 设置待播放的媒体文件路径及断点信息, pbuffer = (__cedar_media_file_inf *) */
    CEDAR_CMD_GET_MESSAGE_CHN,          /* 获取返馈消息通道，return = __krnl_event_t*, (__cedar_feedback_msg_t)     */
    CEDAR_CMD_GET_ERROR_TYPE,           /* 获取媒体播放器的错误类型, error in __cedar_feedback_msg_t    */

    CEDAR_CMD_PLAY = 0x0200,            /* 播放命令,  */
    CEDAR_CMD_STOP,                     /* 停止播放                                                     */
    CEDAR_CMD_PAUSE,                    /* 暂停播放                                                     */
    CEDAR_CMD_FF,                       /* 快进                                                         */
    CEDAR_CMD_REV,                      /* 快退                                                         */
    CEDAR_CMD_JUMP,                     /* 跳播, aux = time, unit:ms, ret = EPDK_OK/EPDK_FAIL, pbuffer = 1表示要求精确跳播           */
    CEDAR_CMD_GET_STATUS,               /* 获取播放模式,aux = 1表示立即返回(返回状态可能是临时态),aux=0表示等状态稳定再返回, return = __cedar_status_t                   */
    CEDAR_CMD_AUDIO_RAW_DATA_ENABLE,    /* 音频是否是raw data输出。aux = 1:enalbe; aux=0;disable*/
    CEDAR_CMD_SET_STOP_MODE,            //aux = CedarStopMode, stop时是保留所有插件,还是卸载所有插件.aux = CEDAR_STOP_MODE_KEEP_PLUGINS:保留插件,会加快文件切换速度,默认使用保留插件的模式.aux = CEDAR_STOP_MODE_UNINSTALL_PLUGINS:卸载所有插件,在这种模式下,无缝切换无效

    CEDAR_CMD_GET_TOTAL_TIME = 0x300,   /* 获取播放总时间   return = total time(ms)                     */
    CEDAR_CMD_GET_CUR_TIME,             /* 获取当前播放时间 return = current play time(ms)              */
    CEDAR_CMD_GET_TAG,                  /* 获取当前播放时间点的标签信息, pbuffer = __cedar_tag_inf_t*, app分配内存                                 */
    CEDAR_CMD_SET_FRSPEED,              /* 设置快进快退速度 aux    = CEDAR_SLOWEST_FRSPEED ~ CEDAR_FASTEST_FRSPEED  */
    CEDAR_CMD_GET_FRSPEED,              /* 获取快进快退速度 return = speed                              */
    CEDAR_CMD_SET_TAG,                  // 播放前设置tag信息,因为CEDAR_CMD_SET_MEDIAFILE附带了tag信息，所以如果要单独设置，必须在调用CEDAR_CMD_SET_MEDIAFILE或者CEDAR_CMD_SET_USER_FILEOP之后,否则无效

    CEDAR_CMD_GET_ABSTYPE = 0x0400,     /* 获取当前正在播放的音频码流格式 return = __cedar_audio_fmt_t                */
    CEDAR_CMD_GET_AUDBPS,               /* 获取音频流的码率 return = bps                                */
    CEDAR_CMD_GET_SAMPRATE,             /* 获取音频采样率   return = sample rate                        */
    CEDAR_CMD_SET_CHN,                  /* 设置声道类型     aux = __cedar_audio_chn_t                   */
    CEDAR_CMD_GET_CHN,                  /* 获取音频的通道数                                             */
    CEDAR_CMD_SET_VOL,                  /* 设置音量         aux = MIN_VOLUME_VAL~MAX_VOLUME_VAL, return = new vol   */
    CEDAR_CMD_GET_VOL,                  /* 获取音量         return = volume                             */
    CEDAR_CMD_VOLUP,                    /* 音量值上调一档                                               */
    CEDAR_CMD_VOLDOWN,                  /* 音量值下调一档，                                             */
    CEDAR_CMD_SET_EQ,                   /* 设置EQ           aux = __cedar_audio_eq_t                    */
    CEDAR_CMD_GET_EQ,                   /* 获取EQ           return = __cedar_audio_eq_t                 */
    CEDAR_CMD_SET_VPS,                  /* 设置变速播放速度 aux    = -4~10，=0-normal; <0-slow; >0-fast, 以0.1倍为单位变速,因此变速范围为原速的0.6~2倍 */
    CEDAR_CMD_GET_VPS,                  /* 获取变速播放速度 return = -4~10                              */
    CEDAR_CMD_SET_PITCH,                /* 设置变调 aux = -12 ~ 12*/
    CEDAR_CMD_GET_PITCH,                /* 获取音调*/
    CEDAR_CMD_SET_AB_A,                 /* 设置A-B播放的起始点(A点)，仅用于音频文件ab播放                                     */
    CEDAR_CMD_SET_AB_B,                 /* 设置A-B播放的结束点(B点)                                     */
    CEDAR_CMD_SET_AB_LOOPCNT,           /* 设置A-B播放的循环次数                                        */
    CEDAR_CMD_CLEAR_AB,                 /* 取消A-B播放                                                  */
    CEDAR_CMD_SET_SPECTRUM,             /* 设置是否打开频谱解析， 0-disable; !0-enable                  */
    CEDAR_CMD_GET_SPECTRUM,             /* 获取实时spectrum (i/o)pbuffer = spectrum (return ok or fail) */
    CEDAR_CMD_SEL_AUDSTREAM,            /* 选择音频流. aux = 数组下标, ret = EPDK_OK/EPDK_FAIL，表示是否将命令发送给CEDAR.并不表示CEDAR处理成功 */
    CEDAR_CMD_GET_AUDSTREAM,            /* 得到当前正在播放的音频流的下标号, ret = suffix or -1(if not find)*/
    CEDAR_CMD_GET_AUDSTREAM_PROFILE,    /* 获取所有音频流的说明信息: pbuffer = __audstream_profile_t, app malloc it. return = EPDK_OK or EPDK_FAIL*/
    CEDAR_CMD_GET_AUDSTREAM_CNT,         /* 得到音频流的数量， ret = 数量(-1代表失败)，配合CEDAR_CMD_GET_AUDSTREAM_PROFILE_V2一起使用; 如果使用CEDAR_CMD_GET_AUDSTREAM_PROFILE,就不要使用本接口得到音频数量 */
    CEDAR_CMD_GET_AUDSTREAM_PROFILE_V2,  /* 得到所有音频流的说明信息，使用方法同CEDAR_CMD_GET_AUDSTREAM_PROFILE，但pbuffer = __audstream_profile_v2_t，并且AudStrmListArray也由app分配，好处是音频流数量无限制*/
    CEDAR_CMD_QUERY_BUFFER_USAGE,        /* 查询CEDAR内部的缓冲buffer的占用率, 包括vbs buffer,abs buffer，ret = EPDK_OK/EPDK_FAIL, pbuffer = __buffer_usage_t*,app分配内存 */

    //A-B播放第二版的实现接口，和第一版的接口不能混用. 第二版支持音视频文件。第一版只能支持音频文件
    //其中如果音频文件设成FSEEK模式, 那么必须在PLAY状态下设置A,B点, 并且A点的时间点设置无效，cedar会自己确定A点的时间
    //视频文件，音频文件的JUMP模式，对状态没有要求
    CEDAR_CMD_SET_AB_A_V2,                 /* 设置A-B播放的起始点(A点),对cedar状态没有要求，但要在文件打开后设置, aux= APointPts, ms, aux=-1:表示让cedar把当前播放点作为A                                     */
    CEDAR_CMD_SET_AB_B_V2,                 /* 设置A-B播放的结束点(B点),对cedar状态没有要求, 但要在文件打开后设置,aux= BPointPts       aux=-1:表示让cedar把当前播放点作为B                            */
    CEDAR_CMD_SET_AB_LOOPCNT_V2,           /* 设置A-B播放的循环次数, 对cedar状态没有要求,但要在文件打开后设置,                                        */
    CEDAR_CMD_CLEAR_AB_V2,                 /* 取消A-B播放,                                                  */
    CEDAR_CMD_ENABLE_AB_V2,                /*  A-B播放生效，会做一些参数合法性判断,合法才能生效      */
    CEDAR_CMD_SET_AUDIO_AB_MODE_V2,        /*  设置音频文件A-B播放的实现方式,方式0:jump; 方式1为fseek，默认为方式0. 在cedar主控模块打开之后就可以设置 */
    CEDAR_CMD_SET_SWGAIN,        /* 设置声音的软件增益*/
    CEDAR_CMD_GET_SWGAIN,        /* 设置声音的软件增益*/
    CEDAR_CMD_GET_AUDID3INFO,       /*获取音频ID3信息，目前只有M4A接口,__cedar_audio_file_id3_inf_t*/

    CEDAR_CMD_GET_VBSTYPE = 0x0500,     /* 获取视频码流格式                                             */
    CEDAR_CMD_GET_VIDBITRATE,           /* 获取视频流的码率                                             */
    CEDAR_CMD_GET_VIDFPS,               /* 获取图像帧率     return = frame rate                         */
    CEDAR_CMD_GET_FRAMESIZE,            /* frame size       return = (width << 16) + height             */
    CEDAR_CMD_SET_VID_LAYERHDL,         /* 设置视频图层句柄 pbuffer = video_layer_hdl                   */
    CEDAR_CMD_SET_VID_WINDOW,           /* 设置视频窗口属性 pbuffer = (__rect_t *)                      */
    CEDAR_CMD_GET_VID_WINDOW,           /* 获取视频窗口属性 return = (__rect_t *)                       */
    CEDAR_CMD_SET_VID_SHOW_MODE,        /* 设置视频图象显示的模式，aux = __cedar_vide_window_ratio_mode_t, return = 设置后的当前显示模式, 如果 aux = CEDAR_VID_WINDOW_USERDEF, pbuffer = __pCBK_t, 参数类型是SrcScnWinPara */
    CEDAR_CMD_GET_VID_SHOW_MODE,        /* 获取视频图象显示模式，return = __cedar_vide_window_ratio_mode_t  */
    CEDAR_CMD_SWITCH_VID_SHOW,          /* 打开或关闭图片显示，aux = 0:关闭图片显示，aux = 1:打开图片显示   */
    CEDAR_CMD_SET_FRPIC_SHOWTIME,       /* 设置快进快退状态下，视频帧显示的持续时间                     */
    CEDAR_CMD_GET_FRPIC_SHOWTIME,       /* 获取快进快退状态下，图频帧显示的持续时间                     */
    CEDAR_CMD_SET_ROTATE,               /* 设置解码图像旋转的角度.aux = 0:默认为原图,1:顺时针90度,2:180度,3:270度,4:水平镜像,5:垂直镜像,注意事项:只允许在文件播放之前设置，播放过程中(即解码过程中)设置无效*/
    CEDAR_CMD_INVALID_VIDEOLAYER,       /* 通知cedar video layer无效. aux = 1, 无效; aux = 0,恢复有效 */
    CEDAR_CMD_SET_FILE_SWITCH_VPLY_MODE,     //aux = CEDAR_FILE_SWITCH_VPLY_MODE_GAP等,设置文件切换播放时的模式,普通模式要关屏, 无缝模式屏一直不关,有上一个视频文件的最后一帧图像显示直到新文件的第一帧开始显示.在Open cedar之后就设置
    //    CEDAR_CMD_SET_VDRV_SCALE_PARA,      //设置vdrv解码时对原始图像的压缩参数,即解码之后的图像相对于源图像的压缩比率, aux = (width_scale << 16) | (height_scale), width_scale = 2, 4; height_scale = 2. 参数意义: width_scale = 4表示宽度缩小4倍,即解码后图像的宽度是原图的1/4.height_scale以此类推.
    //cedar不记忆该参数.只能在播放前设置,播放过程中设置无效,即在调用CEDAR_CMD_PLAY之前设置参数.如果对scale没有要求,播放前不要调用本接口
    CEDAR_CMD_ENABLE_VIDEO_AUTO_SCALE,   //允许vdrv自动做scale,以减少带宽消耗. aux = 1:enable; aux=0:disable.默认为disable.只允许在播放前设置，播放过程中设置无效.
    CEDAR_CMD_SET_RESERVED_MEM_SIZE,    //设置预留的内存数量,当告诉解码器可用的内存数量时,把这部分内存预留下来.
    CEDAR_CMD_SET_TV_MODE,                    /*aux = 1:TV mode,aux = 0: 本地媒体播放模式，如果带有电视功能录制的TS格式文件则设置为aux = 1*/
    CEDAR_CMD_SET_CTRLSYNC_PLAYMODE,            /*CTRL SYNC PLAY 标记,aux set 1 or 0*/

    //支持3D播放模式
    CEDAR_CMD_GET_VIDEO_DISPLAY_MODE_INFO,           /* 获取视频播放模式信息，__cedar_display_mode_info   */
    CEDAR_CMD_SET_VIDEO_DISPLAY_MODE_OUT,            /* 设置视频播放模式信息，__cedar_display_mode_Out parse模块设定; aux=1,用户定义  */
    CEDAR_CMD_GET_VIDEO_DISPLAY_MODE_OUT,            /* 获取视频播放模式信息，__cedar_display_mode_Out    */

    CEDAR_CMD_ENABLE_DEINTERLACE,               //配置deinterlace功能, aux = 1:enable; aux=0:disable;
    CEDAR_CMD_CONFIG_VIDEO_LYR_CK_ENABLE,       //配置设置给cedar的视频图层的ck_enable参数的值, aux = 1:enable; aux = 0:disable

    CEDAR_CMD_GET_LBSTYPE = 0x0600,     /* 获取字幕码流格式,对于字幕,文件类型和编码类型目前是混为一谈的. ret = __cedar_lyric_fmt_t                                             */
    CEDAR_CMD_GET_SUB_INFO,             /* 获取字幕的条目信息, aux = time, pbuffer = __cedar_get_sub_inf_t, (return = __cedar_subtitle_item_t*或__cedar_buf_inf_t*)                         */
    CEDAR_CMD_GET_SUBTITLE_PROFILE,     /* 获取所有字幕的说明信息:例如，所在的stream_idx,语言等，pbuffer = __subtitle_profile_t, app malloc it. return = EPDK_OK or EPDK_FAIL*/
    CEDAR_CMD_SELECT_SUBTITLE,          /* 选择一个字幕播放，用于切换字幕, aux:字幕下标in __subtitle_profile_t*/
    CEDAR_CMD_GET_SUBTITLE,             /* 得到当前正在播放字幕的下标，if not find, return -1*/
    CEDAR_CMD_GET_SUBTITLE_CNT,         /* 得到字幕流的数量，ret = 数量,ret=-1代表还没有解析出结果,还不知道有几条字幕流.应用程序可以等待一段时间再查, 配合CEDAR_CMD_GET_SUBTITLE_PROFILE_V2使用, 如果使用CEDAR_CMD_GET_SUBTITLE_PROFILE接口，就不要用本接口*/
    CEDAR_CMD_GET_SUBTITLE_PROFILE_V2,  /* 同CEDAR_CMD_GET_SUBTITLE_PROFILE，但使用__subtitle_profile_v2_t，SubtitleStreamArray也由app分配，好处是字幕流数量无限制 */
    CEDAR_CMD_SET_SUBTITLE_ITEM_POST_PROCESS, /* 设置是否进行字幕的后处理，对于文件字幕，后处理是去除控制字符;对于图像字幕，暂时没有后处理, aux=1:进行后处理; aux=0不进行后处理, 最好在CEDAR_CMD_PLAY前设置 */
    CEDAR_CMD_ENABLE_EXTERN_SUBTITLE,       //启用外挂字幕检测, aux = 1:启用; aux =0:屏蔽外挂字幕, 在文件播放前设置,最好在CEDAR_CMD_SET_MEDIAFILE之后就调用该接口，默认检测外挂字幕
    CEDAR_CMD_ENABLE_EMBEDED_SUBTITLE,     //是否激活内置字幕. AUX = 1:激活, aux =0:禁止. 默认激活.

    CEDAR_CMD_CAPTURE_PIC = 0x0700,     /* 已废弃, 截屏, pbuffer = __cedar_pic_info_t     */
    CEDAR_CMD_ASK_PIC_BUFSIZE,          /* 已废弃, 向CEDAR查询需要多少buffer以便APP分配，pbuffer = __cedar_pic_info_t, ret = size, -1代表失败*/
    CEDAR_CMD_GET_FRAME_PIC,            /* 截图，该IO接口是同步操作的. 把当前正在显示的frame的picture截取出来，pbuffer = 目标(FB*)，内存必须已分配好，推荐使用ARGB格式，FB的所有参数必须配正确,cedar不检查 */

    CEDAR_CMD_PLAY_AUX_WAV_FILE = 0x800,    /*用于在音视频播放的时候播放一些附加的wav声音数据，该数据跟解码音频数据混音, aux=__cedar_play_aux_wav_mode_t, pbuffer=full filename */
    CEDAR_CMD_PLAY_AUX_WAV_BUFFER,          /*功能同CEDAR_CMD_PLAY_WAV_FILE，参数不一样，aux=__cedar_play_aux_wav_mode_t, pbuffer=(__cedar_pcm_info_t*) */
    CEDAR_CMD_GET_AUX_WAV_BUFFER_SIZE,      /*获取掌声数据播放buffer大小, ioctrl直接返回buffer size*/
    CEDAR_CMD_SET_AUX_WAV_BUFFER_SIZE,           /*设置掌声数据播放buffer大小, aux=buffer size*/

    CEDAR_CMD_SET_PITCH_LEVEL = 0x810,      /*设置变调比例，aux:变调系数，-12<=aux<=12， default: 0*/
    CEDAR_CMD_GET_PITCH_LEVEL,              /*获取变调比例*/

    //集成缩略图中间件
    CEDAR_DUCKWEED_CMD_OPEN_MEDIAFILE = 0x1100,   /* 打开文件, 没有参数, 文件路径的设置通过CEDAR_CMD_SET_MEDIAFILE或CEDAR_CMD_SET_USER_FILEOP */
    CEDAR_DUCKWEED_CMD_CLOSE_MEDIAFILE,           /* 关闭当前的媒体文件，可能还要关闭解码库 */
    CEDAR_DUCKWEED_CMD_GET_FILE_FORMAT  = 0x1200, /* 得到文件信息，包括文件格式 */
    CEDAR_DUCKWEED_CMD_GET_FILE_SIZE,             /* 得到文件的真实大小, pbuffer = __s64*, ret = EPDK_OK,例如odf加密文件中实际的文件大小*/
    CEDAR_DUCKWEED_CMD_GET_TOTAL_TIME,            /* 获取播放总时间   return = total time(ms)                     */
    CEDAR_DUCKWEED_CMD_GET_VIDSTREAM_CNT,         /* 得到视频流的数量， ret = 数量(-1代表失败) */
    CEDAR_DUCKWEED_CMD_GET_VIDSTREAM_PROFILE,     /* 获取所有视频流的说明信息: pbuffer = __vidstream_profile_v2_t, 并且VidStrmListArray也由app分配. return = EPDK_OK or EPDK_FAIL */
    CEDAR_DUCKWEED_CMD_GET_AUDSTREAM_CNT,         /* 得到音频流的数量， ret = 数量(-1代表失败)*/
    CEDAR_DUCKWEED_CMD_GET_AUDSTREAM_PROFILE,     /* 获取所有音频流的说明信息: pbuffer = __audstream_profile_v2_t, 并且AudStrmListArray也由app分配. return = EPDK_OK or EPDK_FAIL */
    CEDAR_DUCKWEED_CMD_GET_PREVIEW_FB = 0x1300,       /* 得到预览图, pbuffer = FB*，FB的参数有要求,现在只支持拿第一帧，一个文件第一帧解完后，再调该接口，直接返回失败 */
    CEDAR_DUCKWEED_CMD_GET_PREVIEW_FB_BY_PTS,         /* 得到预览图, pbuffer = FB*，FB的参数有要求,aux = pts(ms),表示需要在该PTS附近的关键帧 */

    CEDAR_CMD_SET_USER_FILEOP = 0x05000,     /* 设置用户的context,如回调函数,文件长度等, (pbuffer = __cedar_usr_file_op_t *) */

    CEDAR_CMD_GET_PLY_DATA_STATUS,          /*获取playback中一定时间内是否有数据, 非1: 没有数据到来， 1: 已经有数据了*/
    CEDAR_CMD_STREAM_SET_INFO,
    CEDAR_CMD_STREAM_QUERY_BUFFER,//获取h264/265缓存的还未解码帧的个数
    CEDAR_CMD_STREAM_WRITE_BUFFER,//h264/265裸流数据传输接口，每帧数据前4字节表示帧长度
    CEDAR_CMD_STREAM_RESIZE_BUFFER,
    CEDAR_CMD_STREAM_END,
    CEDAR_CMD_SET_VIDEO_TIME,
    CEDAR_CMD_RESET_SYS_TIME,
    CEDAR_CMD_RAWH264_PLAY_START,
    CEDAR_CMD_RAWH264_PLAY_STOP,
    CEDAR_CMD_RAWH264_SUBMIT_DATA,
    CEDAR_CMD_RAWH264_PIC_SHOW_ONOFF,
    
    CEDAR_CMD_FILE_ENCRYPT_STATUS,//获取音视频文件是否加密，-1获取失败；0非加密；1加密
   	CEDAR_CMD_SET_FILE_ENCRYPT_MODE,//aux设置播放音视频文件加密状态，0只播放非加密文件，1只播放加密文件，2加密非加密均可播放
	CEDAR_CMD_USER_SET_SCALEDOWN,
	CEDAR_CMD_USER_SET_LBC_MODE, //1:1.5x; 2:2x; 3:2.5x;
    CEDAR_CMD_
} __cedar_cmd_set_t;

typedef enum tag_CedarFileSwitchVplyMode
{
    CEDAR_FILE_SWITCH_VPLY_MODE_GAP = 0x00,         //2个文件切换时，隔断的，中间有黑屏
    CEDAR_FILE_SWITCH_VPLY_MODE_SEAMLESS,           //无缝, 不黑屏.
    CEDAR_FILE_SWITCH_VPLY_MODE_,
} CedarFileSwitchVplyMode;

typedef enum tag_CedarStopMode
{
    CEDAR_STOP_MODE_KEEP_PLUGINS = 0x00,         //停止播放时,保留插件
    CEDAR_STOP_MODE_UNINSTALL_PLUGINS,           //停止播放时,卸载所有插件
    CEDAR_STOP_MODE_,
} CedarStopMode;

typedef enum __CEDAR_FEEDBACK_MSG           /* 定义媒体播放中间件反馈给应用程序的消息类型               */
{
    CEDAR_ERROR_NOP                 =   0,  /* 空消息                                                   */

    //cedar feed back error type
    CEDAR_ERROR_OPEN_FILE_FAIL      = -20,  /* 打开当前媒体文件失败                                     */
    CEDAR_ERROR_FILE_FMT_UNKNOWN,           /* 无法识别当前媒体文件格式                                 */
    CEDAR_ERROR_FILE_NOT_SUPPORT,           /* 当前媒体文件不支持                                       */
    CEDAR_ERROR_FILE_DATA_ERROR,            /* 媒体文件数据错，无法解析                                 */
    CEDAR_ERROR_NO_FILE_INPUT,              /* 未设置要播放的媒体文件                                   */
    CEDAR_ERROR_READ_FILE_FAIL,             /* 读取文件数据失败                                         */

    CEDAR_ERROR_START_PLG_FAIL      = -40,  /* 启动插件失败                                             */
    CEDAR_ERROR_ADEC_ERROR,                 /* 音频解码插件解码错误                                     */
    CEDAR_ERROR_VDEC_ERROR,                 /* 视频解码插件解码错误                                     */
    CEDAR_ERROR_TDEC_ERROR,                 /* 字幕解码插件解码错误                                     */

    CEDAR_ERROR_PIC_SIZE_TOO_LARGE  = -60,  /* 图片尺寸太大，无法支持解码                               */
    CEDAR_ERROR_REQ_MEMORY_FAIL,            /* 申请内存失败                                             */

    CEDAR_ERROR_UNKOWN              = -80,  /* 未知错误                                                 */

    //cedar feed back status type
    CEDAR_FEDBAK_NO_ERROR           =  1,   /* 空消息                                                   */
    CEDAR_FEDBAK_PLAY_END,                  /* 播放媒体文件结束                                         */
    CEDAR_FEDBAK_
} __cedar_feedback_msg_t;


typedef enum __CEDAR_VID_WINDOW_RATIO_MODE
{
    CEDAR_VID_WINDOW_NOTCARE = 0x00,        /* 不关心图片显示比例，以当前设置的比例         */
    CEDAR_VID_WINDOW_ORIGINAL,              /* 以图片原始大小在窗口内显示，不能溢出窗口     */
    CEDAR_VID_WINDOW_BESTSHOW,              /* 以图片本身的比例缩放至满窗口显示，图片不变形 */
    CEDAR_VID_WINDOW_FULLSCN,               /* 以窗口的比例缩放图片至满窗口显示，可能会变形 */
    CEDAR_VID_WINDOW_4R3MODE,               /* 强制以4:3的模式满屏显示视频图像，图像会变形  */
    CEDAR_VID_WINDOW_16R9MODE,              /* 强制以16:9的模式满屏显示视频图像，图像会变形 */
    CEDAR_VID_WINDOW_CUTEDGE,               /* 图片本身有黑边，需裁掉                       */
    CEDAR_VID_WINDOW_USERDEF,               //用户自定义,自己确定srcwin, scnwin的参数,需要callback函数
    CEDAR_VID_WINDOW_PAN_SCAN,             /*图像高不裁，只裁宽*/
    CEDAR_VID_WINDOW_
} __cedar_vide_window_ratio_mode_t;

typedef enum __CEDAR_STATUS             /* the status os cedar                  */
{
    CEDAR_STAT_PLAY = 0,                /* play                                 */
    CEDAR_STAT_PAUSE,                   /* pause                                */
    CEDAR_STAT_STOP,                    /* stop                                 */
    CEDAR_STAT_FF,                      /* fast forward                         */
    CEDAR_STAT_RR,                      /* fast backward                        */
    CEDAR_STAT_JUMP,                    /* jump                                 */
    CEDAR_STAT_
} __cedar_status_t;

typedef enum __CEDAR_AUDIO_FMT
{
    CEDAR_ABS_TYPE_NONE = 0,            /* 没有音频流                           */
    CEDAR_ABS_TYPE_UNKNOWN,             /* 有音频流,但无法识别                  */

    CEDAR_ABS_TYPE_MPEG12_LAYER1,
    CEDAR_ABS_TYPE_MPEG12_LAYER2,
    CEDAR_ABS_TYPE_MPEG12_LAYER3,
    CEDAR_ABS_TYPE_MPEG_AAC_LC,
    CEDAR_ABS_TYPE_AC3,                 // AC-3
    CEDAR_ABS_TYPE_DTS,                 // DTS audio for DVD-Video
    CEDAR_ABS_TYPE_LPCM_V,
    CEDAR_ABS_TYPE_LPCM_A,
    CEDAR_ABS_TYPE_ADPCM,
    CEDAR_ABS_TYPE_PCM,                 // pcm
    CEDAR_ABS_TYPE_WMA_STANDARD,        // standard WMA audio
    CEDAR_ABS_TYPE_FLAC,
    CEDAR_ABS_TYPE_APE,
    CEDAR_ABS_TYPE_OGG,
    CEDAR_ABS_TYPE_RAAC,
    CEDAR_ABS_TYPE_COOK,
    CEDAR_ABS_TYPE_SIPR,
    CEDAR_ABS_TYPE_ATRC,
    CEDAR_ABS_TYPE_AMR,
    CEDAR_ABS_TYPE_RA,
    CEDAR_ABS_TYPE_MID,
    CEDAR_ABS_TYPE_ALAC,
    CEDAR_ABS_TYPE_DRA,
    CEDAR_ABS_TYPE_
} __cedar_audio_fmt_t;

typedef enum __CEDAR_VIDEO_FMT
{
    CEDAR_VBS_TYPE_NONE = 0,            /* 没有视频流                           */
    CEDAR_VBS_TYPE_UNKNOWN,              /* 有视频流,但无法识别                  */

    CEDAR_VBS_TYPE_JPEG,
    CEDAR_VBS_TYPE_MJPEG,
    CEDAR_VBS_TYPE_MPEG1_ES,
    CEDAR_VBS_TYPE_MPEG2_ES,
    CEDAR_VBS_TYPE_XVID,
    CEDAR_VBS_TYPE_DIVX3,
    CEDAR_VBS_TYPE_DIVX4,
    CEDAR_VBS_TYPE_DIVX5,
    CEDAR_VBS_TYPE_SORENSSON_H263,
    CEDAR_VBS_TYPE_RMG2,
    CEDAR_VBS_TYPE_RMVB8,
    CEDAR_VBS_TYPE_RMVB9,
    CEDAR_VBS_TYPE_H264,
    CEDAR_VBS_TYPE_VC1,
    CEDAR_VBS_TYPE_AVS,
    CEDAR_VBS_TYPE_H263,
    CEDAR_VBS_TYPE_VP6,
    CEDAR_VBS_TYPE_WMV1,
    CEDAR_VBS_TYPE_WMV2,
    CEDAR_VBS_TYPE_DIVX2,
    CEDAR_VBS_TYPE_DIVX1,
    CEDAR_VBS_TYPE_H265,
    CEDAR_VBS_TYPE_VP8,
    CEDAR_VBS_TYPE_
} __cedar_video_fmt_t;

typedef enum __CEDAR_SUBTITLE_ENCODE
{
    CEDAR_SUB_ENCODE_UNKNOWN = -1, // unknown subtitle encode type

    CEDAR_SUB_ENCODE_NONE = 0,  // none subtitle bitstream
    CEDAR_SUB_ENCODE_BITMAP,    // subtitle encode type is bitmap, 4 colors supported.
    CEDAR_SUB_ENCODE_UTF8,      // subtitle encode type is UTF8
    CEDAR_SUB_ENCODE_GB2312,    // subtitle encode type is GB2312
    CEDAR_SUB_ENCODE_UTF16LE,   // subtitle encode type is UTF16-LE
    CEDAR_SUB_ENCODE_UTF16BE,   // subtitle encode type is UTF16-BE
    CEDAR_SUB_ENCODE_UTF32LE,   // subtitle encode type is UTF32-LE
    CEDAR_SUB_ENCODE_UTF32BE,   // subtitle encode type is UTF32-BE
    CEDAR_SUB_ENCODE_BIG5,      // subtitle encode type is BIG5
    CEDAR_SUB_ENCODE_GBK,       // subtitle encode type is GBK
    CEDAR_SUB_ENCODE_ANSI,      // subtitle encode type is text, unknown __s8acter encode type

    CEDAR_SUB_ENCODE_

} __cedar_subtitle_encode_t;    //字幕文件的字符编码类型或图像

typedef enum __CEDAR_LYRIC_FMT
{
    CEDAR_LRC_TYPE_NONE = 0,            /* 没有字幕/歌词流                      */
    CEDAR_LRC_TYPE_NOTSPT,              /* 有字幕/歌词流,但无法识别             */

    CEDAR_LRC_TYPE_LRC,                 /* LRC格式字幕流                        */
    CEDAR_LRC_TYPE_SRT,                 /* SRT格式字幕流                        */
    CEDAR_LRC_TYPE_KSC,                 /* KSC格式字幕流                        */
    CEDAR_LRC_TYPE_SMI,                 /* SMI格式字幕流                        */
    CEDAR_LRC_TYPE_SSA,                 /* SSA格式字幕流                        */
    CEDAR_LRC_TYPE_TXT,                 /* TXT格式字幕流                        */
    CEDAR_LRC_TYPE_SUB,                 /* SUB格式字幕流                        */
    CEDAR_LRC_TYPE_IDXSUB,              /* IDX+SUB格式字幕流                    */
    CEDAR_LRC_TYPE_ITXT,                /* 内嵌文本格式字幕流(mkv)              */
    CEDAR_LRC_TYPE_ISUB,                /* 内嵌SUB格式字幕流                    */
    CEDAR_LRC_TYPE_ISSA,                /* 内嵌SSA格式字幕流                    */
    CEDAR_LRC_TYPE_PGS,                 /* blueray dvd subtitle for m2ts        */

    //CEDAR_LRC_TYPE_OTHER_I = 0x200,     //其他的内置字幕一律从0x200开始，外挂字幕从0开始
    CEDAR_LRC_TYPE_DVBSUB = 0x200, //这是数字电视广播的dvb subtitle 字幕,也作为内置字幕处理
    CEDAR_LRC_TYPE_ISDB        = 0x201,
    CEDAR_LRC_TYPE_TTXT = 202,  // DVB teletext字幕
    CEDAR_LRC_TYPE_

} __cedar_lyric_fmt_t;


typedef enum __CEDAR_MEDIA_FILE_FMT
{
    CEDAR_MEDIA_FILE_FMT_NONE = 0x0000,

    //define audio file format
    CEDAR_MEDIA_FILE_FMT_MP3 = 0x0001,  /* *.mp3, 音频文件                      */
    CEDAR_MEDIA_FILE_FMT_WAV,           /* *.wav, 音频文件                      */
    CEDAR_MEDIA_FILE_FMT_WMA,           /* *.wmv, Windows media音/视频文件      */
    CEDAR_MEDIA_FILE_FMT_APE,           /* *.ape, APE无损音频                   */
    CEDAR_MEDIA_FILE_FMT_FLAC,          /* *.flac, FLAC音频                     */
    CEDAR_MEDIA_FILE_FMT_OGG,           /* *.ogg, OGG Vorbis音频文件            */
    CEDAR_MEDIA_FILE_FMT_RA,            /* *.ra, Real音频                       */
    CEDAR_MEDIA_FILE_FMT_MP1,           /* *.mp1, mp2/mpa音频                   */
    CEDAR_MEDIA_FILE_FMT_MP2,           /* *.mp2, mp2/mpa音频                   */
    CEDAR_MEDIA_FILE_FMT_AAC,           /* *.aac, MPEG4音频                     */
    CEDAR_MEDIA_FILE_FMT_AC3,           /* *      音频文件                      */
    CEDAR_MEDIA_FILE_FMT_DTS,           /* *.dts, DVD音频                       */
    CEDAR_MEDIA_FILE_FMT_AIF,           /* *.aif, Mac音频                       */
    CEDAR_MEDIA_FILE_FMT_AIFF,          /* *.aiff, Mac音频                      */
    CEDAR_MEDIA_FILE_FMT_AIFC,          /* *.aifc, Mac音频                      */
    CEDAR_MEDIA_FILE_FMT_AMR,           /* *.amr, 移动音频                      */
    CEDAR_MEDIA_FILE_FMT_MAC,           /* *.mac, APE无损音频                   */
    CEDAR_MEDIA_FILE_FMT_TTA,           /* *.tta, TTA音频                       */
    CEDAR_MEDIA_FILE_FMT_M4A,           /* *.m4a, MPEG4音频                     */
    CEDAR_MEDIA_FILE_FMT_CDA,           /* *.cda, CD音频                        */
    CEDAR_MEDIA_FILE_FMT_AU,            /* *.au, Sun压缩音频                    */
    CEDAR_MEDIA_FILE_FMT_ACC,           /* *.acc, 音频                          */
    CEDAR_MEDIA_FILE_FMT_MIDI,          /* *.midi, mid音频文件                  */
    CEDAR_MEDIA_FILE_FMT_MID,           /* *.mid, mid音频文件                   */
    CEDAR_MEDIA_FILE_FMT_RMI,           /* *.rmi, mid音频文件                   */
    CEDAR_MEDIA_FILE_FMT_MP5,           /* *.mp5, 音频文件                      */
    CEDAR_MEDIA_FILE_FMT_MPA,           /* *.mpa, mpeg音频文件                  */
    CEDAR_MEDIA_FILE_FMT_MPGA,          /* *.mpga, mp3音频文件                  */
    CEDAR_MEDIA_FILE_FMT_ACT,           /* *.act, 音频文件                      */
    CEDAR_MEDIA_FILE_FMT_ATRC,          /* *.omg,oma,.aa3, sony openMG音频文件  */
    CEDAR_MEDIA_FILE_FMT_M4R,                /* *.m4a, MPEG4音频*/
    CEDAR_MEDIA_FILE_FMT_DRA,                /* *.dra, 音频文件*/

    //define video file file format
    CEDAR_MEDIA_FILE_FMT_AVI = 0x1000,  /* *.avi, 电影视频文件                  */
    CEDAR_MEDIA_FILE_FMT_FLV,           /* *.flv, flash video视频文件           */
    CEDAR_MEDIA_FILE_FMT_MPEG,          /* *.mpeg, mpeg影片视频文件             */
    CEDAR_MEDIA_FILE_FMT_MPG,           /* *.mpg, mpeg影片视频文件              */
    CEDAR_MEDIA_FILE_FMT_VOB,           /* *.vob, mpeg影片视频文件              */
    CEDAR_MEDIA_FILE_FMT_DAT,           /* *.dat, VCD视频文件                   */
    CEDAR_MEDIA_FILE_FMT_MP4,           /* *.mp4, mpeg-4影片视频文件            */
    CEDAR_MEDIA_FILE_FMT_3GP,           /* *.3gp, 移动视频                      */
    CEDAR_MEDIA_FILE_FMT_MOV,           /* *.mov, QuickTime影片                 */
    CEDAR_MEDIA_FILE_FMT_WMV,           /* *.wmv, Windows media音/视频文件      */
    CEDAR_MEDIA_FILE_FMT_ASF,           /* *.asf, Windows media音/视频文件      */
    CEDAR_MEDIA_FILE_FMT_RM,            /* *.rm, real影片                       */
    CEDAR_MEDIA_FILE_FMT_RMVB,          /* *.rmvb, real影片                     */
    CEDAR_MEDIA_FILE_FMT_RAM,           /* *.ram, real影片                      */
    CEDAR_MEDIA_FILE_FMT_SCM,           /* *.scm, real影片                      */
    CEDAR_MEDIA_FILE_FMT_DIVX,          /* *.divx, 电影视频                     */
    CEDAR_MEDIA_FILE_FMT_MKV,           /* *.mkv, matroska影片视频文件          */
    CEDAR_MEDIA_FILE_FMT_OGM,           /* *.ogm, ogg影片视频文件               */
    CEDAR_MEDIA_FILE_FMT_M4V,           /* *.m4v, mpeg-4影片视频                */
    CEDAR_MEDIA_FILE_FMT_M4P,           /* *.m4p, mpeg-4影片视频                */
    CEDAR_MEDIA_FILE_FMT_M4B,           /* *.m4b, mpeg-4影片视频                */
    CEDAR_MEDIA_FILE_FMT_TP,            /* *.tp, mpeg-2影片视频文件             */
    CEDAR_MEDIA_FILE_FMT_TPR,           /* *.tpr, mpeg影片视频文件              */
    CEDAR_MEDIA_FILE_FMT_TS,            /* *.ts, mpeg-2影片视频文件             */
    CEDAR_MEDIA_FILE_FMT_PVA,           /* *.pva, mpeg影片视频文件              */
    CEDAR_MEDIA_FILE_FMT_PSS,           /* *.pss, mpeg影片视频文件              */
    CEDAR_MEDIA_FILE_FMT_MPE,           /* *.mpe, mpeg影片视频文件              */
    CEDAR_MEDIA_FILE_FMT_WV,            /* *.wv, mpeg影片视频文件               */
    CEDAR_MEDIA_FILE_FMT_M2TS,          /* *.m2ts, m2ts视频                     */
    CEDAR_MEDIA_FILE_FMT_EVO,           /* *.evo, HD-DVD视频                    */
    CEDAR_MEDIA_FILE_FMT_RPM,           /* *.rpm, Real影片                      */
    CEDAR_MEDIA_FILE_FMT_3GPP,          /* *.3gp, 移动视频                      */
    CEDAR_MEDIA_FILE_FMT_3G2,           /* *.3gp, 移动视频                      */
    CEDAR_MEDIA_FILE_FMT_3GP2,          /* *.3gp, 移动视频                      */
    CEDAR_MEDIA_FILE_FMT_QT,            /* *.qt, QuickTime影片                  */
    CEDAR_MEDIA_FILE_FMT_WMP,           /* *.wmp, Windows media音/视频文件      */
    CEDAR_MEDIA_FILE_FMT_WM,            /* *.wm, Windows media音/视频文件       */
    CEDAR_MEDIA_FILE_FMT_AMV,           /* *.avm, 压缩视频                      */
    CEDAR_MEDIA_FILE_FMT_DSM,           /* *.dsm, NDS视频                       */
    CEDAR_MEDIA_FILE_FMT_M1V,           /* *.m1v, 影片视频                      */
    CEDAR_MEDIA_FILE_FMT_M2V,           /* *.m2v, 影片视频                      */
    CEDAR_MEDIA_FILE_FMT_PMP,           /* *.pmp, 影片视频                      */
    CEDAR_MEDIA_FILE_FMT_SMK,           /* *.smk, 影片视频                      */
    CEDAR_MEDIA_FILE_FMT_BIK,           /* *.bik, Bink影片视频                  */
    CEDAR_MEDIA_FILE_FMT_RAT,           /* *.rat, ratDVD影片视频                */
    CEDAR_MEDIA_FILE_FMT_VG2,           /* *.vg2, XVD影片视频                   */
    CEDAR_MEDIA_FILE_FMT_IVF,           /* *.ivf, Indeo影片视频                 */
    CEDAR_MEDIA_FILE_FMT_VP6,           /* *.vp6, VP6影片视频                   */
    CEDAR_MEDIA_FILE_FMT_VP7,           /* *.vp7, VP7影片视频                   */
    CEDAR_MEDIA_FILE_FMT_D2V,           /* *.d2v, DVD2AVI Project影片视频       */
    CEDAR_MEDIA_FILE_FMT_MOD,           /* *.mod, 摄像机格式                    */
    CEDAR_MEDIA_FILE_FMT_M2P,           /* *.m2p, 影片视频                      */
    CEDAR_MEDIA_FILE_FMT_VID,           /* *.vid, mp4手机无声影片视频           */
    CEDAR_MEDIA_FILE_FMT_PMP2,          /* *.pmp2, 影片视频                     */
    CEDAR_MEDIA_FILE_FMT_MTS,           /* *.mts, DV视频                        */
    CEDAR_MEDIA_FILE_FMT_F4V,           /* *.f4v, flash video视频文件           */
    CEDAR_MEDIA_FILE_FMT_H264,          /* *.H264, flash video视频文件          */
    CEDAR_MEDIA_FILE_FMT_H265,          /* *.H265, flash video视频文件          */

    //user define format type
    CEDAR_MEDIA_FILE_FMT_PSR = 0x2000,  /* *.psr, 用户自定义的媒体文件格式      */
    CEDAR_MEDIA_FILE_FMT_DEV = 0x2001,  /* *.dev, 流设备文件*/

    //用户自定义的各种加密格式文件或其他格式
    //CEDAR_MEDIA_FILE_FMT_USR_HGV = 0x2100,  //hgv格式，实际上是avi的video chunk的加密.

    //用户自定义3D文件
    CEDAR_MEDIA_FILE_FMT_3DV = 0x2200,  /* *.3dv, 3DV video视频文件, 维尚自定义3d视频格式 */

    //define unknown format type
    CEDAR_MEDIA_FILE_FMT_UNKNOWN = 0xffff, /* 媒体文件格式无法识别              */
    CEDAR_MEDIA_FILE_FMT_
} __cedar_media_file_fmt_t;

typedef enum __CEDAR_PIC_FMT
{
    CEDAR_PIC_TYPE_NONE = 0,            /* 没有图片流                           */
    CEDAR_PIC_TYPE_NOTSPT,              /* 有图片流,但无法识别                  */
    CEDAR_PIC_TYPE_
} __cedar_pic_fmt_t;


//用户自定义音效滤波器的频段数，用户需要自行设定各频段的滤波系数
#define USR_EQ_BAND_CNT             (10)
#define USR_EQ_NEGATIVE_PEAK_VALUE  (-12)
#define USR_EQ_POSITIVE_PEAK_VALUE  (+12)
typedef enum __CEDAR_AUDIO_EQ           /* 音效类型定义                         */
{
    CEDAR_AUD_EQ_TYPE_NORMAL = 0,       /* 自然                                 */
    CEDAR_AUD_EQ_TYPE_DBB,              /* 重低音                               */
    CEDAR_AUD_EQ_TYPE_POP,              /* 流行                                 */
    CEDAR_AUD_EQ_TYPE_ROCK,             /* 摇滚                                 */
    CEDAR_AUD_EQ_TYPE_CLASSIC,          /* 古典                                 */
    CEDAR_AUD_EQ_TYPE_JAZZ,             /* 爵士                                 */
    CEDAR_AUD_EQ_TYPE_VOCAL,            /* 语言                                 */
    CEDAR_AUD_EQ_TYPE_DANCE,            /* 舞曲                                 */
    CEDAR_AUD_EQ_TYPE_SOFT,             /* 柔和                                 */
    CEDAR_AUD_EQ_TYPE_USR_MODE = 0xff,    /* 用户模式, pbuffer = __s8 UsrEqArray[USR_EQ_BAND_CNT],
                                           每个点的值为:
                                           USR_EQ_POSITIVE_PEAK_VALUE~USR_EQ_NEGATIVE_PEAK_VALUE        */
    CEDAR_AUD_EQ_TYPE_
} __cedar_audio_eq_t;

typedef enum  __CEDAR_3D_OUT_MODE_T
{
    CEDAR_DISP_3D_OUT_MODE_LR = 0x00,//LRLR
    CEDAR_DISP_3D_OUT_MODE_RL = 0x01,//RLRL
    CEDAR_DISP_3D_OUT_MODE_MIX_1 = 0x02,//
    CEDAR_DISP_3D_OUT_MODE_MIX_2 = 0x03,//
} __cedar_3d_out_mode_t;

typedef enum __CEDAR_DISPLAY_MODE_T
{
    CEDAR_DISP_2D_MODE_    = 0,
    CEDAR_DISP_3D_MODE_PLR = 1,
    CEDAR_DISP_3D_MODE_PRL = 2,
    CEDAR_DISP_3D_MODE_PTD = 3,
    CEDAR_DISP_3D_MODE_PDT = 4,
    CEDAR_DISP_3D_MODE_IAB = 5,
    CEDAR_DISP_3D_MODE_IBA = 6,
    CEDAR_DISP_3D_MODE_URL = 7,
    CEDAR_DISP_3D_MODE_MVC = 8,
    CEDAR_DISP_3D_MODE_UNKNOWN
} __cedar_display_mode_t;

//video display mode inforamtion
typedef struct __CEDAR_DISPLAY_MODE_INFO     //3dv视频文件头信息
{
    __cedar_display_mode_t      display_mode;    //   DISP_2D_MODE_；DISP_3D_MODE_PLR；DISP_3D_MODE_PRL
    __u16                       display_width;
    __u16                       display_height;
    __u16                       mvcdisplay_width;
    __u16                       mvcdisplay_height;
    __u32                       reserved[13];
} __cedar_display_mode_info;

typedef struct __CEDAR_DISPLAY_MODE_OUT
{
    __u32                         trd_display_enable;  //  1) 0: 2D显示;  2) 1: 3D显示
    __cedar_3d_out_mode_t         trd_mode;      //  DISP_3D_OUT_MODE_LR/DISP_3D_MODE_RL
    __u32                         half_mode;    //         1) 0: 半幅;  2) 1: 全幅
    __u32                         reserved[5];
} __cedar_display_mode_Out;

typedef enum __CEDAR_GET_SUB_INF
{
    CEDAR_GET_SUB_INF_NONE = 0, //invalid, return no data
    CEDAR_GET_SUB_INF_ALL,      //get all subtitle item information
    CEDAR_GET_SUB_INF_ITEM,     //get subtitle item information according to the given time
    CEDAR_GET_SUB_INF_
} __cedar_get_sub_inf_t;

typedef struct __BUFFER_USAGE
{
    __u8    vbs_flg;            //vbs 是否有效
    __u8    abs_flg;            //abs 是否有效
    __u8    ext_flg[2];         //扩展用
    __s32   vbs_usage;          // vbs占用率, 0~100, 代表0%~100%
    __s32   abs_usage;          // abs占用率, 0~100, 代表0%~100%
    __s32   ext[2];            //扩展用
} __buffer_usage_t;

typedef struct __CEDAR_TAG_INF
{
    __u8    tag_magic[4];            /* 媒体文件标签的标志符'A'/'V','T','A','G' */
    __s32   file_offset;                /* 媒体文件的断点在文件内的偏移         */
    long   time_offset;                /* 媒体文件的断点的时间偏移, ms             */
    long    starttime_offset;           /* 媒体文件第一帧携带的时间（TS格式视频） */
    __u8    decinf[CEDAR_TAG_INF_SIZE]; /* 解码器的现场恢复信息                 */
    __u32   total_time;                 //文件播放的总时间
    __s32   reserved[3];                //将来扩展用
} __cedar_tag_inf_t;    //注意__cedarlib_tag_inf_t

typedef struct __STREAM_BUF_INF
{
    __u8  *buffer;
    __u32 buffer_len;
    __u32 frame_rate; // 0~30
} __stream_buf_inf;

typedef struct __STREAM_INF
{
    __u8 isVideoStream;
    __u32 width;
    __u32 height;
    __u32 frame_rate;
} __stream_inf_t;

//the information of the media file which will be input to play
typedef struct __CEDAR_MEDIA_FILE_INF
{
    __s8    file_path[CEDAR_FPATH_SIZE];/* 媒体文件的绝对路径名                 */
    __s32   default_aud_stream;         //默认音轨
    __u32   tag_inf_validflag;          /* 文件断点信息是否有效的标志, =0(invalid), =1(valid)           */
    __cedar_tag_inf_t   tag_inf;        /* 媒体文件断点播放的相关信息           */
    __stream_buf_inf    stream_buf_inf;
    __stream_inf_t stream_inf;
} __cedar_media_file_inf;

#ifdef CONFIG_SOC_SUN3IW1

typedef struct tag_CedarHerbMediaFileInf
//the setting information for cedar sub-system
{
    //音频播放的相关设置
    __u32       uFileFmt;                   /* 要录制的文件格式,__cedar_media_file_fmt_t                         */
    __s8        file_path[CEDAR_FPATH_SIZE]; /* 录制的媒体文件保存的绝对路径名，如果未设置，则保存到default路径, HERB_FPATH_SIZE  */
    __u8        reserved[2048];              // 保留2k字节,以后扩展
} CedarHerbMediaFileInf;    //用于录制的文件信息

#else

typedef struct __CEDAR_AUDIO_INF
{
    __u32       uAbsFmt;                /* 音频码流类型，if(uAbsFmt == HERB_ABS_TYPE_NONE), 将
                                           忽略其它相关音频参数                             */
    __u32       uVolume;                /* 录音的音量值，如果设置非法，则采用default值,MIN_VOLUME_VAL~MAX_VOLUME_VAL      */
    __u32       uChn;                   /* 录音的声道模式，如果设置非法，则采用default值,HERB_AUDIO_CHN_MONO    */
    __u32       uSampWidth;             /* 录音样本点的位宽，如果设置非法，则采用default值  */
    __u32       uAudioBps;              /* 编码后的音频码流的码率，如果设置非法，则采用default值，单位:bit/s    */
    __u32       uAudioSampRate;         /* 音频码流的采样率，如果设置非法，则采用default值  */
    __u32       uAudioSource;           /* 音频源，如果设置非法，则采用default值            */
    __s32       frame_style;            /* for 0:add head,1:raw data                        */

    //额外的不需要应用程序填写的信息,这些信息由herb内部填写，交给内部的其他模块。
    __s32       sample_cnt_per_frame;   /* 每个audio frame包含的sample的数量 */
} __cedar_audio_inf_t;

typedef struct __CEDAR_VIDEO_INF
{
    __u32       uVbsFmt;                /* 视频流类型, if(uVbsFmt == SW_CODEC_TYPE_NONE), 将忽略其它相关的视频参数, __herb_codec_t */
    __s32       nWidth;                 /* 要编码的视频图像的宽度 */
    __s32       nHeight;                /* 要编码的视频图像的高度 */
    __u32       uVideoFrmRate;          /* 视频码流的帧率,如果设置非法，则采用default值, 数值放大1000倍     */
    __s32       nVideoBps;              /* 视频码流的码率,如果设置非法，则采用default值，单位:bit/s     */
    __s32       color_format;           /* yuv, bayerRGB(4种), PIXEL_YUV420*/
    __s32       color_space;            /* BT601, bt709*/
    __s16       qp_max;
    __s16       qp_min;
    __s32       maxKeyInterval;         /*keyframe Interval，指2个关键帧之间的P帧的最大数量             */
    //__u32     uVideoSource;           /* 视频源，如果设置非法，则采用default值                      */
} __cedar_video_inf_t;

typedef struct tag_CedarHerbMediaFileInf
{
    //要录制的文件格式
    __u32       uFileFmt;                   /* 要录制的文件格式,__cedar_media_file_fmt_t                         */
    __s8        file_path[CEDAR_FPATH_SIZE]; /* 录制的媒体文件保存的绝对路径名，如果未设置，则保存到default路径, HERB_FPATH_SIZE  */
    //音频相关信息
    __cedar_audio_inf_t    audio_inf;        /* 音频录制格式信息                                         */

    //视频相关信息
    __cedar_video_inf_t    video_inf;        /* 视频录制格式信息,参考__video_encode_format_t             */

    //标题，时间等
    __s8        title[CEDAR_FPATH_SIZE];
    __s32       create_time;                /* 数字的规格定义，由mux决定，应用程序遵守                  */
    __u8        reserved[2048];              // 保留2k字节,以后扩展
} CedarHerbMediaFileInf;    //用于录制的文件信息

#endif

//the setting information for cedar sub-system
//typedef struct __CEDAR_SETTING_INF
//{
//音频播放的相关设置
//    __u32       volume;                     /* 音量值, 只是记录,实际无用                           */
//    __u32       eq;                         /* 音效类型, 在playmediafile()和换音轨时设给ardr      */
//    __u32       channel;                    /* 声道模式,无用                         */
//    __u32       vpsspeed;                   /* 播放速度, 在playmedifile(),换音轨,和变速设置时直接设给ardr                         */
//    __u32       ab_loopcnt;                 /* A-B播放循环次数,在playmedifile()和换音轨时设给adec,实际上已经无用了.                  */
//    __u32       spectrum;                   /* 是否使能频谱解析,打开文件，换音轨时,会设置ardr                 */
//    __s32     swgain;          //软件音量值
//    __s16       usr_eq_filter[USR_EQ_BAND_CNT]; /* 用户自定义音效的滤波器频段的滤波系数 */
//
//    __u8        volume_flag;                /* volume参数是否有效的标志,现在置1,实际无用         */
//    __u8        eq_flag;                    /* eq参数是否有效的标志,现在置1,永远有效             */
//    __u8        channel_flag;               /* channel参数是否有效的标志,置1,实际无用        */
//    __u8        vpsspeed_flag;              /* vpsspeed参数是否有效的标志,置1,实际没用       */
//    __u8        ab_loopcnt_flag;            /* ab_loopcnt参数是否有效的标志,实际上无用了     */
//    __u8        spectrum_flag;              /* spectrum参数是否有效的标志,置1,实际没用       */
//    __s32       aud_suffix;                 /* 当前播放的音频流在音频流数组的下标,无用   */

//视频播放的相关设置
//    __s32       vid_layer_hdl;              /* 视频物理图层句柄                 */
//    __rect_t    vid_window;                 /* 视频图像窗口属性, 视频窗体的信息,即在屏幕上划定的用于显示的区域的位置和宽高 */
//    __s32       vid_show_mode;              /* 视频图像显示比例模式             */
//    __u32       frpic_hold_time;            /* 快进、快退状态下视频图片的停留时间                   */

//    __s8        vid_layer_hdl_flag;         /* vid_layer_hdl参数是否有效的标志                      */
//    __s8        vid_window_flag;            /* vid_window参数是否有效的标志                         */
//    __s8        vid_show_mode_flag;         /* vid_show_mode参数是否有效的标志                      */
//    __s8        frpic_hold_time_flag;       /* frpic_hold_time参数是否有效的标志                    */

//音、视频播放共享的相关设置
//    __u32       frspeed;                    /* 快进、快退速度                                       */
//    __u8        frspeed_flag;               /* frspeed参数是否有效的标志                            */

//} __cedar_setting_inf_t;


//audio format information
typedef struct  __AUDIO_BS_INFO
{
    __s32       codec_type;     //__cedar_audio_fmt_t
    __s32       channels;
    __s32       bits_per_sample;
    __s32       sample_per_second;
    __s32       avg_bit_rate;
    __s32       max_bit_rate;
    __s32       data_encode_type;    //__cedar_subtitle_encode_t,表示lang[]的字符编码类型.
    __u8        lang[MAX_LANG_CHAR_SIZE];
#ifndef CONFIG_SOC_SUN3IW1
    __s32       nBlockAlign;
#endif
} __audio_bs_info_t;


//video format inforamtion
typedef struct VIDEO_BS_INFO
{
    __u16       codec_type; //__cedar_video_fmt_t, CEDAR_VBS_TYPE_VC1
    __u16       width;
    __u16       height;
    __u16       frame_rate;
    __s32       avg_bit_rate;
    __s32       max_bit_rate;
} __video_bs_info_t;
typedef struct
{
    __u8        nVidStrmNum;    //video number, <= nVidStrmMaxCnt
    __s32       nVidStrmMaxCnt; //数组VidStrmListArray的元素个数
    __video_bs_info_t        *VidStrmListArray;
} __vidstream_profile_v2_t;


#define PALETTE_CNT         (32)
typedef struct
{
    __s8    valid_flg;
    __s32   palette_num;        //valid palette element
    __s32   palette_ary[PALETTE_CNT];//RGB格式的调色板
} __palette_t;

//typedef enum tag_CEDAR_SUBTITLE_SOURCE{
//    CEDAR_SUBTITLE_POS_NONE   = 0,    //no subtitle exist
//    CEDAR_SUBTITLE_POS_EMBED  = 1,    //embeded subtilte in video file
//    CEDAR_SUBTITLE_POS_EXT    = 2,    //extern subtitle in subtitle file
//    CEDAR_SUBTITLE_POS_,
//} __cedar_subtitle_source_t;

//subtitle format information
typedef struct
{
    __s32       subtitle_coming_type;  //embeded subtitle or extern subtitle, __cedar_subtitle_source_t(__subtitle_source_t)
    __s32       subtitle_type;  // srt, sub, etc.__cedar_lyric_fmt_t
    __palette_t palette_info;   //如果是SUB格式的字幕，需要该信息
    __s32       stream_idx;     //in one file(e.g,sub),there will exist several streams, but in srt file, stream_idx means nothing.
    __s32       data_encode_type;    //__cedar_subtitle_encode_t
    __u8        lang[MAX_LANG_CHAR_SIZE];
} __subtitle_stream_info_t;

#define MAX_AUDIO_STREAM_NUM    (16)    //(4), (16)
//information for audio stream.
typedef struct
{
    __u8        nAudStrmNum;    //audio number.<=MAX_AUDIO_STREAM_NUM.
    __audio_bs_info_t        AudStrmList[MAX_AUDIO_STREAM_NUM];
} __audstream_profile_t;

typedef struct
{
    __u8        nAudStrmNum;    //audio number, <= nAudStrmMaxCnt
    __s32       nAudStrmMaxCnt; //数组AudStrmListArray的元素个数
    __audio_bs_info_t        *AudStrmListArray;
} __audstream_profile_v2_t;

typedef enum cedar_id3_img_format
{
    CEDAR_AUDID3_IMG_FORMAT_BMP = 0,
    CEDAR_AUDID3_IMG_FORMAT_JPG,
    CEDAR_AUDID3_IMG_FORMAT_GIF,
    CEDAR_AUDID3_IMG_FORMAT_PNG,
    CEDAR_AUDID3_IMG_FORMAT_UNSUPPORT = -1
} __cedar_id3_img_format_t;

typedef struct cedar_audio_id3_pic_info_t
{
    __u32               length;      // 0 means no picture. The length of the picture info in the file.
    __u32               offset;      // The offset of the picture info in the file.
    __u32               format;      // jpg, bmp, and so on  __cedar_id3_img_format_t
} __cedar_audio_id3_pic_info_t;

#define CEDAR_ID3_SIZE   64
typedef struct cedar_audio_file_id3_inf
{
    __u32                 title_len;
    char                  title[CEDAR_ID3_SIZE];        // empty string : title is unknown.
    __u32                 title_charset;                //__cedar_subtitle_encode_t

    __u32                 artist_len;
    char                  artist[CEDAR_ID3_SIZE];      // empty string : artist is unknown.
    __u32                 artist_charset;               //__cedar_subtitle_encode_t

    __u32                 album_len;
    char                  album[CEDAR_ID3_SIZE];        // empty string : album is unknown.
    __u32                 album_charset;                //__cedar_subtitle_encode_t

    __u32                 genre_len;
    char                  genre[CEDAR_ID3_SIZE];        // empty string : genre is unknown.
    __u32                 genre_charset;                //__cedar_subtitle_encode_t

    __u32                 year_len;
    char                  year[CEDAR_ID3_SIZE];          // empty string : day is unknown.
    __u32                 year_charset;                  //__cedar_subtitle_encode_t

    __cedar_audio_id3_pic_info_t  album_cover_info;
} __cedar_audio_file_id3_inf_t;

//information for lyric/sutitle item
typedef struct
{
    __u8        backpixelColorCode;              //background pixel colorcode
    __u8        patpixelColorCode;               //pattern pixel colorcode
    __u8        pixel1ColorCode;                 //emphasis pixel-1 colorcode
    __u8        pixel2ColorCode;                 //emphasis pixel-2 colorcode
} __colorcode_t;


typedef struct
{
    __u8        backpixelContrast;              //background pixel contrast
    __u8        patpixelContrast;               //pattern pixel contrast
    __u8        pixel1Contrast;                 //emphasis pixel-1 contrast
    __u8        pixel2Contrast;                 //emphasis pixel-2 contrast
} __contrast_t;


typedef struct __CEDAR_SUBTITLE_ITEM
{
    __u32       uPts;           // time stamp of the subtitle item, (ms)
    __u32       uDuration;      // duration of the item, (ms)
    __u16       uDataType;      // subtitle data type, text,bitmap,encode type,字幕项内容
    //__cedar_subtitle_encode_t
    __s32       nPicWidth;      //如果是SUB等图像格式的字幕，需要传送宽高信息
    __s32       nPicHeight;
    __colorcode_t   colorCode;  //如果是SUB等图像格式的字幕，需要传送color code以便从调色板选择颜色
    __contrast_t    contrast;   //如果是SUB等图像格式的字幕，需要传送contrast 以确定是否显示
    __s32       uPtsSize;       // size of the time stamp offset
    __s32       uSubSize;       // size of the subtitle data
    __s16       *pPtsOffset;    // time stamp offset of every __s8acter
    __u8        *pSubtitle;     // subtitle data buffer

} __cedar_subtitle_item_t;

//information for all lyric stream's description in videofile.
#define SUBTITLE_MAX_NUM    (16)
typedef struct
{
    __u8        nSubtitleStrmNum;    //lyric number.<=SUBTITLE_MAX_NUM.
    __subtitle_stream_info_t        SubtitleStreamList[SUBTITLE_MAX_NUM];
} __subtitle_profile_t;

typedef struct
{
    __u8        nSubtitleStrmNum;    //lyric number.<=nSubtitleStrmMaxCnt.
    __s32       nSubtitleStrmMaxCnt;    //数组SubtitleStreamArray的元素个数
    __subtitle_stream_info_t    *SubtitleStreamArray;
} __subtitle_profile_v2_t;
typedef enum MAPLE_SUBTITLE_COLOR_MODE
{
    MAPLE_SUB_COLOR_BLACK       = 0x80,
    MAPLE_SUB_COLOR_RED         = 0x81,
    MAPLE_SUB_COLOR_GREEN       = 0x82,
    MAPLE_SUB_COLOR_YELLOW      = 0x83,
    MAPLE_SUB_COLOR_BLUE        = 0x84,
    MAPLE_SUB_COLOR_MAGENTA     = 0x85,
    MAPLE_SUB_COLOR_CYAN        = 0x86,
    MAPLE_SUB_COLOR_WHITE       = 0x87,
    MAPLE_SUB_COLOR_
} maple_subtitle_color_mode_t;
typedef enum MAPLE_SUBTITLE_STYLE_SIZE_MODE
{
    MAPLE_SUB_STYLE_SMALL_SIZE        = 0x88,
    MAPLE_SUB_STYLE_MIDDLE_SIZE       = 0x89,
    MAPLE_SUB_STYLE_NORMAL_SIZE       = 0x8a,
    MAPLE_SUB_STYLE_SIZE
} maple_subtitle_style_size_t;
typedef enum MAPLE_SUBTITLE_DISPLAY_MODE
{
    MAPLE_SUB_DISPLAY_MODE_HD_SIDE            = 0x01,
    MAPLE_SUB_DISPLAY_MODE_SD_SIDE            = 0x02,
    MAPLE_SUB_DISPLAY_MODE_SD_WIDE_SIDE       = 0x03,
    MAPLE_SUB_DISPLAY_MODE_MOBILE             = 0x04,
    MAPLE_SUB_DISPLAY_MODE_UNKNOW,
    MAPLE_SUB_DISPLAY_MODE_
} maple_subtitle_display_mode_t;
#define ISDBDEC_MAX_LINE_NUM          10   //一个字幕项的最大行数

#define DVBSUBDEC_MAX_LINE_NUM        10   //一个字幕项的最大行数
#define DVBTTXDEC_MAX_LINE_NUM         10

//picture info
typedef struct
{
    __s32   pic_type;   //图像格式，JPEG, BMP等， IMG_FORMAT_BMP
    SIZE    pic_area;   //指定要求的图像宽高, 必须是4的倍数
    __s32   pic_size;   //sizeof(pic_priv)
    __s32   valid_len;  //转换后的图像文件实际占用的buffer大小, <= pic_size
    void    *pic_priv;  //必须物理连续，PALLOC，存储转换好的图像
} __cedar_pic_info_t;

//define cedar buffer parmater
typedef struct __CEDAR_BUF_INF
{
    void        *buf;           // buffer pointer, __cedar_subtitle_item_t为单位.
    __u32       size;           // item size, based on item, item size is user define
} __cedar_buf_inf_t;

typedef struct __CEDAR_USR_FOP
{
    unsigned long                    fp;                            /*文件句柄*/
    __u32                    flag;
    __s64                    file_size;                     /*文件长度*/
    __cedar_media_file_fmt_t media_fmt;                     /*多媒体文件文件格式*/
    __s8                    *pFilePath;                     /*文件路径+文件名, 满足cedar搜寻外挂字幕的需要*/
    __pCBK_t                 usr_fread;                     /*回调函数(读文件)*/
    __pCBK_t                 usr_fwrite;                    /*回调函数(读文件)*/
    __pCBK_t                 usr_fseek;                     /*可以不实现,在usr_fread统一处理就可以.*/
    __u32                    reserved[2];                   /*保留,请设置为0,*/
} __cedar_usr_file_op_t;

typedef struct
{
    unsigned long      fp;
    __s32      size;
    __s64      pos;
    void      *buffer;
} __cedar_usr_file_op_para_t;

typedef struct tag_SrcScnWinPara
{
    __s32       nSrcPixRatio;   //原图的像素宽高比,1000表示1:1.
    __rect_t    SrcWin; //原图的区域, app做好换算后, 转换后的值也填在这里
    __rect_t    ScnWin; //显示区域, app做好换算后, 转换后的值也填在这里
} SrcScnWinPara; //给CEDAR_CMD_SET_VID_SHOW_MODE使用的callback函数的数据结构.


typedef  struct __CEDAR_PCM_INFO
{
    __s32 SampleRate;
    __u16   *PCMPtr;
    __s32 PcmLen;//sample num;    inputA和inputB在调用前是要mix的sample数,调用后是剩余没有mix的sample数.
    __s32 Chan;  //channel num
    int     preamp;//-20 -- 20 db
} __cedar_pcm_info_t;
typedef enum __CEDAR_PLAY_AUX_WAV_MODE
{
    CEDAR_PLAY_AUX_WAV_MODE_APPEND,//如果先前已经有未播放完毕的AUX WAV数据，则当前数据追加到该数据的后面播放
    CEDAR_PLAY_AUX_WAV_MODE_TRUNCATE,//如果先前已经有未播放完毕的AUX WAV数据，则清空先前数据，播放当前设置的数据
    CEDAR_PLAY_AUX_WAV_MODE_MIX,//如果先前已经有未播放完毕的AUX WAV数据，则先前的数据和当前设置的数据进行混音
    CEDAR_PLAY_AUX_WAV_MODE_IGNORE,//如果先前已经有未播放完毕的AUX WAV数据，则忽略当前设置的数据
    CEDAR_PLAY_AUX_WAV_MODE_MAX
} __cedar_play_aux_wav_mode_t;
typedef enum __CEDAR_AUDIO_DEV_QUERY_BUF_SIZE_TYPE
{
    CEDAR_AUDIO_DEV_QUERY_BUF_SIZE_NONE = 0,      /* 未定义要获取空间的类型           */
    CEDAR_AUDIO_DEV_QUERY_BUF_SIZE_DATA,          /* 获取缓冲区内的数据size           */
    CEDAR_AUDIO_DEV_QUERY_BUF_SIZE_FREE,          /* 获取缓冲区内的空闲空间size       */
    CEDAR_AUDIO_DEV_QUERY_BUF_SIZE_TOTAL,         /* 获取缓冲区总空间                 */
    CEDAR_AUDIO_DEV_QUERY_BUF_SIZE_
} __cedar_audio_dev_query_buf_size_type_t;

typedef enum __CEDAR_STREAM_QUERY_BUF_SIZE_TYPE
{
    CEDAR_STREAM_QUERY_BUF_SIZE_NONE = 0,      /* 未定义要获取空间的类型           */
    CEDAR_STREAM_QUERY_BUF_SIZE_DATA,          /* 获取缓冲区内的数据size           */
    CEDAR_STREAM_QUERY_BUF_SIZE_FREE,          /* 获取缓冲区内的空闲空间size       */
    CEDAR_STREAM_QUERY_BUF_SIZE_TOTAL,         /* 获取缓冲区总空间                 */
    CEDAR_STREAM_QUERY_BUF_SIZE_
} __cedar_STREAM_query_buf_size_type_t;

#endif

