/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                               mod_mmp sub-system
*
*                                (c) Copyright 2007-2009, SoftWinners,
*                                               All Rights Reserved
*
* File   : eLIBs_GetAudioFmt.h
* Version: V1.0
* By     : lszhang
* Modify : kevin.z.m, 2009-11-26 19:45
*********************************************************************************************************
*/
#ifndef _ELIBS_AUDIO_FMT_H_
#define _ELIBS_AUDIO_FMT_H_
#include <libc/mediainfo/mediafmt.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum __ID3_IMG_FORMAT
{
    ID3_IMG_FORMAT_BMP = 0,
    ID3_IMG_FORMAT_JPG,
    ID3_IMG_FORMAT_GIF,
    ID3_IMG_FORMAT_PNG,
    ID3_IMG_FORMAT_UNSUPPORT = -1
}
__id3_img_format_t;

typedef enum __A_AUDIO_FONTTYPE
{
    A_AUDIO_FONTTYPE_ISOIEC8859_1 = 0,  //ISO/IEC 8859-1
    A_AUDIO_FONTTYPE_UTF_16LE,
    A_AUDIO_FONTTYPE_UTF_16BE,
    A_AUDIO_FONTTYPE_UTF_8,
    A_AUDIO_FONTTYPE_ = -1
} __a_audio_fonttype_e;

typedef struct __ID3_IMAGE_INFO
{
    __s32   length;         //数据长度
    __s32   FileLocation;   //文件偏移位置
    __id3_img_format_t  img_format; //图片格式
    __s32   pic_type;       //picture type;
    __s32   img_high;       //预留，图片高度
    __s32   img_width;      //预留，图片宽度
    __s32   otherdata;      //预留

} __id3_image_info_t;

typedef struct __AUDIO_FILE_INFO
{
    __u32   ulSampleRate;   // 采样率 sample rate
    __u32   ulBitRate;      // 比特率 bit rate单位：BPS
    __u32   ulChannels;     // 声道数 channel
    __u32   ulDuration;     // 播放时间 duration 单位：ms
    __u32   ulBitsSample;   // 采样点位数 sample 8/16/24/32
    __u32   ulCharEncode;   // 0:GB2312.1:UNICODE

    __s32   ulAudio_name_sz;// 音频格式说明
    char    *ulAudio_name;  // mp3 /RealAudio Cook.sipo. / aac-lc....
    __a_audio_fonttype_e    ulAudio_nameCharEncode;

    __s32   ulGenre_sz;     // 流派
    char    *ulGenre;       // pop soft...
    __a_audio_fonttype_e    ulGenreCharEncode;

    __s32   ultitle_sz;     // 歌曲名
    char    *ultitle;
    __a_audio_fonttype_e     ultitleCharEncode;

    __s32   ulauthor_sz;    // 演唱者
    char    *ulauthor;
    __a_audio_fonttype_e    ulauthorCharEncode;

    __sw_audio_format_e        ulFormat;
    __s32   ulAlbum_sz;     // 专集
    char    *ulAlbum;
    __a_audio_fonttype_e    ulAlbumCharEncode;

    __s32   ulYear_sz;      // 出品年代
    char    *ulYear;
    __a_audio_fonttype_e    ulYearCharEncode;

    __s32    ulAPic_sz;     // attached picture
    __id3_image_info_t      *ulAPic;
    __a_audio_fonttype_e    ulAPicCharEncode;

    __s32    ulUslt_sz;     // 非同步的歌词/文本 抄本
    char    *ulUslt;        // __s32 ulFileLocation；
    __a_audio_fonttype_e    ulUsltCharEncode;

    __s32    ulSylt_sz;     // 同步的歌词/文本
    char    *ulSylt;        // __s32 ulFileLocation；
    __a_audio_fonttype_e    ulSyltCharEncode;

    __s32                   ulbufflag;              //0:对文件进行分析，否则对buf里面的数据进行分析
    __s32                   ulbuf_length;           //buf length;
    char                   *ulbuf;                  //分析buf里面的数据.
    __s32                   data[9];

    char    *ulBSINFO;      // temporary buffer of read data
    __s32   InforBufLeftLength;
    char    *InforBuf;      // 用于存储 ulAudio_name  ultitle ulauthor等信息的buffer

} __audio_file_info_t;


/*
************************************************************************************************************************
*                       GET AUDIO FORMAT
*
*Description: get audio format.
*
*Arguments  : file      full path of audio file.
*             format    file format information.
*
*Return     : result.
*               1:get audio format succssed.
*               0:get audio format failed.
************************************************************************************************************************
*/
extern __bool eLIBs_GetAudioFormat(const char *file, int *format);


/*
************************************************************************************************************************
*                       GET AUDIO FILE INFORMATION
*
*Description: get audio file information.
*
*Arguments  : file      full path of audio file.
*             inf       file information.
*
*Return     : result.
*               1:get audio information succssed.
*               0:get audio information failed.
************************************************************************************************************************
*/
extern __bool eLIBs_GetAudioInfo(const char *file, __audio_file_info_t *inf);


/*
************************************************************************************************************************
*                       GET AUDIO DATA INFORMATION
*
*Description: get audio data information.
*
*Arguments  : file      full path of audio file.
*             inf       file information.
*
*Return     : result.
*               1:get audio information succssed.
*               0:get audio information failed.
************************************************************************************************************************
*/
extern __bool eLIBs_GetAudioDataInfo(const char *pFormat, __audio_file_info_t *AIF, __s8 *buf, __s32 datalen);

#ifdef __cplusplus
}
#endif


#endif //_ELIBS_AUDIO_FMT_H_

