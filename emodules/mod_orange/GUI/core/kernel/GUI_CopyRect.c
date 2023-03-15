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
#include <string.h>
#include "gui_core_kernel_private.h"

static __u32  GUI_MemCopyEx(__u32  bpp, void *srcbuf, __u32 srcbuf_xsize, __u32 srcbuf_ysize,
                            void *dstbuf, __u32 dstbuf_xsize, __u32 dstbuf_ysize
                            , GUI_RECT *srcrect, GUI_RECT *dstrect)
{
    void                *src_addr;
    void                *dst_addr;
    __u32                srcx_off;
    __u32                srcy_off;
    __u32                dstx_off;
    __u32                dsty_off;
    __u32                i;
    __u32                width;
    __u32                height;
    __u32                srcfb_width;
    __u32                dstfb_width;

    if (GUI_GetScnDir() == GUI_SCNDIR_NORMAL)
    {
        srcx_off    =  srcrect->x0;
        srcy_off    =  srcrect->y0;
        dstx_off    =  dstrect->x0;
        dsty_off    =  dstrect->y0;
        srcfb_width = (srcbuf_xsize * bpp + 7) >> 3;
        dstfb_width = (dstbuf_xsize * bpp + 7) >> 3;
        width       = ((srcrect->x1 - srcrect->x0 + 1) * bpp + 7) >> 3;
        height      = srcrect->y1 - srcrect->y0 + 1;
    }
    else if (GUI_GetScnDir() == GUI_SCNDIR_ROTATE90)
    {
        srcx_off    =  srcrect->y0;
        srcy_off    =  srcbuf_xsize - srcrect->x1 - 1;
        dstx_off    =  dstrect->y0;
        dsty_off    =  dstbuf_xsize - dstrect->x1 - 1;
        width      = ((srcrect->y1 - srcrect->y0 + 1) * bpp + 7) >> 3;
        height     = srcrect->x1 - srcrect->x0 + 1;
        srcfb_width = (srcbuf_ysize * bpp + 7) >> 3;
        dstfb_width = (dstbuf_ysize * bpp + 7) >> 3;
    }
    else if (GUI_GetScnDir() == GUI_SCNDIR_ROTATE180)
    {
        srcx_off    =  srcbuf_xsize - srcrect->x1 - 1;
        srcy_off    =  srcbuf_ysize - srcrect->y1 - 1;
        dstx_off    =  dstbuf_xsize - dstrect->x1 - 1;
        dsty_off    =  dstbuf_ysize - dstrect->y1 - 1;
        srcfb_width = (srcbuf_xsize * bpp + 7) >> 3;
        dstfb_width = (dstbuf_xsize * bpp + 7) >> 3;
        width       = ((srcrect->x1 - srcrect->x0 + 1) * bpp + 7) >> 3;
        height      = srcrect->y1 - srcrect->y0 + 1;
    }
    else
    {
        srcx_off    =  srcbuf_ysize - srcrect->y1 - 1;
        srcy_off    =  srcrect->x0;
        dstx_off    =  dstbuf_ysize - dstrect->y1 - 1;
        dsty_off    =  dstrect->x0;
        width       = ((srcrect->y1 - srcrect->y0 + 1) * bpp + 7) >> 3;
        height      = srcrect->x1 - srcrect->x0 + 1;
        srcfb_width = (srcbuf_ysize * bpp + 7) >> 3;
        dstfb_width = (dstbuf_ysize * bpp + 7) >> 3;
    }

    src_addr    = (void *)((unsigned long)srcbuf + srcfb_width * srcy_off + ((srcx_off * bpp + 7) / 8));    /*get source address */
    /*get framebuffer destination address */
    dst_addr    = (void *)((unsigned long)dstbuf + dstfb_width * dsty_off + ((dstx_off * bpp + 7) / 8));

    for (i = 0; i < height; i++)
    {
        orange_memcpy(dst_addr, src_addr, width);
        src_addr = (void *)((unsigned long)src_addr + srcfb_width);
        dst_addr = (void *)((unsigned long)dst_addr + dstfb_width);
    }

    return OK;
}


