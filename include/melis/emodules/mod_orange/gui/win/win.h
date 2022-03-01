/*
*******************************************************************************************************************
*                                                           Mod orange
*                                                   the embedded graphic support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    win.h
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2009-8-1
*
*Description :  Orange Win function and variable definition
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>        <version>     <description>
*
* William Wang     2009-8-1         1.1.0        Create File
*
*******************************************************************************************************************
*/
#ifndef _GUI_WIN_H_
#define _GUI_WIN_H_
#include <mod_charset.h>

#define TYPE_HWND               0x01
#define TYPE_LAYERWIN       0x11
#define TYPE_MANWIN         0x12
#define TYPE_FRAMEWIN       0x13
#define TYPE_CONTROL        0x14
#define TYPE_HDC                0x08
#define TYPE_SCRDC          0x81
#define TYPE_GENDC          0x82
#define TYPE_MEMDC          0x83

#define LOBYTE(w)               ((__u8)(w))
#define HIBYTE(w)               ((__u8)(((__u16)(w) >> 8) & 0xFF))
#define MAKEWORD(low, high)     ((__u16)(((__u8)(low)) | (((__u16)((__u8)(high))) << 8)))
#define LOWORD(l)               ((__u16)(__u32)(l))
#define HIWORD(l)               ((__u16)((((__u32)(l)) >> 16) & 0xFFFF))
#define LOSWORD(l)              ((__s16)(__u32)(l))
#define HISWORD(l)              ((__s16)((((__u32)(l)) >> 16) & 0xFFFF))
#define MAKELONG(low, high)     ((__u32)(((__u16)(low)) | (((__u32)((__u16)(high))) << 16)))

#define GUI_PARSEX(dwAddData2)  (LOSWORD(dwAddData2))
#define GUI_PARSEY(dwAddData2)  (HISWORD(dwAddData2))

typedef enum tag_GUI_LYRWIN_STA
{
    GUI_LYRWIN_STA_ON = 0,                            /* on                                                           */
    GUI_LYRWIN_STA_OFF,                               /* off                                                          */
    GUI_LYRWIN_STA_SLEEP,                             /* off, layer released, but frame buffer not free               */
    GUI_LYRWIN_STA_SUSPEND,                           /* off, layer released, frame buffer free                       */
    GUI_LYRWIN_STA_ERR
} __gui_lyrwin_sta_t;

typedef enum __GUI_PAIN_MOD
{
    GUI_PAIN_MODE_UNFOCUS = 0,
    GUI_PAIN_MODE_FOCUS,
    GUI_PAIN_MODE_FASTDRAW,
    GUI_PAIN_MODE_DRAW,
    GUI_PAIN_MODE_FASTFOCUS,
    GUI_PAIN_MODE_FASTUNFOCUS,
    GUI_PAIN_MODE_ACTIVED,
    GUI_PAIN_MODE_INACTIVED
} __gui_painmod_t;

/*reserver for future*/
typedef enum  WINSTATUS
{
    WS_NONE                         = 0,
    WS_VISIBLE                      = (1 << 0),
    WS_CHILD                        = (1 << 1),
    WS_DISABLED                     = (1 << 2),
    WS_MEMDEV                       = (1 << 3),
    WS_VSCROLL                      = (1 << 4),
    WS_HSCROLL                      = (1 << 5),
    WS_TABSTOP                      = (1 << 6),
    WS_LYRWIN                       = (1 << 7),
    WS_EX_NONE                      = (0),
    WS_EX_TOPMOST                   = (1 << 16),
    WS_EX_TRANSPARENT               = (1 << 17),
    WS_EX_NOPARENTNOTIFY            = (1 << 18),
    WS_EX_CHILDPROC                 = (1 << 19)     /**/
} __gui_winstatus_t;

/* 标准控件ID号 */
typedef enum CONTROLIDS
{
    IDC_STATIC                      = 0,
    IDOK                            = 1,
    IDCANCEL                        = 2,
    IDABORT                         = 3,
    IDRETRY                         = 4,
    IDIGNORE                        = 5,
    IDYES                           = 6,
    IDNO                            = 7,
    IDM_TIMEROUT                    = 8
} __gui_ctrlid_t;

