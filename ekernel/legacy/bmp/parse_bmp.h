/*
*************************************************************************************
*                                   eGon
*                           Application Of eGon2.0
*
*               (c) Copyright 2006-2010, All winners Co,Ld.
*                           All Rights Reserved
*
* File Name     : Parse_Picture.h
*
* Author        : javen
*
* Description   : 图片解析
*
* History       :
*      <author>         <time>          <version >          <desc>
*       javen          2010-09-10          1.0            create this file
*
*************************************************************************************
*/
#ifndef  __PARSE_PICTURE_H__
#define  __PARSE_PICTURE_H__
#include <typedef.h>

typedef struct tag_Picture
{
    void        *Buffer;           /* 存放图片数据     */
    uint32_t    BufferSize;    /* buffer长度       */

    uint32_t    BitCount;      /* 一个像素的bit数  */
    uint32_t    Width;         /* 图片宽度         */
    uint32_t    Height;        /* 图片高度         */
    uint32_t    RowSize;       /* 图片一行的大小   */
} Picture_t;

int32_t Parse_Pic_BMP_ByBuffer(void *Pic_Buffer, uint32_t Pic_BufferSize, Picture_t *PictureInfo);
int32_t Parse_Pic_BMP_ByPath(char *Path, Picture_t *PictureInfo, void *Addr);
int32_t Parse_Pic_BMP_ByRam(const uint32_t base, const uint32_t size, Picture_t *PictureInfo, void *Addr);

#endif   //__PARSE_PICTURE_H__

