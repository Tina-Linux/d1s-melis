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
#ifndef __SYS_PINS_I_H__
#define __SYS_PINS_I_H__
#include <typedef.h>
#include <ktype.h>
#include <kconfig.h>

__bool  PIN_CheckPinHdle(__hdle hPin);

//define pin int hander type
typedef struct __PIN_INT_HDLER
{
    void*           pEntry;             //handler entry
    void            *pArg;              //argument for dma hander
    uint32_t        flag;
} __pin_int_hdler_t;

//define pin item node
typedef struct __PIN_ITEM_NODE
{
    __hdle                  hPin;       //pin handle, request from pin csp
    __pin_int_hdler_t       hHdler;     //pin int handler
    struct __PIN_ITEM_NODE  *pNext;     //pointer for the pin list
    uint32_t                grpSize;
} __pin_item_node_t;

//define the pin manager
#if (defined (CONFIG_SOC_SUN3IW2) || defined (CONFIG_SOC_SUN8IW19)) || defined (CONFIG_SOC_SUN20IW1)
typedef struct __PIN_MANAGER
{
    __pin_item_node_t       *pList;     //list of pin resource
    int32_t                 nCnt;       //pin count
    int32_t                 nIrqNo;     //irq number of PIOC module
} __pin_manager_t;
#elif defined CONFIG_SOC_SUN3IW1
typedef struct __PIN_MANAGER
{
    __pin_item_node_t       *pList;     //list of pin resource
    int32_t                 nCnt;       //pin count
    int32_t                 nIrqNoD;    //irq number of PIOC module
    int32_t                 nIrqNoE;    //irq number of PIOC module
    int32_t                 nIrqNoF;    //irq number of PIOC module
} __pin_manager_t;
#endif

#endif //#ifndef __SYS_PINS_I_H__