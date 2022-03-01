/*
**************************************************************************************************************
*                                                   ePOS
*                                  the Easy Portable/Player Operation System
*                                             update sub-system
*
*                                    (c) Copyright 2008, Andy.zhang China
*                                             All Rights Reserved
*
* File      : utf16_little_parser.h
* Version   : V1.0
* Data      : 2009-5-02
* By        : Andy.zhang
* Biref     : utf16_little编码解析
**************************************************************************************************************
*/
#include "monkey_i.h"
#include "..\parser_plugin_i.h"
#include "utf16_little_parser.h"

static void utf16little_config(txtEncodeParser *thiz, ParserConfig *pconfig);
static void utf16little_start(txtEncodeParser *thiz, __s32 startoffset);
static void utf16little_next(txtEncodeParser *thiz, int current_start);
static void utf16little_prev(txtEncodeParser *thiz, int current_start);
static __s32 utf16little_analysis(txtEncodeParser *thiz, MkLine *lines);
static void utf16little_dispstr(char *s, int len, int x, int y);
static void utf16little_destroy(txtEncodeParser *thiz);

txtEncodeParser *utf16little_parserNew(void)
{
    txtEncodeParser *utf16little_parser;
    utf16little_parser = esMEMS_Malloc(0, sizeof(txtEncodeParser));

    if (!utf16little_parser)
    {
        return NULL;
    }

    eLIBs_memset(utf16little_parser, 0, sizeof(txtEncodeParser));
    utf16little_parser->config      = utf16little_config;
    utf16little_parser->start_seek  = utf16little_start;
    utf16little_parser->next        = utf16little_next;
    utf16little_parser->prev        = utf16little_prev;
    utf16little_parser->analysis    = utf16little_analysis;
    utf16little_parser->dispstr     = utf16little_dispstr;
    utf16little_parser->destroy     = utf16little_destroy;
    utf16little_parser->bufferTxt   = esMEMS_Palloc(2 * BufferLenght / 1024, 0); //分配2*8个页面，每个页面大小1kbytes
    return utf16little_parser;
}

static void utf16little_config(txtEncodeParser *thiz, ParserConfig *config)
{
    __u16 tmp_chinese;
    config->p_dispstr   =   thiz->dispstr;
    thiz->fd            =   config->fd;
    thiz->txtLength     =   config->len;
    thiz->viewWidth     =   config->xScrWidth;
    //  thiz->viewHeight    =   config->yScrHeight;
    //  thiz->lineofpage    =   config->lineofpage;
    //  thiz->FontHeight    =   config->FontHeight;
    thiz->bufferTxtlen  =   0;
    thiz->start         =   config->startOffset;

    if (thiz->start <= 2)
    {
        thiz->bof = EPDK_TRUE;
    }
    else
    {
        thiz->bof = EPDK_FALSE;
    }

    thiz->eof           =   EPDK_FALSE;
    thiz->pFont         =   config->pFont;
    thiz->hlyr          =   config->hlyr;
    GUI_LyrWinSel(thiz->hlyr);
    GUI_SetTextMode(GUI_TM_TRANS);
    GUI_SetFont(thiz->pFont);
    GUI_SetFontMode(GUI_FONTMODE_8BPP256);
    tmp_chinese         = eLIBs_GB2312_to_Unicode(0xced2);
    thiz->chinese_width = GUI_GetCharDistX(tmp_chinese);
}

static void utf16little_start(txtEncodeParser *thiz, int startoffset)
{
    __u8 *buff;
    int len;
    buff  = thiz->bufferTxt + BufferLenght;                         //缓冲区指针

    if (startoffset <= 2)
    {
        thiz->bof           =   EPDK_TRUE;
        thiz->start         =   2;
        eLIBs_fseek(thiz->fd, thiz->start, ELIBS_SEEK_SET);         //待定--------------
        return;
    }

    thiz->bof               =   EPDK_FALSE;
    startoffset = (startoffset % 2) ? (startoffset + 1) : (startoffset);
    eLIBs_fseek(thiz->fd, startoffset, ELIBS_SEEK_SET);
    len = eLIBs_fread(buff, sizeof(char), BufferLenght, thiz->fd);

    if (len != BufferLenght)
    {
        thiz->eof   = EPDK_TRUE;
    }

    thiz->eof = EPDK_FALSE;
    thiz->start = startoffset;
    return;
}

static void utf16little_next(txtEncodeParser *thiz, int current_start)
{
    int len, end;
    thiz->start          = current_start;
    thiz->bof            = (current_start <= 2) ? EPDK_TRUE : EPDK_FALSE;
    thiz->bufferTxtlen   = 0;                                       //文件数据缓冲清0
    eLIBs_fseek(thiz->fd, thiz->start, ELIBS_SEEK_SET);
    len = eLIBs_fread(thiz->bufferTxt, sizeof(char), BufferLenght, thiz->fd);

    if (len != BufferLenght)                                    //已读到了文章的尾部
    {
        thiz->eof = EPDK_TRUE;                                  //置文件尾部标志位
        thiz->bufferTxtlen = len;
        return;
    }

    thiz->eof = EPDK_FALSE;                                     //删除缓冲区中最后一个回车以后的字符
    end = (len % 2) ? (len + 1) : len;
    thiz->bufferTxtlen = end;
    return;
}

