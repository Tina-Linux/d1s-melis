/*
*********************************************************************************************************
*                                                    eMOD
*                                         the melis Operation System
*                                               INPUT sub-system
*                                          input system event module
*
*                                    (c) Copyright 2010-2012, sunny China
*                                              All Rights Reserved
*
* File   : input_event.c
* Version: V1.0
* By     : Sunny
* Date   : 2010-7-10
* Note   : input system event report and dispatch.
*********************************************************************************************************
*/
#include "input_i.h"
#include <port.h>
#include <kapi.h>
#include <log.h>

extern __input_sys_man_t  input_sys;

#define INPUT_EVENT_IGNORE            (0<<0)
#define INPUT_EVENT_TO_LOGIC          (1<<0)
#define INPUT_EVENT_TO_PHYSIC         (1<<1)
#define INPUT_EVENT_TO_ALL            (INPUT_EVENT_TO_LOGIC | INPUT_EVENT_TO_PHYSIC)


int32_t IsEventSupported(uint32_t code, unsigned long *bm, uint32_t max)
{
    __wrn("code=0x%x", code);

    /*code = (code & 0xffff);


    {
        __s32 data;
        data = ((code >> 16) & 0xffff);
        if (data > 50)
        {
            return 0;
        }
    }*/

    return code <= max && test_bit(code, bm);
}


void INPUT_RepeatKey(void *parg)
{
    __input_dev_t   *dev = (__input_dev_t *)parg;
    uint32_t        cpu_sr;

    __inf("Repeat Key Send : code = %d", dev->repeat_key);
    if (test_bit(dev->repeat_key, dev->key) && IsEventSupported(dev->repeat_key, dev->keybit, KEY_MAX))
    {
        int32_t     repeat_valid_cnt;
        int32_t     current_cnt;

        current_cnt         = (dev->rep[REP_DELAY] & 0xff00) >> 8;
        repeat_valid_cnt    = (dev->rep[REP_DELAY] & 0x00ff) >> 0;

        if (current_cnt >= repeat_valid_cnt)
        {
            /* send repeated key, disable all interrupt */
            cpu_sr = awos_arch_lock_irq();
            esINPUT_SendEvent(dev, EV_KEY, dev->repeat_key, 2);
            esINPUT_SendEvent(dev, EV_SYN, 0, 0);
            awos_arch_unlock_irq(cpu_sr);
        }

        current_cnt++;
        if (current_cnt > 255)
        {
            current_cnt = 255;
        }

        dev->rep[REP_DELAY] &= 0x00ff;
        dev->rep[REP_DELAY] |= (current_cnt << 8);
    }
}


static void INPUT_StartAutoRepeat(__input_dev_t *dev, int32_t code)
{
    if (test_bit(EV_REP, dev->evbit) && dev->rep[REP_PERIOD]/*&& dev->rep[REP_DELAY]*/)
    {
        dev->repeat_key = code;
        if (esKRNL_TmrStart(dev->timer) !=  EPDK_TRUE)
        {
            __wrn("INPUT timer start fail!");
        }

        dev->rep[REP_DELAY] &= 0x00ff;
    }
}


static void INPUT_StopAutoRepeat(__input_dev_t *dev)
{
    if (test_bit(EV_REP, dev->evbit) && dev->rep[REP_PERIOD]/*&& dev->rep[REP_DELAY]*/)
    {
        if (esKRNL_TmrStop(dev->timer, OS_TMR_OPT_NONE, NULL) != EPDK_TRUE)
        {
            __wrn("INPUT timer stop fail,error code = %x", esKRNL_TmrError(dev->timer));
        }
        /* invalid repeat key */
        dev->repeat_key = 0;
        dev->rep[REP_DELAY] &= 0x00ff;
    }
}


static int INPUT_DefuzzAbsEvent(int value, int32_t old_val, int32_t fuzz)
{
    if (fuzz)
    {
        if (value > old_val - fuzz / 2 && value < old_val + fuzz / 2)
        {
            return old_val;
        }

        if (value > old_val - fuzz && value < old_val + fuzz)
        {
            return (old_val * 3 + value) / 4;
        }

        if (value > old_val - fuzz * 2 && value < old_val + fuzz * 2)
        {
            return (old_val + value) / 2;
        }
    }

    return value;
}

