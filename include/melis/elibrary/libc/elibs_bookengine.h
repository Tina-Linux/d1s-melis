/*
*********************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Development Kit
*                                               eLibC sub-system
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                              All Rights Reserved
*
* File    : elibs_bookengine.h
* By      : Sunny
* Version : V1.00
* Date    : 2011-5-2 13:24:12
*********************************************************************************************************
*/

#ifndef __ELIBS_BOOKENGINE_H__
#define __ELIBS_BOOKENGINE_H__

//#include "./../../emod/mod_bookengine.h"
#include "./../../module/mod_bookeg.h"

Hebook ebook_open(char *filename, unsigned char *errorno, EBOOK_LANGUAGE lang, EBOOK_CHARENCODE encode);            /* 打开ebook */

Hebookbitmap ebook_get_page(Hebook host, int pageno, unsigned char *errorno, unsigned char *destbuf);            /* 获取指定页码的显示页 */

Hebookbitmap ebook_goto_booktag(Hebook host, EBOOKTAG *tag, unsigned char *errorno, unsigned char *destbuf);        /* 跳转到书签位置*/

Hebookbitmap ebook_goto_end(Hebook host);                       /* 跳转到最后一页 */

Hebookbitmap ebook_find_text(Hebook host, char text[]);         /* 跳转到指定文本的页面 */

void ebook_free_pagemem(Hebookbitmap surface);                  /*释放当前显示页面的mem资源*/

void ebook_close(Hebook host);                                  /*退出该文档*/

Hebookbitmap ebook_close_and_getbookcover(Hebook g_host, int width, int height, unsigned char *errorno, unsigned char *destbuf);      /*获取到该BOOK的bookcover并退出*/


/**********************************************/
int     ebook_set_scnwidth(Hebook host, unsigned int scnwidth);     /* 设置显示屏的宽度 */

int     ebook_set_scnheight(Hebook host, unsigned int scnheight);   /* 设置显示屏的高度 */

int     ebook_set_fontsize(Hebook host, unsigned int fontsize);     /* 设置字体大小（对于PDF无效） */

void    ebook_set_reflow_mode(Hebook host);                         /* 设置reflow显示模式（PDF文档） */

void    ebook_cancel_reflow_mode(Hebook host);                      /* 取消reflow显示模式（PDF文档）*/

void    ebook_set_scale(Hebook host, float s);                      /*设置页面的缩放比例*/

void    ebook_set_margin_left(double left);                         /*设置左边空白大小*/

void    ebook_set_margin_right(double right);                       /*设置右边空白大小*/

void    ebook_set_margin_top(double top);                           /*设置上边空白大小*/

void    ebook_set_margin_bottom(double bottom);                     /*设置下边空白大小*/

void    ebook_setbackcolor(int red, int green, int blue);           /*设置背景色*/

void    ebook_setfontcolor(JM_FONT_COLOR color);                    /*设置字体色*/

/****************************************************************/
Hebookbitmap    ebook_get_bookcover(char *filename, int width, int height, unsigned char *errorno, unsigned char *destbuf);   /*获取书籍封面信息*/

void            ebook_free_bookcover(Hebookbitmap surface);                 /*释放封面的mem资源*/

void            ebook_get_bookinfo(char *filename, EBOOKINFO *bookinfo, unsigned char *errorno);    /*获取书籍相关信息*/

int             ebook_get_totalpagecount(Hebook host);                      /* 获取总的页面数 */

Hebookbitmap    ebook_get_screen(Hebook host_1, int num, unsigned char *errorno, unsigned char *destbuf);

void            ebook_save_booktag(Hebook host, int num, EBOOKTAG *sbooktag, unsigned char *error);         /*保存书签*/

void            ebook_settimezone(time_zone_e tz, int is_daylight);         /*设置时区*/

JM_FONT_COLOR   ebook_get_fontcolor(Hebook host);                           /*获取字体颜色*/

int             ebook_get_fontsize(Hebook host);                            /*获取字体大小*/

int             ebook_get_backcolor(Hebook host);                            /*获取背景色值*/

void            ebook_set_language(Hebook host, EBOOK_LANGUAGE lang);          /*设置电子书的语言*/

void            ebook_set_charencode(Hebook host, EBOOK_CHARENCODE encode);       /*设置电子书的编码格式*/

EBOOK_LANGUAGE  ebook_get_language(Hebook host);                            /*获取电子书的语言*/

EBOOK_CHARENCODE     ebook_get_charencode(Hebook host);                    /*获取电子书的编码格式*/
#endif /* __ELIBS_ebook_H__  */
