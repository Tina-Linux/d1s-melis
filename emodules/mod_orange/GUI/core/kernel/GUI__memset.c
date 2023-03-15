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
#include <string.h>           /* needed for definition of NULL */
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
*  Replacement for the memset function. The advantage is high speed
*  on all systems (sometime up to 10 times as fast as the one
*  in the library)
*  Main idea is to write int-wise.
*/
void GUI__memset(U8 *p, U8 Fill, int NumBytes)
{
    int NumInts;

    /* Write bytes until we are done or have reached an int boundary */
    while (NumBytes && ((sizeof(int) - 1) & (unsigned long)p))
    {
        *p++ = Fill;
        NumBytes--;
    }

    /* Write Ints */
    NumInts = (unsigned)NumBytes / sizeof(int);

    if (NumInts)
    {
        int FillInt;
        int *pInt;
        NumBytes &= (sizeof(int) - 1);

        if (sizeof(int) == 2)          /* May some compilers generate a warning at this line: Condition is alwaws true/false */
        {
            FillInt = Fill * 0x101;      /* May some compilers generate a warning at this line: Unreachable code */
        }
        else if (sizeof(int) == 4)     /* May some compilers generate a warning at this line: Condition is alwaws true/false */
        {
            FillInt = Fill * 0x1010101;  /* May some compilers generate a warning at this line: Unreachable code */
        }

        pInt = (int *)p;

        /* Fill large amount of data at a time */
        while (NumInts >= 4)
        {
            *pInt++ = FillInt;
            *pInt++ = FillInt;
            *pInt++ = FillInt;
            *pInt++ = FillInt;
            NumInts -= 4;
        }

        /* Fill one int at a time */
        while (NumInts)
        {
            *pInt++ = FillInt;
            NumInts--;
        }

        p = (U8 *)pInt;
    }

    /* Fill the remainder byte wise */
    while (NumBytes)
    {
        *p++ = Fill;
        NumBytes--;
    }
}

/*************************** End of file ****************************/
