/*
*******************************************************************************************************************
*                                                           Mod orange
*                                                   the embedded graphic support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º    winclass_private.h
*
*Author£º       William Wang
*
*Version :      1.1.0
*
*Date:          2009-8-6
*
*Description :  windows class private definition
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>       <version>     <description>
*
* William Wang     2009-8-6         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __WINCLASS_PRIVATE_H__
#define __WINCLASS_PRIVATE_H__

#include "winclass_i.h"

__bool          GUI_InitControlClass(void);
void            GUI_TerminateControlClass(void);

pctlclassinfo   GUI_GetControlClassInfo(const char *szClassName);
__s32           GUI_SetControlClassData(__gui_winclass_t *pWndClass);
__s32           GUI_GetControlClassData(__gui_winclass_t *pWndClass);

__s32           GUI_AddNewControlClass(pwinclass pWndClass);
__s32           GUI_DeleteControlClass(const char *szClassName);
void            GUI_EmptyControlClassInfoTable(void);                                             // and return pointer to control hWnd.


#endif
