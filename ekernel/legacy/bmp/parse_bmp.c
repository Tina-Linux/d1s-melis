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
#include "parse_bmp.h"
#include "bmp.h"
#include <string.h>
#include <log.h>
#include <hal_mem.h>

#define spcl_size_align( x, y )         ( ( (x) + (y) - 1 ) & ~( (y) - 1 ) )
#define abs(x) (x) >= 0 ? (x):-(x)

/*
*******************************************************************************
*                     Parse_Pic_BMP_ByBuffer
*
* Description:
*    解析存放在内存中的图片
*
* Parameters:
*    Pic_Buffer     :  input.  存放图片的入口
*    Pic_BufferSize :  input.  缓冲区大小
*    PictureInfo    :  output. 图片解析后的信息
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
int32_t Parse_Pic_BMP_ByBuffer(void *Pic_Buffer, uint32_t Pic_BufferSize, Picture_t *PictureInfo)
{
    bmp_info_head_t *info_head_p = NULL;

    info_head_p = (bmp_info_head_t *)((unsigned long)Pic_Buffer + sizeof(bmp_file_head_t));

    PictureInfo->BitCount   = info_head_p->biBitCount;
    PictureInfo->Width      = info_head_p->biWidth;
    PictureInfo->Height     = abs(info_head_p->biHeight);
    PictureInfo->RowSize    = spcl_size_align(info_head_p->biWidth * (info_head_p->biBitCount >> 3), 4);

    PictureInfo->Buffer     = (void *)((unsigned long)Pic_Buffer + sizeof(bmp_info_head_t) + sizeof(bmp_file_head_t));
    PictureInfo->BufferSize = Pic_BufferSize - (sizeof(bmp_info_head_t) + sizeof(bmp_file_head_t));

    return 0;
}

/*
*******************************************************************************
*                     Parse_Pic_BMP
*
* Description:
*    以路径名来解析图片, 并且把解析处理的图片拷贝到指定的地址，
* 如果指定的地址为NULL, 则可以存放在任何地址。
*
* Parameters:
*    Path           :  input.  图片路径
*    PictureInfo    :  output. 图片解析后的信息
*    Addr           :  input.  存放解析后的图片,
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
int32_t Parse_Pic_BMP_ByPath(char *Path, Picture_t *PictureInfo, void *Addr)
{
    HBMP_i_t hbmp_buf;
    HBMP  hbmp = NULL;

    memset(&hbmp_buf, 0, sizeof(HBMP_i_t));
    hbmp = open_bmp(Path, &hbmp_buf);
    if (hbmp == NULL)
    {
        __err("ERR: open_bmp failed");
        return -1;
    }

    PictureInfo->BitCount   = get_bitcount(hbmp);
    PictureInfo->Width      = get_width(hbmp);
    PictureInfo->Height     = get_height(hbmp);
    PictureInfo->RowSize    = get_rowsize(hbmp);

    PictureInfo->BufferSize = PictureInfo->RowSize * PictureInfo->Height;
    if (Addr)
    {
        PictureInfo->Buffer = (void *)Addr;
    }
    else
    {
        PictureInfo->Buffer = (void *)hal_malloc(PictureInfo->BufferSize);
    }
    if (PictureInfo->Buffer == NULL)
    {
        __log("ERR: wboot_malloc failed");
        goto error;
    }

    memset(PictureInfo->Buffer, 0, PictureInfo->BufferSize);

    get_matrix(hbmp, PictureInfo->Buffer);

    close_bmp(hbmp);
    hbmp = NULL;

    return 0;

error:
    close_bmp(hbmp);

    return -1;
}

#if defined CONFIG_BOOT_LOGO_BMP
int32_t Parse_Pic_BMP_ByRam(uint32_t base, uint32_t size, Picture_t *PictureInfo, void *Addr)
{
    HBMP_i_t hbmp_buf;
    HBMP  hbmp = NULL;

    memset(&hbmp_buf, 0, sizeof(HBMP_i_t));
    hbmp = open_ram_bmp(base, size, &hbmp_buf);
    if (hbmp == NULL)
    {
        __err("ERR: open_bmp failed");
        return -1;
    }

    PictureInfo->BitCount   = get_bitcount(hbmp);
    PictureInfo->Width      = get_width(hbmp);
    PictureInfo->Height     = get_height(hbmp);
    PictureInfo->RowSize    = get_rowsize(hbmp);

    PictureInfo->BufferSize = PictureInfo->RowSize * PictureInfo->Height;
    if (Addr)
    {
        PictureInfo->Buffer = (void *)Addr;
    }
    else
    {
        PictureInfo->Buffer = (void *)hal_malloc(PictureInfo->BufferSize);
    }
    if (PictureInfo->Buffer == NULL)
    {
        __log("ERR: wboot_malloc failed");
        goto error;
    }

    memset(PictureInfo->Buffer, 0, PictureInfo->BufferSize);

    get_matrix(hbmp, PictureInfo->Buffer);

    close_bmp(hbmp);
    hbmp = NULL;

    return 0;

error:
    close_bmp(hbmp);

    return -1;
}
#endif
