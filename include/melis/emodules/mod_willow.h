/*
**********************************************************************************************************************
*                                                   eMOD
*                                  the Easy Portable/Player Operation System
*                                              mod_mmp sub-system
*
*                               (c) Copyright 2006-2007, RICHARD,  China
*                                           All Rights Reserved
*
* File   : mod_willow.h
* Version: V1.0
* By     : Richard.x
**********************************************************************************************************************
*/

#ifndef _MOD_WILLOW_H_
#define _MOD_WILLOW_H_

#include <typedef.h>
#include <kconfig.h>
#include <ktype.h>
#include <libc.h>
#include <fb.h>
#include "mod_defs.h"
/*以下为获取缩略图时的填充模式*/
#define WILLOW_SCALE_RATIO 0         /* 按比例缩小至指定尺寸，不能溢出
                                      * 图片尺寸小于指定尺寸时，保持图片尺寸不变 */
#define WILLOW_SCALE_STRETCH 1       /* 以窗口的比例缩放图片至满窗口显示，可能会变形 */
#define WILLOW_SCALE_RATIO_SCALE 2   /* 按比例缩小至指定尺寸，不能溢出
                                      * 图片尺寸小于指定尺寸时，放大图片以适应尺寸 */
#define WILLOW_SCALE_RATIO_CUTOFF 3  /* 以图片的比例缩放图片至满窗口显示，
                                      * 可能会有部分图片溢出窗口(不可见)*/
#define WILLOW_SCALE_CLEAR_BK 0x100  /*缩略图未填充满时，填充剩余部分为黑色*/
#define WILLOW_SCALE_CLEAR_NONE 0x0  /*缩略图未填充满时，不填充背景色，由应用自己填充*/

#define MAX_TRANSITION_FB_NUM 16

/*图片浏览，放大图片时的相关参数*/
#define WILLOW_SCALER_BASE_RATIO 1000 /*放大倍数基数*/
#define WILLOW_SCALER_MAX_RATIO 1000000 /*放大倍数基数*/


/*图片格式*/
typedef enum
{
    IMG_FORMAT_BMP,                    /*bmp图片*/
    IMG_FORMAT_JPG,                    /*JPEG图片*/
    IMG_FORMAT_GIF,                    /*GIF格式图片*/
    IMG_FORMAT_PNG,                    /*PNG格式图片*/
    IMG_FORMAT_3D,                     /*JPEG图片*/
    IMG_FORMAT_UNSUPPORT               /*图片格式不支持*/
} __willow_img_format_t;

/*willow status*/
typedef enum
{
    WILLOW_STATUS_INIT,
    WILLOW_STATUS_BUSY,
    WILLOW_STATUS_READY,
    WILLOW_STATUS_FINISH
} __willow_img_status_t;


typedef enum
{
    WILLOW_IMAGE_SHOW_ORISIZE,         /* 以图片原始大小在窗口内显示，不能溢出窗口
                                        * 图片尺寸小于窗口时，保持图片尺寸不变 */
    WILLOW_IMAGE_SHOW_STRETCH,         /* 以窗口的比例缩放图片至满窗口显示，可能会变形 */
    WILLOW_IMAGE_SHOW_CUTOFF,          /* 以图片的比例缩放图片至满窗口显示，
                                        * 可能会有部分图片溢出窗口(不可见)*/
    WILLOW_IMAGE_SHOW_AUTO,
    WILLOW_IMAGE_SHOW_RATIO,           /* 以图片的比例缩放图片至满窗口显示，不能溢出窗口
                                        * 图片尺寸小于窗口时，会放大至合适尺寸 */
    WILLOW_IMAGE_SHOW_USRDEF,          /* 用户自定义，图片显示的尺寸由callback 设定*/

} __willow_image_show_mode_t;


/*pshow support switch method*/
typedef enum
{
    DEFAULT_SWITCH  = 0x0,    /*默认无切换效果*/
    FADE_IN_OUT     = 0x1,    /*淡入淡出*/
    PERSIAN_BLIND_H = 0x2,    /*水平百叶窗*/
    PERSIAN_BLIND_V = 0x3,    /*垂直百叶窗*/
    SLID_UP         = 0x4,    /*向上滑动*/
    SLID_DOWN       = 0x5,    /*向下滑动*/
    SLID_LEFT       = 0x6,    /*向左滑动*/
    SLID_RIGHT      = 0x7,    /*向右滑动*/
    STRETCH_UP      = 0x8,    /*向上展开*/
    STRETCH_DOWN    = 0x9,    /*向下展开*/
    STRETCH_LEFT    = 0xA,    /*向左展开*/
    STRETCH_RIGHT   = 0xB,    /*向右展开*/
    MOSAIC          = 0xC,    /*马赛克*/
    ROOM_IN         = 0xD,    /*缩小*/
    ROOM_OUT        = 0xE,    /*放大*/
    ROOM_RANDOM     = 0xF,  /*随机*/
    ROOM_EXIT       = 0x10, /*退出*/
    TOUCH_SLID_H    = 0x50,    /*touch水平方向滑动*/
    TOUCH_SLID_V    = 0x51,   /*touch垂直方向滑动*/
    EXTENT_SWITCH1  = 0x100,
    EXTENT_SWITCH2,
    EXTENT_SWITCH3,
    EXTENT_SWITCH4,
    EXTENT_SWITCH5
} __willow_switch_mode_t;

