/*
*******************************************************************************************************************
*                                                           display driver
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Allwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º    static.h
*
*Author£º       William Wang
*
*Version :      1.1.0
*
*Date:          2009-9-4
*
*Description :  static function and variable definition
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>      <version>     <description>
*
* William Wang     2009-9-4      1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __STATIC_H__
#define __STATIC_H__

#define CTRL_STATIC             ("static")

#define STN_DBLCLK               1

#define STN_ENABLE               2
#define STN_DISABLE              3

#define STN_CLICKED              4
#define STN_PUSHED               5
#define STN_DBCLICKED            6

#define STATICST_FOCUS           (1<<2)

typedef struct tagstnpara
{
    void                        *focus_bmp;
    void                        *unfocus_bmp;
    __pos_t                     bmp_pos;
    char                        *text;
    __pos_t                     text_pos;
    __u32                       ftxt_color;
    __u32                       uftxt_color;
    GUI_FONT                   *draw_font;
    __epdk_charset_enm_e        draw_code;   /*×Ö·û±àÂë*/
    __u32                       txt_align;
    __u32                       bmp_align;
    __u32                       bk_color;
    __bool                      alpha_en;
} static_para_t;

typedef struct tagSTATICDATA
{
    GUI_MEMDEV_Handle   hmem;
    __u32               draw_sts;
    __u32               status;           /* static flags */
    unsigned long               data;             /* bitmap or icon of butotn. */
    unsigned long               str_move;
} static_data_t;

typedef static_data_t *pstaticdata;

#define STATIC_DRAWSTATUS(pCtrl) (((pstaticdata)GUI_CtrlWinGetAddData(pCtrl))->draw_sts)
#define STATIC_STATUS(pCtrl)   (((pstaticdata)GUI_CtrlWinGetAddData(pCtrl))->status)
//#define BUTTON_STSTRANS(pCtrl)  (BUTTON_OLDSTATUS(pCtrl) != BUTTON_STATUS(pCtrl))
#define STATIC_DATA(pCtrl)     (((pstaticdata)GUI_CtrlWinGetAddData(pCtrl))->data)

__bool RegisterStaticControl(void);
//__s32  GUI_StaticSetTxt               (H_WIN hstatic,char *text);
__s32  GUI_StaticSetFocusBmp(H_WIN hbutton, void *bmp);
__s32  GUI_StaticSetUnFocusBmp(H_WIN hbutton, void *bmp);
__s32  GUI_StaticSetText(H_WIN hbutton, char *text);
__s32  GUI_StaticSetUnFcsTxtColor(H_WIN hbutton, __u32 color);
__s32  GUI_StaticSetFcsTxtColor(H_WIN hbutton, __u32  color);
__s32  GUI_StaticSetBmpPos(H_WIN hbutton, __pos_t *pos);
__s32  GUI_StaticSetTxtPos(H_WIN hbutton, __pos_t *pos);
__s32  GUI_StaticStartMove(H_WIN hstatic);
__s32  GUI_StaticStopMove(H_WIN hstatic);
__s32  GUI_StaticSetCharSet(H_WIN hstatic, __epdk_charset_enm_e draw_code);
#endif
