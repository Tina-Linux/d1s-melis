/*
************************************************************************************************************************
*                                                        BITMAP
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : bmp.c
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2009.04.07
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
* Gary.Wang      2009.04.07       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __bmp_c
#define  __bmp_c
#include "bmp.h"
#include <string.h>
#include <stdio.h>
#include <log.h>

#define abs(x)          ((x) >= 0 ? (x):-(x))
#define PALETTE_OFF     ( sizeof(bmp_file_head_t) + sizeof(bmp_info_head_t) )

__bool      AZ100_IsCompress(void *pBuffer, uint32_t Size);
int32_t     AZ100_Uncompress(void *pSource, uint32_t SourceSize, void *pDest, uint32_t DestSize);
uint32_t    AZ100_GetUncompressSize(void *pBuffer, uint32_t Size);

static char     *g_pbmp_buf = NULL;
static int32_t  g_bmp_size = 0;

HBMP open_bmp(const char *bmp_file, HBMP_i_t *hbmp_hdl_buf)
{
    FILE            *fp = NULL;
    bmp_file_head_t file_head;
    bmp_info_head_t info_head;
    HBMP_i          hdl = NULL;
    uint32_t        byte_count;
    char            *pbuf;
    uint32_t        file_len;
    uint32_t        read_len;
    char            *uncompress_buf;
    uint32_t        uncompress_len;

    hdl             = NULL;
    pbuf            = NULL;
    file_len        = 0;
    uncompress_buf  = NULL;
    uncompress_len  = 0;

    if (bmp_file == NULL)
    {
        return NULL;
    }

    hdl = (HBMP_i)hbmp_hdl_buf;
    if (hdl == NULL)
    {
        goto error;
    }

    fp = fopen(bmp_file, "rb");
    if (fp == NULL)
    {
        __err("Error in open bmp file %s.", bmp_file);
        hdl = NULL;
        goto error;
    }

    fseek(fp, 0, SEEK_END);
    file_len = ftell(fp);
    __log("file_len=%d", file_len);

    pbuf = (char *)rt_malloc(file_len);
    if (!pbuf)
    {
        __err("palloc fail...");
        hdl = NULL;
        goto error;
    }
    fseek(fp, 0, SEEK_SET);
    read_len = fread(pbuf, 1, file_len, fp);
    if (read_len != file_len)
    {
        __err("fread fail...");
        hdl = NULL;
        goto error;
    }

    //Î´Ñ¹ËõµÄ±³¾°
    if (0 == AZ100_IsCompress(pbuf, file_len))
    {
        __log("bg pic is uncompress...");
        uncompress_buf = pbuf;
        uncompress_len = file_len;
        pbuf = NULL;
        file_len = 0;
        __log("uncompress_buf=%x", uncompress_buf);
    }
    else//´øÑ¹ËõµÄ±³¾°
    {
        int32_t ret_val;

        __log("bg pic is compress...");
        uncompress_len = AZ100_GetUncompressSize(pbuf, file_len);

        uncompress_buf = (char *)rt_malloc(uncompress_len);
        __log("esMEMS_Palloc: uncompress_buf=%x", uncompress_buf);

        if (!uncompress_buf)
        {
            __log("palloc fail...");
            hdl = NULL;
            goto error;
        }

        ret_val = AZ100_Uncompress(pbuf, file_len, uncompress_buf, uncompress_len);
        if (-1 == ret_val)
        {
            __log("AZ100_Uncompress fail...");
            hdl = NULL;
            goto error;
        }
        rt_free(pbuf);
        __log("esMEMS_Pfree: pbuf=%x, file_len=%d", pbuf, file_len);
        pbuf = NULL;
        file_len = 0;
    }

    g_pbmp_buf = uncompress_buf;
    g_bmp_size = uncompress_len;

    uncompress_buf = NULL;
    uncompress_len = 0;

    /* get bmp file head */
    memcpy(&file_head, g_pbmp_buf, sizeof(file_head));
    if (file_head.bfType[0] != 'B' || file_head.bfType[1] != 'M')
    {
        hdl = NULL;
        goto error;
    }

    /* get bmp information head */
    memcpy(&info_head, (char *)g_pbmp_buf + sizeof(file_head), sizeof(info_head));
    if (info_head.biBitCount != 32)
    {
        __log("Error! bitcount is %d, not 32.", info_head.biBitCount);
        hdl = NULL;
        goto error;
    }

    hdl->width  = info_head.biWidth ;
    hdl->height = info_head.biHeight;
    hdl->bitcount = info_head.biBitCount;
    byte_count = hdl->bitcount >> 3;
    hdl->byte_count = byte_count;
    hdl->row_size = (hdl->width * byte_count + 4 - 1) & (~(4 - 1));
    hdl->matrix_off = file_head.bfOffBits;

