#ifndef __RAT_MINIATURE_H__
#define __RAT_MINIATURE_H__

/*!
* \brief
*     图片文件格式
*/
typedef enum
{
    RAT_PIC_FORMAT_BMP = 0x40,
    RAT_PIC_FORMAT_GIF,
    RAT_PIC_FORMAT_JPG,
    RAT_PIC_FORMAT_PNG,
    RAT_PIC_FORMAT_UNKNOWN
} rat_pic_format_t;

/*!
  * \brief
  *     缩略图输入参数
*/
typedef struct
{
    char file[RAT_MAX_FULL_PATH_LEN];   //文件全路径
    __u8 format;                        //数据格式
    __u32 width;                        //宽度
    __u32 height;                       //高度
    unsigned long mode;                         //缩放模式，0非拉伸，1拉伸
} rat_miniature_para_t;

/*!
* \brief
*     返回的缩略图数据参数
*/
typedef struct
{
    __u32 len;      //数据长度
    char *buf;      //数据
} rat_miniature_t;

/*!
* \brief
*     图片文件格式
*/


typedef struct
{
    rat_pic_format_t    format;
    __u32               width;
    __u32               height;
    rat_miniature_t     miniature;
} rat_pic_info_t;

typedef struct
{
    rat_pic_format_t    format;
    __u32               bitrate;
    __u32               sample_rate;
    __u32               channel;
    __u32               duration;
    __u32               bits;
    rat_miniature_t     album;
} rat_audio_info_t;


/*
**********************************************************************************************************************
*
*
* author:
*
* date:
*
* Description:       等待willow线程结束
*
* parameters:
*
* return:           success 0;  failed:-1
* modify history:
**********************************************************************************************************************
*/
__s32 rat_wait_willow_finish(void);

/*
**********************************************************************************************************************
*
*
* author:
*
* date:
*
* Description:       启动缩略图解码（安装willow.mod，不用每次都安装，不用常驻内存）
*
* parameters:
*
* return:           success 0;  failed:-1
* modify history:
**********************************************************************************************************************
*/
__s32 rat_start_miniature_decode(void);

/*
**********************************************************************************************************************
*
*
* author:
*
* date:
*
* Description:       停止缩略图解码（卸载willow.mod）
*
* parameters:
*
* return:           success 0;  failed:-1
* modify history:
**********************************************************************************************************************
*/
__s32 rat_stop_miniature_decode(void);

/*
**********************************************************************************************************************
*
*
* author:
*
* date:
*
* Description:       获取图片缩略图等信息
*
* parameters:
*
* return:           success 0;  failed:-1
* modify history:
**********************************************************************************************************************
*/
__s32 rat_get_pic_info(rat_miniature_para_t *para, rat_pic_info_t *pic_info);

/*
**********************************************************************************************************************
*
*
* author:
*
* date:
*
* Description:       获取音乐相册等信息
*
* parameters:
*
* return:           success 0;  failed:-1
* modify history:
**********************************************************************************************************************
*/
__s32 rat_get_album_info(rat_miniature_para_t *para, rat_audio_info_t *audio_info);

/*
**********************************************************************************************************************
*
*
* author:
*
* date:
*
* Description:       设置缩略图背景色
*
* parameters:
*
* return:           success 0;  failed:-1
* modify history:
**********************************************************************************************************************
*/
__s32 rat_set_pic_bgcolor(__u32 color);

#endif//__RAT_MINIATURE_H__
