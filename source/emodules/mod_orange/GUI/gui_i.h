/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    gui_i.h
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2009-08-09
*
*Description :  gui internal data and function definition
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>         <version>     <description>
*
* William Wang     2009-10-19         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef _GUI_I_H_
#define _GUI_I_H_

#include "mod_orange_i.h"
//#include "gui_def.h"
#include "gui_x.h"
#include <kapi.h>
#include <elibs_stdio.h>

//#define ORANGE_WRN __log
#define GUI_SUPPORT_MEMDEV 1
//
#if(GUI_WIN_OS == GUI_OS_EPOS)

/*标准线程互斥接口定义*/
#define  pthread_mutex_t    __krnl_event_t*
#define  sem_t              __krnl_event_t*
#define  flags_t            __krnl_flag_grp_t*
#define  pthread_timer_t    __krnl_stmr_t*
#define  pthread_self()     esKRNL_GetTIDCur()

static inline int sem_init(sem_t *sem, int pshared, unsigned int value)
{
    __u32  cnt      = (__u32)value;
    __u16  user_cnt = (cnt & 0xffff);
    *sem = esKRNL_SemCreate((__u16)user_cnt);
    return ORANGE_OK;
}

static inline int sem_destroy(sem_t *sem)
{
    __u8  err;
    esKRNL_SemDel(*sem, OS_DEL_ALWAYS, &err);
    return ORANGE_OK;
}

static inline int sem_wait(sem_t *sem)
{
    __u8        err;
    esKRNL_SemPend(*sem, 0, &err);
    return ORANGE_OK;
}

static inline int sem_trywait(sem_t *sem)
{
    esKRNL_SemAccept(*sem);
    return ORANGE_OK;
}

static inline int sem_post(sem_t *sem)
{
    esKRNL_SemPost(*sem);
    return ORANGE_OK;
}

static inline int sem_getvalue(sem_t *sem, int *sval)
{
    OS_SEM_DATA                 sem_data;
    esKRNL_SemQuery(*sem, &sem_data);
    *sval   =  sem_data.OSCnt;
    return ORANGE_OK;
}

static inline void pthread_mutex_lock(pthread_mutex_t lock)
{
    __u8        err;
    esKRNL_SemPend(lock, 0, &err);
}

static inline void pthread_mutex_unlock(pthread_mutex_t lock)
{
    esKRNL_SemPost(lock);
}

static inline int pthread_mutex_accept(pthread_mutex_t lock)
{
    return (int)esKRNL_SemAccept(lock);
}

static inline void pthread_mutex_init(pthread_mutex_t *lock, void *ptr)
{
#ifdef CONFIG_ARM
    __u32  cnt      = (__u32)ptr;
#else
    __u64  cnt      = (__u64)ptr;
#endif
    __u16  user_cnt = (cnt & 0xffff);
    *lock = esKRNL_SemCreate((__u16)user_cnt);
    return ;
}

static inline void pthread_mutex_destroy(pthread_mutex_t lock)
{
    __u8  err;
    esKRNL_SemDel(lock, OS_DEL_ALWAYS, &err);
}

static inline int flags_Init(flags_t *flags, int pshared, unsigned int value)
{
    __u8  err;
    *flags = esKRNL_FlagCreate(0x00, &err);
    return ORANGE_OK;
}

static inline int flags_destory(flags_t *flags)
{
    __u8    err;
    esKRNL_FlagDel(*flags, OS_DEL_ALWAYS, &err);
    return ORANGE_OK;
}

static inline int flags_wait(flags_t *flags, unsigned int value)
{
    __u8    err;
    esKRNL_FlagPend(*flags, value, ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) | 0, &err);
    return ORANGE_OK;
}

static inline int flags_post(flags_t *flags, unsigned int value)
{
    __u8    err;
    esKRNL_FlagPost(*flags, value, OS_FLAG_SET, &err);
    return ORANGE_OK;
}

static inline int flags_getvalue(flags_t *flags, unsigned int *value)
{
    __krnl_flags_t    flag_value;
    __u8    err;
    flag_value = esKRNL_FlagQuery(*flags, &err);
    *value     = flag_value;
    return ORANGE_OK;
}

#elif(GUI_WIN_OS == GUI_OS_LINUX)