error:
    if (fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }

    if (pbuf)
    {
        rt_free(pbuf);
        pbuf = NULL;
    }

    if (uncompress_buf)
    {
        rt_free(uncompress_buf);
        uncompress_buf = NULL;
    }

    return (HBMP)hdl;
}

#if defined CONFIG_BOOT_LOGO_BMP
HBMP open_ram_bmp(const unsigned long base, const uint32_t size, HBMP_i_t *hbmp_hdl_buf)
{
    FILE            *fp = NULL;
    bmp_file_head_t file_head;
    bmp_info_head_t info_head;
    HBMP_i          hdl = NULL;
    uint32_t        byte_count;
    char            *pbuf;
    uint32_t        file_len;
    uint32_t        read_len;
    char            *uncompress_buf;
    uint32_t        uncompress_len;

    hdl             = NULL;
    pbuf            = NULL;
    file_len        = 0;
    uncompress_buf  = NULL;
    uncompress_len  = 0;

    hdl = (HBMP_i)hbmp_hdl_buf;
    if (hdl == NULL)
    {
        goto error;
    }
    file_len = size;
    pbuf = (char *)base;

    if (0 == AZ100_IsCompress(pbuf, file_len))
    {
        __log("bg pic is uncompress...");
        uncompress_buf = pbuf;
        uncompress_len = file_len;
        pbuf = NULL;
        file_len = 0;
    }
    else//´øÑ¹ËõµÄ±³¾°
    {
        int32_t ret_val;

        __log("bg pic is compress...");
        uncompress_len  = AZ100_GetUncompressSize(pbuf, file_len);

        uncompress_buf  = (char *)rt_malloc(uncompress_len);

        if (!uncompress_buf)
        {
            __log("palloc fail...");
            hdl = NULL;
            goto error;
        }

        ret_val = AZ100_Uncompress(pbuf, file_len, uncompress_buf, uncompress_len);
        if (-1 == ret_val)
        {
            __log("AZ100_Uncompress fail...");
            hdl = NULL;
            goto error;
        }
        rt_free(pbuf);
        pbuf = NULL;
    }

    g_pbmp_buf = uncompress_buf;
    g_bmp_size = uncompress_len;

    uncompress_buf = NULL;
    uncompress_len = 0;

    /* get bmp file head */
    memcpy(&file_head, g_pbmp_buf, sizeof(file_head));
    if (file_head.bfType[0] != 'B' || file_head.bfType[1] != 'M')
    {
        hdl = NULL;
        goto error;
    }

    /* get bmp information head */
    memcpy(&info_head, (char *)g_pbmp_buf + sizeof(file_head), sizeof(info_head));
    if (info_head.biBitCount != 32)
    {
        __log("Error! bitcount is %d, not 32.", info_head.biBitCount);
        hdl = NULL;
        goto error;
    }

    hdl->width      = info_head.biWidth ;
    hdl->height     = info_head.biHeight;
    hdl->bitcount   = info_head.biBitCount;
    byte_count      = hdl->bitcount >> 3;
    hdl->byte_count = byte_count;
    hdl->row_size   = (hdl->width * byte_count + 4 - 1) & (~(4 - 1));
    hdl->matrix_off = file_head.bfOffBits;

error:
    if (fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }

    if (pbuf)
    {
        rt_free(pbuf);
        pbuf = NULL;
    }

    if (uncompress_buf)
    {
        rt_free(uncompress_buf);
        uncompress_buf = NULL;
    }

    return (HBMP)hdl;
}
#endif

