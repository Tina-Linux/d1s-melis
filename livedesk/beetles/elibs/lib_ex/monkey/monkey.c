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

#include "regedit\\monkey_reg.h"
#include "formats\\page_wrap.h"
#include "mnpl\\mnpl.h"
#include "bookmark\\bookmark.h"

/**********************************************************************************************************************/
#define MK_PAGE_CHAR                    4096
#define MK_FILE_NAME                    4096
#define TEST_FILE                       "e:\\all.txt"

typedef struct
{
    /* now play list module */
    MNPL        *npl;                   /* now play list handle     */
    char        filename[MK_FILE_NAME]; /* 文件名称                 */
    ES_FILE     *fd;                    /* 文件句柄                 */
    int         len;                    /* 文件长度                 */

    /* bookmark module      */
    HBMK        bmk;                    /* 书签模块句柄             */
    __bool      exist;                  /* 是否记录上次最后阅读位置 */
    __s32       lastOffset;             /* 上次最后阅读位置         */
    //__u32     startOffset;            /* 起始页偏移量             */

    /* 注册表配置 module    */
    MkFont      mFont;                  /* 字体大小                 */
    MkColor     mColor;                 /* 字体颜色                 */
    MkEncode    defEncodeStyle;         /* 编码方式                 */
    MkSwitch    mSwitch;                /* 页切换模式               */
    MkRotate    mRotate;                /* 旋转模式                 */
    MkBright    mBright;                /* 背景亮度                 */
    MkBack      mBack;                  /* 背景图片                 */
    MkAutoP     mAutoP;                 /* 自动翻页                 */

    /* monkey core module   */
    MkcConfig   mConfig;                /* 资源配置信息             */

    GUI_COLOR   color;                  /* 当前显示颜色             */
    GUI_FONT   *font;                   /* 当前字体                 */
    __s32       fontHeight;             /* 字体高度                 */
    __s32       lineNum;                /* 页行数目                 */

    __bool      lyrFlag;                /* 当前是否在hlyr_1图层显示 */
    char        buff[MK_PAGE_CHAR];     /* 页数据缓冲               */
    __s32       pageStart, pageEnd;     /* 页起始，页结束位置       */
    __u32       *pPal;                  /* 调色板                   */

    /* page wrap module  */
    PageConfig  pConfig;                /* 分页配置信息             */
    PageContent *content;               /* 分页句柄                 */
} MonkeyCore;

enum
{
    LAYER_SHIFT_DIRECT_UP = 0,
    LAYER_SHIFT_DIRECT_DOWN,
    LAYER_SHIFT_DIRECT_LEFT,
    LAYER_SHIFT_DIRECT_RIGHT,
    LAYER_SHIFT_DIRECT_
};

/**********************************************************************************************************************/
static void __changPalette(MonkeyCore *core, __u32 color)
{
    ES_FILE      *p_disp;
    int i;
    color &= 0x00ffffff;

    for (i = 0; i < 256; i++)
    {
        *(core->pPal + i) = (i << 24) | color;
    }

    p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

    if (!p_disp)
    {
        __err("open display device fail!\n");
    }

    //eLIBs_fioctrl(p_disp, DISP_CMD_SET_PALETTE_TBL, 1024, core->pPal);
    __err("not support anymore after d100\n");
    eLIBs_fclose(p_disp);
}

static GUI_COLOR __charColorMap(MkColor mColor)
{
    GUI_COLOR color;

    switch (mColor)
    {
        case MK_COLOR_BLACK:
            color = GUI_BLACK;
            break;

        case MK_COLOR_BLUE:
            color = GUI_RED;
            break;

        case MK_COLOR_GREEN:
            color = GUI_GREEN;
            break;

        case MK_COLOR_RED:
            color = GUI_BLUE;
            break;

        case MK_COLOR_WHITE:
            color = GUI_WHITE;
            break;

        default:
            color = GUI_GREEN;
            break;
    }

    return color;
}

static GUI_FONT *__fontMap(MonkeyCore *core, MkFont mFont)
{
    GUI_FONT *font;

    switch (mFont)
    {
        case MK_FONT_0:
            font = core->mConfig.pFont0;
            break;

        case MK_FONT_1:
            font = core->mConfig.pFont1;
            break;

        default:
            font = core->mConfig.pFont0;
            break;
    }

    return font;
}

static void __restoreRegConfig(MonkeyCore *core)
{
    core->mFont = MK_FONT_0;
    core->mColor = MK_COLOR_BLACK;
    core->mSwitch = MK_SWITCH_NO;
    core->mRotate = MK_ROTATE_0;
    core->mBright = MK_BRIGHT_3;
    core->mBack   = MK_BACK_1;
    core->mAutoP  = MK_AS_0;
    core->defEncodeStyle = MK_CHAR_ENCODE_GBK;
}

static int __getFileLength(ES_FILE *fd)
{
    int len;
    eLIBs_fseek(fd, 0, SEEK_END);
    len = eLIBs_ftell(fd);
    return len;
}

