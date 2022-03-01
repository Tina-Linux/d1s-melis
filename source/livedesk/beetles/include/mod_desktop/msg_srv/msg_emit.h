/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : msg_emit_e.h
* By        : Andy.zhang
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2009-7-24 10:56:33 andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __MSG_EMIT_H__
#define __MSG_EMIT_H__
#include <typedef.h>

typedef __s32(*cb_key_hook)(__gui_msg_t *msg);
typedef __s32(*cb_tp_hook)(__gui_msg_t *msg);
typedef __s32(*cb_init_hook)(__gui_msg_t *msg);
typedef __s32(*cb_tvout_tp_hook)(__gui_msg_t *msg);

typedef struct _memit_hook_t
{
    cb_key_hook     key_hook;
    cb_tp_hook      tp_hook;
    cb_init_hook    init_hook;
} __memit_hook_t;

__s32 msg_emit_init(void);
__s32 msg_emit_deinit(void);
__s32 msg_emit_register_hook(__memit_hook_t *hook);
__s32 msg_emit_register_tvout_tp_hook(cb_tvout_tp_hook hook);

#endif /* __MSG_EMIT_H__ */
