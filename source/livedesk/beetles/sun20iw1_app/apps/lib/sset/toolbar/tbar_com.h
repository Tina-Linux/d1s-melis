/*
************************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : tbar_com.c
* By        : Andy.zhang
* Func      : toolbar component
* Version   : v1.0
* ============================================================================================================
* 2009-10-15 15:27:26 andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
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