#elif(GUI_WIN_OS == GUI_OS_WINCE)

#else
#error "gui win os is invalid"
#endif

/*管理窗口类型，根管理窗口和子管理窗口*/
#define   TYPE_ROOTMAIN         0x100
#define   TYPE_CHILDMAIN        0x101

#define   TYPE_LAYERCTL         0x102
#define   TYPE_FRMCTL           0x103

#define   ORANGE_QUEUE_FULL     (-2)

#define   DEF_MSGQUEUE_LEN      128
#define   SIZE_CLIPRECTHEAP     128
#define   SIZE_INVRECTHEAP      128
#define   SIZE_QMSG_HEAP        128

#define   HWND_BACKWIN          0xFFFFFFFF
#define   HWND_INVALID          0x0

#define  GUI_WinH2P(hWnd)       ((__gui_win_t *) hWnd)
#define  GUI_WinP2H(Wnd_Ptr)    ((H_WIN)Wnd_Ptr)
#define  GUI_LyrH2P(hLyr)       ((__gui_lyrwin_t*)hLyr)
#define  GUI_LyrP2H(Lyr_Ptr)    ((H_LYR)Lyr_Ptr)
#define  GUI_P2Handle(ptr)      ((__hdle)ptr)

#ifndef MAX
#define   MAX(x, y)             ((x > y)?x:y)
#endif

#ifndef MIN
#define   MIN(x, y)             ((x < y)?x:y)
#endif

#ifndef ABS
#define   ABS(x)                (((x)<0) ? -(x) : (x))
#endif

typedef struct tagMEMBLOCKLINK
{
    pthread_mutex_t     lock;

    __u32               bd_size;
    __u32               heap_size;
    __u32               free;
    void               *heap;
} mem_blocklink_t;

typedef mem_blocklink_t *pmemblock;

typedef struct _GUI_CLIPRECT
{
    __gui_rect_t            clip_rect;
    struct _GUI_CLIPRECT   *next;
    struct _GUI_CLIPRECT   *prev;
} __gui_cliprect_t;

typedef __gui_cliprect_t *pcliprect;

/* Clipping Region */
#define GUI_NULLREGION      0x00
#define GUI_SIMPLEREGION    0x01
#define GUI_COMPLEXREGION   0x02

typedef struct _GUI_CLIPRGN
{
    __u8                    type;        /* type of region */
    __u8                    reserved[3];

    __gui_rect_t            rcBound;
    pcliprect               head;
    pcliprect               tail;
    pmemblock               heap;
} __gui_clipregn_t;

typedef __gui_clipregn_t *pclipregn;

/*消息队列定义*/
#define DEF_MAXMSG_LEN      128
#define DEF_MAX_QUIT_WIN    32
#define QS_NOTIFYMSG        0x10000000
#define QS_SYNCMSG          0x20000000
#define QS_POSTMSG          0x40000000
#define QS_QUIT             0x80000000
#define QS_INPUT            0x01000000
#define QS_PAINT            0x02000000
#define QS_USERASYNC        0x04000000
#define QS_TIMER            0x0000FFFF
#define QS_EMPTY            0x00000000

/**/
#define POST_KEYDOWN                (1<<15)
#define POST_TOUCHDOWN              (1<<31)
#define POST_KEYCOUNTADD(status)    status (status & 0x7f)

#define PM_NOREMOVE         0x0000
#define PM_REMOVE           0x0001
#define PM_NOYIELD          0x0002

/*定义内部图层使用状态*/
typedef enum GUI_LAYER_INTERSTA
{
    LAYER_DRAW          = (0),             /*图层用作DRAW缓冲，
                                           (没有在其上建立窗口机制)*/
    LAYER_WINDOWS       = (1 << 1),        /*图层上面建立一个独立的实体窗口*/
    LAYER_CONTAINER     = (1 << 2)         /*图层上面建立一套窗口机制，其中窗口可以互相覆盖*/
} __gui_layer_intersts_t;

typedef __gui_msg_t *pmsg;

typedef struct GUI_QMSG
{
    __gui_msg_t            msg;
    struct GUI_QMSG       *next;
} __gui_qmsg_t;

typedef __gui_qmsg_t *pqmsg;

