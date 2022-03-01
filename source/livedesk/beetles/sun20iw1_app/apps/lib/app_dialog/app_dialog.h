/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2011, CHIPHD, China
*                                            All Rights Reserved
*
* File      : app_dialog.h
* By        : CQQ
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011/06/02 20:56  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#ifndef __APP_DIALOG_H__2C160E70_63CA_4019_A468_4515BA77B392__
#define __APP_DIALOG_H__2C160E70_63CA_4019_A468_4515BA77B392__

#include "beetles_app.h"

#define ADLG_BIT_(_i)                     (1 << (_i))

//消息框风格
typedef enum tag_APP_DIALOG_E
{
    ADLG_OK                           = 0, //消息框包含一个按钮：OK
    ADLG_OKCANCEL                     = 1, //消息框包含两个按钮：OK和Cancel
    ADLG_YESNO                        = 2, //消息框包含两个按钮：Yes和No
    ADLG_RETRYCANCEL                  = 3, //消息框包含两个按钮：Retry和Cancel
    ADLG_ABORTRETRYIGNORE             = 4, //消息框包含三个按钮：Abort，Retry和Ignore
    ADLG_YESNOCANCEL                  = 5,  //消息框包含三个按钮：Yes，No和Cancel
    ADLG_NOTHING                        = 6//只是做一个提示，无按钮
} APP_DIALOG_E;
#define ADLG_BTN_STYLE_BIT            0x0000000fL

//TODO:to be continue
#define ADLG_ICONHAND                 0x00000010L       //在消息框中显示停止标志图标
#define ADLG_ICONQUESTION             0x00000020L       //在消息框中显示问号图标
#define ADLG_ICONEXCLAMATION          0x00000030L       //在消息框中显示感叹号图标
#define ADLG_ICONASTERISK             0x00000040L       //在消息框中显示一个圆包围着字母“i”的图标
#define ADLG_ICONSTOP                 ADLG_ICONHAND     //在消息框中显示停止标志图标
#define ADLG_ICONINFORMATION          ADLG_ICONASTERISK //在消息框中显示一个圆包围着字母“i”的图标

#define ADLG_ICON_STYLE_BIT           0x000000f0L

//对话框结果集
enum
{
    ADLG_IDOK                         = 1,
    ADLG_IDCANCEL                     = 2,
    ADLG_IDABORT                      = 3,
    ADLG_IDRETRY                      = 4,
    ADLG_IDIGNORE                     = 5,
    ADLG_IDYES                        = 6,
    ADLG_IDNO                         = 7,
    ADLG_IDM_TIMEROUT                 = 8
};

//align(base screen)
#define ADLG_TA_LEFT                   ADLG_BIT_(16)
#define ADLG_TA_RIGHT                  ADLG_BIT_(17)
#define ADLG_TA_HCENTER                (ADLG_TA_LEFT | ADLG_TA_RIGHT)
#define ADLG_TA_HORIZONTAL             ADLG_TA_HCENTER

#define ADLG_TA_TOP                    ADLG_BIT_(18)
#define ADLG_TA_BOTTOM                 ADLG_BIT_(19)
#define ADLG_TA_VCENTER                (ADLG_TA_TOP | ADLG_TA_BOTTOM)
#define ADLG_TA_VERTICAL               ADLG_TA_VCENTER

#define ADLG_TA_CENTER                 (ADLG_TA_HCENTER | ADLG_TA_VCENTER)
#define ADLG_TA_(_H, _V)               ((_H) | (_V))

#define ADLG_DS_VISIBLE                ADLG_BIT_(20)

#define ADLG_MAKE_WND_STYLE(_i, _b)    ((_i) | (_b)) //icon and btn style compose
#define ADLG_GET_BTN_STYLE(_s)         ((_s) & ADLG_BTN_STYLE_BIT)
#define ADLG_GET_ICON_STYLE(_s)        ((_s) & ADLG_ICON_STYLE_BIT)
#define ADLG_GET_WND_STYLE(_s)         LOWORD(_s)

