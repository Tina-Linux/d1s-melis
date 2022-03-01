/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    message.h
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2009-7-22
*
*Description :  message definition and function implement
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>        <version>     <description>
*
* William Wang     2009-7-22         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __MESSAGE_H__
#define __MESSAGE_H__
#include <kconfig.h>

#define GUI_MSG_USER   0x2000

typedef enum __GUI_MSGID
{
    GUI_MSG_CREATE = 0,
    GUI_MSG_DESTROY,
    GUI_MSG_PAINT,                                /* draw */
    GUI_MSG_CLOSE,
    GUI_MSG_QUIT,
    GUI_MSG_KEY,
    GUI_MSG_TOUCH,
    GUI_MSG_COMMAND,
    GUI_MSG_ENABLE,
    GUI_MSG_STSTEM,                                 /*广播消息，注册控件消息等，(GUI系统保留)*/
    GUI_MSG_NOTIFY_CHILD_DELETED,
    GUI_MSG_SET_FOCUS,
    GUI_MSG_SET_UNFOCUS,
    GUI_MSG_INVALID,
    GUI_MSG_TOUCHACTIVE,
    GUI_MSG_TOUCHINACTIVE,
    GUI_MSG_INITDIALOG,
    GUI_MSG_NCPAINT,
    GUI_MSG_ERASEBKGND,
    GUI_MSG_TIMER,

    GUI_MSG_WIN_SLEEP = 100,
    GUI_MSG_WIN_WAKEUP,
    GUI_MSG_LYR_MOVE,

    GUI_MSG_KEYTONE,

    GUI_MSG_CONTROL = 0x1000,                       /*控件*/

    GUI_MSG_DESKTOP = 0x2000,



    GUI_MSG_USER_DEF = 0x3000,

    GUI_MSG_
} __gui_msgid_t;

#if (defined CONFIG_SOC_SUN3IW2 || defined CONFIG_SOC_SUN8IW19 || defined CONFIG_SOC_SUN20IW1)
//chengf  add  begin =================================
typedef enum __GUI_MY_MSGID_HOME
{
    GUI_MSG_AITALK_HOME_TO_FM = GUI_MSG_ + 1,
    GUI_MSG_AITALK_HOME_TO_MUSIC,
    GUI_MSG_AITALK_HOME_TO_MOVIE,
    GUI_MSG_AITALK_HOME_TO_PHOTO,
    GUI_MSG_AITALK_HOME_TO_EBOOK,     //+5
    GUI_MSG_AITALK_HOME_TO_RECORD,
    GUI_MSG_AITALK_HOME_TO_LINEIN,
    GUI_MSG_AITALK_HOME_TO_MANAGER,
    GUI_MSG_AITALK_HOME_TO_ALARM,
    GUI_MSG_AITALK_HOME_TO_CALENDAR,  //+10
    GUI_MSG_AITALK_HOME_TO_SETTING,

    GUI_MSG_AITALK_HOME_TO_

} __gui_my_msgid_home_t;


typedef enum __GUI_MY_MSGID_FM
{
    GUI_MSG_AITALK_FM_RETURN_HOME = GUI_MSG_ + 20,
    GUI_MSG_AITALK_FM_RETURN,
    GUI_MSG_AITALK_FM_ATUO_SEARCH,
    GUI_MSG_AITALK_FM_MANUL_SEARCH_UP,
    GUI_MSG_AITALK_FM_MANUL_SEARCH_DWON,
    GUI_MSG_AITALK_FM_BROWSE_FREQ,

    GUI_MSG_AITALK_FM_

} __gui_my_msgid_fm_t;


typedef enum __GUI_MY_MSGID_MUSIC
{
    GUI_MSG_AITALK_MUSIC_RETURN_HOME = GUI_MSG_ + 30,
    GUI_MSG_AITALK_MUSIC_RETURN,
    GUI_MSG_AITALK_MUSIC_PLAY,
    GUI_MSG_AITALK_MUSIC_PREV,
    GUI_MSG_AITALK_MUSIC_NEXT,

    GUI_MSG_AITALK_MUSIC_

} __gui_my_msgid_music_t;


typedef enum __GUI_MY_MSGID_MOVIE
{
    GUI_MSG_AITALK_MOVIE_RETURN_HOME = GUI_MSG_ + 40,
    GUI_MSG_AITALK_MOVIE_RETURN,
    GUI_MSG_AITALK_MOVIE_PLAY,
    GUI_MSG_AITALK_MOVIE_PREV,
    GUI_MSG_AITALK_MOVIE_NEXT,

    GUI_MSG_AITALK_MOVIE_

} __gui_my_msgid_movie_t;


