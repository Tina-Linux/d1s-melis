#ifndef __RAT_COMMON_H__
#define __RAT_COMMON_H__
#include <kconfig.h>

#define RAT_MAX_FULL_PATH_LEN (256+1)

typedef __hdle  HRAT;
typedef HRAT HRATNPL;

/*!
* \brief
*     浏览的媒体类型
*/
typedef enum
{
    RAT_MEDIA_TYPE_ALL = 0,     //所有媒体类型
    RAT_MEDIA_TYPE_PIC,         //所有图片文件
    RAT_MEDIA_TYPE_AUDIO,       //所有音频文件
    RAT_MEDIA_TYPE_VIDEO,       //所有视频文件
    RAT_MEDIA_TYPE_EBOOK,       //所有电子书文件
    RAT_MEDIA_TYPE_GAME,        //所有游戏文件
    RAT_MEDIA_TYPE_FIRMWARE,    //所有固件文件
    RAT_MEDIA_TYPE_UNKNOWN,     //所有未知文件类型
    RAT_MEDIA_TYPE_FOLDER,      //文件夹
    RAT_MEDIA_TYPE_USB_DEVICE,  //USB设备
    RAT_MEDIA_TYPE_SD_DEVICE,   //SD设备
    RAT_MEDIA_TYPE_MAX,         //
} rat_media_type_t;

/* play mode */
typedef enum __rat_play_mode_e
{
    RAT_PLAY_MODE_ONLY_ONCE = 0,
    RAT_PLAY_MODE_ROTATE_ONE,
    RAT_PLAY_MODE_ROTATE_ALL,
    RAT_PLAY_MODE_SEQUENCE,
    RAT_PLAY_MODE_RANDOM,
    RAT_PLAY_MODE_EXIT,
    RAT_PLAY_MODE_MAX
} rat_play_mode_e;

/*!
* \brief
*     分区对应的设备
*/
typedef enum
{
    RAT_UNKNOWN,        //未知
    RAT_LOCAL,          //本地磁盘
    RAT_TF,             //TF卡
    RAT_USB,            //USB设备
    RAT_CD,             //CD-ROM
    RAT_MEDIA           //媒体库
} rat_type_t;

enum
{
    RAT_TF_SWITCH_TO_USB = 0,
    RAT_USB_SWITCH_TO_TF,
    RAT_NOT_FOUND_TF,
    RAT_NOT_FOUND_USB,
};
#endif//__RAT_COMMON_H__
