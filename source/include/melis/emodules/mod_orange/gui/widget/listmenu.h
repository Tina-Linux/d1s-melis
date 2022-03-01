/*
*******************************************************************************************************************
*                                                           Graphic Library
*                                                   the graphic library support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name:       Listmenu.h
*
*Author:          William Wang
*
*Version :        1.1.0
*
*Date:            2008-10-15
*
*Description :    listmenu data structure and const data definition
*
*Others :         No
* History :
*
* <Author>          <time>         <version>     <description>
*
* William Wang     2008-10-15         1.1.0        Create File
*
*******************************************************************************************************************
*/
#ifndef __LIST__MENU__H____
#define __LIST__MENU__H____

#define CTRL_LISTMENU         ("listmenu")

typedef  __hdle  H_ITEM;

typedef struct tag_DRAW_PARA
{
    H_ITEM                          item;
    __u32                           mode;
    __u32                           aux;
    RECT                            invalidrect;
} __draw_para_t;

typedef __s32(* __pITEM_DRAW_CB)(__draw_para_t *item_para);

typedef struct move_para
{
    __s32     dist;
    __u32     dly_time;
} __move_para_t;

typedef struct __LSTM_ITEM_CTRL
{
    char                            *name;                  /*控件窗口名称*/

    char                             *spClassName;          /*控件窗口的类名称*/
    char                             *spCaption;            /*控件窗口的标题文字*/
    __u32                           dwStyle;                /*控件窗口的风格*/
    __u32                           dwExStyle;              /*控件窗口的扩展风格*/
    __u16                           id;                     /*控件窗口的标识ID*/
    RECT                            ctl_rect;               /*控件窗口在Item中的位置*/
    void                            *attr;                  /*控件窗口的私有信息*/
} __lstm_item_ctrl_t;

//------------------------------------------------------------------------------------------------------------
//Mode  移动的模式，方向等
//Steps 一共移动的steps
//CurStep 指示当前所处的位置的索引。0表示开始状态，等于Steps-1表示结束状态
//返回值表示该次状态的y轴位置与上一次状态y轴位置的偏移量以及移动该偏移量之后的延迟时间(以10ms为单位);
//------------------------------------------------------------------------------------------------------------
typedef __move_para_t   *(*__cb_LSTM_Move_t)(__u32 mode, __u32 Steps, __u32 CurStep);

typedef struct __LSTM_ITEM_ATTR
{
    char                          *name;
    __u32                          item_w_unfocs;              /* item的宽度                                                 */
    __u32                          item_w_focs;                /* item的宽度（焦点）                                         */
    __pITEM_DRAW_CB                cb;                         /* item draw回调函数                                          */
    void                          *p_arg;                      /* 用户自定义的数据内容                                       */
    __u32                          nctrl;                    /* item初始数目                                                 */
    __lstm_item_ctrl_t            *ctrls;                    /* item数组的指针                                               */
} __lstm_item_attr_t;

typedef struct __LSTM_ATTR
{
    __u32                          nitem;                    /* item初始数目                                                 */
    __lstm_item_attr_t            *items;                    /* item数组的指针                                               */

    __cb_LSTM_Move_t               pf_cb_Move;               /* 运动轨迹函数                                                 */
    __u32                          steps;                    /* 运动轨迹函数每运行一格，所需要运行的steps数量                */
    __u32                          mode;                     /* listmenu的工作模式，循环，横竖屏......                       */
    /* bit0   framebuffer模式，循环滚动，线性滚动                   */
    /* bit1~3 framebuffer滚动速度设置                               */
    /* bit3   framebuffer横竖屏模式                                 */
    __u32                          curitemno;                /*初始化焦点item编号*/
    __u32                          bk_color;
} __lstm_para_t;

typedef enum roll_mode
{
    LINEAR_ROLL                    = 0,                            /*线性framebuffer模式                                           */
    CIRCLE_ROLL                    = 1 << 0                        /*循环 framebuffer 模式                                         */
} __roll_mode_e;

