/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                               (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : app_ebook.h
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __APP_EBOOK_H__
#define  __APP_EBOOK_H__

#include "app_ebook_i.h"
#define BMK_MAX_NUM 6   //..子书书签的个数
#define BMK_PAGE_NO_LENGTH 32 //..电子书书签页码长度

typedef enum tag_AUTO_TIME
{
    AUTO_TIME_FAST = 0,
    AUTO_TIME_MIDDLE,
    AUTO_TIME_SLOW,
    AUTO_TIME_OFF,
} __auto_time_e;

typedef enum tag_BG_MUSIC
{
    BG_MUSIC_OFF = 0,
    BG_MUSIC_ON,
} __bg_music_e;

typedef enum tag_BOOK_COLOR
{
    BOOK_COLOR_BLACK = 0,
    BOOK_COLOR_WHITE,
    BOOK_COLOR_YELLOW,
    BOOK_COLOR_
} __book_color_e;
typedef struct tag_ebook_sset_para//..电子书签的内容属性
{
    /*GUI_FONT *sset_font;
    H_WIN      layer;
    __s32     main_id;
    __s32     value;
    __s32     total_page;*/

    char bmk_page_no[BMK_MAX_NUM][BMK_PAGE_NO_LENGTH] ; // 电子书书签页码

} ebook_sset_para_t; //..


typedef struct __ebook_bkpoint_inf
{
    __u32   page_no;                /* 上次退出电子书的页码        */
    char    ebookname[RAT_MAX_FULL_PATH_LEN + 1]; /* 上次退出电子书时的电子书的名字  */
    //..char    bmk_page_no[BMK_MAX_NUM][BMK_PAGE_NO_LENGTH];//..电子书签
    __u32   bmk_numth;//..第几个书签

} __ebook_bkpoint_t;

H_WIN app_ebook_create(root_para_t *para);
__s32 app_ebook_notify_delete_sub_scene(H_WIN hmanwin);

#endif

