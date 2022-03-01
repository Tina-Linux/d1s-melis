/*
*********************************************************************************************************
*                                                   ePOS
*                               the Easy Portable/Player Operation System
*                                              eBIOS sub-system
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File   : sys_sios_i.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/
#ifndef _SYS_SIOS_I_H_
#define _SYS_SIOS_I_H_
#include <typedef.h>

typedef struct  __DBGUART_FUNC_SET
{
    uint8_t (*UART_getchar)(void);
    void    (*UART_putstr)(const char *str);
    void    (*UART_gets)(char *str);
    void    (*UART_setbaud)(const uint32_t baud);
    void    (*UART_putarg)(uint32_t arg, char format);
} __dbguart_func_set_t;

#endif //#ifndef _SYS_SIOS_I_H_

