/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：        txtbox.h
*
*Author：           William Wang
*
*Version :          1.1.0
*
*Date:              2008-12-17
*
*Description :      lion txtbox control function implement
*
*Others :           None at present.
*
* History :
*
* <Author>          <time>          <version>     <description>
*
* William Wang     2008-12-17         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __TXTBOX_H__
#define __TXTBOX_H__

#define CTRL_TXTBOX         ("txtbox")

typedef enum txtbox_mode
{
    KALAOK_MODE                    = 0 << 0,
    LINEROLL_MODE                  = 1 << 0,
    HORIZONTAL_MODE                = 0 << 3,
    VERITICAL_MODE                 = 1 << 3
} __txtbox_mode_t;


typedef enum txtbox_switch_mode
{
    PAGE_SWITCH_MODE   = 0,
    LINE_SWITCH_MODE,
} txtbox_switch_mode_t;

typedef struct __TXTBOX_ITEM_ATTR
{
    char                          *buffer;
    __u32                          pstamp;                      /* 用户自定义的数据内容*/
    __u32                          estamp;
} __txtbox_item_attr_t;

typedef struct __TXTBOX_PRAR
{
    __u32                          nitem;                       /* item初始数目*/
    __txtbox_item_attr_t          *items;                       /* item数组的指针*/

    __u32                          mode;                        /*fixed mode or var mode*/
    __u32                          font_size;
    __u32                          dist;
    GUI_FONT                      *txtfont;
    __epdk_charset_enm_e           txt_charen;
    GUI_COLOR                      bk_color;
} __txtbox_para_t;

#define TXTBOXN_PUSHED                1
#define TXTBOXN_UNPUSHED              2

/*
**********************************************************************************************************************
*                                               FUNCTION
**********************************************************************************************************************
*/
__u32   GUI_TxtboxSetStamp(H_WIN txt, __u32 stamp);
__u32   GUI_TxtboxOnDraw(H_WIN txt);
__u32   GUI_TxtboxSetMode(H_WIN txt, __u8 mode);
__u32   GUI_TxtboxGetMode(H_WIN txt);
__s32   GUI_TxtBoxSetPara(H_WIN hbox, __txtbox_para_t *para);
__bool  RegisterTxtboxControl(void);

#endif
