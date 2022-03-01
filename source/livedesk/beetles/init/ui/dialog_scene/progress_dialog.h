/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : progress_dailog.h
* By        : Andy.zhang
* Func      : progress dailog
* Version   : v1.0
* ============================================================================================================
* 2009-10-15 15:27:26 andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __PROGRESS_DAILOG_H__
#define __PROGRESS_dialog_H__


/* 模式进度对话框 */

typedef struct tag_pdialog_create
{
    int             text_id;
    __u16           dialog_id;
} pdialog_create_t;

H_WIN progress_dialog_create(H_WIN owner, H_WIN layer, pdialog_create_t *para);

#endif /* __PROGRESS_DAILOG_H__ */
