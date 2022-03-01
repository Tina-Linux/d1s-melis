/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : dtv_setting_i.h
* By        : hot.lee
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  hot.lee  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __APP_dtv_SETTINGS_I_H_
#define  __APP_dtv_SETTINGS_I_H_

#include "beetles_app.h"
#include "..//dtv_i.h"

#define dtv_SETTING_FOCUS_COLOR   GUI_BLUE
#define dtv_SETTING_UNFOCUS_COLOR  GUI_BLACK

//#define DTV_AUTO_SEARCH_ENABLE

enum
{
    //1、设置
    DTV_SETTING_ALL_FREQ_SEARCH_ID = 0,     //自动搜索,全频点
#ifdef DTV_AUTO_SEARCH_ENABLE
    DTV_SETTING_AUTO_SEARCH_ID,
#endif
    DTV_SETTING_MANU_SEARCH_ID,     //手动搜索,指定频搜台
    DTV_SETTING_DISPLAY_ID,    //显示设置
    DTV_SETTING_CA_ID,       //CA序列号
    DTV_SETTING_EPG,
    DTV_SETTING_WIRELESS_ID,    //天线选择
    DTV_SETTING_MAX,
    DTV_SETTING_TIME_ZONE_ID,    //时区设置
    DTV_SETTING_BACK2LIST,//从子项返回list
    DTV_SETTING_REPLAY,
    DTV_SETTING_2_MANU_SEARCH,   //手动搜台
    DTV_SETTING_SWITCH_PLAY
};
#endif



