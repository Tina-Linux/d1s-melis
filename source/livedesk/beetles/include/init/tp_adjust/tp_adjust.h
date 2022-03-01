/*
**************************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              FM app sample
*
*                                   (c) Copyright 2006-2007, ANDY, China
*                                            All Rights Reserved
*
* File    : lion_adjust.h
* By      :
* Version : V1.00
* time    : 2009-8-14
**************************************************************************************************************
*/
#ifndef __TP_ADJUST_H__
#define __TP_ADJUST_H__

/***************************************************/
int         tpdrv_open(void);

__s32       tpdrv_close(int hdle);

__s32       tp_adjust(int hdle, __u8 cnt, void *pbuf);


#endif /* __TP_ADJUST_H__ */


