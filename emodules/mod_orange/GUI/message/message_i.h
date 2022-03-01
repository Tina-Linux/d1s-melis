/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    gui_message_e.h
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2009-8-4
*
*Description :  message 供外部使用的函数定义
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>                  <version>     <description>
*
* William Wang     2009-8-4         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __GUI_MESSAGE_I_H__
#define __GUI_MESSAGE_I_H__

#include "gui_i.h"

typedef enum __GUI_MSG_WINDOWS
{
    GUI_MSG_ADDNEWMANWIN   = 0x0000,    /*添加主窗口，系统服务消息，同步消息*/
    GUI_MSG_REMOVEMANWIN   = 0x0001,    /*删除主窗口，系统服务消息，同步消息*/
    GUI_MSG_MOVETOTOPMOST   = 0x0002,   /*窗口置顶，  系统服务消息，同步消息*/
    GUI_MSG_SETACTIVEMAIN   = 0x0003,   /*设置焦点framewin，系统服务消息，同步消息*/
    GUI_MSG_GETACTIVEMAIN   = 0x0004,   /*获取焦点framewin，系统服务消息，同步消息*/
    GUI_MSG_SHOWMANWIN     = 0x0005,    /*显示framewin，系统服务消息，同步消息*/
    GUI_MSG_HIDEMANWIN     = 0x0006,    /*隐藏framewin，系统服务消息，同步消息*/
    GUI_MSG_MOVEMANWIN     = 0x0007,    /*移动framewin，系统服务消息，同步消息*/
    GUI_MSG_SETCAPTURE      = 0x0008,   /*设置capture模式，在键盘或者触摸屏点击时调用，系统服务消息，同步消息*/
    GUI_MSG_GETCAPTURE      = 0x0009,   /*获取capture窗口，系统服务消息，同步消息*/

    GUI_MSG_ENABLEMANWIN   = 0x0010,    /*使framewin有效，系统服务消息，同步消息*/
    GUI_MSG_ISENABLED       = 0x0011,   /*判断framewin是否有效，系统服务消息，同步消息*/

    GUI_MSG_GETNEXTMANWIN  = 0x0013,    /*获取下一个framewin*/

    GUI_MSG_SHOWGLOBALCTRL  = 0x0014,   /*显示主窗口外控件，同步消息，系统服务消息*/
    GUI_MSG_HIDEGLOBALCTRL  = 0x0015,
} __gui_msg_windows_t;

#if 0
typedef enum __GUI_MSG_ASYNCMSG
{
    GUI_MSG_SETCURSOR       = 0x0020,
    GUI_HT_UNKNOWN          = 0x00,
    GUI_HT_OUT              = 0x01,
    GUI_HT_MENUBAR          = 0x02,
    GUI_HT_TRANSPARENT      = 0x03,
    GUI_HT_BORDER_TOP       = 0x04,
    GUI_HT_BORDER_BOTTOM    = 0x05,
    GUI_HT_BORDER_LEFT      = 0x06,
    GUI_HT_BORDER_RIGHT     = 0x07,
    GUI_HT_CORNER_TL        = 0x08,
    GUI_HT_CORNER_TR        = 0x09,
    GUI_HT_CORNER_BL        = 0x0A,
    GUI_HT_CORNER_BR        = 0x0B,
    GUI_HT_CLIENT           = 0x0C,
    GUI_HT_NEEDCAPTURE      = 0x10,
    GUI_HT_BORDER           = 0x11,
    GUI_HT_NCLIENT          = 0x12,
    GUI_HT_CAPTION          = 0x13,
    GUI_HT_ICON             = 0x14,
    GUI_HT_CLOSEBUTTON      = 0x15,
    GUI_HT_MAXBUTTON        = 0x16,
    GUI_HT_MINBUTTON        = 0x17,
    GUI_HT_HSCROLL          = 0x18,
    GUI_HT_VSCROLL          = 0x19,

    GUI_SBPOS_LEFTARROW     = 0x81,
    GUI_SBPOS_RIGHTARROW    = 0x82,
    GUI_SBPOS_LEFTSPACE     = 0x83,
    GUI_SBPOS_RIGHTSPACE    = 0x84,
    GUI_SBPOS_UPARROW       = 0x85,
    GUI_SBPOS_DOWNARROW     = 0x86,
    GUI_SBPOS_UPSPACE       = 0x87,
    GUI_SBPOS_DOWNSPACE     = 0x88,
    GUI_SBPOS_THUMB         = 0x89,
    GUI_SBPOS_UNKNOWN       = 0x80,
    GUI_SBPOS_MASK          = 0x80,

    GUI_MSG_NCHITTEST       = 0x0021,     /* this is an async message. */
    GUI_MSG_HITTEST         = GUI_MSG_NCHITTEST,

    GUI_MSG_CHANGESIZE      = 0x0022,
    GUI_MSG_QUERYNCRECT     = 0x0023,
    GUI_MSG_QUERYCLIENTAREA = 0x0024,
    GUI_MSG_SIZECHANGING    = 0x0025,
    GUI_MSG_SIZECHANGED     = 0x0026,
    GUI_MSG_SETFOCUS        = 0x0030,
    GUI_MSG_KILLFOCUS       = 0x0031,
    GUI_MSG_MOUSEACTIVE     = 0x0032,
    GUI_MSG_ACTIVE          = 0x0033,
    GUI_MSG_CHILDHIDDEN     = 0x0034,
    GUI_RCTM_CLICK      = 1,
    GUI_RCTM_KEY        = 2,
    GUI_RCTM_MESSAGE    = 3,
    GUI_RCTM_SHOWCTRL   = 4,

    /* Scroll bar notifying code */
    GUI_SB_LINEUP       = 0x01,
    GUI_SB_LINEDOWN     = 0x02,
    GUI_SB_LINELEFT     = 0x03,
    GUI_SB_LINERIGHT    = 0x04,
    GUI_SB_PAGEUP       = 0x05,
    GUI_SB_PAGEDOWN     = 0x06,
    GUI_SB_PAGELEFT     = 0x07,
    GUI_SB_PAGERIGHT    = 0x08,
    GUI_SB_THUMBPOSITION = 0x09,
    GUI_SB_THUMBTRACK   = 0x0A,
    GUI_SB_ENDSCROLL    = 0x0B,
    GUI_MSG_HSCROLL     = 0x0042,
    GUI_MSG_VSCROLL     = 0x0043,
    GUI_MSG_NCSETCURSOR = 0x0044,
    GUI_MSG_MOUSEMOVEIN = 0x0050
} __gui_msg_asyncmsg_t;

