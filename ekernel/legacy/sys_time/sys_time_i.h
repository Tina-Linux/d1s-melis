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
#ifndef _SYS_TIME_I_H_
#define _SYS_TIME_I_H_
#include <typedef.h>
#include <ktype.h>
#include <rtdef.h>

#define TIMER_USER_NAME_SIZE    (16)

typedef struct __TIME_TMR_NODE
{
    uint32_t    timerId;        //timer id
    uint32_t    precision;      //timer precision
    uint32_t    nPeriod;        //period of the timer
    uint32_t    isOneShot;      //one shot or continuous
    uint32_t    nIrqNo;         //irq numer
    __pCBK_t    pHdlr;          //timer interrupt handle
    void        *pArg;          //argument for interrupt handle
    char        pUsr[TIMER_USER_NAME_SIZE];
} __time_tmr_node_t;

typedef struct __TIME_CNTR_NODE
{
    rt_device_t     hCntr;  //counter handle request from csp
    __pCB_ClkCtl_t  pCb;    //call-back function for clock-change
    __s32           nStat;  //status
    char            pUsr[TIMER_USER_NAME_SIZE];
    struct __TIME_CNTR_NODE *pNext;
} __time_cntr_node_t;

typedef struct __TIME_CNTR
{
    __time_cntr_node_t  *pList;
    __s32               nCnt;
} __time_cntr_t;


#endif /* _SYS_TIME_I_H_    */