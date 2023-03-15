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
#include "touchscreendev_i.h"
#include <kapi.h>
#include <string.h>
#include <log.h>

/* define logical tp device manager handle */
static __lts_dev_man_t  LTSDevMan;


/*
 * logical touchpanel device operations, no-op.
 */
static __hdle LTSDevOpen(void *open_arg, uint32_t mode)
{
    return NULL;
}

static int32_t LTSDevClose(__hdle hKey)
{
    return 0;
}

static uint32_t LTSDevRead(void *pdata, uint32_t size, uint32_t n, __hdle hKey)
{
    return 0;
}

static uint32_t LTSDevWrite(const void *pdata, uint32_t size, uint32_t n, __hdle hKey)
{
    return 0;
}

static int32_t LTSDevIoctrl(__hdle hKey, uint32_t cmd, long aux, void *pbuffer)
{
    return 0;
}

const __dev_devop_t LTSDevOps =
{
    LTSDevOpen,
    LTSDevClose,
    LTSDevRead,
    LTSDevWrite,
    LTSDevIoctrl
};

int32_t LTSDevMan_Lock(void)
{
    esKRNL_SemPend(LTSDevMan.lock, 0, NULL);

    return EPDK_OK;
}

int32_t LTSDevMan_Unlock(void)
{
    esKRNL_SemPost(LTSDevMan.lock);

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                     LOGICAL KEY DEVICE EVENT DISPATCH THEARD
*
* Description: packet logical touchpanel devcie events,
*              dispatch packet event information to all grabers of logical touchpanel device.
*
* Arguments  : p_arg   logical touchpanel device handle.
*
* Returns    : NONE.
*********************************************************************************************************
*/
static void LTSDevGraberThread(void *p_arg)
{
    __input_ldev_t              *ldev = (__input_ldev_t *)p_arg;
    __input_graber_t            *pGraber;
    __input_event_packet_t      *pPacket;
    uint32_t                    cpu_sr;
    int32_t                     i, j;

    pPacket = &(LTSDevMan.packet);
    while (1)
    {
        esKRNL_SemPend(ldev->ev_sem, 0, NULL);

        if (ldev->ev_len == 0)
        {
            /* event buffer is empty */
            continue;
        }

        /*  packet logical key device event buffer to pPacket buffer.
         *  event buffer data format:
         *  -------------------------------------------------------
         *  |lid |pid |type|code|value|......|type|code|value|
         *  -------------------------------------------------------
         *  this process can't been interrupt.
         */
        cpu_sr = awos_arch_lock_irq();

        /* set pid and lid */
        j = ldev->ev_tail;
        pPacket->ldev_id = ldev->evbuf[j];
        j = (j + 1) % ldev->ev_maxlen;
        ldev->ev_len--;
        pPacket->pdev_id = ldev->evbuf[j];
        j = (j + 1) % ldev->ev_maxlen;
        ldev->ev_len--;

        /* physical device send events length no use now,
         * just skip it, By sunny at 2010-9-26 9:18:46.
         */
        j = (j + 1) % ldev->ev_maxlen;
        ldev->ev_len--;

        /* event encode format : |type|code|value|,
         * so the event length must be aligned by 3.
         */
        if ((ldev->ev_len % 3) || (ldev->ev_len == 0))
        {
            __wrn("logical touchpanel deivce event buffer invalid");
            continue;
        }
        i = 0;
        pPacket->event_cnt = 0;
        while ((i < ldev->ev_len) && (pPacket->event_cnt < INPUT_MAX_EVENT_NR))
        {
            pPacket->events[pPacket->event_cnt].type    = ldev->evbuf[j];
            j = (j + 1) % ldev->ev_maxlen;

            pPacket->events[pPacket->event_cnt].code    = ldev->evbuf[j];
            j = (j + 1) % ldev->ev_maxlen;

            pPacket->events[pPacket->event_cnt].value   = ldev->evbuf[j];
            j = (j + 1) % ldev->ev_maxlen;

            /* type + code + value */
            i = i + 3;

            if (pPacket->events[pPacket->event_cnt].type == EV_SYN)
            {
                /* syn event find : end of event buffer */
                pPacket->event_cnt++;
                break;
            }
            pPacket->event_cnt++;
        }
        ldev->ev_len -= i;
        if (ldev->ev_len == 0)
        {
            /* logical device event buffer is empty,
             * we should reset event buffer head and tail.
             */
            ldev->ev_tail = 0;
            ldev->ev_head = 0;
        }
        else
        {
            ldev->ev_tail = j;
        }
        awos_arch_unlock_irq(cpu_sr);

        if (pPacket->event_cnt < 2)
        {
            /* we should filter invalid events packet here,
             * invalid packet: have no any useful event information.
             * by sunny at 2011-1-10 13:25:13.
             */
            __wrn("filter invalid event packet!");
            continue;
        }

        /* submit packet events to all grabers*/
        pGraber = ldev->grabers;
        while (pGraber)
        {
            pPacket->pGrabArg = pGraber->pGrabArg;
            esKRNL_CallBack(pGraber->callback, (void *)pPacket);
            pGraber = pGraber->next;
        }
    }
}


/*
*********************************************************************************************************
*                                     LOGICAL KEY DEVICE EVENT HANDLER
*
* Description: logical touchpanel device event handler.
*
* Arguments  :  dev     physical input device handle.
*               type    event type
*               code    event code
*               value   event value
*
* Returns    : register logic input device result;
*               EPDK_OK     -   handle physical device event successed.
*               EPDK_FAIL   -   handle physical device event failed.
*********************************************************************************************************
*/
static int32_t LTSDevEvent(__input_dev_t *dev, uint32_t type, uint32_t code, int32_t value)
{
    __input_ldev_t  *ldev;
    uint32_t        cpu_sr;
    int32_t         i, j;

    if (dev == NULL)
    {
        __wrn("invalid physical device handle for LTSDevEvent");
        return EPDK_FAIL;
    }

    /* save physical device event to logical device event buffer */
    if ((dev->ev_pos + 3) > INPUT_EV_BUFFER_MAXLEN)
    {
        __wrn("phyical input device [%s] event buffer full!", dev->name);
        return EPDK_FAIL;
    }
    i = dev->ev_pos;
    dev->evbuf[i++] = type;
    dev->evbuf[i++] = code;
    dev->evbuf[i++] = value;
    dev->ev_pos     = i;

    /* not a sync event, continue to receive physical device event */
    if (type != EV_SYN)
    {
        return EPDK_OK;
    }

    ldev = (__input_ldev_t *)dev->ldevp;

    /*
     * check logical device event buffer have enough k_free space.
     * pdev_id, ldev_id, len consume three.
     */
    i = i + 3;
    if (ldev->ev_maxlen - ldev->ev_len < i)
    {
        __wrn("%s event buffer full!", ldev->name);
        return EPDK_FAIL;
    }

    /*
     * save physical touchpanel device event buffer to
     * logical touchpanel device circle event buffer.
     */
    cpu_sr = awos_arch_lock_irq();
    j = ldev->ev_head;

    /* save lid and pid first */
    ldev->evbuf[j] = ldev->seq;
    j = (j + 1) % ldev->ev_maxlen;
    ldev->ev_len++;

    ldev->evbuf[j] = dev->seq;
    j = (j + 1) % ldev->ev_maxlen;
    ldev->ev_len++;

    /* save event length */
    ldev->evbuf[j] = dev->ev_pos;
    j = (j + 1) % ldev->ev_maxlen;
    ldev->ev_len++;

    /*
     * copy physical touchpanel device events to
     * logical touchpanel device circle event buffer.
     */
    for (i = 0; i < dev->ev_pos; i++)
    {
        ldev->evbuf[j] = dev->evbuf[i];
        j = (j + 1) % ldev->ev_maxlen;
    }
    ldev->ev_head   = j;
    ldev->ev_len   += i;

    /*
     * all physical device events have save to logical device circle event buffer,
     * clear physical device event buffer, and invalid device abs(use for defuzz) values.
     */
    dev->ev_pos = 0;
    memset(dev->abs, 0xFF, (ABS_MAX + 1) * sizeof(int));
    awos_arch_unlock_irq(cpu_sr);

    /* wakeup logical touchpanel device event handle thread */
    esKRNL_SemPost(ldev->ev_sem);

    return 0;
}

static const __input_lid_t LTSDevIds[] =
{
    {
        .flags  = INPUT_DEVICE_ID_MATCH_EVBIT  |
        INPUT_DEVICE_ID_MATCH_RELBIT,
        .evbit  = { BIT_MASK(EV_REL) },
        .relbit = { BIT_MASK(REL_X) | BIT_MASK(REL_Y) },
    },  /* two relative axes */
    {
        .flags  = INPUT_DEVICE_ID_MATCH_EVBIT |
        INPUT_DEVICE_ID_MATCH_ABSBIT,
        .evbit  = { BIT_MASK(EV_ABS) },
        .absbit = { BIT_MASK(ABS_X) | BIT_MASK(ABS_Y) },
    },  /* two absolute axes */

    { },    /* Terminating entry */
};

/*
*********************************************************************************************************
*                                     LOGICAL KEY DEVICE INIT
*
* Description:  allocate and initialize logical touchpanel device handle,
*               register logical touchpanel deivce to INPUT manage system,
*               setup logical dispatch thread.
*
* Arguments  :  NONE.
*
* Returns    : initialize logic touchpanel device result;
*               EPDK_OK     -   initalize successed.
*               EPDK_FAIL   -   initalize failed.
*********************************************************************************************************
*/
int32_t INPUT_LTSDevInit(void)
{
    __input_ldev_t *pLTSDev;

    /* allocate space for logical touchpanel device */
    pLTSDev = (__input_ldev_t *)k_malloc(sizeof(struct __INPUT_LDEV));
    if (pLTSDev == NULL)
    {
        __inf("k_malloc for logical devcie failed");
        return EPDK_FAIL;
    }
    memset(pLTSDev, 0, sizeof(struct __INPUT_LDEV));

    /*
     * set logical touchpanel device information,
     * the information for melis general device managmant system.
     */
    pLTSDev->classname = INPUT_CLASS_NAME;
    pLTSDev->name      = INPUT_LTS_DEV_NAME;
    pLTSDev->ops       = &LTSDevOps;
    pLTSDev->parg      = NULL;

    /* set indentify physical input device id table */
    pLTSDev->id_table  = LTSDevIds;

    /* set logical touchpanel device event handler */
    pLTSDev->event     = LTSDevEvent;

    /* initialize event buffer information */
    pLTSDev->ev_sem    = esKRNL_SemCreate(1);
    if (pLTSDev->ev_sem == NULL)
    {
        __wrn("create semaphore logical tp device failed!");
        k_free(pLTSDev);
        return EPDK_FAIL;
    }
    pLTSDev->ev_head   = 0;
    pLTSDev->ev_tail   = 0;
    pLTSDev->ev_len    = 0;
    pLTSDev->ev_maxlen = INPUT_EV_BUFFER_MAXLEN;

    /* register logical device */
    if (esINPUT_RegLdev(pLTSDev) != EPDK_OK)
    {
        __wrn("register logical touchpanel device failed");
        k_free(pLTSDev);
        return EPDK_FAIL;
    }

    /* create logical touchpanel device graber thread */
    pLTSDev->tid = awos_task_create("tp_input", LTSDevGraberThread, pLTSDev, 0x800, CONFIG_RT_TIMER_THREAD_PRIO - 5, 10);
    if (pLTSDev == NULL)
    {
        __wrn("create task fail");
        k_free(pLTSDev);
        return EPDK_FAIL;
    }

    /* attach pLTSDev to LTSDevMan */
    LTSDevMan.LTSDev = pLTSDev;

    /* allocate sem for LTSDevMan lock */
    LTSDevMan.lock = esKRNL_SemCreate(1);
    if (LTSDevMan.lock == NULL)
    {
        __wrn("create semaphore logical tp device failed!");
        k_free(pLTSDev);
        return EPDK_FAIL;
    }

    __inf("logical touchpanel device initialize succeeded!");
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     LOGICAL KEY DEVICE EXIT
*
* Description:  request to delete logical device dispatch thread,
*               unregister logical touchpanel deivce from INPUT manage system,
*               k_free logical touchpanel device handle.
*
* Arguments  :  NONE.
*
* Returns    :  NONE
*********************************************************************************************************
*/
void INPUT_LTSDevExit(void)
{
    awos_task_delete(LTSDevMan.LTSDev->tid);
    esINPUT_UnregLdev(LTSDevMan.LTSDev);
    k_free(LTSDevMan.LTSDev);
}



static int32_t console_buffer[64];
static int32_t valid_key_number = 0;
int32_t console_LTSDevEvent(uint32_t type, uint32_t code, int32_t value)
{
    __input_ldev_t  *ldev;
    unsigned long   cpu_sr;
    int32_t         i, j;


    console_buffer[valid_key_number++]  = type;
    console_buffer[valid_key_number++]  = code;
    console_buffer[valid_key_number++]  = value;

    if(EV_SYN != type)
    {
        __inf("record the keyvalue before syn event!");
        return ;   
    }
    if(LTSDevMan.LTSDev == NULL)
    {
        __log("input sys keydev not init!");
        valid_key_number = 0;
        return;
    }
    ldev =  LTSDevMan.LTSDev;

    /*
     * save physical key device event buffer to
     * logical key device circle event buffer.
     */
    cpu_sr = awos_arch_lock_irq();
    j = ldev->ev_head;

    /* save lid and pid first */
    ldev->evbuf[j]  = ldev->seq;
    j = (j + 1) % ldev->ev_maxlen;
    ldev->ev_len++;

    ldev->evbuf[j]  = 0;
    j = (j + 1) % ldev->ev_maxlen;
    ldev->ev_len++;

    ldev->evbuf[j]  = 0;
    j = (j + 1) % ldev->ev_maxlen;
    ldev->ev_len++;

    /* save event length */
    for(i = 0; i < valid_key_number; i ++)
    {
        ldev->evbuf[j]  = console_buffer[i];
        j = (j + 1) % ldev->ev_maxlen;
        ldev->ev_len++;
    }

    valid_key_number = 0;

    ldev->ev_head   = j;

    /*
     * all physical device events have save to logical device circle event buffer,
     * clear physical device event buffer.
     */
    awos_arch_unlock_irq(cpu_sr);

    /* wakeup logical key device event handle thread */
    
    esKRNL_SemPost(ldev->ev_sem);

    return 0;
}