typedef enum MSGBOX_TYPE
{
    MB_OK                           = 0,
    MB_OKCANCEL                     = 1,
    MB_YESNO                        = 2,
    MB_RETRYCANCEL                  = 3,
    MB_ABORTRETRYIGNORE             = 4,
    MB_YESNOCANCEL                  = 5
} __gui_msgboxtype_t;


#define GUI_SW_HIDE                 0x0000
#define GUI_SW_SHOW                 0x0010
#define GUI_SW_SHOWNORMAL           0x0100

typedef struct gui_rect_zoom
{
    __u32 src_w;
    __u32 src_h;
    __u32 dst_w;
    __u32 dst_h;
} __gui_rect_zoom;

typedef struct gui_rect
{
    __s32  left;
    __s32  top;
    __s32  right;
    __s32  bottom;
} __gui_rect_t;

typedef __gui_rect_t *pguirect;

typedef struct __show_info_move
{
    H_LYR                 hlayer;
    RECT                  region;
    GUI_FONT             *pFont;
    __epdk_charset_enm_e  encode_id;
    char                 *string;
    GUI_COLOR             bkColor;
    void                 *bmp;
    __pos_t               bmp_pos;
    __bool                alpha_en;
    GUI_COLOR             fontColor;
    __u32                 align;
} __show_info_move_t;

typedef __show_info_move_t *pshowinfo;

typedef struct __GUI_WIN_PAINPARA
{
    __gui_painmod_t                 mode;                          /* 绘制模式设置 foucs,unfoucs......                             */
    __s32                           aux;
    RECT                            invalidrect;
} __gui_win_painpara_t;

typedef struct __GUI_TIMER_PROCPARA
{
    H_WIN                           hWnd;
    __u16                           id;
    __u32                           dwAddData;
    __u32                           dwReserved;
} __gui_timer_procpara_t;

/*窗口通知函数定义，窗口通知函数用来实现窗口的具体操作功能(播放下一首等)，
根据通知函数是否为空来确定是否需要向父窗口发送通知码*/
typedef void (* NOTIFPROC)(__gui_notify_msg_t *msg);
typedef __s32(* __pGUI_WIN_CB)(__gui_msg_t *msg);
typedef __s32(* __pGUI_TIMERPROC)(__gui_timer_procpara_t *para);

typedef enum
{
    GUI_LYRWIN_NORMAL = 0,
    GUI_LYRWIN_SPRITE
} __lyrwin_type_e;

typedef struct __CURSOR_ATTR
{
    void                 *pat_mem;
    void                 *palette;
} __gui_cursor_t;

/*图层的创建信息结构*/
typedef struct __LAYERWIN_ATTR
{
    char                            *name;

    __disp_layer_para_t            *lyrpara;                /* used to request a hw layer                                   */
    __gui_lyrwin_sta_t              initsta;                /* init status of lion layer                                    */

    __u8                            type;
} __layerwincreate_para_t;

typedef __layerwincreate_para_t     *playerwincreate;

/*framewin窗口的创建信息结构*/
typedef struct _FRAMECREATE
{
    char                            *name;

    __u32                           dwStyle;                /*窗口风格*/
    __u32                           dwExStyle;              /*窗口扩展风格*/
    const char                     *spCaption;              /*窗口的标题*/
    H_WIN                           hOwner;                 /*framewin的拥有者句柄*/
    H_WIN                           hHosting;               /*framewin的主窗口句柄*/
    H_WIN                           hLayer;                 /*framewin的图层窗口*/
    __pGUI_WIN_CB                   FrameWinProc;           /*framewin的消息处理过程*/
    __u16                           id;                     /*framewin的ID*/
    RECT                            rect;                   /*framewin在图层窗口中的位置*/
    __color_t                       BkColor;                /*framewin的背景色*/
    void                            *attr;                  /*framewin的私有属性信息*/

    /*保留参数，可以不填*/
    unsigned long                           dwReserved;
} __gui_framewincreate_para_t;