typedef struct GUI_SYNCMSG
{
    __gui_msg_t             msg;
    __s32                   retval;
    pthread_mutex_t         sem_handle;
    struct GUI_SYNCMSG       *pNext;
} __gui_syncmsg_t;

typedef __gui_syncmsg_t     *psyncmsg;

typedef struct WIN_MSGQUEUE
{
    /*消息队列状态*/
    __u32               dwState;
    H_WIN               quit_win[DEF_MAX_QUIT_WIN];
    H_WIN               loop_win[DEF_MAX_QUIT_WIN];
    pthread_mutex_t     lock;
    sem_t               wait;

    /*通知消息队列*/
    __gui_qmsg_t       *pFirstNotifyMsg;
    __gui_qmsg_t       *pLastNotifyMsg;

    /*同步消息队列*/
    __gui_syncmsg_t    *pFirstSyncMsg;
    __gui_syncmsg_t    *pLastSyncMsg;

    /*异步消息队列 */
    __gui_msg_t        *pAsyncMsg;
    __gui_msg_t        *pPaintMsg;
    __u32               len;
    __u8                readpos;
    __u8                writepos;
    __u8                ireadpos;
    __u8                iwritepos;
    __u8                keydown;
    unsigned long       keyvalue;
    __u8                keycount;
    __u8                touchdown;
    __u8                touchcount;
    unsigned long       touchvalue;
    __u8                longcount;
    unsigned long       keyactionvalue;

    /*timer消息处理结构*/
    __u32               FirstTimerSlot;
    __u32               TimerMask;
} __win_msgqueue_t;

typedef __win_msgqueue_t *pmsgqueue;

typedef struct _CTLCLASSINFO
{
    char                        name[GUI_CLASSNAME_MAX + 1];
    // class name
    /*
     * common properties of this class
     */
    __u32                       dwStyle;          // Default control styles.
    __u32                       dwExStyle;        // Default control extended styles.

    __color_t                   iBkColor;         // control background color.

    __pGUI_WIN_CB               ControlProc;
    // control procedure.

    __u32                       dwAddData;            // the additional data.

    __u32                       nUseCount;              // use count.
    struct _CTLCLASSINFO       *next;
    // next class info
} __gui_ctlclass_info_t;

typedef __gui_ctlclass_info_t *pctlclassinfo;

/*窗口的剪切域定义*/
typedef struct __GUI_CLIPINFO
{
    pthread_mutex_t         lock;
    __gui_clipregn_t        crgn;
    __s32                   age;
} __gui_clipinfo_t;

typedef __gui_clipinfo_t   *pclipinfo;

/*窗口的无效区域定义*/
typedef struct __WIN_INVREGN
{
    pthread_mutex_t         lock;
    __gui_clipregn_t        rgn;
    __s32                   frozen;
} __win_invregn_t;

typedef __win_invregn_t *pinvregn;

/*窗口Z序节点定义*/
typedef struct _GUI_ZORDERNODE
{
    H_WIN                   hWnd;          // Handle of window

    struct _GUI_ZORDERNODE *pNext; // Next window
} __gui_zordernode_t;

typedef __gui_zordernode_t *pzordernode;

/*Z序信息定义*/
typedef struct __GUI_ZORDERINFO
{
    __u32                   nNumber;        // Number of windows

    H_WIN                   hWnd;           // Handle of host window

    __gui_zordernode_t     *pTopMost;   // the top most Z order node
} __gui_zorderinfo_t;

typedef __gui_zorderinfo_t *pzorderinfo;


/*滚动条定义*/
typedef struct __SCROLLWINDOWINFO
{
    __u32                   iOffx;
    __u32                   iOffy;
    const __gui_rect_t     *rc1;
    const __gui_rect_t     *rc2;
} __gui_scrollinfo_t;

typedef __gui_scrollinfo_t *pscrollinfo;

#define SBS_NORMAL          0x00
#define SBS_DISABLED        0x01
#define SBS_HIDE            0x02

typedef struct _SCROLLBARINFO
{
    __s32                    minPos;        // min value of scroll range.
    __s32                    maxPos;        // max value of scroll range.
    __s32                    curPos;        // current scroll pos.
    __s32                    pageStep;      // steps per page.
    __s32                    barStart;      // start pixel of bar.
    __s32                    barLen;        // length of bar.
    __u8                     status;        // status of scroll bar.
} __gui_scrollbarinfo_t;

