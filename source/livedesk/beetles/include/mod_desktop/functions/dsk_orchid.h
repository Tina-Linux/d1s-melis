/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : dsk_orchid.h
* By        : Andy.zhang
* Func      : orchid op
* Version   : v1.0
* ============================================================================================================
* 2010-1-8 9:46:40  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __DSK_ORCHID_H__
#define __DSK_ORCHID_H__

///* 插拔usb 之后，需要调此接口更新媒体库 */
//__s32 dsk_orchid_notify_update(void);

/* 每次启动后，需要调此接口, 重建媒体库 */
__s32 dsk_orchid_check(void);

/* 每次启动后，需要调此接口, 格式化 z 盘 */
__s32 dsk_orchid_createDisk(void);

__s32 dsk_orchid_save_npl(void);

__s32 dsk_orchid_load_last_npl(void);

__s32 dsk_orchid_update(void);

#endif /* __DSK_ORCHID_H__ */