typedef __gui_framewincreate_para_t *pframewincreate;

/*主窗口的创建信息结构*/
typedef struct __MAINCREATE
{
    char                            *name;                  /*主窗口名称*/

    __u16                           id;                     /*主窗口的窗口ID*/
    H_WIN                           hParent;                /*主窗口的父窗口句柄*/
    H_WIN                           hHosting;               /*主窗口的拥有者窗口*/
    __pGUI_WIN_CB                   ManWindowProc;         /*主窗口的消息处理过程*/
    void                           *attr;                   /*主窗口的私有属性*/
} __gui_manwincreate_para_t;

typedef __gui_manwincreate_para_t *pmanwincreate;



/*控件窗口的创建信息结构*/
typedef struct __CONTROLCREATE
{
    char                            *name;                  /*控件窗口名称*/

    char                             *spClassName;          /*控件窗口的类名称*/
    char                             *spCaption;            /*控件窗口的标题文字*/
    __u32                           dwStyle;                /*控件窗口的风格*/
    __u32                           dwExStyle;              /*控件窗口的扩展风格*/
    __u16                           id;                     /*控件窗口的标识ID*/
    RECT                            ctl_rect;               /*控件窗口在父窗口中的位置*/
    H_WIN                           hParent;                /*控件窗口的父窗口*/
    H_LYR                           hLayer;                 /*控件的图层句柄，只有对窗口风格
                                                            为WS_LYRWIN时有效*/
    void                            *attr;                  /*控件窗口的私有信息*/
} __gui_ctlwincreate_para_t;

typedef __gui_ctlwincreate_para_t   *pctlwincreate;
/**
 * Structure which defines a control.
 */
typedef struct _CTRLDATA
{
    char                           *class_name;             /** 控件的类名 */
    char                           *name;                   /*  控件名称*/
    __u32                           dwStyle;                /** 控件的风格 */
    RECT                            ctl_rect;               /** 控件在framewin中的矩形区域 */
    __u16                           id;                     /** 控件ID **/
    char                           *caption;                /** 控件的标题 */
    unsigned long                           dwAddData;              /** 控件的附加数据 */
    __u32                           dwExStyle;              /** 控件的扩展风格 */
} __gui_ctrldata_t;

typedef __gui_ctrldata_t *pctrldata;

typedef struct _DLGTEMPLATE
{
    __u32                           dwStyle;                /** 对话框风格 */
    __u32                           dwExStyle;              /** 对话框扩展风格 */
    RECT                            dlg_rect;               /** 对话框在framebuffer中的矩形区域*/
    const char                     *caption;                /** 对话框标题 */
    __u32                           controlnr;              /** 对话框控件数量 */
    pctrldata                       controls;               /** 对话框的控件数组 */
    unsigned long                           dwAddData;              /** 对话框的额外数据 */
} __gui_dlgtemplate_t;

typedef __gui_dlgtemplate_t *pdlgtemplate;

typedef struct __DIALOGCREATE
{
    char                            *name;                  /** 对话框名称 */
    __gui_dlgtemplate_t             *pDlgTemplate;          /** 对话框创建模板 */
    H_WIN                           hOwner;                 /** 对话框对话框的创建窗口句柄 */
    H_WIN                           hHosting;               /* 对话框主窗口句柄*/
    H_WIN                           hLayer;                 /** 对话框的图层句柄 */
    __u16                           id;                     /*控件窗口的标识ID*/
    __pGUI_WIN_CB                   DialogProc;             /** 对话框的默认处理函数 */
    unsigned long                           dwAddData;              /** 对话框创建附加数据 */
} __gui_dialogcreate_para_t;

typedef __gui_dialogcreate_para_t *pdialogcreate;

typedef struct __GUI_BlitInfo
{
    void           *src_addr;
    __pos_t         srcstart;
    __pos_t         srcend;
    __u32           src_width;
    __u32           src_height;
    __u8            bpp;
    void           *dst_addr;
    __pos_t         dststart;
    __pos_t         dstend;
    __u32           dst_width;
    __u32           dst_height;
} __gui_blitinfo_t;

