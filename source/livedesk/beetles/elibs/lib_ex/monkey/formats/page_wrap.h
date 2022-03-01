/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : page_wrap.h
* By        : Andy.zhang
* Version   : v1.0
* ============================================================================================================
* 2009-11-3 9:39:42  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __PAGE_WRAP_H__
#define __PAGE_WRAP_H__


struct  tag_PageContent;
typedef struct tag_PageContent PageContent;

/* 分页可配置参数 */
typedef struct
{
    ES_FILE     *fd;                /* 文件句柄     */
    int          len;               /* 文件长度     */
    H_LYR       hlyr;               /* 图层参数     */
    GUI_FONT    *pFont;             /* 图层字体句柄 */
    __s32       lineWidth;          /* 行宽         */
    __s32       lineNum;            /* 页行数       */
    MkEncode    defEncodeStyle;     /* 默认编码方式 */
    __s32       startOffset;        /* 初始偏移量   */
} PageConfig;

/* page wrap 创建 */
PageContent *pagewrap_create(PageConfig *para);

/* page wrap 设置 */
__s32   pagewrap_setting(PageContent *content, PageConfig *para);

/* 读初始页内容 */
__s32   pagewrap_start(PageContent *content);

/* 读取下一页内容 */
__s32   pagewrap_next(PageContent *content);

/* 读取上一页内容 */
__s32   pagewrap_prev(PageContent *content);

/* 跳转到offset处 */
__s32   pagewrap_fseek(PageContent *content,  int offset);

/* 查询行信息表 */
MkLine *pagewrap_getLine(PageContent *content);

/* 显示一行字符*/
void    pagewrap_displayStr(PageContent *content, char *s, int len, int x, int y);

/* 查询页头偏移量 */
__s32   pagewrap_getStartOffset(PageContent *content);

/* 查询页尾偏移量 */
__s32   pagewrap_getEndOffset(PageContent *content);

/* 查询是否到文章最开始处*/
__bool  pagewrap_isHead(PageContent *content);

/* 查询是否到文章结尾处 */
__bool  pagewrap_isTail(PageContent *content);

/* 删除 page wrap 句柄 */
__s32   pagewrap_delete(PageContent *content);

#endif /* __PAGE_WRAP_H__ */