static void utf16little_prev(txtEncodeParser *thiz, int current_start)
{
    int start;
    int len;
    //__u8 *b;
    start       = current_start - BufferLenght;
    thiz->bof   = EPDK_FALSE;

    if (start < 0)
    {
        start = 2;
        thiz->bof = EPDK_TRUE;
        eLIBs_fseek(thiz->fd, start, ELIBS_SEEK_SET);
        len = eLIBs_fread(thiz->bufferTxt, sizeof(char), current_start, thiz->fd);
        thiz->bufferTxtlen = current_start;
        thiz->start = 0;
        return;
    }

    start = (start % 2) ? (start + 1) : start;
    thiz->start = start;
    eLIBs_fseek(thiz->fd, start, ELIBS_SEEK_SET);
    len = eLIBs_fread(thiz->bufferTxt, sizeof(char), BufferLenght, thiz->fd);

    if (len != BufferLenght)                                //已读到了文章的尾部
    {
        thiz->eof           = EPDK_TRUE;                                //置文件尾部标志位
    }

    thiz->bufferTxtlen = len;
}

static __s32 utf16little_analysis(txtEncodeParser *thiz, MkLine *line)
{
    __u16 CharWidth;
    int c = thiz->cur_offset;       //buffer索引
    int w = 0;                      //每行的字符总宽度
    int l = 0;                      //每行包含的字符数
    GUI_LyrWinSel(thiz->hlyr);
    GUI_SetTextMode(GUI_TM_TRANS);
    GUI_SetFont(thiz->pFont);
    GUI_SetFontMode(GUI_FONTMODE_8BPP256);

    while (c < thiz->bufferTxtlen)
    {
        __u16 b = thiz->bufferTxt[c] & 0xff;
        b += (thiz->bufferTxt[c + 1] & 0xff) << 8; //小端格式，低位在前, 高位在后

        if ((b == 13) || (b == 10))  //\r 换行\r\n
        {
            __u16 tmp = thiz->bufferTxt[c + 2] & 0xff;
            tmp += (thiz->bufferTxt[c + 3] & 0xff) << 8; //小端格式，低位在前, 高位在后

            if (tmp == 10)
            {
                c += 4;
                l += 4;
            }
            else
            {
                c += 2;
                l += 2;
            }

            //          tmp_line.len = l;
            //          tmp_line.txt_off = thiz->start + c -l;  //注意thiz->currentoffset的初始化，应该record一下。
            //          set_txt_line_value_push(lines, &tmp_line);
            line->start     = thiz->start + c - l;
            line->len       = l;
            thiz->cur_offset = c;
            w = 0;
            l = 0;
            //c +=2;
            return 0;
        }

        if (b < 0x80)
        {
            CharWidth = GUI_GetCharDistX(b);
        }
        else
        {
            CharWidth = thiz->chinese_width;
        }

        //CharWidth = GUI_GetCharDistX(b);
        if (w + CharWidth > thiz->viewWidth)
        {
            //当前行无法显示完整
            //          tmp_line.len = l;
            //          tmp_line.txt_off = thiz->start + c -l;
            //          set_txt_line_value_push(lines, &tmp_line);
            line->start     = thiz->start + c - l;
            line->len       = l;
            thiz->cur_offset = c;
            w = 0;
            l = 0;
            //c +=2;
            return 0;
        }
        else
        {
            c += 2;
            l += 2;
            w += CharWidth;
        }
    }

    thiz->cur_offset = 0;

    if (l > 0)
    {
        //      tmp_line.len = l;
        //      tmp_line.txt_off = thiz->start + c -l;
        //      set_txt_line_value_push(lines, &tmp_line);
        line->start     = thiz->start + c - l;
        line->len       = l;
        return -2;
    }

    return -1;
}

static void utf16little_dispstr(char *s, int len, int x, int y)
{
    __u16 xdist;
    __u16 b, tmp;
    int i = 0;
    xdist = x;

    while (i < len)
    {
        tmp = s[i] & 0xff;
        b   = s[i + 1] & 0xff;
        tmp += b << 8;

        if ((tmp == 13) || (tmp == 10))  // 换行\r\n
        {
            i += 2;
            continue;
        }

        if (tmp == 9)
        {
            tmp = 0x20;
        }

        GUI_DispCharAt(tmp, xdist, y);
        xdist += GUI_GetCharDistX(tmp);
        i += 2;
    }
}


static void utf16little_destroy(txtEncodeParser *thiz)
{
    esMEMS_Pfree(thiz->bufferTxt, 2 * BufferLenght / 1024);
    esMEMS_Mfree(0, thiz);
}