/*pshow support splended effect*/
enum
{
    EXTENT_EFFECT1,
    EXTENT_EFFECT2,
    EXTENT_EFFECT3,
    EXTENT_EFFECT4,
    EXTENT_EFFECT5
};

typedef enum
{
    WILLOW_LYR_TOP,               /*将willow中的图层设置为top*/
    WILLOW_LYR_BOTTOM,            /*将willow中的图层设置为bottom*/
    WILLOW_LYR_PIPE_0,            /*将willow中的图层设置为pipe0*/
    WILLOW_LYR_PIPE_1,            /*将willow中的图层设置为pipe1*/
    WILLOW_LYR_HIDE,              /*将willow中的图层设置为不可见*/
    WILLOW_LYR_SHOW,              /*将willow中的图层设置为可见*/
} __willow_lyr_cfg_t;


typedef enum
{
    PSHOW_ROTATE_NONE = 0,          /*不旋转*/
    PSHOW_ROTATE_RIGHT = 1,         /*向右旋转90度*/
    PSHOW_ROTATE_180 = 2,           /*旋转180度*/
    PSHOW_ROTATE_LEFT = 3,          /*向左旋转90度*/
    PSHOW_FLIP_HORIZONTAL = 4,      /*水平翻转*/
    PSHOW_FLIP_VERTICAL = 5,        /*垂直翻转*/
    PSHOW_ROTATE_UNSUPPORTED = 6,   /*======以下暂时不支持======*/
} __willow_rotate_mode_t;


