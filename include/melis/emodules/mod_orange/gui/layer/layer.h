/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lion
* File    : layer.h
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-4 11:28:56
**********************************************************************************************************************
*/
#ifndef __GUI_LYR_H__
#define __GUI_LYR_H__

#define MAX_BKNAME   50

typedef enum tag_GUI_LYR_STA
{
    GUI_LYR_STA_ON = 0,                            /* on                                                           */
    GUI_LYR_STA_OFF,                               /* off                                                          */
    GUI_LYR_STA_SLEEP,                             /* off, layer released, but frame buffer not free               */
    GUI_LYR_STA_SUSPEND,                           /* off, layer released, frame buffer free                       */
    GUI_LYR_STA_ERR
} __gui_lyr_sta_t;

typedef enum tag_GUI_LYR_SLEEP_MOD
{
    GUI_LYR_SLEEP_DEEP,
    GUI_LYR_SLEEP_SHALLOW
} __gui_lyr_sleep_mode_t;

typedef enum tag_GUI_BACK_SRC
{
    GUI_BACK_FILE,
    GUI_BACK_FB
} __gui_back_src_t;

/* information for create a new layer               */
typedef struct __GUI_LYRCREATE_INFO
{
    char                 *name;

    __disp_layer_para_t *lyrpara;                   /* used to request a hw layer                                   */

    __gui_lyr_sta_t      initsta;                  /* init status of lion layer                                    */

} __gui_lyrcreate_info_t;



/* global functions                                 */
//extern H_LYR  GUI_LyrCreate        (__gui_lyrcreate_info_t * create_info);
//extern __s32  GUI_LyrDelete        (H_LYR h_lyr);
//
//extern __s32  GUI_LyrRegWin        (H_WIN h_win);
//extern __s32  GUI_LyrUnregWin      (H_WIN h_win);
//
//extern __s32  GUI_LyrSetSrcWindow  (H_LYR h_lyr, const RECT * rect);
//extern __s32  GUI_LyrSetScnWindow  (H_LYR h_lyr, const RECT * rect);
//extern __s32  GUI_LyrGetSrcWindow  (H_LYR h_lyr, RECT * rect);
//extern __s32  GUI_LyrGetScnWindow  (H_LYR h_lyr, RECT * rect);
//
//extern H_LYR  GUI_LyrGetBackLyr       (void);
//
extern __s32  GUI_LyrMove(H_LYR h_lyr, __s32 x, __s32 y);
//
//extern __s32  GUI_LyrSetFB         (H_LYR h_lyr, const FB * fb);
//__s32           GUI_LyrSetBackFB     (__gui_back_src_t mode,void *src);
//extern __s32  GUI_LyrGetFB         (H_LYR h_lyr, FB * fb);
//
//extern __s32  GUI_LyrSetClipRect   (RECT * p_rect);
//
//extern __s32  GUI_LyrSel           (H_LYR h_lyr);
//
//extern __s32  GUI_LyrSetFocus      (H_LYR h_lyr);
//
//extern __s32  GUI_LyrSetFocusWin   (H_WIN h_win);
//extern __s32  GUI_LyrCacheOn       (void);
//extern __s32  GUI_LyrCacheOff      (void);
//
//extern __s32  GUI_LyrSetTop        (H_LYR h_lyr);
//extern __s32  GUI_LyrSetBottom     (H_LYR h_lyr);
//
//extern __s32  GUI_LyrCKOn          (H_LYR h_lyr);
//extern __s32  GUI_LyrCKOff         (H_LYR h_lyr);
//
//extern __gui_lyr_sta_t GUI_LyrGetSta(H_LYR h_lyr);
//extern __s32           GUI_LyrSetSta(H_LYR h_lyr, __gui_lyr_sta_t status);
//
//extern __s32           GUI_LyrFlushFB(H_LYR h_lyr);
//
//extern __s32 GUI_LyrSetAlpha(H_LYR h_lyr, __u8 alpha);
//extern __s32 GUI_LyrAlphaOn(H_LYR h_lyr);
//extern __s32 GUI_LyrAlphaOff(H_LYR h_lyr);

#endif /* #ifndef __GUI_LYR_H__ */
