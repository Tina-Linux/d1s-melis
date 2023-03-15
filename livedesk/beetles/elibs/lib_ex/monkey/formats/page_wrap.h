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
