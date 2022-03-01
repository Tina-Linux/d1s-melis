/*
*********************************************************************************************************
*                                                    eMOD
*                                         the melis Operation System
*                                               input sub-system
*                                          keyboard logic device handler
*
*                                    (c) Copyright 2010-2012, sunny China
*                                              All Rights Reserved
*
* File   : keyboarddev.c
* Version: V1.0
* By     : Sunny
* Date   : 2010-7-11
* Note   : this module is for general key device handler,
*          all key class physic devices use this module to dispatch and package message.
*********************************************************************************************************
*/
#include "keyboarddev_i.h"
#include <string.h>
#include <kapi.h>
#include <log.h>
#include <rtthread.h>

#define SUNXI_KEYBOARD_NAME		"sunxi-keyboard"

static void sunxi_keyboard_thread(void *p_arg);

static __lkey_dev_man_t  LKeyDevMan;

static __hdle LKeyDevOpen(void *open_arg, uint32_t mode)
{
    return NULL;
}

static int32_t LKeyDevClose(__hdle hKey)
{
    return 0;
}

static uint32_t LKeyDevRead(void *pdata, uint32_t size, uint32_t n, __hdle hKey)
{
    return 0;
}

static uint32_t LKeyDevWrite(const void *pdata, uint32_t size, uint32_t n, __hdle hKey)
{
    return 0;
}

static int32_t LKeyDevIoctrl(__hdle hKey, uint32_t cmd, long aux, void *pbuffer)
{
    return 0;
}

const __dev_devop_t LKeyDevOps =
{
    LKeyDevOpen,
    LKeyDevClose,
    LKeyDevRead,
    LKeyDevWrite,
    LKeyDevIoctrl
};

int32_t LKeyDevMan_Lock(void)
{
    esKRNL_SemPend(LKeyDevMan.lock, 0, NULL);

    return EPDK_OK;
}