typedef enum __GUI_MY_MSGID_PHOTO
{
    GUI_MSG_AITALK_PHOTO_RETURN_HOME = GUI_MSG_ + 50,
    GUI_MSG_AITALK_PHOTO_RETURN,
    GUI_MSG_AITALK_PHOTO_PLAY,
    GUI_MSG_AITALK_PHOTO_PAUSE,
    GUI_MSG_AITALK_PHOTO_PREV,
    GUI_MSG_AITALK_PHOTO_NEXT,

    GUI_MSG_AITALK_PHOTO_

} __gui_my_msgid_photo_t;


typedef enum __GUI_MY_MSGID_EBOOK
{
    GUI_MSG_AITALK_EBOOK_RETURN_HOME = GUI_MSG_ + 60,
    GUI_MSG_AITALK_EBOOK_RETURN,
    GUI_MSG_AITALK_EBOOK_PLAY,
    GUI_MSG_AITALK_EBOOK_PAUSE,
    GUI_MSG_AITALK_EBOOK_PREV_PAGE,
    GUI_MSG_AITALK_EBOOK_NEXT_PAGE,

    GUI_MSG_AITALK_EBOOK_

} __gui_my_msgid_ebook_t;


typedef enum __GUI_MY_MSGID_LINEIN
{
    GUI_MSG_AITALK_LINEIN_RETURN_HOME = GUI_MSG_ + 70,
    GUI_MSG_AITALK_LINEIN_RETURN,

    GUI_MSG_AITALK_LINEIN_

} __gui_my_msgid_linein_t;


typedef enum __GUI_MY_MSGID_CALENDAR
{
    GUI_MSG_AITALK_CALENDAR_RETURN_HOME = GUI_MSG_ + 80,
    GUI_MSG_AITALK_CALENDAR_RETURN,
    GUI_MSG_AITALK_CALENDAR_SAVE,
    GUI_MSG_AITALK_CALENDAR_CANCEL,

    GUI_MSG_AITALK_CALENDAR_

} __gui_my_msgid_calendar_t;


typedef enum __GUI_MY_MSGID_RECORD
{
    GUI_MSG_AITALK_RECORD_RETURN_HOME  = GUI_MSG_ + 90,
    GUI_MSG_AITALK_RECORD_RETURN,
    GUI_MSG_AITALK_RECORD_QUALITY,
    GUI_MSG_AITALK_RECORD_STYLE,
    GUI_MSG_AITALK_RECORD_FILE_LIST,

    GUI_MSG_AITALK_RECORD_

} __gui_my_msgid_record_t;


typedef enum __GUI_MY_MSGID_EXPLORER
{
    GUI_MSG_AITALK_EXPLORER_RETURN_HOME = GUI_MSG_ + 100,
    GUI_MSG_AITALK_EXPLORER_RETURN,
    GUI_MSG_AITALK_EXPLORER_NEXT,
    GUI_MSG_AITALK_EXPLORER_PREV,
    GUI_MSG_AITALK_EXPLORER_ENTER,

    GUI_MSG_AITALK_EXPLORER_

} __gui_my_msgid_explorer_t;



typedef enum __GUI_MY_MSGID_SETTING
{
    GUI_MSG_AITALK_SETTING_RETURN_HOME = GUI_MSG_ + 110,
    GUI_MSG_AITALK_SETTING_RETURN,
    GUI_MSG_AITALK_SETTING_ENTER,
    GUI_MSG_AITALK_SETTING_PREV,
    GUI_MSG_AITALK_SETTING_NEXT,
    GUI_MSG_AITALK_SETTING_GENERAL,
    GUI_MSG_AITALK_SETTING_POWER,
    GUI_MSG_AITALK_SETTING_ALARM,

    GUI_MSG_AITALK_SETTING_

} __gui_my_msgid_setting_t;


typedef enum __GUI_MY_MSGID_KARAOKE
{
    GUI_MSG_KARAOKE_UPDATE_LIST = GUI_MSG_ + 120,

    GUI_MSG_KARAOKE_

} __gui_my_msgid_karaoke_t;

//chengf  add  end =====================================



