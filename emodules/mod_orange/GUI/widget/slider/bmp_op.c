/*
************************************************************************************************************************
*                                                        BITMAP
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : bmp_op.c
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2009.07.20
*
* Description :
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.07.20       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __bmp_op_c
#define  __bmp_op_c


#include "../winclass_i.h"
#include "ctrl_redef.h"
#include "bmp_op.h"
#include <log.h>

typedef struct _bmp_file_head_t
{
    char    bfType[2];        // "BM"
    __u32   bfSize;           // total size of bmp file
    __u32   bfReserved;       // reserved field
    __u32   bfOffBits;        // pixel matrix offset from file head
} __attribute__((packed)) bmp_file_head_t;

typedef struct _bmp_info_head_t
{
    __u32   biSize;           // the size of bmp information head
    __u32   biWidth;          // the width of bmp, unit: pixel
    __s32  biHeight;         // the height of bmp, unit:pixel
    __u16   biPlanes;         // always 1
    __u16   biBitCount;       // bits per pixel. 1-mono, 4-16color, 8-256color, 24-truecolor
    __u32   biCompression;    // copression format. 0: no compression.
    __u32   biSizeImage;      // the size of pixel matirx.
    __u32   biXPelsPerMeter;  // horizontal dis, unit : pixel/meter
    __u32   biYPelsPerMeter;  // vertical dis, unit : pixel/meter
    __u32   biClrUsed;        // number of used colors, 0 means 2^biBitCount
    __u32   biClrImportant;   // number of important colors, 0 means that all colors are important.
} __attribute__((packed)) bmp_info_head_t;

typedef struct _HBMP_i_t
{
    __u32   bitcount;
    __u32   width;
    __s32   height;
    __u32   file_size;
} HBMP_i_t, *HBMP_i;


/**************************************************************************************************/
/*                                         function list                                          */
/**************************************************************************************************/

HBMP  bmp_open(const void *bmp_buf)
{
    bmp_file_head_t  file_head;
    bmp_info_head_t  info_head;
    HBMP_i  hbmp_i = NULL;

    if (bmp_buf == NULL)
    {
        __err("buf is null.");
        return 0;
    }

    hbmp_i = (HBMP_i)g_malloc(sizeof(HBMP_i_t));

    if (hbmp_i == NULL)
    {
        return 0;
    }

    g_memset(hbmp_i, 0, sizeof(HBMP_i_t));
    /* get bmp file head */
    g_memcpy(&file_head, bmp_buf, sizeof(file_head));

    if (file_head.bfType[0] != 'B' || file_head.bfType[1] != 'M')
    {
        goto error;
    }

    /* get bmp information head */
    g_memcpy(&info_head, (__u8 *)bmp_buf + sizeof(file_head), sizeof(info_head));

    if (info_head.biBitCount != 32 && info_head.biBitCount != 8)
    {
        goto error;
    }

    hbmp_i->width     = info_head.biWidth ;
    hbmp_i->height    = info_head.biHeight;
    hbmp_i->bitcount  = info_head.biBitCount;
    hbmp_i->file_size = file_head.bfSize;
    return (HBMP)hbmp_i;
error:

    if (hbmp_i != NULL)
    {
        g_free(hbmp_i);
        hbmp_i = NULL;
    }

    return 0;
}
__s32 bmp_close(HBMP hbmp)
{
    HBMP_i hbmp_i = (HBMP_i)hbmp;

    if (hbmp_i != NULL)
    {
        g_free(hbmp_i);
        hbmp_i = NULL;
    }

    return 0;
}

__u32 bmp_get_bitcount(HBMP hbmp)
{
    HBMP_i hbmp_i = (HBMP_i)hbmp;
    return (hbmp_i->bitcount);
}

__u32 bmp_get_width(HBMP hbmp)
{
    HBMP_i hbmp_i = (HBMP_i)hbmp;
    return (hbmp_i->width);
}

__u32 bmp_get_height(HBMP hbmp)
{
    HBMP_i hbmp_i = (HBMP_i)hbmp;
    return ABS(hbmp_i->height);
}

__u32 bmp_get_size(HBMP hbmp)
{
    HBMP_i hbmp_i = (HBMP_i)hbmp;
    return (hbmp_i->file_size);
}

void bmp_get_matrix(const void *bmp_buf, void *matrix)
{
    __u32   bitcount;
    __u32   byte_count;
    __u32   row_size;
    __u32   real_row_size;
    __u32   width;
    __s32   height;
    __u32   abs_height;
    __u32   matrix_off;
    __u32   i;
    __u8   *q;
    __u8   *p;
    bmp_info_head_t  info_head;
    bmp_file_head_t  file_head;
    g_memcpy(&file_head, bmp_buf, sizeof(bmp_file_head_t));
    g_memcpy(&info_head, (__u8 *)bmp_buf + sizeof(bmp_file_head_t), sizeof(bmp_info_head_t));
    bitcount   = info_head.biBitCount;
    byte_count = bitcount >> 3;
    width     = info_head.biWidth ;
    height    = info_head.biHeight;
    row_size  = spcl_size_align(width * byte_count, 4);
    abs_height = ABS(height);
    real_row_size = width * byte_count;
    matrix_off = file_head.bfOffBits;

    if (height < 0)
    {
        p = (__u8 *)bmp_buf + matrix_off;
        q = (__u8 *)matrix;

        for (i = 0;  i < abs_height;  i++)
        {
            g_memcpy(q, p, real_row_size);
            p += row_size;
            q += real_row_size;
        }
    }
    else
    {
        p = (__u8 *)bmp_buf + matrix_off;
        q = (__u8 *)matrix + real_row_size * (height - 1);

        for (i = 0;  i < abs_height;  i++)
        {
            g_memcpy(q, p, real_row_size);
            p += row_size;
            q -= real_row_size;
        }
    }
}
#endif     //  ifndef __bmp_op_c
/* end of bmp_op.c  */
