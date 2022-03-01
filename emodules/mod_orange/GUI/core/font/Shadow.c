/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : Shadow.c
* Purpose : shadow for SFT
*
* By      : Zhao Jingman
* Version : v1.0
* Date    : 2009-1-6 11:55:42
**********************************************************************************************************************
*/


/***************************************************************
    字体阴影实现函数shadow，输入为存储文字的buffer，传入的接口
    为：char * in,点阵的长width，点阵的宽height,以及点阵每一行
    的字节数linebytes。

    数据类型：
    U32  :  unsigned int
    U8   :  unsigned char
***************************************************************/

#include "gui_font_private.h"
#define SHADOW_SIZE   2

static void shadow_move(U32 *in, U32 *out, U32 xsize, U32 ysize);
static void shadow_smooth(U32 *in, U32 *out, U32 xsize, U32 ysize);

void shadow(U32 *pBuffer, U32 xsize, U32 ysize)
{
    U32     x;
    U32     y;
    void    *lptemp;
    void    *lptemp1;
    U8      *lpPtr;
    U8      *lpTempPtr;

    if ((lptemp = (void *)esMEMS_Malloc(0, (xsize + SHADOW_SIZE) * 4 * (ysize + SHADOW_SIZE))) == NULL)
    {
        __inf("malloc lptemp fail\n");
    }

    if ((lptemp1 = (void *)esMEMS_Malloc(0, (xsize + SHADOW_SIZE) * 4 * (ysize + SHADOW_SIZE))) == NULL)
    {
        __inf("malloc lptemp1 fail\n");
    }

    for (y = 0; y < ysize + SHADOW_SIZE; y++)
    {
        for (x = 0; x < xsize + SHADOW_SIZE; x++)
        {
            if ((y >= ysize) || (x >= xsize))
            {
                lpTempPtr = (U8 *)lptemp + y * (xsize + SHADOW_SIZE) * 4 + 4 * x;
                *(U32 *)lpTempPtr = 0x00000000;
            }
            else
            {
                lpTempPtr = (U8 *)lptemp + y * (xsize + SHADOW_SIZE) * 4 + 4 * x;
                lpPtr = (U8 *)pBuffer + y * xsize * 4 + 4 * x;
                *(U32 *)lpTempPtr = *(U32 *)lpPtr;
            }
        }
    }

    xsize += SHADOW_SIZE;
    ysize += SHADOW_SIZE;
    //  memcpy((U8 *)pBuffer,(U8 *)lptemp,xsize*ysize*4);
    orange_memcpy((U8 *)lptemp1, (U8 *)lptemp, xsize * ysize * 4);
    shadow_move((U32 *)lptemp, pBuffer, xsize, ysize);
    orange_memcpy((U8 *)lptemp, (U8 *)pBuffer, xsize * ysize * 4);
    orange_memset((U8 *)pBuffer, 0, xsize * ysize * 4);
    shadow_smooth((U32 *)lptemp, pBuffer, xsize, ysize);
    orange_memcpy((U8 *)lptemp, (U8 *)pBuffer, xsize * ysize * 4);
    orange_memset((U8 *)pBuffer, 0, xsize * ysize * 4);
    shadow_smooth((U32 *)lptemp, pBuffer, xsize, ysize);

    //原始图像反色处理
    for (y = 0; y < ysize; y++)
    {
        for (x = 0; x < xsize; x++)
        {
            lpPtr = (U8 *)lptemp1 + y * xsize * 4 + 4 * x;

            if (*(lpPtr + 3) != 0)
            {
                lpTempPtr = (U8 *)pBuffer + y * xsize * 4 + 4 * x;
                *(int *)(lpTempPtr) = *(int *)lpPtr;
                /*
                                *(lpTempPtr) =(alpha0*(*(lpPtr))+(255-alpha0)*(*(lpTempPtr)))>>8;
                                lpTempPtr++;
                                lpPtr++;
                                *(lpTempPtr) =(alpha0*(*(lpPtr))+(255-alpha0)*(*(lpTempPtr)))>>8;
                                lpTempPtr++;
                                lpPtr++;
                                *(lpTempPtr) =(alpha0*(*(lpPtr))+(255-alpha0)*(*(lpTempPtr)))>>8;
                                lpTempPtr++;
                                lpPtr++;
                                *(lpTempPtr) =(alpha0*(*(lpPtr))+(255-alpha0)*(*(lpTempPtr)))>>8;
                */
            }
        }
    }

    esMEMS_Mfree(0, lptemp);
    esMEMS_Mfree(0, lptemp1);
}

