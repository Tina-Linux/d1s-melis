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
#ifndef  __bmp_c
#define  __bmp_c

//#include "Esh_shell_private.h"
#include <typedef.h>
#include <libc.h>
#include "bmp.h"
#include "libc/eLIBs_az100.h"


#define abs(x) (x) >= 0 ? (x):-(x)

#define PALETTE_OFF           ( sizeof(bmp_file_head_t) + sizeof(bmp_info_head_t) )

static char *g_pbmp_buf = NULL;
static __s32 g_bmp_size = 0;

HBMP  open_bmp(const char *bmp_file, HBMP_i_t *hbmp_hdl_buf)
{
    void *fp = NULL;
    bmp_file_head_t  file_head;
    bmp_info_head_t  info_head;
    HBMP_i hdl = NULL;
    __u32  byte_count;
    char *pbuf;
    __u32 file_len;
    __u32 read_len;
    char *uncompress_buf;
    __u32 uncompress_len;
    hdl = NULL;
    pbuf = NULL;
    file_len = 0;
    uncompress_buf = NULL;
    uncompress_len = 0;

    if (bmp_file == NULL)
    {
        return 0;
    }

    hdl = (HBMP_i)hbmp_hdl_buf;

    if (hdl == NULL)
    {
        goto error;
    }

    fp = eLIBs_fopen(bmp_file, "rb");

    if (fp == NULL)
    {
        __wrn("Error in open bmp file %s.", bmp_file);
        hdl = NULL;
        goto error;
    }

    eLIBs_fseek(fp, 0, SEEK_END);
    file_len = eLIBs_ftell(fp);
    __msg("file_len=%d", file_len);
    pbuf = esMEMS_Palloc((file_len + 1023) / 1024, 0);
    __msg("esMEMS_Palloc: pbuf=%x", pbuf);

    if (!pbuf)
    {
        __msg("palloc fail...");
        hdl = NULL;
        goto error;
    }

    eLIBs_fseek(fp, 0, SEEK_SET);
    read_len = eLIBs_fread(pbuf, 1, file_len, fp);

    if (read_len != file_len)
    {
        __msg("fread fail...");
        hdl = NULL;
        goto error;
    }

    //未压缩的背景
    if (EPDK_FALSE == AZ100_IsCompress(pbuf, file_len))
    {
        __msg("bg pic is uncompress...");
        uncompress_buf = pbuf;
        uncompress_len = file_len;
        pbuf = NULL;
        file_len = 0;
        __msg("uncompress_buf=%x", uncompress_buf);
    }
    else//带压缩的背景
    {
        __s32 ret_val;
        __msg("bg pic is compress...");
        uncompress_len = AZ100_GetUncompressSize(pbuf, file_len);
        uncompress_buf = esMEMS_Palloc((uncompress_len + 1023) / 1024, 0);
        __msg("esMEMS_Palloc: uncompress_buf=%x", uncompress_buf);

        if (!uncompress_buf)
        {
            __msg("palloc fail...");
            hdl = NULL;
            goto error;
        }

        ret_val = AZ100_Uncompress(pbuf, file_len, uncompress_buf, uncompress_len);

        if (EPDK_FAIL == ret_val)
        {
            __msg("AZ100_Uncompress fail...");
            hdl = NULL;
            goto error;
        }

        //__here__;
        esMEMS_Pfree(pbuf, (file_len + 1023) / 1024);
        __msg("esMEMS_Pfree: pbuf=%x, file_len=%d", pbuf, file_len);
        pbuf = NULL;
        file_len = 0;
        //__here__;
    }

    g_pbmp_buf = uncompress_buf;
    g_bmp_size = uncompress_len;
    uncompress_buf = NULL;
    uncompress_len = 0;
    /* get bmp file head */
    eLIBs_memcpy(&file_head, g_pbmp_buf, sizeof(file_head));

    if (file_head.bfType[0] != 'B' || file_head.bfType[1] != 'M')
    {
        hdl = NULL;
        goto error;
    }

    /* get bmp information head */
    eLIBs_memcpy(&info_head, (char *)g_pbmp_buf + sizeof(file_head), sizeof(info_head));

    if (info_head.biBitCount != 32
        && info_head.biBitCount != 16
        && info_head.biBitCount != 8
        && info_head.biBitCount != 4
        && info_head.biBitCount != 1)
    {
        __wrn("Error! bitcount is %d, not supported.", info_head.biBitCount);
        hdl = NULL;
        goto error;
    }

    hdl->width  = info_head.biWidth ;
    hdl->height = info_head.biHeight;
    hdl->bitcount = info_head.biBitCount;
    //bit per line aligned by 8
    hdl->row_size = spcl_size_align(hdl->width * hdl->bitcount, 8);
    //byte per line
    hdl->row_size >>= 3;
    hdl->real_row_size = hdl->row_size;
    //byte per line aligned by 4
    hdl->row_size = spcl_size_align(hdl->row_size, 4);
    hdl->matrix_off = file_head.bfOffBits;
error:

    if (fp != NULL)
    {
        eLIBs_fclose(fp);
        fp = NULL;
    }

    if (pbuf)
    {
        esMEMS_Pfree(pbuf, (file_len + 1023) / 1024);
        pbuf = NULL;
    }

    if (uncompress_buf)
    {
        esMEMS_Pfree(uncompress_buf, (uncompress_len + 1023) / 1024);
        uncompress_buf = NULL;
    }

    return (HBMP)hdl;
}



