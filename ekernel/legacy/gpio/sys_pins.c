/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Pin Manage Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : sys_pin.c
* By      : kevin.z
* Version : v2.0
* Date    : 2010-11-27 9:26
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-27 9:26     kevin.z     2.0     build the file;
*           2011-6-18 13:24:07  sunny       2.0     add pin int support.
*********************************************************************************************************
*/
#include "sys_pins_i.h"
#include <rtthread.h>
#include <arch.h>
#include <port.h>
#include <string.h>
#include <log.h>
#include "sunxi_drv_gpio.h"
#include <script.h>

//pin manager
static __pin_manager_t PINManger = {NULL, 0, 0};

/*
*********************************************************************************************************
*                                   CHECK PIN HANDLE
*
* Description: check whether the pin handle passed by apps exists in PINManger.pList;
*
* Arguments  : hPin         pin handle.
*
* Returns    : EPDK_TRUE    valid
*              EPDK_FALSE   invalid
*
* Note       :
*********************************************************************************************************
*/
__bool PIN_CheckPinHdle(__hdle hPin)
{
    uint32_t            cpu_sr;
    __pin_item_node_t   *tmpNode;

    cpu_sr  = awos_arch_lock_irq();
    tmpNode = PINManger.pList;
    while (tmpNode)
    {
        if (tmpNode == (__pin_item_node_t *)hPin)
        {
            awos_arch_unlock_irq(cpu_sr);
            return EPDK_TRUE;
        }
        tmpNode = tmpNode->pNext;
    }
    awos_arch_unlock_irq(cpu_sr);

    return EPDK_FALSE;
}


/*
*********************************************************************************************************
*                                               REQUEST PINS FOR DEVICE or GPIO
*
* Description: request pins for device.
*
* Arguments  : pGrpStat         point of struct user_gpio_set, gpio description
*              GrpSize          the number of user_gpio_set structure
*
* Returns    : the handle of the pins.
*
* Notes      :
*
*********************************************************************************************************
*/
__hdle esPINS_PinGrpReq(user_gpio_set_t *pGrpStat, __u32 GrpSize)
{
    uint32_t            cpu_sr;
    __pin_item_node_t   *tmpNode = NULL;
    uint32_t            logicPinNum = 0, i = 0;

    if(NULL == pGrpStat)
    {
        __err("parameter invalid!");
        return NULL;
    }

    if(GrpSize > 1)
    {
        __log("do not support request multiple gpio one time!");
        return NULL;
    }

    //allocate a pin item node
    tmpNode = (__pin_item_node_t *)k_malloc(sizeof(__pin_item_node_t) + sizeof(user_gpio_set_t));
    if (tmpNode == NULL)
    {
        __err("try to allocate pin resource node failed!");
        return NULL;
    }

    rt_memset(tmpNode, 0, sizeof(__pin_item_node_t));
    tmpNode->hPin = (__hdle)((unsigned long)tmpNode + sizeof(__pin_item_node_t));

    if(pGrpStat[i].port == 0)
    {
        __err("sys_config.fex script tools defines the PA=1");
        return NULL;
    }
    /*old melis sdk and pack tools define that portA = 1, but this sdk bsp starts PA0 from 0*/
    logicPinNum = (pGrpStat[i].port - 1) * PINS_PER_BANK + (pGrpStat[i].port_num);
    drv_gpio_pinmux_set_function(logicPinNum, pGrpStat[i].mul_sel);
    drv_gpio_set_pull_status(logicPinNum, pGrpStat[i].pull);
    drv_gpio_set_driving_level(logicPinNum, pGrpStat[i].drv_level);

    rt_memcpy(tmpNode->hPin, (void *)pGrpStat, sizeof(user_gpio_set_t));
    tmpNode->grpSize = 1;

    //insert the pin item node to the list of pin manager
    cpu_sr = awos_arch_lock_irq();
    tmpNode->pNext  = PINManger.pList;
    PINManger.pList = tmpNode;
    PINManger.nCnt++;
    awos_arch_unlock_irq(cpu_sr);

    return (__hdle)tmpNode;
}