/*通用窗口创建与删除函数*/
extern H_WIN     GUI_FrmWinCreate(pframewincreate create_info);                         /*创建framewin*/
extern __s32     GUI_FrmWinDelete(H_WIN  hframewin);                                    /*删除framewin*/
extern H_WIN     GUI_ManWinCreate(pmanwincreate create_info);                           /*创建主窗口窗口*/
extern __s32     GUI_ManWinDelete(H_WIN  hmanwin);                                      /*删除主窗口*/
extern H_WIN     GUI_LyrWinCreate(playerwincreate create_info);                         /*创建图层窗口*/
extern __s32     GUI_LyrWinDelete(H_WIN  hlayer);                                       /*删除图层窗口*/
extern H_WIN     GUI_CtrlWinCreate(__gui_ctlwincreate_para_t *create_info);             /*创建控件窗口*/
extern __s32     GUI_CtrlWinDelete(H_WIN  hWnd);                                        /*删除控件窗口*/
extern void      GUI_WinThreadCleanup(H_WIN hManWnd);                                   /*窗口与消息循环相关的数据结构的清除*/

/*窗口设焦处理函数*/
extern H_WIN     GUI_SetActiveManWin(H_WIN  hWnd);                                      /*设置active 主窗口，用来接收外部消息的主窗口，返回值为原来的active主窗口*/
extern H_WIN     GUI_GetActiveManWin(void);                                             /*获取active 主窗口句柄*/
extern H_WIN     GUI_WinSetFocusChild(H_WIN hWnd);                                      /*设置focus主窗口,参数可以为任意类型的窗口(设置完成之后该窗口是系统的焦点子窗口)*/
extern H_WIN     GUI_WinGetFocusChild(H_WIN hWnd);                                      /*获取某个窗口的焦点子窗口*/
extern H_WIN     GUI_WinSetActiveChild(H_WIN hWnd);                                     /*设置窗口为某一窗口的焦点子窗口(设置完成之后该窗口不一定是系统的焦点子窗口) */


/*窗口类型获取，窗口关系获取和判断函数*/
extern __s32     GUI_WinGetType(H_WIN hWnd);                                            /*获取某个窗口的窗口类型*/
extern H_WIN     GUI_WinGetLyrWin(H_WIN hWnd);                                          /*获取某个窗口的图层句柄*/
extern H_WIN     GUI_WinGetMainManWin(H_WIN hWnd);                                      /*获取主管理窗口的窗口句柄*/
extern H_WIN     GUI_WinGetManWin(H_WIN hWnd);                                          /*获取窗口的主窗口句柄*/
extern H_WIN     GUI_WinGetFrmWin(H_WIN hWnd);                                          /*获取某个窗口的framewin句柄*/
extern H_WIN     GUI_WinGetParent(H_WIN hWnd);                                          /*得到某个窗口的父窗口*/
extern H_WIN     GUI_WinGetFirstChild(H_WIN hWnd);                                      /*得到某个窗口的第一个子窗口*/
extern H_WIN     GUI_WinGetNextBro(H_WIN hWnd);                                         /*得到某个窗口的兄弟窗口*/
extern __bool    GUI_WinIsAncestor(H_WIN hWnd, H_WIN hChild);                           /*判断某个窗口是否是另一个窗口的祖先*/
extern __bool    GUI_WinIsChild(H_WIN HWnd, H_WIN hParent);                             /*判断某个窗口是否是另外一个窗口的儿子窗口*/
extern H_WIN     GUI_WinGetRootWin(void);                                               /*得到根窗口的窗口句柄*/
extern H_WIN     GUI_WinGetNextHostedWin(H_WIN  hWnd);
extern H_WIN     GUI_WinGetFirstHostedWin(H_WIN  hWnd);
extern H_WIN     GUI_WinGetOwnerWin(H_WIN  hWnd);
extern H_WIN     GUI_WinGetDlgItem(H_WIN hDlg, __s32 nIDDlgItem);                       /*获取一个frmwin的窗口矩形区域*/
extern __u32     GUI_WinGetItemId(H_WIN hItem);                                         /*根据窗口的句柄获取窗口的ID*/
extern H_WIN     GUI_WinGetPrimitiveWin(H_WIN hWnd);
extern H_WIN     GUI_WinGetHandFromId(H_WIN hParent, __s32 nID);

