/*
*******************************************************************************************************************
*                                                           Module Orange
*                                                   the embedded Graphical module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    gmsgbox.h
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2009-10-19
*
*Description :  global message box definition
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>          <version>     <description>
*
* William Wang     2009-10-19         1.1.0          Create File
*
*******************************************************************************************************************
*/

#ifndef _GUI_GMSGBOX_H_
#define _GUI_GMSGBOX_H_

typedef struct taggmsgboxpara
{
    void                        *bk_bmp;
    char                        *text;
    __pos_t                     text_pos;
    __u32                       txt_color;
    GUI_FONT                   *draw_font;
    __epdk_charset_enm_e        draw_code;   /*字符编码*/
    __bool                      alpha_en;
    __u32                       timeout;
} msgbox_para_t;

typedef struct __GMSGBOXCREATE
{
    char                            *name;                  /** 全局消息对话框名称 */

    /*对msgbox而言，前几项必须是button的para信息*/
    __gui_dlgtemplate_t             *pDlgTemplate;          /** 全局消息对话框创建模板 */
    H_WIN                           hOwner;                 /** 全局消息对话框对话框的创建窗口句柄 */
    H_WIN                           hHosting;               /* 全局消息对话框对话框主窗口句柄*/
    H_WIN                           hLayer;                 /** 全局消息对话框的图层句柄 */
    __u32                           type;                   /*  全局消息对话框的类型*/
    msgbox_para_t                   *skin;                  /** 全局消息对话框创建附加数据 */
} __gui_gmsgboxcreate_para_t;

typedef __gui_gmsgboxcreate_para_t *pmsgboxcreate;


__u32  GUI_GMsgBox(__gui_gmsgboxcreate_para_t *create_info);
__s32  GUI_GMsgBoxSetText(H_WIN hbox, char *text);
__s32  GUI_GMsgBoxSetTextPos(H_WIN hbox, __pos_t *pos);

#endif /* #ifndef _GUI_MSGBOX_H_ */