/**********************************************************************************************************************/
/* 图层直接切换函数 */
static void  __lyrDirectSwc(H_LYR hllay1, H_LYR hllay2, __s32 width, __s32 direct)
{
    RECT    rect1;
    RECT    rect2;
    G_FILE *hdis = NULL;
    /* open display driver */
    hdis = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

    if (hdis == NULL)
    {
        return;
    }

    GUI_LyrWinGetScnWindow(hllay1, &rect1);
    GUI_LyrWinGetScnWindow(hllay2, &rect2);

    switch (direct)
    {
        case LAYER_SHIFT_DIRECT_UP    :
            rect1.y -= width;
            rect2.y -= width;
            break;

        case LAYER_SHIFT_DIRECT_DOWN  :
            rect1.y += width;
            rect2.y += width;
            break;

        case LAYER_SHIFT_DIRECT_LEFT  :
            rect1.x -= width;
            rect2.x -= width;
            break;

        case LAYER_SHIFT_DIRECT_RIGHT :
            rect1.x += width;
            rect2.x += width;
            break;

        default :
            return;
    }

    /* enable cache */
    esMODS_MIoctrl(hdis, DISP_CMD_START_CMD_CACHE, 0, NULL);
    GUI_LyrWinSetScnWindow(hllay1, &rect1);
    GUI_LyrWinSetScnWindow(hllay2, &rect2);
    /* disable cache */
    esMODS_MIoctrl(hdis, DISP_CMD_EXECUTE_CMD_AND_STOP_CACHE, 0, NULL);

    /* close display driver */
    if (hdis != NULL)
    {
        eLIBs_fclose(hdis);
        hdis = NULL;
    }
}