#define ADLG_GET_ALIGN(_s)             ((_s) & (ADLG_TA_HORIZONTAL | ADLG_TA_VERTICAL))//HIWORD(_s)
#define ADLG_GET_ALIGN_H(_s)           ((_s) & ADLG_TA_HORIZONTAL)
#define ADLG_GET_ALIGN_V(_s)           ((_s) & ADLG_TA_VERTICAL)

#define ADLG_STYLE_(_align, _wndstyle) (_align | _wndstyle)
//for example : ADLG_STYLE_(ADLG_TA_CENTER, ADLG_YESNO)


//传入图片资源id顺序
enum
{
    ADLG_BMP_SZID_BG = 0, //背景
    ADLG_BMP_SZID_BTN_F,  //按钮焦点
    ADLG_BMP_SZID_BTN_N,  //按钮普通
    ADLG_BMP_SZID_ICON,   //图标
    ADLG_BMP_MAX_SZID     //id个数
};

//传入字符串资源id顺序
enum
{
    ADLG_STR_SZID_TITLE = 0,   //消息框标题
    ADLG_STR_SZID_CONTENT,     //提示内容
    ADLG_STR_MAX_SZID,         //id个数
};

#define ADLG_STR_TITLE_MEM_SIZE        (16 * 3)
#define ADLG_STR_CONTENT_MEM_SIZE      (48 * 3)//((64 - 16) * 3)

enum
{
    ADLG_CMD_EXIT = 0,
    ADLG_CMD_SET_UI,
    ADLG_END_CMD,
};

//创建对话框
/************************************************************************/
// h_parent --- dialog parent window handle (must be main window)
// win_id   --- dialog window id of dialog parent win (size: __u16)
// style    --- style of button and alignment
// bmp_id   --- bmp32 res id array (array size -- ADLG_BMP_MAX_SZID)
// str_id   --- string res id array (array size -- ADLG_STR_MAX_SZID)
// timeout  --- auto close dialog time (xxxx ms), 0 for infinitude
// return value --- dialog frame window handle
// remark : to show dialog, need to use app_dialog_set_ui(), see app_dialog_set_ui remark
/************************************************************************/
H_WIN app_dialog_create(H_WIN h_parent, __s32 win_id, __u32 style,
                        __s32 bmp_id[], __s32 str_id[], __u32 timeout, const char *str);

#define app_center_dialog_create(_h, _id, _wnd_s, _bmp, _str)  \
    app_dialog_create(_h, _id, (ADLG_TA_CENTER | ADLG_DS_VISIBLE | (_wnd_s)), _bmp, _str, 0)

#define default_dialog(_dialog, _parent, _id, _btn_s, _str)  do \
    {\
        __s32 _bmp[] = {ID_DIALOG_MSGBOX_BMP, ID_DIALOG_MSGBOX_BTN_F_BMP, ID_DIALOG_MSGBOX_BTN_N_BMP, 0}; \
        if (_btn_s == ADLG_NOTHING){\
            _bmp[0] = ID_DIALOG_MSGBOX_1_BMP;}\
        _dialog = app_dialog_create(_parent, _id, (ADLG_TA_CENTER | ADLG_DS_VISIBLE | ADLG_MAKE_WND_STYLE(ADLG_ICONINFORMATION, _btn_s)), _bmp, _str, 0, 0); \
    }while (0)

#define default_dialog_timeout(_dialog, _parent, _id, _btn_s, _str, timeout)  do \
    {\
        __s32 _bmp[] = {ID_DIALOG_MSGBOX_BMP, ID_DIALOG_MSGBOX_BTN_F_BMP, ID_DIALOG_MSGBOX_BTN_N_BMP, 0}; \
        if (_btn_s == ADLG_NOTHING){\
            _bmp[0] = ID_DIALOG_MSGBOX_1_BMP;}\
        _dialog = app_dialog_create(_parent, _id, (ADLG_TA_CENTER | ADLG_DS_VISIBLE | ADLG_MAKE_WND_STYLE(ADLG_ICONINFORMATION, _btn_s)), _bmp, _str, timeout, 0); \
    }while (0)
