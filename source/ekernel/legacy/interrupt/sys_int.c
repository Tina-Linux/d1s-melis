/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                           Interrupt manage Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : sys_int.c
* By      : kevin.z
* Version : v1.0
* Date    : 2010-11-24 16:45
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-24 16:45    kevin.z     1.0     build the file.
*********************************************************************************************************
*/
#include <arch.h>
#include <log.h>
#include "ktype.h"
#include "interrupt.h"


/*
*********************************************************************************************************
*                                   SET IRQ PRIORITY
*
* Description: set irq priority;
*
* Arguments  : irqno    number of irq;
*              prio     priority of irq;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       : priority is just processed by software.
*********************************************************************************************************
*/
int32_t esINT_SetIRQPrio(uint32_t irqno, uint32_t prio)
{
    /*plic set all priority as 1 through plic_irq_toggle function*/
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   ENABLE INTERRUPT
*
* Description: enable interrupt.
*
* Arguments  : irqno    irq number which need be enable;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esINT_EnableINT(uint32_t irqno)
{
    enable_irq(irqno);
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   DISABLE INTERRUPT
*
* Description: disable interrupt;
*
* Arguments  : irqno    irq number which need be disable;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esINT_DisableINT(uint32_t irqno)
{
    disable_irq(irqno);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   INSTALL ISR
*
* Description: install isr;
*
* Arguments  : irqno    irq number;
*              pIsr     isr handler;
*              pArg     argument for isr;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esINT_InsISR(uint32_t irq, char* name, void *handler, void *argv)
{
    unsigned long  cpu_sr;
    int32_t    ret = 0;

    cpu_sr = awos_arch_lock_irq();
    ret = request_irq(irq, handler, 0, (name == NULL)?"noname":name, argv);
    awos_arch_unlock_irq(cpu_sr);

    return ret;
}


/*
*********************************************************************************************************
*                                   UNINSTALL ISR
*
* Description: uninstall isr;
*
* Arguments  : irqno    irq numer;
*              pIsr     isr handler;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esINT_UniISR(uint32_t irq, void *dev_id)
{
    unsigned long cpu_sr;

    cpu_sr = awos_arch_lock_irq();
    free_irq(irq, dev_id);
    awos_arch_unlock_irq(cpu_sr);
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                   INSTALL FIRQ SERVER
*
* Description: install the firq server.
*
* Arguments  : fiqno        firq number.
*              pFirMain     main process of the firq server.
*              pFirTail     tail process of the firq server.
*              pArg         argument of the firq server.
*
* Returns    : result.
*                   EPDK_OK,    install isr successed.
*                   EPDK_FAIL,  install isr failed.
*
* Note       :
*********************************************************************************************************
*/
int32_t esINT_InsFIR(uint32_t fiqno, __pISR_t pFirMain, __pCBK_t pFirTail, void *pArg)
{
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   UNINSTALL THE FIRQ SERVER
*
* Description: uninstall firq server.
*
* Arguments  : fiqno        firq number.
*              pFirMain     firq server main process.
*              pFirTail     firq server tail process.
*
* Returns    : result
*                   EPDK_OK,    uninstall firq server successed.
*                   EPDK_FAIL,  uninstall firq server failed.
*
* Note       :
*********************************************************************************************************
*/
int32_t esINT_UniFIR(uint32_t fiqno, __pISR_t pFirMain, __pCBK_t pFirTail)
{
    return EPDK_OK;
}

