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
#include "gui_core_memdev_private.h"

#if GUI_SUPPORT_MEMDEV

/*********************************************************************
*
*       GUI_USAGE_BM structure
*
**********************************************************************
*/

typedef struct
{
    GUI_USAGE Public;
    struct
    {
        int BytesPerLine;
    } Private;
} GUI_USAGE_BM;

/*********************************************************************
*
*       static code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_USAGE_BM_AddPixel
*/
static void GUI_USAGE_BM_AddPixel(GUI_USAGE *p, int x, int y)
{
    U8 *pData;
    GUI_USAGE_BM *pThis = (GUI_USAGE_BM *)p;
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL

    if ((x >= pThis->Public.x0 + pThis->Public.XSize) | (x < pThis->Public.x0)
        | (y >= pThis->Public.y0 + pThis->Public.YSize) | (y < pThis->Public.y0))
    {
        GUI_DEBUG_ERROROUT2("GUI_USAGE_BM_AddPixel: parameters out of bounds", x, y);
    }

#endif
    x -= pThis->Public.x0;
    pData = (U8 *)(pThis + 1);
    pData += (y - pThis->Public.y0) * pThis->Private.BytesPerLine;
    pData += x >> 3;
    *pData |= 0x80 >> (x & 7);
}

/*********************************************************************
*
*       GUI_USAGE_BM_AddHLine
*/
static void GUI_USAGE_BM_AddHLine(GUI_USAGE *p, int x, int y, int len)
{
#if 0   /* Enable for the slower, but smaller version ... xxx*/

    while (len-- > 0)
    {
        GUI_USAGE_BM_AddPixel(h, x++, y);
    }

#else
    U8 *pData;
    GUI_USAGE_BM *pThis = (GUI_USAGE_BM *)p;
    /* Asserts */
    GUI_DEBUG_ERROROUT3_IF(x < pThis->Public.x0, "GUIDEV.c: MarkPixels: negative x offset, Act= %d, Border= %d, Clip= %d"
                           , x, pThis->Public.x0, GUI_Context.ClipRect[GUI_Context.SelLayer].x0);
    /* Calculate pointers */
    x -= pThis->Public.x0;
    pData = (U8 *)(pThis + 1);
    pData += (y - pThis->Public.y0) * pThis->Private.BytesPerLine;
    pData += x >> 3;
    /* Set bits */
    {
        int x1 = x + len - 1; /* last pixel */
        int NumBytes = (x1 >> 3) - (x >> 3);
        U8 Mask0 = 0xff >> (x & 7);
        U8 Mask1 = 0xff << (7 - (x1 & 7));

        if (NumBytes == 0)
        {
            *pData |= (Mask0 & Mask1);
        }
        else
        {
            *pData++ |= Mask0;               /* Mark first byte */

            /* Mark middle bytes */
            if (--NumBytes > 0)
            {
                memset(pData, 0xff, NumBytes);
                pData += NumBytes;
            }

            *pData |= Mask1;                 /* Mark last bytes */
        }
    }
#endif
}

/*********************************************************************
*
*       GUI_USAGE_BM_Clear
*/
static void GUI_USAGE_BM_Clear(GUI_USAGE *p)
{
    GUI_USAGE_BM *pThis = (GUI_USAGE_BM *) p;
    memset(pThis + 1, 0, pThis->Public.YSize * pThis->Private.BytesPerLine);
}

/*********************************************************************
*
*       GUI_USAGE_BM_GetNextDirty
*/
static int GUI_USAGE_BM_GetNextDirty(GUI_USAGE *p, int *pxOff, int yOff)
{
    int             x       = *pxOff;
    int             xEnd;
    GUI_USAGE_BM    *pThis  = (GUI_USAGE_BM *)p;
    int             xSize   = pThis->Public.XSize;
    U8             *pData   = (U8 *)(pThis + 1);

    if (yOff >= pThis->Public.YSize)
    {
        return 0;
    }

    pData += yOff * pThis->Private.BytesPerLine;
    pData += (x >> 3);

    if (x >= xSize)
    {
        return 0;
    }

    {
        /* Find first bit */
        int BytesLeft = ((xSize - 1) >> 3) - (x >> 3);
        /* Check first byte */
        U8 Data = (*pData++) << (x & 7);

        while (Data == 0)
        {
            if (BytesLeft == 0)
            {
                return 0;
            }

            Data = *pData++;
            BytesLeft--;
            x = (x + 8) & ~7;
        }

        while ((Data & 0x80) == 0)
        {
            Data <<= 1;
            x++;
        }

        /* Find last cleared byte */
        if (Data != 0xff)
        {
            /* This line is simply a speed-opt and may be eliminated */
            xEnd = x;

            while (Data & 0x40)
            {
                Data <<= 1;
                xEnd++;
            }
        }
        else
        {
            xEnd = x + 7;
        }

        if ((xEnd & 7) == 7)
        {
            while (--BytesLeft >= 0)
            {
                if ((Data = *pData++) == 0xff)
                {
                    xEnd += 8;
                }
                else
                {
                    while (Data & 0x80)
                    {
                        Data <<= 1;
                        xEnd++;
                    }

                    break;
                }
            }
        }
    }

    *pxOff = x;
    return xEnd - x + 1;
}

/*********************************************************************
*
*       Delete
*/
static void _GUI_USAGE_BM_Delete(GUI_MEMDEV_Handle hDevUsage)
{
    GUI_ALLOC_Free(hDevUsage);
}

/*********************************************************************
*
*       API List
*/
static const tUSAGE_APIList API =
{
    GUI_USAGE_BM_AddPixel,     /* tUSAGE_AddPixel*         */
    GUI_USAGE_BM_AddHLine,     /* tUSAGE_AddHLine*         */
    GUI_USAGE_BM_Clear,        /* tUSAGE_Clear*            */
    0,                         /* tUSAGE_CreateCompatible* */
    _GUI_USAGE_BM_Delete,       /* tUSAGE_Delete*           */
    GUI_USAGE_BM_GetNextDirty  /* tUSAGE_GetNextDirty*     */
};

/*********************************************************************
*
*       Exported routines
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_USAGE_BM_Create
*/
GUI_USAGE_Handle GUI_USAGE_BM_Create(int x0, int y0, int xsize, int ysize, int Flags)
{
    int MemSize;
    int BytesPerLine;
    GUI_USAGE_Handle hMem;
    GUI_USE_PARA(Flags);
    BytesPerLine = ((xsize + 15) >> 4) << 1; /* 2 byte alignment  xsize/8  mcl  */
    MemSize = ysize * BytesPerLine + sizeof(GUI_USAGE_BM);
    hMem = GUI_ALLOC_AllocZero(MemSize);

    /* Check if we can alloc sufficient memory */
    if (!hMem)
    {
        GUI_DEBUG_ERROROUT("GUI_USAGE_BM_Create: Too little memory");
        return 0;
    }

    {
        GUI_USAGE_BM *pUsage;
        GUI_LOCK();
        pUsage = (GUI_USAGE_BM *)GUI_ALLOC_h2p(hMem);
        pUsage->Public.x0           = x0;
        pUsage->Public.y0           = y0;
        pUsage->Public.XSize        = xsize;
        pUsage->Public.YSize        = ysize;
        pUsage->Public.pAPI         = &API;
        pUsage->Public.UseCnt       = 1;
        pUsage->Private.BytesPerLine = BytesPerLine;
        GUI_UNLOCK();
    }

    return hMem;
}

#else

void GUIDEV_UsageBM(void) {} /* avoid empty object files */

#endif /* GUI_SUPPORT_MEMDEV */

/*************************** end of file ****************************/
