/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, DEREK, China
*                                            All Rights Reserved
*
* File          : orange_api_entry.h
* By            : derek.lai
* Version   : v1.0
* ============================================================================================================
*/

#ifndef __MOD_ORANGE_API_H__
#define __MOD_ORANGE_API_H__
#include <kconfig.h>
#include <mod_orange.h>

typedef struct ORANGEMOD_API_ENTRY
{
    H_WIN(*ptr_GUI_LyrWinCreate)(playerwincreate create_info);                                       /*创建图层窗口*/
    __s32(*ptr_GUI_LyrWinDelete)(H_WIN  hlayer);                                                     /*删除图层窗口*/
    void (*ptr_GUI_WinThreadCleanup)(H_WIN hManWnd);
    H_WIN(*ptr_GUI_WinSetFocusChild)(H_WIN hWnd);
    H_WIN(*ptr_GUI_WinGetFocusChild)(H_WIN hWnd);                                                   /*获取某个窗口的焦点子窗口*/
    H_WIN(*ptr_GUI_WinSetActiveChild)(H_WIN hWnd);
    __s32(*ptr_GUI_WinGetType)(H_WIN hWnd);                                                         /*获取某个窗口的窗口类型*/
    H_WIN(*ptr_GUI_WinGetLyrWin)(H_WIN hWnd);                                                       /*获取某个窗口的图层句柄*/
    H_WIN(*ptr_GUI_WinGetMainManWin)(H_WIN hWnd);                                                   /*获取主管理窗口的窗口句柄*/
    H_WIN(*ptr_GUI_WinGetManWin)(H_WIN hWnd);                                                       /*获取窗口的主窗口句柄*/
    H_WIN(*ptr_GUI_WinGetFrmWin)(H_WIN hWnd);                                                       /*获取某个窗口的framewin句柄*/
    H_WIN(*ptr_GUI_WinGetParent)(H_WIN hWnd);                                                       /*得到某个窗口的父窗口*/
    H_WIN(*ptr_GUI_WinGetFirstChild)(H_WIN hWnd);                                                   /*得到某个窗口的第一个子窗口*/
    H_WIN(*ptr_GUI_WinGetNextBro)(H_WIN hWnd);                                                      /*得到某个窗口的兄弟窗口*/
    __bool(*ptr_GUI_WinIsAncestor)(H_WIN hWnd, H_WIN hChild);
    __bool(*ptr_GUI_WinIsChild)(H_WIN HWnd, H_WIN hParent);
    H_WIN(*ptr_GUI_WinGetRootWin)(void);                                                            /*得到根窗口的窗口句柄*/
    H_WIN(*ptr_GUI_WinGetNextHostedWin)(H_WIN  hWnd);
    H_WIN(*ptr_GUI_WinGetFirstHostedWin)(H_WIN  hWnd);
    H_WIN(*ptr_GUI_WinGetOwnerWin)(H_WIN  hWnd);
    H_WIN(*ptr_GUI_WinGetDlgItem)(H_WIN hDlg, __s32 nIDDlgItem);                                    /*获取一个frmwin的窗口矩形区域*/
    __u32(*ptr_GUI_WinGetItemId)(H_WIN hItem);                                                      /*根据窗口的句柄获取窗口的ID*/
    H_WIN(*ptr_GUI_WinGetPrimitiveWin)(H_WIN hWnd);
    H_WIN(*ptr_GUI_WinGetHandFromId)(H_WIN hParent, __s32 nID);
    __s32(*ptr_GUI_WinGetName)(H_WIN hWnd, char *name);                                             /*得到某个窗口的窗口名称*/
    H_WIN(*ptr_GUI_WinGetHandFromName)(char *name);                                                 /*根据窗口名来获取窗口句柄*/
    void             *(*ptr_GUI_WinGetAttr)(H_WIN hWnd);                                            /*得到某个窗口的私有属性*/
    __s32(*ptr_GUI_WinSetAttr)(H_WIN hWnd, void *attr);                                             /*设置某个窗口的私有属性*/
    unsigned long (*ptr_GUI_WinGetAddData)(H_WIN hWnd);                                                      /*得到某个窗口的私有控制信息属性*/
    __s32(*ptr_GUI_WinSetAddData)(H_WIN hWnd, unsigned long dwAddData);                                     /*设置某个窗口的私有控制信息属性*/
    __u32(*ptr_GUI_WinGetStyle)(H_WIN hWnd);                                                        /*获取窗口的风格*/
    __s32(*ptr_GUI_WinSetStyle)(H_WIN hWnd, __u32 style);                                           /*设置窗口风格*/
    __u32(*ptr_GUI_WinGetExStyle)(H_WIN hWnd);                                                      /*获取窗口的扩展风格*/
    __s32(*ptr_GUI_WinSetExStyle)(H_WIN hWnd, __u32 style);                                         /*设置窗口的扩展风格*/
    __bool(*ptr_GUI_WinIsVisible)(H_WIN hWnd);                                                      /*判断窗口是否可见*/
    NOTIFPROC(*ptr_GUI_WinSetNotifyCallback)(H_WIN hwnd, NOTIFPROC notif_proc);
    NOTIFPROC(*ptr_GUI_WinGetNotifyCallback)(H_WIN hwnd);
    __pGUI_WIN_CB(*ptr_GUI_WinSetCallback)(H_WIN hWnd, __pGUI_WIN_CB cb);                       /*设置窗口的callback函数指针*/
    __pGUI_WIN_CB(*ptr_GUI_WinGetCallback)(H_WIN hWnd);                                         /*获取窗口的callback函数指针*/
    void (*ptr_GUI_NotifyParent)(__gui_notify_msg_t *msg);                                          /*通知父窗口的执行函数*/
    void (*ptr_GUI_WinUpdate)(H_WIN hWnd, __bool fErase);                                           /*更新窗口区域*/
    __bool(*ptr_GUI_InvalidateRect)(H_WIN hWnd, const RECT *prc, __bool bEraseBkgnd);
    H_WIN(*ptr_GUI_WinSetCaptureWin)(H_WIN hWnd);                                                   /*设置某个窗口为capture窗口*/
    H_WIN(*ptr_GUI_WinGetCaptureWin)(void);                                                         /*获取当前的capture窗口*/
    void (*ptr_GUI_WinReleaseCapture)(void);                                                        /*释放当前的capture窗口*/
    H_WIN(*ptr_GUI_WinSetTouchFocus)(H_WIN hWnd);                                                   /*设置某个窗口为capture窗口*/
    H_WIN(*ptr_GUI_WinGetTouchFocus)(void);                                                         /*获取当前的capture窗口*/
    void (*ptr_GUI_WinReleaseTouchFocus)(void);                                                     /*释放当前的capture窗口*/
    __s32(*ptr_GUI_WinGetClientRECT)(H_WIN hWnd, RECT *Client_Rect);
    __s32(*ptr_GUI_WinSetClientRECT)(H_WIN hWnd, RECT *Win_Rect);
    __s32(*ptr_GUI_WinGetClient)(H_WIN hWnd, RECT *rect);                                           /*获取窗口客户区矩形区域*/
    __s32(*ptr_GUI_WinGetWinRECT)(H_WIN hWnd, RECT *Win_Rect);                                      /*获取窗口的窗口矩形区域*/
    __s32(*ptr_GUI_WinSetWinRECT)(H_WIN hWnd, RECT *Win_Rect);
    void (*ptr_GUI_WinGetClientFBRect)(H_WIN hWnd, RECT *fb_rect);
    void (*ptr_GUI_WinGetWinFBRect)(H_WIN hWnd, RECT *fb_rect);
    void (*ptr_GUI_ClientToScreen)(H_WIN hWnd, __pos_t *client_pos, __pos_t *sc_pos);
    void (*ptr_GUI_ScreenToClient)(H_WIN hWnd, __pos_t *sc_pos, __pos_t *client_pos);
    void (*ptr_GUI_WindowToScreen)(H_WIN hWnd, __pos_t *win_pos, __pos_t *sc_pos);
    void (*ptr_GUI_ScreenToWindow)(H_WIN hWnd, __pos_t *sc_pos, __pos_t *win_pos);
    void (*ptr_GUI_FBToScreen)(H_WIN hWnd, __pos_t *fb_pos, __pos_t *sc_pos);
    void (*ptr_GUI_ScreenToFB)(H_WIN hWnd, __pos_t *sc_pos, __pos_t *fb_pos);
    __s32(*ptr_GUI_LyrWinSetSrcWindow)(H_WIN lyrwin, const RECT *rect);
    __s32(*ptr_GUI_LyrWinSetScnWindow)(H_WIN lyrwin, const RECT *rect);
    __s32(*ptr_GUI_LyrWinGetSrcWindow)(H_WIN lyrwin, RECT *rect);
    __s32(*ptr_GUI_LyrWinGetScnWindow)(H_WIN lyrwin, RECT *rect);
    __s32(*ptr_GUI_LyrWinGetFB)(H_WIN lyrwin, FB *fb);
    __s32(*ptr_GUI_LyrWinSetFB)(H_WIN lyrwin, const FB *fb);
    __s32(*ptr_GUI_LyrWinSetSta)(H_WIN lyrwin, __gui_lyrwin_sta_t status);                           /*设置某一个图层的状态*/
    __gui_lyrwin_sta_t(*ptr_GUI_LyrWinGetSta)(H_WIN lyrwin);                               /*获取某一个图层的状态*/
    __s32(*ptr_GUI_LyrWinSel)(H_WIN lyrwin);                                                         /*选择某一个图层的操作*/
    __s32(*ptr_GUI_LyrWinCacheOn)(void);                                                             /*打开显示的cache操作*/
    __s32(*ptr_GUI_LyrWinCacheOff)(void);                                                        /*关闭显示的cache操作*/
    __s32(*ptr_GUI_LyrWinFlushFB)(H_WIN lyrwin);                                                 /*刷图层的framgbuffercache*/
    __s32(*ptr_GUI_LyrWinSetAlpha)(H_WIN lyrwin, __u8 alpha);                                    /*设置图层的alpha*/
    __s32(*ptr_GUI_LyrWinAlphaOn)(H_WIN lyrwin);                                                 /*打开图层的alpha*/
    __s32(*ptr_GUI_LyrWinAlphaOff)(H_WIN lyrwin);                                                    /*关闭图层的alpha*/
    __s32(*ptr_GUI_LyrWinCKOn)(H_WIN lyrwin);                                                        /*打开图层的colorkey*/
    __s32(*ptr_GUI_LyrWinCKOff)(H_WIN lyrwin);                                                   /*关闭图层的colorkey*/
    __s32(*ptr_GUI_LyrWinSetBottom)(H_WIN lyrwin);                                               /*图层置底*/
    __s32(*ptr_GUI_LyrWinSetTop)(H_WIN lyrwin);                                                      /*图层置顶*/
    __bool(*ptr_GUI_SetTimer)(H_WIN hWnd, __u16 id, __u32  speed, __pGUI_TIMERPROC timer_proc);
    __s32(*ptr_GUI_KillTimer)(H_WIN hWnd, __u16 id);                                               /*删除Timer*/
    __bool(*ptr_GUI_ResetTimer)(H_WIN hWnd, __u16 id, __u32 speed, __pGUI_TIMERPROC timer_proc);
    __bool(*ptr_GUI_IsTimerInstalled)(H_WIN hWnd, __u16 id);                                        /*判断某一个Timer是否已经设置*/
    __bool(*ptr_GUI_HaveFreeTimer)(void);                                                           /*判断是否还有空闲的Timer存在*/
    __u32(*ptr_GUI_GetTickCount)(void);                                                             /*得到当前tick count数量*/
    __u32(*ptr_GUI_GetTimerExpired)(H_WIN hWnd, __u16 id);
    __s32(*ptr_GUI_WinInit)(void);
    __s32(*ptr_GUI_WinExit)(void);
    __s32(*ptr_GUI_CharSetToEncode)(__s32 charset_enm);
    __s32(*ptr_GUI_CursorCreate)(__gui_cursor_t *create_info);
    __s32(*ptr_GUI_CursorDelete)(void);
    __s32(*ptr_GUI_CursorShow)(void);
    __s32(*ptr_GUI_CursorHide)(void);
    __s32(*ptr_GUI_CursorSetPos)(__pos_t *pos);
    __s32(*ptr_GUI_CursorGetPos)(__pos_t *pos);
    __s32(*ptr_GUI_CursorSetPara)(__gui_cursor_t *para);
    int (*ptr_GUI_GetScnDir)(void);
    int (*ptr_GUI_SetScnDir)(__u8  scn_dir);
	int (*ptr_GUI_SetZoomWin)(int src_w,int src_h,int dst_w,int dst_h);    
    __s32(*ptr_GUI_GetScreenSize)(__s32 *p_width, __s32 *p_height);
    __s32(*ptr_SysRectToGuiRect)(RECT *SysRect, __gui_rect_t *GuiRect);
    void (*ptr_GUI_Lock)(void);
    void (*ptr_GUI_Unlock)(void);
    __s32(*ptr_GUI_LockAccept)(void);
    void (*ptr_GUITASK_Init)(void);
    void (*ptr_GUITASK_CopyContext)(void);
    int (*ptr_GUI_Init_layer)(void);
    void (*ptr_GUI_SetDefault)(void);
    GUI_DRAWMODE(*ptr_GUI_SetDrawMode)(GUI_DRAWMODE dm);
    GUI_FONTMODE(*ptr_GUI_SetFontMode)(GUI_FONTMODE fm);
    void (*ptr_GUI_SaveContext)(GUI_CONTEXT *pContext);
    void (*ptr_GUI_RestoreContext)(const GUI_CONTEXT *pContext);
    void (*ptr_GUI_OpenAlphaBlend)(void);
    void (*ptr_GUI_CloseAlphaBlend)(void);
    int (*ptr_GUI_RectsIntersect)(const GUI_RECT *pr0, const GUI_RECT *pr1);
    void (*ptr_GUI_MoveRect)(GUI_RECT *pRect, int x, int y);
    void (*ptr_GUI_MergeRect)(GUI_RECT *pDest, const GUI_RECT *pr0, const GUI_RECT *pr1);
    int (*ptr_GUI__IntersectRects)(GUI_RECT *pDest, const GUI_RECT *pr0, const GUI_RECT *pr1);
    void (*ptr_GUI__IntersectRect)(GUI_RECT *pDest, const GUI_RECT *pr0);
    const GUI_RECT   *(*ptr_GUI_SetClipRect)(const GUI_RECT *pRect);     //mcl  2008.12.18
    I32(*ptr_GUI__DivideRound32)(I32 a, I32 b);
    U8(*ptr_GUI_GetPenSize)(void);
    U8(*ptr_GUI_GetPenShape)(void);
    U8(*ptr_GUI_GetLineStyle)(void);
    void (*ptr_GUI_SetBkColor)(GUI_COLOR);
    void (*ptr_GUI_SetColor)(GUI_COLOR);
    U8(*ptr_GUI_SetPenSize)(U8 Size);
    U8(*ptr_GUI_SetPenShape)(U8 Shape);
    U8(*ptr_GUI_SetLineStyle)(U8 Style);
    void (*ptr_GUI_Log)(const char *s);
    void (*ptr_GUI_Log1)(const char *s, int p0);
    void (*ptr_GUI_Log2)(const char *s, int p0, int p1);
    void (*ptr_GUI_Log3)(const char *s, int p0, int p1, int p2);
    void (*ptr_GUI_Log4)(const char *s, int p0, int p1, int p2, int p3);
    void (*ptr_GUI_Warn)(const char *s);
    void (*ptr_GUI_Warn1)(const char *s, int p0);
    void (*ptr_GUI_Warn2)(const char *s, int p0, int p1);
    void (*ptr_GUI_Warn3)(const char *s, int p0, int p1, int p2);
    void (*ptr_GUI_Warn4)(const char *s, int p0, int p1, int p2, int p3);
    void (*ptr_GUI_ErrorOut)(const char *s);
    void (*ptr_GUI_ErrorOut1)(const char *s, int p0);
    void (*ptr_GUI_ErrorOut2)(const char *s, int p0, int p1);
    void (*ptr_GUI_ErrorOut3)(const char *s, int p0, int p1, int p2);
    void (*ptr_GUI_ErrorOut4)(const char *s, int p0, int p1, int p2, int p3);
    int (*ptr_GUI_BMP_Draw)(const void *pFileData, int x0, int y0);
    int (*ptr_GUI_BitString_Draw)(FB *fb, int x0, int y0);                    // mcl add for fast draw ARGB 2008.11.18
    int (*ptr_GUI_BitString_DrawEx)(FB *fb, int x0, int y0);                 // mcl add for accelerate drawing speed
    int (*ptr_GUI_ARGB_Draw)(const void *pBMP, int x0, int y0);                // mcl add 2008.12.04
    int (*ptr_GUI_BMP_Draw_Trans)(const void *pBMP, int x0, int y0, GUI_COLOR  color, U8 index);               //added by MCL  for bmp trans
    int (*ptr_GUI_BMP_GetXSize)(const void *pFileData);
    int (*ptr_GUI_BMP_GetYSize)(const void *pFileData);
    void (*ptr_GUI_Clear)(void);
    void (*ptr_GUI_ClearRect)(int x0, int y0, int x1, int y1);
    void (*ptr_GUI_ClearRectEx)(const GUI_RECT *pRect);
    void (*ptr_GUI_DrawArc)(int x0, int y0, int rx, int ry, int a0, int a1);
    void (*ptr_GUI_DrawCircle)(int x0, int y0, int r);
    void (*ptr_GUI_DrawEllipse)(int x0, int y0, int rx, int ry);
    void (*ptr_GUI_DrawGraph)(I16 *pay, int NumPoints, int x0, int y0);
    void (*ptr_GUI_DrawHLine)(int y0, int x0, int x1);
    void (*ptr_GUI_DrawLine)(int x0, int y0, int x1, int y1);
    void (*ptr_GUI_DrawLineRel)(int dx, int dy);
    void (*ptr_GUI_DrawLineTo)(int x, int y);
    void (*ptr_GUI_DrawPie)(int x0, int y0, int r, int a0, int a1, int Type);
    void (*ptr_GUI_DrawPixel)(int x, int y);
    void (*ptr_GUI_DrawPoint)(int x, int y);
    void (*ptr_GUI_DrawPolygon)(const GUI_POINT *pPoints, int NumPoints, int x0, int y0);
    void (*ptr_GUI_DrawRect)(int x0, int y0, int x1, int y1);
    void (*ptr_GUI_DrawRectEx)(const GUI_RECT *pRect);
    void (*ptr_GUI_DrawVLine)(int x0, int y0, int y1);
    void (*ptr_GUI_FillCircle)(int x0, int y0, int r);
    void (*ptr_GUI_FillEllipse)(int x0, int y0, int rx, int ry);
    void (*ptr_GUI_FillPolygon)(const GUI_POINT *pPoints, int NumPoints, int x0, int y0);
    void (*ptr_GUI_FillRect)(int x0, int y0, int x1, int y1);
    void (*ptr_GUI_FillRectEx)(const GUI_RECT *pRect);
    void (*ptr_GUI_InvertRect)(int x0, int y0, int x1, int y1);
    void (*ptr_GUI_MoveRel)(int dx, int dy);
    void (*ptr_GUI_MoveTo)(int x, int y);
    void (*ptr_GUI_SetRectMem)(GUI_RECT *pDestRect, void *pBuffer, U32 bufferXSize, U32 bufferYSize, U32 rectX0, U32 rectY0);
    void (*ptr_GUI_GetRectMem)(GUI_RECT *pSrc, void *pBuffer, U32 bufferXSize, U32 bufferYSize, U32 rectX0, U32 rectY0);
    void (*ptr_GUI_DispCEOL)(void);
    void (*ptr_GUI_DispChar)(U16 c);
    void (*ptr_GUI_DispChars)(U16 c, int Cnt);
    void (*ptr_GUI_DispCharAt)(U16 c, I16P x, I16P y);
    void (*ptr_GUI_DispString)(const char GUI_UNI_PTR *s);
    void (*ptr_GUI_DispStringAt)(const char GUI_UNI_PTR *s, int x, int y);
    void (*ptr_GUI_DispStringAtCEOL)(const char GUI_UNI_PTR *s, int x, int y);
    void (*ptr_GUI_DispStringHCenterAt)(const char GUI_UNI_PTR *s, int x, int y);
    void (*ptr_GUI__DispStringInRect)(const char GUI_UNI_PTR *s, GUI_RECT *pRect, int TextAlign, int MaxNumChars);
    void (*ptr_GUI_DispStringInRect)(const char GUI_UNI_PTR *s, GUI_RECT *pRect, int Flags);
    void (*ptr_GUI_DispStringInRectMax)(const char GUI_UNI_PTR *s, GUI_RECT *pRect, int TextAlign, int MaxLen);              /* Not to be doc. */
    void (*ptr_GUI_DispStringLen)(const char GUI_UNI_PTR *s, int Len);
    __s32(*ptr_GUI_DispStringInRectWrap)(const char GUI_UNI_PTR *s, GUI_RECT *pRect, int TextAlign, GUI_WRAPMODE WrapMode);                   //mcl add
    void (*ptr_GUI_GetTextExtend)(GUI_RECT *pRect, const char GUI_UNI_PTR *s, int Len);
    int (*ptr_GUI_GetYAdjust)(void);
    const GUI_FONT   *(*ptr_GUI_GetFont)(void);
    int (*ptr_GUI_GetCharDistX)(U16 c);
    int (*ptr_GUI_GetStringDistX)(const char GUI_UNI_PTR *s);
    int (*ptr_GUI_GetFontDistY)(void);
    int (*ptr_GUI_GetFontSizeY)(void);
    void (*ptr_GUI_GetFontInfo)(const GUI_FONT GUI_UNI_PTR *pFont, GUI_FONTINFO *pfi);
    int (*ptr_GUI_GetYSizeOfFont)(const GUI_FONT GUI_UNI_PTR *pFont);
    int (*ptr_GUI_GetYDistOfFont)(const GUI_FONT GUI_UNI_PTR *pFont);
    int (*ptr_GUI_GetTextAlign)(void);
    int (*ptr_GUI_GetTextMode)(void);
    char(*ptr_GUI_IsInFont)(const GUI_FONT GUI_UNI_PTR *pFont, U16 c);
    int (*ptr_GUI_SetTextAlign)(int Align);
    int (*ptr_GUI_SetTextMode)(int Mode);
    const GUI_FONT   *(*ptr_GUI_SetFont)(const GUI_FONT GUI_UNI_PTR *pNewFont);
    char(*ptr_GUI_GotoXY)(int x, int y);
    char(*ptr_GUI_GotoX)(int x);
    char(*ptr_GUI_GotoY)(int y);
    void (*ptr_GUI_DispNextLine)(void);
    GUI_FONT         *(*ptr_GUI_GetDefaultFont)(void);
    int (*ptr_GUI_UC_GetCharSize)(const char GUI_UNI_PTR *s);
    U16(*ptr_GUI_UC_GetCharCode)(const char GUI_UNI_PTR *s);
    U16(*ptr_Big5_to_Unicode)(U16 big5);
    void (*ptr_GUI_UC_SetEncodeNone)(void);
    void (*ptr_GUI_UC_SetEncodeUTF8)(void);
    void (*ptr_GUI_UC_SetEncodeGB2312)(void);
    void (*ptr_GUI_UC_SetEncodeGBK)(void);
    void (*ptr_GUI_UC_SetEncodeBig5)(void);
    void (*ptr_GUI_UC_SetEncodeSJIS)(void);
    void (*ptr_GUI_UC_SetEncodeISO885916)(void);
    void (*ptr_GUI_UC_SetEncodeISO885915)(void);
    void (*ptr_GUI_UC_SetEncodeISO885914)(void);
    void (*ptr_GUI_UC_SetEncodeISO885913)(void);
    void (*ptr_GUI_UC_SetEncodeISO885912)(void);
    void (*ptr_GUI_UC_SetEncodeISO885911)(void);
    void (*ptr_GUI_UC_SetEncodeISO885910)(void);
    void (*ptr_GUI_UC_SetEncodeISO88599)(void);
    void (*ptr_GUI_UC_SetEncodeISO88598)(void);
    void (*ptr_GUI_UC_SetEncodeISO88597)(void);
    void (*ptr_GUI_UC_SetEncodeISO88596)(void);
    void (*ptr_GUI_UC_SetEncodeISO88595)(void);
    void (*ptr_GUI_UC_SetEncodeISO88594)(void);
    void (*ptr_GUI_UC_SetEncodeISO88593)(void);
    void (*ptr_GUI_UC_SetEncodeISO88592)(void);
    void (*ptr_GUI_UC_SetEncodeISO88591)(void);
    void (*ptr_GUI_UC_SetEncodeKIO8R)(void);
    void (*ptr_GUI_UC_SetEncodeEUCKR)(void);
    void (*ptr_GUI_UC_SetEncodeEUCJP)(void);
    void (*ptr_GUI_UC_SetEncodeCP874)(void);
    void (*ptr_GUI_UC_SetEncodeCP1250)(void);
    void (*ptr_GUI_UC_SetEncodeCP1251)(void);
    void (*ptr_GUI_UC_SetEncodeCP1253)(void);
    void (*ptr_GUI_UC_SetEncodeCP1255)(void);
    void (*ptr_GUI_UC_SetEncodeCP1256)(void);
    void (*ptr_GUI_UC_SetEncodeCP1257)(void);
    void (*ptr_GUI_UC_SetEncodeCP1258)(void);
    void (*ptr_GUI_DispBin)(U32  v, U8 Len);
    void (*ptr_GUI_DispBinAt)(U32  v, I16P x, I16P y, U8 Len);
    void (*ptr_GUI_DispHex)(U32 v, U8 Len);
    void (*ptr_GUI_DispHexAt)(U32 v, I16P x, I16P y, U8 Len);
    GUI_ALLOC_DATATYPE(*ptr_GUI_ALLOC_GetNumFreeBytes)(void);
    GUI_HMEM(*ptr_GUI_ALLOC_AllocInit)(const void *pInitData, GUI_ALLOC_DATATYPE Size);
    GUI_HMEM(*ptr_GUI_ALLOC_AllocNoInit)(GUI_ALLOC_DATATYPE size);
    GUI_HMEM(*ptr_GUI_ALLOC_AllocZero)(GUI_ALLOC_DATATYPE size);
    void (*ptr_GUI_ALLOC_Free)(GUI_HMEM  hMem);
    void (*ptr_GUI_ALLOC_FreePtr)(GUI_HMEM *phMem);
    GUI_ALLOC_DATATYPE(*ptr_GUI_ALLOC_GetSize)(GUI_HMEM  hMem);
    GUI_ALLOC_DATATYPE(*ptr_GUI_ALLOC_GetMaxSize)(void);
    void             *(*ptr_GUI_ALLOC_h2p)(GUI_HMEM  hMem);
    __s32 (*ptr_GUI_ALLOC_Init)(void);
    void (*ptr_GUI_SelectLCD)(void);
    int (*ptr_GUI_SelectLayer)(unsigned int Index);
    int (*ptr_GUI_CreateLayer)(void *pFBAddr, unsigned int width, unsigned int height, int colorMode);               // mcl add
    int (*ptr_GUI_ReleaseLayer)(int index);                   // mcl add
    char(*ptr_GUI_SetLCDSize)(int x, int y);
    char(*ptr_GUI_SetLCDDirection)(U8 dir);
    void (*ptr_GUI_TOUCH_X_ActivateX)(void);
    void (*ptr_GUI_TOUCH_X_ActivateY)(void);
    int (*ptr_GUI_TOUCH_X_MeasureX)(void);
    int (*ptr_GUI_TOUCH_X_MeasureY)(void);
    LCD_COLOR(*ptr_LCD_L0_Index2Color)(int Index);
    LCD_COLOR(*ptr_LCD_L0_1_Index2Color)(int Index);
    LCD_COLOR(*ptr_LCD_L0_2_Index2Color)(int Index);
    LCD_COLOR(*ptr_LCD_L0_3_Index2Color)(int Index);
    unsigned int (*ptr_LCD_L0_Color2Index)(LCD_COLOR Color);
    unsigned int (*ptr_LCD_L0_1_Color2Index)(LCD_COLOR Color);
    unsigned int (*ptr_LCD_L0_2_Color2Index)(LCD_COLOR Color);
    unsigned int (*ptr_LCD_L0_3_Color2Index)(LCD_COLOR Color);
    unsigned int (*ptr_LCD_L0_GetIndexMask)(void);
    unsigned int (*ptr_LCD_L0_1_GetIndexMask)(void);
    unsigned int (*ptr_LCD_L0_2_GetIndexMask)(void);
    unsigned int (*ptr_LCD_L0_3_GetIndexMask)(void);
    int (*ptr_LCD_GetXSize)(void);
    int (*ptr_LCD_GetYSize)(void);
    int (*ptr_LCD_GetXSizeEx)(int Index);
    int (*ptr_LCD_GetYSizeEx)(int Index);
    U32(*ptr_LCD_GetNumColorsEx)(int Index);
    I32(*ptr_LCD_GetFixedPaletteEx)(int Index);
    int (*ptr_LCD_GetXMag)(void);
    int (*ptr_LCD_GetYMag)(void);
    int (*ptr_LCD_GetSwapRBEx)(int LayerIndex);
    void (*ptr_LCD_DrawHLine)(int x0, int y0,  int x1);
    void (*ptr_LCD_DrawPixel)(int x0, int y0);
    void (*ptr_LCD_DrawVLine)(int x, int y0,  int y1);
    void (*ptr_LCD_SetClipRectEx)(const LCD_RECT *pRect);
    void (*ptr_LCD_SetClipRectMax)(void);
    int (*ptr_LCD_Init)(void);
    int (*ptr_LCD_Init_layer)(void);
    void (*ptr_LCD_SetBkColor)(LCD_COLOR Color);                 /* Set background color */
    void (*ptr_LCD_SetColor)(LCD_COLOR Color);                   /* Set foreground color */
    void (*ptr_LCD_SetPixelIndex)(int x, int y, int ColorIndex);
    LCD_DRAWMODE(*ptr_LCD_SetDrawMode)(LCD_DRAWMODE dm);
    LCD_FONTMODE(*ptr_LCD_SetFontMode)(LCD_FONTMODE fm)  ;           // mcl  add
    void (*ptr_LCD_SetColorIndex)(int Index);
    void (*ptr_LCD_SetBkColorIndex)(int Index);
    void (*ptr_LCD_FillRect)(int x0, int y0, int x1, int y1);
    LCD_COLOR(*ptr_LCD_MixColors256)(LCD_COLOR Color, LCD_COLOR BkColor, unsigned Intens);
    LCD_COLOR(*ptr_LCD_GetPixelColor)(int x, int y);              /* Get RGB color of pixel */
    unsigned int (*ptr_LCD_GetPixelIndex)(int x, int y);
    int (*ptr_LCD_L0_CheckInit)(void);
    int (*ptr_LCD_Color2Index)(LCD_COLOR Color);
    LCD_COLOR(*ptr_LCD_Index2Color)(int Index);
    __s32(*ptr_GUI_LyrMove)(H_LYR h_lyr, __s32 x, __s32 y);
    void (*ptr_GUI_LyrLock)(void);
    void (*ptr_GUI_LyrUnlock)(void);
    __u16(*ptr_GUI_LyrLockAccept)(void);
    void (*ptr_GUI_LyrInitOS)(void);
    __bool(*ptr_GUI_HavePendingMessage)(H_WIN hWnd);                                         /**/
    __s32(*ptr_GUI_BroadcastMessage)(__gui_msg_t *pmsg);
    __s32(*ptr_GUI_PostSyncMessage)(__gui_msg_t *pmsg);
    __s32(*ptr_GUI_SendMessage)(__gui_msg_t *msg);
    __s32(*ptr_GUI_SendMessageTimeout)(__gui_msg_t *msg, __u32 timeout);
    __s32(*ptr_GUI_PostMessage)(__gui_msg_t *msg);
    __s32(*ptr_GUI_SendNotifyMessage)(__gui_msg_t *msg);
    __s32(*ptr_GUI_GetMessage)(__gui_msg_t *msg, H_WIN  hManWin);
    __s32(*ptr_GUI_GetMessageEx)(__gui_msg_t *msg, H_WIN  hManWin);
    __s32(*ptr_GUI_MsgSetRepeatTimes)(H_WIN  hManWin, __u32 count);
    __bool(*ptr_GUI_QueryMessage)(H_WIN  hManWin);
    __s32(*ptr_GUI_PeekMessage)(__gui_msg_t *msg, H_WIN  hManWin, __u32 len);
    __s32(*ptr_GUI_DispatchMessage)(__gui_msg_t *msg);
    __s32(*ptr_GUI_PostQuitMessage)(H_WIN  hWnd);
    __s32(*ptr_GUI_SendAsyncMessage)(__gui_msg_t *msg);
    __s32(*ptr_GUI_ThrowAwayMessages)(H_WIN hWnd);
    __s32(*ptr_GUI_SetSyncMsgRetVal)(__gui_msg_t *msg, __s32 ret);
    __s32(*ptr_GUI_PostSyncSem)(__gui_msg_t *msg);
    HLANG(*ptr_Lang_Open)(char *szAppFile, __u32 mode);
    int (*ptr_Lang_Read)(HLANG hLang, int address, int length, char *buffer);
    int (*ptr_Lang_GetStringAddress)(HLANG hLang, short LangID, short StringID);
    int (*ptr_Lang_GetStringSize)(HLANG hLang, short LangID, short StringID);
    int (*ptr_Lang_GetString)(HLANG hLang, short LangID, short StringID, char *buffer, int length);
    int (*ptr_Lang_Close)(HLANG hLang);
    HRES(*ptr_OpenRes)(char *szAppFile, __u32 mode);
    __s32(*ptr_CloseRes)(HRES hRes);
    __u32(*ptr_GetResAddr)(HRES hRes, __u16 StyleID, __u16 ID);
    __u32(*ptr_ReadRes)(HRES hRes, __u32 Address, __u32 Length, void *Buffer);
    __u16(*ptr_GetResType)(HRES hRes, __u16 StyleID, __u16 ID);
    __u32(*ptr_GetResSize)(HRES hRes, __u16 StyleID, __u16 ID);
    __s32(*ptr_GetRes)(HRES hRes, __u16 StyleID, __u16 ID, void *Buffer, __u32 Length);
    U8(*ptr_GUI_SetFrameColor8bpp32)(U8 frameColor);
    H_WIN(*ptr_GUI_ManWinCreate)(pmanwincreate create_info);                                        /*创建主窗口窗口*/
    __s32(*ptr_GUI_ManWinDelete)(H_WIN  hmanwin);                                                   /*删除主窗口*/
    H_WIN(*ptr_GUI_SetActiveManWin)(H_WIN  hWnd);
    H_WIN(*ptr_GUI_GetActiveManWin)(void);                                                          /*获取active 主窗口句柄*/
    __s32(*ptr_GUI_ManWinDefaultProc)(__gui_msg_t *msg);
    H_WIN(*ptr_GUI_GetTouchFocusWin)(__s32 x, __s32 y);
    __bool(*ptr_GUI_SceenPointInWin)(H_WIN hWnd, __s32 x, __s32 y);
    H_WIN(*ptr_GUI_FrmWinCreate)(pframewincreate create_info);                                      /*创建framewin*/
    __s32(*ptr_GUI_FrmWinDelete)(H_WIN  hframewin);                                                 /*删除framewin*/
    __s32(*ptr_GUI_FrmWinDefaultProc)(__gui_msg_t *msg);
    H_WIN(*ptr_GUI_CtrlWinCreate)(__gui_ctlwincreate_para_t *create_info);                          /*创建控件窗口*/
    __s32(*ptr_GUI_CtrlWinDelete)(H_WIN  hWnd);                                                     /*删除控件窗口*/
    unsigned long(*ptr_GUI_CtrlWinGetAddData)(H_WIN hWnd);                                                  /*获取control的附加数据*/
    __s32(*ptr_GUI_CtrlWinSetAddData)(H_WIN hWnd, unsigned long dwAddData);                                 /*设置control的附加数据*/
    __s32(*ptr_GUI_CtrlWinGetClassName)(H_WIN hWnd, char *name);                                    /*获取控件的类名*/
    __s32(*ptr_GUI_CtrlWinDefaultProc)(__gui_msg_t *msg);
    GUI_MEMDEV_Handle(*ptr_GUI_MEMDEV_Create)(int x0, int y0, int XSize, int YSize);
    GUI_MEMDEV_Handle(*ptr_GUI_MEMDEV_CreateEx)(int x0, int y0, int XSize, int YSize, int Flags);
    GUI_MEMDEV_Handle(*ptr_GUI_MEMDEV_CreateFixed)(int x0, int y0, int xsize, int ysize, int Flags, const tLCDDEV_APIList *pMemDevAPI, const LCD_API_COLOR_CONV *pColorConvAPI);
    void (*ptr_GUI_MEMDEV_Clear)(GUI_MEMDEV_Handle hMem);
    void (*ptr_GUI_MEMDEV_CopyFromLCD)(GUI_MEMDEV_Handle hMem);
    void (*ptr_GUI_MEMDEV_CopyToLCD)(GUI_MEMDEV_Handle hMem);
    void (*ptr_GUI_MEMDEV_CopyToLCDAt)(GUI_MEMDEV_Handle hMem, int x, int y);
    int (*ptr_GUI_MEMDEV_CompareWithLCD)(GUI_MEMDEV_Handle hMem, int *px, int *py, int *pExp, int *pAct);
    void (*ptr_GUI_MEMDEV_Delete)(GUI_MEMDEV_Handle MemDev);
    int (*ptr_GUI_MEMDEV_GetXSize)(GUI_MEMDEV_Handle hMem);
    int (*ptr_GUI_MEMDEV_GetYSize)(GUI_MEMDEV_Handle hMem);
    void (*ptr_GUI_MEMDEV_ReduceYSize)(GUI_MEMDEV_Handle hMem, int YSize);
    GUI_MEMDEV_Handle(*ptr_GUI_MEMDEV_Select)(GUI_MEMDEV_Handle hMem);    /* Select (activate) a particular memory device. */
    void (*ptr_GUI_MEMDEV_SetOrg)(GUI_MEMDEV_Handle hMem, int x0, int y0);
    void (*ptr_GUI_MEMDEV_WriteAt)(GUI_MEMDEV_Handle hMem, int x, int y);
    void (*ptr_GUI_MEMDEV_Write)(GUI_MEMDEV_Handle hMem);
    void (*ptr_GUI_MEMDEV_WriteExAt)(GUI_MEMDEV_Handle hMem, int x, int y, int xMag, int yMag, int Alpha);
    void (*ptr_GUI_MEMDEV_WriteEx)(GUI_MEMDEV_Handle hMem, int xMag, int yMag, int Alpha);
    void             *(*ptr_GUI_MEMDEV_GetDataPtr)(GUI_MEMDEV_Handle hMem);
    H_WIN(*ptr_GUI_DlgGetFirstButton)(H_WIN hDlg);
    __s32(*ptr_GUI_ModuleDialogCreate)(__gui_dialogcreate_para_t *create_info);                     /*创建一个模式对话框*/
    __bool(*ptr_GUI_EndDialog)(H_WIN hDlg, __s32 endCode);                                          /*退出一个模式对话框*/
    H_WIN(*ptr_GUI_GetDlgItem)(H_WIN hDlg, __s32 nIDDlgItem);                                       /*获取Dialog中的某个Item的句柄*/
    __s32(*ptr_GUI_DialogDefaultProc)(__gui_msg_t    *msg);                                         /*默认的对话框处理函数*/
    H_WIN(*ptr_GUI_ModulelessDialogCreate)(__gui_dialogcreate_para_t *create_info);                 /*创建一个无模式对话框*/
    __bool(*ptr_GUI_ModulelessDialogDelete)(H_WIN hManWin);                                        /*删除一个无模式对话框*/
    H_WIN(*ptr_GUI_GetNextDlgTabItem)(H_WIN hDlg, H_WIN hCtl, __bool bPrevious);
    __s32(*ptr_GUI_AddNewControlClass)(pwinclass  pWndClass);
    __s32(*ptr_GUI_SetCtrlClassAddData)(const char *szClassName, __u32 dwAddData);
    void (*ptr_GUI_EmptyControlClassInfoTable)(void);
    __s32(*ptr_GUI_GetCtrlClassAddData)(const char *szClassName, __u32 *pAddData);
    __s32(*ptr_GUI_DeleteControlClass)(const char *szClassName);
    void (*ptr_GUI_TerminateControlClass)(void);
    __hdle(*ptr_GUI_LongStringCreate)(__show_info_move_t *str_move);
    __s32(*ptr_GUI_LongStringDelete)(__hdle hString);
    __s32(*ptr_GUI_LongStringStart)(__hdle hString);
    __s32(*ptr_GUI_LongStringStop)(__hdle hString);
    __s32(*ptr_GUI_LongStringReset)(__hdle hString, __show_info_move_t *str_move);
    GUI_FONT         *(*ptr_GUI_SFT_CreateFont)(unsigned int  pixelSize, const char  *font_file);
    void (*ptr_GUI_SFT_ReleaseFont)(GUI_FONT *pFont);
    GUI_FONT         *(*ptr_GUI_TTF_CreateFont)(GUI_TTF_ATTR *pTTF_ATTR);
    int (*ptr_GUI_TTF_GetFamilyName)(GUI_FONT *pFont, char *pBuffer, int NumBytes);
    int (*ptr_GUI_TTF_GetStyleName)(GUI_FONT *pFont, char *pBuffer, int NumBytes);
    void (*ptr_GUI_TTF_Done)(GUI_FONT *pFont);
    __bool(*ptr_RegisterButtonControl)(void);
    __s32(*ptr_GUI_ButtonSetFocusBmp)(H_WIN hbutton, void *bmp);
    __s32(*ptr_GUI_ButtonSetUnFocusBmp)(H_WIN hbutton, void *bmp);
    __s32(*ptr_GUI_ButtonSetText)(H_WIN hbutton, char *text);
    __s32(*ptr_GUI_ButtonSetUnFcsTxtColor)(H_WIN hbutton, __u32 color);
    __s32(*ptr_GUI_ButtonSetFcsTxtColor)(H_WIN hbutton, __u32  color);
    __s32(*ptr_GUI_ButtonSetBkColor)(H_WIN hbutton, __u32  color);
    __s32(*ptr_GUI_ButtonSetBmpPos)(H_WIN hbutton, __pos_t *pos);
    __s32(*ptr_GUI_ButtonSetTxtPos)(H_WIN hbutton, __pos_t *pos);
    __bool(*ptr_RegisterStaticControl)(void);
    __s32(*ptr_GUI_StaticSetFocusBmp)(H_WIN hbutton, void *bmp);
    __s32(*ptr_GUI_StaticSetUnFocusBmp)(H_WIN hbutton, void *bmp);
    __s32(*ptr_GUI_StaticSetText)(H_WIN hbutton, char *text);
    __s32(*ptr_GUI_StaticSetUnFcsTxtColor)(H_WIN hbutton, __u32 color);
    __s32(*ptr_GUI_StaticSetFcsTxtColor)(H_WIN hbutton, __u32  color);
    __s32(*ptr_GUI_StaticSetBmpPos)(H_WIN hbutton, __pos_t *pos);
    __s32(*ptr_GUI_StaticSetTxtPos)(H_WIN hbutton, __pos_t *pos);
    __s32(*ptr_GUI_StaticStartMove)(H_WIN hstatic);
    __s32(*ptr_GUI_StaticStopMove)(H_WIN hstatic);
    __s32(*ptr_GUI_StaticSetCharSet)(H_WIN hstatic, __epdk_charset_enm_e draw_code);
    H_ITEM(*ptr_GUI_LstmGetCurItem)(H_WIN lstm);
    __u32(*ptr_GUI_LstmGetItemNo)(H_WIN H_Item);
    __u32(*ptr_GUI_LstmSetCurNo)(H_WIN lstm, __u32 curno);
    __u32(*ptr_GUI_LstmDrawCurNo)(H_WIN lstm, __u32 curno);
    __u32(*ptr_GUI_LstmStopSlip)(H_WIN lstm);
    H_ITEM(*ptr_GUI_LstmGetItem)(H_WIN lstm, __u32 no);
    __s32(*ptr_GUI_LstmUpdateItem)(H_WIN lstm, __u32 no);
    __bool(*ptr_GUI_LstmChildInFb)(H_WIN lstm, H_ITEM handle);
    __u32(*ptr_GUI_LstmDelCurItem)(H_WIN lstm);
    __u32(*ptr_GUI_LstmAddItemTail)(H_WIN lstm, __lstm_item_attr_t *NewItem);
    __u32(*ptr_GUI_LstmGetItemCount)(H_WIN lstm);
    H_LYR(*ptr_GUI_LstmGetItemLyrWin)(H_WIN H_Item);
    __s32(*ptr_GUI_LstmGetItemRect)(H_WIN H_Item, RECT *rect);
    unsigned long (*ptr_GUI_LstmGetItemAttr)(H_WIN H_Item);
    H_WIN(*ptr_GUI_LstmGetHostWin)(H_WIN H_Item);
    __bool(*ptr_RegisterListMenuControl)(void);
    __bool(*ptr_RegisterProgSheetControl)(void);
    __bool(*ptr_RegisterSliderControl)(void);
    __s32(*ptr_SLIDER_SetRange)(H_WIN hwin, __s32 min, __s32 max);
    __s32(*ptr_SLIDER_SetValue)(H_WIN hwin, __s32 value);
    __s32(*ptr_SLIDER_GetMin)(H_WIN hwin);
    __s32(*ptr_SLIDER_GetMax)(H_WIN hwin);
    __s32(*ptr_SLIDER_GetValue)(H_WIN hwin);
    __s32(*ptr_SLIDER_SetLineSize)(H_WIN hwin, __u32 line_size);
    __s32(*ptr_SLIDER_GetLineSize)(H_WIN hwin);
    HBMP(*ptr_bmp_open)(const void *bmp_buf);
    __s32(*ptr_bmp_close)(HBMP hbmp);
    __u32(*ptr_bmp_get_bitcount)(HBMP hbmp);
    __u32(*ptr_bmp_get_width)(HBMP hbmp);
    __u32(*ptr_bmp_get_height)(HBMP hbmp);
    __u32(*ptr_bmp_get_size)(HBMP hbmp);
    void (*ptr_bmp_get_matrix)(const void *bmp_buf, void *matrix);
    __u32(*ptr_GUI_TxtboxSetStamp)(H_WIN txt, __u32 stamp);
    __u32(*ptr_GUI_TxtboxOnDraw)(H_WIN txt);
    __u32(*ptr_GUI_TxtboxSetMode)(H_WIN txt, __u8 mode);
    __u32(*ptr_GUI_TxtboxGetMode)(H_WIN txt);
    __s32(*ptr_GUI_TxtBoxSetPara)(H_WIN hbox, __txtbox_para_t *para);
    __bool(*ptr_RegisterTxtboxControl)(void);
    __u32(*ptr_GUI_GMsgBox)(__gui_gmsgboxcreate_para_t *create_info);
    __s32(*ptr_GUI_GMsgBoxSetText)(H_WIN hbox, char *text);
    __s32(*ptr_GUI_GMsgBoxSetTextPos)(H_WIN hbox, __pos_t *pos);
    __bool(*ptr_RegisterSpinboxControl)(void);
    __s32(*ptr_GUI_SpinboxGetCurValue)(H_WIN hspin, __s16  *value);
    __s32(*ptr_GUI_SpinboxSetCurValue)(H_WIN hspin, __s16  value);
    __s32(*ptr_GUI_SpinboxSetMaxValue)(H_WIN hspin, __s16  value);
    __s32(*ptr_GUI_SpinboxSetMinValue)(H_WIN hspin, __s16  value);
    __s32(*ptr_GUI_SpinboxSetBkColor)(H_WIN hspin, __u32 color);
    __s32(*ptr_GUI_SpinboxSetUnFcsTxtColor)(H_WIN hspin, __u32 color);
    __s32(*ptr_GUI_SpinboxSetFcsTxtColor)(H_WIN hspin, __u32  color);
    __s32(*ptr_GUI_SpinboxSetFocusUpBmp)(H_WIN hspin, void *bmp);
    __s32(*ptr_GUI_SpinboxSetFocusDownBmp)(H_WIN hspin, void *bmp);
    __s32(*ptr_GUI_SpinboxSetUnFocusUpBmp)(H_WIN hspin, void *bmp);
    __s32(*ptr_GUI_SpinboxSetUnFocusDownBmp)(H_WIN hspin, void *bmp);
    __s32(*ptr_GUI_SpinboxSetUpBmpPos)(H_WIN hspin, __pos_t *pos);
    __s32(*ptr_GUI_SpinboxSetDownBmpPos)(H_WIN hspin, __pos_t *pos);
    __bool(*ptr_RegisterProgbarControl)(void);
    __s32(*ptr_PROGBAR_SetRange)(H_WIN hwin, __s32 min, __s32 max);
    __s32(*ptr_PROGBAR_SetValue)(H_WIN hwin, __s32 value);
    __s32(*ptr_PROGBAR_GetMin)(H_WIN hwin);
    __s32(*ptr_PROGBAR_GetMax)(H_WIN hwin);
    __s32(*ptr_PROGBAR_GetValue)(H_WIN hwin);
    __s32(*ptr_SListbox_UpdateMenulist)(H_WIN  hlistbox, __slistbox_para_t *para);
    __u32(*ptr_SListbox_GetItemNo)(h_listboxitem H_Item);
    unsigned long(*ptr_SListbox_GetItemAttr)(h_listboxitem H_Item);
    H_WIN(*ptr_SListbox_GetHostWin)(h_listboxitem H_Item);
    H_LYR(*ptr_SListbox_GetItemLyrWin)(h_listboxitem H_Item);
    __u32(*ptr_SListbox_GetItemCount)(H_WIN  listbox);
    h_listboxitem(*ptr_SListbox_GetItem)(H_WIN  hlistbox, __u32 no);
    h_listboxitem(*ptr_SListbox_GetCurItem)(H_WIN  listbox);
    __s32(*ptr_SListbox_PrevPage)(H_WIN  hlistbox);
    __s32(*ptr_SListbox_NextPage)(H_WIN  hlistbox);
    __s32(*ptr_SListbox_ItemDisable)(H_WIN  hlistbox, __u32 index);
    __s32(*ptr_SListbox_ItemEnable)(H_WIN  hlistbox, __u32 index);
    __s32(*ptr_SListbox_GetHeadindex)(H_WIN  hlistbox);
    __s32(*ptr_SListbox_DelCurItem)(H_WIN  hlistbox);
    __s32(*ptr_SListbox_AddTail)(H_WIN  hlistbox, __slistbox_item_attr_t *item_para);
    __s32(*ptr_SListbox_SetCurItem)(H_WIN  hlistbox, __u32 cur_index);
    __u32(*ptr_SListbox_Disable)(H_WIN  hlistbox);
    __s32(*ptr_SListbox_GetItemRect)(h_listboxitem hitem, RECT *rect);
    __bool(*ptr_RegisterListBoxControl)(void);
    __bool(*ptr_UnRegisterListBoxControl)(void);
    __s32(*ptr_GUI_Fb2Dispfb)(FB *fb, __disp_fb_t *dispfb);
    __s32(*ptr_GUI_Dispfb2Fb)(__disp_fb_t *dispfb, FB *fb);
    __s32(*ptr_GUI_LogicLyr2DispLyr)(__disp_layer_para_t *log_lyr, __disp_layer_info_t *disp_lyr);
    __s32(*ptr_GUI_DispLyr2LogicLyr)(__disp_layer_para_t *log_lyr, __disp_layer_info_t *disp_lyr);
    int (*ptr_GUI_BMP_DrawEx)(const void *pBMP, int x0, int y0);
#if (defined CONFIG_SOC_SUN3IW2 ||defined CONFIG_SOC_SUN8IW19 || defined CONFIG_SOC_SUN20IW1)
    void (*ptr_GUI_Memdev_FrameBuffer_Release)(__u8 type);
    //int (*ptr_GUI_BitString_Draw2)(FB *fb, int dst_x, int dst_y, int src_width, int src_height);
#elif defined CONFIG_SOC_SUN3IW1
    //void (*ptr_GUI_SetFrameColor32bpp)(void);
    H_LYR(*ptr_GUI_LyrWinGetHWLyr)(H_LYR h_lyr);
#endif
    void *(*ptr_GUI_CreateAutoPanel)(int id);
    int (*ptr_GUI_DestroyAutoPanel)(void *hdle);
    int (*ptr_GUI_BMP_Draw_Rotate)(const U8 *pData, int x0, int y0, int XSize, int YSize);
} orangemod_api_entry_t;