/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_SetRectMem
*/
void GUI_SetRectMem(GUI_RECT *pDestRect, void *pBuffer, U32 bufferXSize, U32 bufferYSize, U32 rectX0, U32 rectY0)
{
    GUI_LOCK();

    if (pDestRect->x0 < GUI_Context.ClipRect[GUI_Context.SelLayer].x0)
    {
        pDestRect->x0 = GUI_Context.ClipRect[GUI_Context.SelLayer].x0;
    }

    if (pDestRect->x1 > GUI_Context.ClipRect[GUI_Context.SelLayer].x1)
    {
        pDestRect->x1 = GUI_Context.ClipRect[GUI_Context.SelLayer].x1;
    }

    if (pDestRect->y0 < GUI_Context.ClipRect[GUI_Context.SelLayer].y0)
    {
        pDestRect->y0 = GUI_Context.ClipRect[GUI_Context.SelLayer].y0;
    }

    if (pDestRect->y1 > GUI_Context.ClipRect[GUI_Context.SelLayer].y1)
    {
        pDestRect->y1 = GUI_Context.ClipRect[GUI_Context.SelLayer].y1;
    }

    if (GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_COLOR_ARGB8888
        || GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_COLOR_RGB0888)
    {
        if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_NORMAL)
        {
            //  memcpy
            int  i = 0;
            int  ySize = pDestRect->y1 - pDestRect->y0 + 1;
            int  xSize = pDestRect->x1 - pDestRect->x0 + 1;
            U32 *pDestBuffer = (U32 *)((U32 *)orange.fbaddr[GUI_Context.SelLayer] + pDestRect->y0 * (Screen_Context.LCDXSize[GUI_Context.SelLayer]) + pDestRect->x0);
            U32 *pSrcBuffer  = (U32 *)((U32 *)pBuffer + rectY0 * bufferXSize + rectX0);

            for (i = 0; i < ySize; i++)
            {
                memcpy(pDestBuffer, pSrcBuffer, xSize << 2);
                pDestBuffer =  pDestBuffer + Screen_Context.LCDXSize[GUI_Context.SelLayer];
                pSrcBuffer  =  pSrcBuffer +  bufferXSize;
            }
        }
        else
        {
            // set pixel
            int  i = 0;
            int j = 0;
            int  ySize = pDestRect->y1 - pDestRect->y0 + 1;
            int  xSize = pDestRect->x1 - pDestRect->x0 + 1;
            U32 *pSrcBuffer  = (U32 *)((U32 *)pBuffer + rectY0 * bufferXSize + rectX0);

            for (i = 0; i < ySize; i++)
            {
                for (j = 0; j < xSize; j++)
                {
                    LCDDEV_L0_SetPixelIndex(pDestRect->x0 + j,  pDestRect->y0 + i, *(pSrcBuffer + j));
                }

                pSrcBuffer  =  pSrcBuffer +  bufferXSize;
            }
        }
    }
    else
    {
        if (GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_MONO_8BPP)
        {
            if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_NORMAL)
            {
                //  memcpy
                int  i = 0;
                int  ySize = pDestRect->y1 - pDestRect->y0 + 1;
                int  xSize = pDestRect->x1 - pDestRect->x0 + 1;
                U8 *pDestBuffer = (U8 *)((U8 *)orange.fbaddr[GUI_Context.SelLayer] + pDestRect->y0 * (Screen_Context.LCDXSize[GUI_Context.SelLayer]) + pDestRect->x0);
                U8 *pSrcBuffer  = (U8 *)((U8 *)pBuffer + rectY0 * bufferXSize + rectX0);

                for (i = 0; i < ySize; i++)
                {
                    memcpy(pDestBuffer, pSrcBuffer, xSize);
                    pDestBuffer =  pDestBuffer + Screen_Context.LCDXSize[GUI_Context.SelLayer];
                    pSrcBuffer  =  pSrcBuffer +  bufferXSize;
                }
            }
            else
            {
                // set pixel
                int  j = 0;
                int  i = 0;
                int  ySize = pDestRect->y1 - pDestRect->y0 + 1;
                int  xSize = pDestRect->x1 - pDestRect->x0 + 1;
                //U8 *pDestBuffer =  (U8*)((U8 *)orange.fbaddr[GUI_Context.SelLayer] + pDestRect->y0 *(Screen_Context.LCDXSize[GUI_Context.SelLayer]) + pDestRect->x0);
                U8 *pSrcBuffer  = (U8 *)((U8 *)pBuffer + rectY0 * bufferXSize + rectX0);

                for (i = 0; i < ySize; i++)
                {
                    for (j = 0; j < xSize; j++)
                    {
                        LCDDEV_L0_SetPixelIndex(pDestRect->x0 + j,  pDestRect->y0 + i, *(pSrcBuffer + j));
                    }

                    pSrcBuffer  =  pSrcBuffer +  bufferXSize;
                }
            }
        }
    }

    GUI_UNLOCK();
}

