/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <arch.h>
#include <log.h>
#include "ktype.h"
#include <hal_interrupt.h>


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
    hal_enable_irq(irqno);
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
    hal_disable_irq(irqno);

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
    ret = hal_request_irq(irq, handler, (name == NULL)?"noname":name, argv);
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
int32_t esINT_UniISR(uint32_t irq)
{
    unsigned long cpu_sr;

    cpu_sr = awos_arch_lock_irq();
    hal_free_irq(irq);
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