typedef __gui_scrollbarinfo_t *pscrollbarinfo;

typedef struct tag__GUI_LYRWIN
{
    char                        name[GUI_NAME_MAX];   /* 窗口名称*/

    H_LYR                       h_swlyr;              /* sw layer handle                                              */
    __u8                        type;

    H_LYR                       h_uselyr;             /*关联的图层句柄，在Ex模式下有用，用来表示该图层强占某一图层的硬件句柄*/
    H_LYR                       h_hwlyr;              /* hw layer                                                     */
    H_LYR                       h_exlyr;
    __disp_layer_para_t         lyrpara;
    __mp                        *devfp;

    __gui_lyrwin_sta_t          status;                //图层的状态信息
    __gui_layer_intersts_t      use_sts;               //图层的使用状态信息

    /* 剪切域 */
    __gui_clipinfo_t           *pGCRInfo;             // 窗口的剪切域信息.
    __gui_clipinfo_t            GCRInfo;              //剪切域信息结构

    /* z序*/
    __gui_zorderinfo_t          ManWinZOrder;         //图层的通用窗口Z序列表
    __gui_zorderinfo_t          TopMostWinZOrder;     //图层的最顶窗口的Z序列表

    struct tag__GUI_LYRWIN      *Next;
} __gui_lyrwin_t;


/* window handler                                   */
typedef struct tag__GUI_WIN
{
    char                         name[GUI_NAME_MAX];   /* 窗口名称*/

    __u16                        DataType;              /* 窗口的数据类型，用来判断窗口句柄是否有效.*/
    __u16                        WinType;               /* 窗口类型*/

    __u32                        dwStyle;               /* 窗口的style.*/
    __u32                        dwExStyle;            /* 窗口的扩展style*/

    /* 该主窗口所属图层窗口句柄 */
    __gui_lyrwin_t               *LyrWin;
    /* window tree */
    struct tag__GUI_WIN          *Parent;              /* 窗口的父窗口指针. */
    struct tag__GUI_WIN          *FirstChild;          /* 第一个子窗口的句柄*/
    struct tag__GUI_WIN          *Next;                /* 窗口的下一个兄弟窗口*/                                       // the next sibling control.
    struct tag__GUI_WIN          *Prev;                /* 窗口的前一个兄弟窗口*/                                 // the prev sibling control.
    struct tag__GUI_WIN          *ActiveChild;         /* 当前激活的子窗口的句柄*/
    struct tag__GUI_WIN          *OldUnderPointer;     /* 上一次子窗口触摸屏下的子窗口句柄*/
    struct tag__GUI_WIN          *Primitive;           /* the premitive child of mouse event.*/

    __u32                         id;                   // 窗口ID

    __gui_rect_t                  win_rect;             // 窗口在图层(或者framwin)中的矩形区域
    __gui_rect_t                  client_rect;          // 窗口的客户区矩形区域

    __color_t                     bkColor;              // 窗口的背景色.
    GUI_FONT                      *pLogFont;             // 窗口的字体.

    __win_invregn_t               InvRgn;               // 窗口的无效区信息.
    __gui_clipinfo_t             *pGCRInfo;             // 窗口的剪切域信息.
    __gui_zordernode_t           *pZOrderNode;          // 窗口的Z序节点

    __gui_clipinfo_t              GCRInfo;              // 剪切域信息结构

    __pGUI_WIN_CB                 WindowProc;           // 窗口的消息处理函数
    NOTIFPROC                     NotifProc;            // 通知消息处理函数.

    __gui_scrollbarinfo_t         vscroll;              // 窗口的滚动条信息
    __gui_scrollbarinfo_t         hscroll;

    unsigned long                 dwAddData;            // 用户传递窗口时需要传递的数据
    unsigned long                 dwCtrlData;           // 用户保存的相应窗口控制信息的数据结构
    //(重载窗口时需要控制信息时使用)
} __gui_win_t;

