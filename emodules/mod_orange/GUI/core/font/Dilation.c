/****************************************************

该函数实现给文字加边框的效果by JM……

****************************************************/

#include "gui_font_private.h"

static  U8 FRAME_A = 0xcf;
static  U32 FRAME_A_32BPP = 0x0;


U8 GUI_SetFrameColor8bpp32(U8 frameColor)
{
    U8 oldFrameColor;
    oldFrameColor = FRAME_A;
    FRAME_A = frameColor;
    return oldFrameColor;
}

U32 GUI_SetFrameColor32bpp(U32 frameColor)
{
    U32 oldFrameColor;
    oldFrameColor = FRAME_A_32BPP;
    FRAME_A_32BPP = frameColor;
    return oldFrameColor;
}




void shadow_Dilation(unsigned char *in, unsigned char *out, unsigned int xsize, unsigned int ysize)
{
    unsigned char   *src;
    unsigned char   *dest;
    unsigned int    num;
    unsigned int    i, j;
    unsigned char   *ptemp;
    xsize += 4;
    ysize += 4;
    ptemp = (unsigned char *)esMEMS_Malloc(0, (xsize) * (ysize));
    orange_memset(out, 0x0, xsize * ysize);

    for (i = 2; i < ysize - 2; i++)
    {
        eLIBs_memcpy(out + i * xsize + 2, in + (i - 2) * (xsize - 4), xsize - 4);
    }

    eLIBs_memcpy(ptemp, out, xsize * ysize);

    for (i = 1; i < ysize - 3 + 1 + 1; i++) // 行(除去边缘几行)
    {
        for (j = 1; j < xsize - 3 + 1 + 1; j++) // 列(除去边缘几列)
        {
            dest = out + xsize * (i) + j;
            src = ptemp + xsize * (i) + (j);
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = num;
                continue;
            }

            src = src - xsize - 1;
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = FRAME_A;
                continue;
            }

            src = src + 1;
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = FRAME_A;
                continue;
            }

            src = src + 1;
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = FRAME_A;
                continue;
            }

            src = src + xsize;
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = FRAME_A;
                continue;
            }

            src = src - 2;
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = FRAME_A;
                continue;
            }

            src = src + xsize ;
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = FRAME_A;
                continue;
            }

            src = src + 1;
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = FRAME_A;
                continue;
            }

            src = src + 1;
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = FRAME_A;
            }
        }
    }

    //eLIBs_memcpy(in, out, xsize*ysize);
    esMEMS_Mfree(0, ptemp);
}