static void __lyrGlideSwc(H_LYR hllay1, H_LYR hllay2, __s32 width, __s32 direct)
{
    RECT    rect1;
    RECT    rect2;
    __s32   delt;
    __s32   cur = 0;
    __s32   first_advance = 5;
    __s32   diff = 4;
    __s32   end_curve[] = { 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    __s32   sum;
    __s32   dist;
    __s32   i;
    G_FILE *hdis = NULL;
    /* open display driver */
    hdis = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

    if (hdis == NULL)
    {
        return;
    }

    GUI_LyrWinGetScnWindow(hllay1, &rect1);
    GUI_LyrWinGetScnWindow(hllay2, &rect2);

    for (i = 0, sum = 0;  i < sizeof(end_curve) / sizeof(end_curve[0]);  i++)
    {
        sum += end_curve[i];
    }

    /* the first shift */
    for (delt = 0, dist = width - sum; delt < dist; delt += cur)
    {
        if (delt == 0)
        {
            cur = first_advance;
        }
        else
        {
            cur += diff;
        }

        if ((dist - delt - cur) <= end_curve[0])
        {
            cur += (dist - delt - cur);
        }

        switch (direct)
        {
            case LAYER_SHIFT_DIRECT_UP    :
                rect1.y -= cur;
                rect2.y -= cur;
                break;

            case LAYER_SHIFT_DIRECT_DOWN  :
                rect1.y += cur;
                rect2.y += cur;
                break;

            case LAYER_SHIFT_DIRECT_LEFT  :
                rect1.x -= cur;
                rect2.x -= cur;
                break;

            case LAYER_SHIFT_DIRECT_RIGHT :
                rect1.x += cur;
                rect2.x += cur;
                break;

            default :
                return;
        }

        /* enable cache */
        esMODS_MIoctrl(hdis, DISP_CMD_START_CMD_CACHE, 0, NULL);
        GUI_LyrWinSetScnWindow(hllay1, &rect1);
        GUI_LyrWinSetScnWindow(hllay2, &rect2);
        /* disable cache */
        esMODS_MIoctrl(hdis, DISP_CMD_EXECUTE_CMD_AND_STOP_CACHE, 0, NULL);
    }

    /* the last fhift */
    for (i = 0, sum = 0;  i < sizeof(end_curve) / sizeof(end_curve[0]);  i++)
    {
        cur = end_curve[i];

        switch (direct)
        {
            case LAYER_SHIFT_DIRECT_UP    :
                rect1.y -= cur;
                rect2.y -= cur;
                break;

            case LAYER_SHIFT_DIRECT_DOWN  :
                rect1.y += cur;
                rect2.y += cur;
                break;

            case LAYER_SHIFT_DIRECT_LEFT  :
                rect1.x -= cur;
                rect2.x -= cur;
                break;

            case LAYER_SHIFT_DIRECT_RIGHT :
                rect1.x += cur;
                rect2.x += cur;
                break;

            default :
                return;
        }

        /* enable cache */
        esMODS_MIoctrl(hdis, DISP_CMD_START_CMD_CACHE, 0, NULL);
        GUI_LyrWinSetScnWindow(hllay1, &rect1);
        GUI_LyrWinSetScnWindow(hllay2, &rect2);
        /* disable cache */
        esMODS_MIoctrl(hdis, DISP_CMD_EXECUTE_CMD_AND_STOP_CACHE, 0, NULL);
    }

    /* close display driver */
    if (hdis != NULL)
    {
        eLIBs_fclose(hdis);
        hdis = NULL;
    }
}


/* 图层切换显示 */
static void __lyrSwitchNext(MonkeyCore *core)
{
    if (MK_SWITCH_NO == core->mSwitch)
    {
        if (core->mRotate == MK_ROTATE_0)
        {
            __lyrDirectSwc(core->mConfig.hlyr_1, core->mConfig.hlyr_2, core->mConfig.sHeight, LAYER_SHIFT_DIRECT_UP);
        }
        else if (core->mRotate == MK_ROTATE_90)
        {
            __lyrDirectSwc(core->mConfig.hlyr_1, core->mConfig.hlyr_2, core->mConfig.sWidth, LAYER_SHIFT_DIRECT_LEFT);
        }
        else if (core->mRotate == MK_ROTATE_180)
        {
            __lyrDirectSwc(core->mConfig.hlyr_1, core->mConfig.hlyr_2, core->mConfig.sHeight, LAYER_SHIFT_DIRECT_DOWN);
        }
        else
        {
            __lyrDirectSwc(core->mConfig.hlyr_1, core->mConfig.hlyr_2, core->mConfig.sWidth, LAYER_SHIFT_DIRECT_RIGHT);
        }
    }
    else
    {
        if (core->mRotate == MK_ROTATE_0)
        {
            __lyrGlideSwc(core->mConfig.hlyr_1, core->mConfig.hlyr_2, core->mConfig.sHeight, LAYER_SHIFT_DIRECT_UP);
        }
        else if (core->mRotate == MK_ROTATE_90)
        {
            __lyrGlideSwc(core->mConfig.hlyr_1, core->mConfig.hlyr_2, core->mConfig.sWidth, LAYER_SHIFT_DIRECT_LEFT);
        }
        else if (core->mRotate == MK_ROTATE_180)
        {
            __lyrGlideSwc(core->mConfig.hlyr_1, core->mConfig.hlyr_2, core->mConfig.sHeight, LAYER_SHIFT_DIRECT_DOWN);
        }
        else
        {
            __lyrGlideSwc(core->mConfig.hlyr_1, core->mConfig.hlyr_2, core->mConfig.sWidth, LAYER_SHIFT_DIRECT_RIGHT);
        }
    }
}

static void __lyrSwitchPrev(MonkeyCore *core)
{
    if (MK_SWITCH_NO == core->mSwitch)
    {
        if (core->mRotate == MK_ROTATE_0)
        {
            __lyrDirectSwc(core->mConfig.hlyr_1, core->mConfig.hlyr_2, core->mConfig.sHeight, LAYER_SHIFT_DIRECT_DOWN);
        }
        else if (core->mRotate == MK_ROTATE_90)
        {
            __lyrDirectSwc(core->mConfig.hlyr_1, core->mConfig.hlyr_2, core->mConfig.sWidth,  LAYER_SHIFT_DIRECT_RIGHT);
        }
        else if (core->mRotate == MK_ROTATE_180)
        {
            __lyrDirectSwc(core->mConfig.hlyr_1, core->mConfig.hlyr_2, core->mConfig.sHeight, LAYER_SHIFT_DIRECT_UP);
        }
        else
        {
            __lyrDirectSwc(core->mConfig.hlyr_1, core->mConfig.hlyr_2, core->mConfig.sWidth, LAYER_SHIFT_DIRECT_LEFT);
        }
    }
    else
    {
        if (core->mRotate == MK_ROTATE_0)
        {
            __lyrGlideSwc(core->mConfig.hlyr_1, core->mConfig.hlyr_2, core->mConfig.sHeight, LAYER_SHIFT_DIRECT_DOWN);
        }
        else if (core->mRotate == MK_ROTATE_90)
        {
            __lyrGlideSwc(core->mConfig.hlyr_1, core->mConfig.hlyr_2, core->mConfig.sWidth, LAYER_SHIFT_DIRECT_RIGHT);
        }
        else if (core->mRotate == MK_ROTATE_180)
        {
            __lyrGlideSwc(core->mConfig.hlyr_1, core->mConfig.hlyr_2, core->mConfig.sHeight, LAYER_SHIFT_DIRECT_UP);
        }
        else
        {
            __lyrGlideSwc(core->mConfig.hlyr_1, core->mConfig.hlyr_2, core->mConfig.sWidth, LAYER_SHIFT_DIRECT_LEFT);
        }
    }
}

static void __selectLCDContext(MonkeyCore *core)
{
    if (core->mRotate == MK_ROTATE_0)
    {
        GUI_RECT rect;
        GUI_SetLCDSize(core->mConfig.contentLyrWidth, core->mConfig.contentLyrHeight);
        GUI_SetLCDDirection(0);
        rect.x0 = 0;
        rect.y0 = 0;
        rect.x1 = core->mConfig.contentLyrWidth;
        rect.y1 = core->mConfig.contentLyrHeight;
        GUI_SetClipRect(&rect);
    }
    else if (core->mRotate == MK_ROTATE_90)
    {
        GUI_RECT rect;
        GUI_SetLCDSize(core->mConfig.contentLyrHeight, core->mConfig.contentLyrWidth);
        GUI_SetLCDDirection(1);
        rect.x0 = 0;
        rect.y0 = 0;
        rect.x1 = core->mConfig.contentLyrHeight;
        rect.y1 = core->mConfig.contentLyrWidth;
        GUI_SetClipRect(&rect);
    }
    else if (core->mRotate == MK_ROTATE_180)
    {
        GUI_RECT rect;
        GUI_SetLCDSize(core->mConfig.contentLyrWidth, core->mConfig.contentLyrHeight);
        GUI_SetLCDDirection(2);
        rect.x0 = 0;
        rect.y0 = 0;
        rect.x1 = core->mConfig.contentLyrWidth;
        rect.y1 = core->mConfig.contentLyrHeight;
        GUI_SetClipRect(&rect);
    }
    else if (core->mRotate == MK_ROTATE_270)
    {
        GUI_RECT rect;
        GUI_SetLCDSize(core->mConfig.contentLyrHeight, core->mConfig.contentLyrWidth);
        GUI_SetLCDDirection(3);
        rect.x0 = 0;
        rect.y0 = 0;
        rect.x1 = core->mConfig.contentLyrHeight;
        rect.y1 = core->mConfig.contentLyrWidth;
        GUI_SetClipRect(&rect);
    }
    else
    {
        __err(" rotate type error \n");
    }
}

static void __showOnePage(H_MKC mCore, MkLine *line)
{
    int i;
    int x, y;
    MonkeyCore *core = (MonkeyCore *)mCore;
    MkLine  *tmp;
    __msg("page start = %d, page end = %d \n", core->pageStart, core->pageEnd);
    g_memset(core->buff, 0, MK_PAGE_CHAR * sizeof(char));
    eLIBs_fseek(core->fd, core->pageStart, ELIBS_SEEK_SET);
    eLIBs_fread(core->buff, sizeof(char), core->pageEnd - core->pageStart + 1, core->fd);
    x = 0;
    y = 0;

    for (i = 0; i < core->lineNum; i++)
    {
        tmp = line + i;

        //__msg("start = %d, len = %d \n", tmp->start, tmp->len);
        if (tmp != NULL)
        {
            pagewrap_displayStr(core->content, core->buff + tmp->start - core->pageStart, tmp->len, x, y);
            y += core->mConfig.lineofspace + core->fontHeight;
        }
    }
}

static void __showPageStart(MonkeyCore *core, MkLine *line)
{
    if (EPDK_TRUE == core->lyrFlag)
    {
        GUI_LyrWinSel(core->mConfig.hlyr_1);
    }
    else
    {
        GUI_LyrWinSel(core->mConfig.hlyr_2);
    }

    __selectLCDContext(core);
    GUI_SetTextMode(GUI_TM_TRANS);
    GUI_SetFont(core->font);
    GUI_SetFontMode(GUI_FONTMODE_8BPP256);
    GUI_SetBkColor(0);
    GUI_Clear();
    __showOnePage((H_MKC)core, line);
    GUI_LyrWinCacheOn();

    if (EPDK_TRUE == core->lyrFlag)
    {
        GUI_LyrWinSetSta(core->mConfig.hlyr_2, GUI_LYRWIN_STA_SLEEP);
        GUI_LyrWinSetSta(core->mConfig.hlyr_1, GUI_LYRWIN_STA_ON);
    }
    else
    {
        GUI_LyrWinSetSta(core->mConfig.hlyr_1, GUI_LYRWIN_STA_SLEEP);
        GUI_LyrWinSetSta(core->mConfig.hlyr_2, GUI_LYRWIN_STA_ON);
    }

    GUI_LyrWinCacheOff();
}

static void __showPageNext(MonkeyCore *core, MkLine *line)
{
    RECT rect;

    /* 调整图层位置 */
    if (EPDK_TRUE == core->lyrFlag)
    {
        GUI_LyrWinGetScnWindow(core->mConfig.hlyr_1, &rect);
    }
    else
    {
        GUI_LyrWinGetScnWindow(core->mConfig.hlyr_2, &rect);
    }

    if (core->mRotate == MK_ROTATE_0)
    {
        rect.y += core->mConfig.sHeight;
    }
    else if (core->mRotate == MK_ROTATE_90)
    {
        rect.x += core->mConfig.sWidth;
    }
    else if (core->mRotate == MK_ROTATE_180)
    {
        rect.y -= core->mConfig.sHeight;
    }
    else    //270
    {
        rect.x -= core->mConfig.sWidth;
    }

    if (EPDK_TRUE == core->lyrFlag)
    {
        GUI_LyrWinSetScnWindow(core->mConfig.hlyr_2, &rect);
        GUI_LyrWinSetSta(core->mConfig.hlyr_2, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSel(core->mConfig.hlyr_2);
    }
    else
    {
        GUI_LyrWinSetScnWindow(core->mConfig.hlyr_1, &rect);
        GUI_LyrWinSetSta(core->mConfig.hlyr_1, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSel(core->mConfig.hlyr_1);
    }

    /* 调整 gui lcd context */
    __selectLCDContext(core);
    /* 写内容到图层上去*/
    GUI_SetTextMode(GUI_TM_TRANS);
    GUI_SetFont(core->font);
    GUI_SetFontMode(GUI_FONTMODE_8BPP256);
    GUI_SetBkColor(0);
    GUI_Clear();
    __showOnePage((H_MKC)core, line);
    __lyrSwitchNext(core);

    /*  图层切换 */
    if (EPDK_TRUE == core->lyrFlag)
    {
        GUI_LyrWinCacheOn();
        GUI_LyrWinSetSta(core->mConfig.hlyr_1, GUI_LYRWIN_STA_SLEEP);
        GUI_LyrWinSetSta(core->mConfig.hlyr_2, GUI_LYRWIN_STA_ON);
        GUI_LyrWinCacheOff();
    }
    else
    {
        GUI_LyrWinCacheOn();
        GUI_LyrWinSetSta(core->mConfig.hlyr_2, GUI_LYRWIN_STA_SLEEP);
        GUI_LyrWinSetSta(core->mConfig.hlyr_1, GUI_LYRWIN_STA_ON);
        GUI_LyrWinCacheOff();
    }

    core->lyrFlag = !core->lyrFlag;
}

static void __showPagePrev(MonkeyCore *core, MkLine *line)
{
    RECT rect;

    /* 调整图层位置 */
    if (EPDK_TRUE == core->lyrFlag)
    {
        GUI_LyrWinGetScnWindow(core->mConfig.hlyr_1, &rect);
    }
    else
    {
        GUI_LyrWinGetScnWindow(core->mConfig.hlyr_2, &rect);
    }

    if (core->mRotate == MK_ROTATE_0)
    {
        rect.y -= core->mConfig.sHeight;
    }
    else if (core->mRotate == MK_ROTATE_90)
    {
        rect.x -= core->mConfig.sWidth;
    }
    else if (core->mRotate == MK_ROTATE_180)
    {
        rect.y += core->mConfig.sHeight;
    }
    else    //270
    {
        rect.x += core->mConfig.sWidth;
    }

    if (EPDK_TRUE == core->lyrFlag)
    {
        GUI_LyrWinSetScnWindow(core->mConfig.hlyr_2, &rect);
        GUI_LyrWinSetSta(core->mConfig.hlyr_2, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSel(core->mConfig.hlyr_2);
    }
    else
    {
        GUI_LyrWinSetScnWindow(core->mConfig.hlyr_1, &rect);
        GUI_LyrWinSetSta(core->mConfig.hlyr_1, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSel(core->mConfig.hlyr_1);
    }

    /* 调整 gui lcd context */
    __selectLCDContext(core);
    /* 写内容到图层上去*/
    GUI_SetTextMode(GUI_TM_TRANS);
    GUI_SetFont(core->font);
    GUI_SetFontMode(GUI_FONTMODE_8BPP256);
    GUI_SetBkColor(0);
    GUI_Clear();
    __showOnePage((H_MKC)core, line);
    __lyrSwitchPrev(core);

    /*  图层切换 */
    if (EPDK_TRUE == core->lyrFlag)
    {
        GUI_LyrWinCacheOn();
        GUI_LyrWinSetSta(core->mConfig.hlyr_1, GUI_LYRWIN_STA_SLEEP);
        GUI_LyrWinSetSta(core->mConfig.hlyr_2, GUI_LYRWIN_STA_ON);
        GUI_LyrWinCacheOff();
    }
    else
    {
        GUI_LyrWinCacheOn();
        GUI_LyrWinSetSta(core->mConfig.hlyr_2, GUI_LYRWIN_STA_SLEEP);
        GUI_LyrWinSetSta(core->mConfig.hlyr_1, GUI_LYRWIN_STA_ON);
        GUI_LyrWinCacheOff();
    }

    core->lyrFlag = !core->lyrFlag;
}

//static void __showPageSeek(MonkeyCore *core, MkLine *line)
//{
//  RECT rect;
//
//  if( EPDK_TRUE == core->lyrFlag )
//      GUI_LyrWinSel(core->mConfig.hlyr_2);
//  else
//      GUI_LyrWinSel(core->mConfig.hlyr_1);
//
//  __selectLCDContext(core);
//
//  GUI_SetTextMode(GUI_TM_TRANS);
//  GUI_SetFont(core->font);
//  GUI_SetFontMode(GUI_FONTMODE_8BPP256);
//  GUI_SetBkColor(0);
//  GUI_Clear();
//  _showOnePage((H_MKC)core, line);
//
//  GUI_LyrWinCacheOn();
//  if( EPDK_TRUE == core->lyrFlag )
//  {
//      GUI_LyrWinGetScnWindow( core->mConfig.hlyr_1, &rect );
//      GUI_LyrWinSetScnWindow( core->mConfig.hlyr_2, &rect );
//  }
//  else
//  {
//      GUI_LyrWinGetScnWindow( core->mConfig.hlyr_2, &rect );
//      GUI_LyrWinSetScnWindow( core->mConfig.hlyr_1, &rect );
//  }
//  GUI_LyrWinCacheOff();
//
//  core->lyrFlag = !core->lyrFlag;
//}

/**********************************************************************************************************************/

/***********************************************************************************************************************
*Name        : mkc_open
*Prototype   : H_MKC mkc_open(void)
*Arguments   : void
*Return      : monkey core handle, we will use this handle in other interface;
*Description : open now play list module , bookmark module, register module, and init monkey core the control struct
*Other       :
***********************************************************************************************************************/
H_MKC mkc_open(void)
{
    MonkeyCore *core;
    __s32       index;
    /* create monkey core handle */
    //core = (MonkeyCore *)g_malloc(sizeof(MonkeyCore));
    core = (MonkeyCore *)esMEMS_Palloc(1 + sizeof(MonkeyCore) / 1024, 0);

    if (!core)
    {
        __err(" MonkeyCore palloc error \n");
        return NULL;
    }

    g_memset(core, 0, sizeof(MonkeyCore));
    /* int now play list module  */
    core->npl = mnpl_create(ORC_MEDIA_TYPE_EBOOK);
    /******************for test purpose **************************/
    //  mnpl_clear(core->npl);
    //  mnpl_addItem(core->npl, TEST_FILE);
    //  mnpl_playingSeek(core->npl, ORC_NPL_SEEK_SET , 0);
    /*************************************************************/
    index     = mnpl_getCur(core->npl);
    mnpl_indexToFile(core->npl, index, core->filename);
    core->fd  = g_fopen(core->filename, "rb");

    if (NULL == core->fd)
    {
        __wrn(" open file error \n");
        return (H_MKC)core;
    }

    __msg("txt file name : %s \n", core->filename);
    core->len = __getFileLength(core->fd);
    /* init bookmark module */
    __here__
    core->bmk   = bmk_open(core->filename, 0);
    __here__
    core->exist = bmk_get_last_offset(core->bmk, &(core->lastOffset));
    /* init regedit module */
    __here__
    core->mFont = mkr_getFont();
    core->mColor = mkr_getColor();
    core->mSwitch = mkr_getSwitch();
    core->mRotate = mkr_getRotate();
    core->mBright = mkr_getBright();
    core->mBack  = mkr_getBackground();
    core->mAutoP = mkr_getAutoPage();
    core->defEncodeStyle = MK_CHAR_ENCODE_GBK;
    __here__
    // create pallet
    core->pPal       = (__u32 *)esMEMS_Malloc(0, 1024);
    return (H_MKC)core;
}

/***********************************************************************************************************************
*Name        : mkc_showConfig
*Prototype   : __s32  mkc_showConfig(H_MKC mCore, MkcConfig *para)
*Arguments   : para : monkey core show gui res, such as font , layer, the show scene size.
*Return      :
*Description : this interface must be called, before mkc_showStart, mkc_showNext, mkc_showPrev, mkc_showSeek
*Other       :
***********************************************************************************************************************/
__s32  mkc_showConfig(H_MKC mCore, MkcConfig *para)
{
    PageConfig  pConfig;
    MonkeyCore  *core = (MonkeyCore *)mCore;
    /* init MonkeyCore struct */
    g_memcpy(&(core->mConfig), para, sizeof(MkcConfig));
    core->font  = __fontMap(core, core->mFont);
    core->color = __charColorMap(core->mColor);
    GUI_LyrWinSel(para->hlyr_1);
    GUI_SetFont(core->font);
    core->fontHeight = GUI_GetFontDistY();
    core->lineNum    = para->yScrHeight / (para->lineofspace + core->fontHeight);
    core->lyrFlag    = EPDK_TRUE;
    /* set the pallet */
    __changPalette(core, core->color);
    /* init page wrap module */
    pConfig.fd          = core->fd;
    pConfig.hlyr        = para->hlyr_1;
    pConfig.pFont       = core->font;
    pConfig.lineWidth   = para->xScrWidth;
    pConfig.lineNum     = core->lineNum;
    pConfig.defEncodeStyle = core->defEncodeStyle;

    if (!core->content)
    {
        if (EPDK_TRUE == core->exist)
        {
            pConfig.startOffset = core->lastOffset;
        }
        else
        {
            pConfig.startOffset = 0;
        }

        core->content       = pagewrap_create(&pConfig);
    }
    else
    {
        pConfig.startOffset = core->pageStart;
        pagewrap_setting(core->content, &pConfig);
    }

    g_memcpy(&(core->pConfig), &pConfig, sizeof(PageConfig));
    return EPDK_OK;
}

/***********************************************************************************************************************
*Name        : mkc_showStart
*Prototype   : __s32 mkc_showStart(H_MKC mCore)
*Arguments   : mCore : MonkeyCore handle
*Return      :
*Description : show the start page, every time we chang register config module or gui res para, wu should call this
               interface to show first page, notice the different beturn mkc_showSeek
*Other       :
***********************************************************************************************************************/
__s32 mkc_showStart(H_MKC mCore)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    MkLine     *line;
    __s32       ret;
    __u32       rOffset;
    __u64       tmp;
    ret = pagewrap_start(core->content);

    if (ret != -1)
    {
        core->pageStart = pagewrap_getStartOffset(core->content);
        core->pageEnd   = pagewrap_getEndOffset(core->content);
        line            = pagewrap_getLine(core->content);
        __showPageStart(core, line);
        tmp         = ((__u64)(core->pageStart)) * 10000;
        rOffset     = (__u32)(tmp / core->len);
        return rOffset;
    }

    return -1;
}

/***********************************************************************************************************************
*Name        : mkc_showNext
*Prototype   : __s32 mkc_showNext(H_MKC mCore)
*Arguments   : mCore : MonkeyCore handle
*Return      :
*Description : show the next content
*Other       :
***********************************************************************************************************************/
__s32 mkc_showNext(H_MKC mCore)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    MkLine *line;
    __s32   ret;
    __u32       rOffset;
    __u64       tmp;
    ret = pagewrap_next(core->content);

    if (ret != -1)
    {
        core->pageStart = pagewrap_getStartOffset(core->content);
        core->pageEnd   = pagewrap_getEndOffset(core->content);
        line            = pagewrap_getLine(core->content);
        __showPageNext(core, line);
        tmp         = ((__u64)(core->pageStart)) * 10000;
        rOffset     = (__u32)(tmp / core->len);
        return rOffset;
    }

    return -1;
}

/***********************************************************************************************************************
*Name        : mkc_showNext
*Prototype   : __s32 mkc_showNext(H_MKC mCore)
*Arguments   : mCore : MonkeyCore handle
*Return      :
*Description : show the prev content
*Other       :
***********************************************************************************************************************/
__s32 mkc_showPrev(H_MKC mCore)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    MkLine *line;
    __s32  ret;
    __u32       rOffset;
    __u64       tmp;
    ret = pagewrap_prev(core->content);

    if (ret != -1)
    {
        core->pageStart = pagewrap_getStartOffset(core->content);
        core->pageEnd   = pagewrap_getEndOffset(core->content);
        line            = pagewrap_getLine(core->content);
        __showPagePrev(core, line);
        tmp         = ((__u64)(core->pageStart)) * 10000;
        rOffset     = (__u32)(tmp / core->len);
        return rOffset;
    }

    return -1;
}

/***********************************************************************************************************************
*Name        : mkc_showNext
*Prototype   : __s32 mkc_showSeek(H_MKC mCore, __s32 offset)
*Arguments   : mCore : MonkeyCore handle
*Return      : the true page start offset ( 1/1000 * txtleng )
*Description :
*Other       :
***********************************************************************************************************************/
__s32 mkc_showSeek(H_MKC mCore, __s32 offset)       /* offset : 相对全文偏移量比例，1/10000 为单位 */
{
    MonkeyCore  *core = (MonkeyCore *)mCore;
    MkLine      *line;
    __s32       ret;
    __u32       seekOffset;
    __u64       tmp;
    __u32       rOffset;
    tmp         = ((__u64)(core->len)) * offset;
    seekOffset  = (__u32)(tmp / 10000);
    __msg(" seekOffset = %d \n", seekOffset);
    ret = pagewrap_fseek(core->content, seekOffset);

    if (ret != -1)
    {
        core->pageStart = pagewrap_getStartOffset(core->content);
        core->pageEnd   = pagewrap_getEndOffset(core->content);
        line            = pagewrap_getLine(core->content);
        __showPageStart(core, line);
        tmp         = ((__u64)(core->pageStart)) * 10000;
        rOffset     = (__u32)(tmp / core->len);
        return rOffset;
    }

    return -1;
}

/**********************************************************************************************************************/
__s32 mkc_setFont(H_MKC mCore, MkFont mfont)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    GUI_FONT   *font;
    font                = __fontMap(core, mfont);
    core->mFont         = mfont;
    core->font          = font;
    GUI_LyrWinSel(core->mConfig.hlyr_1);
    GUI_SetFont(core->font);
    core->fontHeight = GUI_GetFontDistY();
    core->lineNum    = core->mConfig.yScrHeight / (core->mConfig.lineofspace + core->fontHeight);
    core->pConfig.pFont = font;
    core->pConfig.lineNum = core->lineNum;
    core->pConfig.startOffset = core->pageStart;
    pagewrap_setting(core->content, &(core->pConfig));
    mkc_showStart(mCore);
    return EPDK_OK;
}

__s32 mkc_setCharColor(H_MKC mCore, MkColor mcolor)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    core->mColor     = mcolor;
    core->color      = __charColorMap(mcolor);
    __changPalette(core, core->color);
    return EPDK_OK;
}