/*
*********************************************************************************************************
*                                    SEND EVENT TO INPUT SYSTEM
*
* Description: input device input event.
*
* Arguments  : dev      handle of physic input device;
*              type     type of event;
               code     code of event;
               value    value of event;
*
* Returns    : result of input event;
*               EPDK_OK     -   input event successed,
*               EPDK_FAIL   -   input event failed;
*********************************************************************************************************
*/
int32_t esINPUT_SendEvent(__input_dev_t *dev, uint32_t type, uint32_t code, int32_t value)
{
    int32_t     disposition = INPUT_EVENT_IGNORE;

    __inf("Send Event: type = %d, code = %d, value = %d", type, code, value);
    if (dev == NULL)
    {
        __wrn("invalid physical device handle for report event");
        return EPDK_FAIL;
    }

    if (!IsEventSupported(type, dev->evbit, EV_MAX))
    {
        __wrn("report unsupport event by physic device %s", dev->name);
        return EPDK_FAIL;
    }

    switch (type)
    {
        case EV_SYN:
        {
            disposition = INPUT_EVENT_TO_LOGIC;
            break;
        }

        case EV_KEY:
        {
            if (IsEventSupported(code, dev->keybit, KEY_MAX) && !!test_bit(code, dev->key) != value)
            {
                if (value != 2)
                {
                    __change_bit(code, dev->key);
                    if (value) /*key press down then start repeat timer ,comment by swb*/
                    {
                        /* key down action  */
                        INPUT_StartAutoRepeat(dev, code);
                    }
                    else /*key release then stop repeat timer ,comment by swb*/
                    {
                        /* key up action    */
                        INPUT_StopAutoRepeat(dev);
                    }
                }
                disposition = INPUT_EVENT_TO_LOGIC;
            }
            break;
        }

        case EV_ABS:
        {
            if (IsEventSupported(code, dev->absbit, ABS_MAX))
            {
                value = INPUT_DefuzzAbsEvent(value, dev->abs[code], dev->absfuzz[code]);
                if (dev->abs[code] != value)
                {
                    dev->abs[code] = value;
                    disposition = INPUT_EVENT_TO_LOGIC;
                }
            }
            break;
        }

        case EV_REL:
        {
            if (IsEventSupported(code, dev->relbit, REL_MAX) && value)
            {
                disposition = INPUT_EVENT_TO_LOGIC;
            }
            break;
        }

        case EV_LED:
        {
            if (IsEventSupported(code, dev->ledbit, LED_MAX) && !!test_bit(code, dev->led) != value)
            {
                __change_bit(code, dev->led);
                disposition = INPUT_EVENT_TO_ALL;
            }
            break;
        }

        case EV_SND:
        {
            if (IsEventSupported(code, dev->sndbit, SND_MAX))
            {
                if (!!test_bit(code, dev->snd) != !!value)
                {
                    __change_bit(code, dev->snd);
                }
                disposition = INPUT_EVENT_TO_ALL;
            }
            break;
        }

        case EV_REP:
        {
            if (code <= REP_MAX && value >= 0 && dev->rep[code] != value)
            {
                dev->rep[code] = value;
                disposition = INPUT_EVENT_TO_ALL;
            }
            break;
        }

        case EV_FF:
        {
            if (value >= 0)
            {
                disposition = INPUT_EVENT_TO_ALL;
            }
            break;
        }
    }

    /* pass event to physical device */
    if ((disposition & INPUT_EVENT_TO_PHYSIC) && dev->event)
    {
        dev->event(dev, type, code, value);
    }

    /* pass event to logical device */
    if (disposition & INPUT_EVENT_TO_LOGIC)
    {
        ((__input_ldev_t *)(dev->ldevp))->event(dev, type, code, value);
    }
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                     LOGIC INPUT DEVICE FEED-BACK
*
* Description: logic input device feed-back.
*
* Arguments  : graber   handle of graber.
*              packet   event packet.
*
* Returns    : process feed back result:
*              EPDK_OK     -   process feedback successed.
*              EPDK_FAIL   -   process feedback failed.
*********************************************************************************************************
*/
int32_t esINPUT_LdevFeedback(__hdle graber, __input_event_packet_t *packet)
{
    int32_t             i = 0;
    __input_ldev_t      *pLdev;
    __input_dev_t       *pPdev;

    /* check if handle is valid */
    if (packet == NULL)
    {
        __wrn("handle is invalid when feed-back!");
        return EPDK_FAIL;
    }

    /* get logical device */
    if (graber == NULL)
    {
        /* get logical device accord to logical device id */
        if (packet->ldev_id > INPUT_LOGICALDEV_MAX)
        {
            __wrn("invalid target logical device id.");
            return EPDK_FAIL;
        }
        pLdev = input_sys.input_ldevs[packet->ldev_id];
        if (pLdev == NULL)
        {
            __wrn("invalid target logical device handle.");
            return EPDK_FAIL;
        }
    }
    else
    {
        pLdev = ((__input_graber_t *)graber)->ldev;
    }

    /* get physical device */
    if (packet->pdev_id > INPUT_CHILDREN_MAX)
    {
        __wrn("invalid target physical device id.");
        return EPDK_FAIL;
    }
    pPdev = pLdev->children[packet->pdev_id];
    if (pPdev == NULL)
    {
        __wrn("invalid target physical device handle.");
        return EPDK_FAIL;
    }

    /* the last event must be syn event */
    if (packet->events[packet->event_cnt - 1].type != EV_SYN)
    {
        __wrn("invalid event packet to feedback.");
        return EPDK_FAIL;
    }

    /* check physical device support event feedback or not */
    if (pPdev->event == NULL)
    {
        __wrn("physical device not support event feedback.");
        return EPDK_FAIL;
    }

    /* feedback events to physical device */
    i = 0;
    while (i < packet->event_cnt)
    {
        pPdev->event(pPdev, packet->events[i].type, packet->events[i].code, packet->events[i].value);
        i++;
    }

    return EPDK_OK;
}
