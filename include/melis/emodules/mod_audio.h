/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                               hello world sample
*
*                                (c) Copyright 2006-2007, Steven.ZGJ China
*                                            All    Rights Reserved
*
* File    : drv_audio.h
* By      : kevin
* Version : V1.00
* Date    : 2009-4-6 18:34
*********************************************************************************************************
*/
#ifndef _MOD_AUDIO_H_
#define _MOD_AUDIO_H_

#include "typedef.h"
#include <kconfig.h>


#define AUDIO_DEVICE_VOLUME_MIN     (0)      /*  PLAY 音频设备音量最小值               */

#ifdef CONFIG_PLAN_SPRITE
#define AUDIO_DEVICE_VOLUME_MAX     (12)    /*  PLAY 音频设备音量最大值（原型机）              */
#else
#define AUDIO_DEVICE_VOLUME_MAX     (60)    /*  PLAY 音频设备音量最大值（开发板）               */
#endif

#define AUDIO_MIC_VOLUME_MIN        (0)      /* 音频设备音量最小值               */
#define AUDIO_MIC_VOLUME_MAX        (25)    /* 音频设备音量最大值               */


#ifndef O_RDONLY
#define O_RDONLY        00000000
#endif
#ifndef O_WRONLY
#define O_WRONLY        00000001
#endif
#ifndef O_RDWR
#define O_RDWR          00000002
#endif
#ifndef O_CREAT
#define O_CREAT         00000100  /* not fcntl */
#endif