__s32 mkc_setRotate(H_MKC mCore, MkRotate rotate)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    core->mRotate = rotate;
    return EPDK_OK;
}

__s32 mkc_setSwitchMode(H_MKC mCore, MkSwitch mode)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    core->mSwitch = mode;
    return EPDK_OK;
}

__s32 mkc_setDefEncode(H_MKC mCore, MkEncode encode)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    core->defEncodeStyle = encode;
    core->pConfig.defEncodeStyle = encode;
    pagewrap_setting(core->content, &(core->pConfig));
    /* notice that : we call mkc_showStart, buf not interface : mkc_showSeek */
    mkc_showStart(mCore);
    return EPDK_OK;
}

__s32  mkc_setBright(H_MKC mCore, MkBright mBright)
{
    return EPDK_TRUE;
}

__s32  mkc_setAutoPage(H_MKC mCore, MkAutoP mAutoP)
{
    return EPDK_TRUE;
}

/* 保存背景图片类型到注册表中, 此接口没有真正改变背景图片 */
__s32 mkc_setBack(H_MKC mCore, MkBack mBack)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    core->mBack = mBack;
    return EPDK_OK;
}

/* 当前页的起始和结束位置的绝对偏移量*/
__s32 mkc_getCurPageOffset(H_MKC mCore, __s32 *start, __s32 *end)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    *start = core->pageStart;
    *end   = core->pageEnd;
    return EPDK_OK;
}

