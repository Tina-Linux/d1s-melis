/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
    __u32 input_type;                   // 0 file 1 buffer
    __u32 input_size;                   //buffer大小
    char *input_buf;                    //buffer地址
    char file[RAT_MAX_FULL_PATH_LEN];   //文件全路径
    __u8 format;                        //数据格式
    __u32 width;                        //宽度
    __u32 height;                       //高度
    unsigned long mode;                 //缩放模式，0非拉伸，1拉伸
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