typedef struct __MANWIN
{
    __gui_win_t                   base_win;            //基本窗口信息结构

    __u16                         ManWinType;         //管理窗口子类型  根主窗口类型或者子管理窗口类型

    __win_msgqueue_t             *pMessages;           //消息队列

    __u32                          threadid;               //主窗口处理消息队列的线程ID

    __gui_win_t                  *pHosting;            //主窗口的宿主窗口

    __gui_win_t                  *pFirstHosted;        //主窗口的第一个hosted窗口句柄

    __gui_win_t                  *pNextHosted;         //主窗口的下一个hosted窗口

    __gui_win_t                  *pRootMain;           //主窗口的root主窗口
} __gui_manwin_t;

typedef struct tag__GUI_FRMWIN
{
    __gui_win_t                   base_win;            //基本窗口信息结构

    __gui_manwin_t               *pManWin;             //指向主窗口的指针

    __gui_win_t                  *pHosting;            //framewin的宿主窗口

    __gui_win_t                  *pFirstHosted;        //framewin的第一个hosted窗口句柄

    __gui_win_t                  *pNextHosted;         //framewin的下一个hosted窗口
} __gui_frmwin_t;


typedef __gui_win_t               *pguiwin;
typedef __gui_lyrwin_t            *pguilayer;
typedef __gui_manwin_t           *pmanwin;
typedef __gui_frmwin_t            *pframewin;


typedef struct __WIDGET
{
    __gui_win_t                     base_win;

    __u16                           CtrlWinType;         //控件窗口子类型  图层窗口类型或者通用窗口类型

    __gui_frmwin_t                  *pFrameWin;          // control的framewin句柄

    __gui_ctlclass_info_t           *pcci;               // pointer to Control Class Info struct.

    unsigned long                   dwAddData;           /*control控件自身的私有数据*/
} __gui_widget_t;

typedef __gui_widget_t              *pwidget;

extern __gui_rect_t         g_rcScr;;
extern __u8                 gui_scndir;
extern __bool               InitMsgQueue(__win_msgqueue_t *qmsg, __u32  buf_len);
extern void                 DestroyMsgQueue(__win_msgqueue_t *qmsg);
extern __win_msgqueue_t    *GUI_WinGetMsgQueue(H_WIN hWnd);
extern void                 GUI_PhyPt2LogPt(__u8 direct, GUI_POINT *pPhyPoint, GUI_POINT *pLogPoint);
extern void                 GUI_LogPt2PhyPt(__u8 direct, GUI_POINT *pLogPoint, GUI_POINT *pPhyPoint);

#ifndef GUI_MAXTASK
#define GUI_MAXTASK           (256)
#endif

#ifndef GUI_MAX_LOCK_NESTING
#define GUI_MAX_LOCK_NESTING  48   /* If a lot of nested dialogs are used, this may not be sufficient. */
#endif

#define DEF_NR_TIMERS           32

/*GUI系统timer间隔(10ms)*/
#define DEF_TIMER_PEROID        2

/*GUI嵌套锁数据结构*/
typedef struct __GUI_NESTLOCK
{
    pthread_mutex_t     lock;
    U32                 TaskID[GUI_MAXTASK];
    int                 _CurrentTaskNo;
    int                 _EntranceCnt;
    U32                 _TaskIDLock;
    void               *TaskContext[GUI_MAXTASK];
    __u32               ContextSize;
} __gui_nestlock_t;

typedef struct __GUI_TIMER
{
    H_WIN               hWnd;
    __u16               id;
    __u32               speed;
    __u32               count;
    __u32               expired;

    __pGUI_TIMERPROC    proc;
    __u32               tick_count;
    __win_msgqueue_t    *msg_queue;
} __gui_timer_t;

typedef __gui_timer_t *ptimer;

void    GUI_ScnPhyPt2LogPt(__u8 direct, GUI_POINT *pPhyPoint, GUI_POINT *pLogPoint);
void    GUI_ScnLogPt2PhyPt(__u8 direct, GUI_POINT *pLogPoint, GUI_POINT *pPhyPoint);
__s16   GUI_TouchLogDirToPhyDir(__u8 direct, __s16 LogDir);
__s16   GUI_TouchPhyDirToLogDir(__u8 direct, __s16 PhyDir);

#include "common/common_e.h"
#include "gui_e.h"
#include "widget/winclass_e.h"
#include "kernel/GUI_Kernel_e.h"

#endif  /* _GUI_PRIVATE_H_ */