/*
*********************************************************************************************************
*                                               RELEASE PINS FOR DEVICE or GPIOs
*
* Description: release pins for device.
*
* Arguments  : hPin     pins handle for device.
*              bRestore 1: set the pin function to disable which set the gpio register
*                       0: only move out the pin handler from the PINManger.pList and free the memory
*
* Returns    : result.
*
* Notes      :
*
*********************************************************************************************************
*/
int32_t esPINS_PinGrpRel(__hdle hPin, __s32 bRestore)
{
    uint32_t            cpu_sr;
    __pin_item_node_t   *tmpNode = NULL, *tmpNode0 = NULL;

    //check if the pin handle is valid
    if (PIN_CheckPinHdle(hPin) != EPDK_TRUE)
    {
        __err("pin handle is invalid!");
        return EPDK_FAIL;
    }

    //delete the node from the list
    cpu_sr = awos_arch_lock_irq();
    tmpNode = tmpNode0 = PINManger.pList;
    while (tmpNode)
    {
        if (tmpNode == (__pin_item_node_t *)hPin)
        {
            if (tmpNode == PINManger.pList)// if the hPin match the head of PINManger.pList
            {
                PINManger.pList = tmpNode->pNext;
            }
            else// else let the previewer node(tmpNode0) link the next node
            {
                tmpNode0->pNext = tmpNode->pNext;
            }
            PINManger.nCnt--;
            break;
        }
        tmpNode0 = tmpNode;
        tmpNode  = tmpNode->pNext;
    }
    awos_arch_unlock_irq(cpu_sr);

    if (tmpNode)
    {
        //release pin resource to csp pin
        if(bRestore)
        {
            user_gpio_set_t *pGrpStat = (user_gpio_set_t *)(tmpNode->hPin);
            uint32_t        logicPinNum = 0, i = 0;

            for(i = 0; i < tmpNode->grpSize; i ++)
            {
                if(0 == pGrpStat[i].port) 
                {
                    __err("sys_config.fex script tools defines the PA=1");
                    continue;
                }
                logicPinNum = (pGrpStat[i].port - 1) * PINS_PER_BANK + (pGrpStat[i].port_num);
                drv_gpio_pinmux_set_function(logicPinNum, 7);//set gpio func as disable status.
            }
        }

        //k_free pin item node
        k_free(tmpNode);
        return EPDK_OK;
    }

    return EPDK_FAIL;
}


