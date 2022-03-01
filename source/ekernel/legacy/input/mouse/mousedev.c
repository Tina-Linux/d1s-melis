/*
*********************************************************************************************************
*                                                    eMOD
*                                         the melis Operation System
*                                               INPUT sub-system
*                                          mouse logic device handler
*
*                                    (c) Copyright 2010-2012, sunny China
*                                              All Rights Reserved
*
* File   : mousedev.c
* Version: V1.0
* By     : Sunny
* Date   : 2010-7-11
* Note   : this module is for general mouse device handler,
*          all mouse class physic devices use this module to dispatch and package message.
*********************************************************************************************************
*/
#include "mousedev_i.h"
#include <kapi.h>
#include <string.h>
#include <log.h>

/* define logical mouse device manager handle */
static __lmouse_dev_man_t  LMouseDevMan;

/*
 * logical mouse device operations, no-op.
 */
static __hdle LMouseDevOpen(void *open_arg, __u32 mode)
{
    return NULL;
}

static int32_t LMouseDevClose(__hdle hKey)
{
    return 0;
}

static uint32_t LMouseDevRead(void *pdata, uint32_t size, uint32_t n, __hdle hKey)
{
    return 0;
}

static uint32_t LMouseDevWrite(const void *pdata, uint32_t size, uint32_t n, __hdle hKey)
{
    return 0;
}

static int32_t  LMouseDevIoctrl(__hdle hKey, uint32_t cmd, long aux, void *pbuffer)
{
    return 0;
}

const __dev_devop_t LMouseDevOps =
{
    LMouseDevOpen,
    LMouseDevClose,
    LMouseDevRead,
    LMouseDevWrite,
    LMouseDevIoctrl
};

int32_t LMouseDevMan_Lock(void)
{
    esKRNL_SemPend(LMouseDevMan.lock, 0, NULL);

    return EPDK_OK;
}

int32_t LMouseDevMan_Unlock(void)
{
    esKRNL_SemPost(LMouseDevMan.lock);

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                     LOGICAL KEY DEVICE EVENT DISPATCH THEARD
*
* Description: packet logical mouse devcie events,
*              dispatch packet event information to all grabers of logical mouse device.
*
* Arguments  : p_arg   logical mouse device handle.
*
* Returns    : NONE.
*********************************************************************************************************
*/
static void LMouseDevGraberThread(void *p_arg)
{
    __input_ldev_t          *ldev = (__input_ldev_t *)p_arg;
    __input_graber_t        *pGraber;
    __input_event_packet_t  *pPacket;
    uint32_t                cpu_sr;
    int32_t                 i, j;

    pPacket = &(LMouseDevMan.packet);
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
            __wrn("logical mouse deivce event buffer invalid");
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
* Description: logical mouse device event handler.
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
static int32_t LMouseDevEvent(__input_dev_t *dev, uint32_t type, uint32_t code, int32_t value)
{
    __input_ldev_t  *ldev;
    uint32_t        cpu_sr;
    int32_t         i, j;

    if (dev == NULL)
    {
        __wrn("invalid physical device handle for LMouseDevEvent");
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
     * save physical mouse device event buffer to
     * logical mouse device circle event buffer.
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
     * copy physical mouse device events to
     * logical mouse device circle event buffer.
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
     * clear physical device event buffer.
     */
    dev->ev_pos     = 0;
    awos_arch_unlock_irq(cpu_sr);

    /* wakeup logical mouse device event handle thread */
    esKRNL_SemPost(ldev->ev_sem);

    return 0;
}

static const __input_lid_t LMouseDevIds[] =
{
    {
        .flags  = INPUT_DEVICE_ID_MATCH_EVBIT  |
        INPUT_DEVICE_ID_MATCH_KEYBIT |
        INPUT_DEVICE_ID_MATCH_RELBIT,
        .evbit  = { BIT_MASK(EV_KEY) | BIT_MASK(EV_REL) },
        .keybit = { [BIT_WORD(BTN_LEFT)] = BIT_MASK(BTN_LEFT) },
        .relbit = { BIT_MASK(REL_X) | BIT_MASK(REL_Y) },
    },  /* A mouse like device, at least one button,
           two relative axes */
    {
        .flags  = INPUT_DEVICE_ID_MATCH_EVBIT |
        INPUT_DEVICE_ID_MATCH_RELBIT,
        .evbit  = { BIT_MASK(EV_KEY) | BIT_MASK(EV_REL) },
        .relbit = { BIT_MASK(REL_WHEEL) },
    },  /* A separate scrollwheel */
    {
        .flags  = INPUT_DEVICE_ID_MATCH_EVBIT |
        INPUT_DEVICE_ID_MATCH_KEYBIT |
        INPUT_DEVICE_ID_MATCH_ABSBIT,
        .evbit  = { BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) },
        .keybit = { [BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH) },
        .absbit = { BIT_MASK(ABS_X) | BIT_MASK(ABS_Y) },
    },  /* A tablet like device, at least touch detection,
           two absolute axes */
    {
        .flags  = INPUT_DEVICE_ID_MATCH_EVBIT |
        INPUT_DEVICE_ID_MATCH_KEYBIT |
        INPUT_DEVICE_ID_MATCH_ABSBIT,
        .evbit  = { BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) },
        .keybit = {
            [BIT_WORD(BTN_TOOL_FINGER)] =
            BIT_MASK(BTN_TOOL_FINGER)
        },
        .absbit = {
            BIT_MASK(ABS_X) | BIT_MASK(ABS_Y) |
            BIT_MASK(ABS_PRESSURE) |
            BIT_MASK(ABS_TOOL_WIDTH)
        },
    },  /* A touchpad */
    {
        .flags  = INPUT_DEVICE_ID_MATCH_EVBIT |
        INPUT_DEVICE_ID_MATCH_KEYBIT |
        INPUT_DEVICE_ID_MATCH_ABSBIT,
        .evbit  = { BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) },
        .keybit = { [BIT_WORD(BTN_LEFT)] = BIT_MASK(BTN_LEFT) },
        .absbit = { BIT_MASK(ABS_X) | BIT_MASK(ABS_Y) },
    },  /* Mouse-like device with absolute X and Y but ordinary
           clicks, like hp ILO2 High Performance mouse */

    { },/* Terminating entry */
};