__bool mkc_isHead(H_MKC mCore)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    __bool      ret;
    ret = pagewrap_isHead(core->content);
    return ret;
}

__bool mkc_isTail(H_MKC mCore)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    __bool ret;
    ret = pagewrap_isTail(core->content);
    return ret;
}

/* 查询背景图片类型 */
MkBack mkc_getBack(H_MKC mCore)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    return core->mBack;
}

MkFont   mkc_getFont(H_MKC mCore)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    return core->mFont;
}

MkColor  mkc_getCharColor(H_MKC mCore)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    return core->mColor;
}

MkRotate mkc_getRotate(H_MKC mCore)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    return core->mRotate;
}

MkSwitch mkc_getSwitch(H_MKC mCore)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    return core->mSwitch;
}

MkEncode mkc_getDefEncode(H_MKC mCore)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    return core->defEncodeStyle;
}

MkBright mkc_getBright(H_MKC mCore)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    return core->mBright;
}

MkAutoP  mkc_getAutoPage(H_MKC mCore)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    return core->mAutoP;
}

__s32  mkc_getFileName(H_MKC mCore, char *fileName)     /* 查询当前打开文件名称*/
{
    char    *p;
    MonkeyCore *core = (MonkeyCore *)mCore;
    p = eLIBs_strchrlast(core->filename, '\\');
    eLIBs_strcpy(fileName, p + 1);
    return EPDK_OK;
}