static void shadow_move(U32 *in, U32 *out, U32 xsize, U32 ysize)
{
    U8      *src;
    U8      *dest;
    U32     num;
    U32     i;
    U32     j;
    U32     step;
    step = 2;

    for (i = 0; i < ysize; i++)
    {
        for (j = 0; j < xsize; j++)
        {
            dest = (U8 *) out + xsize * 4 * i + 4 * j;
            *(U32 *)dest = 0x00000000;
        }
    }

    for (i = 0; i < ysize; i++)  // 行(除去边缘几行)
    {
        for (j = 0; j < xsize ; j++)// 列(除去边缘几列)
        {
            // 指向DIB第i行，第j 个象素的指针
            src = (U8 *)in + xsize * 4 * (i) + 4 * j ;
            num = *(src + 3);

            if (num != 0)
            {
                // 指向新DIB第i行，第j个象素的指针
                dest = (U8 *) out + xsize * 4 * ((i + (step + 1))) + 4 * (j + step);
                *(unsigned char *)(dest + 3) = (unsigned char) num;
            }
        }
    }
}

static void shadow_smooth(U32 *in, U32 *out, U32 xsize, U32 ysize)
{
    U8      *src;
    U8      *dest;
    U32     Result;
    U32     Tmp;
    U32     i;
    U32     j;
    U32     k;
    U32     l;

    /*  for(i = 1; i < ysize - 4 + 2 + 1; i++)  // 行(除去边缘几行)
        {
    //第一个数据点的处理
            Result=0;
            j = 1;
            // 指向新DIB第i行，第j个象素的指针
            dest = (U8 *)out + xsize*4*i + 4 * j;
            for (k = 0; k < 4 ; k++)
            {
                for (l = 0; l < 4 ; l++)
                {
                    // 指向DIB第i - iTempMY + k行，第j - iTempMX + l个象素的指针
                    src = (U8 *)in + xsize*4*(  i - 2 + k)+4*(j-2+l);
                    // 保存象素值
                    Result += (* (src+3)) ;
                }
            }

            Tmp = Result/20;                    // 乘上系数
            if(Tmp > 255)
            {
                * (dest+3) = (U8 )255;
            }

            else
            {
                * (dest+3) = (U8) (Tmp);
            }

    //之后的数据点的处理，利用模板平移，简化运算
            for(j=j+1; j < xsize - 4 + 2 + 1; j++)// 列(除去边缘几列)
            {
                // 指向新DIB第i行，第j个象素的指针
                dest = (U8 *)out  + xsize*4*i +4 * j;

                // 指向源DIB第 i - 2 行，第j-3 个象素的指针
                src = (U8 *)in + xsize*4*( i - 2 )+4*(j-3);
                Result -= *(src+3) ;
                // 指向源DIB第 i -1 行，第j-3 个象素的指针
                src += xsize*4;
                Result -= *(src+3) ;
                // 指向源DIB第 i   行，第j-3 个象素的指针
                src += xsize*4;
                Result -= *(src+3) ;
                // 指向源DIB第 i + 1   行，第j-3 个象素的指针
                src += xsize*4;
                Result -= *(src+3) ;
                // 指向源DIB第 i + 1  行，第j + 1 个象素的指针
                src += 4*4;
                Result += *(src+3) ;
                // 指向源DIB第 i  行，第j + 1 个象素的指针
                src -= xsize*4;
                Result += *(src+3) ;
                // 指向源DIB第 i - 1 行，第j + 1 个象素的指针
                src -= xsize*4;
                Result += *(src+3) ;
                // 指向源DIB第 i - 2 行，第j + 1 个象素的指针
                src -= xsize*4;
                Result += *(src+3) ;

                Tmp = Result/20;                    // 乘上系数
                if(Tmp > 255)
                {
                    * (dest+3)= (U8 )255;
                }
                else
                {
                    * (dest+3)= (U8) Tmp;
                }

            }
        }*/

    for (i = 2; i < (ysize - 4 + 2 + 1); i++)    // 行(除去边缘几行)
    {
        for (j = 2; j < (xsize - 4 + 2 + 1); j++)
        {
            Result = 0;
            // 指向新DIB第i行，第j个象素的指针
            dest = (unsigned char *)out +  xsize * 4 * (i) + 4 * j;

            for (k = 0; k < 4; k++)
            {
                for (l = 0; l < 4; l++)
                {
                    // 指向DIB第i - iTempMY + k行，第j - iTempMX + l个象素的指针
                    src = (unsigned  char *)in + xsize * 4 * (
                              i - 2 + k) + 4 * (j - 2 + l);
                    // 保存象素值
                    Result += (* (unsigned char *)(src + 3));
                }
            }

            Tmp = Result / 20;                  // 乘上系数

            if (Tmp > 255)
            {
                * (unsigned char *)(dest + 3) = (unsigned char)255;
            }
            else
            {
                *(unsigned char *)(dest + 3) = (unsigned char)(Tmp);
            }
        }
    }
}