#ifndef USED_BY_ORANGEMOD   //not used by orangemod module itsef
#define ORANGEMOD_ENTRY_TBL   ((EMOD_TYPE_MOD_ORANGE << 20) |(0xe0000000))   //this address is depended on the link address of mod_orangemod
#define ORANGEMOD_FUNC_ENTRY  ((orangemod_api_entry_t *)ORANGEMOD_ENTRY_TBL)
#define GUI_LyrWinCreate                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinCreate             )
#define GUI_LyrWinDelete                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinDelete             )
#define GUI_WinThreadCleanup                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinThreadCleanup         )
#define GUI_WinSetFocusChild                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinSetFocusChild         )
#define GUI_WinGetFocusChild                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetFocusChild         )
#define GUI_WinSetActiveChild                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinSetActiveChild        )
#define GUI_WinGetType                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetType               )
#define GUI_WinGetLyrWin                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetLyrWin             )
#define GUI_WinGetMainManWin                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetMainManWin         )
#define GUI_WinGetManWin                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetManWin             )
#define GUI_WinGetFrmWin                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetFrmWin             )
#define GUI_WinGetParent                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetParent             )
#define GUI_WinGetFirstChild                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetFirstChild         )
#define GUI_WinGetNextBro                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetNextBro            )
#define GUI_WinIsAncestor                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinIsAncestor            )
#define GUI_WinIsChild                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinIsChild               )
#define GUI_WinGetRootWin                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetRootWin            )
#define GUI_WinGetNextHostedWin                         (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetNextHostedWin      )
#define GUI_WinGetFirstHostedWin                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetFirstHostedWin     )
#define GUI_WinGetOwnerWin                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetOwnerWin           )
#define GUI_WinGetDlgItem                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetDlgItem            )
#define GUI_WinGetItemId                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetItemId             )
#define GUI_WinGetPrimitiveWin                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetPrimitiveWin       )
#define GUI_WinGetHandFromId                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetHandFromId         )
#define GUI_WinGetName                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetName               )
#define GUI_WinGetHandFromName                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetHandFromName       )
#define GUI_WinGetAttr                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetAttr               )
#define GUI_WinSetAttr                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinSetAttr               )
#define GUI_WinGetAddData                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetAddData            )
#define GUI_WinSetAddData                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinSetAddData            )
#define GUI_WinGetStyle                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetStyle              )
#define GUI_WinSetStyle                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinSetStyle              )
#define GUI_WinGetExStyle                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetExStyle            )
#define GUI_WinSetExStyle                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinSetExStyle            )
#define GUI_WinIsVisible                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinIsVisible             )
#define GUI_WinSetNotifyCallback                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinSetNotifyCallback     )
#define GUI_WinGetNotifyCallback                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetNotifyCallback     )
#define GUI_WinSetCallback                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinSetCallback           )
#define GUI_WinGetCallback                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetCallback           )
#define GUI_NotifyParent                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_NotifyParent             )
#define GUI_WinUpdate                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinUpdate                )
#define GUI_InvalidateRect                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_InvalidateRect           )
#define GUI_WinSetCaptureWin                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinSetCaptureWin         )
#define GUI_WinGetCaptureWin                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetCaptureWin         )
#define GUI_WinReleaseCapture                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinReleaseCapture        )
#define GUI_WinSetTouchFocus                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinSetTouchFocus         )
#define GUI_WinGetTouchFocus                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetTouchFocus         )
#define GUI_WinReleaseTouchFocus                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinReleaseTouchFocus     )
#define GUI_WinGetClient                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetClient             )
#define GUI_WinGetWinRECT                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetWinRECT            )
#define GUI_WinSetWinRECT                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinSetWinRECT            )
#define GUI_WinGetClientRECT                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetClientRECT         )
#define GUI_WinSetClientRECT                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinSetClientRECT         )
#define GUI_WinGetClientFBRect                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetClientFBRect       )
#define GUI_WinGetWinFBRect                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetWinFBRect          )
#define GUI_WinGetWinFBRect                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinGetWinFBRect          )
#define GUI_ClientToScreen                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ClientToScreen           )
#define GUI_ScreenToClient                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ScreenToClient           )
#define GUI_WindowToScreen                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WindowToScreen           )
#define GUI_ScreenToWindow                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ScreenToWindow           )
#define GUI_FBToScreen                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_FBToScreen               )
#define GUI_ScreenToFB                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ScreenToFB               )
#define GUI_LyrWinSetSrcWindow                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinSetSrcWindow       )
#define GUI_LyrWinSetScnWindow                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinSetScnWindow       )
#define GUI_LyrWinGetSrcWindow                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinGetSrcWindow       )
#define GUI_LyrWinGetScnWindow                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinGetScnWindow       )
#define GUI_LyrWinGetFB                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinGetFB              )
#define GUI_LyrWinSetFB                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinSetFB              )
#define GUI_LyrWinSetSta                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinSetSta             )
#define GUI_LyrWinGetSta                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinGetSta             )
#define GUI_LyrWinSel                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinSel                )
#define GUI_LyrWinCacheOn                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinCacheOn            )
#define GUI_LyrWinCacheOff                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinCacheOff           )
#define GUI_LyrWinFlushFB                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinFlushFB            )
#define GUI_LyrWinSetAlpha                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinSetAlpha           )
#define GUI_LyrWinAlphaOn                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinAlphaOn            )
#define GUI_LyrWinAlphaOff                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinAlphaOff           )
#define GUI_LyrWinCKOn                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinCKOn               )
#define GUI_LyrWinCKOff                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinCKOff              )
#define GUI_LyrWinSetBottom                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinSetBottom          )
#define GUI_LyrWinSetTop                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinSetTop             )
#define GUI_SetTimer                                    (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetTimer                 )
#define GUI_KillTimer                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_KillTimer                )
#define GUI_ResetTimer                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ResetTimer               )
#define GUI_IsTimerInstalled                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_IsTimerInstalled         )
#define GUI_HaveFreeTimer                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_HaveFreeTimer            )
#define GUI_GetTickCount                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetTickCount             )
#define GUI_GetTimerExpired                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetTimerExpired          )
#define GUI_WinInit                                     (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinInit                  )
#define GUI_WinExit                                     (ORANGEMOD_FUNC_ENTRY->ptr_GUI_WinExit                  )
#define GUI_CharSetToEncode                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_CharSetToEncode          )
#define GUI_CursorCreate                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_CursorCreate             )
#define GUI_CursorDelete                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_CursorDelete             )
#define GUI_CursorShow                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_CursorShow               )
#define GUI_CursorHide                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_CursorHide               )
#define GUI_CursorSetPos                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_CursorSetPos             )
#define GUI_CursorGetPos                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_CursorGetPos             )
#define GUI_CursorSetPara                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_CursorSetPara            )
#define GUI_GetScnDir                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetScnDir                )
#define GUI_SetScnDir                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetScnDir                )
#define GUI_SetZoomWin                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetZoomWin               )
#define GUI_GetScreenSize                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetScreenSize            )
#define SysRectToGuiRect                                (ORANGEMOD_FUNC_ENTRY->ptr_SysRectToGuiRect             )
#define GUI_Lock                                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_Lock                     )
#define GUI_Unlock                                      (ORANGEMOD_FUNC_ENTRY->ptr_GUI_Unlock                   )
#define GUI_LockAccept                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LockAccept               )
#define GUITASK_Init                                    (ORANGEMOD_FUNC_ENTRY->ptr_GUITASK_Init                 )
#define GUITASK_CopyContext                             (ORANGEMOD_FUNC_ENTRY->ptr_GUITASK_CopyContext          )
#define GUI_Init_layer                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_Init_layer               )
#define GUI_SetDefault                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetDefault               )
#define GUI_SetDrawMode                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetDrawMode              )
#define GUI_SetFontMode                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetFontMode              )
#define GUI_SaveContext                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SaveContext              )
#define GUI_RestoreContext                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_RestoreContext           )
#define GUI_OpenAlphaBlend                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_OpenAlphaBlend           )
#define GUI_CloseAlphaBlend                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_CloseAlphaBlend          )
#define GUI_RectsIntersect                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_RectsIntersect           )
#define GUI_MoveRect                                    (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MoveRect                 )
#define GUI_MergeRect                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MergeRect                )
#define GUI__IntersectRects                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI__IntersectRects          )
#define GUI__IntersectRect                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI__IntersectRect           )
#define GUI_SetClipRect                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetClipRect              )
#define GUI__DivideRound32                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI__DivideRound32           )
#define GUI_GetPenSize                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetPenSize       )
#define GUI_GetPenShape                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetPenShape      )
#define GUI_GetLineStyle                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetLineStyle     )
#define GUI_SetBkColor                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetBkColor       )
#define GUI_SetColor                                    (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetColor         )
#define GUI_GetPenSize                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetPenSize       )
#define GUI_GetPenShape                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetPenShape      )
#define GUI_GetLineStyle                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetLineStyle     )
#define GUI_Log                                         (ORANGEMOD_FUNC_ENTRY->ptr_GUI_Log          )
#define GUI_Log1                                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_Log1         )
#define GUI_Log2                                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_Log2         )
#define GUI_Log3                                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_Log3         )
#define GUI_Log4                                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_Log4         )
#define GUI_Warn                                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_Warn         )
#define GUI_Warn1                                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_Warn1        )
#define GUI_Warn2                                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_Warn2        )
#define GUI_Warn3                                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_Warn3        )
#define GUI_Warn4                                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_Warn4        )
#define GUI_ErrorOut                                    (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ErrorOut     )
#define GUI_ErrorOut1                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ErrorOut1    )
#define GUI_ErrorOut2                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ErrorOut2    )
#define GUI_ErrorOut3                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ErrorOut3    )
#define GUI_ErrorOut4                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ErrorOut4    )
#define GUI_BMP_Draw                                    (ORANGEMOD_FUNC_ENTRY->ptr_GUI_BMP_Draw             )
#define GUI_BitString_Draw                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_BitString_Draw   )
#define GUI_BitString_DrawEx                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_BitString_DrawEx )
#define GUI_ARGB_Draw                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ARGB_Draw    )
#define GUI_BMP_Draw_Trans                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_BMP_Draw_Trans   )
#define GUI_BMP_GetXSize                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_BMP_GetXSize         )
#define GUI_BMP_GetYSize                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_BMP_GetYSize         )
#define GUI_Clear                                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_Clear                )
#define GUI_ClearRect                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ClearRect            )
#define GUI_ClearRectEx                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ClearRectEx          )
#define GUI_DrawArc                                     (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DrawArc              )
#define GUI_DrawCircle                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DrawCircle           )
#define GUI_DrawEllipse                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DrawEllipse          )
#define GUI_DrawGraph                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DrawGraph            )
#define GUI_DrawHLine                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DrawHLine            )
#define GUI_DrawLine                                    (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DrawLine             )
#define GUI_DrawLineRel                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DrawLineRel          )
#define GUI_DrawLineTo                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DrawLineTo           )
#define GUI_DrawPie                                     (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DrawPie              )
#define GUI_DrawPixel                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DrawPixel            )
#define GUI_DrawPoint                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DrawPoint            )
#define GUI_DrawPolygon                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DrawPolygon          )
#define GUI_DrawRect                                    (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DrawRect             )
#define GUI_DrawRectEx                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DrawRectEx           )
#define GUI_DrawVLine                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DrawVLine            )
#define GUI_FillCircle                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_FillCircle           )
#define GUI_FillEllipse                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_FillEllipse          )
#define GUI_FillPolygon                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_FillPolygon          )
#define GUI_FillRect                                    (ORANGEMOD_FUNC_ENTRY->ptr_GUI_FillRect             )
#define GUI_FillRectEx                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_FillRectEx           )
#define GUI_InvertRect                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_InvertRect           )
#define GUI_MoveRel                                     (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MoveRel              )
#define GUI_MoveTo                                      (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MoveTo               )
#define GUI_SetRectMem                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetRectMem   )
#define GUI_GetRectMem                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetRectMem   )
#define GUI_DispCEOL                                    (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispCEOL     )
#define GUI_DispChar                                    (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispChar     )
#define GUI_DispChars                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispChars    )
#define GUI_DispCharAt                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispCharAt   )
#define GUI_DispString                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispString           )
#define GUI_DispStringAt                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringAt         )
#define GUI_DispStringAtCEOL                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringAtCEOL     )
#define GUI_DispStringHCenterAt                         (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringHCenterAt  )
#define GUI__DispStringInRect                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI__DispStringInRect    )
#define GUI_DispStringInRect                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringInRect     )
#define GUI_DispStringInRectMax                         (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringInRectMax  )
#define GUI_DispStringLen                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringLen        )
#define GUI_DispStringInRectWrap                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringInRectWrap )
#define GUI_GetTextExtend                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetTextExtend    )
#define GUI_GetYAdjust                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetYAdjust   )
#define GUI_GetFont                                     (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetFont  )
#define GUI_GetCharDistX                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetCharDistX )
#define GUI_GetStringDistX                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetStringDistX   )
#define GUI_GetFontDistY                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetFontDistY )
#define GUI_GetFontSizeY                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetFontSizeY )
#define GUI_GetFontInfo                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetFontInfo      )
#define GUI_GetYSizeOfFont                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetYSizeOfFont   )
#define GUI_GetYDistOfFont                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetYDistOfFont   )
#define GUI_GetTextAlign                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetTextAlign )
#define GUI_GetTextMode                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetTextMode  )
#define GUI_IsInFont                                    (ORANGEMOD_FUNC_ENTRY->ptr_GUI_IsInFont )
#define GUI_SetTextAlign                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetTextAlign )
#define GUI_SetTextMode                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetTextMod )
#define GUI_SetFont                                     (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetFont  )
#define GUI_GotoXY                                      (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GotoXY   )
#define GUI_GotoX                                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GotoX    )
#define GUI_GotoY                                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GotoY    )
#define GUI_DispNextLine                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispNextLine )
#define GUI_GetDefaultFont                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetDefaultFont)
#define GUI_UC_GetCharSize                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_GetCharSize       )
#define GUI_UC_GetCharCode                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_GetCharCode       )
#define Big5_to_Unicode                                 (ORANGEMOD_FUNC_ENTRY->ptr_Big5_to_Unicode          )
#define GUI_UC_SetEncodeNone                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeNone     )
#define GUI_UC_SetEncodeUTF8                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeUTF8     )
#define GUI_UC_SetEncodeGB2312                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeGB2312   )
#define GUI_UC_SetEncodeGBK                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeGBK      )
#define GUI_UC_SetEncodeBig5                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeBig5     )
#define GUI_UC_SetEncodeSJIS                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeSJIS             )
#define GUI_UC_SetEncodeISO885916                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeISO885916        )
#define GUI_UC_SetEncodeISO885915                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeISO885915        )
#define GUI_UC_SetEncodeISO885914                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeISO885914        )
#define GUI_UC_SetEncodeISO885913                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeISO885913        )
#define GUI_UC_SetEncodeISO885912                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeISO885912        )
#define GUI_UC_SetEncodeISO885911                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeISO885911        )
#define GUI_UC_SetEncodeISO885910                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeISO885910        )
#define GUI_UC_SetEncodeISO88599                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeISO88599         )
#define GUI_UC_SetEncodeISO88598                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeISO88598         )
#define GUI_UC_SetEncodeISO88597                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeISO88597         )
#define GUI_UC_SetEncodeISO88596                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeISO88596         )
#define GUI_UC_SetEncodeISO88595                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeISO88595         )
#define GUI_UC_SetEncodeISO88594                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeISO88594         )
#define GUI_UC_SetEncodeISO88593                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeISO88593         )
#define GUI_UC_SetEncodeISO88592                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeISO88592         )
#define GUI_UC_SetEncodeISO88591                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeISO88591         )
#define GUI_UC_SetEncodeKIO8R                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeKIO8R            )
#define GUI_UC_SetEncodeEUCKR                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeEUCKR            )
#define GUI_UC_SetEncodeEUCJP                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeEUCJP            )
#define GUI_UC_SetEncodeCP874                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeCP874            )
#define GUI_UC_SetEncodeCP1250                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeCP1250           )
#define GUI_UC_SetEncodeCP1251                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeCP1251           )
#define GUI_UC_SetEncodeCP1253                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeCP1253           )
#define GUI_UC_SetEncodeCP1255                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeCP1255           )
#define GUI_UC_SetEncodeCP1256                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeCP1256           )
#define GUI_UC_SetEncodeCP1257                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeCP1257           )
#define GUI_UC_SetEncodeCP1258                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_UC_SetEncodeCP1258           )
#define GUI_DispBin                                     (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispBin      )
#define GUI_DispBinAt                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispBinAt    )
#define GUI_DispHex                                     (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispHex      )
#define GUI_DispHexAt                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispHexAt    )
#define GUI_ALLOC_GetNumFreeBytes                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ALLOC_GetNumFreeBytes    )
#define GUI_ALLOC_AllocInit                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ALLOC_AllocInit      )
#define GUI_ALLOC_AllocNoInit                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ALLOC_AllocNoInit    )
#define GUI_ALLOC_AllocZero                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ALLOC_AllocZero      )
#define GUI_ALLOC_Free                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ALLOC_Free           )
#define GUI_ALLOC_FreePtr                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ALLOC_FreePtr        )
#define GUI_ALLOC_GetSize                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ALLOC_GetSize        )
#define GUI_ALLOC_GetMaxSize                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ALLOC_GetMaxSize     )
#define GUI_ALLOC_h2p                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ALLOC_h2p            )
#define GUI_ALLOC_Init                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ALLOC_Init           )
#define GUI_SelectLCD                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SelectLCD    )
#define GUI_SelectLayer                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SelectLayer  )
#define GUI_CreateLayer                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_CreateLayer  )
#define GUI_ReleaseLayer                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ReleaseLayer )
#define GUI_SetLCDSize                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetLCDSize   )
#define GUI_SetLCDDirection                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetLCDDirection  )
#define GUI_TOUCH_X_ActivateX                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_TOUCH_X_ActivateX    )
#define GUI_TOUCH_X_ActivateY                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_TOUCH_X_ActivateY    )
#define GUI_TOUCH_X_MeasureX                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_TOUCH_X_MeasureX )
#define GUI_TOUCH_X_MeasureY                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_TOUCH_X_MeasureY )
#define LCD_L0_Index2Color                              (ORANGEMOD_FUNC_ENTRY->ptr_LCD_L0_Index2Color   )
#define LCD_L0_1_Index2Color                            (ORANGEMOD_FUNC_ENTRY->ptr_LCD_L0_1_Index2Color )
#define LCD_L0_2_Index2Color                            (ORANGEMOD_FUNC_ENTRY->ptr_LCD_L0_2_Index2Color )
#define LCD_L0_3_Index2Color                            (ORANGEMOD_FUNC_ENTRY->ptr_LCD_L0_3_Index2Color )
#define LCD_L0_Color2Index                              (ORANGEMOD_FUNC_ENTRY->ptr_LCD_L0_Color2Index   )
#define LCD_L0_1_Color2Index                            (ORANGEMOD_FUNC_ENTRY->ptr_LCD_L0_1_Color2Index )
#define LCD_L0_2_Color2Index                            (ORANGEMOD_FUNC_ENTRY->ptr_LCD_L0_2_Color2Index )
#define LCD_L0_3_Color2Index                            (ORANGEMOD_FUNC_ENTRY->ptr_LCD_L0_3_Color2Index )
#define LCD_L0_GetIndexMask                             (ORANGEMOD_FUNC_ENTRY->ptr_LCD_L0_GetIndexMask      )
#define LCD_L0_1_GetIndexMask                           (ORANGEMOD_FUNC_ENTRY->ptr_LCD_L0_1_GetIndexMask    )
#define LCD_L0_2_GetIndexMask                           (ORANGEMOD_FUNC_ENTRY->ptr_LCD_L0_2_GetIndexMask    )
#define LCD_L0_3_GetIndexMask                           (ORANGEMOD_FUNC_ENTRY->ptr_LCD_L0_3_GetIndexMask    )
#define LCD_GetXSize                                    (ORANGEMOD_FUNC_ENTRY->ptr_LCD_GetXSize )
#define LCD_GetYSize                                    (ORANGEMOD_FUNC_ENTRY->ptr_LCD_GetYSize )
#define LCD_GetXSizeEx                                  (ORANGEMOD_FUNC_ENTRY->ptr_LCD_GetXSizeEx   )
#define LCD_GetYSizeEx                                  (ORANGEMOD_FUNC_ENTRY->ptr_LCD_GetYSizeEx   )
#define LCD_GetNumColorsEx                              (ORANGEMOD_FUNC_ENTRY->ptr_LCD_GetNumColorsEx   )
#define LCD_GetFixedPaletteEx                           (ORANGEMOD_FUNC_ENTRY->ptr_LCD_GetFixedPaletteEx    )
#define LCD_GetXMag                                     (ORANGEMOD_FUNC_ENTRY->ptr_LCD_GetXMag  )
#define LCD_GetYMag                                     (ORANGEMOD_FUNC_ENTRY->ptr_LCD_GetYMag  )
#define LCD_GetSwapRBEx                                 (ORANGEMOD_FUNC_ENTRY->ptr_LCD_GetSwapRBEx  )
#define LCD_DrawHLine                                   (ORANGEMOD_FUNC_ENTRY->ptr_LCD_DrawHLine    )
#define LCD_DrawPixel                                   (ORANGEMOD_FUNC_ENTRY->ptr_LCD_DrawPixel    )
#define LCD_DrawVLine                                   (ORANGEMOD_FUNC_ENTRY->ptr_LCD_DrawVLine    )
#define LCD_SetClipRectEx                               (ORANGEMOD_FUNC_ENTRY->ptr_LCD_SetClipRectEx    )
#define LCD_SetClipRectMax                              (ORANGEMOD_FUNC_ENTRY->ptr_LCD_SetClipRectMax   )
#define LCD_Init                                        (ORANGEMOD_FUNC_ENTRY->ptr_LCD_Init )
#define LCD_Init_layer                                  (ORANGEMOD_FUNC_ENTRY->ptr_LCD_Init_layer   )
#define LCD_SetBkColor                                  (ORANGEMOD_FUNC_ENTRY->ptr_LCD_SetBkColor       )
#define LCD_SetColor                                    (ORANGEMOD_FUNC_ENTRY->ptr_LCD_SetColor         )
#define LCD_SetPixelIndex                               (ORANGEMOD_FUNC_ENTRY->ptr_LCD_SetPixelIndex    )
#define LCD_SetDrawMode                                 (ORANGEMOD_FUNC_ENTRY->ptr_LCD_SetDrawMode      )
#define LCD_SetFontMode                                 (ORANGEMOD_FUNC_ENTRY->ptr_LCD_SetFontMode  )
#define LCD_SetColorIndex                               (ORANGEMOD_FUNC_ENTRY->ptr_LCD_SetColorIndex    )
#define LCD_SetBkColorIndex                             (ORANGEMOD_FUNC_ENTRY->ptr_LCD_SetBkColorIndex  )
#define LCD_FillRect                                    (ORANGEMOD_FUNC_ENTRY->ptr_LCD_FillRect )
#define LCD_MixColors256                                (ORANGEMOD_FUNC_ENTRY->ptr_LCD_MixColors256 )
#define LCD_GetPixelColor                               (ORANGEMOD_FUNC_ENTRY->ptr_LCD_GetPixelColor    )
#define LCD_GetPixelIndex                               (ORANGEMOD_FUNC_ENTRY->ptr_LCD_GetPixelIndex    )
#define LCD_L0_CheckInit                                (ORANGEMOD_FUNC_ENTRY->ptr_LCD_L0_CheckInit )
#define LCD_Color2Index                                 (ORANGEMOD_FUNC_ENTRY->ptr_LCD_Color2Index      )
#define LCD_Index2Color                                 (ORANGEMOD_FUNC_ENTRY->ptr_LCD_Index2Color      )
#define GUI_LyrMove                                     (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrMove              )
#define GUI_LyrLock                                     (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrLock  )
#define GUI_LyrUnlock                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrUnlock    )
#define GUI_LyrLockAccept                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrLockAccept    )
#define GUI_LyrInitOS                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrInitOS    )
#define GUI_HavePendingMessage                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_HavePendingMessage   )
#define GUI_BroadcastMessage                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_BroadcastMessage     )
#define GUI_PostSyncMessage                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_PostSyncMessage      )
#define GUI_SendMessage                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SendMessage          )
#define GUI_SendMessageTimeout                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SendMessageTimeout   )
#define GUI_PostMessage                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_PostMessage          )
#define GUI_SendNotifyMessage                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SendNotifyMessage    )
#define GUI_GetMessage                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetMessage           )
#define GUI_GetMessageEx                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetMessageEx         )
#define GUI_MsgSetRepeatTimes                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MsgSetRepeatTimes    )
#define GUI_QueryMessage                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_QueryMessage         )
#define GUI_PeekMessage                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_PeekMessage          )
#define GUI_DispatchMessage                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispatchMessage      )
#define GUI_PostQuitMessage                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_PostQuitMessage      )
#define GUI_SendAsyncMessage                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SendAsyncMessage     )
#define GUI_ThrowAwayMessages                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ThrowAwayMessages    )
#define GUI_SetSyncMsgRetVal                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetSyncMsgRetVal     )
#define GUI_PostSyncSem                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_PostSyncSem          )
#define Lang_Open                                       (ORANGEMOD_FUNC_ENTRY->ptr_Lang_Open)
#define Lang_Read                                       (ORANGEMOD_FUNC_ENTRY->ptr_Lang_Read)
#define Lang_GetStringAddress                           (ORANGEMOD_FUNC_ENTRY->ptr_Lang_GetStringAddress    )
#define Lang_GetStringSize                              (ORANGEMOD_FUNC_ENTRY->ptr_Lang_GetStringSize   )
#define Lang_GetString                                  (ORANGEMOD_FUNC_ENTRY->ptr_Lang_GetString   )
#define Lang_Close                                      (ORANGEMOD_FUNC_ENTRY->ptr_Lang_Close   )
#define OpenRes                                         (ORANGEMOD_FUNC_ENTRY->ptr_OpenRes  )
#define CloseRes                                        (ORANGEMOD_FUNC_ENTRY->ptr_CloseRes     )
#define GetResAddr                                      (ORANGEMOD_FUNC_ENTRY->ptr_GetResAddr   )
#define ReadRes                                         (ORANGEMOD_FUNC_ENTRY->ptr_ReadRes      )
#define GetResType                                      (ORANGEMOD_FUNC_ENTRY->ptr_GetResType   )
#define GetResSize                                      (ORANGEMOD_FUNC_ENTRY->ptr_GetResSize   )
#define GetRes                                          (ORANGEMOD_FUNC_ENTRY->ptr_GetRes       )
#define GUI_SetFrameColor8bpp32                         (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetFrameColor8bpp32)
#define GUI_ManWinCreate                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ManWinCreate             )   /*manwin*/
#define GUI_ManWinDelete                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ManWinDelete             )
#define GUI_SetActiveManWin                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetActiveManWin          )
#define GUI_GetActiveManWin                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetActiveManWin          )
#define GUI_ManWinDefaultProc                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ManWinDefaultProc        )
#define GUI_GetTouchFocusWin                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetTouchFocusWin         )
#define GUI_SceenPointInWin                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SceenPointInWin          )
#define GUI_FrmWinCreate                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_FrmWinCreate             )   /*frmwin*/
#define GUI_FrmWinDelete                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_FrmWinDelete             )
#define GUI_FrmWinDefaultProc                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_FrmWinDefaultProc        )
#define GUI_CtrlWinCreate                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_CtrlWinCreate            )   /*ctrlwin*/
#define GUI_CtrlWinDelete                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_CtrlWinDelete            )
#define GUI_CtrlWinGetAddData                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_CtrlWinGetAddData        )
#define GUI_CtrlWinSetAddData                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_CtrlWinSetAddData        )
#define GUI_CtrlWinGetClassName                         (ORANGEMOD_FUNC_ENTRY->ptr_GUI_CtrlWinGetClassName      )
#define GUI_CtrlWinDefaultProc                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_CtrlWinDefaultProc       )
#define GUI_MEMDEV_Create                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_Create        )       /*memdev*/
#define GUI_MEMDEV_CreateEx                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_CreateEx      )
#define GUI_MEMDEV_CreateFixed                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_CreateFixed   )
#define GUI_MEMDEV_Clear                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_Clear             )
#define GUI_MEMDEV_CopyFromLCD                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_CopyFromLCD       )
#define GUI_MEMDEV_CopyToLCD                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_CopyToLCD         )
#define GUI_MEMDEV_CopyToLCDAt                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_CopyToLCDAt       )
#define GUI_MEMDEV_CompareWithLCD                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_CompareWithLCD    )
#define GUI_MEMDEV_Delete                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_Delete            )
#define GUI_MEMDEV_GetXSize                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_GetXSize          )
#define GUI_MEMDEV_GetYSize                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_GetYSize          )
#define GUI_MEMDEV_ReduceYSize                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_ReduceYSize       )
#define GUI_MEMDEV_Select                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_Select    )
#define GUI_MEMDEV_SetOrg                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_SetOrg            )
#define GUI_MEMDEV_WriteAt                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_WriteAt           )
#define GUI_MEMDEV_Write                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_Write             )
#define GUI_MEMDEV_WriteExAt                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_WriteExAt         )
#define GUI_MEMDEV_WriteEx                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_WriteEx           )
#define GUI_MEMDEV_GetDataPtr                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_MEMDEV_GetDataPtr        )
#define GUI_DlgGetFirstButton                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DlgGetFirstButton        ) /*dialog*/
#define GUI_ModuleDialogCreate                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ModuleDialogCreate       )
#define GUI_EndDialog                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_EndDialog                )
#define GUI_GetDlgItem                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetDlgItem               )
#define GUI_DialogDefaultProc                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DialogDefaultProc        )
#define GUI_ModulelessDialogCreate                      (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ModulelessDialogCreate   )
#define GUI_ModulelessDialogDelete                      (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ModulelessDialogDelete   )
#define GUI_GetNextDlgTabItem                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetNextDlgTabItem        )
#define GUI_AddNewControlClass                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_AddNewControlClass   )       /*widget*/
#define GUI_SetCtrlClassAddData                         (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetCtrlClassAddData  )
#define GUI_EmptyControlClassInfoTable                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_EmptyControlClassInfoTable   )
#define GUI_GetCtrlClassAddData                         (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GetCtrlClassAddData  )
#define GUI_DeleteControlClass                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DeleteControlClass   )
#define GUI_TerminateControlClass                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_TerminateControlClass    )
#define GUI_LongStringCreate                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LongStringCreate         ) /*longname*/
#define GUI_LongStringDelete                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LongStringDelete         )
#define GUI_LongStringStart                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LongStringStart          )
#define GUI_LongStringStop                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LongStringStop           )
#define GUI_LongStringReset                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LongStringReset          )
#define GUI_SFT_CreateFont                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SFT_CreateFont   )               /*SFT*/
#define GUI_SFT_ReleaseFont                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SFT_ReleaseFont  )
#define GUI_TTF_CreateFont                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_TTF_CreateFont   )           /*TTF*/
#define GUI_TTF_GetFamilyName                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_TTF_GetFamilyName    )
#define GUI_TTF_GetStyleName                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_TTF_GetStyleName )
#define GUI_TTF_Done                                    (ORANGEMOD_FUNC_ENTRY->ptr_GUI_TTF_Done )
#define RegisterButtonControl                           (ORANGEMOD_FUNC_ENTRY->ptr_RegisterButtonControl            )/*button*/
#define GUI_ButtonSetFocusBmp                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ButtonSetFocusBmp            )
#define GUI_ButtonSetUnFocusBmp                         (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ButtonSetUnFocusBmp          )
#define GUI_ButtonSetText                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ButtonSetText                )
#define GUI_ButtonSetUnFcsTxtColor                      (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ButtonSetUnFcsTxtColor       )
#define GUI_ButtonSetFcsTxtColor                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ButtonSetFcsTxtColor         )
#define GUI_ButtonSetBkColor                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ButtonSetBkColor             )
#define GUI_ButtonSetBmpPos                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ButtonSetBmpPos              )
#define GUI_ButtonSetTxtPos                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_ButtonSetTxtPos              )
#define RegisterStaticControl                           (ORANGEMOD_FUNC_ENTRY->ptr_RegisterStaticControl            )   /*static*/
#define GUI_StaticSetFocusBmp                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_StaticSetFocusBmp            )
#define GUI_StaticSetUnFocusBmp                         (ORANGEMOD_FUNC_ENTRY->ptr_GUI_StaticSetUnFocusBmp          )
#define GUI_StaticSetText                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_StaticSetText                )
#define GUI_StaticSetUnFcsTxtColor                      (ORANGEMOD_FUNC_ENTRY->ptr_GUI_StaticSetUnFcsTxtColor       )
#define GUI_StaticSetFcsTxtColor                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_StaticSetFcsTxtColor         )
#define GUI_StaticSetBmpPos                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_StaticSetBmpPos              )
#define GUI_StaticSetTxtPos                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_StaticSetTxtPos              )
#define GUI_StaticStartMove                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_StaticStartMove              )
#define GUI_StaticStopMove                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_StaticStopMove               )
#define GUI_StaticSetCharSet                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_StaticSetCharSet             )
#define GUI_LstmGetCurItem                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LstmGetCurItem       )   /*listmenu*/
#define GUI_LstmGetItemNo                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LstmGetItemNo        )
#define GUI_LstmSetCurNo                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LstmSetCurNo         )
#define GUI_LstmDrawCurNo                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LstmDrawCurNo        )
#define GUI_LstmStopSlip                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LstmStopSlip         )
#define GUI_LstmGetItem                                 (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LstmGetItem          )
#define GUI_LstmUpdateItem                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LstmUpdateItem       )
#define GUI_LstmChildInFb                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LstmChildInFb        )
#define GUI_LstmDelCurItem                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LstmDelCurItem       )
#define GUI_LstmAddItemTail                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LstmAddItemTail      )
#define GUI_LstmGetItemCount                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LstmGetItemCount     )
#define GUI_LstmGetItemLyrWin                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LstmGetItemLyrWin    )
#define GUI_LstmGetItemRect                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LstmGetItemRect      )
#define GUI_LstmGetItemAttr                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LstmGetItemAttr      )
#define GUI_LstmGetHostWin                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LstmGetHostWin           )
#define RegisterListMenuControl                         (ORANGEMOD_FUNC_ENTRY->ptr_RegisterListMenuControl  )
#define RegisterProgSheetControl                        (ORANGEMOD_FUNC_ENTRY->ptr_RegisterProgSheetControl     )   /*progsheet*/
#define RegisterSliderControl                           (ORANGEMOD_FUNC_ENTRY->ptr_RegisterSliderControl    )       /*slider*/
#define SLIDER_SetRange                                 (ORANGEMOD_FUNC_ENTRY->ptr_SLIDER_SetRange      )
#define SLIDER_SetValue                                 (ORANGEMOD_FUNC_ENTRY->ptr_SLIDER_SetValue      )
#define SLIDER_GetMin                                   (ORANGEMOD_FUNC_ENTRY->ptr_SLIDER_GetMin        )
#define SLIDER_GetMax                                   (ORANGEMOD_FUNC_ENTRY->ptr_SLIDER_GetMax        )
#define SLIDER_GetValue                                 (ORANGEMOD_FUNC_ENTRY->ptr_SLIDER_GetValue      )
#define SLIDER_SetLineSize                              (ORANGEMOD_FUNC_ENTRY->ptr_SLIDER_SetLineSize   )
#define SLIDER_GetLineSize                              (ORANGEMOD_FUNC_ENTRY->ptr_SLIDER_GetLineSize   )
#define bmp_open                                        (ORANGEMOD_FUNC_ENTRY->ptr_bmp_open)
#define bmp_close                                       (ORANGEMOD_FUNC_ENTRY->ptr_bmp_close)
#define bmp_get_bitcount                                (ORANGEMOD_FUNC_ENTRY->ptr_bmp_get_bitcount)
#define bmp_get_width                                   (ORANGEMOD_FUNC_ENTRY->ptr_bmp_get_width)
#define bmp_get_height                                  (ORANGEMOD_FUNC_ENTRY->ptr_bmp_get_height)
#define bmp_get_size                                    (ORANGEMOD_FUNC_ENTRY->ptr_bmp_get_size)
#define bmp_get_matrix                                  (ORANGEMOD_FUNC_ENTRY->ptr_bmp_get_matrix)
#define GUI_TxtboxSetStamp                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_TxtboxSetStamp       )   /*txtbox*/
#define GUI_TxtboxOnDraw                                (ORANGEMOD_FUNC_ENTRY->ptr_GUI_TxtboxOnDraw         )
#define GUI_TxtboxSetMode                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_TxtboxSetMode            )
#define GUI_TxtboxGetMode                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_TxtboxGetMode        )
#define GUI_TxtBoxSetPara                               (ORANGEMOD_FUNC_ENTRY->ptr_GUI_TxtBoxSetPara        )
#define RegisterTxtboxControl                           (ORANGEMOD_FUNC_ENTRY->ptr_RegisterTxtboxControl    )
#define GUI_GMsgBox                                     (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GMsgBox  )               /*gmsgbox*/
#define GUI_GMsgBoxSetText                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GMsgBoxSetText   )
#define GUI_GMsgBoxSetTextPos                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_GMsgBoxSetTextPos    )
#define RegisterSpinboxControl                          (ORANGEMOD_FUNC_ENTRY->ptr_RegisterSpinboxControl   )   /*spinbox*/
#define GUI_SpinboxGetCurValue                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SpinboxGetCurValue   )
#define GUI_SpinboxSetCurValue                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SpinboxSetCurValue   )
#define GUI_SpinboxSetMaxValue                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SpinboxSetMaxValue   )
#define GUI_SpinboxSetMinValue                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SpinboxSetMinValue   )
#define GUI_SpinboxSetBkColor                           (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SpinboxSetBkColor    )
#define GUI_SpinboxSetUnFcsTxtColor                     (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SpinboxSetUnFcsTxtColor  )
#define GUI_SpinboxSetFcsTxtColor                       (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SpinboxSetFcsTxtColor    )
#define GUI_SpinboxSetFocusUpBmp                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SpinboxSetFocusUpBmp )
#define GUI_SpinboxSetFocusDownBmp                      (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SpinboxSetFocusDownBmp   )
#define GUI_SpinboxSetUnFocusUpBmp                      (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SpinboxSetUnFocusUpBmp   )
#define GUI_SpinboxSetUnFocusDownBmp                    (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SpinboxSetUnFocusDownBmp )
#define GUI_SpinboxSetUpBmpPos                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SpinboxSetUpBmpPos   )
#define GUI_SpinboxSetDownBmpPos                        (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SpinboxSetDownBmpPos )
#define RegisterProgbarControl                          (ORANGEMOD_FUNC_ENTRY->ptr_RegisterProgbarControl   )   /*progbar*/
#define PROGBAR_SetRange                                (ORANGEMOD_FUNC_ENTRY->ptr_PROGBAR_SetRange     )
#define PROGBAR_SetValue                                (ORANGEMOD_FUNC_ENTRY->ptr_PROGBAR_SetValue     )
#define PROGBAR_GetMin                                  (ORANGEMOD_FUNC_ENTRY->ptr_PROGBAR_GetMin       )
#define PROGBAR_GetMax                                  (ORANGEMOD_FUNC_ENTRY->ptr_PROGBAR_GetMax       )
#define PROGBAR_GetValue                                (ORANGEMOD_FUNC_ENTRY->ptr_PROGBAR_GetValue     )
#define SListbox_UpdateMenulist                         (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_UpdateMenulist          )   /*slistbox*/
#define SListbox_GetItemNo                              (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_GetItemNo               )
#define SListbox_GetItemAttr                            (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_GetItemAttr         )
#define SListbox_GetHostWin                             (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_GetHostWin              )
#define SListbox_GetItemLyrWin                          (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_GetItemLyrWin           )
#define SListbox_GetItemCount                           (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_GetItemCount            )
#define SListbox_GetItem                                (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_GetItem             )
#define SListbox_GetCurItem                             (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_GetCurItem              )
#define SListbox_PrevPage                               (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_PrevPage                )
#define SListbox_NextPage                               (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_NextPage                )
#define SListbox_ItemDisable                            (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_ItemDisable             )
#define SListbox_ItemEnable                             (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_ItemEnable              )
#define SListbox_GetHeadindex                           (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_GetHeadindex            )
#define SListbox_DelCurItem                             (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_DelCurItem              )
#define SListbox_AddTail                                (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_AddTail                 )
#define SListbox_SetCurItem                             (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_SetCurItem              )
#define SListbox_Disable                                (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_Disable                 )
#define SListbox_GetItemRect                            (ORANGEMOD_FUNC_ENTRY->ptr_SListbox_GetItemRect             )
#define RegisterListBoxControl                          (ORANGEMOD_FUNC_ENTRY->ptr_RegisterListBoxControl           )
#define UnRegisterListBoxControl                        (ORANGEMOD_FUNC_ENTRY->ptr_UnRegisterListBoxControl         )
#define GUI_Fb2Dispfb                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_Fb2Dispfb        )
#define GUI_Dispfb2Fb                                   (ORANGEMOD_FUNC_ENTRY->ptr_GUI_Dispfb2Fb        )
#define GUI_LogicLyr2DispLyr                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LogicLyr2DispLyr         )
#define GUI_DispLyr2LogicLyr                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispLyr2LogicLyr         )
#define GUI_BMP_DrawEx                                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_BMP_DrawEx       )
#if (defined CONFIG_SOC_SUN3IW2 ||defined CONFIG_SOC_SUN8IW19 || defined CONFIG_SOC_SUN20IW1)
#define GUI_Memdev_FrameBuffer_Release                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_Memdev_FrameBuffer_Release       )
//#define GUI_BitString_Draw2                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_BitString_Draw2  )
#elif defined CONFIG_SOC_SUN3IW1
//#define GUI_SetFrameColor32bpp                          (ORANGEMOD_FUNC_ENTRY->ptr_GUI_SetFrameColor32bpp)
#define GUI_LyrWinGetHWLyr                              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_LyrWinGetHWLyr       )
#endif
#define GUI_CreateAutoPanel                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_CreateAutoPanel          )
#define GUI_DestroyAutoPanel                            (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DestroyAutoPanel         )
#define GUI_BMP_Draw_Rotate                             (ORANGEMOD_FUNC_ENTRY->ptr_GUI_BMP_Draw_Rotate             )
#endif
#endif
