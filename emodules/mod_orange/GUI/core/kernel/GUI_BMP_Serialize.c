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
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define DIB_HEADER_MARKER ((U16) ('M' << 8) | 'B') /* DIB Header Marker - used in writing DIBs to files */

#define SIZEOF_BITMAPFILEHEADER 14
#define SIZEOF_BITMAPINFOHEADER 40

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _SendU16
*/
static void _SendU16(GUI_CALLBACK_VOID_U8_P *pfSerialize, U16 Data, void *p)
{
    (*pfSerialize)((U8)Data, p);
    (*pfSerialize)(Data >> 8, p);
}

/*********************************************************************
*
*       _SendU32
*/
static void _SendU32(GUI_CALLBACK_VOID_U8_P *pfSerialize, U32 Data, void *p)
{
    _SendU16(pfSerialize, (U16)Data, p);
    _SendU16(pfSerialize, Data >> 16, p);
}

/*********************************************************************
*
*       Exported routines
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_BMP_SerializeEx
*/
void GUI_BMP_SerializeEx(GUI_CALLBACK_VOID_U8_P *pfSerialize, int x0, int y0, int xSize, int ySize, void *p)
{
    int x, y, i;
    U32 BitmapOffset, FileSize;
    int BPP          = LCD_GetBitsPerPixel();
    int BytesPerLine = ((BPP > 8) ? (2 * xSize + 2) : (xSize + 3)) & ~3;
    int NumColors    = (BPP > 8) ? 0 : (1 << BPP);
    /* Write BITMAPFILEHEADER */
    BitmapOffset   = SIZEOF_BITMAPFILEHEADER
                     + SIZEOF_BITMAPINFOHEADER
                     + NumColors * 4;
    FileSize       = BitmapOffset
                     + ySize * BytesPerLine;
    _SendU16(pfSerialize, DIB_HEADER_MARKER, p);       /* bfType */
    _SendU32(pfSerialize, FileSize, p);                /* bfSize */
    _SendU32(pfSerialize, 0, p);                       /* bfReserved1, bfReserved2 */
    _SendU32(pfSerialize, BitmapOffset, p);            /* bfOffBits */
    /* Write BITMAPINFOHEADER */
    _SendU32(pfSerialize, SIZEOF_BITMAPINFOHEADER, p); /* biSize */
    _SendU32(pfSerialize, xSize, p);                   /* biWidth */
    _SendU32(pfSerialize, ySize, p);                   /* biHeighth */
    _SendU16(pfSerialize, 1, p);                       /* biPlanes */
    _SendU16(pfSerialize, (BPP  <= 8) ? 8 : 16, p);    /* biBitCount */
    _SendU32(pfSerialize, 0, p);                       /* biCompression */
    _SendU32(pfSerialize, 0, p);                       /* biSizeImage */
    _SendU32(pfSerialize, 0, p);                       /* biXPelsPerMeter */
    _SendU32(pfSerialize, 0, p);                       /* biYPelsPerMeter */
    _SendU32(pfSerialize, NumColors, p);               /* biClrUsed */
    _SendU32(pfSerialize, 0, p);                       /* biClrImportant */

    /* Write palette */
    for (i = 0; i < NumColors; i++)
    {
        U32 Color;
        Color = GUI_Index2Color(i);
        Color = ((Color >> 16) & 255) | (Color & 0xff00) | ((Color & 0xff) << 16);
        _SendU32(pfSerialize, Color, p);
    }

    /* Write pixels */
    for (y = ySize - 1; y >= 0; y--)
    {
        for (x = 0; x < xSize; x++)
        {
            if (BPP <= 8)
            {
                pfSerialize(LCD_GetPixelIndex(x0 + x, y0 + y), p);
            }
            else
            {
                U16 Color16;
                U32 Color = LCD_GetPixelColor(x0 + x, y0 + y);
                int b = ((Color >> 16) * 31 + 127) / 255;
                int g = (((Color >> 8) & 255) * 31 + 127) / 255;
                int r = ((Color & 255) * 31 + 127) / 255;
                Color16 = (r << 10) | (g << 5) | b;       /* 16 bpp Bitmaps in windows are 555: rrrrrgggggbbbbb */
                _SendU16(pfSerialize, Color16, p);
            }
        }

        /* Align pointer to next U32 */
        for (i = BytesPerLine & 3; i > 0; i--)
        {
            pfSerialize(0, p);
        }
    }
}

/*********************************************************************
*
*       GUI_BMP_Serialize
*/
void GUI_BMP_Serialize(GUI_CALLBACK_VOID_U8_P *pfSerialize, void *p)
{
    GUI_BMP_SerializeEx(pfSerialize, 0, 0, LCD_GetXSize(), LCD_GetYSize(), p);
}

/*************************** End of file ****************************/
