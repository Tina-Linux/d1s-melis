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
* File    : fb_ops.h
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-4 11:24:32
**********************************************************************************************************************
*/
#ifndef __FB_OPS_H__
#define __FB_OPS_H__

typedef enum tag_GUI_FB_STA
{
    GUI_FB_STA_WAKE,
    GUI_FB_STA_SLEEP,
    GUI_FB_STA_ERR
} __gui_fb_sta_t;

__s32 GUI_FBSuspend(FB *fb);
__s32 GUI_FBResume(FB *fb);

__gui_fb_sta_t GUI_FBGetStatus(FB *fb);
__s32          GUI_FBFlushCache(FB *fb);

#endif /* __FB_OPS_H__ */
