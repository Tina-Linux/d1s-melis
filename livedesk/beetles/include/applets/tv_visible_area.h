/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  :
* File    : tv_visible_area.h
*
* By      : Gary.Wang
* Version : v1.0
* Date    : 2009-12-28 11:23:49
**********************************************************************************************************************
*/

#ifndef __TV_VISIBLE_AREA_H__
#define __TV_VISIBLE_AREA_H__

#define SCREEN_SIZE_1280_720_VISIBLE_WIDTH        1208
#define SCREEN_SIZE_1280_720_VISIBLE_HEIGHT       680
#define SCREEN_SIZE_1280_720_VISIBLE_X            ( (1280-SCREEN_SIZE_1280_720_VISIBLE_WIDTH)/2 )
#define SCREEN_SIZE_1280_720_VISIBLE_Y            ( (720-SCREEN_SIZE_1280_720_VISIBLE_HEIGHT)/2 )

#define SCREEN_SIZE_1920_1080_VISIBLE_WIDTH       1812
#define SCREEN_SIZE_1920_1080_VISIBLE_HEIGHT      1020
#define SCREEN_SIZE_1920_1080_VISIBLE_X           ( (1920-SCREEN_SIZE_1920_1080_VISIBLE_WIDTH)/2 )
#define SCREEN_SIZE_1920_1080_VISIBLE_Y           ( (1080-SCREEN_SIZE_1920_1080_VISIBLE_HEIGHT)/2 )

#define SCREEN_SIZE_720_480_VISIBLE_WIDTH         660
#define SCREEN_SIZE_720_480_VISIBLE_HEIGHT        440
#define SCREEN_SIZE_720_480_VISIBLE_X             ( (720-SCREEN_SIZE_720_480_VISIBLE_WIDTH)/2 )
#define SCREEN_SIZE_720_480_VISIBLE_Y             ( (480-SCREEN_SIZE_720_480_VISIBLE_HEIGHT)/2 )

#define SCREEN_SIZE_720_576_VISIBLE_WIDTH         660
#define SCREEN_SIZE_720_576_VISIBLE_HEIGHT        536
#define SCREEN_SIZE_720_576_VISIBLE_X             ( (720-SCREEN_SIZE_720_576_VISIBLE_WIDTH)/2 )
#define SCREEN_SIZE_720_576_VISIBLE_Y             ( (576-SCREEN_SIZE_720_576_VISIBLE_HEIGHT)/2 )

enum
{
    SCREEN_TYPE_400_240,
	SCREEN_TYPE_480_640,
    SCREEN_TYPE_480_272,
    SCREEN_TYPE_720_480,
    SCREEN_TYPE_720_576,
    SCREEN_TYPE_800_480,
    SCREEN_TYPE_1024_768,
    SCREEN_TYPE_1280_720,
    SCREEN_TYPE_1920_1080,
    SCREEN_TYPE_LAST
};



#endif