typedef enum __AUDIO_DEV_CMD_SET
{
    AUDIO_DEV_CMD_NONE = 0,         /* 音频设备空命令，不做响应                 */
    AUDIO_DEV_CMD_START,            /* 启动音频设备                             */
    AUDIO_DEV_CMD_STOP,             /* 停止音频设备                             */
    AUDIO_DEV_CMD_PAUSE,            /* 暂停音频设备                             */
    AUDIO_DEV_CMD_CONTINUE,         /* 继续音频设备                             */

    AUDIO_DEV_CMD_GET_SAMPCNT = 20, /* 获取音频的采样点数, return = sample count    */
    AUDIO_DEV_CMD_SET_SAMPCNT,      /* 设置音频的采样点数, aux = sample count       */
    AUDIO_DEV_CMD_GET_PARA,         /* 设置用户参数, pbuffer = (__audio_dev_para_t *)   */
    AUDIO_DEV_CMD_SET_PARA,         /* 支持混响的版本改接口无效，采样率根据外部正在播放的歌曲而变化。获取用户参数, pbuffer = (__audio_dev_para_t *)   */
    AUDIO_DEV_CMD_GET_VOLUME,       /* 获取音量, pbuffer = __audio_device_type_t, return = volume                   */
    AUDIO_DEV_CMD_SET_VOLUME,       /* 设置音量, aux = volume, pbuffer = __audio_device_type_t, return = new volume */

    AUDIO_DEV_CMD_REG_USERMODE = 40, /* 音频用户注册用户模式, aux = __audio_play_user_t / __audio_rec_user_t */
    AUDIO_DEV_CMD_CHANGE_IF,        /* 用户改变播放模式，aux = AUDIO_DEV_IF_IIS, AUDIO_DEV_IF_CODEC */
    AUDIO_DEV_CMD_FLUSH_BUF,        /* 清除音频设备用户缓冲区内的音频数据           */
    AUDIO_DEV_CMD_QUERY_BUFSIZE,    /* 查询用户缓冲区相关空间参数,aux = __audio_dev_query_buf_size_type_t   */
    AUDIO_DEV_CMD_RESIZE_BUF,       /* 重新指定用户缓冲区的大小, aux = buffer size, be careful for use      */
    AUDIO_DEV_CMD_CLOSE_DEV,        /* 关闭音设备,会关闭硬件接口，慎用！！！, aux = __audio_device_type_t   */
    AUDIO_DEV_CMD_READ_DATA,        /* 从音频设备读取数据，aux=data size, pbuffer=buffer    */
    AUDIO_DEV_CMD_WRITE_DATA,       /* 向音频设备写入数据，aux=data size, pbuffer=buffer    */
    AUDIO_DEV_CMD_DATA_FINISH,      /* 通知音频设备当前用户的数据已经输入完毕 */

    AUDIO_DEV_CMD_SET_PROTECT_VOL = 60, /* 设置音量保护值, aux = max volume value,(0-16)        */
    AUDIO_DEV_CMD_GET_PROTECT_VOL,  /* 获取音量保护值, return = current max volume value    */

    AUDIO_DEV_CMD_SET_CHAN_MODE,    /* 设置声道模式(左、右、立体声), aux = __audio_dev_chan_mode_t, return = EPDK_OK/EPDK_FAIL  */
    AUDIO_DEV_CMD_GET_CHAN_MODE,    /* 获取声道模式, return = __audio_dev_chan_mode_t               */

    AUDIO_DEV_CMD_ENTER_STANDBY = 80, /* 进入 standby模式       */
    AUDIO_DEV_CMD_EXIT_STANDBY,     /* 退出 standby模式       */

    AUDIO_DEV_CMD_REG_CALLBACK = 90, /* 注册一个回调函数       */
    AUDIO_DEV_CMD_UNREG_CALLBACK,   /* 卸载一个回调函数       */
    AUDIO_DEV_CMD_GET_INTERFACE,    /* 取得当前音频播放的接口  return = __audio_dev_interface_t */

    AUDIO_DEV_CMD_SET_PLAYMODE = 100, /* 当提供给REC使用，设置录音时候是否需要同时播放，默认不播放, aux = __audio_dev_rec_mode_t        */
    /* 当提供给PLAY使用，目前设置SPDIF模式下是否是RAWDATA模式     aux = __audio_dev_spdif_playmode_t  */

    AUDIO_DEV_CMD_SET_DAC_MAX_GAIN = 110,//use "B:\\AUDIO\\PLAY" to set maximum value of Power Amplifier gain to @aux, must <=63
    AUDIO_DEV_CMD_GET_DAC_MAX_GAIN,      //get maximum value of Power Amplifier gain with "B:\\AUDIO\\CTRL"

    AUDIO_DEV_CMD_SET_SW_VOL_MAX,      //use "B:\\AUDIO\\CTRL" to change maximum value of software volume to @aux, this value is the maximum value for end users
    AUDIO_DEV_CMD_GET_SW_VOL_MAX,      //get maximum value of software volume with "b:\\audio\\rec", this value is the maximum value which end users use

    AUDI_DEV_CMD_SET_HPCOM_DRIVE_MODE,      //set HeadPhone driver mode to @aux,  0 means AC mode, 1 means DC mode, other values are reserved(use "B:\\AUDIO\\CTRL")
    AUDIO_DEV_CMD_SWAP_OUTPUT_CHANNELS,     //Swap output channels, if @aux is 1, left dac output to right channel and right dac to left channel. if 0, not swap(use "B:\\AUDIO\\CTRL")

    AUDIO_DEV_CMD_MUTE,//aux: 0: not mute, 1:mute
    AUDIO_DEV_CD_SET_USE_USER_VOLUME_MAP,   /*使用用户声音级别映射pbuffer = 长度为AUDIO_DEVICE_VOLUME_MAX+1的数组，数组下标是软件value,目前不支持record声音映射*/

    AUDIO_DEV_CMD_SET_MIC_PRE_GAIN,     //set MIC pregain val: 0<=aux<=3 0db 35db 38db 41db   default:41db
    AUDIO_DEV_CMD_MIC_PRE_GAIN_ONOFF,   //MIC pregain onoff: aux=0:off  aux=1:on     default:1
    AUDIO_DEV_CMD_GET_MUTE,             //return: 0: not mute, 1:mute

    AUDIO_DEV_CMD_ENABLE_ECHO = 160,    /*设置是否支持混响，aux:为0代表不支持，为1代表支持，默认支持*/
    AUDIO_DEV_CMD_SET_ECHO_BUF_MODE,    /*设置BUFFER模式，aux:0代表不会自动清空buffer(录音应用)，为1代表自动清空buffer(非录音的应用)，默认不会自动清空*/
    AUDIO_DEV_CMD_SET_ECHO_REC_GAIN,    /*(-20, 20)*/
    AUDIO_DEV_CMD_GET_ECHO_REC_GAIN,
    AUDIO_DEV_CMD_SET_ECHO_DELAY_NUM,   /*aux: 0<= aux <= 10 设置混响延时次数，默认为3*/
    AUDIO_DEV_CMD_SET_ECHO_DELAY_TIME,    /*aux:10<= aux <= 3000,默认为100，以毫秒为单位*/
    AUDIO_DEV_CMD_ECHO_SET_PITCH_MODE,  /*该接口暂时不用，CPU跑不够。设置变调模式:
                                        aux: 0xff:用户模式，1:变女声 2:变男声 3:变童声
                                        pbuffer:如果aux=0xff,那么pbuffer为变调等级-8<=pbuffer<=8, 如果aux!=0xff，那么pbuffer无效*/

    AUDIO_DEV_CMD_SET_CAP_RX_SYNC_MODE, /*设置录音RX同步，aux,0:不同步 1:同步*/
    AUDIO_DEV_CMD_ENABLE_HUB_MOD,      /*ahub模式使能，aux,0:关闭 1:打开*/
    AUDIO_DEV_CMD_ENABLE_LOOPLACK,     /*loopback使能，aux,0:关闭 1:打开*/
    AUDIO_DEV_CMD_ENABLE_ECHO_CANCEL,  /*回音消除使能，aux,0:关闭 1:打开*/
    AUDIO_DEV_CMD_ENABLE_ECHO_OUTPUT,  /*使能混响输出，aux，0:关闭混响输出，1:打开混响输出*/
    AUDIO_DEV_CMD_ENABLE_RECORD_ECHO,  /*录音录下来的数据是否带混响，aux，0:不带混响，1:带混响，默认为0*/
    AUDIO_DEV_CMD_SET_SW_GAIN,
    AUDIO_DEV_CMD_GET_SW_GAIN,
    AUDIO_DEV_CMD_ECHO_SET_FIXED_FS,   /*设置混响采样率, aux=48000/44100/24000/22050...，提供在不同场景下设置不同采样率的需要，采样率高了占用CPU资源多，录音或混响效果好*/
    AUDIO_DEV_CMD_DV_MODE,              /*设置DV模式，aux:1代表DV，0代表非DV*/
    AUDIO_DEV_CMD_FORCE_STOP,           /*强制停止录音，如果用AUDIO_DEV_CMD_STOP并不会停止录音DMA，还有资源未释放*/

    AUDIO_DEV_CMD_PLAY_AUX_WAV_FILE = 200,      /*用于在音频播放的时候播放一些附加的wav声音数据，该数据跟原音频数据混音, aux=__audio_play_aux_wav_mode_t, pbuffer=full filename */
    AUDIO_DEV_CMD_PLAY_AUX_WAV_BUFFER,          /*功能同 AUDIO_DEV_CMD_PLAY_AUX_WAV_FILE ，参数不一样，aux=__audio_play_aux_wav_mode_t, pbuffer=(__cedar_pcm_info_t*) */
    AUDIO_DEV_CMD_GET_AUX_WAV_BUFFER_SIZE,   /*获取掌声数据播放buffer大小, ioctrl直接返回buffer size*/
    AUDIO_DEV_CMD_SET_AUX_WAV_BUFFER_SIZE,    /*设置掌声数据播放buffer大小, aux=buffer size*/
    AUDIO_DEV_CMD_GET_AUX_WAV_DATA_SIZE,      /*获取掌声数据缓冲区有效数据长度*/
    AUDIO_DEV_CMD_FOURCE_SET_PARA,            /*强制设置采样率，用于aitalk应用，应用要求采样率必须为16k*/
    AUDIO_DEV_CMD_SET_UNRUN_SAMPLE_SEL,       /*没有audio数据的时候是否静音: aux:0 mute ,  1 not mute*/
    AUDIO_DEV_CMD_PRINT_REGS,                 /*打印DA相关寄存器*/
    AUDIO_DEV_CMD_START_DUMP_PLAY_DAT,
    AUDIO_DEV_CMD_STOP_DUMP_PLAY_DAT,
    AUDIO_DEV_CMD_SET_SPSC_REC_USR_MODE,   /* aux = 1代表为卡拉ok audio rec*/
    AUDIO_DEV_CMD_SET_LR_CHANNEL,           /*设置左右声道模式，aux:0正常，1:左右声道互换*/
    AUDIO_DEV_CMD_ENABLE_REC_LOW_PASS,          /*使能低通滤波，aux为1代表打开，为0代表关闭*/

    AUDIO_DEV_CMD_DRAIN,                    /*等缓冲区数据播完*/

#ifdef CONFIG_SOC_SUN3IW1
    AUDIO_DEV_CMD_SET_LR_EN_BITS,           /*设置DAC左右通道的使能(默认左右通道都打开)，aux:0代表左声道，1代表右声道，pbuffer:0代表关闭，1代表打开*/

    AUDIO_DEV_CMD_GET_REC_AVE_VAL = 240,  /*获取采样平均大小*/
    AUDIO_DEV_CMD_SONIC_INIT,
    AUDIO_DEV_CMD_SONIC_EXIT,
    AUDIO_DEV_CMD_SONIC_SET_CB, //设置回调函数，接收到命令时做什么动作
    AUDIO_DEV_CMD_SONIC_SET_FAIL_CB, //设置回调函数，接收到错误命令时做什么动作


    AUDIO_DEV_CMD_MAGIC_STOP_SET_CB,
    AUDIO_DEV_CMD_MAGIC_START_SET_CB,
    AUDIO_DEV_CMD_MAGIC_SET_VAL,
    AUDIO_DEV_CMD_MAGIC_SET_START_FLAG,
    AUDIO_DEV_CMD_MAGIC_GET_CIRBUF_AND_SIZE,
    AUDIO_DEV_CMD_MAGIC_FLUSH_CIRCLY_BUF,
    AUDIO_DEV_CMD_EXIT,




    //以下几个接口主要适用于需要ADC数据通过I2S输出的情况
    AUDIO_DEV_RECORD_SET, //需要打开的是"AUDIO//REC"
    //只录MIC or FM or LINEIN单一输入的声音不需要设置，录制MIC和本地音乐解码混音的数据时设置
    //aux=0, pbuffer=0   : MIC+本地音乐录制关
    //aux=1, puffer=0:   :MIC+本地音乐录制开
    //aux=0, pbuffer=1     : MIC+FM录制关
    //aux=1, puffer=1:     :MIC+FM录制开
    //aux=0, pbuffer=2     : MIC+LINEIN录制关
    //aux=1, puffer=2:     :MIC+LINEIN录制开

    AUDIO_DEV_CMD_SET_REC_OUTPUT_FLAG, //需要打开的是""b:\\AUDIO\\CTRL""
    //aux=0:录音只录，不输出  aux=1:ADC录音的同时也输出DAC or I2S

    AUDIO_DEV_CMD_SET_OUTPUT_MODE,//需要打开的是""b:\\AUDIO\\CTRL""
    //aux=0:只输出DAC  1:只输出i2s  2:dac+i2s

    AUDIO_DEV_CMD_SET_REC_MODE,//需要打开的是""b:\\AUDIO\\CTRL""
    //0:默认回写到MIC的 pre_buf 供ginkgo录音  1:不需要回写到MIC的pre_Buf


    AUDIO_DEV_CMD_SET_PATH_GAIN,//aux = 0: mic+本地   aux=1:fm+mic or linein+mic



    AUDIO_DEV_CMD_GET_PCM_PITCH,//需要打开的是""b:\\AUDIO\\CTRL""
    AUDIO_DEV_CMD_GET_PCM_BUFFER,//需要打开的是""b:\\AUDIO\\CTRL""

    AUDIO_DEV_CMD_FM_CHANS_SEL,//需要打开的是""b:\\AUDIO\\CTRL""       aux:1 ->left  2->right 3->left &right   pbuffer:0 :off  1:on

#endif
} __audio_dev_cmd_set_t;


