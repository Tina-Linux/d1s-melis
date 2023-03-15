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
#include <stddef.h>           /* needed for definition of NULL */
#include "gui_core_kernel_private.h"
/*********************************************************************
*
*       public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI__memset
*
* Purpose:
*  Equivalent to the memset function, for 16 bit Data.
*/
void GUI__memset16(U16 *p, U16 Fill, int NumWords)
{
    /* Code for 16 bit CPUs */
    if (sizeof(int) == 2)   /* May some compilers generate a warning at this line: Condition is alwaws true/false */
    {
        if (NumWords >= 8)    /* May some compilers generate a warning at this line: Unreachable code */
        {
            *p++ = Fill;
            *p++ = Fill;
            *p++ = Fill;
            *p++ = Fill;
            *p++ = Fill;
            *p++ = Fill;
            *p++ = Fill;
            *p++ = Fill;
            NumWords -= 8;
        }

        while (NumWords)
        {
            *p++ = Fill;
            NumWords--;
        }
    }
    else
    {
        int NumInts;          /* May some compilers generate a warning at this line: Unreachable code */

        /* Write 16 bit until we are done or have reached an int boundary */
        if (2 & (unsigned long)p)
        {
            *p++ = Fill;
            NumWords--;
        }

        /* Write Ints */
        NumInts = (unsigned)NumWords / (sizeof(int) / 2);

        if (NumInts)
        {
            int FillInt;
            int *pInt;
            FillInt = Fill * 0x10001;
            pInt = (int *)p;

            /* Fill large amount of data at a time */
            if (NumInts >= 4)
            {
                do
                {
                    *pInt       = FillInt;
                    *(pInt + 1) = FillInt;
                    *(pInt + 2) = FillInt;
                    *(pInt + 3) = FillInt;
                    pInt += 4;
                } while ((NumInts -= 4) >= 4);
            }

            /* Fill one int at a time */
            while (NumInts)
            {
                *pInt++ = FillInt;
                NumInts--;
            }

            p = (U16 *)pInt;
        }

        /* Fill the remainder */
        if (NumWords & 1)
        {
            *p = Fill;
        }
    }
}

/*************************** End of file ****************************/
