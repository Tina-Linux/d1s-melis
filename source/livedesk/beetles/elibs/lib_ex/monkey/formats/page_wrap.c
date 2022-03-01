/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : page_wrap.c
* By        : Andy.zhang
* Version   : v1.0
* ============================================================================================================
* 2009-11-3 9:39:42  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "monkey_i.h"
#include "page_wrap.h"
#include "circbuf.h"

struct tag_PageContent
{
    /* 断页上下文*/
    ES_FILE     *fd;                /* 文件句柄     */
    __s32       len;                /* 文件长度     */
    MkEncode    encodeType;         /* 编码格式     */
    H_LYR       hlyr;               /* 图层参数     */

    GUI_FONT    *pFont;             /* 图层字体句柄 */
    __s32       lineWidth;          /* 行宽         */

    MkLine  *line;                  /* 行信息表     */
    __u32   lineNum;                /* 页行数       */
    __s32   startOffset;            /* 页头偏移量   */
    __s32   endOffset;              /* 页尾偏移量   */

    CircularBuff *cb;
    /* string show function for different encode type such as  utf_8 , unicode,  gbk*/
    void (*p_dispstr)(char *s, int len, int x, int y);
};

/*********************************************************************************************************************/
static MkEncode get_txtCharEncodeType(ES_FILE *fd, MkEncode defStyle)
{
    MkEncode  encode;
    char     buff[3];
    eLIBs_fseek(fd, 0, SEEK_SET);
    eLIBs_fread(buff, 3, sizeof(char), fd);

    if ((buff[0] == 0xff) && (buff[1] == 0xfe))
    {
        encode = MK_CHAR_ENCODE_UNICODE;
    }
    else if ((buff[0] == 0xfe) && (buff[1] == 0xff))
    {
        encode = MK_CHAR_ENCODE_UNICODE_BIG;
    }
    else if ((buff[0] == 0xef) && (buff[1] == 0xbb) && (buff[2] == 0xbf))
    {
        encode = MK_CHAR_ENCODE_UTF8;
    }
    else
    {
        encode = defStyle;
    }

    return encode;
}



/* page wrap 句柄创建 */
PageContent *pagewrap_create(PageConfig *para)
{
    PageContent *content;
    LineConfig   config;
    /* 创建 PageContent 句柄 */
    content = (PageContent *)g_malloc(sizeof(PageContent));

    if (!content)
    {
        __err(" PageContent malloc error \n");
        return NULL;
    }

    g_memset(content, 0, sizeof(PageContent));
    content->fd         = para->fd;
    content->len        = para->len;
    content->encodeType = get_txtCharEncodeType(content->fd, para->defEncodeStyle);
    content->hlyr       = para->hlyr;
    content->pFont      = para->pFont;
    content->lineWidth  = para->lineWidth;
    content->lineNum    = para->lineNum;
    content->line       = (MkLine *)g_malloc(sizeof(MkLine) * content->lineNum);

    if (!(content->line))
    {
        __err(" PageContent malloc error \n");
        return NULL;
    }

    g_memset(content->line, 0, sizeof(MkLine)*content->lineNum);
    /* 创建 CircularBuff 句柄 */
    g_memset(&config, 0, sizeof(LineConfig));
    config.fd   = content->fd;
    config.len  = content->len;
    config.encodeType = content->encodeType;
    config.lineofpage = para->lineNum;
    config.xScrWidth  = para->lineWidth;
    config.startOffset = para->startOffset;
    config.hlyr       = para->hlyr;
    config.pFont      = para->pFont;
    content->cb       = circbuf_create(&config);
    /* 取得函数指针 */
    content->p_dispstr = config.p_dispstr;
    return content;
}

/* page wrap 设置 */
__s32   pagewrap_setting(PageContent *content, PageConfig *para)
{
    LineConfig   config;
    /* 设置 page wrap 信息 */
    content->fd         = para->fd;
    content->len        = para->len;
    content->encodeType = get_txtCharEncodeType(content->fd, para->defEncodeStyle);
    content->hlyr       = para->hlyr;
    content->pFont      = para->pFont;
    content->lineWidth  = para->lineWidth;

    if (content->lineNum != para->lineNum)
    {
        g_free(content->line);
        content->line = (MkLine *)g_malloc(sizeof(MkLine) * (para->lineNum));
        g_memset(content->line, 0, sizeof(MkLine) * (para->lineNum));
        content->lineNum = para->lineNum;
    }

    /* 设置 CircularBuff 信息*/
    g_memset(&config, 0, sizeof(LineConfig));
    config.fd   = content->fd;
    config.len  = content->len;
    config.encodeType = content->encodeType;
    config.lineofpage = para->lineNum;
    config.xScrWidth  = para->lineWidth;
    config.startOffset = para->startOffset;
    config.hlyr       = para->hlyr;
    config.pFont      = para->pFont;
    circbuf_setting(content->cb, &config);
    content->p_dispstr = config.p_dispstr;      // 取得函数指针
    return EPDK_OK;
}

__s32 pagewrap_start(PageContent *content)
{
    __s32 ret;
    g_memset(content->line, 0, sizeof(MkLine) * (content->lineNum));
    ret = circbuf_getStartLines(content->cb, content->line, content->lineNum);
    return ret;
}

/* 读取下一页内容 */
__s32   pagewrap_next(PageContent *content)
{
    __s32 ret;
    g_memset(content->line, 0, sizeof(MkLine) * (content->lineNum));
    ret = circbuf_getNextLines(content->cb, content->line, content->lineNum);
    return ret;
}

/* 读取上一页内容 */
__s32   pagewrap_prev(PageContent *content)
{
    __s32 ret;
    g_memset(content->line, 0, sizeof(MkLine) * (content->lineNum));
    ret = circbuf_getPrevLines(content->cb, content->line, content->lineNum);
    return ret;
}

/* 跳转到offset处( 仅仅改变偏移量 ) */
__s32   pagewrap_fseek(PageContent *content,  __s32 offset)
{
    __s32 ret;
    g_memset(content->line, 0, sizeof(MkLine) * (content->lineNum));
    circbuf_seek(content->cb, offset);
    ret = circbuf_getStartLines(content->cb, content->line, content->lineNum);
    return ret;
}


/* 查询行信息表 */
MkLine *pagewrap_getLine(PageContent *content)
{
    return content->line;
}

/* 显示一行字符*/
void    pagewrap_displayStr(PageContent *content, char *s, int len, int x, int y)
{
    content->p_dispstr(s, len, x, y);
}

/* 查询页头偏移量 */
__s32   pagewrap_getStartOffset(PageContent *content)
{
    return content->line[0].start;
}

/* 查询页尾偏移量 */
__s32   pagewrap_getEndOffset(PageContent *content)
{
    MkLine *line;
    int     end;
    int     i;

    for (i = content->lineNum - 1 ; i >= 0; i--)
    {
        if (content->line[i].len != 0)
        {
            break;
        }
    }

    line = content->line + i;
    end  = line->start + line->len - 1;
    return end;
}

/* 查询是否到文章最开始处*/
__bool  pagewrap_isHead(PageContent *content)
{
    int start;
    start = pagewrap_getStartOffset(content);
    return (start == 0);
}

/* 查询是否到文章结尾处 */
__bool  pagewrap_isTail(PageContent *content)
{
    int end;
    end = pagewrap_getEndOffset(content);
    return (end == content->len);
}

/* 删除 page wrap 句柄 */
__s32   pagewrap_delete(PageContent *content)
{
    circbuf_delete(content->cb);
    g_free(content->line);
    g_free(content);
    return EPDK_OK;
}

