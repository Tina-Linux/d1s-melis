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
#ifndef  __robin_cmd_mutex_c
#define  __robin_cmd_mutex_c


#include "foundation_i.h"



g_mutex_t  volatile  robin_cmd_mutex = NULL;


/***************************************************************************************************
*Name        : robin_cmd_mutex_create
*Prototype   : __s32 robin_cmd_mutex_create( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    g_create_mutex
*Description : create cmd mutex
*Other       :
***************************************************************************************************/
__s32 robin_cmd_mutex_create(void)
{
    if (robin_cmd_mutex == NULL)
    {
        robin_cmd_mutex = g_create_mutex();

        if (robin_cmd_mutex == NULL)
        {
            __err("Error in creating robin_cmd_mutex!\n");
            return -1;
        }
    }

    return 0;
}



/***************************************************************************************************
*Name        : robin_cmd_mutex_delete
*Prototype   : void  robin_cmd_mutex_delete( void )
*Arguments   : void
*Return      : void
*Description : delete cmd mutex
*Other       :
***************************************************************************************************/
void  robin_cmd_mutex_delete(void)
{
    __u8  err;

    if (robin_cmd_mutex != NULL)
    {
        g_delete_mutex(robin_cmd_mutex, &err);
        robin_cmd_mutex = NULL;
    }
}





#endif     //  ifndef __robin_cmd_mutex_c

/* end of robin_cmd_mutex.c */