typedef enum __GUI_MSG_KEYID
{
    GUI_MSG_KEY_UP = 0x0,
    GUI_MSG_KEY_DOWN,
    GUI_MSG_KEY_ENTER,
    GUI_MSG_KEY_RIGHT,
    GUI_MSG_KEY_LEFT,
    GUI_MSG_KEY_ESCAPE,
    GUI_MSG_KEY_VADD,
    GUI_MSG_KEY_VDEC,
    GUI_MSG_KEY_RISE,
    GUI_MSG_KEY_POWER,
    GUI_MSG_KEY_SOURCE, // 10
    GUI_MSG_KEY_CLEAR,
    GUI_MSG_KEY_DISPLAY,
    GUI_MSG_KEY_NUM0,
    GUI_MSG_KEY_NUM1,
    GUI_MSG_KEY_NUM2,
    GUI_MSG_KEY_NUM3,
    GUI_MSG_KEY_NUM4,
    GUI_MSG_KEY_NUM5,
    GUI_MSG_KEY_NUM6,
    GUI_MSG_KEY_NUM7, // 20
    GUI_MSG_KEY_NUM8,
    GUI_MSG_KEY_NUM9,
    GUI_MSG_KEY_REPEATE,
    GUI_MSG_KEY_BLOCK,
    GUI_MSG_KEY_PLAY_PAUSE,
    GUI_MSG_KEY_TITLE,
    GUI_MSG_KEY_MENU,
    GUI_MSG_KEY_SETUP,
    GUI_MSG_KEY_GOTO,
    GUI_MSG_KEY_SEL, // 30
    GUI_MSG_KEY_SHIFT,
    GUI_MSG_KEY_DISC,
    GUI_MSG_KEY_ATT,
    GUI_MSG_KEY_POWEROFF,
    GUI_MSG_KEY_RST,
    GUI_MSG_KEY_CANCLE,
    GUI_MSG_KEY_ZOOM_UP,
    GUI_MSG_KEY_ZOOM_DOWN,
    GUI_MSG_KEY_HOLD,
    GUI_MSG_KEY_FM, // 40
    GUI_MSG_KEY_KARAOKE, //chengf  add
    GUI_MSG_KEY_VIDEO,
    GUI_MSG_KEY_AUDIO,
    GUI_MSG_KEY_EBOOK,
    GUI_MSG_KEY_PHOTO,
    GUI_MSG_KEY_TV,
    GUI_MSG_KEY_LCDOFF,
    GUI_MSG_KEY_RESERVED,
    GUI_MSG_KEY_MODE,
    GUI_MSG_KEY_MUTE,
    GUI_MSG_KEY_AITALK,
    GUI_MSG_KEY_MIC_ONOFF,
    GUI_MSG_KEY_REC_ONOFF,
    GUI_MSG_KEY_MIC_VOL_ADD,
    GUI_MSG_KEY_MIC_VOL_DEC,
    GUI_MSG_KEY_ECHO_ONOFF,
    GUI_MSG_KEY_ECHO_SETTING,
    GUI_MSG_KEY_ACCOMP_ONOFF,
    GUI_MSG_KEY_ADD_LYRIC_TIME,
    GUI_MSG_KEY_DEC_LYRIC_TIME,
    GUI_MSG_KEY_KARAOKE_MUTE,
    GUI_MSG_KEY_EQ,
    GUI_MSG_KEY_TVOUT,
    GUI_MSG_KEY_SINGER,
    GUI_MSG_KEY_ALPHABET,
    GUI_MSG_KEY_DIGITAL,
    GUI_MSG_KEY_FAVOR,
    GUI_MSG_KEY_NEWSONG,
    GUI_MSG_KEY_SEL_LIST,
    GUI_MSG_KEY_DELETE,
    GUI_MSG_KEY_CUTSONG,
    GUI_MSG_KEY_AUX,
    GUI_MSG_KEY_PREV,
    GUI_MSG_KEY_NEXT,
    GUI_MSG_KEY_FF,
    GUI_MSG_KEY_RR,
    GUI_MSG_KEY_NTFS_PAL,
    GUI_MSG_KEY_SEL_TIME,
    GUI_MSG_KEY_USB_TF,
    GUI_MSG_KEY_TACK,
    GUI_MSG_KEY_SCAN,
    GUI_MSG_KEY_EPG,
    GUI_MSG_KEY_LIST,
    GUI_MSG_KEY_RECORD,
    GUI_MSG_KEY_ECHO_VOL,

    GUI_MSG_KEY_LONGUP,
    GUI_MSG_KEY_LONGDOWN,
    GUI_MSG_KEY_LONGENTER,
    GUI_MSG_KEY_LONGRIGHT,
    GUI_MSG_KEY_LONGLEFT,
    GUI_MSG_KEY_LONGESCAPE,
    GUI_MSG_KEY_LONGVADD,
    GUI_MSG_KEY_LONGVDEC,
    GUI_MSG_KEY_LONGRISE,
    GUI_MSG_KEY_LONGPOWER,
    GUI_MSG_KEY_LONGSOURCE,
    GUI_MSG_KEY_LONGCLEAR,
    GUI_MSG_KEY_LONGDISPLAY,
    GUI_MSG_KEY_LONGNUM0,
    GUI_MSG_KEY_LONGNUM1,
    GUI_MSG_KEY_LONGNUM2,
    GUI_MSG_KEY_LONGNUM3,
    GUI_MSG_KEY_LONGNUM4,
    GUI_MSG_KEY_LONGNUM5,
    GUI_MSG_KEY_LONGNUM6,
    GUI_MSG_KEY_LONGNUM7,
    GUI_MSG_KEY_LONGNUM8,
    GUI_MSG_KEY_LONGNUM9,
    GUI_MSG_KEY_LONGREPEATE,
    GUI_MSG_KEY_LONGBLOCK,
    GUI_MSG_KEY_LONGPLAY_PAUSE,
    GUI_MSG_KEY_LONGTITLE,
    GUI_MSG_KEY_LONGMENU,
    GUI_MSG_KEY_LONGSETUP,
    GUI_MSG_KEY_LONGGOTO,
    GUI_MSG_KEY_LONGSEL,
    GUI_MSG_KEY_LONGSHIFT,
    GUI_MSG_KEY_LONGDISC,
    GUI_MSG_KEY_LONGATT,
    GUI_MSG_KEY_LONGPOWEROFF,
    GUI_MSG_KEY_LONGRST,
    GUI_MSG_KEY_LONGCANCLE,
    GUI_MSG_KEY_LONGZOOM_UP,
    GUI_MSG_KEY_LONGZOOM_DOWN,
    GUI_MSG_KEY_LONGHOLD,
    GUI_MSG_KEY_LONGFM,
    GUI_MSG_KEY_LONGKARAOKE, //chengf  add
    GUI_MSG_KEY_LONGVIDEO,
    GUI_MSG_KEY_LONGAUDIO,
    GUI_MSG_KEY_LONGEBOOK,
    GUI_MSG_KEY_LONGPHOTO,
    GUI_MSG_KEY_LONGTV,
    GUI_MSG_KEY_LONGLCDOFF,
    GUI_MSG_KEY_LONGRESERVED,
    GUI_MSG_KEY_LONGMODE,
    GUI_MSG_KEY_LONGMUTE,
    GUI_MSG_KEY_LONGAITALK,
    GUI_MSG_KEY_LONG_MIC_ONOFF,
    GUI_MSG_KEY_LONG_REC_ONOFF,
    GUI_MSG_KEY_LONG_MIC_VOL_ADD,
    GUI_MSG_KEY_LONG_MIC_VOL_DEC,
    GUI_MSG_KEY_LONG_ECHO_ONOFF,
    GUI_MSG_KEY_LONG_ECHO_SETTING,
    GUI_MSG_KEY_LONG_ACCOMP_ONOFF,
    GUI_MSG_KEY_LONG_ADD_LYRIC_TIME,
    GUI_MSG_KEY_LONG_DEC_LYRIC_TIME,
    GUI_MSG_KEY_LONG_KARAOKE_MUTE,
    GUI_MSG_KEY_LONGEQ,
    GUI_MSG_KEY_LONGTVOUT,
    GUI_MSG_KEY_LONG_SINGER,
    GUI_MSG_KEY_LONG_ALPHABET,
    GUI_MSG_KEY_LONG_DIGITAL,
    GUI_MSG_KEY_LONG_FAVOR,
    GUI_MSG_KEY_LONG_NEWSONG,
    GUI_MSG_KEY_LONG_SEL_LIST,
    GUI_MSG_KEY_LONG_DELETE,
    GUI_MSG_KEY_LONG_CUTSONG,
    GUI_MSG_KEY_LONGAUX,
    GUI_MSG_KEY_LONGPREV,
    GUI_MSG_KEY_LONGNEXT,
    GUI_MSG_KEY_LONGFF,
    GUI_MSG_KEY_LONGRR,
    GUI_MSG_KEY_LONG_NTFS_PAL,
    GUI_MSG_KEY_LONG_SEL_TIME,
    GUI_MSG_KEY_LONG_USB_TF,
    GUI_MSG_KEY_LONG_TACK,
    GUI_MSG_KEY_LONG_SCAN,
    GUI_MSG_KEY_LONG_EPG,
    GUI_MSG_KEY_LONG_LIST,
    GUI_MSG_KEY_LONG_RECORD,
    GUI_MSG_KEY_LONG_ECHO_VOL,
    GUI_MSG_KEY_TF_USB,
    GUI_MSG_KEY_MAX
} __gui_msg_keyid_t;
#elif defined CONFIG_SOC_SUN3IW1
typedef enum __GUI_MSG_KEYID
{
    GUI_MSG_KEY_UP = 0x0,
    GUI_MSG_KEY_DOWN,
    GUI_MSG_KEY_ENTER,
    GUI_MSG_KEY_RIGHT,
    GUI_MSG_KEY_LEFT,
    GUI_MSG_KEY_ESCAPE,
    GUI_MSG_KEY_VADD,
    GUI_MSG_KEY_VDEC,
    GUI_MSG_KEY_RISE,
    GUI_MSG_KEY_POWER,
    GUI_MSG_KEY_SOURCE,
    GUI_MSG_KEY_CLEAR,
    GUI_MSG_KEY_DISPLAY,
    GUI_MSG_KEY_NUM0,
    GUI_MSG_KEY_NUM1,
    GUI_MSG_KEY_NUM2,
    GUI_MSG_KEY_NUM3,
    GUI_MSG_KEY_NUM4,
    GUI_MSG_KEY_NUM5,
    GUI_MSG_KEY_NUM6,
    GUI_MSG_KEY_NUM7,
    GUI_MSG_KEY_NUM8,
    GUI_MSG_KEY_NUM9,
    GUI_MSG_KEY_REPEATE,
    GUI_MSG_KEY_BLOCK,
    GUI_MSG_KEY_PLAY_PAUSE,
    GUI_MSG_KEY_TITLE,
    GUI_MSG_KEY_MENU,
    GUI_MSG_KEY_SETUP,
    GUI_MSG_KEY_GOTO,
    GUI_MSG_KEY_SEL,
    GUI_MSG_KEY_SHIFT,
    GUI_MSG_KEY_DISC,
    GUI_MSG_KEY_ATT,
    GUI_MSG_KEY_POWEROFF,
    GUI_MSG_KEY_RST,
    GUI_MSG_KEY_CANCLE,
    GUI_MSG_KEY_ZOOM_UP,
    GUI_MSG_KEY_ZOOM_DOWN,
    GUI_MSG_KEY_HOLD,

    GUI_MSG_KEY_LONGUP = 0x50,
    GUI_MSG_KEY_LONGDOWN,
    GUI_MSG_KEY_LONGENTER,
    GUI_MSG_KEY_LONGRIGHT,
    GUI_MSG_KEY_LONGLEFT,
    GUI_MSG_KEY_LONGESCAPE,
    GUI_MSG_KEY_LONGVADD,
    GUI_MSG_KEY_LONGVDEC,
    GUI_MSG_KEY_LONGRISE,
    GUI_MSG_KEY_LONGPOWER,
    GUI_MSG_KEY_LONGSOURCE,
    GUI_MSG_KEY_LONGCLEAR,
    GUI_MSG_KEY_LONGDISPLAY,
    GUI_MSG_KEY_LONGNUM0,
    GUI_MSG_KEY_LONGNUM1,
    GUI_MSG_KEY_LONGNUM2,
    GUI_MSG_KEY_LONGNUM3,
    GUI_MSG_KEY_LONGNUM4,
    GUI_MSG_KEY_LONGNUM5,
    GUI_MSG_KEY_LONGNUM6,
    GUI_MSG_KEY_LONGNUM7,
    GUI_MSG_KEY_LONGNUM8,
    GUI_MSG_KEY_LONGNUM9,
    GUI_MSG_KEY_LONGREPEATE,
    GUI_MSG_KEY_LONGBLOCK,
    GUI_MSG_KEY_LONGPLAY_PAUSE,
    GUI_MSG_KEY_LONGTITLE,
    GUI_MSG_KEY_LONGMENU,
    GUI_MSG_KEY_LONGSETUP,
    GUI_MSG_KEY_LONGGOTO,
    GUI_MSG_KEY_LONGSEL,
    GUI_MSG_KEY_LONGSHIFT,
    GUI_MSG_KEY_LONGDISC,
    GUI_MSG_KEY_LONGATT,
    GUI_MSG_KEY_LONGPOWEROFF,
    GUI_MSG_KEY_LONGRST,
    GUI_MSG_KEY_LONGCANCLE,
    GUI_MSG_KEY_LONGZOOM_UP,
    GUI_MSG_KEY_LONGZOOM_DOWN,
    GUI_MSG_KEY_LONGHOLD,
} __gui_msg_keyid_t;
#endif