/*willow support command*/
typedef enum
{
    WILLOW_CMD_QUERY_STATUS          = 0x0,  /* 查询状态，
                                              * 返回值为__willow_img_status_t
                                              */

    WILLOW_CMD_GET_THUMBS            = 0x1,  /* 设置获取缩略图所需要的信息
                                              * pbuffer = (__willow_get_thumbs_param_t *)
                                              */

    WILLOW_CMD_SHOW_IMG_FROM_FILE    = 0x2,  /* 设置显示图片的文件信息
                                              * pbuffer = (__willow_show_file_param_t *)
                                              */

    WILLOW_CMD_SHOW_IMG_FROM_BUFFER  = 0x3,  /* 设置显示的argb数据
                                              * pbuffer = (__willow_show_buffer_param_t *)
                                              * 注意，此命令要求buffer中的数据为argb8888,
                                              * 并不是解码buffer中的图片，
                                              * 解码buffer中的图片使用命令WILLOW_CMD_CFG_INPUT
                                              */

    WILLOW_CMD_SET_SWITCH_MODE       = 0x4,  /* 设置图片浏览时候的切换模式
                                              * aux = (__willow_switch_mode_t)
                                              */


    WILLOW_CMD_GET_SHOW_PARAM        = 0x5,  /* 获取当前显示参数
                                              * pbuffer = (__willow_showing_img_info_t *)
                                              */

    WILLOW_CMD_GET_IMG_INFO          = 0x6,  /* 获取图片信息
                                              * pbuffer = (__willow_img_info_t *)
                                              */
    /*change current img show*/
    WILLOW_CMD_SCALE                 = 0x7,  /* 缩小/放大图片
                                              * aux = -3，-2，-1，0，1，2，3
                                              * 分别代表缩小8/4/2倍，原始大小，放大2/4/8倍
                                              */

    WILLOW_CMD_ROTATE                = 0x8,  /* 旋转图片
                                              * aux = -270, -180, -90, 0, 90, 180, 270
                                              * 负数表示逆时针旋转,正数表示顺时针旋转
                                              */

    WILLOW_CMD_MOVE                  = 0x9,  /* 移动图片，在图片放大后有效
                                              * pbuffer = (__pos_t *) 为移动的offset
                                              */

    WILLOW_CMD_COME_BACK             = 0xA,  /* 用于缩小/放大图片后返回原始大小*/

    WILLOW_CMD_OPEN_SHOW             = 0xB,  /* 打开图片浏览*/
    WILLOW_CMD_SHUT_SHOW             = 0xC,  /* 关闭图片浏览*/

    WILLOW_CMD_CHECK_IMG             = 0xD,  /* 检查图片是否支持*/
    WILLOW_CMD_START_DEC             = 0xE,  /* 开始解码*/
    WILLOW_CMD_START_SHOW            = 0xF,  /* 显示图片，获取缩略图*/
    WILLOW_CMD_STOP                  = 0x10, //

    WILLOW_CMD_SET_ALBUM_ART         = 0x11, /* 设置专辑图片信息,
                                              * pbuffer = (__willow_get_albumart_param_t *)
                                              */
    WILLOW_CMD_SET_SCN               = 0x12, /* 设置slider show的显示区域。
                                              * pbuffer = __rect_t
                                              */
    WILLOW_CMD_GET_VER               = 0x13, /* 获取willow版本号
                                              * (__u32 *)pbuffer = WILLOW_VERSION
                                              */
    WILLOW_CMD_START_SHOW_EXT        = 0x14, /* 获取缩略图,预先用户指定填充方式,
                                              * pbuffer = (__u32 *)(fill_mode | clear_flag)
                                              * fill_mode可以为: WILLOW_SCALE_RATIO,
                                              *                  WILLOW_SCALE_STRETCH
                                              *                  WILLOW_SCALE_RATIO_SCALE
                                              *                  WILLOW_SCALE_RATIO_CUTOFF
                                              * clear_flag可以为 WILLOW_SCALE_CLEAR_BK
                                              *                  WILLOW_SCALE_CLEAR_NONE
                                              * 具体含义请参考宏定义中的注释。
                                              */

    WILLOW_CMD_CATCH_SCREEN          = 0x15, /* 获取当前显示的fb
                                              * pbuffer = (FB *)
                                              */

    WILLOW_CMD_AUTO_ROTATE           = 0x16, /* 当高度 > 宽度时，是否需要自动旋转，
                                              * aux = __willow_rotate_mode_t
                                              */

    WILLOW_CMD_GET_ERRNO             = 0x17, /* 获取错误号，目前未提供*/

    WILLOW_CMD_SET_SHOW_MODE         = 0x18, /* 配置显示模式
                                              * aux = __willow_image_show_mode_t，
                                              * 当aux = WILLOW_IMAGE_SHOW_USRDEF是，pbuffer = __pCBK_t，
                                              * 参数为 __willow_showmod_usr_para_t
                                              */

    WILLOW_CMD_CFG_LYR               = 0x19, /* 配置willow图层属性,
                                              * aux = __willow_lyr_cfg_t
                                              */

    WILLOW_CMD_ROT_LYR               = 0x20, /* 将图片旋转一个角度后显示, 注意图片最终显示的旋转
                                              * 方向为WILLOW_CMD_ROT_LYR和 WILLOW_CMD_AUTO_ROTATE两
                                              * 者共同作用的结果。
                                              * aux = __willow_rotate_mode_t
                                              */

    WILLOW_CMD_CFG_OUTPUT            = 0x21, /* 设置输出到指定的fb中，
                                              * pbuffer = (__willow_output_t *)
                                              */

    WILLOW_CMD_CFG_INPUT             = 0x22, /* 设置指定的输入方式，
                                              * aux = __willow_input_src_e
                                              * 当aux = WILLOW_INPUT_BUFFER时,pbuffer = (__willow_buf_info_t *)
                                              */

    WILLOW_CMD_SET_PLAYLIST          = 0x23, /* 设置图片预解码信息
                                              * pbuffer = (__willow_img_plst_info_t *)
                                              */

    WILLOW_CMD_MOV_TRACK             = 0x24, /* 设置拖拽运动轨迹
                                              * pbuffer = (slider_step_para_t *)
                                              */

    WILLOW_CMD_GET_CUR_INDEX         = 0x25, /* 获取拖拽结束后图片在播放列表中的index
                                              * 返回index
                                              */

    WILLOW_CMD_SCALE_EXT             = 0x26, /* 无级缩放图片,可以任意实现小于WILLOW_SCALER_MAX_RATIO倍数缩放图片
                                              * aux = 放大倍数 * WILLOW_SCALER_BASE_RATIO
                                              * aux = 2300表示放大 2.3倍
                                              */
    WILLOW_CMD_CHECK_CUR             = 0x27,     /* 获取拖拽结束后获取当前图片是否支持 */

    WILLOW_CMD_SET_3D_MOD            = 0x100, /* 设置3D 显示模式
                                               * aux = __willow_show_3d_mode_e
                                               */
    WILLOW_CMD_SET_COLOR_MOD         = 0x101, //设置色彩模式,黑白or彩色
    WILLOW_CMD_SET_SW_SCALE          = 0x201, //客户选择是硬件scale还是软件scale.同显的时候de一直被占用了，所以willow不能使用硬件scale.如果调用了那只能使用软件scale
    WILLOW_CMD_SET_USR_LAYER_CNT,    /*应用设置多窗口显示图片，1,2,3,4默认是1个*/
    WILLOW_CMD_SET_BACKCOLOR,        /*设置背景色*/
} __willow_main_cmd_t;

