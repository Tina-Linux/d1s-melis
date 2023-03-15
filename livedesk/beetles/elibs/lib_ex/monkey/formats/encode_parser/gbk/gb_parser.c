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
#include "gb_parser.h"

static void  gb_config(txtEncodeParser *thiz, ParserConfig *pconfig);
static void  gb_start(txtEncodeParser *thiz, __s32 startoffset);
static void  gb_next(txtEncodeParser *thiz, int current_start);
static void  gb_prev(txtEncodeParser *thiz, int current_start);
static __s32 gb_analysis(txtEncodeParser *thiz, MkLine *lines);
static void  gb_dispstr(char *s, int len, int x, int y);
static void  gb_destroy(txtEncodeParser *thiz);

txtEncodeParser *gb_parserNew(void)
{
    txtEncodeParser *gb_parser;
    gb_parser = (txtEncodeParser *)g_malloc(sizeof(txtEncodeParser));

    if (!gb_parser)
    {
        __err(" gb_parserNew error \n");
        return NULL;
    }

    g_memset(gb_parser, 0, sizeof(txtEncodeParser));
    gb_parser->config       = gb_config;
    gb_parser->start_seek   = gb_start;
    gb_parser->next         = gb_next;
    gb_parser->prev         = gb_prev;
    gb_parser->analysis     = gb_analysis;
    gb_parser->dispstr      = gb_dispstr;
    gb_parser->destroy      = gb_destroy;
    gb_parser->bufferTxt    = esMEMS_Palloc(2 * BufferLenght / 1024, 0); //分配2*8个页面，每个页面大小1kbytes
    return gb_parser;
}