typedef enum __GUI_MSG_TOUCHID
{
    GUI_MSG_TOUCH_DOWN = 0x80,
    GUI_MSG_TOUCH_UP,
    GUI_MSG_TOUCH_MOVE,
    GUI_MSG_TOUCH_ZOOMIN,
    GUI_MSG_TOUCH_ZOOMOUT,
    GUI_MSG_TOUCH_NCDOWN,
    GUI_MSG_TOUCH_NCUP,
    GUI_MSG_TOUCH_NCMOVE,
    GUI_MSG_TOUCH_OVERDOWN,
    GUI_MSG_TOUCH_OVERUP,
    GUI_MSG_TOUCH_OVERMOVE,
    GUI_MSG_TOUCH_OVERZOOMIN,
    GUI_MSG_TOUCH_OVERZOOMOUT,
    GUI_MSG_TOUCH_LONGDOWN,
    GUI_MSG_TOUCH_OVERLONGDOWN
} __gui_msg_touchid_t;

typedef struct __GUI_KEY_INFO
{
    __u32  key;                                     /* key value                                                    */
    __u32  presscnt;
} __gui_keyinfo_t;


typedef struct __GUI_MSG
{
    __u32       id;
    H_WIN       h_srcwin;
    H_WIN       h_deswin;
    unsigned long       dwAddData1;
    unsigned long       dwAddData2;
    unsigned long       dwReserved;
    void       *p_arg;
} __gui_msg_t;