/*
*********************************************************************************************************
*                                   PINS Get All STATUS
*
* Description: 获取申请的多组GPIO的全部状态(do not support temp)
*
* Arguments  : hPin     pins handle;
*              pGrpStat point of struct user_gpio_set, gpio description
*              GrpSize  the number of user_gpio_set structure
*              bFromHW  0: copy from the hPin structure, 
*                       1: read from the gpio register
*
* Returns    : result;
*
* Note       :
*********************************************************************************************************
*/
int32_t esPINS_GetPinGrpStat(__hdle hPin, user_gpio_set_t *pGrpStat, __u32 GrpSize, __bool bFromHW)
{
    __pin_item_node_t *tmpNode = NULL;

    //check if the pin handle is valid
    if (PIN_CheckPinHdle(hPin) != EPDK_TRUE)
    {
        __err("pin handle is invalid!");
        return EPDK_FAIL;
    }

#if 0
    if((NULL == pGrpStat) || (0 == GrpSize))
    {
        return EPDK_FAIL;
    }

    tmpNode = (__pin_item_node_t *)hPin;

    if(0 == bFromHW)
    {
        rt_memcpy((void *)pGrpStat, (void *)(tmpNode->hPin),  GrpSize * sizeof(user_gpio_set_t));
    }
    else
    {
        __u32 logicPinNum = 0, i = 0;
        unsigned long recvData = 0;
        user_gpio_set_t *grpStat = (user_gpio_set_t *)(tmpNode->hPin);

        for(i = 0; i < tmpNode->grpSize; i ++)
        {
            if(0 == grpStat[i].port)
            {
                __err("sys_config.fex script tools defines the PA=1");
                continue;
            }
            logicPinNum = (grpStat[i].port - 1) * PINS_PER_BANK + (grpStat[i].port_num);
            rt_strncpy(pGrpStat[i].gpio_name, grpStat[i].gpio_name, 32/*rt_strlen(grpStat[i].gpio_name)*/);
            pGrpStat[i].port     = grpStat[i].port;
            pGrpStat[i].port_num = grpStat[i].port_num;
            pGrpStat[i].mul_sel  = -1;/*drv_gpio_pinmux_get_function(logicPinNum, &recvData); 
                                        there is no function of reading mux value in the following layer*/
            pGrpStat[i].pull     = drv_gpio_get_pull_status(logicPinNum, (void*)&recvData);
            pGrpStat[i].drv_level= drv_gpio_get_driving_level(logicPinNum, (void*)&recvData);
            if(0 == pGrpStat[i].mul_sel)
            {
                pGrpStat[i].data = drv_gpio_get_input(logicPinNum, (void*)&recvData);
            }
            else if(1 == pGrpStat[i].mul_sel)
            {
                pGrpStat[i].data = drv_gpio_get_output(logicPinNum, (void*)&recvData);
            }
            else
            {
                pGrpStat[i].data = -1;
            }
        }
    }
#else
    __log("do not support get status of multiple gpio one time!");
#endif
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   PINS Get ONE STATUS
*
* Description: 获取申请的一组GPIO的，其中一个GPIO的状态，用名称进行匹配
*
* Arguments  : hPin      pins handle;
*              pPinStat  pointer to struct user_gpio_set_t
*              pPinName  pPin name
*              bFromHW   0: copy to pPinStat from the hPin structure, 
                         1: read to pPinStat from the gpio register
*
* Returns    : result;
*
* Note       :
*********************************************************************************************************
*/
int32_t esPINS_GetPinStat(__hdle hPin, user_gpio_set_t *pPinStat, const char *pPinName, __bool bFromHW)
{
    __pin_item_node_t   *tmpNode = NULL;
    uint32_t            logicPinNum = 0, i = 0; 
    unsigned long       recvData = 0;
    user_gpio_set_t     *grpStat = NULL;

    //check if the pin handle is valid
    if (PIN_CheckPinHdle(hPin) != EPDK_TRUE)
    {
        __err("pin handle is invalid!");
        return EPDK_FAIL;
    }

    if(NULL == pPinStat)
    {
        __err("parameter invalid, pPinStat=0x%X", pPinStat);
        return EPDK_FAIL;
    }

    tmpNode = (__pin_item_node_t *)hPin;
    grpStat = (user_gpio_set_t *)(tmpNode->hPin);

    if(tmpNode->grpSize > 1)
    {
        __err("only support set one pin");
        return EPDK_FAIL;
    }

    if(0 == grpStat[i].port)
    {
        __err("sys_config.fex script tools defines the PA=1");
        return EPDK_FAIL;
    }

    if(0 == bFromHW)
    {
        rt_memcpy((void *)pPinStat, (void *)(&grpStat[i]), sizeof(user_gpio_set_t));
    }
    else
    {
        logicPinNum = (grpStat[i].port - 1) * PINS_PER_BANK + (grpStat[i].port_num);
        rt_strncpy(pPinStat->gpio_name, grpStat[i].gpio_name, 32/*rt_strlen(grpStat[i].gpio_name)*/);
        pPinStat->port	   = grpStat[i].port;
        pPinStat->port_num = grpStat[i].port_num;
        pPinStat->mul_sel  = -1;/*drv_gpio_pinmux_get_function(logicPinNum, &recvData); 
                                    there is no function of reading mux value in the following layer*/
        pPinStat->pull	   = drv_gpio_get_pull_status(logicPinNum, (void*)&recvData);
        pPinStat->drv_level= drv_gpio_get_driving_level(logicPinNum, (void*)&recvData);
        if(0 == pPinStat->mul_sel)
        {
            pPinStat->data = drv_gpio_get_input(logicPinNum, (void*)&recvData);
        }
        else if(1 == pPinStat->mul_sel)
        {
            pPinStat->data = drv_gpio_get_output(logicPinNum, (void*)&recvData);
        }
        else
        {
            pPinStat->data = -1;
        }
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   PINS Set ONE STATUS
*
* Description: 设置申请的一组GPIO的，其中一个GPIO的状态，用名称进行匹配
*
* Arguments  : hPin             pins handle;
*              pGrpStat         pointer to struct user_gpio_set_t
*              pPinName         pPin name
*              bSetUserStat     1: change the hardware depending on pPinStat
*                               0: change the hardware depending on hPin
*
* Returns    : result;
*
* Note       :
*********************************************************************************************************
*/
int32_t esPINS_SetPinStat(__hdle hPin, user_gpio_set_t *pPinStat, const char *pPinName, __bool bSetUserStat)
{
    __pin_item_node_t   *tmpNode = NULL;
    uint32_t            logicPinNum = 0, i = 0;
    user_gpio_set_t     *setPinStat = NULL, *grpStat = NULL;

    //check if the pin handle is valid
    if (PIN_CheckPinHdle(hPin) != EPDK_TRUE)
    {
        __err("pin handle is invalid!");
        return EPDK_FAIL;
    }

    if(bSetUserStat && (NULL == pPinStat))
    {
        __err("parameter invalid, bSetUserStat=%d pPinStat=0x%X", bSetUserStat, pPinStat);
        return EPDK_FAIL;
    }

    tmpNode = (__pin_item_node_t *)hPin;
    grpStat = (user_gpio_set_t *)(tmpNode->hPin);

    if(tmpNode->grpSize > 1)
    {
        __err("only support set one pin");
        return EPDK_FAIL;
    }

    if(bSetUserStat)
    {
        setPinStat = pPinStat;
    }
    else
    {
        setPinStat = &grpStat[i];
    }

    if(0 == setPinStat->port)
    {
        __err("sys_config.fex script tools defines the PA=1");
        return EPDK_FAIL;
    }

    logicPinNum = (setPinStat->port - 1) * PINS_PER_BANK + (setPinStat->port_num);
    drv_gpio_pinmux_set_function(logicPinNum, setPinStat->mul_sel);
    drv_gpio_set_pull_status(logicPinNum, setPinStat->pull);
    drv_gpio_set_driving_level(logicPinNum, setPinStat->drv_level);

    if(1 == setPinStat->mul_sel)
    {
        drv_gpio_set_output(logicPinNum, setPinStat->data);
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   PINS Set ONE IO STATUS
*
* Description: 设置申请的一组GPIO的，其中一个GPIO的输入输出状态，用名称进行匹配
*              如果用户申请的时候只有一个GPIO，则名称填0即可
*
* Arguments  : hPin         pins handle;
*              bOut         GPIO_DIRECTION_INPUT
*                           GPIO_DIRECTION_OUTPUT
*              pPinName     gpio name string
*
* Returns    : result;
*
* Note       :
*********************************************************************************************************
*/
int32_t esPINS_SetPinIO(__hdle hPin, __bool bOut, const char *pPinName)
{
    __pin_item_node_t   *tmpNode = NULL;
    uint32_t            logicPinNum = 0, i = 0;
    user_gpio_set_t     *grpStat = NULL;

    //check if the pin handle is valid
    if (PIN_CheckPinHdle(hPin) != EPDK_TRUE)
    {
        __err("pin handle is invalid!");
        return EPDK_FAIL;
    }

    tmpNode = (__pin_item_node_t *)hPin;
    grpStat = (user_gpio_set_t *)(tmpNode->hPin);

    if(tmpNode->grpSize > 1)
    {
        __err("only support set one pin");
        return EPDK_FAIL;
    }

    if(0 == grpStat[i].port)
    {
        __err("sys_config.fex script tools defines the PA=1");
        return EPDK_FAIL;
    }
    logicPinNum = (grpStat[i].port - 1) * PINS_PER_BANK + (grpStat[i].port_num);
    drv_gpio_set_direction(logicPinNum, bOut);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   PINS Set ONE PULL STATUS
*
* Description: 设置申请的一组GPIO的，其中一个GPIO的上拉下拉状态，用名称进行匹配
*              如果用户申请的时候只有一个GPIO，则名称填0即可
*
* Arguments  : hPin         pins handle;
*              PullStat     GPIO_PULL_DOWN_DISABLED
*                           GPIO_PULL_UP
*                           GPIO_PULL_DOWN
*              pPinName     gpio name string
*
* Returns    : result;
*
* Note       :
*********************************************************************************************************
*/
int32_t esPINS_SetPinPull(__hdle hPin, __u32 PullStat, const char *pPinName)
{
    __pin_item_node_t   *tmpNode = NULL;
    uint32_t            logicPinNum = 0, i = 0;
    user_gpio_set_t     *grpStat = NULL;

    //check if the pin handle is valid
    if (PIN_CheckPinHdle(hPin) != EPDK_TRUE)
    {
        __err("pin handle is invalid!");
        return EPDK_FAIL;
    }

    tmpNode = (__pin_item_node_t *)hPin;
    grpStat = (user_gpio_set_t *)(tmpNode->hPin);

    if(tmpNode->grpSize > 1)
    {
        __err("only support set one pin");
        return EPDK_FAIL;
    }

    if(0 == grpStat[i].port)
    {
        __err("sys_config.fex script tools defines the PA=1");
        return EPDK_FAIL;
    }

    logicPinNum = (grpStat[i].port - 1) * PINS_PER_BANK + (grpStat[i].port_num);
    drv_gpio_set_pull_status(logicPinNum, PullStat);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   PINS Set ONE DRIVER LEVEL STATUS
*
* Description: 设置申请的一组GPIO的，其中一个GPIO的驱动能力状态，用名称进行匹配
*              如果用户申请的时候只有一个GPIO，则名称填0即可
*
* Arguments  : hPin         pins handle;
*              DriveLevel   GPIO_DRIVING_LEVEL0
*                           GPIO_DRIVING_LEVEL1
*                           GPIO_DRIVING_LEVEL2
*                           GPIO_DRIVING_LEVEL3
*              pPinName     gpio name string
*
* Returns    : result;
*
* Note       :
*********************************************************************************************************
*/
int32_t esPINS_SetPinDrive(__hdle hPin, __u32 DriveLevel, const char *pPinName)
{
    __pin_item_node_t   *tmpNode = NULL;
    uint32_t            logicPinNum = 0, i = 0;
    user_gpio_set_t     *grpStat = NULL;

    //check if the pin handle is valid
    if (PIN_CheckPinHdle(hPin) != EPDK_TRUE)
    {
        __err("pin handle is invalid!");
        return EPDK_FAIL;
    }

    tmpNode = (__pin_item_node_t *)hPin;
    grpStat = (user_gpio_set_t *)(tmpNode->hPin);

    if(tmpNode->grpSize > 1)
    {
        __err("only support set one pin");
        return EPDK_FAIL;
    }

    if(0 == grpStat[i].port)
    {
        __err("sys_config.fex script tools defines the PA=1");
        return EPDK_FAIL;
    }

    logicPinNum = (grpStat[i].port - 1) * PINS_PER_BANK + (grpStat[i].port_num);
    drv_gpio_set_driving_level(logicPinNum, DriveLevel);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   PINS READ ONE DATA STATUS
*
* Description: 读取申请的一组GPIO的，其中一个GPIO的电平状态，用名称进行匹配
*              如果用户申请的时候只有一个GPIO，则名称填0即可
*
* Arguments  : hPin         pins handle;
*              pPinName     gpio name string
*
* Returns    : result;
*
* Note       :
*********************************************************************************************************
*/
int32_t esPINS_ReadPinData(__hdle hPin, const char *pPinName)
{
    __pin_item_node_t   *tmpNode = NULL;
    uint32_t            logicPinNum = 0, i = 0;
    unsigned long       recvData = 0;
    user_gpio_set_t     *grpStat = NULL;

    //check if the pin handle is valid
    if (PIN_CheckPinHdle(hPin) != EPDK_TRUE)
    {
        __err("pin handle is invalid!");
        return EPDK_FAIL;
    }

    tmpNode = (__pin_item_node_t *)hPin;
    grpStat = (user_gpio_set_t *)(tmpNode->hPin);

    if(tmpNode->grpSize > 1)
    {
        __err("only support set one pin");
        return EPDK_FAIL;
    }

    if(0 == grpStat[i].port)
    {
        __err("sys_config.fex script tools defines the PA=1");
        return EPDK_FAIL;
    }

    if(1)//0 == grpStat[i].mul_sel)
    {
        logicPinNum = (grpStat[i].port - 1) * PINS_PER_BANK + (grpStat[i].port_num);
        drv_gpio_get_input(logicPinNum, (void*)&recvData);
    }
    else
    {
        __err("the pin mul_sel is not the input");
    }

    return recvData;
}


/*
*********************************************************************************************************
*                                   PINS WRITE ONE DATA STATUS
*
* Description: 设置申请的一组GPIO的，其中一个GPIO的电平状态，用名称进行匹配
*              如果用户申请的时候只有一个GPIO，则名称填0即可
*
* Arguments  : hPin         pins handle;
*              Value        GPIO_DATA_LOW
*                           GPIO_DATA_HIGH
*              pPinName     gpio name string
*
* Returns    : result;
*
* Note       :
*********************************************************************************************************
*/
int32_t esPINS_WritePinData(__hdle hPin, __u32 Value, const char *pPinName)
{
    __pin_item_node_t   *tmpNode = NULL;
    uint32_t            logicPinNum = 0, i = 0;
    user_gpio_set_t     *grpStat = NULL;

    //check if the pin handle is valid
    if (PIN_CheckPinHdle(hPin) != EPDK_TRUE)
    {
        __err("pin handle is invalid!");
        return EPDK_FAIL;
    }

    tmpNode = (__pin_item_node_t *)hPin;
    grpStat = (user_gpio_set_t *)(tmpNode->hPin);

    if(tmpNode->grpSize > 1)
    {
        __err("only support set one pin");
        return EPDK_FAIL;
    }

    if(0 == grpStat[i].port)
    {
        __err("sys_config.fex script tools defines the PA=1");
        return EPDK_FAIL;
    }

    if(1 == grpStat[i].mul_sel)
    {
        logicPinNum = (grpStat[i].port - 1) * PINS_PER_BANK + (grpStat[i].port_num);
        drv_gpio_set_output(logicPinNum, Value);
    }
    else
    {
        __err("the pin mul_sel is not the output");
    }

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       SET PIN INT MODE
*
* Description: set pin interrupte mode.
*
* Arguments  : hPin         pins handle;
*              IntMode      IRQ_TYPE_NONE
                            IRQ_TYPE_EDGE_RISING
                            IRQ_TYPE_EDGE_FALLING
                            IRQ_TYPE_EDGE_BOTH
                            IRQ_TYPE_LEVEL_HIGH
                            IRQ_TYPE_LEVEL_LOW
*
* Returns    : EPDK_OK if succeeded, EPDK_FAIL if failed.
*
* Note       :
*********************************************************************************************************
*/
int32_t esPINS_SetIntMode(__hdle hPin, __u32 IntMode)
{
    int32_t             cpu_sr;
    __pin_item_node_t   *tmpNode = NULL;

    //check if the pin handle is valid
    if (PIN_CheckPinHdle(hPin) != EPDK_TRUE)
    {
        __wrn("pin handle is invalid!");
        return EPDK_FAIL;
    }

    cpu_sr  = awos_arch_lock_irq();
    tmpNode = (__pin_item_node_t *)hPin;
    tmpNode->hHdler.flag   = IntMode;
    awos_arch_unlock_irq(cpu_sr);
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       REG PIN INT HANDER
*
* Description: register pin int service handler.
*
* Arguments  : hPin         pins handle;
*              hdler        handler for pin int call-back;
*              arg          argument for pin int handler;
*
* Returns    : EPDK_OK if succeeded, EPDK_FAIL if failed.
*
* Note       : hdler for the whole port
*********************************************************************************************************
*/
int32_t esPINS_RegIntHdler(__hdle hPin, __pCBK_t hdler, void *arg)
{
    int32_t             cpu_sr;
    __pin_item_node_t   *tmpNode = NULL;

    //check if the pin handle is valid
    if (PIN_CheckPinHdle(hPin) != EPDK_TRUE)
    {
        __wrn("pin handle is invalid!");
        return EPDK_FAIL;
    }

    cpu_sr  = awos_arch_lock_irq();
    tmpNode = (__pin_item_node_t *)hPin;
    tmpNode->hHdler.pEntry = hdler;
    tmpNode->hHdler.pArg   = arg;

    
    awos_arch_unlock_irq(cpu_sr);
    uint32_t        logicPinNum = 0;
    user_gpio_set_t *pGrpStat = (user_gpio_set_t *)(tmpNode->hPin);
    logicPinNum = (pGrpStat->port - 1) * PINS_PER_BANK + (pGrpStat->port_num);
    //int ret = drv_gpio_set_debounce(logicPinNum,0x70);
    //drv_gpio_get_debounce(hPin);
    //__err("langaojie test ret = %d",ret);

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       UNREG PIN INT HANDER
*
* Description: un-register pin int service handler.
*
* Arguments  : hPin         pins handle;
*              hdler        handler for pin int call-back.
*
* Returns    : EPDK_OK if succeeded, EPDK_FAIL if failed.
*
* Note       :
*********************************************************************************************************
*/
int32_t esPINS_UnregIntHdler(__hdle hPin, __pCBK_t hdler)
{
    int32_t             cpu_sr;
    __pin_item_node_t   *tmpNode = NULL;

    //check if the pin handle is valid
    if (PIN_CheckPinHdle(hPin) != EPDK_TRUE)
    {
        __wrn("pin handle is invalid!");
        return EPDK_FAIL;
    }

    cpu_sr  = awos_arch_lock_irq();
    tmpNode = (__pin_item_node_t *)hPin;
    tmpNode->hHdler.pEntry = NULL;
    tmpNode->hHdler.pArg   = NULL;
    tmpNode->hHdler.flag   = IRQ_TYPE_NONE;
    awos_arch_unlock_irq(cpu_sr);

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       ENABLE PIN INT
*
* Description: enable pin int.
*
* Arguments  : hPin         pins handle;
*
* Returns    : EPDK_OK      succeeded
*              EPDK_FAIL    failed.
*
* Note       :
*********************************************************************************************************
*/
int32_t esPINS_EnbaleInt(__hdle hPin)
{
    __pin_item_node_t   *tmpNode = NULL;
    uint32_t            logicPinNum = 0, virq = 0;
    user_gpio_set_t     *grpStat = NULL;

    //check if the pin handle is valid
    if (PIN_CheckPinHdle(hPin) != EPDK_TRUE)
    {
        __err("pin handle is invalid!");
        return EPDK_FAIL;
    }

    tmpNode = (__pin_item_node_t *)hPin;
    grpStat = (user_gpio_set_t *)(tmpNode->hPin);

    if(tmpNode->grpSize > 1)
    {
        __err("only support set one pin");
        return EPDK_FAIL;
    }

    /*calculate the pin number start from PA0, 'port * PINS_PER_BANK + port_number' */
    logicPinNum = (grpStat[0].port - 1) * PINS_PER_BANK + (grpStat[0].port_num);

    if(0 != drv_gpio_to_irq(logicPinNum, &virq))
    {
        __err("the pin[%d] is not within the virtual irq arrange!", logicPinNum);
        return EPDK_FAIL;
    }

    if(NULL == tmpNode->hHdler.pEntry)
    {
        __err("please set the irq pin service, service parameter and trigger mode!");
        return EPDK_FAIL;
    }
    drv_gpio_irq_request(virq, tmpNode->hHdler.pEntry, tmpNode->hHdler.flag, tmpNode->hHdler.pArg);

    return drv_gpio_irq_enable(virq);
}

/*
*********************************************************************************************************
*                                       DISABLE PIN INT
*
* Description: disable pin int.
*
* Arguments  : hPin         pins handle.
*
* Returns    : EPDK_OK if succeeded, EPDK_FAIL if failed.
*
* Note       :
*********************************************************************************************************
*/
int32_t esPINS_DisbaleInt(__hdle hPin)
{
    __pin_item_node_t   *tmpNode = NULL;
    uint32_t            logicPinNum = 0, virq = 0;
    user_gpio_set_t     *grpStat = NULL;

    //check if the pin handle is valid
    if (PIN_CheckPinHdle(hPin) != EPDK_TRUE)
    {
        __wrn("pin handle is invalid!");
        return EPDK_FAIL;
    }

    tmpNode = (__pin_item_node_t *)hPin;
    grpStat = (user_gpio_set_t *)(tmpNode->hPin);

    if(tmpNode->grpSize > 1)
    {
        __err("only support set one pin");
        return EPDK_FAIL;
    }

    /*calculate the pin number start from PA0, 'port * PINS_PER_BANK + port_number' */
    logicPinNum = (grpStat[0].port - 1) * PINS_PER_BANK + (grpStat[0].port_num);

    if(0 != drv_gpio_to_irq(logicPinNum, &virq))
    {
        __err("the pin[%d] is not within the virtual irq arrange!", logicPinNum);
        return EPDK_FAIL;
    }

    drv_gpio_irq_free(virq);

    return drv_gpio_irq_disable(virq);
}

/*
*********************************************************************************************************
*                                      QUERY PIN INT
*
* Description: query pin int status
*
* Arguments  : hPin         pins handle;
*              pIntStatus   buffer to store int status.
*
* Returns    : EPDK_OK if succeeded, EPDK_FAIL if failed.
*
* Note       :
*********************************************************************************************************
*/
int32_t esPINS_QueryInt(__hdle hPin, __bool *pIntStatus)
{
    __pin_item_node_t *tmpNode = NULL;

    //check if the pin handle is valid
    if (PIN_CheckPinHdle(hPin) != EPDK_TRUE)
    {
        __wrn("pin handle is invalid!");
        return EPDK_FAIL;
    }

    __log("do not support query pin interrupt status!");

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       CLEAR INT PENDING
*
* Description: clear pin int pending.
*
* Arguments  : hPin         pins handle.
*
* Returns    : EPDK_OK if succeeded, EPDK_FAIL if failed.
*
* Note       :
*********************************************************************************************************
*/
int32_t esPINS_ClearPending(__hdle hPin)
{
    __log("interrupt flag bit will be clear by hal layer automatically!");
    return EPDK_OK;
}