typedef enum __GUI_MSG_CREATE
{
    GUI_MSG_NCCREATE        = 0x0061,
    GUI_MSG_INITPANES       = 0x0062,
    GUI_MSG_DESTROYPANES    = 0x0063,
    GUI_MSG_NCDESTROY       = 0x0065,
    GUI_MSG_CLOSE           = 0x0066,
    GUI_MSG_NCCALCSIZE      = 0x0067,
    GUI_MSG_MAXIMIZE        = 0x0068,
    GUI_MSG_MINIMIZE        = 0x0069,
} __gui_msg_create_t;

typedef enum __GUI_MSG_SYSTEM
{
    GUI_MSG_QUIT                = 0x0140,
    GUI_MSG_IDLE                = 0x0142,
    GUI_MSG_DOESNEEDIME         = 0x0150,
    GUI_MSG_WINDOWCHANGED       = 0x0165,
    GUI_MSG_BROADCASTMSG        = 0x0166,
    GUI_MSG_REGISTERWNDCLASS    = 0x0167,
    GUI_MSG_UNREGISTERWNDCLASS  = 0x0168,
    GUI_MSG_NEWCTRLINSTANCE     = 0x0169,
    GUI_MSG_REMOVECTRLINSTANCE  = 0x016A,
    GUI_MSG_GETCTRLCLASSINFO    = 0x016B,
    GUI_MSG_CTRLCLASSDATAOP     = 0x016C
} __gui_msg_system_t;

typedef enum __GUI_MSG_CONTROL
{
    GUI_MSG_SYSCOMMAND          = 0x0121,
    GUI_MSG_GETDLGCODE          = 0x0122,
    GUI_MSG_INITDIALOG          = 0x0123,
    GUI_MSG_NEXTDLGCTRL         = 0x0124,
    GUI_MSG_ENTERIDLE           = 0x0125,
    GUI_MSG_DLG_GETDEFID        = 0x0126,
    GUI_MSG_DLG_SETDEFID        = 0x0127,
    GUI_MSG_DLG_REPOSITION      = 0x0128,
    GUI_MSG_INITPAGE            = 0x0129,
    GUI_MSG_SHOWPAGE            = 0x012A,
    GUI_MSG_SHEETCMD            = 0x012B,
    GUI_MSG_FONTCHANGING        = 0x0130,
    GUI_MSG_FONTCHANGED         = 0x0131,
    GUI_MSG_GETTEXTLENGTH       = 0x0132,
    GUI_MSG_GETTEXT             = 0x0133,
    GUI_MSG_SETTEXT             = 0x0134,
    GUI_MSG_ENABLE              = 0x0135,
} __gui_msg_control_t;

typedef enum __GUI_WIDGETCLASS_OP
{
    GUI_WCDOP_GETCCI            = 0x01,
    GUI_WCDOP_SETCCI            = 0x02
} __gui_widgetclass_op_t;

#endif
#define MSGSTS_KEYDOWN          (0x01)      /*按键来到还没有采样发出*/
#define MSGSTS_LONGKEYDOWN      (0x02)      /*按键被按下，长按键还没有采样发出*/
#define MSGSTS_POSTLONGKEY      (0x03)      /*长按键已被采样并发出*/
#define MSGSTS_POSTKEY          (0x04)      /*第一个按下键已被采样并被发出*/
#define MSGSTS_POSTKEYRISE      (0x05)      /*第一个RISE键已被采样并被发出*/
#define MSGSTS_TOUCHDOWN        (0x06)      /*按键来到还没有采样发出*/
#define MSGSTS_LONGTOUCHDOWN    (0x07)      /*按键被按下，长按键还没有采样发出*/
#define MSGSTS_POSTLONGTOUCH    (0x03)      /*长按键已被采样并发出*/
#define MSGSTS_POSTTOUCH        (0x08)
#define MSGSTS_POSTTOUCHRISE    (0x09)

/*判断为长按键累加键值数量*/
#define LONGKEY_OFFSET           (GUI_MSG_KEY_LONGUP - GUI_MSG_KEY_UP)
#define LONGTOUCH_OFFSET         (GUI_MSG_TOUCH_LONGDOWN - GUI_MSG_TOUCH_DOWN)
#define LONGKEY_COUNT            5
#define LONGTOUCH_COUNT          5
#define LOCK_MSGQ(pMsgQueue)     pthread_mutex_lock(&(pMsgQueue)->lock)
#define UNLOCK_MSGQ(pMsgQueue)   pthread_mutex_unlock(&(pMsgQueue)->lock)

#define IS_MSG_WANTED(message) \
    ( (iMsgFilterMin <= 0 && iMsgFilterMax <= 0) || \
      (iMsgFilterMin > 0 && iMsgFilterMax >= iMsgFilterMin && \
       message >= iMsgFilterMin && message <= iMsgFilterMax) )
#endif /* __GUI_WIN_I_H__ */