typedef __gui_msg_t  *pmsg;

/*子窗口用来向父窗口的通知消息结构*/
typedef struct __GUI_NOTIFY_MSG
{
    H_WIN        hWnd;                       /*子窗口句柄*/
    unsigned long        id;                         /*子窗口在父窗口中的唯一标识*/
    unsigned long        msgcode;                    /*通知消息的通知码*/
    unsigned long        dwAddData;                  /*通知消息的附加值*/
    unsigned long        dwReserved;                 /*通知消息的保留附加值*/
} __gui_notify_msg_t;

extern __bool   GUI_HavePendingMessage(H_WIN hWnd);                             /**/
extern __s32    GUI_BroadcastMessage(__gui_msg_t *pmsg);
extern __s32    GUI_PostSyncMessage(__gui_msg_t *pmsg);
extern __s32    GUI_SendMessage(__gui_msg_t *msg);
extern __s32    GUI_SendMessageTimeout(__gui_msg_t *msg, __u32 timeout);
extern __s32    GUI_PostMessage(__gui_msg_t *msg);
extern __s32    GUI_SendNotifyMessage(__gui_msg_t *msg);
extern __s32    GUI_GetMessage(__gui_msg_t *msg, H_WIN  hManWin);
extern __s32    GUI_GetMessageEx(__gui_msg_t *msg, H_WIN  hManWin);
extern __s32    GUI_MsgSetRepeatTimes(H_WIN  hManWin, __u32 count);
extern __bool   GUI_QueryMessage(H_WIN  hManWin);
extern __s32    GUI_PeekMessage(__gui_msg_t *msg, H_WIN  hManWin, __u32 len);
extern __s32    GUI_DispatchMessage(__gui_msg_t *msg);
extern __s32    GUI_PostQuitMessage(H_WIN  hWnd);
extern __s32    GUI_SendAsyncMessage(__gui_msg_t *msg);
extern __s32    GUI_ThrowAwayMessages(H_WIN hWnd);
extern __s32    GUI_SetSyncMsgRetVal(__gui_msg_t *msg, __s32 ret);
extern __s32    GUI_PostSyncSem(__gui_msg_t *msg);
#endif