int32_t close_bmp(HBMP hbmp)
{
    HBMP_i hdl = (HBMP_i)hbmp;

    if (hdl == NULL)
    {
        return -1;
    }

    if (g_pbmp_buf)
    {
        __log("esMEMS_Pfree: g_pbmp_buf=%x, g_bmp_size=%d", g_pbmp_buf, g_bmp_size);
        rt_free(g_pbmp_buf);
        g_pbmp_buf = NULL;
        g_bmp_size = 0;
    }

    memset(hdl, 0, sizeof(HBMP_i_t));

    return 0;
}

int32_t read_pixel(HBMP hbmp, uint32_t x, uint32_t y, full_color_t *color_p, int32_t origin_pos)
{
    uint32_t    offset;
    uint32_t    byte_count;
    HBMP_i      hdl = (HBMP_i)hbmp;
    uint32_t    abs_height = abs(hdl->height);

    if (hbmp == NULL)
    {
        return -1;
    }

    if (x >= hdl->width || y >= abs_height)
    {
        return -1;
    }

    byte_count = hdl->byte_count;
    if (hdl->height > 0)
    {
        if (origin_pos == ORIGIN_POS_UPPER_LEFT)
        {
            offset = hdl->matrix_off + (abs_height - 1 - y) * hdl->row_size + byte_count * x;
        }
        else if (origin_pos == ORIGIN_POS_LOWER_LEFT)
        {
            offset = hdl->matrix_off + y * hdl->row_size + byte_count * x;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        if (origin_pos == ORIGIN_POS_UPPER_LEFT)
        {
            offset = hdl->matrix_off + y * hdl->row_size + byte_count * x;
        }
        else if (origin_pos == ORIGIN_POS_LOWER_LEFT)
        {
            offset = hdl->matrix_off + (abs_height - 1 - y) * hdl->row_size + byte_count * x;
        }
        else
        {
            return -1;
        }
    }

    if (g_pbmp_buf)
    {
        memcpy(color_p, (char *)g_pbmp_buf + offset, byte_count);
    }

    return 0;
}



int32_t  get_matrix(HBMP hbmp, void *buf)
{
    HBMP_i hdl  = (HBMP_i)hbmp;
    uint32_t    row_size;
    uint32_t    real_row_size;
    uint32_t    height;
    uint32_t    i;
    uint8_t     *q;
    int32_t     pad_count;

    if (hbmp == NULL || buf == NULL || g_pbmp_buf == NULL)
    {
        return -1;
    }

    row_size        = hdl->row_size;
    real_row_size   = hdl->width * hdl->byte_count;
    height          = abs(hdl->height);
    pad_count       = row_size - real_row_size;
    if (hdl->height < 0)
    {
        uint8_t *pSrc   = (uint8_t *)g_pbmp_buf + hdl->matrix_off;

        q = (uint8_t *)buf;
        for (i = 0;  i < height;  i++)
        {
            if (g_pbmp_buf)
            {
                memcpy(q, pSrc, real_row_size);
                pSrc += real_row_size;
            }

            pSrc += pad_count;
            q += real_row_size;
        }
    }
    else
    {
        uint8_t *pSrc = (uint8_t *)g_pbmp_buf + hdl->matrix_off;

        q = (uint8_t *)buf + real_row_size * (height - 1);
        for (i = 0;  i < height;  i++)
        {
            if (g_pbmp_buf)
            {
                memcpy(q, pSrc, real_row_size);
                pSrc += real_row_size;
            }

            pSrc += pad_count;
            q -= real_row_size;
        }
    }
    return 0;

}




uint32_t get_bitcount(HBMP hbmp)
{
    HBMP_i hdl = (HBMP_i)hbmp;

    return (hdl->bitcount);
}



uint32_t get_width(HBMP hbmp)
{
    HBMP_i hdl = (HBMP_i)hbmp;

    return (hdl->width);
}



int32_t get_height(HBMP hbmp)
{
    HBMP_i hdl = (HBMP_i)hbmp;

    return abs(hdl->height);
}



uint32_t get_rowsize(HBMP hbmp)
{
    HBMP_i hdl = (HBMP_i)hbmp;

    return (hdl->row_size);
}
#endif     //  ifndef __bmp_c