typedef enum __AUDIO_DEVICE_TYPE
{
    AUDIO_DEV_NONE = 0,         /* 未定义音频设备                               */
    AUDIO_DEV_PLAY,             /* 音频播放设备                                 */
    AUDIO_DEV_REC,              /* 音频采样设备                                 */
    AUDIO_DEV_FM,               /* FM设备                                       */
    AUDIO_DEV_

} __audio_device_type_t;


typedef enum __AUDIO_PLAY_USER
{
    AUDIO_PLAY_USR_NONE = 0,    /* 音频播放用户未定义模式                       */
    AUDIO_PLAY_USR_MASTER,      /* 用户使用master模式播放音频                   */
    AUDIO_PLAY_USR_SLAVE,       /* 用户使用slave模式播放音频                    */
    AUDIO_PLAY_USR_KEY,         /* 用户使用key tone播放音频                     */
    AUDIO_PLAY_USR_

} __audio_play_user_t;

typedef enum __AUDIO_REC_USER
{
    AUDIO_REC_USR_NONE = 0,    /* 录音用户未定义模式                           */
    AUDIO_REC_USR_LINEIN,      /* 录音用户LINE IN模式                          */
    AUDIO_REC_USR_FMIN,        /* 录音用户FM IN模式                            */
    AUDIO_REC_USR_MIC,         /* 录音用户MIC模式                              */
    AUDIO_REC_USR_MIXER,       /* 录音用户MIXER模式                            */
    AUDIO_REC_USR_
} __audio_rec_user_t;