/***********************************************************************
*
*       GUI_GetRectMem
*/
void GUI_GetRectMem(GUI_RECT *pSrcRect, void *pBuffer, U32 bufferXSize, U32 bufferYSize, U32 rectX0, U32 rectY0)
{
    GUI_LOCK();

    if (pSrcRect->x0 < GUI_Context.ClipRect[GUI_Context.SelLayer].x0)
    {
        // clip rect
        pSrcRect->x0 = GUI_Context.ClipRect[GUI_Context.SelLayer].x0;
    }

    if (pSrcRect->x1 > GUI_Context.ClipRect[GUI_Context.SelLayer].x1)
    {
        pSrcRect->x1 = GUI_Context.ClipRect[GUI_Context.SelLayer].x1;
    }

    if (pSrcRect->y0 < GUI_Context.ClipRect[GUI_Context.SelLayer].y0)
    {
        pSrcRect->y0 = GUI_Context.ClipRect[GUI_Context.SelLayer].y0;
    }

    if (pSrcRect->y1 > GUI_Context.ClipRect[GUI_Context.SelLayer].y1)
    {
        pSrcRect->y1 = GUI_Context.ClipRect[GUI_Context.SelLayer].y1;
    }

    if (GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_COLOR_ARGB8888
        || GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_COLOR_RGB0888)
    {
        if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_NORMAL)
        {
            int  i = 0;
            int  ySize = pSrcRect->y1 - pSrcRect->y0 + 1;
            int  xSize = pSrcRect->x1 - pSrcRect->x0 + 1;
            U32 *pSrcBuffer  = (U32 *)((U32 *)orange.fbaddr[GUI_Context.SelLayer] + pSrcRect->y0 * (Screen_Context.LCDXSize[GUI_Context.SelLayer]) + pSrcRect->x0);
            U32 *pDestBuffer = (U32 *)((U32 *)pBuffer + rectY0 * bufferXSize + rectX0);

            for (i = 0; i < ySize; i++)
            {
                memcpy(pDestBuffer, pSrcBuffer, xSize << 2);
                pDestBuffer =  pDestBuffer + bufferXSize;
                pSrcBuffer  =  pSrcBuffer + Screen_Context.LCDXSize[GUI_Context.SelLayer] ;
            }
        }
        else
        {
            int  i = 0;
            int  j = 0;
            int  ySize = pSrcRect->y1 - pSrcRect->y0 + 1;
            int  xSize = pSrcRect->x1 - pSrcRect->x0 + 1;
            //U32 *pSrcBuffer  =  (U32*)((U32 *)lemon.fbaddr[GUI_Context.SelLayer] + pSrcRect->y0 *(Screen_Context.LCDXSize[GUI_Context.SelLayer]) + pSrcRect->x0);
            U32 *pDestBuffer = (U32 *)((U32 *)pBuffer + rectY0 * bufferXSize + rectX0);

            for (i = 0; i < ySize; i++)
            {
                for (j = 0; j < xSize; j++)
                {
                    *(pDestBuffer + j) = LCDDEV_L0_GetPixelIndex(pSrcRect->x0 + j, pSrcRect->y0 + i);
                }

                pDestBuffer =  pDestBuffer + bufferXSize;
            }
        }
    }
    else
    {
        if (GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_MONO_8BPP)
        {
            if (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_NORMAL)
            {
                int  i = 0;
                int  ySize = pSrcRect->y1 - pSrcRect->y0 + 1;
                int  xSize = pSrcRect->x1 - pSrcRect->x0 + 1;
                U8 *pSrcBuffer  = (U8 *)((U8 *)orange.fbaddr[GUI_Context.SelLayer] + pSrcRect->y0 * (Screen_Context.LCDXSize[GUI_Context.SelLayer]) + pSrcRect->x0);
                U8 *pDestBuffer = (U8 *)((U8 *)pBuffer + rectY0 * bufferXSize + rectX0);

                for (i = 0; i < ySize; i++)
                {
                    memcpy(pDestBuffer, pSrcBuffer, xSize);
                    pDestBuffer =  pDestBuffer + bufferXSize;
                    pSrcBuffer  =  pSrcBuffer + Screen_Context.LCDXSize[GUI_Context.SelLayer] ;
                }
            }
            else
            {
                // set pixel
                int  i = 0;
                int  j;
                int  ySize = pSrcRect->y1 - pSrcRect->y0 + 1;
                int  xSize = pSrcRect->x1 - pSrcRect->x0 + 1;
                //U8   *pSrcBuffer  =  (U8*)((U8 *)orange.fbaddr[GUI_Context.SelLayer] + pSrcRect->y0 *(Screen_Context.LCDXSize[GUI_Context.SelLayer]) + pSrcRect->x0);
                U8   *pDestBuffer = (U8 *)((U8 *)pBuffer + rectY0 * bufferXSize + rectX0);

                for (i = 0; i < ySize; i++)
                {
                    for (j = 0; j < xSize; j++)
                    {
                        *(pDestBuffer + j) = LCDDEV_L0_GetPixelIndex(pSrcRect->x0 + j, pSrcRect->y0 + i);
                    }

                    pDestBuffer =  pDestBuffer + bufferXSize;
                }
            }
        }
    }

    GUI_UNLOCK();
}