/*获取和设置窗口的属性函数*/
extern __s32     GUI_WinGetName(H_WIN hWnd, char *name);                                /*得到某个窗口的窗口名称*/
extern H_WIN     GUI_WinGetHandFromName(char *name);                                    /*根据窗口名来获取窗口句柄*/
extern void     *GUI_WinGetAttr(H_WIN hWnd);                                            /*得到某个窗口的私有属性*/
extern __s32     GUI_WinSetAttr(H_WIN hWnd, void *attr);                                /*设置某个窗口的私有属性*/
extern unsigned long     GUI_WinGetAddData(H_WIN hWnd);                                         /*得到某个窗口的私有控制信息属性*/
extern __s32     GUI_WinSetAddData(H_WIN hWnd, unsigned long dwAddData);                        /*设置某个窗口的私有控制信息属性*/
extern __u32     GUI_WinGetStyle(H_WIN hWnd);                                           /*获取窗口的风格*/
extern __s32     GUI_WinSetStyle(H_WIN hWnd, __u32 style);                              /*设置窗口风格*/
extern __u32     GUI_WinGetExStyle(H_WIN hWnd);                                         /*获取窗口的扩展风格*/
extern __s32     GUI_WinSetExStyle(H_WIN hWnd, __u32 style);                            /*设置窗口的扩展风格*/
extern unsigned long GUI_CtrlWinGetAddData(H_WIN hWnd);                                     /*获取control的附加数据*/
extern __s32     GUI_CtrlWinSetAddData(H_WIN hWnd, unsigned long dwAddData);                    /*设置control的附加数据*/
extern __bool    GUI_WinIsVisible(H_WIN hWnd);                                          /*判断窗口是否可见*/
extern __s32     GUI_CtrlWinGetClassName(H_WIN hWnd, char *name);                       /*获取控件的类名*/
extern H_WIN     GUI_DlgGetFirstButton(H_WIN hDlg);

/*获取和设置窗口的执行函数*/
extern NOTIFPROC GUI_WinSetNotifyCallback(H_WIN hwnd, NOTIFPROC notif_proc);            /*设置窗口的NotifyCallback函数指针*/
extern NOTIFPROC GUI_WinGetNotifyCallback(H_WIN hwnd);                                  /*获取窗口的NotifyCallback函数指针*/
extern __pGUI_WIN_CB GUI_WinSetCallback(H_WIN hWnd, __pGUI_WIN_CB cb);                  /*设置窗口的callback函数指针*/
extern __pGUI_WIN_CB GUI_WinGetCallback(H_WIN hWnd);                                    /*获取窗口的callback函数指针*/


/*窗口操作函数集*/
extern void      GUI_NotifyParent(__gui_notify_msg_t *msg);                             /*通知父窗口的执行函数*/
extern void      GUI_WinUpdate(H_WIN hWnd, __bool fErase);                              /*更新窗口区域*/
extern __bool    GUI_InvalidateRect(H_WIN hWnd, const RECT *prc, __bool bEraseBkgnd);
extern H_WIN     GUI_WinSetCaptureWin(H_WIN hWnd);                                      /*设置某个窗口为capture窗口*/
extern H_WIN     GUI_WinGetCaptureWin(void);                                            /*获取当前的capture窗口*/
extern void      GUI_WinReleaseCapture(void);                                           /*释放当前的capture窗口*/
extern H_WIN     GUI_WinSetTouchFocus(H_WIN hWnd);                                      /*设置某个窗口为capture窗口*/
extern H_WIN     GUI_WinGetTouchFocus(void);                                            /*获取当前的capture窗口*/
extern void      GUI_WinReleaseTouchFocus(void);                                        /*释放当前的capture窗口*/
extern __s32     GUI_ManWinDefaultProc(__gui_msg_t *msg);
extern __s32     GUI_CtrlWinDefaultProc(__gui_msg_t *msg);
extern __s32     GUI_FrmWinDefaultProc(__gui_msg_t *msg);
extern H_WIN     GUI_GetTouchFocusWin(__s32 x, __s32 y);
extern __bool    GUI_SceenPointInWin(H_WIN hWnd, __s32 x, __s32 y);

