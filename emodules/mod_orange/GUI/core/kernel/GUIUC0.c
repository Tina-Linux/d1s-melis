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
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_UC2DB
*
* Purpose:
*   Convert Convert UNICODE 16-bit value into double byte version
*   - Byte0: First byte, msb
*   - Byte1: Last  byte, lsb
*/
void GUI_UC2DB(U16 Code, U8 *pOut)
{
    /* move regular ASCII to (unused) 0xE000-0xE0ff area */
    if (Code < 0x100)
    {
        if (Code != 0) /* end marker ? */
        {
            Code += 0xe000;
        }
    }
    else
    {
        /* If Low byte 0: Move to (unused) 0xE100-0xE1ff area */
        if ((Code & 0xff) == 0)
        {
            Code = (Code >> 8) + 0xe100;
        }
    }

    /* Convert into double byte, putting msb first*/
    *pOut++ = Code >> 8;     /* Save msb first */
    *pOut   = Code & 0xff;
}

/*********************************************************************
*
*       GUI_DB2UC
*
* Purpose:
*   Convert Unicode double byte into UNICODE.
*   - Byte0: First byte, msb
*   - Byte1: Last  byte, lsb
*/
U16 GUI_DB2UC(U8 Byte0, U8 Byte1)
{
    if (Byte0 == 0)
    {
        return 0;
    }

    if ((Byte0 & 0xfe) == 0xe0)
    {
        if (Byte0 == 0xe0)        /* ASCII */
        {
            return Byte1;
        }

        return ((U16)Byte1) << 8; /* low byte was zero */
    }

    return Byte1 | (((U16)Byte0) << 8);
}

/*************************** End of file ****************************/