void GUI_SetRectMemEx(GUI_RECT *pDestRect, void *pBuffer, U32 bufferXSize, U32 bufferYSize, U32 rectX0, U32 rectY0)
{
    GUI_LOCK();

    if (pDestRect->x0 < GUI_Context.ClipRect[GUI_Context.SelLayer].x0)
    {
        pDestRect->x0 = GUI_Context.ClipRect[GUI_Context.SelLayer].x0;
    }

    if (pDestRect->x1 > GUI_Context.ClipRect[GUI_Context.SelLayer].x1)
    {
        pDestRect->x1 = GUI_Context.ClipRect[GUI_Context.SelLayer].x1;
    }

    if (pDestRect->y0 < GUI_Context.ClipRect[GUI_Context.SelLayer].y0)
    {
        pDestRect->y0 = GUI_Context.ClipRect[GUI_Context.SelLayer].y0;
    }

    if (pDestRect->y1 > GUI_Context.ClipRect[GUI_Context.SelLayer].y1)
    {
        pDestRect->y1 = GUI_Context.ClipRect[GUI_Context.SelLayer].y1;
    }

    {
        __u32       bpp;
        void        *srcbuf;
        __u32       srcbuf_xsize;
        __u32       srcbuf_ysize;
        void        *dstbuf;
        __u32       dstbuf_xsize;
        __u32       dstbuf_ysize;
        GUI_RECT    srcrect;
        GUI_RECT    dstrect;
        srcbuf          = pBuffer;
        srcbuf_xsize    = bufferXSize;
        srcbuf_ysize    = bufferYSize;
        dstbuf          = (void *)orange.fbaddr[GUI_Context.SelLayer];
        dstbuf_xsize    = Screen_Context.LCDXSize[GUI_Context.SelLayer];
        dstbuf_ysize    = Screen_Context.LCDYSize[GUI_Context.SelLayer];
        srcrect.x0      = rectX0;
        srcrect.y0      = rectY0;
        srcrect.x1      = rectX0 + pDestRect->x1 - pDestRect->x0;
        srcrect.y1      = rectY0 + pDestRect->y1 - pDestRect->y0;
        dstrect.x0      = pDestRect->x0;
        dstrect.y0      = pDestRect->y0;
        dstrect.x1      = pDestRect->x1;
        dstrect.y1      = pDestRect->y1;

        if (GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_COLOR_ARGB8888
            || GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_COLOR_RGB0888)
        {
            bpp = 32;
        }
        else if (GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_MONO_8BPP)
        {
            bpp = 8;
        }
        else
        {
            bpp = 16;
        }

        GUI_MemCopyEx(bpp, srcbuf, srcbuf_xsize, srcbuf_ysize, dstbuf, dstbuf_xsize, dstbuf_ysize, &srcrect, &dstrect);
    }

    GUI_UNLOCK();
}