typedef enum
{
    WILLOW_INPUT_FILENAME = 0x0,            /*输入为本地文件的文件名*/
    WILLOW_INPUT_BUFFER   = 0x1,            /*输入为buffer，所有文件预先读入到内存*/
    WILLOW_INPUT_RAWDATA  = 0x2,            /*输入为解码后的数据*/
    WILLOW_INPUT_ERR      = 0x3,
} __willow_input_src_e;


typedef struct __WILLOW_BUF_INFO
{
    void *buffer;
    __u32 buffersize;
} __willow_buf_info_t;

typedef struct
{
    __u8    format;
    __u8    seq;

    __u32    nimg;
    __u32     height;
    __u32     width;
    void     *data;
} __willow_thumbs_t;

typedef struct
{
    __s32    is_supported;          /*是否支持该图片(图片尺寸太大时不支持，flag = -1)*/
    __u32    reserve[10];           /*保留，用于以后扩展使用*/
    FB       img_fb;                /*图像解码后数据*/
} __willow_output_t;


/*音乐文件专辑图片信息，用于解码图片数据在本地文件中有偏移的情况*/
typedef struct
{
    __u8 enable;                          /*数据结构是否有效*/
    __willow_img_format_t img_type;       /*图片类型*/
    __u32 offset;                         /*图片数据在文件中的偏移*/
    __u32 length;                         /*图片数据的长度*/
} __willow_AlbumArt_ctrl_t;

/*willow thumbs command param*/
typedef struct
{
    __u8            inputType;                    //数据输入类型， 0文件输入， 1 buf输入
    __u8            *filename;                  //img file name
    __u8            *inputBuf;
    __u32           inputSize;
    __u8            *buf;                        //thumbs data dest addr
    __u8            format;                      //thumbs format
    __rectsz_t      size;
    __s8            filethumbs;
} __willow_get_thumbs_param_t;


/*willow album art command param*/
typedef struct
{
    __willow_get_thumbs_param_t thumbs;
    __willow_AlbumArt_ctrl_t album_art_info;

} __willow_get_albumart_param_t;

/*willow show buffer comamand param*/
typedef struct
{
    __rect_t    img_size;
    __u8        *img_buf;                          //img addr
    __u32         buf_size;                        //img size
    __u8        img_format;                        //img format

} __willow_show_buffer_param_t;



/*willow show img file command param*/
typedef struct
{
    char        *filename;                                    //file name
    __u32        img_no;
} __willow_show_file_param_t;

typedef struct
{
    RECT        show_region;
    __hdle        layer_hdle;
    __mp        *de;
} __willow_showing_img_info_t;

typedef struct
{
    char    *name;
    __u32    file_len;

    SIZE    size;
    __u8    format;
    __u8    color_depth;
    __u32    aperture;
    __u32    shutter;
} __willow_img_info_t;

typedef struct
{
    __pCBK_t  get_imagename_by_index;
    __s32     total;
    FB        *decoding_fb[MAX_TRANSITION_FB_NUM];
    FB        *unsupported_fb;
    __s32     cycEnabled;
} __willow_img_plst_info_t;


typedef enum
{
    SLIDER_STEP_START,            /*开始拖拽*/
    SLIDER_STEP_MOVE,             /*移动到目标位置*/
    SLIDER_STEP_FINISH,           /*结束拖拽*/
} slider_step_cmd_e;


typedef struct
{
    slider_step_cmd_e    cmd;
    OFFSET               offset;
} slider_step_para_t;

typedef enum
{
    WILLOW_SHOW_2D     = 0x0,              /*2D模式显示3d图片*/
    WILLOW_SHOW_3D     = 0x1,              /*3D模式显示3d图片*/
} __willow_show_3d_mode_e;

typedef enum
{
    WILLOW_SHOW_MONO  = 0x0,            //显示为黑白图片
    WILLOW_SHOW_COLOR = 0x1,            //显示为彩色图片
} __willow_show_color_mod_e;

typedef struct
{
    __rectsz_t  imgSize;        /* 原始图像的大小，callback的输入 */
    __rectsz_t  dstSize;        /* 缩放处理后全图的大小 */
    __rect_t    srcRect;        /* 要显示的部分缩放前处于原始图像中的区域/位置 */
    __rect_t    dstRect;        /* 要显示的部分缩放后处于dstSize图像中的区域/位置 */
} __willow_showmod_usr_para_t;
#endif  /* _MOD_WILLOW_H_ */