#ifdef CONFIG_SOC_SUN3IW2
typedef enum __AUDIO_FM_MODE
{
    AUDIO_FM_MODE_LINEIN,
    AUDIO_FM_MODE_FMIN,
    AUDIO_FM_MODE_
} __audio_fm_mode;
#endif

typedef enum __AUDIO_CHN_MODE
{
    AUDIO_CHN_MODE_NONE = 0,    /* 音频通道模式未定义                           */
    AUDIO_CHN_MODE_MONO,        /* 单通道模式                                   */
    AUDIO_CHN_MODE_STEREO,      /* 立体声模式                                   */
    AUDIO_CHN_MODE_

} __audio_chn_mode_t;


//用户参数数据结构
typedef struct __AUDIO_DEV_PARA
{
    __u8        chn;            /* 音频通道数, __audio_chn_mode_t               */
    __u8        bps;            /* 音频采样点位宽， 8/16/24                     */
    __u32       fs;             /* 音频采样率                                   */

} __audio_dev_para_t;


typedef struct __AUDIO_IIS_TO_HDMI
{
    __u16  mclk_div;            /* Mclock除频系数                               */
    __u8   channel;             /* 音频通道数                                   */
    __u8   bitpersample;        /* 音频样本的位宽                               */

} __audio_iis_to_hdmi_t;


