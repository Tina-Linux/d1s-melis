/*
*******************************************************************************************************************
*                                                           ProgSheet  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    ProgSheet.h
*
*Author：       Terry Zeng
*
*Version :      1.1.0
*
*Date:          2009-09-02
*
*Description :  ProgSheet control implement
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>         <version>     <description>
*
* Terry.Zeng     2009-09-02         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __GUI_PROGSHEET_H__
#define __GUI_PROGSHEET_H__
#include <char_enc.h>

#define CTRL_PROGSHEET      ("propsheet")


#define PROGSHEET_ADD_PAGE              0x100
#define PROGSHEET_SET_ACTIVEINDEX       0x101
#define PROGSHEET_PAINT_PAGE            0x102

typedef struct
{
    void                        *focus_bmp;      /*焦点bmp位图*/
    void                        *unfocus_bmp;    /*非焦点bmp位图*/
    __pos_t                     bmp_pos;         /*bmp位图在控件中的位置*/
    char                        *text;           /*page的标题*/
    __pos_t                     text_pos;        /*page标题在控件中的位置*/
    __u32                       ftxt_color;      /*焦点时的字体颜色*/
    __u32                       uftxt_color;     /*非焦点时的字体颜色*/
    GUI_FONT                   *draw_font;       /*标题对应的字体*/
    __epdk_charset_enm_e        draw_code;       /*标题字符编码*/
    RECT                        head;
} page_para_t;

typedef struct page_s
{

    __u32 data;                                  /*page对应的（page_para_t *）数据*/
    RECT  rect;                                 /* page对应的标题区域*/
    H_WIN hwnd;                                 /* page对应的静态窗口的句柄*/
    __pGUI_WIN_CB proc;                         /* page 对应的静态窗口的回调*/
    struct page_s *next;                         /*下一page的指针*/
} page_t;


typedef struct
{
    RECT                        head_rect;        /*progsheet对应的头部区域，也即page标题栏所在的区域*/
    void                        *bmp;             /*bmp位图*/
    __pos_t                     bmp_pos;          /*bmp位图在控件中的位置*/
    char                        *text;        /*page的标题*/
    __pos_t                     text_pos;        /*page标题在控件中的位置*/
    GUI_FONT                   *draw_font;       /*标题对应的字体*/
    __epdk_char_encoding_e      draw_code;       /*标题字符编码*/
} progsheet_para_t;


typedef struct
{
    GUI_MEMDEV_Handle   hmem;                    /*progsheet对应的内存设备句柄*/
    __s32 page_count;                           /*当前有几个page*/
    page_t *active_page;                        /*当前active的page*/
    page_t *head;                               /*page链表的第一个page指针*/
    __s32 data;                                 /*progsheet属性，对应progsheet_para_t **/
} progsheet_data_t;


__bool RegisterProgSheetControl(void);

#endif /*#ifndef __GUI_PROGSHEET_H__*/

