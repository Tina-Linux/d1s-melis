/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : mod_init_i.h
* By        : Andy.zhang
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2010-9-25 17:09:34  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __MOD_INIT_I_H__
#define __MOD_INIT_I_H__
#include "mod_update.h"
//#include "sunxi_drv_watchdog.h"
#include "apps.h"

#define THEM_BIN_PATH   BEETLES_APP_ROOT"apps\\theme.bin"

#define NOTIFY_MSG(mid, src, des, d1, d2) \
    do{\
        __gui_msg_t  exmsg; \
        exmsg.id = mid; \
        exmsg.h_srcwin = src; \
        exmsg.h_deswin = des; \
        exmsg.dwAddData1= (__u32)d1; \
        exmsg.dwAddData2 = (__u32)d2; \
        GUI_SendNotifyMessage(&exmsg); \
    }while(0)

#define SEND_MSG(mid, src, des, d1, d2) \
    do{\
        __gui_msg_t  exmsg; \
        exmsg.id = mid; \
        exmsg.h_srcwin = src; \
        exmsg.h_deswin = des; \
        exmsg.dwAddData1= (__u32)d1; \
        exmsg.dwAddData2 = (__u32)d2; \
        GUI_SendMessage(&exmsg); \
    }while(0)

#define POST_MSG(mid, src, des, d1, d2) \
    do{\
        __gui_msg_t  exmsg; \
        exmsg.id = mid; \
        exmsg.h_srcwin = src; \
        exmsg.h_deswin = des; \
        exmsg.dwAddData1= (__u32)d1; \
        exmsg.dwAddData2 = (__u32)d2; \
        GUI_PostMessage(&exmsg); \
    }while(0)

typedef struct
{
    __u32           mid;
    __u32           init_tid;
} __init_mod_t;

__s32 INIT_MInit(void);
__s32 INIT_MExit(void);
__mp *INIT_MOpen(__u32 mid, __u32 mode);
__s32 INIT_MClose(__mp *mp);
__u32 INIT_MRead(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 INIT_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
long INIT_MIoctrl(__mp *mp, uint32_t cmd, int32_t aux, void *pbuffer);
#endif /* __MOD_INIT_I_H__ */