int32_t LKeyDevMan_Unlock(void)
{
    esKRNL_SemPost(LKeyDevMan.lock);
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                     LOGICAL KEY DEVICE EVENT DISPATCH THEARD
*
* Description: packet logical key devcie events,
*              dispatch packet event information to all grabers of logical key device.
*
* Arguments  : p_arg   logical key device handle.
*
* Returns    : NONE.
*********************************************************************************************************
*/
static void LKeyDevGraberThread(void *p_arg)
{
    __input_ldev_t          *ldev = (__input_ldev_t *)p_arg;
    __input_graber_t        *pGraber;
    __input_event_packet_t  *pPacket;
    uint32_t                cpu_sr;
    int32_t                 i, j;

    pPacket = &(LKeyDevMan.packet);
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
         * just skip it, By sunny at 2010-9-25 20:07:59.
         */
        j = (j + 1) % ldev->ev_maxlen;
        ldev->ev_len--;

        /* event encode format : |type|code|value|,
         * so the event length must be aligned by 3 and type is uint32_t.
         */
        if ((ldev->ev_len % 3) || (ldev->ev_len == 0))
        {
            __wrn("logical key deivce event buffer invalid");
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

        /* submit packeted events to all grabers */
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
* Description: logical key device event handler.
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
static int32_t LKeyDevEvent(__input_dev_t *dev, uint32_t type, uint32_t code, int32_t value)
{
    __input_ldev_t  *ldev;
    uint32_t        cpu_sr;
    int32_t         i, j;
#if !defined(CONFIG_SOC_SUN20IW1)
    if (dev == NULL)
    {
        __wrn("invalid physical device handle for LKeyDevEvent!");
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
     * save physical key device event buffer to
     * logical key device circle event buffer.
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
     * copy physical key device events to
     * logical key device circle event buffer.
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

    /* wakeup logical key device event handle thread */
    esKRNL_SemPost(ldev->ev_sem);
#else
    __inf("this platform only support the following keyboard_thread!");
#endif
    return 0;
}

static const __input_lid_t LKeyDevIds[] =
{
    {
        .flags = INPUT_DEVICE_ID_MATCH_EVBIT,
        .evbit = { BIT_MASK(EV_KEY) },
    },

    {
        .flags = INPUT_DEVICE_ID_MATCH_EVBIT,
        .evbit = { BIT_MASK(EV_SND) },
    },

    { },    /* Terminating entry */
};

/*
*********************************************************************************************************
*                                     LOGICAL KEY DEVICE INIT
*
* Description:  allocate and initialize logical key device handle,
*               register logical key deivce to INPUT manage system,
*               setup logical dispatch thread.
*
* Arguments  :  NONE.
*
* Returns    : initialize logic key device result;
*               EPDK_OK     -   initalize successed.
*               EPDK_FAIL   -   initalize failed.
*********************************************************************************************************
*/
int32_t INPUT_LKeyDevInit(void)
{
    __input_ldev_t *pLKeyDev;

    /* allocate space for logical key device */
    pLKeyDev = (__input_ldev_t *)k_malloc(sizeof(struct __INPUT_LDEV));
    if (pLKeyDev == NULL)
    {
        __err("k_malloc for logical devcie failed");
        return EPDK_FAIL;
    }
    memset(pLKeyDev, 0, sizeof(struct __INPUT_LDEV));

    /*
     * set logical key device information,
     * the information for melis general device managmant system.
     */
    pLKeyDev->classname = INPUT_CLASS_NAME;
    pLKeyDev->name      = INPUT_LKEYBOARD_DEV_NAME;
    pLKeyDev->ops       = &LKeyDevOps;
    pLKeyDev->parg      = NULL;

    /* set indentify physical input device id table */
    pLKeyDev->id_table  = LKeyDevIds;

    /* set logical keyboard device event handler */
    pLKeyDev->event     = LKeyDevEvent;

    /* initialize event buffer information */
    pLKeyDev->ev_sem    = esKRNL_SemCreate(1);
    if (pLKeyDev->ev_sem == NULL)
    {
        __err("create semaphore logical keyboard device failed!");
        k_free(pLKeyDev);
        return EPDK_FAIL;
    }
    pLKeyDev->ev_head   = 0;
    pLKeyDev->ev_tail   = 0;
    pLKeyDev->ev_len    = 0;
    pLKeyDev->ev_maxlen = INPUT_EV_BUFFER_MAXLEN;

    /* register logical device */
    if (esINPUT_RegLdev(pLKeyDev) != EPDK_OK)
    {
        __wrn("register logical keyboard device failed");
        k_free(pLKeyDev);
        return EPDK_FAIL;
    }

#if 1//!defined(CONFIG_SOC_SUN20IW1)
    /* create logical keyboard device graber thread */
    pLKeyDev->tid   = awos_task_create("kb_input", LKeyDevGraberThread, pLKeyDev, 0x2000, RT_TIMER_THREAD_PRIO - 5, 10);
#else
/*20201205 comment this code temporarily, because input & gpadc have not been work.*/
    //pLKeyDev->tid   = awos_task_create("kb_input", sunxi_keyboard_thread, pLKeyDev, 0x1000, RT_TIMER_THREAD_PRIO - 5, 10);
#endif
    if (pLKeyDev-> tid == NULL)
    {
        __err("create thread fail");
        k_free(pLKeyDev);
        return EPDK_FAIL;
    }

    /* attach pLKeyDev to LKeyDevMan */
    LKeyDevMan.LKeyDev = pLKeyDev;

    /* allocate sem for LKeyDevMan lock */
    LKeyDevMan.lock = esKRNL_SemCreate(1);
    if (LKeyDevMan.lock == NULL)
    {
        __err("create semaphore logical keyboard device failed!");
        k_free(pLKeyDev);
        return EPDK_FAIL;
    }
    __inf("logical keyboard device initialize succeeded!");
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     LOGICAL KEY DEVICE EXIT
*
* Description:  request to delete logical dispatch thread,
*               unregister logical key deivce from INPUT manage system,
*               k_free logical key device handle.
*
* Arguments  :  NONE.
*
* Returns    :  NONE
*********************************************************************************************************
*/
void INPUT_LKeyDevExit(void)
{
    awos_task_delete(LKeyDevMan.LKeyDev->tid);
    esINPUT_UnregLdev(LKeyDevMan.LKeyDev);
    k_free(LKeyDevMan.LKeyDev);
}


struct sunxi_input_event
{
    unsigned int type;
    unsigned int code;
    unsigned int value;
};

extern int32_t INPUT_GetLdevPos(const char *name, __input_ldev_t **pLdev);
extern int sunxi_input_open(const char *name);
extern int sunxi_input_read(int fd, void *buffer, unsigned int size);
static void sunxi_keyboard_thread(void *p_arg)
{
	int32_t                     keyboardfd = 0;
	int32_t                     count = 0;
    struct sunxi_input_event    keyevent = {0};
    __input_event_packet_t      pPacket = {0};
    __input_ldev_t              *ldev = (__input_ldev_t *)p_arg;
    __input_graber_t            *pGraber = NULL;

	do{
		rt_thread_delay(20);
/*20201205 comment this code temporarily, because input & gpadc have not been work.*/
		//keyboardfd = sunxi_input_open(SUNXI_KEYBOARD_NAME);
	}while(-1 == keyboardfd);

	while(1)
	{
		rt_memset((void*)&keyevent, 0, sizeof(struct sunxi_input_event));

        pPacket.ldev_id     = ldev->seq;
        pPacket.pdev_id     = 0xA5A5;

		/*there is a semphore in sunxi_input_read func, so this thread do not need pend process*/
        do{
/*20201205 comment this code temporarily, because input & gpadc have not been work.*/
            //count = sunxi_input_read(keyboardfd, (void*)&keyevent, sizeof(struct sunxi_input_event));
            if(count != sizeof(struct sunxi_input_event))
            {
                __inf("fetch input event empty!!");
                break;
            }

            pPacket.events[pPacket.event_cnt].type  = keyevent.type;
            pPacket.events[pPacket.event_cnt].code  = keyevent.code;
            pPacket.events[pPacket.event_cnt].value = keyevent.value;
            pPacket.event_cnt++;

            if(EV_SYN == pPacket.events[pPacket.event_cnt].type)
            {
                break;
            }
        }while(pPacket.event_cnt < INPUT_MAX_EVENT_NR);/*if read correctly, continue fetch input_event*/

        if (pPacket.event_cnt < 2)
        {
            /*for examples, only a EV_SYN event, no need to send the packet to msg_emit*/
            __wrn("filter invalid event packet!");
            continue;
        }

        /* submit packeted events to all grabers */
        pGraber = ldev->grabers;
        while (pGraber)
        {
            pPacket.pGrabArg = pGraber->pGrabArg;
            esKRNL_CallBack(pGraber->callback, (void *)&pPacket);
            pGraber = pGraber->next;
        }
	}
}


int32_t console_buffer[64];
int32_t valid_key_number = 0;
int32_t console_LKeyDevEvent(__input_dev_t *dev, uint32_t type, uint32_t code, int32_t value)
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
    if(LKeyDevMan.LKeyDev == NULL)
    {
        __log("input sys keydev not init!");
        valid_key_number = 0;
        return ;
    }
    ldev = LKeyDevMan.LKeyDev;

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