/*窗口区域和坐标转换函数集*/
extern __s32     GUI_WinGetClient(H_WIN hWnd, RECT *rect);                              /*获取窗口客户区矩形区域*/
extern __s32     GUI_WinGetWinRECT(H_WIN hWnd, RECT *Win_Rect);                         /*获取窗口的窗口矩形区域*/
extern __s32     GUI_WinSetWinRECT(H_WIN hWnd, RECT *Win_Rect);                /*设置窗口的窗口矩形区域，并不会立即移动窗口
                                                                                        需要等下一次更新窗口时才会移动*/
extern __s32     GUI_WinGetClientRECT(H_WIN hWnd, RECT *Client_Rect);
extern __s32     GUI_WinSetClientRECT(H_WIN hWnd, RECT *Win_Rect);                /*设置窗口的窗口客户区域，并不会立即移动窗口
                                                                                        需要等下一次更新窗口时才会移动*/
extern void      GUI_WinGetClientFBRect(H_WIN hWnd, RECT *fb_rect);
extern void      GUI_WinGetWinFBRect(H_WIN hWnd, RECT *fb_rect);
extern void      GUI_ClientToScreen(H_WIN hWnd, __pos_t *client_pos, __pos_t *sc_pos);
extern void      GUI_ScreenToClient(H_WIN hWnd, __pos_t *sc_pos, __pos_t *client_pos);
extern void      GUI_WindowToScreen(H_WIN hWnd, __pos_t *win_pos, __pos_t *sc_pos);
extern void      GUI_ScreenToWindow(H_WIN hWnd, __pos_t *sc_pos, __pos_t *win_pos);
extern void      GUI_FBToScreen(H_WIN hWnd, __pos_t *fb_pos, __pos_t *sc_pos);
extern void      GUI_ScreenToFB(H_WIN hWnd, __pos_t *sc_pos, __pos_t *fb_pos);

/*对话框相关的函数接口实现*/
extern __s32     GUI_ModuleDialogCreate(__gui_dialogcreate_para_t *create_info);        /*创建一个模式对话框*/
extern __bool    GUI_EndDialog(H_WIN hDlg, __s32 endCode);                              /*退出一个模式对话框*/
extern H_WIN     GUI_GetDlgItem(H_WIN hDlg, __s32 nIDDlgItem);                          /*获取Dialog中的某个Item的句柄*/
extern __s32     GUI_DialogDefaultProc(__gui_msg_t    *msg);                            /*默认的对话框处理函数*/
extern H_WIN     GUI_ModulelessDialogCreate(__gui_dialogcreate_para_t *create_info);    /*创建一个无模式对话框*/
extern __bool    GUI_ModulelessDialogDelete(H_WIN hManWin);                            /*删除一个无模式对话框*/
extern H_WIN     GUI_GetNextDlgTabItem(H_WIN hDlg, H_WIN hCtl, __bool bPrevious);

