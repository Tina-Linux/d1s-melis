/*
*********************************************************************************************************
*                                                   ePDK
*                                  the Easy Portable/Player Development Kits
*                                              elibs sub-system
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File   : pub0.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/
#ifndef _PUB0_H_
#define _PUB0_H_
#include "pub0/elibs_cacheop.h"
#include "pub0/elibs_reg.h"
#include <arch.h>

#ifdef __cplusplus
extern "C" {
#endif

/*critical operation    */
#define ENTER_CRITICAL(__cpsr) { __cpsr = awos_arch_lock_irq(); }
#define EXIT_CRITICAL(__cpsr) { awos_arch_unlock_irq(__cpsr);  }

#ifdef __cplusplus
}
#endif
#endif /* end of stdio.h */