typedef enum __AUDIO_DEV_INTERFACE
{
    AUDIO_DEV_IF_NONE = 0,      /* 未定义音频设备接口                           */
    AUDIO_DEV_IF_CODEC,         /* 采用audio codec作为音频设备接口              */
    AUDIO_DEV_IF_IIS,           /* 采用IIS总线作为音频设备接口                  */
    AUDIO_DEV_IF_SPDIF,         /* ????SPDIF??????为??频?璞?涌?                */
    AUDIO_DEV_IF_CODEC_IIS,    /*audio codec and iis 同时????*/
    AUDIO_DEV_INTERFACE_
} __audio_dev_interface_t;

typedef enum __AUDIO_DEV_QUERY_BUF_SIZE_TYPE
{
    AUDIO_DEV_QUERY_BUF_SIZE_NONE = 0,      /* 未定义要获取空间的类型           */
    AUDIO_DEV_QUERY_BUF_SIZE_DATA,          /* 获取缓冲区内的数据size           */
    AUDIO_DEV_QUERY_BUF_SIZE_FREE,          /* 获取缓冲区内的空闲空间size       */
    AUDIO_DEV_QUERY_BUF_SIZE_TOTAL,         /* 获取缓冲区总空间                 */

    AUDIO_DEV_QUERY_BUF_SIZE_

} __audio_dev_query_buf_size_type_t;


