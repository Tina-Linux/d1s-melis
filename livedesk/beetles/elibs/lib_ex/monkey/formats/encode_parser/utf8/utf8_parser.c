/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "monkey_i.h"
#include "..\parser_plugin_i.h"
#include "utf8_parser.h"

static void utf8_config(txtEncodeParser *thiz, ParserConfig *pconfig);
static void utf8_start(txtEncodeParser *thiz, __s32 startoffset);
static void utf8_next(txtEncodeParser *thiz, int current_start);
static void utf8_prev(txtEncodeParser *thiz, int current_start);
static __s32 utf8_analysis(txtEncodeParser *thiz, MkLine *lines);
static void utf8_dispstr(char *s, int len, int x, int y);
static void utf8_destroy(txtEncodeParser *thiz);

txtEncodeParser *utf8_parserNew(void)
{
    txtEncodeParser *utf8_parser;
    utf8_parser = esMEMS_Malloc(0, sizeof(txtEncodeParser));

    if (!utf8_parser)
    {
        return NULL;
    }

    eLIBs_memset(utf8_parser, 0, sizeof(txtEncodeParser));
    utf8_parser->config         = utf8_config;
    utf8_parser->start_seek     = utf8_start;
    utf8_parser->next           = utf8_next;
    utf8_parser->prev           = utf8_prev;
    utf8_parser->analysis       = utf8_analysis;
    utf8_parser->dispstr        = utf8_dispstr;
    utf8_parser->destroy        = utf8_destroy;
    utf8_parser->bufferTxt      = esMEMS_Palloc(2 * BufferLenght / 1024, 0); //分配2*8个页面，每个页面大小1kbytesbufferTxt
    return utf8_parser;
}


