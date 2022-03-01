/*
**************************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              FM app sample
*
*                                   (c) Copyright 2006-2007, ANDY, China
*                                            All Rights Reserved
*
* File    : mbook_show.h
* By      : lyn
* Version : V1.00
* time    : 2009-8-14
**************************************************************************************************************
*/
#ifndef __MBOOK_SHOW_H__
#define __MBOOK_SHOW_H__

typedef     void   *H_SHOW;

typedef struct tag_SHOW_CONFIG
{
    __u32       show_start;             // 显示的起始位置
    __u32       show_width;             // 可显示的宽度
    __u32         scroll_width;
    __u32       page_row;               // 一页显示的行数
    __u32       row_height;             // 行与行间高度
    __u32       col_width;              // 列与列的宽度
    __u32       font_color;             // 字体的颜色
    __u32       back_color;             // 背景的颜色
    GUI_FONT   *char_font;              // 字库句柄
    __s32      *char_table;             // 字符字宽表

    __epdk_charset_enm_e    charset;    // 字符编码
} __show_config_t;


H_SHOW  MBOOK_Show_Init(void);

__s32   MBOOK_Show_Config(H_SHOW hdle, __show_config_t *config);

__s32   MBOOK_Show_Page(H_SHOW hdle, __hdle lyr, void *data);

__s32   MBOOK_Show_Uninit(H_SHOW hdle);

#endif /* __MBOOK_SHOW_H__ */


