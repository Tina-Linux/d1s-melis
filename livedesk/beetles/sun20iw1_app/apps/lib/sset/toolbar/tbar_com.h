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
#ifndef __TBAR_COM_H__
#define __TBAR_COM_H__

#define     ID_TOOL_BAR     3030

////////////
#define DECLARE_BMP_RES_Pt(_name)   \
    void* _name; \
    HTHEME _name ## _h
//
// #define Create_BMP_RES_Pt(_p, _id)   do {_p ## _h = dsk_theme_open(_id); _p = dsk_theme_hdl2buf(_p ## _h);}while(0)
//
// #define Destroy_BMP_RES_Pt(_p)       dsk_theme_close(_p ## _h)
/////////////

/* toolbar 通知码   */
#define     TB_PUSHED       0
#define     TB_SWITCH       1
#define     TB_UNPUSHED     2
#define     TB_ESCAPE       3

/* toolbar 设置码   */
#define     TB_SET_UPDATE_LANGUAGE  (GUI_MSG_USER_DEF+1)

/**
 * toolbar create para
 */
typedef struct tag_tbar_item
{
    //  void        *focus_bmp;             // 焦点图片
    DECLARE_BMP_RES_Pt(focus_bmp);
    //  void        *unfocus_bmp;           // 非焦点图片
    DECLARE_BMP_RES_Pt(unfocus_bmp);
    char        text[GUI_TITLE_MAX];    // 标题内容

    __s32           type;
} tbar_item_t;

typedef struct tag_tbar_para
{
    tbar_item_t *item;              // item info
    int         item_nr;            // item 数目

    int         focus_item;         // 焦点 item index
    SIZE        frame_size;         // 矩形框大小
    __pos_t     bmp_pos;            // 图片位置(相对矩形框)
    __pos_t     text_pos;           // 文本位置(相对矩形框)
    int         scn_width;          // 屏幕宽度

    __u32       focus_color;        // 标题focus color
    __u32       unfocus_color;      // 标题unfocus color
    GUI_FONT    *draw_font;         // font

    int         src_start;          // 第一个item开始显示位置
    void       *bg_bmp;             //背景图片
    HTHEME      h_bgbmp;             //背景图片
    __u16       h_ssetid;               //所属窗口判断标志
} tbar_para_t;

H_WIN tbar_frm_create(H_WIN parent, H_WIN layer, tbar_para_t *tb_para);

__s32 tbar_frm_delete(H_WIN frmwin);

__epdk_charset_enm_e get_language_charset(void);


#endif /* __TBAR_COM_H__ */
