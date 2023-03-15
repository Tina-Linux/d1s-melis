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
#ifndef __PARSER_PLUGIN_I_H__
#define __PARSER_PLUGIN_I_H__

#include "monkey_i.h"


#define BufferLenght        4*1024                          //8k数据    8*1024

struct tag_txtEncodeParser;
typedef struct tag_txtEncodeParser  txtEncodeParser;

typedef struct tag_ParserConfig
{
    ES_FILE     *fd;                                // 文件句柄
    __s32       len;                                // 当前文件的长度
    __s32       lineofpage;                         // 当前视图情况下可以显示的行数
    MkEncode    encodeType;                         // 编码格式

    __u32       xScrWidth;                          // 可视区域的宽
    __u32       startOffset;                        // 初始偏移量
    H_LYR       hlyr;                               // 图层句柄
    GUI_FONT    *pFont;                             // 文字句柄

    void (*p_dispstr)(char *s, int len, int x, int y);
} ParserConfig;


typedef void (*txt_parserConfig)(txtEncodeParser *thiz,     ParserConfig *pconfig);
typedef void (*txt_parserCacheStart)(txtEncodeParser *thiz, int startoffset);
typedef void (*txt_parserCacheNext)(txtEncodeParser *thiz,  int current_start);
typedef void (*txt_parserCachePrev)(txtEncodeParser *thiz,  int current_start);
typedef __s32(*txt_parserAnalysis)(txtEncodeParser *thiz,  MkLine *line);
typedef void (*txt_parserDispstr)(char *s, int len, int x, int y);
typedef void (*txt_parserDestroy)(txtEncodeParser *thiz);

struct tag_txtEncodeParser
{
    txt_parserConfig                config;
    txt_parserCacheStart            start_seek;
    txt_parserCacheNext             next;
    txt_parserCachePrev             prev;
    txt_parserAnalysis              analysis;
    txt_parserDispstr               dispstr;
    txt_parserDestroy               destroy;

    /* context */
    ES_FILE *fd;                            // 文件句柄
    __s32   txtLength;                      // 当前文件的长度
    __s32   viewWidth;                      // 可视区域的宽
    __s32   chinese_width;                  // 汉字的字符宽度(因为汉字的宽度一样大小，能够加速分析过程)

    __u8    *bufferTxt;                     // 文件缓冲
    __s32   bufferTxtlen;                   // 文件缓冲长度
    __s32   start;                          // 文件缓冲起始偏移量, 注意 : start 不一定和 ParserConfig.startOffset相同
    __s32   cur_offset;                     // analysis 当前偏移量
    __bool  eof;                            // 缓冲区内容是否已到文档最后面
    __bool  bof;                            // 缓冲区内容是否已到文档最前面

    H_LYR       hlyr;
    GUI_FONT    *pFont;

};

#endif /* __PARSER_PLUGIN_I_H__ */
