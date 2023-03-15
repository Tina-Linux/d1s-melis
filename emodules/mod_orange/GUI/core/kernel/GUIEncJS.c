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
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       DB2SJIS
*/
static U16 DB2SJIS(U8 Byte0, U8 Byte1)
{
    return Byte1 | (((U16)Byte0) << 8);
}

/*********************************************************************
*
*       _GetLineDistX_SJIS
*/
static int _GetLineDistX_SJIS(const char GUI_UNI_PTR *s, int Len)
{
    int Dist = 0;

    if (s)
    {
        U8 c0;

        while (((c0 = *(const U8 *)s) != 0) && Len >= 0)
        {
            s++;
            Len--;

            if (c0 > 127)
            {
                U8  c1 = *(const U8 *)s++;
                Len--;
                Dist += GUI_GetCharDistX(DB2SJIS(c0, c1));
            }
            else
            {
                Dist += GUI_GetCharDistX(c0);
            }
        }
    }

    return Dist;
}

/*********************************************************************
*
*       _GetLineLen_SJIS
* Purpose:
*   Returns the number of characters in a string.
*
* NOTE:
*   The return value can be used as offset into the
*   string, which means that double characters count double
*/
static int _GetLineLen_SJIS(const char GUI_UNI_PTR *s, int MaxLen)
{
    int Len = 0;
    U8 c0;

    while (((c0 = *(const U8 *)s) != 0) && Len < MaxLen)
    {
        s++;

        if (c0 > 127)
        {
            Len++;
            s++;
        }
        else
        {
            switch (c0)
            {
                case '\n':
                    return Len;
            }
        }

        Len++;
    }

    return Len;
}

/*********************************************************************
*
*       _DispLine_SJIS
*/
static void _DispLine_SJIS(const char GUI_UNI_PTR *s, int Len)
{
    U8 c0;

    while (--Len >= 0)
    {
        c0 = *(const U8 *)s++;

        if (c0 > 127)
        {
            U8  c1 = *(const U8 *)s++;
            Len--;
            GL_DispChar(DB2SJIS(c0, c1));
        }
        else
        {
            GL_DispChar(c0);
        }
    }
}

/*********************************************************************
*
*       GUI_ENC_APIList_SJIS, API list
*/
const tGUI_ENC_APIList GUI_ENC_APIList_SJIS =
{
    _GetLineDistX_SJIS,
    _GetLineLen_SJIS,
    _DispLine_SJIS
};

/*************************** End of file ****************************/