typedef enum __AUDIO_DEV_CHAN_MODE
{
    AUDIO_DEV_CHANNEL_STEREO = 0,           /* 立体声模式                       */
    AUDIO_DEV_CHANNEL_LEFT,                 /* 左声模式                         */
    AUDIO_DEV_CHANNEL_RIGHT,                /* 右声模式                         */
    AUDIO_DEV_CHANNEL_EXIT,
    AUDIO_DEV_CHANNEL_

} __audio_dev_chan_mode_t;

typedef enum __AUDIO_DEV_REC_PLAYMODE
{
    AUDIO_DEV_REC_NOPLAY = 0,               /* 录音时候不需要播放               */
    AUDIO_DEV_REC_WITHPLAY,                 /* 录音时候需要播放                 */

    AUDIO_DEV_REC_

} __audio_dev_rec_playmode_t;


#ifdef CONFIG_SOC_SUN3IW1
typedef enum __AUDIO_DEV_SPDIF_PLAYMODE
{
    AUDIO_DEV_SPDIF_NORMALPLAY = 0,         /* SPDIF时?虿シ沤???????            */
    AUDIO_DEV_SPDIF_RAWDATAPLAY,            /* SPDIF时?虿シ?RWDATA????          */
    AUDIO_DEV_SPDIF_
} __audio_dev_spdif_playmode_t;
#endif

typedef struct __AUDIO_PCM_INFO
{
    //input infor
    __s32   SampleRate;
    __u16  *PCMPtr;
    __s32   PcmLen;//sample num;    inputA和inputB在调用前是要mix的sample数,调用后是剩余没有mix的sample数.
    __s32   Chan;  //channel num
    //reserved
    int     preamp;//-20 -- 20 db
} __audio_pcm_info_t;

#ifdef CONFIG_SOC_SUN3IW1
typedef struct sonic_index
{
    __u16    index1; //一???说?
    __u16    index2;//?????说?
} sonic_index_t;

typedef struct pcm_buffer_s
{
    __u8 *pbuf;
    __s32 size;
} pcm_buffer_t;
#endif

//定义AUDIO_PLAY_AUX_WAV_FILE的模式
typedef enum __AUDIO_PLAY_AUX_WAV_MODE
{
    AUDIO_PLAY_AUX_WAV_MODE_APPEND,//如果先前已经有未播放完毕的AUX WAV数据，则当前数据追加到该数据的后面播放
    AUDIO_PLAY_AUX_WAV_MODE_TRUNCATE,//如果先前已经有未播放完毕的AUX WAV数据，则清空先前数据，播放当前设置的数据
    AUDIO_PLAY_AUX_WAV_MODE_MIX,//如果先前已经有未播放完毕的AUX WAV数据，则先前的数据和当前设置的数据进行混音
    AUDIO_PLAY_AUX_WAV_MODE_IGNORE,//如果先前已经有未播放完毕的AUX WAV数据，则忽略当前设置的数据
    AUDIO_PLAY_AUX_WAV_MODE_MAX
} __audio_play_aux_wav_mode_t;

typedef enum
{
    AUDIO_REC_FM_L     = (1<<0),//请勿同时使能FM和LINEIN
    AUDIO_REC_FM_R     = (1<<1),
    AUDIO_REC_LINEIN_L = (1<<2),
    AUDIO_REC_LINEIN_R = (1<<3),
    AUDIO_REC_MIC      = (1<<4),
}__audio_record_channel_t;

#endif /* _DRV_AUDIO_H_ */