void utf8_config(txtEncodeParser *thiz, ParserConfig *config)
{
    __u16 tmp_chinese;
    config->p_dispstr   =   thiz->dispstr;
    thiz->fd            =   config->fd;
    thiz->txtLength     =   config->len;
    thiz->viewWidth     =   config->xScrWidth;
    //thiz->viewHeight  =   config->yScrHeight;
    //thiz->lineofpage  =   config->lineofpage;
    //thiz->FontHeight  =   config->FontHeight;
    thiz->bufferTxtlen  =   0;
    thiz->start         =   config->startOffset;

    if (thiz->start <= 3)
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

/*
** fseek the start position of a word
** arg startoffset may be the middle part of a word
*/
void utf8_start(txtEncodeParser *thiz, int startoffset)
{
    __u8 *buff;
    int len, i;
    buff  = thiz->bufferTxt + BufferLenght;                         //缓冲区指针

    if (startoffset <= 3)
    {
        thiz->bof           =   EPDK_TRUE;
        thiz->start         =   3;
        eLIBs_fseek(thiz->fd, thiz->start, ELIBS_SEEK_SET);
        return;
    }

    thiz->bof               =   EPDK_FALSE;
    eLIBs_fseek(thiz->fd, startoffset, ELIBS_SEEK_SET);
    len = eLIBs_fread(buff, sizeof(char), BufferLenght, thiz->fd);

    if (len != BufferLenght)
    {
        thiz->eof   = EPDK_TRUE;
    }

    i = 0;

    while (i < len)
    {
        if ((buff[i] & 0xc0) == 0x80)
        {
            i++;
        }
        else
        {
            break;
        }
    }

    if (i == len)
    {
        thiz->start = startoffset;
    }
    else
    {
        thiz->start = startoffset + i;
    }

    return;
}

void utf8_next(txtEncodeParser *thiz, int current_start)
{
    int len, end;
    thiz->start          = current_start;
    thiz->bof            = (current_start < 3) ? EPDK_TRUE : EPDK_FALSE;
    thiz->bufferTxtlen   = 0;                                       //文件数据缓冲清0
    eLIBs_fseek(thiz->fd, thiz->start, ELIBS_SEEK_SET);
    len = eLIBs_fread(thiz->bufferTxt, sizeof(char), BufferLenght, thiz->fd);

    if (len != BufferLenght)                                        //已读到了文章的尾部
    {
        thiz->eof = EPDK_TRUE;                                  //置文件尾部标志位
        thiz->bufferTxtlen = len;
        return;
    }

    thiz->eof = EPDK_FALSE;                                     //删除缓冲区中最后一个回车以后的字符
    end = len;

    while (end > 0)
    {
        int v = thiz->bufferTxt[end - 1] & 0xff;

        if ((v & 0xc0) == 0x80)              // 1100 0000
        {
            end--;
        }
        else
        {
            break;
        }
    }

    if (0 == end)                                                    // 没找到\n 换行
    {
        thiz->bufferTxtlen = len;
        return;
    }

    thiz->bufferTxtlen = end;
    return;
}


void utf8_prev(txtEncodeParser *thiz, int current_start)
{
    int start, tmp;
    int len;
    __u8 *b;
    start       = current_start - BufferLenght;
    thiz->eof   = EPDK_FALSE;
    b           = thiz->bufferTxt + BufferLenght;

    if (start <= 3)
    {
        start = 3;
        thiz->bof = EPDK_TRUE;
        eLIBs_fseek(thiz->fd, start, ELIBS_SEEK_SET);
        len = eLIBs_fread(thiz->bufferTxt, sizeof(char), current_start, thiz->fd);
        thiz->bufferTxtlen = current_start - 3;
        thiz->start = 3;
        return;
    }

    eLIBs_fseek(thiz->fd, start, ELIBS_SEEK_SET);
    len = eLIBs_fread(b, sizeof(char), BufferLenght, thiz->fd);

    if (len != BufferLenght)                                //已读到了文章的尾部
    {
        thiz->eof = EPDK_TRUE;                              //置文件尾部标志位
        thiz->bufferTxtlen = len;
    }

    tmp = 0;

    while (tmp < len)
    {
        int v = b[tmp++] & 0xff;

        if ((v & 0xc0) != 0x80)
        {
            break;
        }
    }

    if (tmp >= len - 1)
    {
        tmp = 0;
    }

    eLIBs_memcpy(thiz->bufferTxt, b + tmp, len - tmp + 1);
    thiz->bufferTxtlen  = len - tmp;
    thiz->start         = start + tmp;
}

/*
 * return : 0: cache analysis is not finished;
 *         -1: cache analysis is finished;
 */

static __s32 utf8_analysis(txtEncodeParser *thiz, MkLine *line)
{
    __u16 CharDist, CharWidth;
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

        if ((b == 13) || (b == 10))  //\r 换行\r\n
        {
            __u16 tmp = thiz->bufferTxt[c + 1] & 0xff;

            if (tmp == 10)
            {
                c += 2;
                l += 2;
            }
            else
            {
                c++;
                l++;
            }

            //tmp_line.len = l;
            //tmp_line.txt_off = thiz->start + c -l;    //注意thiz->currentoffset的初始化，应该record一下。
            //set_txt_line_value_push(lines, &tmp_line);
            line->start     = thiz->start + c - l;
            line->len       = l;
            thiz->cur_offset = c;
            w = 0;
            l = 0;
            return 0;
        }

        if ((b & 0x80) == 0)                 /* Single byte (ASCII)  */
        {
            if (b == 9)
            {
                b = 0x20;    //tab->空格
            }

            CharDist    = b;
            CharWidth   = GUI_GetCharDistX(CharDist);

            if (w + CharWidth > thiz->viewWidth)
            {
                //当前行无法显示完整
                //tmp_line.len = l;
                //tmp_line.txt_off = thiz->start + c -l;
                //set_txt_line_value_push(lines, &tmp_line);
                line->start     = thiz->start + c - l;
                line->len       = l;
                thiz->cur_offset = c;
                w = 0;
                l = 0;
                return 0;
            }
            else
            {
                c++;
                l++;
                w += CharWidth;
            }
        }
        else if ((b & 0xe0) == 0xc0)             /* Double byte sequence */
        {
            /*
            CharDist = (b & 0x1f) << 6;
            b  = thiz->bufferTxt[c+1] & 0xff;
            b &= 0x3f;
            CharDist |= b;
            */
            //CharWidth     = GUI_GetCharDistX(CharDist);
            CharWidth = thiz->chinese_width ;

            if (w + CharWidth > thiz->viewWidth)
            {
                //当前行无法显示完整
                //              tmp_line.len = l;
                //              tmp_line.txt_off = thiz->start + c -l;
                //              set_txt_line_value_push(lines, &tmp_line);
                line->start     = thiz->start + c - l;
                line->len       = l;
                thiz->cur_offset = c;
                w = 0;
                l = 0;
                return 0;
            }
            else
            {
                c += 2;
                l += 2;
                w += CharWidth;
            }
        }
        else if ((b & 0xf0) == 0xe0)         /* 3 byte sequence    */
        {
            /*
            CharDist = (b & 0x0f) << 12;
            b = thiz->bufferTxt[c+1] & 0xff;;
            b &= 0x3f;
            CharDist |= (b << 6);
            b = thiz->bufferTxt[c+2] & 0xff;
            b &= 0x3f;
            CharDist |= b;
            */
            //CharWidth     = GUI_GetCharDistX(CharDist);
            CharWidth = thiz->chinese_width ;

            if (w + CharWidth > thiz->viewWidth)
            {
                //当前行无法显示完整
                //              tmp_line.len = l;
                //              tmp_line.txt_off = thiz->start + c -l;
                //              set_txt_line_value_push(lines, &tmp_line);
                line->start     = thiz->start + c - l;
                line->len       = l;
                thiz->cur_offset = c;
                w = 0;
                l = 0;
                return 0;
            }
            else
            {
                c += 3;
                l += 3;
                w += CharWidth;
            }
        }
        else        //不支持的编码
        {
            c++;
            b = thiz->bufferTxt[c];

            while ((b & 0xc0) == 0x80)
            {
                c++;
                b = thiz->bufferTxt[c];
            }
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

static void utf8_dispstr(char *s, int len, int x, int y)
{
    int xdist;
    __u16 b, tmp;
    int i = 0;
    xdist = x;

    while (i < len)
    {
        b = s[i] & 0xff;

        if ((b & 0x80) == 0)     /* Single byte (ASCII)  */
        {
            if (b == 9)
            {
                b = 0x20;    //tab->空格
            }

            GUI_DispCharAt(b, xdist, y);
            xdist += GUI_GetCharDistX(b);
            i++;
        }
        else if ((b & 0xe0) == 0xc0) /* Double byte sequence */
        {
            tmp = (b & 0x1f) << 6;
            b  = s[i + 1] & 0xff;
            b &= 0x3f;
            tmp |= b;
            GUI_DispCharAt(tmp, xdist, y);
            xdist += GUI_GetCharDistX(tmp);
            i += 2;
        }
        else if ((b & 0xf0) == 0xe0)
        {
            tmp = (b & 0x0f) << 12;
            b = s[i + 1] & 0xff;;
            b &= 0x3f;
            tmp |= (b << 6);
            b = s[i + 2] & 0xff;
            b &= 0x3f;
            tmp |= b;
            GUI_DispCharAt(tmp, xdist, y);
            xdist += GUI_GetCharDistX(tmp);
            i += 3;
        }
        else
        {
            i++;
            b = s[i] & 0xff;

            while ((b & 0xc0) == 0x80)
            {
                i++;
                b = s[i];
            }
        }
    }
}

void utf8_destroy(txtEncodeParser *thiz)
{
    esMEMS_Pfree(thiz->bufferTxt, 2 * BufferLenght / 1024);
    esMEMS_Mfree(0, thiz);
}
