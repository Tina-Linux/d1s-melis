/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                 WEBCAM Driver
*
*                                    (c) Copyright 2012-2016, Kingvan.Tong China
*                                             All Rights Reserved
*
* File    : drv_webcam.h
* By      : Kingvan
* Version : v1.0
* Date    : 2012-9-27
* Descript:
* Update  : date                auther         ver     notes
*           2012-9-27 11:09:13  Kingvan.Tong   2.0     build the file.
*********************************************************************************************************
*/
#include  "webcam_dev_i.h"



__webcam_mode_all_t webcam_mode_all;//����ʹ��ģʽ
__webcam_mode_t webcam_mode;//��ǰʹ��ģʽ

__s32  webcam_dev_init(void)
{
	webcam_mode.color_format	= PIXEL_OTHERFMT;			//PIXEL_OTHERFMT; PIXEL_YUV422;
	webcam_mode.component_seq	= YUV_SEQ_UVUV;				//YUV_SEQ_UVUV; YUV_SEQ_YUYV;
	webcam_mode.store_mode		= YUV_MOD_UV_MB_COMBINED;	//YUV_MOD_UV_MB_COMBINED; YUV_MOD_INTERLEAVED;
	webcam_mode.size.width		= 1280;						//1280; 640;
	webcam_mode.size.height		= 720;						//720; 480;
	webcam_mode.frame_rate 		= 25000;					//֡�ʣ���λHz
	webcam_mode.frame_period 	= 40000;					//֡����=1/֡�ʣ���λus��
	webcam_mode.color_space 		= BT601;					//ɫ�ʿռ�
	//webcam_mode.csi_buf_scrab_mode = CSI0_FULL;

	webcam_mode_all.number = 2;
	webcam_mode_all.webcam_mode[0] = webcam_mode;
	webcam_mode_all.webcam_mode[1] = webcam_mode;

	
	return EPDK_OK;
}

__s32  webcam_dev_exit(void)
{
	return EPDK_OK;
}

__s32 webcam_dev_set_mode(__u32 mode)
{
	return EPDK_OK;
}

