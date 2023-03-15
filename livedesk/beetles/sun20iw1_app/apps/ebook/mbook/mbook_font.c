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
#include <log.h>
#include <kapi.h>
#include "mbook_private.h"

void *My_Malloc(void *heap, __u32 size);
void  My_Mfree(void *heap, void *f);

typedef struct tag_MBOOK_FONT
{
    __s32       font_size;
    __s32      *font_table;
    GUI_FONT   *char_font;
} __mbook_font_t;


/*
************************************************************************************************************************
*                                       MBOOK_Font_Init
*
*Description: 创建字体操作句柄
*
*Arguments  : path: 字库存放路径
*             size：字体的大小
*
*
*Return     : 成功：返回操作句柄
*             失败：NULL
*
************************************************************************************************************************
*/
H_FONT  MBOOK_Font_Init(GUI_FONT *ebook_font, __u32 size)
{
    __u32               i;
    __mbook_font_t     *p_font;
    // 申请内存
    p_font = (__mbook_font_t *)My_Malloc(0, sizeof(__mbook_font_t));

    if (p_font == NULL)
    {
        __err(" malloc error \n");
        return NULL;
    }

    p_font->char_font = ebook_font;
    // 申请字符字宽存放内存
    p_font->font_table = (__s32 *)My_Malloc(0, 1024);   // 申请内存

    if (p_font->font_table == NULL)
    {
        __err(" malloc error \n");
        goto FONT_ERROR_2;
    }

    // 设置
    GUI_SetFont(p_font->char_font);

    for (i = 0; i < 256; i++)
    {
        *(p_font->font_table + i) = GUI_GetCharDistX(i);
    }

    return p_font;
FONT_ERROR_2:
    // GUI_SetFont(&GUI_Font6x8);
    GUI_TTF_Done(p_font->char_font);
    p_font->char_font = NULL;
    //FONT_ERROR_1:
    My_Mfree(0, p_font);
    return NULL;
}

/*
************************************************************************************************************************
*                                       MBOOK_Font_Uninit
*
*Description: 释放字体操作句柄
*
*Arguments  : hdle：字体操作句柄
*
*
*Return     : 成功：EPDK_OK
*             失败：EPDK_FAIL
*
************************************************************************************************************************
*/
__s32   MBOOK_Font_Uninit(H_FONT hdle)
{
    __mbook_font_t     *p_font;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_font = (__mbook_font_t *)hdle;

    if (p_font->font_table)
    {
        My_Mfree(0, p_font->font_table);
        p_font->font_table = NULL;
    }

    My_Mfree(0, p_font);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       MBOOK_Font_GetTable
*
*Description: 获取字符的字宽表
*
*Arguments  : hdle：字体操作句柄
*
*
*Return     : 成功：字符字宽表
*             失败：NULL
*
************************************************************************************************************************
*/
__s32  *MBOOK_Font_GetTable(H_FONT hdle)
{
    __mbook_font_t     *p_font;

    if (hdle == NULL)
    {
        return NULL;
    }

    p_font = (__mbook_font_t *)hdle;
    return p_font->font_table;
}

/*
************************************************************************************************************************
*                                       MBOOK_Font_GetTable
*
*Description: 获取字库句柄
*
*Arguments  : hdle：字体操作句柄
*
*
*Return     : 成功：字库句柄
*             失败：NULL
*
************************************************************************************************************************
*/
GUI_FONT   *MBOOK_Font_GetFont(H_FONT hdle)
{
    __mbook_font_t     *p_font;

    if (hdle == NULL)
    {
        return NULL;
    }

    p_font = (__mbook_font_t *)hdle;
    return p_font->char_font;
}
