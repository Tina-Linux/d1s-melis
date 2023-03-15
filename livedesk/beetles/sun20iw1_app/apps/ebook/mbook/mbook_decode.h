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
#ifndef __MBOOK_DECODE_H__
#define __MBOOK_DECODE_H__

#define     DECODE_MBOOK_NUM     3

typedef     void   *H_BMK;
typedef     void   *H_DECODE;

typedef struct tag_DECODE_CONFIG
{
    GUI_FONT  *char_font;
    __u32       font_size;              // 字体大小
    __u32       border_width;           // 两边留白宽度
    __u32       bottom_width;           // 底边留白宽度
    __u32       scroll_width;           //滑动条的宽度
    __u32       show_width;             // 显示的宽度
    __u32       show_height;            // 显示的高度
    __u32       row_space;              // 行间距
    __u32       col_space;              // 列间距
    __u32       font_color;             // 字体的颜色
    __u32       back_color;             // 背景的颜色
    __u32       show_rotate;            // 横竖屏显示
} __decode_config_t;


#define     MAX_FILE_NAME       1024


typedef enum tag__DECODE_MODE
{
    DECODE_NULL = 0,                                /* 不解码任何页         */
    DECODE_CURR,                                    /* 解码当前页           */
    DECODE_PREV,                                    /* 解码当前页的前面页   */
    DECODE_NEXT,                                    /* 解码所有页的后面页   */
} __decode_mode_e;

typedef struct tag_MBOOK_CACHE               // 页缓冲
{
    __s32       page_sta;                   // 页面解码成功标示：1 成功、 0 失败
    __s32       page_no;                    // 缓存里的页面号
    H_WIN       page_lyr;                   // 缓存页对应的软图层
} __page_cache_t;


typedef struct tag_MBOOK_DECODE
{
    char                    file_path[MAX_FILE_NAME];       // 文件路径
    ES_FILE                *file_hdle;                      // 文件句柄
    void              *analysis_hdle;                  // 文件分析句柄
    void                   *show_hdle;                      // 页面显示句柄

    void                   *font_hdle;                      // 字体操作句柄

    __s64                   file_size;                      // 文件大小
    __epdk_charset_enm_e    default_charset;                // 默认编码格式
    GUI_FONT               *char_font;                      // 字库句柄
    __s32                  *char_table;                     // 字符字宽表

    H_WIN                   show_lyr;                       // 显示图层
    __page_cache_t          page[DECODE_MBOOK_NUM];          // 页面缓冲器
    __u32                   data_len;                       // 一页数据的大小
    __u8                   *page_data;                      // 一页数据存放地址

    __u8                   *curt_page_data;              //当前页数据存放地址，为tts服务

    __s32                   total_page;                     // 文件总页数
    __s32                   current_page;                   // 当前显示页号

    __s32                   move_state;                     // 页面移动控制标示
    __s32                   move_height;                    // 页面移动高度

    __u32                   decode_tsk;                     // 预解线程
    __s32                   decode_cnt;                     // 预解页循环计数
    __krnl_event_t         *decode_sem;                     // 预解控制信号量
    __decode_mode_e         decode_sta;                     // 预解模式

    __decode_config_t       config;                         // 配置信息
    __epdk_charset_enm_e    charset;                        // 文件编码
} __mbook_decode_t;






H_DECODE   MBOOK_Decode_Init(char *filename, __u8 *err);

__s32   MBOOK_Decode_Config(H_DECODE hdle, __decode_config_t *config);

__s32   MBOOK_Decode_ShowPage(H_DECODE hdle, __u32 offset);

__s32   MBOOK_Decode_ShowNext(H_DECODE hdle);

__s32   MBOOK_Decode_ShowPrev(H_DECODE hdle);

__s32   MBOOK_Decode_SeekPage(H_DECODE hdle, __u32 page_no);

__s32   MBOOK_Decode_SeekOffset(H_DECODE hdle, __u32 offset);

__s32   MBOOK_Decode_MovePage(H_DECODE hdle, __s32 x, __s32 y);

__s32   MBOOK_Decode_RotatePage(H_DECODE hdle);

__s32   MBOOK_Decode_GetRotate(H_DECODE hdle);

__s32   MBOOK_Decode_GetTotalPage(H_DECODE hdle);

__s32   MBOOK_Decode_GetCurrentPage(H_DECODE hdle);

__s32   MBOOK_Decode_GetPageOffset(H_DECODE hdle, __u32 page_no);

__s32   MBOOK_Decode_GetFileName(H_DECODE hdle, char *name);

__s64   MBOOK_Decode_GetFileSize(H_DECODE hdle);

__s32   MBOOK_Decode_GetFilePath(H_DECODE hdle, char *path);

__s32   MBOOK_Decode_GetFileContext(H_DECODE hdle, char *context);

__s32   MBOOK_Decode_SetRowSpace(H_DECODE hdle, __u32 space);

__s32   MBOOK_Decode_SetColSpace(H_DECODE hdle, __u32 space);

__s32   MBOOK_Decode_SetBorderWidth(H_DECODE hdle, __u32 width);

__s32   MBOOK_Decode_SetBackColor(H_DECODE hdle, __u32 color);

__s32   MBOOK_Decode_SetFontColor(H_DECODE hdle, __u32 color);

__s32   MBOOK_Decode_SetFontSize(H_DECODE hdle, __u32 size);

__s32   MBOOK_Decode_SetDefaultCharset(H_DECODE hdle, __epdk_charset_enm_e charset);

__s32   MBOOK_Decode_Uninit(H_DECODE hdle);

void MBOOK_TTS_court_page(H_DECODE hdle, char  **tts_curt_page_data, __s32 *char_num);

__epdk_charset_enm_e EBookGetSysDefaultCharset(void);

__s32   BOOK_Mark_SetPage(H_BMK hdle, __u32 num, __u32 offset, char *context);
__s32   MBOOK_Decode_GetPage(H_DECODE hdle, __u32 page_offset);

__s32   BOOK_Mark_GetPageOffset(H_BMK hdle, __u32 num);
__s32   MBOOK_Decode_GetPage(H_DECODE hdle, __u32 page_offset);
__s32   BOOK_Mark_Close(H_BMK hdle);
__s32   BOOK_Mark_Uninit(H_BMK hdle);
__s32   BOOK_Mark_Open(H_BMK hdle, char *file);
H_BMK   BOOK_Mark_Init(char *mark_file);
__s32  MBOOK_Decode_get_filename(char *filename);
__s32   MBOOK_Show_LastPage(H_DECODE hdle, __u32 page_no);








#endif /* __MBOOK_DECODE_H__ */
