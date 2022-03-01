/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : monkey.h
* By        : Andy.zhang
* Func      : monkey core interface
* Version   : v1.0
* ============================================================================================================
* 2009-11-3 9:39:42  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __MONKEY_H__
#define __MONKEY_H__

typedef void *H_MKC;

/* 编码格式 */
typedef enum
{
    MK_CHAR_ENCODE_UNKNOWN = 0,
    MK_CHAR_ENCODE_UTF8,
    MK_CHAR_ENCODE_GBK,
    MK_CHAR_ENCODE_UNICODE,
    MK_CHAR_ENCODE_UNICODE_BIG
} MkEncode;

/* 旋转方向 */
typedef enum
{
    MK_ROTATE_0 = 1,
    MK_ROTATE_90,
    MK_ROTATE_180,
    MK_ROTATE_270
} MkRotate;

/* 书签选项 */
typedef enum
{
    MK_BMK_1 = 1,
    MK_BMK_2,
    MK_BMK_3,
    MK_BMK_4,
    MK_BMK_5,
    MK_BMK_6,
    MK_BMK_7,
    MK_BMK_8,
    MK_BMK_
} MkBmk;

/* 字体 */
typedef enum
{
    MK_FONT_0 = 1,
    MK_FONT_1,
    MK_FONT_
} MkFont;

/* 颜色 */
typedef enum
{
    MK_COLOR_BLACK = 1,
    MK_COLOR_BLUE,
    MK_COLOR_GREEN,
    MK_COLOR_RED,
    MK_COLOR_WHITE,
    MK_COLOR_
} MkColor;

/* 页切换模式 */
typedef enum
{
    MK_SWITCH_NO = 1,       // 无切换效果
    MK_SWITCH_GLIDE         // 滑动切换
} MkSwitch;

/* 亮度 */
typedef enum
{
    MK_BRIGHT_0 = 1,
    MK_BRIGHT_1,
    MK_BRIGHT_2,
    MK_BRIGHT_3,
    MK_BRIGHT_4,
    MK_BRIGHT_
} MkBright;

/* 设置背景图片 */
typedef enum
{
    MK_BACK_1 = 1,
    MK_BACK_2,
    MK_BACK_3,
    MK_BACK_4,
    MK_BACK_
} MkBack;

/* 自动切换时间*/
typedef enum
{
    MK_AS_0 = 1,    // 禁止自动切换
    MK_AS_10,       // 10s自动切换
    MK_AS_15,       // 15s自动切换
    MK_AS_20,       // 20s自动切换
    MK_AS_25,       // 25s自动切换
    MK_AS_30        // 30s自动切换
} MkAutoP;

/* 行信息 */
typedef struct
{
    __s32 start;
    __s32 len;
} MkLine;


typedef struct
{
    /* gui 资源 */
    H_LYR       hlyr_1;                                     //文字图层句柄
    H_LYR       hlyr_2;                                     //文字图层句柄
    int         contentLyrWidth;                            //图层宽度
    int         contentLyrHeight;                           //图层高度
    int         sWidth;                                     //屏幕宽度
    int         sHeight;                                    //屏幕高度

    GUI_FONT    *pFont0;                                    //字体句柄1
    GUI_FONT    *pFont1;                                    //字体句柄2

    /* 屏幕属性*/
    __u32       xStart, yStart;                             //左上角坐标
    __u32       xScrWidth, yScrHeight, lineofspace;         //屏幕显示区域
} MkcConfig;


/* 打开句柄*/
H_MKC mkc_open(void);

/* 查询设置信息 */
MkFont   mkc_getFont(H_MKC mCore);      /* 查询当前字体类型 */
MkColor  mkc_getCharColor(H_MKC mCore); /* 查询当前字体颜色 */
MkRotate mkc_getRotate(H_MKC mCore);    /* 查询旋转方向     */
MkSwitch mkc_getSwitch(H_MKC mCore);    /* 查询切换方式     */
MkEncode mkc_getDefEncode(H_MKC mCore); /* 查询默认编码方式 */
MkBright mkc_getBright(H_MKC mCore);    /* 查询背光强度     */
MkAutoP  mkc_getAutoPage(H_MKC mCore);  /* 查询自动翻页时间 */
MkBack   mkc_getBack(H_MKC mCore);      /* 查询背景图片标号 */

/* 查询文件信息 */
__s32  mkc_getFileName(H_MKC mCore, char *fileName);    /* 查询当前打开文件名称*/

/* 查询书签选项 */
__s32  mkc_openBmk(H_MKC mCore,         MkBmk    mBmk);     /* 跳转到指定书签指定位移 */
__s32  mkc_saveBmk(H_MKC mCore,         MkBmk    mBmk);     /* 保存当前偏移量为书签 */
__s32  mkc_getBmkOffset(H_MKC mCore,    MkBmk    mBmk);     /* 查询当前书签偏移百分比, 偏移量单位: 万分之一 */

/* 显示相关 */
__s32  mkc_showConfig(H_MKC mCore,      MkcConfig *para);   /* 配置显示相关属性 */
__s32  mkc_showStart(H_MKC mCore);                          /* 首页, 返回页首偏移量, 偏移量单位: 万分之一  */
__s32  mkc_showNext(H_MKC mCore);                           /* 下一页, 返回页首偏移量, 偏移量单位: 万分之一*/
__s32  mkc_showPrev(H_MKC mCore);                           /* 上一页, 返回页首偏移量, 偏移量单位: 万分之一*/
__s32  mkc_showSeek(H_MKC mCore,        __s32 offset);      /* 跳转功能, 返回实际页首偏移量, 偏移量单位: 万分之一 */

/* 设置选项 */
__s32  mkc_setFont(H_MKC mCore,         MkFont   mFont);    /* 设置字体大小*/
__s32  mkc_setCharColor(H_MKC mCore,    MkColor  mColor);   /* 设置字体颜色*/
__s32  mkc_setRotate(H_MKC mCore,       MkRotate mRotate);  /* 设置旋转角度*/
__s32  mkc_setSwitchMode(H_MKC mCore,   MkSwitch mMode);    /* 设置页切换模式*/
__s32  mkc_setDefEncode(H_MKC mCore,    MkEncode mEncode);  /* 设置默认编码格式*/
__s32  mkc_setBright(H_MKC mCore,       MkBright mBright);  /* 背景亮度 */
__s32  mkc_setAutoPage(H_MKC mCore,     MkAutoP  mAutoP);   /* 自动换页时间 */
__s32  mkc_setBack(H_MKC mCore,         MkBack   mBack);    /* 保存背景图片类型 */

__s32  mkc_getCurPageOffset(H_MKC mCore, __s32 *start, __s32 *end); /* 查询当前页，页头页尾偏移量*/
__bool mkc_isHead(H_MKC mCore);                                     /* 是否到头 */
__bool mkc_isTail(H_MKC mCore);                                     /* 是否到尾 */


/* 删除句柄 */
__s32  mkc_close(H_MKC mCore);

#endif /* __MONKEY_H__ */


