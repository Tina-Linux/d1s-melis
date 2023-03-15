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
#include <math.h>
#include "gui_core_kernel_private.h"
/*********************************************************************
*
*       Types
*
**********************************************************************
*/

typedef struct
{
    float x, y;
} tfPoint;

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _fround
*/
static int _fround(float f)
{
    if (f > 0)
    {
        return f + 0.5;
    }

    return f - 0.5;
}

/*********************************************************************
*
*       _Normalize
*/
static void _Normalize(tfPoint *pfPoint)
{
    float fx = pfPoint->x;
    float fy = pfPoint->y;
    float r = sqrt(fx * fx + fy * fy);

    if (r > 0)
    {
        pfPoint->x = fx / r;
        pfPoint->y = fy / r;
    }
}

/*********************************************************************
*
*       _ReverseLen
*/
static void _ReverseLen(tfPoint *pfPoint)
{
    float fx = pfPoint->x;
    float fy = pfPoint->y;
    float r = sqrt(fx * fx / 2 + fy * fy / 2);

    if (r > 0)
    {
        pfPoint->x = fx / r / r;
        pfPoint->y = fy / r / r;
    }
}

/*********************************************************************
*
*       _GetVect
*/
static void _GetVect(tfPoint *pfPoint, const GUI_POINT *pSrc, int NumPoints, int Index)
{
    int Off0 = (Index + NumPoints - 1) % NumPoints;
    int Off1 = Index % NumPoints;
    pfPoint->x = pSrc[Off1].x - pSrc[Off0].x;
    pfPoint->y = pSrc[Off1].y - pSrc[Off0].y;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_EnlargePolygon
*/
#if 0
void GUI_EnlargePolygon(GUI_POINT *pDest, const GUI_POINT *pSrc, int NumPoints, int Len)
{
    int j;

    /* Calc destination points */
    for (j = 0; j < NumPoints; j++)
    {
        int x, y;
        tfPoint aVect[2];
        /* Get the vectors */
        _GetVect(&aVect[0], pSrc, NumPoints, j);
        _GetVect(&aVect[1], pSrc, NumPoints, j + 1);
        /* Normalize the vectors and add vectors */
        _Normalize(&aVect[0]);
        _Normalize(&aVect[1]);
        aVect[0].x += aVect[1].x;
        aVect[0].y += aVect[1].y;
        /* Resulting vector needs to be normalized again */
        _Normalize(&aVect[0]);
        x =  _fround(aVect[0].y * Len);
        y = -_fround(aVect[0].x * Len);
        /* Store destination */
        (pDest + j)->x = (pSrc + j)->x + x;
        (pDest + j)->y = (pSrc + j)->y + y;
    }
}

#else

void GUI_EnlargePolygon(GUI_POINT *pDest, const GUI_POINT *pSrc, int NumPoints, int Len)
{
    int j;

    /* Calc destination points */
    for (j = 0; j < NumPoints; j++)
    {
        int x, y;
        tfPoint aVect[2];
        /* Get the vectors */
        _GetVect(&aVect[0], pSrc, NumPoints, j);
        _GetVect(&aVect[1], pSrc, NumPoints, j + 1);
        /* Normalize the vectors and add vectors */
        _Normalize(&aVect[0]);
        _Normalize(&aVect[1]);
        aVect[0].x += aVect[1].x;
        aVect[0].y += aVect[1].y;
        /* Resulting vector needs to be normalized again */
        _ReverseLen(&aVect[0]);
        x =  _fround(aVect[0].y * Len);
        y = -_fround(aVect[0].x * Len);
        /* Store destination */
        (pDest + j)->x = (pSrc + j)->x + x;
        (pDest + j)->y = (pSrc + j)->y + y;
    }
}
#endif

/*************************** End of file ****************************/