/*图层窗口的相关操作函数集*/
extern __s32     GUI_LyrWinSetSrcWindow(H_WIN lyrwin, const RECT *rect);
extern __s32     GUI_LyrWinSetScnWindow(H_WIN lyrwin, const RECT *rect);
extern __s32     GUI_LyrWinGetSrcWindow(H_WIN lyrwin, RECT *rect);
extern __s32     GUI_LyrWinGetScnWindow(H_WIN lyrwin, RECT *rect);
extern __s32     GUI_LyrWinGetFB(H_WIN lyrwin, FB *fb);
extern __s32     GUI_LyrWinSetFB(H_WIN lyrwin, const FB *fb);
extern __s32     GUI_LyrWinSetSta(H_WIN lyrwin, __gui_lyrwin_sta_t status);             /*设置某一个图层的状态*/
extern __gui_lyrwin_sta_t GUI_LyrWinGetSta(H_WIN lyrwin);                               /*获取某一个图层的状态*/
extern __s32     GUI_LyrWinSel(H_WIN lyrwin);                                           /*选择某一个图层的操作*/
extern __s32     GUI_LyrWinCacheOn(void);                                               /*打开显示的cache操作*/
extern __s32     GUI_LyrWinCacheOff(void);                                              /*关闭显示的cache操作*/
extern __s32     GUI_LyrWinFlushFB(H_WIN lyrwin);                                       /*刷图层的framgbuffercache*/
extern __s32     GUI_LyrWinSetAlpha(H_WIN lyrwin, __u8 alpha);                          /*设置图层的alpha*/
extern __s32     GUI_LyrWinAlphaOn(H_WIN lyrwin);                                       /*打开图层的alpha*/
extern __s32     GUI_LyrWinAlphaOff(H_WIN lyrwin);                                      /*关闭图层的alpha*/
extern __s32     GUI_LyrWinCKOn(H_WIN lyrwin);                                          /*打开图层的colorkey*/
extern __s32     GUI_LyrWinCKOff(H_WIN lyrwin);                                         /*关闭图层的colorkey*/
extern __s32     GUI_LyrWinSetBottom(H_WIN lyrwin);                                     /*图层置底*/
extern __s32     GUI_LyrWinSetTop(H_WIN lyrwin);                                        /*图层置顶*/

/*timer相关的接口定义*/
extern __bool    GUI_SetTimer(H_WIN hWnd, __u16 id,
                              __u32  speed, __pGUI_TIMERPROC timer_proc);/*设置Timer*/
extern __s32     GUI_KillTimer(H_WIN hWnd, __u16 id);                                  /*删除Timer*/
extern __bool    GUI_ResetTimer(H_WIN hWnd, __u16 id,
                                __u32 speed, __pGUI_TIMERPROC timer_proc);  /*重新设置Timer*/
extern __bool    GUI_IsTimerInstalled(H_WIN hWnd, __u16 id);                            /*判断某一个Timer是否已经设置*/
extern __bool    GUI_HaveFreeTimer(void);                                               /*判断是否还有空闲的Timer存在*/
extern __u32     GUI_GetTickCount(void);                                                /*得到当前tick count数量*/
extern __u32     GUI_GetTimerExpired(H_WIN hWnd, __u16 id);

extern int       GUI_DestroyAutoPanel(void *hdle);
extern void     *GUI_CreateAutoPanel(int id);
extern __s32     GUI_WinInit(void);
extern __s32     GUI_WinExit(void);
extern __s32     GUI_CharSetToEncode(__s32 charset_enm);
extern __hdle    GUI_LongStringCreate(__show_info_move_t *str_move);
extern __s32     GUI_LongStringDelete(__hdle hString);
extern __s32     GUI_LongStringStart(__hdle hString);
extern __s32     GUI_LongStringStop(__hdle hString);
extern __s32     GUI_LongStringReset(__hdle hString, __show_info_move_t *str_move);

extern __s32     GUI_CursorCreate(__gui_cursor_t *create_info);
extern __s32     GUI_CursorDelete(void);
extern __s32     GUI_CursorShow(void);
extern __s32     GUI_CursorHide(void);
extern __s32     GUI_CursorSetPos(__pos_t *pos);
extern __s32     GUI_CursorGetPos(__pos_t *pos);
extern __s32     GUI_CursorSetPara(__gui_cursor_t *para);

extern int       GUI_GetScnDir(void);
extern int       GUI_SetScnDir(__u8  scn_dir);
extern int 		 GUI_SetZoomWin(int src_w,int src_h,int dst_w,int dst_h);
extern __s32     GUI_GetScreenSize(__s32 *p_width, __s32 *p_height);
extern __s32     SysRectToGuiRect(RECT *SysRect, __gui_rect_t *GuiRect);
#endif /* #ifndef _GUI_WIN_H_ */