/*
*********************************************************************************************************
*                                     LOGICAL KEY DEVICE INIT
*
* Description:  allocate and initialize logical mouse device handle,
*               register logical mouse deivce to INPUT manage system,
*               setup logical dispatch thread.
*
* Arguments  :  NONE.
*
* Returns    : initialize logic mouse device result;
*               EPDK_OK     -   initalize successed.
*               EPDK_FAIL   -   initalize failed.
*********************************************************************************************************
*/
int32_t INPUT_LMouseDevInit(void)
{
    __input_ldev_t *pLMouseDev;

    /* allocate space for logical mouse device */
    pLMouseDev = (__input_ldev_t *)k_malloc(sizeof(struct __INPUT_LDEV));
    if (pLMouseDev == NULL)
    {
        __inf("k_malloc for logical devcie failed");
        return EPDK_FAIL;
    }
    memset(pLMouseDev, 0, sizeof(struct __INPUT_LDEV));

    /*
     * set logical mouse device information,
     * the information for melis general device managmant system.
     */
    pLMouseDev->classname = INPUT_CLASS_NAME;
    pLMouseDev->name      = INPUT_LMOUSE_DEV_NAME;
    pLMouseDev->ops       = &LMouseDevOps;
    pLMouseDev->parg      = NULL;

    /* set indentify physical input device id table */
    pLMouseDev->id_table  = LMouseDevIds;

    /* set logical mouse device event handler */
    pLMouseDev->event     = LMouseDevEvent;

    /* initialize event buffer information */
    pLMouseDev->ev_sem    = esKRNL_SemCreate(1);
    if (pLMouseDev->ev_sem == NULL)
    {
        __wrn("create semaphore logical mouse device failed!");
        k_free(pLMouseDev);
        return EPDK_FAIL;
    }
    pLMouseDev->ev_head   = 0;
    pLMouseDev->ev_tail   = 0;
    pLMouseDev->ev_len    = 0;
    pLMouseDev->ev_maxlen = INPUT_EV_BUFFER_MAXLEN;

    /* register logical device */
    if (esINPUT_RegLdev(pLMouseDev) != EPDK_OK)
    {
        __wrn("register logical mouse device failed");
        k_free(pLMouseDev);
        return EPDK_FAIL;
    }

    /* create logical mouse device graber thread */
    pLMouseDev->tid = awos_task_create("mose_input", LMouseDevGraberThread, pLMouseDev, 0x800, RT_TIMER_THREAD_PRIO - 5, 10);

    /* attach pLMouseDev to LMouseDevMan */
    LMouseDevMan.LMouseDev = pLMouseDev;

    /* allocate sem for LMouseDevMan lock */
    LMouseDevMan.lock = esKRNL_SemCreate(1);
    if (LMouseDevMan.lock == NULL)
    {
        __wrn("create semaphore logical mouse device failed!");
        k_free(pLMouseDev);
        return EPDK_FAIL;
    }

    __inf("logical mouse device initialize succeeded!");
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                     LOGICAL KEY DEVICE EXIT
*
* Description:  request to delete logical device dispatch thread,
*               unregister logical mouse deivce from INPUT manage system,
*               k_free logical mouse device handle.
*
* Arguments  :  NONE.
*
* Returns    :  NONE
*********************************************************************************************************
*/
void INPUT_LMouseDevExit(void)
{
    awos_task_delete(LMouseDevMan.LMouseDev->tid);
    esINPUT_UnregLdev(LMouseDevMan.LMouseDev);
    k_free(LMouseDevMan.LMouseDev);
}
