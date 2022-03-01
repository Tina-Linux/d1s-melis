/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : schedules_uipara.h
* By        : lantian.lin
* Func  : ui parameter
* Version   : v1.0
* ============================================================================================================
**************************************************************************************************************
*/

#ifndef __SCHEDULE_UIPARA_H_
#define  __SCHEDULE_UIPARA_H_
#include "schedule_i.h"

#if 0
typedef struct schedule_uipara_s
{
    res_pos_t   lyr_win;
    res_pos_t   lyr_scn;
    res_pos_t   item;       //每个item的位置坐标和大小
    res_pos_t   left_button;    //向左方向的三角形的按钮
    res_pos_t   right_button;   //向右方向的三角形按钮
    OFFSET txt_pos;     //通用设置    ，电源管理字符坐标
    res_pos_t   string_detail_pos_de;   //每个选项对应的详细信息说明文字所在坐标
    res_pos_t   string_detail_pos_ex;   //每个选项对应的详细信息说明文字所在坐标
    res_pos_t   item_title_pos;
    res_pos_t   item_content_pos;
    res_pos_t   return_bmp;         //返回图片坐标位置
    res_pos_t   long_string_area ;  // long string scroll area
    tips_uipara_t  schedule_tips_uipara;    //提示框坐标位置
} schedule_uipara_t;

/*
typedef struct adjust_uipara
{
    RECT    adjust_finish;//校准完成提示语
    OFFSET  cross_pos_1;
    OFFSET  cross_pos_2;
    OFFSET  cross_pos_3;
    OFFSET  cross_pos_4;
}adjust_uipara_t;
*/
schedule_uipara_t *schedule_get_uipara(void);
//tips_uipara_t* schedule_get_tips_uipara(void);

#endif
#endif      /*__SCHEDULE_UIPARA_H_*/