typedef enum roll_speed
{
    FAST_SPEED                     = 0 << 1,                       /*中速 滚动                                                     */
    LOW_SPEED                      = 1 << 1,                        /*慢速滚动模式                                                  */
    SPD_LEVEL1                     = 2 << 1,
    SPD_LEVEL2                     = 3 << 1,
    SPD_LEVEL3                     = 4 << 1,
    SPD_LEVEL4                     = 5 << 1,
    SPD_LEVEL5                     = 6 << 1,
    SPD_LEVEL6                     = 7 << 1
} __roll_speed_e;


typedef enum lstm_mode
{
    HORIZONTAL_MOD                = 0 << 4,
    VERITICAL_MOD                 = 1 << 4,
    LISTMENU_MOD                  = 0 << 5,
    LISTBOX_MOD                   = 1 << 5,
    KEYTONE_ON                    = 0 << 6,
    KEYTONE_OFF                   = 1 << 6,
    FOCUS_MOD                     = 0 << 7,
    UNFOCUS_MOD                   = 1 << 7
} __lstm_mode_e;

#define LSTM_DRAWCB_ERROR           0xffff
#define LSTM_DRAWCB_OK              0


#define LSTMN_SELCHANGE             1
#define LSTMN_DBCLCLK               2
#define LSTMN_SELCANCEL             3
#define LSTMN_SETFOCUS              4
#define LSTMN_KILLFOCUS             5
#define LSTMN_CLICKCHECKMARK        6
#define LSTMN_CLICKED               7
#define LSTMN_PUSHED                8
#define LSTMN_UNPUSHED              9

#define LSTMM_USERDEF               (GUI_MSG_CONTROL)
#define LSTMM_RUNUP                 (GUI_MSG_CONTROL + 0)
#define LSTMM_RUNDOWN               (GUI_MSG_CONTROL + 1)
#define LSTMM_RUNLEFT               (GUI_MSG_CONTROL + 2)
#define LSTMM_RUNRIGHT              (GUI_MSG_CONTROL + 3)
#define LSTMM_RUNSTOP               (GUI_MSG_CONTROL + 4)
#define LSTMM_MOVEUP                (GUI_MSG_CONTROL + 5)
#define LSTMM_MOVEDOWN              (GUI_MSG_CONTROL + 6)
#define LSTMM_MOVEMSG               (GUI_MSG_CONTROL + 7)

/*
**********************************************************************************************************************
*                                               FUNCTION
**********************************************************************************************************************
*/
H_ITEM      GUI_LstmGetCurItem(H_WIN lstm);
__u32       GUI_LstmGetItemNo(H_WIN H_Item);
__u32       GUI_LstmSetCurNo(H_WIN lstm, __u32 curno);
__u32       GUI_LstmDrawCurNo(H_WIN lstm, __u32 curno);
__u32       GUI_LstmStopSlip(H_WIN lstm);
H_ITEM      GUI_LstmGetItem(H_WIN lstm, __u32 no);
__s32       GUI_LstmUpdateItem(H_WIN lstm, __u32 no);
__bool      GUI_LstmChildInFb(H_WIN lstm, H_ITEM handle);
__u32       GUI_LstmDelCurItem(H_WIN lstm);
__u32       GUI_LstmAddItemTail(H_WIN lstm, __lstm_item_attr_t *NewItem);
__u32       GUI_LstmGetItemCount(H_WIN lstm);
//__u32       GUI_LstmRegSlaveLyr     (H_WIN lstm,H_LYR lyr);
H_LYR       GUI_LstmGetItemLyrWin(H_WIN H_Item);
__s32       GUI_LstmGetItemRect(H_WIN H_Item, RECT *rect);
unsigned long   GUI_LstmGetItemAttr(H_WIN H_Item);
H_WIN       GUI_LstmGetHostWin(H_WIN H_Item);
__bool      RegisterListMenuControl(void);
#endif
