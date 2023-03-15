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
#ifndef  __cmdq_c
#define  __cmdq_c


#include "foundation_i.h"
#include <kapi.h>


#define ROBIN_CMDQ_LEN                  1
#define ROBIN_MSG_BUF_LEN               16        // must be 2^n


g_Queue  volatile  robin_cmdQ = NULL;


static robin_msg_t              robin_msg_array[ROBIN_MSG_BUF_LEN];
static __u32          volatile  robin_msg_counter = 0;


/***************************************************************************************************
*Name        : cmdQ_create
*Prototype   : __s32 cmdQ_create( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : create command queue
*Other       :
***************************************************************************************************/
__s32 cmdQ_create(void)
{
    if (robin_cmdQ == NULL)
    {
        robin_cmdQ = g_QCreate(ROBIN_CMDQ_LEN);

        if (robin_cmdQ == NULL)
        {
            __err("Error in creating cmdQ!\n");
            return -1;
        }
    }

    return 0;
}



/***************************************************************************************************
*Name        : cmdQ_delete
*Prototype   : void  cmdQ_delete( void )
*Arguments   : void
*Return      : void
*Description : delete command queue
*Other       :
***************************************************************************************************/
void  cmdQ_delete(void)
{
    __u8  err;

    if (robin_cmdQ != NULL)
    {
        g_QDel(robin_cmdQ, OS_DEL_ALWAYS, &err);
        robin_cmdQ = NULL;
    }
}



robin_msg_t  *get_robin_msg_buf(void)
{
    return  &(robin_msg_array[robin_msg_counter++ & (ROBIN_MSG_BUF_LEN - 1)]);
}





#endif     //  ifndef __cmdq_c

/* end of cmdq.c */