__s32 close_bmp(HBMP hbmp)
{
    HBMP_i hdl = (HBMP_i)hbmp;

    if (hdl == NULL)
    {
        return -1;
    }

    if (g_pbmp_buf)
    {
        __wrn("esMEMS_Pfree: g_pbmp_buf=%x, g_bmp_size=%d ,Psize = %d", g_pbmp_buf, g_bmp_size,(g_bmp_size + 1023) / 1024);
        //esMEMS_Pfree(g_pbmp_buf, (g_bmp_size + 1023) / 1024);
        eLIBs_free(g_pbmp_buf);
        g_pbmp_buf = NULL;
        g_bmp_size = 0;
    }

    eLIBs_memset(hdl, 0, sizeof(HBMP_i_t));
    return 0;
}

HBMP open_ram_bmp(const void *base, const uint32_t size, HBMP_i_t *hbmp_hdl_buf)
{
    bmp_file_head_t  file_head;
    bmp_info_head_t  info_head;
    HBMP_i hdl = NULL;
    uint32_t  byte_count;
    char *pbuf;
    uint32_t file_len;
    uint32_t read_len;
    char *uncompress_buf;
    uint32_t uncompress_len;

	__wrn("base=0x%x, size=0x%x\n", base, size);

    hdl = NULL;

    pbuf = NULL;
    file_len = 0;
    uncompress_buf = NULL;
    uncompress_len = 0;

    hdl = (HBMP_i)hbmp_hdl_buf;
    if (hdl == NULL)
    {
        goto error;
    }
    file_len = size;
    pbuf = (char *)base;

    if (0 == AZ100_IsCompress(pbuf, file_len))
    {
        __wrn("bg pic is uncompress...file_len = %d",file_len);
        uncompress_buf = pbuf;
        uncompress_len = file_len;
        pbuf = NULL;
        file_len = 0;
    }
    else//带压缩的背景
    {
        int32_t ret_val;

        __wrn("bg pic is compress...");
        uncompress_len = AZ100_GetUncompressSize(pbuf, file_len);

        uncompress_buf = (char *)eLIBs_malloc(uncompress_len);

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
        eLIBs_free(pbuf);
        pbuf = NULL;
    }

    g_pbmp_buf = uncompress_buf;
    g_bmp_size = uncompress_len;

    uncompress_buf = NULL;
    uncompress_len = 0;

    /* get bmp file head */
    eLIBs_memcpy(&file_head, g_pbmp_buf, sizeof(file_head));
    if (file_head.bfType[0] != 'B' || file_head.bfType[1] != 'M')
    {
        hdl = NULL;
        goto error;
    }

    /* get bmp information head */
    eLIBs_memcpy(&info_head, (char *)g_pbmp_buf + sizeof(file_head), sizeof(info_head));
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
    if (pbuf)
    {
        eLIBs_free(pbuf);
        pbuf = NULL;
    }

    if (uncompress_buf)
    {
        eLIBs_free(uncompress_buf);
        uncompress_buf = NULL;
    }

    return (HBMP)hdl;
}

__s32 read_pixel(HBMP hbmp, __u32 x, __u32 y, full_color_t *color_p, __s32 origin_pos)
{
    __u32  offset;
    __u32  byte_count;
    HBMP_i hdl = (HBMP_i)hbmp;
    __u32  abs_height = abs(hdl->height);

    if (hbmp == 0)
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
        eLIBs_memcpy(color_p, (char *)g_pbmp_buf + offset, byte_count);
    }

    return 0;
}



__s32  get_matrix(HBMP hbmp, void *buf)
{
    HBMP_i hdl = (HBMP_i)hbmp;
    __u32   row_size;
    __u32   real_row_size;
    __u32   height;
    __u32   i;
    __u8    *q;
    __s32   pad_count;

    if (hbmp == 0 || buf == NULL || g_pbmp_buf == NULL)
    {
        return -1;
    }

    row_size = hdl->row_size;
    real_row_size = hdl->width * hdl->byte_count;
    height = abs(hdl->height);
    pad_count = row_size - real_row_size;

    if (hdl->height < 0)
    {
        __u8 *pSrc = (__u8 *)g_pbmp_buf + hdl->matrix_off;
        q = (__u8 *)buf;

        for (i = 0;  i < height;  i++)
        {
            if (g_pbmp_buf)
            {
                eLIBs_memcpy(q, pSrc, real_row_size);
                pSrc += real_row_size;
            }

            pSrc += pad_count;
            q += real_row_size;
        }
    }
    else
    {
        __u8 *pSrc = (__u8 *)g_pbmp_buf + hdl->matrix_off;
        q = (__u8 *)buf + real_row_size * (height - 1);

        for (i = 0;  i < height;  i++)
        {
            if (g_pbmp_buf)
            {
                eLIBs_memcpy(q, pSrc, real_row_size);
                pSrc += real_row_size;
            }

            pSrc += pad_count;
            q -= real_row_size;
        }
    }

    return 0;
}
#endif


__u32   get_bitcount(HBMP hbmp)
{
    HBMP_i hdl = (HBMP_i)hbmp;
    return (hdl->bitcount);
}

__u32   get_width(HBMP hbmp)
{
    HBMP_i hdl = (HBMP_i)hbmp;
    return (hdl->width);
}

__s32  get_height(HBMP hbmp)
{
    HBMP_i hdl = (HBMP_i)hbmp;
    return abs(hdl->height);
}

__u32   get_rowsize(HBMP hbmp)
{
    HBMP_i hdl = (HBMP_i)hbmp;
    return (hdl->row_size);
}

/* end of bmp.c  */