__s32  mkc_openBmk(H_MKC mCore, MkBmk mBmk)             /* 跳转到书签指定位置 */
{
    int     offset;
    //__s32     ret;
    MonkeyCore *core = (MonkeyCore *)mCore;

    if (EPDK_TRUE == bmk_get_offset(core->bmk, (int)mBmk - 1, &offset))
    {
        core->pConfig.startOffset = offset;
        pagewrap_setting(core->content, &(core->pConfig));
        mkc_showStart(mCore);
    }

    return EPDK_OK;
}

__s32  mkc_saveBmk(H_MKC mCore, MkBmk mBmk)                 /* 保存当前偏移量为书签 */
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    bmk_set_offset(core->bmk, (int)mBmk - 1, core->pageStart);
    return EPDK_OK;
}

__s32  mkc_getBmkOffset(H_MKC mCore, MkBmk mBmk)            /* 查询指定书签偏移百分比, 偏移量单位: 万分之一 */
{
    int offset;
    MonkeyCore *core = (MonkeyCore *)mCore;

    if (EPDK_TRUE == bmk_get_offset(core->bmk, (int)mBmk - 1, &offset))
    {
        __u32       rOffset;
        __u64       tmp;
        tmp         = (__u64)(offset) * 10000;
        rOffset     = (__u32)(tmp / core->len);
        return rOffset;
    }
    else
    {
        return -1;      /* 书签没有保存 */
    }
}

/* 关闭句柄 */
__s32 mkc_close(H_MKC mCore)
{
    MonkeyCore *core = (MonkeyCore *)mCore;
    /* 保存注册标选项 */
    mkr_setFont(core->mFont);
    mkr_setColor(core->mColor);
    mkr_setSwitch(core->mSwitch);
    mkr_setRotate(core->mRotate);
    mkr_setBright(core->mBright);
    mkr_setBackground(core->mBack);
    mkr_setAutoPage(core->mAutoP);
    //core->defEncodeStyle = MK_CHAR_ENCODE_GBK;
    /* 保存bmk 选项  */
    bmk_set_last_offset(core->bmk, core->pageStart);
    bmk_close(core->bmk);
    /* 关闭 now play list */
    mnpl_delete(core->npl);
    /* 释放资源      */
    pagewrap_delete(core->content);
    /* 释放调色板 */
    esMEMS_Mfree(0, core->pPal);
    /* 关闭文件句柄 */
    g_fclose(core->fd);
    /* 释放句柄     */
    //g_free(core);
    esMEMS_Pfree(core, 1 + sizeof(MonkeyCore) / 1024);
    return EPDK_OK;
}