/***********************************************************************
*
*       GUI_GetRectMem
*/
void GUI_GetRectMemEx(GUI_RECT *pSrcRect, void *pBuffer, U32 bufferXSize, U32 bufferYSize, U32 rectX0, U32 rectY0)
{
    GUI_LOCK();

    if (pSrcRect->x0 < GUI_Context.ClipRect[GUI_Context.SelLayer].x0)
    {
        // clip rect
        pSrcRect->x0 = GUI_Context.ClipRect[GUI_Context.SelLayer].x0;
    }

    if (pSrcRect->x1 > GUI_Context.ClipRect[GUI_Context.SelLayer].x1)
    {
        pSrcRect->x1 = GUI_Context.ClipRect[GUI_Context.SelLayer].x1;
    }

    if (pSrcRect->y0 < GUI_Context.ClipRect[GUI_Context.SelLayer].y0)
    {
        pSrcRect->y0 = GUI_Context.ClipRect[GUI_Context.SelLayer].y0;
    }

    if (pSrcRect->y1 > GUI_Context.ClipRect[GUI_Context.SelLayer].y1)
    {
        pSrcRect->y1 = GUI_Context.ClipRect[GUI_Context.SelLayer].y1;
    }

    {
        __u32       bpp;
        void        *srcbuf;
        __u32       srcbuf_xsize;
        __u32       srcbuf_ysize;
        void        *dstbuf;
        __u32       dstbuf_xsize;
        __u32       dstbuf_ysize;
        GUI_RECT    srcrect;
        GUI_RECT    dstrect;
        srcbuf          = (void *)orange.fbaddr[GUI_Context.SelLayer];
        srcbuf_xsize    = Screen_Context.LCDXSize[GUI_Context.SelLayer];
        srcbuf_ysize    = Screen_Context.LCDYSize[GUI_Context.SelLayer];
        dstbuf          = pBuffer;
        dstbuf_xsize    = bufferXSize;
        dstbuf_ysize    = bufferYSize;
        srcrect.x0      = pSrcRect->x0;
        srcrect.y0      = pSrcRect->y0;
        srcrect.x1      = pSrcRect->x1;
        srcrect.y1      = pSrcRect->y1;
        dstrect.x0      = rectX0;
        dstrect.y0      = rectY0;
        dstrect.x1      = rectX0 + pSrcRect->x1 - pSrcRect->x0;
        dstrect.y1      = rectY0 + pSrcRect->y1 - pSrcRect->y0;

        if (GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_COLOR_ARGB8888
            || GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_COLOR_RGB0888)
        {
            bpp = 32;
        }
        else if (GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_MONO_8BPP)
        {
            bpp = 8;
        }
        else
        {
            bpp = 16;
        }

        GUI_MemCopyEx(bpp, srcbuf, srcbuf_xsize, srcbuf_ysize, dstbuf, dstbuf_xsize, dstbuf_ysize, &srcrect, &dstrect);
    }

    GUI_UNLOCK();
}



/*************************** End of file ****************************/