#define default_dialog_ex(_dialog, _parent, _id, _btn_s, _str, str_ex)  do \
    {\
        __s32 _bmp[] = {ID_DIALOG_MSGBOX_BMP, ID_DIALOG_MSGBOX_BTN_F_BMP, ID_DIALOG_MSGBOX_BTN_N_BMP, 0}; \
        _dialog = app_dialog_create(_parent, _id, (ADLG_TA_CENTER | ADLG_DS_VISIBLE | ADLG_MAKE_WND_STYLE(ADLG_ICONINFORMATION, _btn_s)), _bmp, _str, 0, str_ex); \
    }while (0)

//创建对话框
/************************************************************************/
// h_parent  --- dialog parent handle (must be main window)
// win_id    --- dialog window id of dialog parent window (size: __u16)
// style     --- style of button
// bmp_id    --- bmp32 res id array (array size -- ADLG_BMP_MAX_SZID)
// str_id    --- string res id array (array size -- ADLG_STR_MAX_SZID)
// timeout   --- auto close dialog time (xxxx ms), 0 for infinitude
// dialog_rt --- dialog screen rect
// return value --- dialog frame window handle
/************************************************************************/
//TODO:to be continue
H_WIN app_dialog_create_ex(H_WIN h_parent, __s32 win_id, __u32 btn_style,
                           __s32 bmp_id[], __s32 str_id[], __u32 timeout, GUI_RECT *dialog_rt);

//销毁对话框
/************************************************************************/
// msg --- message of dialog parent window callback function received
// msg->h_deswin   --- dialog parent window
// msg->h_srcwin   --- dialog frame window handle
// msg->id         --- GUI_MSG_COMMAND
// msg->dwAddData1 --- LOWORD : dialog window id; HIWORD : ADLG_CMD_EXIT
// msg->dwAddData2 --- result of dialog(such as ADLG_IDYES...)
// msg->dwReserved --- reserve
// return value    --- 0
// remark :
//       1. when dialog is to be closed, dialog send this message to it's parent,
//          dialog parent window should call this function to delete dialog.
//       2. app should set focus after this function if necessary.
/************************************************************************/
__s32 app_dialog_destroy(H_WIN h_dialog);

// 重置对话框ui参数
enum
{
    // coordinate base on dialog window
    ADLG_UI_CMD_TITLE = 0, //reset title, (wParam, lParam) -> (x, y)
    ADLG_UI_CMD_CONTENT,   //reset content, lParam : point of RECT(x, y, width, height)
    ADLG_UI_CMD_ICON,      //reset icon, (wParam, lParam) -> (x, y)
    ADLG_UI_CMD_START_BTN, //reset the first button, (wParam, lParam) -> (x, y)
    ADLG_END_UI_CMD,       //use this command to show dialog
    ADLG_MAX_UI_CMD
};
#define ADLG_UI_OK          ADLG_END_UI_CMD
/************************************************************************/
// ui_id  --- such as ADLG_UI_CMD_TITLE ...
// wParam --- depend on ui_id
// lParam --- depend on ui_id
// return --- 0
/************************************************************************/
__s32 app_dialog_set_ui(H_WIN h_dialog, __s32 ui_id, __s32 wParam, signed long lParam);


/* //app use dialog, for example:
__s32 xxx_win_cb(__gui_msg_t *msg)
{
    switch(msg->id)
    {
    case GUI_MSG_COMMAND:
        {
            __u32 result;
            if (LOWORD(msg->dwAddData1) == ID_DIALOG)//(ID_DIALOG range [0, 0xffff])
            {
                switch(HIWORD(msg->dwAddData1))
                {
                case ADLG_CMD_EXIT:
                    {
                        result = msg->dwAddData2;
                        //to delete dialog
                        app_dialog_destroy(msg);
                        //do something for result
                    }
                    return 0;
                case ADLG_CMD_SET_UI:
                    {//set dialog ui para
                        app_dialog_set_ui(msg->h_srcwin, ADLG_UI_OK, 0, 0);
                    }
                    break;
                }
            }
        }
        break;
    }
    return GUI_FrmWinDefaultProc(msg);
}
*/

#endif //__APP_DIALOG_H__2C160E70_63CA_4019_A468_4515BA77B392__
//End of this file

