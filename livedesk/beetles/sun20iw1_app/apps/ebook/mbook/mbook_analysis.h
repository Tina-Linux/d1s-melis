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
#ifndef __MBOOK_ANALYSIS_H__
#define __MBOOK_ANALYSIS_H__

#define     MAX_SHOW_PAGE       100000

typedef     void   *H_ANALYSIS;

typedef struct tag_ONE_PAGE     // 页：12 byte
{
    __u32       page_no;        //页号
    __u32       page_start;     //起始位置 文件偏移量
    __u32       page_end;       //结束位置 文件偏移量
} __one_page_t;

typedef struct tag_INFO_CONFIG
{
    __u32       show_width;     // 文本显示的宽度
    __u32       page_row;       // 一页可显示的行数
    __u32       col_width;      // 列与列间的宽度
    __s32      *char_table;     // 字符字宽表

    __epdk_charset_enm_e    charset;
} __analysis_config_t;


H_ANALYSIS  MBOOK_Analysis_Init(ES_FILE *fd);

__s32   MBOOK_Analysis_Config(H_ANALYSIS hdle, __analysis_config_t *config);

__s32   MBOOK_Analysis_Work(H_ANALYSIS hdle);

__s32   MBOOK_Analysis_GetInfo(H_ANALYSIS hdle, __u32 page_no, __one_page_t *page_info);

__s32   MBOOK_Analysis_GetPage(H_ANALYSIS hdle, __u32 offset);

__s32   MBOOK_Analysis_GetTotalPage(H_ANALYSIS hdle);

__s32   MBOOK_Analysis_Uninit(H_ANALYSIS hdle);


#endif /* __MBOOK_ANALYSIS_H__ */
