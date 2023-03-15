/*
********************************************************************************
*                                    eMOD
*                   the Easy Portable/Player Develop Kits
*                               mod_lotus sub-system
*
*          (c) Copyright 2009-2010, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : mod_video_play.h
* Version: V1.0
* By     : Eric_wang
* Date   : 2009-12-21
********************************************************************************
*/
#ifndef _MOD_VIDEO_PLAY_H_
#define _MOD_VIDEO_PLAY_H_

/*******************************************************************************
* type name: __video_play_cmd_set_t
* Description:
*    视频(csi/tvin)显示媒体(IOCtrol), 控制函数原型:
*    __s32 esMODS_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
*    mp: video_play中间件模块句柄
*    cmd: __video_play_cmd_set_t定义的命令字
*    aux:
*    pbuffer: 具体命令的输入/输出参数, 不同的命令输入不同。
*             一般来说，如果是结构体需要传入指针，枚举值直接传入具体值
*    ret = EPDK_OK/EPDK_FAIL
*******************************************************************************/
typedef enum __VIDEO_PLAY_CMD_SET
{
    VIDEO_PLAY_START      ,             //开启cvbs显示
    VIDEO_PLAY_STOP      ,              //停止cvbs显示
 	VIDEO_PLAY_SET_ROTATE_ANGLE      , 	//设置视频旋转角度 aux = dir: __video_play_rotate_t 默认不旋转
 	VIDEO_PLAY_GET_ROTATE_ANGLE,		//获取当前视频旋转角度 return = dir
 	VIDEO_PLAY_SET_CHANNEL,				//设置cvbs输入通道 aux= channel select，默认 channel 0
 	VIDEO_PLAY_GET_CHANNEL,				//获取当前cvbs输入通道
 	VIDEO_PLAY_GET_SYSTEM,				//获取制式 return = 0: NTSC      return = 1 PAL
 	VIDEO_PLAY_GET_SIGNALE_DECT,		//获取当前cvbs有无信号 return = 1有信号 return = 0无信号
	VIDEO_PLAY_SET_SCREEN_RECT, 		//设置显示参数			pbuffer:  __disp_rect_t 默认全屏显示
 	VIDEO_PLAY_GET_SCREEN_RECT,			//获取当前显示参数			return __disp_rect_t*
  	VIDEO_PLAY_SET_SOURCE_RECT,  		//设置输入视频参数（裁剪） pbuffer: __disp_rect_t,默认不裁剪
	VIDEO_PLAY_GET_SOURCE_RECT, 		//获取当前输入视频参数			   return  __disp_rect_t*
  	VIDEO_PLAY_SET_CONTRAST,            //设置对比度 aux = contrast: 0 <= aux <= 255 默认128
	VIDEO_PLAY_GET_CONTRAST,			//获取对比度 return = contrast
  	VIDEO_PLAY_SET_BRIGHTNESS,				//设置亮度 aux =     bright 0 <= aux <= 255 默认32
	VIDEO_PLAY_GET_BRIGHTNESS,				//获取亮度 return =	 bright
  	VIDEO_PLAY_SET_SATURATION,			//设置饱和度 aux = saturation       0 <= aux <= 255 默认128
	VIDEO_PLAY_GET_SATURATION,			//获取饱和度 return = saturation
	VIDEO_PLAY_SET_SHARPNESS,           //设置锐度
	VIDEO_PLAY_GET_SHARPNESS,           //获取锐度
	VIDEO_PLAY_SET_HUE,                 //设置色度  0 <= aux <= 255
	VIDEO_PLAY_GET_HUE,                 //获取色度
   	VIDEO_PLAY_CMD_UNKNOWN                     = 0xFFFF,
} __video_play_cmd_set_t;


typedef enum
{
	VIDEO_PLAY_ROTATE_0         = 0x0,//rotate clockwise 0 ROTgree
	VIDEO_PLAY_ROTATE_90        = 0x1,//rotate clockwise 90 ROTgree
	VIDEO_PLAY_ROTATE_180       = 0x2,//rotate clockwise 180 ROTgree
	VIDEO_PLAY_ROTATE_270       = 0x3,//rotate clockwise 270 ROTgree
	VIDEO_PLAY_ROTATE_HFLIP     = 0x4,//horizontal flip
	VIDEO_PLAY_HFLIP_ROT_90 	= 0x5,//first rotate clockwise 90 ROTgree then horizontal flip
	VIDEO_PLAY_ROTATE_VFLIP     = 0x6,//vertical flip
	VIDEO_PLAY_VFLIP_ROT_90 	= 0x7,//first rotate clockwise 90 ROTgree then vertical flip
} __video_play_rotate_t;

#endif