static void gb_config(txtEncodeParser *thiz, ParserConfig *config)
{
    __u16 tmp_chinese;
    config->p_dispstr   =   thiz->dispstr;
    thiz->fd            =   config->fd;
    thiz->txtLength     =   config->len;
    thiz->viewWidth     =   config->xScrWidth;
    //  thiz->lineofpage    =   config->lineofpage;
    thiz->bufferTxtlen  =   0;
    thiz->start         =   config->startOffset;

    if (thiz->start == 0)
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

static void gb_start(txtEncodeParser *thiz, int startoffset)
{
    __u8    *buff;
    int     len, i;
    buff  = thiz->bufferTxt + BufferLenght;                         //缓冲区指针

    if (startoffset == 0)
    {
        thiz->bof           =   EPDK_TRUE;
        thiz->start         =   startoffset;
        eLIBs_fseek(thiz->fd, thiz->start, ELIBS_SEEK_SET);
        //thiz->cur_offset = 0;
        return;
    }

    thiz->bof               =   EPDK_FALSE;
    eLIBs_fseek(thiz->fd, startoffset, ELIBS_SEEK_SET);
    len = eLIBs_fread(buff, sizeof(char), BufferLenght, thiz->fd);

    if (len != BufferLenght)
    {
        thiz->eof   = EPDK_TRUE;
    }
    else
    {
        thiz->eof   = EPDK_FALSE;
    }

    i = 0;

    while (i < len)
    {
        if ((buff[i] & 0xff) <= 0x80)                                     // ansi 字符
        {
            break;
        }

        if (i < len - 1)
        {
            if ((buff[i] == 0xa1) && (buff[i + 1] == 0xa3))                  // '.'
            {
                i++;
                break;
            }

            if ((buff[i] == 0xa3) && (buff[i + 1] == 0xac))                   // ','
            {
                i++;
                break;
            }
        }

        i++;
    }

    if (i == len - 1)
    {
        thiz->start = startoffset;
    }
    else
    {
        thiz->start = startoffset + i + 1;
    }

    //thiz->cur_offset = 0;
    return;
}

static void gb_next(txtEncodeParser *thiz, int current_start)
{
    int len, end;
    thiz->start          = current_start;
    thiz->bof            = (current_start == 0) ? EPDK_TRUE : EPDK_FALSE;
    thiz->bufferTxtlen   = 0;                                   //文件数据缓冲清0
    eLIBs_fseek(thiz->fd, thiz->start, ELIBS_SEEK_SET);
    len = eLIBs_fread(thiz->bufferTxt, sizeof(char), BufferLenght, thiz->fd);

    if (len != BufferLenght)                                    //已读到了文章的尾部
    {
        thiz->eof = EPDK_TRUE;                                  //置文件尾部标志位
        thiz->bufferTxtlen = len;
        //thiz->cur_offset = 0;
        return;
    }

    thiz->eof = EPDK_FALSE;                                     //删除缓冲区中最后一个回车以后的字符
    end = len;

    while (end > 0)
    {
        int v = thiz->bufferTxt[end - 1] & 0xff;

        if (v <= 0x80)
        {
            break;                                                  // \n 换行
        }

        if (end > 1)
        {
            int u = thiz->bufferTxt[end - 2] & 0xff;

            if ((u == 0xa1) && (v == 0xa3))
            {
                break;
            }

            if ((u == 0xa3) && (v == 0xac))
            {
                break;
            }
        }

        end--;
    }

    if (0 == end)                                                    // 没找到\n 换行
    {
        thiz->bufferTxtlen = len;
        return;
    }

    thiz->bufferTxtlen = end;
    //thiz->cur_offset = 0;
    return;
}

static void gb_prev(txtEncodeParser *thiz, int current_start)
{
    int start, tmp;
    int len;
    __u8 *b;
    start       = current_start - BufferLenght;
    thiz->bof   = EPDK_FALSE;
    b           = thiz->bufferTxt + BufferLenght;

    if (start <= 0)
    {
        start = 0;
        thiz->bof = EPDK_TRUE;
        eLIBs_fseek(thiz->fd, start, ELIBS_SEEK_SET);
        len = eLIBs_fread(thiz->bufferTxt, sizeof(char), current_start, thiz->fd);
        thiz->bufferTxtlen = current_start;
        thiz->start = 0;
        //thiz->cur_offset = 0;
        return;
    }

    eLIBs_fseek(thiz->fd, start, ELIBS_SEEK_SET);
    len = eLIBs_fread(b, sizeof(char), BufferLenght, thiz->fd);

    if (len != BufferLenght)                                    //已读到了文章的尾部
    {
        thiz->eof = EPDK_TRUE;                                  //置文件尾部标志位
        thiz->bufferTxtlen = len;
    }

    tmp = 0;

    while (tmp < len)
    {
        int v = b[tmp] & 0xff;

        if (v <= 0x80)          //\n 换行
        {
            break;
        }

        if (tmp < len - 1)
        {
            if ((b[tmp] == 0xa1) && (b[tmp + 1] == 0xa3))
            {
                tmp += 2;
                break;
            }

            if ((b[tmp] == 0xa3) && (b[tmp + 1] == 0xac))
            {
                tmp += 2;
                break;
            }
        }

        tmp++;
    }

    if (tmp >= len - 1)
    {
        tmp = 0;
    }

    eLIBs_memcpy(thiz->bufferTxt, b + tmp, len - tmp + 1);
    thiz->bufferTxtlen = len - tmp;
    thiz->start      = start + tmp;
    //thiz->cur_offset = 0;
    return;
}

/*
 * return : 0: cache analysis is not finished;
 *         -1: cache analysis is finished;
 */

static __s32 gb_analysis(txtEncodeParser *thiz, MkLine *line)
{
    __u16 CharDist, CharWidth;
    int c = thiz->cur_offset;       //buffer索引
    int w = 0;                      //每行的字符总宽度
    int l = 0;                      //每行包含的字符数s
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

            line->start     = thiz->start + c - l;
            line->len       = l;
            thiz->cur_offset = c;
            w = 0;
            l = 0;
            return 0;
        }

        //字符处理
        if (b > 0x7f)   //汉字
        {
            CharDist  = thiz->bufferTxt[c + 1] & 0xff;
            CharDist += b << 8;
            //CharWidth = GUI_GetCharDistX(CharDist);
            CharWidth = thiz->chinese_width;

            if (w + CharWidth > thiz->viewWidth)
            {
                //当前行无法显示完整
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
        else            //英文
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
                line->start = thiz->start + c - l;
                line->len = l;
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
    }

    thiz->cur_offset = 0;

    if (l > 0)
    {
        line->start = thiz->start + c - l;
        line->len = l;
        return -2;
    }

    return -1;
}

static void  gb_dispstr(char *s, int len, int x, int y)
{
    __u16 xdist;
    __u16 b, tmp;
    __u16 unicode;
    int i = 0;
    xdist = x;

    while (i < len)
    {
        b = s[i] & 0xff;

        if ((b == 13) || (b == 10))  // 换行\r\n
        {
            tmp = s[i + 1] & 0xff;

            if (tmp == 10)
            {
                i += 2;
            }
            else
            {
                i++;
            }
        }
        else if (b > 0x7f)  //汉字
        {
            tmp  = s[i + 1] & 0xff;
            tmp += b << 8;
            unicode = eLIBs_GB2312_to_Unicode(tmp);
            GUI_DispCharAt(unicode, xdist, y);
            xdist += GUI_GetCharDistX(unicode);
            i += 2;
        }
        else                //英文
        {
            if (b == 9)
            {
                b = 0x20;
            }

            GUI_DispCharAt(b, xdist, y);
            xdist += GUI_GetCharDistX(b);
            i ++;
        }
    }
}

static void gb_destroy(txtEncodeParser *thiz)
{
    esMEMS_Pfree(thiz->bufferTxt, 2 * BufferLenght / 1024);
    g_free(thiz);
}
