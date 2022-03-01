/*
*********************************************************************************************************
*                                                    eMOD
*                                         the melis Operation System
*                                               input sub-system
*                                          input system core module
*
*                                    (c) Copyright 2010-2012, sunny China
*                                              All Rights Reserved
*
* File   : input.c
* Version: V1.0
* By     : Sunny
* Date   : 2010-7-10
* Note   : input system core module.
*********************************************************************************************************
*/
#include "input_i.h"
#include <string.h>
#include <kapi.h>
#include <log.h>

/* define input system manager handle */
__input_sys_man_t  input_sys;

/*
*********************************************************************************************************
*                                     INPUT SYSTEM INIT
*
* Description: input system init.
*
* Arguments  : none
*
* Returns    : module init result;
*               EPDK_OK     -   module init successed,
*               EPDK_FAIL   -   module init failed;
*********************************************************************************************************
*/
int32_t INPUT_CoreInit(void)
{
    int32_t i = 0;

    while (i < INPUT_LOGICALDEV_MAX)
    {
        input_sys.input_ldevs[i++] = NULL;
    }

    input_sys.lock = esKRNL_SemCreate(1);
    if (input_sys.lock == NULL)
    {
        __err("create semaphore for input system failed!");
        return EPDK_FAIL;
    }

    /* create event dispath server thread */

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     INPUT SYSTEM EXIT
*
* Description: input system exit.
*
* Arguments  : none
*
* Returns    : module exit result;
*               EPDK_OK     -   module exit successed,
*               EPDK_FAIL   -   module exit failed;
*********************************************************************************************************
*/
int32_t INPUT_CoreExit(void)
{
    int32_t     i = 0;

    while (i < INPUT_LOGICALDEV_MAX)
    {
        if (input_sys.input_ldevs[i++])
        {
            __wrn("some input device is exsit when exit input system!");
            return EPDK_FAIL;
        }
    }

    if (input_sys.lock)
    {
        esKRNL_SemDel(input_sys.lock, 0, NULL);
        input_sys.lock = 0;
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     INPUT SYSTEM LOCK
*
* Description: input system lock.
*
* Arguments  : none.
*
* Returns    : lock input system result;
*               EPDK_OK     -   lock input system successed.
*               EPDK_FAIL   -   lock input system failed.
*********************************************************************************************************
*/
static int32_t INPUT_Lock(void)
{
    if (input_sys.lock == NULL)
    {
        __wrn("input system lock is invalid!");
        return EPDK_FAIL;
    }

    esKRNL_SemPend(input_sys.lock, 0, NULL);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     INPUT SYSTEM UNLOCK
*
* Description: input system unlock.
*
* Arguments  : none.
*
* Returns    : none;
*********************************************************************************************************
*/
static void INPUT_Unlock(void)
{
    esKRNL_SemPost(input_sys.lock);
    return;
}


/*
*********************************************************************************************************
*                                   GET LOGICAL DEVICE POSITION
*
* Description: get logical device position in input logical array.
*
* Arguments  : name     the target logical device name.
*              pLdev    to store target logical device address.
* Returns    : get logical device position;
*               -1          -   get logical device position failed;
*               EPDK_OK     -   index number of logical device position in input logical devices array.
*********************************************************************************************************
*/
int32_t INPUT_GetLdevPos(const char *name, __input_ldev_t **pLdev)
{
    int32_t     i = 0;

    /* find logical device specially by name within input logical devices array */
    while (i < INPUT_LOGICALDEV_MAX)
    {
        if (input_sys.input_ldevs[i])
        {
            if (!strcmp(name, input_sys.input_ldevs[i]->name))
            {
                if (pLdev)
                {
                    *pLdev = input_sys.input_ldevs[i];
                }
                break;
            }
        }
        i++;
    }

    if (i == INPUT_LOGICALDEV_MAX)
    {
        /* logical device not found */
        i = -1;
    }

    return i;
}

/*
*********************************************************************************************************
*                                     REGISTER LOGIC INPUT DEVICE
*
* Description: register logic input device to input system.
*
* Arguments  : ldev     logic input device handle.
*
* Returns    : register logic input device result;
*               EPDK_OK     -   register logical device successed.
*               EPDK_FAIL   -   register logical device failed.
*********************************************************************************************************
*/
int32_t esINPUT_RegLdev(__input_ldev_t *ldev)
{
    int32_t     i;

    /* check if logic device handle is valid */
    if (ldev == NULL)
    {
        __wrn("device handle is null when register logical input device!");
        return EPDK_FAIL;
    }

    /* lock input system                      */
    if (INPUT_Lock() != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    /* check if same device has been registered already */
    if (INPUT_GetLdevPos(ldev->name, NULL) != -1)
    {
        __wrn("input device(class:%s, name:%s) registered already!", ldev->classname, ldev->name);
        INPUT_Unlock();
        return EPDK_FAIL;
    }

    /* register logical device to general device managment system */
    ldev->devnode = esDEV_DevReg(ldev->classname, ldev->name, ldev->ops, ldev->parg);
    if (ldev->devnode == NULL)
    {
        __wrn("DevReg(class:%s, name:%s) to device system failed!", ldev->classname, ldev->name);
        INPUT_Unlock();
        return EPDK_FAIL;
    }

    /* record logical device to input logical devices array */
    for (i = 0; i < INPUT_LOGICALDEV_MAX; i++)
    {
        if (input_sys.input_ldevs[i] == NULL)
        {
            input_sys.input_ldevs[i] = ldev;
            ldev->seq = i;
            break;
        }
    }
    if (i == INPUT_LOGICALDEV_MAX)
    {
        __wrn("max logical device reached when register logical dev %s!", ldev->name);
        INPUT_Unlock();
        return EPDK_FAIL;
    }

    /* initialize logical device grabers */
    ldev->grabers = NULL;

    __inf("register device (class:%s, name:%s) to input system sccessed!", ldev->classname, ldev->name);
    INPUT_Unlock();
    return EPDK_OK;
}



/*
*********************************************************************************************************
*                                     UNREGISTER LOGIC INPUT DEVICE
*
* Description: unregister logic input device from input system.
*
* Arguments  : ldev     logic input device handle.
*
* Returns    : unregister logic input device result;
*               EPDK_OK     -   unregister logical device successed,
*               EPDK_FAIL   -   unregister logical device failed;
*********************************************************************************************************
*/
int32_t esINPUT_UnregLdev(__input_ldev_t *ldev)
{
    /* check if logic device handle is valid    */
    if (ldev == NULL)
    {
        __wrn("device handle is null when unregister logical input device!");
        return EPDK_FAIL;
    }

    /* lock input system                          */
    if (INPUT_Lock() != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    /* unregister device from general device managemant system */
    if (esDEV_DevUnreg(ldev->devnode) != EPDK_OK)
    {
        __wrn("unregister device %s from general device managemant system failed",
              ldev->name);
        INPUT_Unlock();
        return EPDK_FAIL;
    }
    ldev->devnode = NULL;

    /* delete logical device from input logical devices array */
    input_sys.input_ldevs[ldev->seq] = NULL;

    __inf("un-reg logic device(class:%s, name:%s) from input system sccessed!",
          ldev->classname, ldev->name);
    INPUT_Unlock();
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                     GRAB LOGIC INPUT DEVICE
*
* Description: grab logic input device.
*
* Arguments  : ldev     logic input device name.
*              callback call-back function.
*              pArg     private argument for callback.
*              aux      reserved para.
*
* Returns    : logic device operation handle;
*               NULL        -   grab logic input device failed,
*               !NULL       -   grab logic input device succssed;
*********************************************************************************************************
*/
__hdle esINPUT_LdevGrab(char *ldev, __pCBK_t callback, void *pArg, int32_t aux)
{
    int32_t             index;
    __input_ldev_t      *tmpLdev;
    __input_graber_t    *tmpGrab;

    /* check if handle is valid */
    if (ldev == NULL || callback == NULL)
    {
        __wrn("handle is invalid when grab input device!");
        return NULL;
    }

    /*  lock input system */
    if (INPUT_Lock() != EPDK_OK)
    {
        return NULL;
    }

    /* check if logical device is exist or not */
    index = INPUT_GetLdevPos(ldev, &tmpLdev);
    if (index == -1)
    {
        __wrn("logical device %s not exist!", ldev);
        INPUT_Unlock();
        return NULL;
    }

    /* allocate space for graber handle */
    tmpGrab = (__input_graber_t *)k_malloc(sizeof(__input_graber_t));
    if (tmpGrab == NULL)
    {
        __wrn("allocate space for graber handle failed!");
        INPUT_Unlock();
        return NULL;
    }

    /* initialize allocated graber */
    tmpGrab->callback   = esKRNL_GetCallBack(callback);
    tmpGrab->pGrabArg   = pArg;
    tmpGrab->ldev       = tmpLdev;
    tmpGrab->next       = NULL;

    /* insert new graber to logical device grabers list head */
    tmpGrab->next       = tmpLdev->grabers;
    tmpLdev->grabers    = tmpGrab;

    __inf("get input logical device %s graber successed!", ldev);
    INPUT_Unlock();
    return (__hdle)tmpGrab;
}


/*
*********************************************************************************************************
*                                     RELEASE LOGIC INPUT DEVICE
*
* Description: release logic input device.
*
* Arguments  : graber   handle of graber.
*
* Returns    : release graber handle result;
*               EPDK_OK     -   release graber successed,
*               EPDK_FAIL   -   release graber failed;
*********************************************************************************************************
*/
int32_t esINPUT_LdevRelease(__hdle graber)
{
    __input_ldev_t      *tmpLdev;
    __input_graber_t    *relGrab;
    __input_graber_t    *prevGrab;
    __bool              FindGrab;

    /* check if the handle is valid */
    if (graber == NULL)
    {
        __wrn("handle is invalid when release graber!");
        return EPDK_FAIL;
    }

    /*  lock input system             */
    if (INPUT_Lock() != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    /* delete graber from logical device */
    relGrab     = (__input_graber_t *)graber;
    tmpLdev     = relGrab->ldev;
    prevGrab    = tmpLdev->grabers;
    FindGrab    = 0;
    if (relGrab == prevGrab)
    {
        tmpLdev->grabers = relGrab->next;
        relGrab->next    = NULL;
        FindGrab         = 1;
    }
    else
    {
        /* seach release graber's prev graber */
        while (prevGrab)
        {
            if (prevGrab->next == relGrab)
            {
                prevGrab->next  = relGrab->next;
                relGrab->next   = NULL;
                FindGrab        = 1;
                break;
            }
            prevGrab = prevGrab->next;
        }
    }
    if (FindGrab == 0)
    {
        __wrn("release invalid graber handle");
        return EPDK_FAIL;
    }

    /* k_free graber allcoted space */
    k_free(relGrab);
    __inf("release graber successed!");
    INPUT_Unlock();
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                               GET LOGICAL DEVICE ID
*
* Description: get the id of logical device.
*
* Arguments  :
*              graber   handle of graber.
*
* Returns    : id of logical device.
*              -1 - get logical device id failed.
*********************************************************************************************************
*/
int32_t esINPUT_GetLdevID(__hdle graber)
{
    __input_graber_t  *tmpGrab;

    if (graber == NULL)
    {
        __wrn("invalid parameters for get logical device id");
        return -1;
    }

    tmpGrab = (__input_graber_t *)graber;

    return (tmpGrab->ldev->seq);
}

/*
*********************************************************************************************************
*                                       SET DEVICE REP PERIOD
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int32_t INPUT_SetRepPeriod(int32_t LDevID, uint32_t period)
{
    uint32_t            i;
    __input_ldev_t      *tmpLdev;

    /* get logical device handle by id */
    tmpLdev = input_sys.input_ldevs[LDevID];
    if (tmpLdev == NULL)
    {
        __wrn("logical device id [%d] handler is NULL", LDevID);
        return EPDK_FAIL;
    }

    /* only key class logical device support setting req period */
    if (strcmp(INPUT_LKEYBOARD_DEV_NAME, tmpLdev->name) != 0)
    {
        __wrn("only key class device support setting req period");
        return EPDK_FAIL;
    }

    /* adjust phyical devices rep period of logical device */
    for (i = 0; i < tmpLdev->children_cnt; i++)
    {
        __input_dev_t  *tmpDev;
        tmpDev = tmpLdev->children[i];
        if (tmpDev->rep[REP_PERIOD])
        {
            /* in order to set device rep period,
             * we should delete current timer and recreate a new timer.
             */
            tmpDev->rep[REP_DELAY]  = (period >> 16) & 0xff;
            tmpDev->rep[REP_PERIOD] = (period & 0xffff);


            /* delete device current timer */
            if (esKRNL_TmrDel(tmpDev->timer) != EPDK_TRUE)
            {
                __wrn("delete device timer failed");
                return EPDK_FAIL;
            }

            /* recreate device timer */
            tmpDev->timer = esKRNL_TmrCreate(tmpDev->rep[REP_PERIOD], OS_TMR_OPT_PRIO_HIGH | OS_TMR_OPT_PERIODIC, INPUT_RepeatKey, tmpDev);
            if (tmpDev->timer == NULL)
            {
                __wrn("create timer fail!");
                return EPDK_FAIL;
            }
        }
    }

    /* set logical device rep period succeeded */
    __inf("set repeat period to [%d] succeeded", period);
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                          CONTROL LOGICAL DEVICE
*
* Description: control logical device.
*
* Arguments  :
*               LdevId   -id of logical device.
*               cmd      -control command.
*               aux      -reserved argument.
*               pBuffer  -argument of command.
*
* Returns    :  EPDK_OK     -   control logical device successed.
*               EPDK_FAIL   -   control logical device failed.
*********************************************************************************************************
*/
int32_t esINPUT_LdevCtl(int32_t LdevId, int32_t cmd, int32_t aux, void *pBuffer)
{
    if ((LdevId < 0) || (LdevId > INPUT_LOGICALDEV_MAX))
    {
        __wrn("invalid logical device id for IOCTL");
        return EPDK_FAIL;
    }

    switch (cmd)
    {
        case INPUT_SET_REP_PERIOD:
        {
            /* set device rep period */
            return INPUT_SetRepPeriod(LdevId, aux);
        }

        default:
        {
            __wrn("invalid command for logical device IOCTL");
            return EPDK_FAIL;
        }
    }

    /* unreached */
    //return EPDK_OK;
}

/*
*********************************************************************************************************
*                          MATCH physical DEVICE AND LOGIC DEVICE
*
* Description: check physical device and logic device match or not.
*
* Arguments  : ldev     logic input device name.
*              pdev     physical input device handle.
*
* Returns    : unregister physical input device result;
*              EPDK_TRUE   -   physical device and logic device match,
*              EPDK_FALSE  -   physical device and logic device not match.
*********************************************************************************************************
*/
#define MATCH_BIT(bit, max) \
    for (i = 0; i < BITS_TO_LONGS(max); i++) \
        if ((id->bit[i] & pdev->bit[i]) != id->bit[i]) \
            break; \
    if (i != BITS_TO_LONGS(max)) \
        continue;

static __bool INPUT_MatchDevice(__input_ldev_t *ldev, __input_dev_t *pdev)
{
    int32_t         i;
    __input_lid_t   *id = (__input_lid_t *)(ldev->id_table);

    /* 如果逻辑输入设备的id识别组没有设置，将被认为支持所有物理输入设备 */
    if (id == NULL)
    {
        return EPDK_FALSE;
    }

    /* 逻辑输入设备可以拥有多组独立的身份识别，只要其中的一组同物理输入设备匹配，
     * 即被认为被检查的物理输入设备将被该逻辑输入设备支持
     */
    for (; id->flags; id++)
    {
        /* 需要比较产品型号 */
        if (id->flags & INPUT_DEVICE_ID_MATCH_PRODUCT)
            if (id->product != pdev->id.product)
            {
                continue;
            }

        /* 需要比较产品版本号 */
        if (id->flags & INPUT_DEVICE_ID_MATCH_VERSION)
            if (id->version != pdev->id.version)
            {
                continue;
            }

        /*
         * 将物理输入设备和逻辑输入设备的所有事件类型和事件码相比较，如果物理输入设备的
         * 事件类型、事件编码集合大于等于逻辑输入设备的事件类型、事件编码集合，则认为逻
         * 辑和物理输入设备是相匹配的。
         * 否则继续匹配逻辑输入设备的下一组id。
         */
        MATCH_BIT(evbit,  EV_MAX);
        MATCH_BIT(keybit, KEY_MAX);
        MATCH_BIT(relbit, REL_MAX);
        MATCH_BIT(absbit, ABS_MAX);
        MATCH_BIT(ledbit, LED_MAX);
        MATCH_BIT(sndbit, SND_MAX);
        MATCH_BIT(ffbit,  FF_MAX);

        return EPDK_TRUE;
    }

    return EPDK_FALSE;
}

/*
*********************************************************************************************************
*                          CLEANSE PHYSICAL DEVICE BITMASKS
*
* Description: chean the bitmasks not mentioned in dev->evbit.
*
* Arguments  :
*              dev     physical input device handle.
*
* Returns    : NONE.
*********************************************************************************************************
*/
#define INPUT_CLEANSE_BITMASK(dev, type, bits)  \
    do {                                        \
        if (!test_bit(EV_##type, dev->evbit))   \
            memset(dev->bits##bit, 0,       \
                   sizeof(dev->bits##bit));        \
    } while (0)

static void INPUT_CleanseBitmasks(__input_dev_t *dev)
{
    INPUT_CLEANSE_BITMASK(dev, KEY, key);
    INPUT_CLEANSE_BITMASK(dev, REL, rel);
    INPUT_CLEANSE_BITMASK(dev, ABS, abs);
    INPUT_CLEANSE_BITMASK(dev, LED, led);
    INPUT_CLEANSE_BITMASK(dev, SND, snd);
    INPUT_CLEANSE_BITMASK(dev, FF, ff);
}

/*
*********************************************************************************************************
*                                   GET LOGICAL DEVICE NAME
*
* Description: get logical device name.
*
* Arguments  : pDev         physical input device handle.
*              pLdevName    to store logical device name address.
*
* Returns    : get logical device name;
*              EPDK_OK      -   get logical device name succeeded,
*              EPDK_FALL    -   get logical device name failed.
*********************************************************************************************************
*/
static int32_t INPUT_GetLdevName(__input_dev_t *pDev, char **pLdevName)
{
    int32_t     i;
    struct INPUTDevNameMap
    {
        uint32_t    device_class;
        const char  *device_name;
    };
    struct INPUTDevNameMap INPUTMapTable[] =
    {
        { INPUT_KEYBOARD_CLASS,  INPUT_LKEYBOARD_DEV_NAME },
        { INPUT_MOUSE_CLASS,  INPUT_LMOUSE_DEV_NAME    },
        { INPUT_TS_CLASS,  INPUT_LTS_DEV_NAME       },
    };
    for (i = 0; i < (sizeof(INPUTMapTable) / sizeof(INPUTMapTable[0])); i++)
    {
        if (INPUTMapTable[i].device_class == pDev->device_class)
        {
            *pLdevName = (char *)(INPUTMapTable[i].device_name);
            return EPDK_OK;
        }
    }
    return EPDK_FAIL;
}

/*
*********************************************************************************************************
*                          ATTACH PHYSICAL DEVICE TO LOGICAL DEVICE
*
* Description: seach matched logical device for physical device,
*              attach physical device to logical device.
*
* Arguments  :
*              dev     physical input device handle.
*
* Returns    : attach physical device;
*               EPDK_OK     -   attach physical device successed.
*               EPDK_FAIL   -   attach physical device failed.
*********************************************************************************************************
*/
static int32_t INPUT_AttachLdev(__input_dev_t *dev)
{
    int32_t         i;
    int32_t         index;
    __input_ldev_t  *tmpLdev  = NULL;
    char            *LdevName = NULL;

    /* accord to device type to get logical device name */
    if (INPUT_GetLdevName(dev, &LdevName) != EPDK_OK)
    {
        __wrn("get logical device name failed!");
        return EPDK_FAIL;
    }

    /* seach the matched logical device index number    */
    index = INPUT_GetLdevPos(LdevName, &tmpLdev);
    if (index == -1 || tmpLdev == NULL)
    {
        __wrn("get logical device position failed!");
        return EPDK_FAIL;
    }
    if (tmpLdev->children_cnt >= INPUT_CHILDREN_MAX)
    {
        __wrn("Max children reached within %s!", tmpLdev->name);
        return EPDK_FAIL;
    }

    /* check physical device and logical device matched or not */
    if (INPUT_MatchDevice(tmpLdev, dev) != EPDK_TRUE)
    {
        __wrn("can't match logic device to deal with physical device [%s]",
              dev->name);
        return EPDK_FAIL;
    }

    /* attach physical device to matched logic device children array */
    for (i = 0; i < INPUT_CHILDREN_MAX; i++)
    {
        if (tmpLdev->children[i])
        {
            if (strcmp(tmpLdev->children[i]->name, dev->name) == 0)
            {
                __wrn("same children name %s found under %s!",
                      dev->name, tmpLdev->name);
                return EPDK_FAIL;
            }
        }
    }
    for (i = 0; i < INPUT_CHILDREN_MAX; i++)
    {
        if (tmpLdev->children[i] == NULL)
        {
            tmpLdev->children[i] = dev;
            dev->ldevp           = (__hdle)tmpLdev;
            dev->seq             = i;
            tmpLdev->children_cnt++;
            break;
        }
    }
    if (i == INPUT_CHILDREN_MAX)
    {
        __wrn("can't find k_free children postion when reg input dev %s", dev->name);
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                          DEATTACH PHYSICAL DEVICE FROM LOGICAL DEVICE
*
* Description: deattach physical device from logical device.
*
* Arguments  :
*              dev     physical input device handle.
*
* Returns    : deattach physical device;
*               EPDK_OK     -   deattach physical device successed.
*               EPDK_FAIL   -   deattach physical device failed.
*********************************************************************************************************
*/
static int32_t INPUT_DeattachLdev(__input_dev_t *dev)
{
    if (dev == NULL)
    {
        __wrn("handle is invalid when deattach physical device!");
        return EPDK_FAIL;
    }

    /* delete physical device from logical device children array */
    if (dev->ldevp)
    {
        __input_ldev_t *pLdev;
        pLdev = (__input_ldev_t *)dev->ldevp;
        pLdev->children[dev->seq] = NULL;
        pLdev->children_cnt--;
        dev->ldevp = NULL;
        return EPDK_OK;
    }

    return EPDK_FAIL;
}

/*
*********************************************************************************************************
*                                     REGISTER physical INPUT DEVICE
*
* Description: register physical input device to input system.
*
* Arguments  : dev     physical input device handle.
*
* Returns    : register physical input device result;
*               EPDK_OK     -   register physical device successed,
*               EPDK_FAIL   -   register physical device failed;
*********************************************************************************************************
*/
int32_t esINPUT_RegDev(__input_dev_t *dev)
{
    /* check if parameter is valid                      */
    if (dev == NULL)
    {
        __wrn("handle is invalid when reg physical input device!");
        return EPDK_FAIL;
    }

    /* lock input system                                  */
    if (INPUT_Lock() != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    /* Every input device generates EV_SYN/SYN_REPORT events. */
    __set_bit(EV_SYN, dev->evbit);

    /* KEY_RESERVED is not supposed to be transmitted to userspace. */
    __clear_bit(KEY_RESERVED, dev->keybit);

    /* Make sure that bitmasks not mentioned in dev->evbit are clean. */
    INPUT_CleanseBitmasks(dev);

    /* attach physical device to matched logical device */
    if (INPUT_AttachLdev(dev) != EPDK_OK)
    {
        __wrn("attach physical device to logical device failed!");
        goto err;
    }

    /* request timer for repeat event if necessary */
    if (IsEventSupported(EV_REP, dev->evbit, EV_MAX) && (dev->rep[REP_DELAY] == 0) && (dev->rep[REP_PERIOD] == 0))
    {
        /* melis system timer unit is 1ms.
         * so the repeat key timer interleave is 0.5s = 1ms * 500.
         */
        dev->rep[REP_DELAY]  = 4;
        dev->rep[REP_PERIOD] = 500;
        dev->timer = esKRNL_TmrCreate(dev->rep[REP_PERIOD], OS_TMR_OPT_PRIO_HIGH | OS_TMR_OPT_PERIODIC, INPUT_RepeatKey, dev);
        if (dev->timer == NULL)
        {
            __wrn("create timer fail!");
            goto err_dev;
        }
    }

    /* register physical device to general device managemant system */
    dev->devnode = esDEV_DevReg(dev->classname, dev->name, dev->ops, dev->parg);
    if (!dev->devnode)
    {
        __wrn("devreg (class:%s, name:%s) to device system fail!", dev->classname, dev->name);
        goto err_timer;
    }

    /* initialize physical device event buffer manage information */
    dev->ev_pos = 0;

    __inf("reg physical device(class:%s, name:%s) to input system ok!", dev->classname, dev->name);
    INPUT_Unlock();
    return EPDK_OK;

err_timer:
    esKRNL_TmrDel(dev->timer);
    dev->timer = NULL;

err_dev:
    INPUT_DeattachLdev(dev);

err:
    INPUT_Unlock();
    return EPDK_FAIL;
}


/*
*********************************************************************************************************
*                                     UNREGISTER PHYSICAL INPUT DEVICE
*
* Description: unregister physical input device from input system.
*
* Arguments  : dev     physical device handle to unregister.
*
* Returns    : unregister physical input device result;
*               EPDK_OK     -   unregister physical device successed,
*               EPDK_FAIL   -   unregister physical device failed;
*********************************************************************************************************
*/
int32_t esINPUT_UnregDev(__input_dev_t *dev)
{
    /* check if parameter is valid                              */
    if (dev == NULL)
    {
        __wrn("handle is invalid when unreg physical input device!");
        return EPDK_FAIL;
    }

    /* lock input system                                          */
    if (INPUT_Lock() != EPDK_OK)
    {
        return EPDK_FAIL;
    }

    /* unregister device from general device managemant system  */
    if (esDEV_DevUnreg(dev->devnode) != EPDK_OK)
    {
        __wrn("unregister device %s from general device managemant failed", dev->name);
        INPUT_Unlock();
        return EPDK_FAIL;
    }
    dev->devnode = NULL;

    /* delete physical device timer                             */
    if (dev->timer)
    {
        esKRNL_TmrDel(dev->timer);
    }
    dev->timer = NULL;

    /* deattach physical device from matched logical device     */
    INPUT_DeattachLdev(dev);

    __inf("un-reg device(class:%s, name:%s) from INPUT!", dev->classname, dev->name);
    INPUT_Unlock();
    return EPDK_OK;
}

