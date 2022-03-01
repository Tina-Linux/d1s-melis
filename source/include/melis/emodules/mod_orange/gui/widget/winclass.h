/*
*******************************************************************************************************************
*                                                           Mod orange
*                                                   the embedded graphic support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    winclass.h
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2009-8-4
*
*Description :  GUI 窗口类的数据结构和常量定义
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>       <version>     <description>
*
* William Wang     2009-8-4         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __GUI_WINCLASS_H__
#define __GUI_WINCLASS_H__

typedef struct _GUI_WNDCLASS
{
    char           *spClassName;            /** 窗口类名 */
    __u32           opMask;                 /*内部操作码*/
    __u32           dwStyle;                /*类的窗口风格*/
    __u32           dwExStyle;              /*窗口类的扩展风格*/
    __pGUI_WIN_CB   WinProc;                /*窗口类的窗口处理函数*/
    __u32           dwAddData;              /*窗口类的附加数据*/
} __gui_winclass_t;

typedef __gui_winclass_t *pwinclass;


extern __s32 GUI_AddNewControlClass(pwinclass  pWndClass);
extern __s32 GUI_SetCtrlClassAddData(const char *szClassName, __u32 dwAddData);
extern void  GUI_EmptyControlClassInfoTable(void);
extern __s32 GUI_GetCtrlClassAddData(const char *szClassName, __u32 *pAddData);
extern __s32 GUI_DeleteControlClass(const char *szClassName);
extern void GUI_TerminateControlClass(void);

#endif
